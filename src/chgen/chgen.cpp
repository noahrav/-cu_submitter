#include "chgen.h"

#include <utility>
#include "../utils/error.h"
#include "../utils/log.h"
#include "../utils/utils.h"

using Commands = lcf::rpg::EventCommand::Code;

namespace chgen {

    /**
     * @brief Lists the content of a directory
     * @param path the path of the directory
     * @return a vector containing the names of the files in the directory
     */
    std::vector<std::string> list_directory_content(std::string path) {
        std::vector<std::string> content;

        try {
            if (!fs::exists(path)) {
                error("The specified path does not exist.");
                return content;
            }

            if (!fs::is_directory(path)) {
                error("The specified path is not a directory.");
                return content;
            }

            for (const auto &file: fs::directory_iterator(path)) {
                content.push_back(file.path().filename().string());
            }

        } catch (const std::exception &e) {
            error(std::string(e.what()));
        }

        return content;
    }

    /**
     * @brief Lists all events containing a play music event in a map
     * @param map the map we want to analyze
     * @param base_track the name of the main music of the map; we don't want to scan bgm events that return to this track
     * @return a list of the BGMEvent objects
     */
    std::vector<data::BGMEvent> list_bgm_events(std::unique_ptr<lcf::rpg::Map> map, std::string base_track) {
        std::vector<data::BGMEvent> bgm_events;

        for (const auto &event: map->events) {
            for (const auto &page: event.pages) {
                for (const auto &command: page.event_commands) {
                    // We don't list the BGM event if it returns to the main music of the map
                    if (command.code == static_cast<int>(Commands::PlayBGM) && command.string.data() != base_track) {
                        data::BGMEvent bgm_event;

                        bgm_event.coordinates_.x = event.x;
                        bgm_event.coordinates_.y = event.y;
                        bgm_event.track_name_ = command.string.data();
                        auto params = command.parameters.begin();
                        bgm_event.volume_ = *std::next(params, 1);
                        bgm_event.speed_ = *std::next(params, 2);

                        bgm_events.push_back(bgm_event);
                    }
                }
            }
        }

        return bgm_events;
    }

    std::vector<data::Asset>
    add_assets(std::string base_path, std::string modified_path, data::AssetCategory category) {
        std::vector<data::Asset> assets;

        std::string folder;
        switch (category) {
            case data::AssetCategory::MENU_THEME:
                folder = "System";
                break;
            case data::AssetCategory::CHARSET:
                folder = "CharSet";
                break;
            case data::AssetCategory::CHIPSET:
                folder = "ChipSet";
                break;
            case data::AssetCategory::MUSIC:
                folder = "Music";
                break;
            case data::AssetCategory::SOUND:
                folder = "Sound";
                break;
            case data::AssetCategory::PANORAMA:
                folder = "Panorama";
                break;
            case data::AssetCategory::PICTURE:
                folder = "Picture";
                break;
            case data::AssetCategory::BATTLE_ANIMATION:
                folder = "Battle";
                break;
        }

        auto base_asset_content = list_directory_content(std::string(base_path / fs::path(folder)));
        auto modified_asset_content = list_directory_content(std::string(modified_path / fs::path(folder)));

        for (const auto &asset: base_asset_content) {
            const auto asset_name = asset.substr(0, asset.find_first_of('.'));
            /*if (asset_name.rfind("record_player_", 0) == 0) {
                // ignore record player assets
                continue;
            }*/

            if (std::find(begin(modified_asset_content), end(modified_asset_content), asset) ==
                end(modified_asset_content)) {
                // asset removed
                data::Asset changelog_asset;
                changelog_asset.category_ = category;
                changelog_asset.status_ = data::Status::REMOVED;
                changelog_asset.name_ = asset_name;
                changelog_asset.filename_ = asset;
                assets.push_back(changelog_asset);
            }
        }

        for (const auto &asset: modified_asset_content) {
            const auto asset_name = asset.substr(0, asset.find_first_of('.'));
            /*if (asset_name.rfind("record_player_", 0) == 0) {
                // ignore record player assets
                continue;
            }*/

            if (std::find(begin(base_asset_content), end(base_asset_content), asset) ==
                end(base_asset_content)) {
                // asset added
                data::Asset changelog_asset;
                changelog_asset.category_ = category;
                changelog_asset.status_ = data::Status::ADDED;
                changelog_asset.name_ = asset_name;
                changelog_asset.filename_ = asset;
                assets.push_back(changelog_asset);
            } else {
                // asset modified
                try {
                    const auto base_asset_path = base_path / fs::path(folder) / fs::path(asset);
                    const auto modified_asset_path = modified_path / fs::path(folder) / fs::path(asset);

                    const auto base_lastwritetime = fs::last_write_time(base_asset_path);
                    const auto modified_lastwritetime = fs::last_write_time(modified_asset_path);


                    if (base_lastwritetime != modified_lastwritetime) {
                        if (utils::compareFiles(base_asset_path, modified_asset_path)) {
                            // asset unchanged
                            continue;
                        }

                        data::Asset changelog_asset;
                        changelog_asset.category_ = category;
                        changelog_asset.status_ = data::Status::MODIFIED;
                        changelog_asset.name_ = asset_name;
                        changelog_asset.filename_ = asset;
                        assets.push_back(changelog_asset);
                    }
                } catch (const std::exception &e) {
                    error(std::string(e.what()));
                }
            }
        }

        return assets;
    }

    std::vector<data::CommonEvent>
    add_ce(std::vector<lcf::rpg::CommonEvent> base_ce_list, std::vector<lcf::rpg::CommonEvent> modified_ce_list) {
        std::vector<data::CommonEvent> commonEvents;

        for (size_t i = 0; i < std::min(base_ce_list.size(), modified_ce_list.size()); i++) {
            const auto &base_ce = base_ce_list[i];
            const auto &modified_ce = modified_ce_list[i];

            if (base_ce == modified_ce) {
                // no modification
                continue;
            }

            const std::string &base_ce_name = base_ce.name.data();
            const std::string &modified_ce_name = modified_ce.name.data();

            data::CommonEvent changelog_ce;
            changelog_ce.id_ = modified_ce.ID;
            changelog_ce.name_ = modified_ce_name;


            if (base_ce_name.empty()) {
                // added
                changelog_ce.status_ = data::Status::ADDED;
            } else if (modified_ce_name.empty()) {
                // removed
                changelog_ce.status_ = data::Status::REMOVED;
                changelog_ce.name_ = base_ce_name;
            } else {
                // modified
                changelog_ce.status_ = data::Status::MODIFIED;
            }

            commonEvents.push_back(changelog_ce);
        }

        return commonEvents;
    }

    std::vector<data::TilesetInfo>
    add_tilesets(std::vector<lcf::rpg::Chipset> base_tileset_list,
                 std::vector<lcf::rpg::Chipset> modified_tileset_list) {
        std::vector<data::TilesetInfo> tilesets;

        for (size_t i = 0; i < std::min(base_tileset_list.size(), modified_tileset_list.size()); i++) {
            const auto &base_tileset = base_tileset_list[i];
            const auto &modified_tileset = modified_tileset_list[i];

            if (base_tileset == modified_tileset) {
                // no modification
                continue;
            }

            const std::string &base_tileset_name = base_tileset.name.data();
            const std::string &modified_tileset_name = modified_tileset.name.data();

            data::TilesetInfo changelog_tileset;
            changelog_tileset.id_ = modified_tileset.ID;
            changelog_tileset.name_ = modified_tileset_name;
            changelog_tileset.chipset_name_ = modified_tileset.chipset_name.data();


            if (base_tileset_name.empty()) {
                // added
                changelog_tileset.status_ = data::Status::ADDED;
            } else if (modified_tileset_name.empty()) {
                // removed
                changelog_tileset.status_ = data::Status::REMOVED;
                changelog_tileset.name_ = base_tileset_name;
            } else {
                // modified
                changelog_tileset.status_ = data::Status::MODIFIED;
            }

            tilesets.push_back(changelog_tileset);
        }

        return tilesets;
    }

    std::vector<data::Switch>
    add_switches(std::vector<lcf::rpg::Switch> base_switch_list, std::vector<lcf::rpg::Switch> modified_variable_list) {
        std::vector<data::Switch> switches;

        for (size_t i = 0; i < std::min(base_switch_list.size(), modified_variable_list.size()); i++) {
            const auto &base_switch = base_switch_list[i];
            const auto &modified_switch = modified_variable_list[i];

            if (base_switch == modified_switch) {
                // no modification
                continue;
            }

            const std::string &base_switch_name = base_switch.name.data();
            const std::string &modified_switch_name = modified_switch.name.data();

            data::Switch changelog_switch;
            changelog_switch.id_ = modified_switch.ID;
            changelog_switch.name_ = modified_switch_name;

            if (base_switch_name.empty()) {
                // added
                changelog_switch.status_ = data::Status::ADDED;
            } else if (modified_switch_name.empty()) {
                // removed
                changelog_switch.status_ = data::Status::REMOVED;
                changelog_switch.name_ = base_switch_name;
            } else {
                // modified
                changelog_switch.status_ = data::Status::MODIFIED;
            }

            switches.push_back(changelog_switch);
        }

        return switches;
    }

    std::vector<data::Variable>
    add_variables(std::vector<lcf::rpg::Variable> base_var_list, std::vector<lcf::rpg::Variable> modified_var_list) {
        std::vector<data::Variable> variables;

        for (size_t i = 0; i < std::min(base_var_list.size(), modified_var_list.size()); i++) {
            const auto &base_var = base_var_list[i];
            const auto &modified_var = modified_var_list[i];

            if (base_var == modified_var) {
                // no modification
                continue;
            }

            const std::string &base_var_name = base_var.name.data();
            const std::string &modified_var_name = modified_var.name.data();

            data::Variable changelog_var;
            changelog_var.id_ = modified_var.ID;
            changelog_var.name_ = modified_var_name;


            if (base_var_name.empty()) {
                // added
                changelog_var.status_ = data::Status::ADDED;
            } else if (modified_var_name.empty()) {
                // removed
                changelog_var.status_ = data::Status::REMOVED;
                changelog_var.name_ = base_var_name;
            } else {
                // modified
                changelog_var.status_ = data::Status::MODIFIED;
            }

            variables.push_back(changelog_var);
        }

        return variables;
    }

    std::vector<data::Animation>
    add_animations(std::vector<lcf::rpg::Animation> base_anim_list,
                   std::vector<lcf::rpg::Animation> modified_anim_list) {
        std::vector<data::Animation> animations;

        for (size_t i = 0; i < std::min(base_anim_list.size(), modified_anim_list.size()); i++) {
            const auto &base_anim = base_anim_list[i];
            const auto &modified_anim = modified_anim_list[i];

            if (base_anim == modified_anim) {
                // no modification
                continue;
            }

            const std::string &base_anim_name = base_anim.name.data();
            const std::string &modified_anim_name = modified_anim.name.data();

            data::Animation changelog_anim;
            changelog_anim.id_ = modified_anim.ID;
            changelog_anim.name_ = modified_anim_name;

            if (base_anim_name.empty()) {
                // added
                changelog_anim.status_ = data::Status::ADDED;
            } else if (modified_anim_name.empty()) {
                // removed
                changelog_anim.status_ = data::Status::REMOVED;
                changelog_anim.name_ = base_anim_name;
            } else {
                // modified
                changelog_anim.status_ = data::Status::MODIFIED;
            }

            animations.push_back(changelog_anim);
        }

        return animations;
    }

    /**
     * @brief Lists all events containing a transfer player event to another map  in a map
     * @param map The map we want to analyze
     * @param map_id The ID of the map in argument map; We don't want to list inter-map warps
     * @return A list of the Connection objects
     */
    std::vector<data::Connection> list_warp_events(std::unique_ptr<lcf::rpg::Map> map, int map_id) {
        std::vector<data::Connection> connections;
        data::Map map_data;
        map_data.id_ = map_id;

        for (const auto &event: map->events) {
            for (const auto &page: event.pages) {
                for (const auto &command: page.event_commands) {
                    if (command.code != static_cast<int>(Commands::Teleport)) {
                        continue;
                    }
                    data::Connection connection;
                    connection.type_ = data::ConnectionType::ONEWAY;
                    connection.status_ = data::Status::ADDED;
                    connection.from_map_ = map_data;
                    connection.from_coordinates_ = data::Coordinates{event.x, event.y};

                    auto params = command.parameters.begin();
                    connection.to_map_ = data::Map{};
                    connection.to_map_.id_ = *params;
                    connection.to_coordinates_ = data::Coordinates{*std::next(params, 1), *std::next(params, 2)};

                    if (connection.from_map_.id_ == connection.to_map_.id_) {
                        // same map
                        continue;
                    }
                    connections.push_back(connection);
                }
            }
        }

        return connections;
    }


    /**
     * @brief Scans two RPG Maker game files for changes that are relevant in Collective Unconscious.
     * @param base_path The base path, usually the newest devbuild
     * @param modified_path The path of the build we made changes on
     * @return A changelog object containing the changes between the two builds
     */
    std::shared_ptr<data::Changelog>
    ChangelogGenerator::scan(const std::string &base_path, const std::string &modified_path) {
        log("Scanning changes between " + base_path + " and " + modified_path + "...");

        auto base_content = list_directory_content(base_path);
        if (base_content.empty()) {
            return nullptr;
        }

        auto modified_content = list_directory_content(modified_path);
        if (modified_content.empty()) {
            return nullptr;
        }

        auto changelog = std::make_shared<data::Changelog>();

        // TODO: get developer name
        changelog->developer_ = "No_dev_name";

        auto now = time(nullptr);
        changelog->date_ = localtime(&now);

        // TODO: get summary
        changelog->summary_ = "";

        // TODO: get policies
        changelog->map_policy_ = "";
        changelog->asset_policy_ = "";

        // maps
        std::vector<std::string> base_maps;
        std::remove_copy_if(begin(base_content), end(base_content), std::back_inserter(base_maps),
                            [](const std::string &s) {
                                return s.substr(s.find_last_of('.') + 1) != "lmu";
                            });

        std::sort(begin(base_maps), end(base_maps), [](const std::string &a, const std::string &b) {
            return std::stoi(a.substr(3, a.find_first_of('.'))) < std::stoi(b.substr(3, b.find_first_of('.')));
        });

        std::vector<std::string> modified_maps;
        std::remove_copy_if(begin(modified_content), end(modified_content), std::back_inserter(modified_maps),
                            [](const std::string &s) {
                                return s.substr(s.find_last_of('.') + 1) != "lmu";
                            });

        std::sort(begin(modified_maps), end(modified_maps), [](const std::string &a, const std::string &b) {
            return std::stoi(a.substr(3, a.find_first_of('.'))) < std::stoi(b.substr(3, b.find_first_of('.')));
        });

        // map tree
        fs::path base_lmt_path = base_path / fs::path("RPG_RT.lmt");
        if (!fs::exists(base_lmt_path)) {
            error("Missing file: " + std::string(base_lmt_path));
            return nullptr;
        }

        auto base_map_tree = lcf::LMT_Reader::Load(std::string(base_lmt_path));

        fs::path modified_lmt_path = modified_path / fs::path("RPG_RT.lmt");
        if (!fs::exists(modified_lmt_path)) {
            error("Missing file: " + std::string(modified_lmt_path));
            return nullptr;
        }

        auto modified_map_tree = lcf::LMT_Reader::Load(std::string(modified_lmt_path));

        // maps scan
        for (const auto &map: modified_maps) {

            if (std::find(begin(base_maps), end(base_maps), map) == end(base_maps)) {
                // empty map file added
                continue;
            }

            const auto base_lastwritetime = fs::last_write_time(base_path / fs::path(map));
            const auto modified_lastwritetime = fs::last_write_time(modified_path / fs::path(map));

            if (base_lastwritetime == modified_lastwritetime) {
                // map file unchanged
                continue;
            }

            const int map_id = std::stoi(map.substr(3, map.find_first_of('.')));

            if (map_id == 7) {
                // ignore record player
                continue;
            }

            auto base_map = base_map_tree->maps[map_id];
            auto modified_map = modified_map_tree->maps[map_id];

            if (base_map == modified_map) {
                // map unchanged
                continue;
            }

            const std::string modified_map_name = modified_map.name.data();
            const std::string base_map_name = base_map.name.data();
            if (modified_map_name.length() < 5) {
                // empty map (just the id in the name)
                continue;
            }

            data::Map changelog_map;

            if (base_map_name != modified_map_name) {
                changelog_map.status_ = data::Status::ADDED;
            } else {
                changelog_map.status_ = data::Status::MODIFIED;
            }

            changelog_map.id_ = map_id;
            changelog_map.name_ = modified_map_name.substr(5);

            auto modified_lmu = lcf::LMU_Reader::Load(std::string(modified_path / fs::path(map)));
            auto modified_lmu_copy = std::make_unique<lcf::rpg::Map>(*modified_lmu);

            auto base_lmu = lcf::LMU_Reader::Load(std::string(base_path / fs::path(map)));

            if (map_id != 7) {
                // ignore bgm events for record player
                changelog_map.bgm_events_ = list_bgm_events(std::move(modified_lmu), modified_map.music.name);
            }

            changelog_map.main_music_ = modified_map.music;

            changelog->maps_.push_back(changelog_map);

            // connections
            // TODO: put a warning to tell the user that all connections to a different map ID will be noted
            auto base_warps = list_warp_events(std::move(base_lmu), map_id);
            auto modified_warps = list_warp_events(std::move(modified_lmu_copy), map_id);

            std::vector<data::Connection> warps;

            for (const auto &warp: modified_warps) {
                if (std::find(begin(base_warps), end(base_warps), warp) == end(base_warps)) {
                    // warp added
                    warps.push_back(warp);
                }
            }

            for (const auto &warp: base_warps) {
                if (std::find(begin(modified_warps), end(modified_warps), warp) == end(modified_warps)) {
                    // warp removed
                    auto removed_warp = warp;
                    removed_warp.status_ = data::Status::REMOVED;
                    warps.push_back(removed_warp);
                } else {
                    // warp modified
                    auto modified_warp = std::find(begin(modified_warps), end(modified_warps), warp);
                    if (warp != *modified_warp) {
                        auto modified_warp = warp;
                        modified_warp.status_ = data::Status::MODIFIED;
                        warps.push_back(modified_warp);
                    }
                }
            }

            for (const auto &warp: warps) {
                changelog->connections_.push_back(warp);
            }
        }


        // database stuff
        auto base_db = lcf::LDB_Reader::Load(std::string(base_path / fs::path("RPG_RT.ldb")));
        auto modified_db = lcf::LDB_Reader::Load(std::string(modified_path / fs::path("RPG_RT.ldb")));

        changelog->common_events_ = add_ce(base_db->commonevents, modified_db->commonevents);
        changelog->tilesets_ = add_tilesets(base_db->chipsets, modified_db->chipsets);
        changelog->switches_ = add_switches(base_db->switches, modified_db->switches);
        changelog->variables_ = add_variables(base_db->variables, modified_db->variables);
        changelog->animations_ = add_animations(base_db->animations, modified_db->animations);

        // assets
        changelog->menu_themes_ = add_assets(base_path, modified_path, data::AssetCategory::MENU_THEME);
        changelog->charsets_ = add_assets(base_path, modified_path, data::AssetCategory::CHARSET);
        changelog->chipsets_ = add_assets(base_path, modified_path, data::AssetCategory::CHIPSET);
        changelog->musics_ = add_assets(base_path, modified_path, data::AssetCategory::MUSIC);
        changelog->sounds_ = add_assets(base_path, modified_path, data::AssetCategory::SOUND);
        changelog->panoramas_ = add_assets(base_path, modified_path, data::AssetCategory::PANORAMA);
        changelog->pictures_ = add_assets(base_path, modified_path, data::AssetCategory::PICTURE);
        changelog->animation_files_ = add_assets(base_path, modified_path, data::AssetCategory::BATTLE_ANIMATION);


        return changelog;
    }

    /**
     * @brief Generates a plain text changelog file from a Changelog object. The name of the file will be <developer name>_<date>_changelog.txt
     * @param changelog The changelog we want to save in a file
     * @param at The path where we want to save the file. If empty, the file will be saved in the current directory.
     */
    void ChangelogGenerator::generate(const std::shared_ptr<data::Changelog> &changelog, const std::string& at) {
        std::string date = data::date_string(changelog->date_);
        std::string date_formatted = date.substr(0, 2) + date.substr(3, 3) + date.substr(7, date.length());

        std::string filename = at / fs::path(changelog->developer_ + "_" + date_formatted + "_changelog.txt");

        // this ensures that we don't overwrite any existing file
        for (int i = 2; fs::exists(filename); i++) {
            filename = at / fs::path(changelog->developer_ + "_" + date_formatted + "_changelog_" +
                       std::to_string(i) + ".txt");
        }

        std::ofstream file;
        file.open(filename);

        file << changelog->stringify() << '\n';

        if (!fs::exists(filename)) {
            error("Could not generate file " + filename);
        } else {
            log("File generated: " + filename);
        }

        file.close();
    }

} // chgen
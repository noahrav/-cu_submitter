#ifndef CU_SUBMITTER_CHANGELOG_H
#define CU_SUBMITTER_CHANGELOG_H

#include <string>
#include <vector>
#include <ctime>
#include <lcf/rpg/music.h>


namespace data {

    /**
     * @brief Data structure used to represent wether an entry was added, removed or modified.
     */
    enum Status {
        ADDED,
        REMOVED,
        MODIFIED
    };

    /**
     * @brief Wrapper for coordinate.
     */
    struct Coordinates {
        int x;
        int y;

        std::string stringify();
    };

    inline bool operator==(const Coordinates &lhs, const Coordinates &rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    inline bool operator!=(const Coordinates &lhs, const Coordinates &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent a BGM event.
     */
    struct BGMEvent {
        Coordinates coordinates_;

        std::string track_name_;
        int volume_;
        int speed_;

        std::string stringify();
    };

    inline bool operator==(const BGMEvent &lhs, const BGMEvent &rhs) {
        return lhs.coordinates_ == rhs.coordinates_ && lhs.track_name_ == rhs.track_name_ &&
               lhs.volume_ == rhs.volume_ && lhs.speed_ == rhs.speed_;
    }

    inline bool operator!=(const BGMEvent &lhs, const BGMEvent &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent an open connection.
     */
    struct OpenConnection {
        Status status_;
        Coordinates coordinates_;

        std::string stringify();
    };

    inline bool operator==(const OpenConnection &lhs, const OpenConnection &rhs) {
        return lhs.coordinates_ == rhs.coordinates_;
    }

    inline bool operator!=(const OpenConnection &lhs, const OpenConnection &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent a closed connection.
     */
    struct ClosedConnection {
        Status status_;
        Coordinates coordinates_;

        std::string stringify();
    };

    inline bool operator==(const ClosedConnection &lhs, const ClosedConnection &rhs) {
        return lhs.coordinates_ == rhs.coordinates_;
    }

    inline bool operator!=(const ClosedConnection &lhs, const ClosedConnection &rhs) {
        return !(lhs == rhs);
    }

    /**
     * Converts an int in a string of a 4 digit number
     * @param id The number to turn into a string
     * @returns The 4 digit stringized version of the number
     */
    std::string id_string(unsigned int id);

    /**
     * @brief Data structure used to represent a map.
     */
    struct Map {
        Status status_;
        unsigned int id_;
        std::string name_;

        std::vector<std::string> notes_;
        std::vector<BGMEvent> bgm_events_;
        std::vector<OpenConnection> open_connections_;
        std::vector<ClosedConnection> closed_connections_;

        lcf::rpg::Music main_music_;

        std::string stringify();
    };

    inline bool operator==(const Map &lhs, const Map &rhs) {
        return lhs.id_ == rhs.id_ && lhs.name_ == rhs.name_ && lhs.bgm_events_ == rhs.bgm_events_ &&
               lhs.open_connections_ == rhs.open_connections_ && lhs.closed_connections_ == rhs.closed_connections_ &&
               lhs.main_music_ == rhs.main_music_;
    }

    inline bool operator!=(const Map &lhs, const Map &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent a connection type.
     * @details ONEWAY: The connection is one-way. BOTHWAY: The connection is two-way. UNLOCKED: The player must unlock the connection from one side.
     */
    enum ConnectionType {
        ONEWAY,
        BOTHWAY,
        UNLOCKED
    };

    /**
     * @brief Data structure used to represent a connection between two maps.
     */
    struct Connection {
        Status status_;

        Map from_map_;
        Coordinates from_coordinates_;

        Map to_map_;
        Coordinates to_coordinates_;

        ConnectionType type_;

        std::vector<std::string> notes_;

        std::string stringify();
    };

    inline bool operator==(const Connection &lhs, const Connection &rhs) {
        return lhs.from_map_ == rhs.from_map_ && lhs.from_coordinates_ == rhs.from_coordinates_ &&
               lhs.to_map_ == rhs.to_map_ && lhs.to_coordinates_ == rhs.to_coordinates_ && lhs.type_ == rhs.type_;
    }

    inline bool operator!=(const Connection &lhs, const Connection &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent a common event.
     */
    struct CommonEvent {
        Status status_;
        unsigned int id_;
        std::string name_;
        std::vector<std::string> notes_;

        std::string stringify();
    };

    inline bool operator==(const CommonEvent &lhs, const CommonEvent &rhs) {
        return lhs.id_ == rhs.id_ && lhs.name_ == rhs.name_;
    }

    inline bool operator!=(const CommonEvent &lhs, const CommonEvent &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent a tileset entry in the database.
     */
    struct TilesetInfo {
        Status status_;
        unsigned int id_;
        std::string name_;
        std::string chipset_name_;
        std::vector<std::string> notes_;

        std::string stringify();
    };

    inline bool operator==(const TilesetInfo &lhs, const TilesetInfo &rhs) {
        return lhs.id_ == rhs.id_ && lhs.name_ == rhs.name_ && lhs.chipset_name_ == rhs.chipset_name_;
    }

    inline bool operator!=(const TilesetInfo &lhs, const TilesetInfo &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent a switch.
     */
    struct Switch {
        Status status_;
        unsigned int id_;
        std::string name_;
        std::vector<std::string> notes_;

        std::string stringify();
    };

    inline bool operator==(const Switch &lhs, const Switch &rhs) {
        return lhs.id_ == rhs.id_ && lhs.name_ == rhs.name_;
    }

    inline bool operator!=(const Switch &lhs, const Switch &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent a variable.
     */
    struct Variable {
        Status status_;
        unsigned int id_;
        std::string name_;
        std::vector<std::string> notes_;

        std::string stringify();
    };

    inline bool operator==(const Variable &lhs, const Variable &rhs) {
        return lhs.id_ == rhs.id_ && lhs.name_ == rhs.name_;
    }

    inline bool operator!=(const Variable &lhs, const Variable &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent an animation entry in the database.
     */
    struct Animation {
        Status status_;
        unsigned int id_;
        std::string name_;
        std::string animation_name_;
        std::vector<std::string> notes_;

        std::string stringify();
    };

    inline bool operator==(const Animation &lhs, const Animation &rhs) {
        return lhs.id_ == rhs.id_ && lhs.name_ == rhs.name_ && lhs.animation_name_ == rhs.animation_name_;
    }

    inline bool operator!=(const Animation &lhs, const Animation &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Data structure used to represent an asset category.
     */
    enum AssetCategory {
        MENU_THEME,
        CHARSET,
        CHIPSET,
        MUSIC,
        SOUND,
        PANORAMA,
        PICTURE,
        BATTLE_ANIMATION
    };

    /**
     * @brief Data structure used to represent an asset.
     */
    struct Asset {
        Status status_;
        AssetCategory category_;
        std::string name_;
        std::string filename_;
        std::vector<std::string> notes_;

        // outside contributors only
        std::string contributors_;

        std::string stringify();
    };

    inline bool operator==(const Asset &lhs, const Asset &rhs) {
        return lhs.category_ == rhs.category_ && lhs.name_ == rhs.name_;
    }

    inline bool operator!=(const Asset &lhs, const Asset &rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Converts a date to a string according to the format specified in the changelog template.
     * @param date
     * @return A string representation of the date.
     */
    std::string date_string(tm *date);

    /**
     * @brief Data structure used to generate the changelog.
     */
    struct Changelog {
        /**
         * @brief Name of the developer
         */
        std::string developer_;
        /**
         * @brief Date of submit
         */
        tm *date_{};

        /**
         * @brief Summary of the submit
         */
        std::string summary_;

        /**
         * @brief Map policy specifications (who is allowed to edit and/or add content to your maps, and in what ways: open connections only, can create a warp but it has to be this specific shape/color, etc.)
         */
        std::string map_policy_;
        /**
         * @brief Asset policy specifications (who is allowed to use and/or edit the assets you made)
         */
        std::string asset_policy_;

        /**
         * @brief List of maps
         */
        std::vector<Map> maps_;
        /**
         * @brief List of connections between worlds (a world can consist of multiple maps. You don't have to write down warps between a map and its subareas)
         */
        std::vector<Connection> connections_;

        /**
         * @brief List of common events
         */
        std::vector<CommonEvent> common_events_;

        /**
         * @brief List of tileset entries in the database
         */
        std::vector<TilesetInfo> tilesets_;

        /**
         * @brief List of switches
         */
        std::vector<Switch> switches_;

        /**
         * @brief List of variables
         */
        std::vector<Variable> variables_;

        /**
         * @brief List of animation entries in the database
         */
        std::vector<Animation> animations_;

        /**
         * @brief List of menu theme files
         */
        std::vector<Asset> menu_themes_;

        /**
         * @brief List of charsets
         */
        std::vector<Asset> charsets_;

        /**
         * @brief List of chipsets
         */
        std::vector<Asset> chipsets_;

        /**
         * @brief List of music files
         */
        std::vector<Asset> musics_;

        /**
         * @brief List of sound effects
         */
        std::vector<Asset> sounds_;

        /**
         * @brief List of panoramas
         */
        std::vector<Asset> panoramas_;

        /**
         * @brief List of pictures
         */
        std::vector<Asset> pictures_;

        /**
         * @brief List of animation files
         */
        std::vector<Asset> animation_files_;

        std::string stringify();

        Changelog() = default;
    };

    inline bool operator==(const Changelog &lhs, const Changelog &rhs) {
        return lhs.developer_ == rhs.developer_ && lhs.summary_ == rhs.summary_ && lhs.map_policy_ == rhs.map_policy_ &&
               lhs.asset_policy_ == rhs.asset_policy_ && lhs.maps_ == rhs.maps_ &&
               lhs.connections_ == rhs.connections_ && lhs.common_events_ == rhs.common_events_ &&
               lhs.tilesets_ == rhs.tilesets_ && lhs.switches_ == rhs.switches_ && lhs.variables_ == rhs.variables_ &&
               lhs.animations_ == rhs.animations_ && lhs.menu_themes_ == rhs.menu_themes_ &&
               lhs.charsets_ == rhs.charsets_ && lhs.chipsets_ == rhs.chipsets_ && lhs.musics_ == rhs.musics_ &&
               lhs.sounds_ == rhs.sounds_ && lhs.panoramas_ == rhs.panoramas_ && lhs.pictures_ == rhs.pictures_ &&
               lhs.animation_files_ == rhs.animation_files_;
    }

    inline bool operator!=(const Changelog &lhs, const Changelog &rhs) {
        return !(lhs == rhs);
    }

} // data

#endif //CU_SUBMITTER_CHANGELOG_H

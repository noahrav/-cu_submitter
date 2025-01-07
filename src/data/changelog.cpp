#include "changelog.h"

namespace data {

    std::string Coordinates::stringify() {
        return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }

    std::string status_string(Status status) {
        switch (status) {
            case ADDED:
                return "+";
            case REMOVED:
                return "-";
            case MODIFIED:
                return "*";
        }

        return "";
    }

    std::string BGMEvent::stringify() {

        std::string s = "| BGM in event at " + coordinates_.stringify();

        if (!track_name_.empty() && volume_ != 0 && speed_ != 0) {
            s += " (track: " + track_name_ + ", volume: " + std::to_string(volume_) + "%, speed: " +
                 std::to_string(speed_) +
                 "%)";
        }

        return s;
    }

    std::string OpenConnection::stringify() {
        return status_string(status_) + " Open connection at " + coordinates_.stringify();
    }

    std::string ClosedConnection::stringify() {
        return status_string(status_) + " Closed connection at " + coordinates_.stringify();
    }

    std::string id_string(unsigned int id) {
        std::string s;

        if (id < 1000) {
            s += "0";
        }

        if (id < 100) {
            s += "0";
        }

        if (id < 10) {
            s += "0";
        }

        s += std::to_string(id);
        return s;
    }

    std::string Map::stringify() {
        std::string s = status_string(status_) + " MAP[" + id_string(id_) + "] - " + name_;

        if (!notes_.empty()) {
            for (auto &note: notes_) {
                s += "\n\t| " + note;
            }
        }

        if (!bgm_events_.empty()) {
            for (auto &bgm_event: bgm_events_) {
                s += "\n\t" + bgm_event.stringify();
            }
        }

        if (!open_connections_.empty()) {
            for (auto &open_connection: open_connections_) {
                s += "\n\t" + open_connection.stringify();
            }
        }

        if (!closed_connections_.empty()) {
            for (auto &closed_connection: closed_connections_) {
                s += "\n\t" + closed_connection.stringify();
            }
        }

        return s;
    }

    std::string connection_type_string(ConnectionType connection_type) {
        switch (connection_type) {
            case ONEWAY:
                return "one-way";
            case BOTHWAY:
                return "both-way";
            case UNLOCKED:
                return "unlocked from the other side";
        }

        return "";
    }

    std::string Connection::stringify() {
        std::string s =
                status_string(status_) + " Connection from MAP[" + id_string(from_map_.id_) + "]." +
                from_coordinates_.stringify() + " to MAP[" + id_string(to_map_.id_) + "]." +
                to_coordinates_.stringify() +
                "(" + connection_type_string(type_) + ")";

        if (!notes_.empty()) {
            for (auto &note: notes_) {
                s += "\n\t| " + note;
            }
        }

        return s;
    }

    std::string CommonEvent::stringify() {
        std::string s = status_string(status_) + " CE[" + id_string(id_) + "] - " + name_;

        if (!notes_.empty()) {
            for (auto &note: notes_) {
                s += "\n\t| " + note;
            }
        }

        return s;
    }

    std::string TilesetInfo::stringify() {
        std::string s = status_string(status_) + " Tileset[" + id_string(id_) + "] - " + name_;

        if (!chipset_name_.empty() && status_ == ADDED) {
            s += " (" + chipset_name_ + ")";
        }

        if (!notes_.empty()) {
            for (auto &note: notes_) {
                s += "\n\t| " + note;
            }
        }

        return s;
    }

    std::string Switch::stringify() {
        std::string s = status_string(status_) + " Switch[" + id_string(id_) + "] - " + name_;

        if (!notes_.empty()) {
            for (auto &note: notes_) {
                s += "\n\t| " + note;
            }
        }

        return s;
    }

    std::string Variable::stringify() {
        std::string s = status_string(status_) + " Variable[" + id_string(id_) + "] - " + name_;

        if (!notes_.empty()) {
            for (auto &note: notes_) {
                s += "\n\t| " + note;
            }
        }

        return s;
    }

    std::string Animation::stringify() {
        std::string s = status_string(status_) + " Animation[" + id_string(id_) + "] - " + name_;

        if (!animation_name_.empty() && status_ == ADDED) {
            s += " (file: " + animation_name_ + ")";
        }

        if (!notes_.empty()) {
            for (auto &note: notes_) {
                s += "\n\t| " + note;
            }
        }

        return s;
    }

    std::string Asset::stringify() {
        std::string s = status_string(status_);

        switch (category_) {
            case MENU_THEME:
                s += " System ";
                break;
            case CHARSET:
                s += " CharSet ";
                break;
            case CHIPSET:
                s += " ChipSet ";
                break;
            case MUSIC:
                s += " Music ";
                break;
            case SOUND:
                s += " Sound ";
                break;
            case PANORAMA:
                s += " Panorama ";
                break;
            case PICTURE:
                s += " Picture ";
                break;
            case BATTLE_ANIMATION:
                s += " Battle ";
                break;
        }

        s += name_;

        if (!contributors_.empty()) {
            s += " (by " + contributors_ + ")";
        }

        if (!notes_.empty()) {
            for (auto &note: notes_) {
                s += "\n\t| " + note;
            }
        }

        return s;
    }

    std::string date_string(tm* date) {
        std::string s;

        int day = date->tm_mday;
        if (day < 10) {
            s += '0';
        }
        s += std::to_string(day) + "/";

        switch (date->tm_mon) {
            case 0:
                s += "Jan";
                break;
            case 1:
                s += "Feb";
                break;
            case 2:
                s += "Mar";
                break;
            case 3:
                s += "Apr";
                break;
            case 4:
                s += "May";
                break;
            case 5:
                s += "Jun";
                break;
            case 6:
                s += "Jul";
                break;
            case 7:
                s += "Aug";
                break;
            case 8:
                s += "Sep";
                break;
            case 9:
                s += "Oct";
                break;
            case 10:
                s += "Nov";
                break;
            case 11:
                s += "Dec";
                break;
        }

        s += "/" + std::to_string(date->tm_year + 1900);

        return s;
    }

    std::string Changelog::stringify() {
        const std::string separator = "---------------------------------------------------\n";

        std::string s = "Developer: " + developer_ + "\nDate: " + date_string(date_) + "\n\n";

        if (!summary_.empty()) {
            s += "// " + summary_ + "\n";
        }

        if (!map_policy_.empty() || !asset_policy_.empty()) {
            s += separator;
        }

        if (!map_policy_.empty()) {
            s += "Map policy... " + map_policy_ + "\n";
        }

        if (!asset_policy_.empty()) {
            s += "Asset policy... " + asset_policy_ + "\n";
        }

        // Maps
        if (!maps_.empty()) {
            s += separator;
        }

        for (auto &map: maps_) {
            s += map.stringify() + "\n";
        }

        // Connections
        if (!connections_.empty()) {
            s += "\n";
        }

        for (auto &connection: connections_) {
            s += connection.stringify() + "\n";
        }

        // Common events
        if (!common_events_.empty()) {
            s += separator;
        }

        for (auto &common_event: common_events_) {
            s += common_event.stringify() + "\n";
        }

        // Tilesets
        if (!tilesets_.empty()) {
            s += separator;
        }

        for (auto &tileset: tilesets_) {
            s += tileset.stringify() + "\n";
        }

        // Switches
        if (!switches_.empty()) {
            s += separator;
        }

        for (auto &sw: switches_) {
            s += sw.stringify() + "\n";
        }

        // Variables
        if (!variables_.empty()) {
            s += separator;
        }

        for (auto &var: variables_) {
            s += var.stringify() + "\n";
        }

        // Animations
        if (!animations_.empty()) {
            s += separator;
        }

        for (auto &animation: animations_) {
            s += animation.stringify() + "\n";
        }

        // Menu themes
        if (!menu_themes_.empty()) {
            s += separator;
        }

        for (auto &menu_theme: menu_themes_) {
            s += menu_theme.stringify() + "\n";
        }

        // Charsets
        if (!charsets_.empty()) {
            s += separator;
        }

        for (auto &charset: charsets_) {
            s += charset.stringify() + "\n";
        }

        // Chipsets
        if (!chipsets_.empty()) {
            s += separator;
        }

        for (auto &chipset: chipsets_) {
            s += chipset.stringify() + "\n";
        }

        // Musics
        if (!musics_.empty()) {
            s += separator;
        }

        for (auto &music: musics_) {
            s += music.stringify() + "\n";
        }

        // Sound effects
        if (!sounds_.empty()) {
            s += separator;
        }

        for (auto &sound: sounds_) {
            s += sound.stringify() + "\n";
        }

        // Panoramas
        if (!panoramas_.empty()) {
            s += separator;
        }

        for (auto &panorama: panoramas_) {
            s += panorama.stringify() + "\n";
        }

        // Pictures
        if (!pictures_.empty()) {
            s += separator;
        }

        for (auto &picture: pictures_) {
            s += picture.stringify() + "\n";
        }

        // Animation files
        if (!animation_files_.empty()) {
            s += separator;
        }

        for (auto &animation: animation_files_) {
            s += animation.stringify() + "\n";
        }

        return s;
    }

} // data
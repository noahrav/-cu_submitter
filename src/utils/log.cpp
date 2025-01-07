#include "log.h"

inline std::string digit_to_string(int digit) {
    std::string str;
    if (digit < 10) {
        str += '0';
    }

    return str + std::to_string(digit);
}

void log(const std::string& message) {
    auto now = time(nullptr);
    const auto date = localtime(&now);

    std::string date_string = "[" + digit_to_string(date->tm_mday) + "/" + digit_to_string(date->tm_mon + 1) + "/" + std::to_string(date->tm_year + 1900);
    date_string += " - " + digit_to_string(date->tm_hour) + ":" + digit_to_string(date->tm_min) + ":" + digit_to_string(date->tm_sec) + "] ";

    const std::string message_view = date_string + message;
    std::cout <<  message_view << '\n';
}

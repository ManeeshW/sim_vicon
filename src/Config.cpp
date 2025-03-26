#include "Config.h"
#include <fstream>
#include <sstream>
#include <iostream>

Config readConfig(const std::string& filename) {
    Config config;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << ". Using default values.\n";
        return config;
    }

    std::string line, section;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line[0] == '[') {
            section = line.substr(1, line.find(']')-1);
            continue;
        }

        std::istringstream iss(line);
        std::string key, value;
        std::getline(iss, key, '='); 
        std::getline(iss, value);
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));

        try {
            if (section == "vicon") {
                if (key == "object_name") config.object_name = value;
                else if (key == "server_ip") config.server_ip = value;
                else if (key == "frequency") config.frequency = std::stoi(value);
            }
            else if (section == "position") {
                if (key == "x1") config.pos[0] = std::stod(value);
                else if (key == "x2") config.pos[1] = std::stod(value);
                else if (key == "x3") config.pos[2] = std::stod(value);
            }
            else if (section == "rotation") {
                if (key == "yaw") config.euler[0] = std::stod(value);
                else if (key == "pitch") config.euler[1] = std::stod(value);
                else if (key == "roll") config.euler[2] = std::stod(value);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing " << key << "=" << value << ": " << e.what() << "\n";
        }
    }
    return config;
}
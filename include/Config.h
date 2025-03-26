#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    std::string object_name = "Origins";
    std::string server_ip = "10.0.0.200";
    int frequency = 200;
    double pos[3] = {1.0, -2.0, 1.5};
    double euler[3] = {0.0, 0.0, 0.0};
};

Config readConfig(const std::string& filename);

#endif
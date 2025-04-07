// Config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    std::string object_name = "Origins";
    std::string server_ip = "10.0.0.200";
    int frequency = 200;
    double pos[3] = {1.0, -2.0, 1.5};
    double euler[3] = {0.0, 0.0, 0.0};
    double trajectory_reset_time = 120.0;
    double lissajous_A = 1.0;
    double lissajous_B = 1.0;
    double lissajous_a = 1.0;
    double lissajous_b = 1.0;
    double lissajous_delta = 0.0;
    double circular_radius = 1.0;
    double circular_speed = 1.0;
};

// Function declaration only
Config readConfig(const std::string& filename);

#endif
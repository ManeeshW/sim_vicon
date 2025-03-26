#include "SimViconTracker.h"
#include "RotationUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <iomanip>

struct Config {
    std::string object_name = "Origins";
    std::string server_ip = "10.0.0.200";
    int frequency = 200;
    double pos[3] = {1.0, -2.0, 1.5};
    double euler[3] = {0.0, 0.0, 0.0};
};

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
    
    std::cout << "Config loaded:\n"
              << "  object_name = " << config.object_name << "\n"
              << "  server_ip = " << config.server_ip << "\n"
              << "  frequency = " << config.frequency << " Hz\n"
              << "  position = [" << config.pos[0] << ", " << config.pos[1] << ", " << config.pos[2] << "]\n"
              << "  euler = [" << config.euler[0] << ", " << config.euler[1] << ", " << config.euler[2] << "]\n";
    return config;
}

void printPose(const double pos[3], const std::vector<std::vector<double>>& R, 
               const std::vector<double>& quat_vrpn) {
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Position (x, y, z): ["
              << pos[0] << ", " << pos[1] << ", " << pos[2] << "]\n";
    std::cout << "Quaternion VRPN (x, y, z, w): ["
              << quat_vrpn[0] << ", " << quat_vrpn[1] << ", " << quat_vrpn[2] << ", " << quat_vrpn[3] << "]\n";
    std::cout << "Rotation Matrix (local):\n";
    for (int i = 0; i < 3; ++i) {
        std::cout << "  [";
        for (int j = 0; j < 3; ++j) {
            std::cout << std::setw(8) << R[i][j];
            if (j < 2) std::cout << ", ";
        }
        std::cout << "]\n";
    }
}

int main() {
    Config config = readConfig("sim_vicon.ini");
    
    if (config.server_ip.empty()) {
        std::cerr << "Error: Server IP is empty. Using default '10.0.0.200'.\n";
        config.server_ip = "10.0.0.200";
    }

    if (config.frequency <= 0) {
        std::cerr << "Error: Invalid frequency " << config.frequency << ". Using default 200 Hz.\n";
        config.frequency = 200;
    }

    std::string connection_str = config.server_ip + ":3883";
    std::cout << "Attempting to connect to: " << connection_str << "\n";

    vrpn_Connection* connection = vrpn_create_server_connection(connection_str.c_str());
    if (!connection) {
        std::cerr << "Failed to create VRPN connection on " << connection_str << "\n";
        return -1;
    }

    // Convert Euler to Quaternion (qw, qx, qy, qz order)
    auto quat = RotationUtils::eulerToQuaternion(config.euler[0], 
                                               config.euler[1], 
                                               config.euler[2]);
    
    // Convert to VRPN order (qx, qy, qz, qw) and apply conjugate if needed
    // For identity, we want VRPN to see [0, 0, 0, 1]
    std::vector<double> quat_vrpn = {quat[1], quat[2], quat[3], quat[0]}; // qx, qy, qz, qw

    // Compute local rotation matrix for display (using original quat)
    auto R = RotationUtils::quaternionToRotationMatrix(
        quat[0], quat[1], quat[2], quat[3]);

    // Print initial pose
    std::cout << "Initial pose:\n";
    printPose(config.pos, R, quat_vrpn);

    // Create tracker with VRPN-ordered quaternion
    SimViconTracker tracker(config.object_name, connection, 
                           config.pos[0], config.pos[1], config.pos[2],
                           quat_vrpn[0], quat_vrpn[1], quat_vrpn[2], quat_vrpn[3]);

    std::cout << "Sim Vicon tracker running at " << config.object_name 
              << "@" << connection_str << "...\n";

    auto sleep_ms = std::chrono::milliseconds(1000 / config.frequency);
    int count = 0;

    while (true) {
        tracker.mainloop();
        connection->mainloop();
        
        if (++count % 50 == 0) {
            std::cout << "\nCurrent pose:\n";
            printPose(config.pos, R, quat_vrpn);
            count = 0;
        }
        
        std::this_thread::sleep_for(sleep_ms);
    }

    delete connection;
    return 0;
}
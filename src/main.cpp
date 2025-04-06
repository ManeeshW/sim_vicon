#include "SimViconTracker.h"
#include "ViconGUI.h"
#include "Config.h"
#include <QApplication>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

std::atomic<bool> running(true); // Flag to stop the thread

void trackerThread(SimViconTracker* tracker, vrpn_Connection* connection, int frequency) {
    auto sleep_ms = std::chrono::milliseconds(1000 / frequency);
    while (running) {
        tracker->mainloop();
        connection->mainloop();
        std::this_thread::sleep_for(sleep_ms);
    }
}

int main(int argc, char* argv[]) {
    // Read configuration
    Config config = readConfig("../sim_vicon.ini");
    
    if (config.server_ip.empty()) {
        std::cerr << "Error: Server IP is empty. Using default '10.0.0.200'.\n";
        config.server_ip = "10.0.0.200";
    }
    if (config.frequency <= 0) {
        std::cerr << "Error: Invalid frequency " << config.frequency << ". Using default 200 Hz.\n";
        config.frequency = 200;
    }

    // Create VRPN connection
    std::string connection_str = config.server_ip + ":3883";
    vrpn_Connection* connection = vrpn_create_server_connection(connection_str.c_str());
    if (!connection) {
        std::cerr << "Failed to create VRPN connection on " << connection_str << "\n";
        return -1;
    }
    std::cout << "VRPN connection established at " << connection_str << "\n";

    // Initial quaternion
    auto quat = RotationUtils::eulerToQuaternion(config.euler[0], config.euler[1], config.euler[2]);
    std::vector<double> quat_vrpn = {quat[1], quat[2], quat[3], quat[0]};

    // Initialize tracker
    SimViconTracker tracker(config.object_name, connection, 
                           config.pos[0], config.pos[1], config.pos[2],
                           quat_vrpn[0], quat_vrpn[1], quat_vrpn[2], quat_vrpn[3]);

    // Start tracker thread
    std::thread tracker_thread(trackerThread, &tracker, connection, config.frequency);

    // Start Qt GUI in main thread
    QApplication app(argc, argv);
    ViconGUI gui(&tracker, config);
    gui.show();

    // Run Qt event loop
    int result = app.exec();

    // Stop tracker thread
    running = false;
    tracker_thread.join();

    // Cleanup
    delete connection;
    return result;
}
#include <vrpn_Tracker.h>
#include <vrpn_Connection.h>
#include <iostream>
#include <chrono>
#include <thread>

class SimViconTracker : public vrpn_Tracker {
public:
    SimViconTracker(const char *name, vrpn_Connection *c)
        : vrpn_Tracker(name, c) {}

    void send_sim_pose() {
        struct timeval timestamp;
        vrpn_gettimeofday(&timestamp, NULL);

        // Sim position (change these values for testing)
        pos[0] = 1.0; // X position
        pos[1] = -2.0; // Y position
        pos[2] = 1.5; // Z position
        
        // Sim orientation (Quaternion: qw, qx, qy, qz)
        d_quat[0] = 0.0; // qw
        d_quat[1] = 0.0;   // qx
        d_quat[2] = 0.0; // qy
        d_quat[3] = 0.0;   // qz

        char msgbuf[1000];
        int len = vrpn_Tracker::encode_to(msgbuf);
        if (len > 0) {
            d_connection->pack_message(len, timestamp, position_m_id, d_sender_id, msgbuf, vrpn_CONNECTION_RELIABLE);
        }
    }

    // Implement the pure virtual function
    void mainloop() override {
        send_sim_pose();
        server_mainloop();
    }
};

int main() {
    // Bind VRPN server to a specific IP and port
    vrpn_Connection *connection = vrpn_create_server_connection("10.0.0.200:3883");

    if (!connection) {
        std::cerr << "Failed to create VRPN connection on 10.0.0.200:3883\n";
        return -1;
    }

    SimViconTracker tracker("Origins", connection);

    std::cout << "Sim Vicon tracker running at Origins@10.0.0.200:3883...\n";

    while (true) {
        tracker.mainloop();   // Ensure VRPN updates
        connection->mainloop();
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); // 50 Hz update rate
    }

    return 0;
}

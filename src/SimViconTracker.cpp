#include "SimViconTracker.h"
#include <iostream>
#include <iomanip>

SimViconTracker::SimViconTracker(const std::string& name, vrpn_Connection* c,
                                double x, double y, double z,
                                double qx, double qy, double qz, double qw)
    : vrpn_Tracker(name.c_str(), c) {
    current_pos[0] = x;
    current_pos[1] = y;
    current_pos[2] = z;
    current_quat[0] = qx; // VRPN order: qx, qy, qz, qw
    current_quat[1] = qy;
    current_quat[2] = qz;
    current_quat[3] = qw;
}

void SimViconTracker::send_sim_pose() {
    struct timeval timestamp;
    vrpn_gettimeofday(&timestamp, NULL);

    pos[0] = current_pos[0];
    pos[1] = current_pos[1];
    pos[2] = current_pos[2];
    
    d_quat[0] = current_quat[0];
    d_quat[1] = current_quat[1];
    d_quat[2] = current_quat[2];
    d_quat[3] = current_quat[3];

    // Print pose before publishing
    // std::cout << std::fixed << std::setprecision(3);
    // std::cout << "Vicon Pose (pre-publish) - Position: ["
    //           << pos[0] << ", " << pos[1] << ", " << pos[2] << "] "
    //           << "Quaternion (x, y, z, w): ["
    //           << d_quat[0] << ", " << d_quat[1] << ", "
    //           << d_quat[2] << ", " << d_quat[3] << "]\n";

    char msgbuf[1000];
    int len = vrpn_Tracker::encode_to(msgbuf);
    if (len > 0) {
        int result = d_connection->pack_message(len, timestamp, position_m_id, 
                                               d_sender_id, msgbuf, vrpn_CONNECTION_RELIABLE);
        if (result < 0) {
            std::cerr << "Error: Failed to pack message for publishing\n";
        }
    } else {
        std::cerr << "Error: Failed to encode tracker message\n";
    }
}

void SimViconTracker::mainloop() {
    send_sim_pose();
    server_mainloop();
}

void SimViconTracker::updatePosition(double x, double y, double z) {
    current_pos[0] = x;
    current_pos[1] = y;
    current_pos[2] = z;
}

void SimViconTracker::updateQuaternion(double qx, double qy, double qz, double qw) {
    current_quat[0] = qx;
    current_quat[1] = qy;
    current_quat[2] = qz;
    current_quat[3] = qw;
}
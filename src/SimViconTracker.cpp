#include "SimViconTracker.h"
#include <iostream>

SimViconTracker::SimViconTracker(const std::string& name, vrpn_Connection* c,
                                double x, double y, double z,
                                double qw, double qx, double qy, double qz)
    : vrpn_Tracker(name.c_str(), c) {
    init_pos[0] = x;
    init_pos[1] = y;
    init_pos[2] = z;
    init_quat[0] = qw;
    init_quat[1] = qx;
    init_quat[2] = qy;
    init_quat[3] = qz;
}

void SimViconTracker::send_sim_pose() {
    struct timeval timestamp;
    vrpn_gettimeofday(&timestamp, NULL);

    pos[0] = init_pos[0];
    pos[1] = init_pos[1];
    pos[2] = init_pos[2];
    
    d_quat[0] = init_quat[0];
    d_quat[1] = init_quat[1];
    d_quat[2] = init_quat[2];
    d_quat[3] = init_quat[3];

    char msgbuf[1000];
    int len = vrpn_Tracker::encode_to(msgbuf);
    if (len > 0) {
        d_connection->pack_message(len, timestamp, position_m_id, 
                                 d_sender_id, msgbuf, vrpn_CONNECTION_RELIABLE);
    }
}

void SimViconTracker::mainloop() {
    send_sim_pose();
    server_mainloop();
}
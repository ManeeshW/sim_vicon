#ifndef SIM_VICON_TRACKER_H
#define SIM_VICON_TRACKER_H

#include <vrpn_Tracker.h>
#include <vrpn_Connection.h>
#include <string>

class SimViconTracker : public vrpn_Tracker {
public:
    SimViconTracker(const std::string& name, vrpn_Connection* c,
                   double x, double y, double z,
                   double qx, double qy, double qz, double qw);
    
    void send_sim_pose();
    void mainloop() override;
    
    void updatePosition(double x, double y, double z);
    void updateQuaternion(double qx, double qy, double qz, double qw);

private:
    double current_pos[3];
    double current_quat[4];
};

#endif
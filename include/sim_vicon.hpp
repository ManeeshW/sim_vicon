#pragma once

#include <vrpn_Tracker.h>
#include <vrpn_Connection.h>
#include <string>
#include <Eigen/Dense>

class SimViconTracker : public vrpn_Tracker {
public:
    SimViconTracker(const std::string& name, vrpn_Connection* c);
    void setPose(const Eigen::Vector3d& pos, const Eigen::Quaterniond& quat);
    void send_sim_pose();
    void mainloop() override;

private:
    Eigen::Vector3d position_;
    Eigen::Quaterniond orientation_;
};

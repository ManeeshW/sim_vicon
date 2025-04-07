// SimViconTracker.h
#ifndef SIM_VICON_TRACKER_H
#define SIM_VICON_TRACKER_H

#include <vrpn_Tracker.h>
#include <vrpn_Connection.h>
#include <string>
#include <chrono>
#include "Config.h"

class SimViconTracker : public vrpn_Tracker {
public:
    SimViconTracker(const std::string& name, vrpn_Connection* c,
                   double x, double y, double z,
                   double qx, double qy, double qz, double qw);
    
    void send_sim_pose();
    void mainloop() override;
    
    void updatePosition(double x, double y, double z);
    void updateQuaternion(double qx, double qy, double qz, double qw);
    void updateVelocity(double vx, double vy, double vz);
    void updateAcceleration(double ax, double ay, double az);
    void updateAngularVelocity(double wx, double wy, double wz);
    
    void setTrajectoryMode(int mode, const Config& config);
    void setAccelerationMode(bool enabled);
    void reset(const Config& config);
    
    double getTime() const;

    // New getter methods
    const double* getPosition() const { return current_pos; }
    const double* getVelocity() const { return velocity; }
    const double* getAcceleration() const { return acceleration; }
    const double* getAngularVelocity() const { return angular_velocity; }
    const double* getQuaternion() const { return current_quat; }

private:
    double current_pos[3];
    double current_quat[4];
    double velocity[3] = {0.0, 0.0, 0.0};
    double acceleration[3] = {0.0, 0.0, 0.0};
    double angular_velocity[3] = {0.0, 0.0, 0.0};
    
    int trajectory_mode = 0;
    std::chrono::steady_clock::time_point start_time;
    double last_update_time = 0.0;
    double reset_time_interval = 120.0;
    
    double lissajous_A, lissajous_B, lissajous_a, lissajous_b, lissajous_delta;
    double circular_radius, circular_speed;
};

#endif
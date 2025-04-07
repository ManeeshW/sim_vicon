// SimViconTracker.cpp
#include "SimViconTracker.h"
#include "RotationUtils.h"
#include <iostream>
#include <iomanip>
#include <cmath>

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
    
    velocity[0] = velocity[1] = velocity[2] = 0.0;
    acceleration[0] = acceleration[1] = acceleration[2] = 0.0;
    angular_velocity[0] = angular_velocity[1] = angular_velocity[2] = 0.0;
    
    trajectory_mode = 0; // Default: no trajectory
    start_time = std::chrono::steady_clock::now();
    last_update_time = 0.0;
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
    double t = getTime();
    
    if (trajectory_mode > 0 && t - last_update_time >= reset_time_interval) {
        reset(Config());
        t = 0.0;
    }
    
    // Calculate delta time first
    double dt = t - last_update_time;
    
    if (dt > 0) { // Only update if time has passed
        if (trajectory_mode == 1) { // Lissajous
            current_pos[0] = lissajous_A * sin(lissajous_a * t);
            current_pos[1] = lissajous_B * sin(lissajous_b * t + lissajous_delta);
            velocity[0] = lissajous_A * lissajous_a * cos(lissajous_a * t);
            velocity[1] = lissajous_B * lissajous_b * cos(lissajous_b * t + lissajous_delta);
            acceleration[0] = -lissajous_A * lissajous_a * lissajous_a * sin(lissajous_a * t);
            acceleration[1] = -lissajous_B * lissajous_b * lissajous_b * sin(lissajous_b * t + lissajous_delta);
        } else if (trajectory_mode == 2) { // Circular
            current_pos[0] = circular_radius * cos(circular_speed * t);
            current_pos[1] = circular_radius * sin(circular_speed * t);
            velocity[0] = -circular_radius * circular_speed * sin(circular_speed * t);
            velocity[1] = circular_radius * circular_speed * cos(circular_speed * t);
            acceleration[0] = -circular_radius * circular_speed * circular_speed * cos(circular_speed * t);
            acceleration[1] = -circular_radius * circular_speed * circular_speed * sin(circular_speed * t);
        } else if (trajectory_mode == 3) { // Acceleration mode
            // Integrate acceleration to velocity and position
            for (int i = 0; i < 3; ++i) {
                velocity[i] += acceleration[i] * dt;
                current_pos[i] += velocity[i] * dt;
            }

            // Fix: Proper quaternion integration from angular velocity
            double wx = angular_velocity[0] * M_PI / 180.0; // Convert to rad/s
            double wy = angular_velocity[1] * M_PI / 180.0;
            double wz = angular_velocity[2] * M_PI / 180.0;
            
            // Calculate magnitude of angular velocity
            double omega_mag = sqrt(wx*wx + wy*wy + wz*wz);
            
            if (omega_mag > 1e-6) {
                // Normalized axis of rotation
                double axis_x = wx / omega_mag;
                double axis_y = wy / omega_mag;
                double axis_z = wz / omega_mag;
                
                // Angle rotated in this time step
                double angle = omega_mag * dt;
                
                // Create delta quaternion
                double sin_half_angle = sin(angle / 2.0);
                double cos_half_angle = cos(angle / 2.0);
                
                double dq_x = axis_x * sin_half_angle;
                double dq_y = axis_y * sin_half_angle;
                double dq_z = axis_z * sin_half_angle;
                double dq_w = cos_half_angle;
                
                // Multiply current quaternion by delta quaternion
                double qx = current_quat[0];
                double qy = current_quat[1];
                double qz = current_quat[2];
                double qw = current_quat[3];
                
                current_quat[0] = dq_w * qx + dq_x * qw + dq_y * qz - dq_z * qy;
                current_quat[1] = dq_w * qy - dq_x * qz + dq_y * qw + dq_z * qx;
                current_quat[2] = dq_w * qz + dq_x * qy - dq_y * qx + dq_z * qw;
                current_quat[3] = dq_w * qw - dq_x * qx - dq_y * qy - dq_z * qz;
                
                // Normalize the resulting quaternion
                double norm = sqrt(current_quat[0]*current_quat[0] + 
                              current_quat[1]*current_quat[1] + 
                              current_quat[2]*current_quat[2] + 
                              current_quat[3]*current_quat[3]);
                if (norm > 0.0) {
                    current_quat[0] /= norm;
                    current_quat[1] /= norm;
                    current_quat[2] /= norm;
                    current_quat[3] /= norm;
                }
            }
        }
    }

    send_sim_pose(); // Broadcast updated position and orientation
    server_mainloop();
    last_update_time = t;
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

void SimViconTracker::updateVelocity(double vx, double vy, double vz) {
    velocity[0] = vx;
    velocity[1] = vy;
    velocity[2] = vz;
}

void SimViconTracker::updateAcceleration(double ax, double ay, double az) {
    acceleration[0] = ax;
    acceleration[1] = ay;
    acceleration[2] = az;
}

void SimViconTracker::updateAngularVelocity(double wx, double wy, double wz) {
    angular_velocity[0] = wx;
    angular_velocity[1] = wy;
    angular_velocity[2] = wz;
}

void SimViconTracker::setTrajectoryMode(int mode, const Config& config) {
    std::cout << "Setting trajectory mode: " << mode << std::endl;

    trajectory_mode = mode;
    start_time = std::chrono::steady_clock::now();
    last_update_time = 0.0;
    reset_time_interval = config.trajectory_reset_time;

    lissajous_A = config.lissajous_A;
    lissajous_B = config.lissajous_B;
    lissajous_a = config.lissajous_a;
    lissajous_b = config.lissajous_b;
    lissajous_delta = config.lissajous_delta;
    circular_radius = config.circular_radius;
    circular_speed = config.circular_speed;

    if (mode == 3) {
        // Reset velocity and position on mode switch to avoid sudden jumps
        velocity[0] = velocity[1] = velocity[2] = 0.0;
        current_pos[0] = config.pos[0];
        current_pos[1] = config.pos[1];
        current_pos[2] = config.pos[2];
        auto quat = RotationUtils::eulerToQuaternion(config.euler[0], config.euler[1], config.euler[2]);
        current_quat[0] = quat[1]; // qx
        current_quat[1] = quat[2]; // qy
        current_quat[2] = quat[3]; // qz
        current_quat[3] = quat[0]; // qw
    }
}

void SimViconTracker::setAccelerationMode(bool enabled) {
    if (enabled) {
        setTrajectoryMode(3, Config()); // Use acceleration mode
    } else {
        setTrajectoryMode(0, Config()); // Back to position mode
    }
}

void SimViconTracker::reset(const Config& config) {
    start_time = std::chrono::steady_clock::now();
    last_update_time = 0.0;
    updatePosition(config.pos[0], config.pos[1], config.pos[2]);
    auto quat = RotationUtils::eulerToQuaternion(config.euler[0], config.euler[1], config.euler[2]);
    updateQuaternion(quat[1], quat[2], quat[3], quat[0]); // VRPN order: qx, qy, qz, qw
    updateVelocity(0.0, 0.0, 0.0);
    updateAcceleration(0.0, 0.0, 0.0);
    updateAngularVelocity(0.0, 0.0, 0.0);
    setTrajectoryMode(trajectory_mode, config); // Reapply current mode with new config
}

double SimViconTracker::getTime() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - start_time).count();
}
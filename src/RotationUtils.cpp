#include "RotationUtils.h"
#include <cmath>

std::vector<double> RotationUtils::eulerToQuaternion(double yaw, double pitch, double roll) {
    // Convert degrees to radians
    yaw = yaw * M_PI / 180.0;
    pitch = pitch * M_PI / 180.0;
    roll = roll * M_PI / 180.0;

    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);

    std::vector<double> quat(4);
    quat[0] = cy * cp * cr + sy * sp * sr;  // qw
    quat[1] = cy * cp * sr - sy * sp * cr;  // qx
    quat[2] = sy * cp * sr + cy * sp * cr;  // qy
    quat[3] = sy * cp * cr - cy * sp * sr;  // qz

    return quat;
}

std::vector<std::vector<double>> RotationUtils::quaternionToRotationMatrix(
    double qw, double qx, double qy, double qz) {
    std::vector<std::vector<double>> R(3, std::vector<double>(3));

    // Normalize quaternion (just in case)
    double norm = sqrt(qw*qw + qx*qx + qy*qy + qz*qz);
    qw /= norm; qx /= norm; qy /= norm; qz /= norm;

    // Rotation matrix elements
    R[0][0] = 1 - 2*qy*qy - 2*qz*qz;
    R[0][1] = 2*qx*qy - 2*qz*qw;
    R[0][2] = 2*qx*qz + 2*qy*qw;
    
    R[1][0] = 2*qx*qy + 2*qz*qw;
    R[1][1] = 1 - 2*qx*qx - 2*qz*qz;
    R[1][2] = 2*qy*qz - 2*qx*qw;
    
    R[2][0] = 2*qx*qz - 2*qy*qw;
    R[2][1] = 2*qy*qz + 2*qx*qw;
    R[2][2] = 1 - 2*qx*qx - 2*qy*qy;

    return R;
}
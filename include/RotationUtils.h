#ifndef ROTATION_UTILS_H
#define ROTATION_UTILS_H

#include <vector>

class RotationUtils {
public:
    static std::vector<double> eulerToQuaternion(double yaw, double pitch, double roll);
    static std::vector<std::vector<double>> quaternionToRotationMatrix(
        double qw, double qx, double qy, double qz);
};

#endif
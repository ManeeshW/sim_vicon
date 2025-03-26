# **Sim Vicon Tracker**

Sim Vicon Tracker is a lightweight simulation tool that emulates a Vicon motion capture system using the [VRPN](https://github.com/vrpn/vrpn) protocol. It enables developers to test and develop Vicon-dependent software **without requiring physical Vicon hardware**.

You can create a Vicon server **anywhere**, send custom pose messages, and simulate a complete 6DOF (position and orientation) tracking environment. This makes it ideal for offline development, debugging, or deployment on embedded and portable systems like Jetson or PCs.

---

## ✨ Features

- Emulates a VRPN-based Vicon tracker.
- Sends synthetic 6DOF pose data (position + quaternion) with configurable values.
- Runs on any machine—no Vicon hardware or setup required.
- Configurable via an INI file for tracker name, IP, frequency, position, and orientation.
- Adjustable update rate (default configurable via INI, e.g., 200 Hz).
- Includes utilities for converting Euler angles to quaternions and rotation matrices.
- Easily integrates with VRPN-compatible software.

---

## 🛠️ Installation

### Prerequisites

- C++17 compiler (e.g., `g++`, `clang++`)
- CMake ≥ 3.10
- [VRPN Library](https://github.com/vrpn/vrpn) installed (including `quat`)

> On Ubuntu:
```bash
sudo apt-get install libvrpn-dev libquat-dev cmake build-essential
```

### Build Steps

```bash
git clone https://github.com/yourusername/sim_vicon_tracker.git
cd sim_vicon_tracker
mkdir build && cd build
cmake ..
make
```

---

## 🚀 How to Run

```bash
./sim_vicon_tracker
```

This starts a simulated Vicon server broadcasting pose data based on settings in `sim_vicon.ini`. By default, it uses the tracker name `Origins` on IP `10.0.0.200` and port `3883`.

Make sure your client code connects to:
```
"Origins@10.0.0.200:3883"
```

### To Customize Pose

Edit the `sim_vicon.ini` file in the project root to set your desired tracker name, server IP, update frequency, position, and orientation (Euler angles in degrees). Example:
```
[vicon]
object_name = Origins
server_ip = 10.0.0.200
frequency = 200
[position]
x1 = 1.0
x2 = -2.0
x3 = 1.5
[rotation]
yaw = 90.0
pitch = 0.0
roll = 0.0
```

Copy `sim_vicon.ini` to the `build` directory (or adjust the path in `main.cpp`) before running.

---

## 📁 File Structure

```
sim_vicon/
├── CMakeLists.txt         # CMake build configuration
├── sim_vicon.ini          # Configuration file for tracker settings
├── README.md              # This file
├── include/               # Header files
│   ├── RotationUtils.h    # Quaternion and rotation matrix utilities
│   └── SimViconTracker.h  # Simulated Vicon tracker class definition
└── src/                   # Source files
    ├── main.cpp           # Main program entry point
    ├── RotationUtils.cpp  # Implementation of rotation utilities
    └── SimViconTracker.cpp# Implementation of the simulated tracker
```

---

## 📜 License

This project is licensed under the terms specified in the `LICENSE` file provided with the source code. (Note: Add a `LICENSE` file to your repository if not already present.)

---

## 👤 Author

Developed and maintained by:

**Maneesha Wickramasuriya (Maneesh)**  
Graduate Researcher  
George Washington University  
📧 [maneesh@gwmail.gwu.edu](mailto:maneesh@gwmail.gwu.edu)

---

### Notes for Implementation
1. **File Structure**: Updated to reflect the current project layout with `include/` and `src/` directories, plus the `sim_vicon.ini` file.
2. **C++17**: Changed from C++11 to C++17 to match your `CMakeLists.txt`.
3. **INI Configuration**: Highlighted the use of `sim_vicon.ini` for customization, replacing the original instruction to edit `send_sim_pose()`.
4. **Frequency**: Made it clear the update rate is configurable via INI, not hardcoded at 50 Hz.

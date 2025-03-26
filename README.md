# **Sim Vicon Tracker**

Sim Vicon Tracker is a lightweight simulation tool that mimics a Vicon motion capture system using the [VRPN](https://github.com/vrpn/vrpn) protocol. It allows developers to test and develop Vicon-dependent software **without access to physical Vicon hardware**.

You can create a Vicon server **anywhere**, send custom pose messages, and simulate a complete pose tracking environment. This makes it ideal for offline development, debugging, or use on embedded and portable systems.

---

## ✨ Features

- Emulates a VRPN-based Vicon tracker.
- Sends synthetic 6DOF pose data (position + quaternion).
- Runs on any machine (Jetson, PC, etc.)—no Vicon setup needed.
- Customizable pose messages for testing.
- 50 Hz update rate (adjustable).
- Easily integrable into VRPN-compatible software.

---

## 🛠️ Installation

### Prerequisites

- C++11 compiler (e.g., `g++`, `clang++`)
- CMake ≥ 3.10
- [VRPN Library](https://github.com/vrpn/vrpn) installed (including `quat`)

> On Ubuntu:
```bash
sudo apt-get install libvrpn-dev libvrpn-dev cmake build-essential
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

This starts a simulated Vicon server broadcasting pose data with the tracker name `Origins` on IP `10.0.0.200` and port `3883`.

Make sure your client code connects to:
```
"Origins@10.0.0.200:3883"
```

### To Customize Pose

Edit the `send_sim_pose()` function in `sim_vicon_tracker.cpp` to set your desired simulated position and orientation (quaternion).

---

## 📁 File Structure

```bash
sim_vicon_tracker/
├── CMakeLists.txt
├── sim_vicon_tracker.cpp
└── LICENSE
```

---

## 📜 License

This project is licensed under the terms specified in the `LICENSE` file provided with the source code.

---

## 👤 Author

Developed and maintained by:

**Maneesha Wickramasuriya (Maneesh)**  
Graduate Researcher  
George Washington University  
📧 [maneesh@gwmail.gwu.edu](mailto:maneesh@gwmail.gwu.edu)

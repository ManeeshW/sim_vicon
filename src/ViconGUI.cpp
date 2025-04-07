// ViconGUI.cpp
#include "ViconGUI.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QStackedWidget>  
#include <QComboBox>
#include <iomanip>
#include <iostream>

ViconGUI::ViconGUI(SimViconTracker* tracker, const Config& config, QWidget* parent)
    : QMainWindow(parent), tracker(tracker), config(config) {
    setupUI();

    // Initialize position/attitude
    for (int i = 0; i < 3; ++i) {
        posEdits[i]->setText(QString::number(config.pos[i]));
        angleEdits[i]->setText(QString::number(config.euler[i]));
        accEdits[i]->setText("0.0");
        angVelEdits[i]->setText("0.0");
    }
    
    // Initialize trajectory parameters
    lissajousEdits[0]->setText(QString::number(config.lissajous_A));
    lissajousEdits[1]->setText(QString::number(config.lissajous_B));
    lissajousEdits[2]->setText(QString::number(config.lissajous_a));
    lissajousEdits[3]->setText(QString::number(config.lissajous_b));
    lissajousEdits[4]->setText(QString::number(config.lissajous_delta));
    circularEdits[0]->setText(QString::number(config.circular_radius));
    circularEdits[1]->setText(QString::number(config.circular_speed));

    displayTimer = new QTimer(this);
    connect(displayTimer, &QTimer::timeout, this, &ViconGUI::updateDisplay);
    displayTimer->start(100);

    updatePose();
}

ViconGUI::~ViconGUI() {
    // Clean up any resources if needed
    if (displayTimer) {
        displayTimer->stop();
        delete displayTimer;
    }
}

void ViconGUI::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 5, 10, 10);

    // Mode selection
    modeCombo = new QComboBox(this);
    modeCombo->addItems({"Position & Orientation", "Trajectory - Lissajous", "Trajectory - Circular", "Acceleration & Angular Velocity"});
    mainLayout->addWidget(new QLabel("Mode:"));
    mainLayout->addWidget(modeCombo);
    connect(modeCombo, &QComboBox::currentIndexChanged, this, &ViconGUI::modeChanged);

    // Create stacked widget for different control sets
    QStackedWidget* controlStack = new QStackedWidget(this);
    mainLayout->addWidget(controlStack);

    // Page 1: Position and Orientation controls
    QWidget* posOrientationPage = new QWidget();
    QVBoxLayout* posOrientationLayout = new QVBoxLayout(posOrientationPage);
    
    // Position controls
    QGroupBox* posGroup = new QGroupBox("Position (m)");
    QVBoxLayout* posLayout = new QVBoxLayout(posGroup);
    QString posLabels[3] = {"X", "Y", "Z"};
    for (int i = 0; i < 3; ++i) {
        QHBoxLayout* row = new QHBoxLayout();
        row->addWidget(new QLabel(posLabels[i] + ":"));
        posEdits[i] = new QLineEdit(this);
        posEdits[i]->setValidator(new QDoubleValidator(this));
        row->addWidget(posEdits[i]);
        QPushButton* incBtn = new QPushButton("+", this);
        QPushButton* decBtn = new QPushButton("-", this);
        connect(incBtn, &QPushButton::clicked, [this, i]() { incrementValue(i, true); });
        connect(decBtn, &QPushButton::clicked, [this, i]() { decrementValue(i, true); });
        row->addWidget(incBtn);
        row->addWidget(decBtn);
        posLayout->addLayout(row);
    }
    posOrientationLayout->addWidget(posGroup);

    // Orientation controls
    QGroupBox* orientationGroup = new QGroupBox("Orientation (deg)");
    QVBoxLayout* orientationLayout = new QVBoxLayout(orientationGroup);
    QString orientationLabels[3] = {"Yaw", "Pitch", "Roll"};
    for (int i = 0; i < 3; ++i) {
        QHBoxLayout* row = new QHBoxLayout();
        row->addWidget(new QLabel(orientationLabels[i] + ":"));
        angleEdits[i] = new QLineEdit(this);
        angleEdits[i]->setValidator(new QDoubleValidator(this));
        row->addWidget(angleEdits[i]);
        QPushButton* incBtn = new QPushButton("+", this);
        QPushButton* decBtn = new QPushButton("-", this);
        connect(incBtn, &QPushButton::clicked, [this, i]() { incrementValue(i, false); });
        connect(decBtn, &QPushButton::clicked, [this, i]() { decrementValue(i, false); });
        row->addWidget(incBtn);
        row->addWidget(decBtn);
        orientationLayout->addLayout(row);
    }
    posOrientationLayout->addWidget(orientationGroup);
    controlStack->addWidget(posOrientationPage);

    // Page 2: Acceleration and Angular Velocity controls
    QWidget* accAngVelPage = new QWidget();
    QVBoxLayout* accAngVelLayout = new QVBoxLayout(accAngVelPage);
    
    // Acceleration controls
    QGroupBox* accGroup = new QGroupBox("Acceleration (m/s²)");
    QVBoxLayout* accLayout = new QVBoxLayout(accGroup);
    QString accLabels[3] = {"X", "Y", "Z"};
    for (int i = 0; i < 3; ++i) {
        QHBoxLayout* row = new QHBoxLayout();
        row->addWidget(new QLabel(accLabels[i] + ":"));
        accEdits[i] = new QLineEdit(this);
        accEdits[i]->setValidator(new QDoubleValidator(this));
        row->addWidget(accEdits[i]);
        QPushButton* incBtn = new QPushButton("+", this);
        QPushButton* decBtn = new QPushButton("-", this);
        connect(incBtn, &QPushButton::clicked, [this, i]() { incrementAccValue(i); });
        connect(decBtn, &QPushButton::clicked, [this, i]() { decrementAccValue(i); });
        row->addWidget(incBtn);
        row->addWidget(decBtn);
        accLayout->addLayout(row);
    }
    accAngVelLayout->addWidget(accGroup);

    // Angular Velocity controls
    QGroupBox* angVelGroup = new QGroupBox("Angular Velocity (deg/s)");
    QVBoxLayout* angVelLayout = new QVBoxLayout(angVelGroup);
    QString angVelLabels[3] = {"X", "Y", "Z"};
    for (int i = 0; i < 3; ++i) {
        QHBoxLayout* row = new QHBoxLayout();
        row->addWidget(new QLabel(angVelLabels[i] + ":"));
        angVelEdits[i] = new QLineEdit(this);
        angVelEdits[i]->setValidator(new QDoubleValidator(this));
        row->addWidget(angVelEdits[i]);
        QPushButton* incBtn = new QPushButton("+", this);
        QPushButton* decBtn = new QPushButton("-", this);
        connect(incBtn, &QPushButton::clicked, [this, i]() { incrementAngVelValue(i); });
        connect(decBtn, &QPushButton::clicked, [this, i]() { decrementAngVelValue(i); });
        row->addWidget(incBtn);
        row->addWidget(decBtn);
        angVelLayout->addLayout(row);
    }
    accAngVelLayout->addWidget(angVelGroup);
    controlStack->addWidget(accAngVelPage);

    // Trajectory parameters (visible in modes 1 and 2)
    QGroupBox* trajGroup = new QGroupBox("Trajectory Parameters");
    QVBoxLayout* trajLayout = new QVBoxLayout(trajGroup);
    QString lissajousLabels[5] = {"A", "B", "a", "b", "δ"};
    for (int i = 0; i < 5; ++i) {
        QHBoxLayout* row = new QHBoxLayout();
        row->addWidget(new QLabel(lissajousLabels[i] + ":"));
        lissajousEdits[i] = new QLineEdit(this);
        lissajousEdits[i]->setValidator(new QDoubleValidator(this));
        row->addWidget(lissajousEdits[i]);
        trajLayout->addLayout(row);
    }
    QString circularLabels[2] = {"Radius", "Speed"};
    for (int i = 0; i < 2; ++i) {
        QHBoxLayout* row = new QHBoxLayout();
        row->addWidget(new QLabel(circularLabels[i] + ":"));
        circularEdits[i] = new QLineEdit(this);
        circularEdits[i]->setValidator(new QDoubleValidator(this));
        row->addWidget(circularEdits[i]);
        trajLayout->addLayout(row);
    }
    mainLayout->addWidget(trajGroup);
    trajGroup->setVisible(false);

    // Current pose display
    QGroupBox* displayGroup = new QGroupBox("Current Pose");
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    timeLabel = new QLabel(this);
    posLabel = new QLabel(this);
    velLabel = new QLabel(this);
    accLabel = new QLabel(this);
    angVelLabel = new QLabel(this);
    quatLabel = new QLabel(this);
    rotMatrixLabel = new QLabel(this);
    displayLayout->addWidget(timeLabel);
    displayLayout->addWidget(posLabel);
    displayLayout->addWidget(velLabel);
    displayLayout->addWidget(accLabel);
    displayLayout->addWidget(angVelLabel);
    displayLayout->addWidget(quatLabel);
    displayLayout->addWidget(rotMatrixLabel);
    mainLayout->addWidget(displayGroup);

    // Reset button
    QPushButton* resetBtn = new QPushButton("Reset", this);
    connect(resetBtn, &QPushButton::clicked, this, &ViconGUI::reset);
    mainLayout->addWidget(resetBtn);

    // Connect editing finished signals
    for (auto edit : posEdits) connect(edit, &QLineEdit::editingFinished, this, &ViconGUI::updatePose);
    for (auto edit : angleEdits) connect(edit, &QLineEdit::editingFinished, this, &ViconGUI::updatePose);
    for (auto edit : lissajousEdits) connect(edit, &QLineEdit::editingFinished, this, &ViconGUI::updatePose);
    for (auto edit : circularEdits) connect(edit, &QLineEdit::editingFinished, this, &ViconGUI::updatePose);
    for (auto edit : accEdits) connect(edit, &QLineEdit::editingFinished, this, &ViconGUI::updatePose);
    for (auto edit : angVelEdits) connect(edit, &QLineEdit::editingFinished, this, &ViconGUI::updatePose);

    // Style sheet
    setStyleSheet(
        "QMainWindow { background-color: #2b2b2b; }"
        "QGroupBox { background-color: #353535; border: 1px solid #555555; border-radius: 5px; margin-top: 1ex; color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 3px; color: #ffffff; }"
        "QLabel { color: #ffffff; }"
        "QLineEdit { background-color: #404040; color: #ffffff; border: 1px solid #555555; padding: 2px; }"
        "QPushButton { background-color: #505050; color: #ffffff; border: 1px solid #555555; padding: 2px; }"
        "QPushButton:hover { background-color: #606060; }"
        "QPushButton:pressed { background-color: #404040; }"
        "QComboBox { background-color: #404040; color: #ffffff; border: 1px solid #555555; padding: 2px; }"
        "QComboBox::drop-down { border-left: 1px solid #555555; }"
        "QComboBox QAbstractItemView { background-color: #404040; color: #ffffff; selection-background-color: #606060; }"
    );

    setWindowTitle("Sim Vicon Tracker");
    resize(500, 800);
}

void ViconGUI::incrementValue(int index, bool isPosition) {
    QLineEdit* edit = isPosition ? posEdits[index] : angleEdits[index];
    double value = edit->text().toDouble();
    value += isPosition ? 0.1 : 1.0;
    edit->setText(QString::number(value));
    updatePose();
}

void ViconGUI::decrementValue(int index, bool isPosition) {
    QLineEdit* edit = isPosition ? posEdits[index] : angleEdits[index];
    double value = edit->text().toDouble();
    value -= isPosition ? 0.1 : 1.0;
    edit->setText(QString::number(value));
    updatePose();
}

void ViconGUI::incrementAccValue(int index) {
    double value = accEdits[index]->text().toDouble();
    value += 0.001;
    accEdits[index]->setText(QString::number(value));
    // Only update acceleration, not the whole pose
    tracker->updateAcceleration(
        accEdits[0]->text().toDouble(),
        accEdits[1]->text().toDouble(),
        accEdits[2]->text().toDouble()
    );
}

void ViconGUI::decrementAccValue(int index) {
    double value = accEdits[index]->text().toDouble();
    value -= 0.001;
    accEdits[index]->setText(QString::number(value));
    tracker->updateAcceleration(
        accEdits[0]->text().toDouble(),
        accEdits[1]->text().toDouble(),
        accEdits[2]->text().toDouble()
    );
}

void ViconGUI::incrementAngVelValue(int index) {
    double value = angVelEdits[index]->text().toDouble();
    value += 0.1;
    angVelEdits[index]->setText(QString::number(value));
    tracker->updateAngularVelocity(
        angVelEdits[0]->text().toDouble(),
        angVelEdits[1]->text().toDouble(),
        angVelEdits[2]->text().toDouble()
    );
}

void ViconGUI::decrementAngVelValue(int index) {
    double value = angVelEdits[index]->text().toDouble();
    value -= 0.1;
    angVelEdits[index]->setText(QString::number(value));
    tracker->updateAngularVelocity(
        angVelEdits[0]->text().toDouble(),
        angVelEdits[1]->text().toDouble(),
        angVelEdits[2]->text().toDouble()
    );
}

void ViconGUI::modeChanged(int index) {
    std::cout << "Mode changed to: " << index << std::endl;

    // Get the stacked widget
    QStackedWidget* controlStack = centralWidget()->findChild<QStackedWidget*>();
    if (!controlStack) {
        std::cerr << "Error: Could not find control stack widget" << std::endl;
        return;
    }

    // Show the appropriate page based on mode
    if (index == 0 || index == 1 || index == 2) { // Position/Orientation or Trajectory modes
        controlStack->setCurrentIndex(0); // First page (position/orientation)
    } else if (index == 3) { // Acceleration mode
        controlStack->setCurrentIndex(1); // Second page (acceleration/angular velocity)
    }

    // Handle trajectory parameter visibility
    QGroupBox* trajGroup = centralWidget()->findChild<QGroupBox*>("Trajectory Parameters");
    if (trajGroup) {
        trajGroup->setVisible(index == 1 || index == 2);
        if (index == 1) { // Lissajous
            for (int i = 0; i < 5; ++i) if (lissajousEdits[i]) lissajousEdits[i]->setVisible(true);
            for (int i = 0; i < 2; ++i) if (circularEdits[i]) circularEdits[i]->setVisible(false);
        } else if (index == 2) { // Circular
            for (int i = 0; i < 5; ++i) if (lissajousEdits[i]) lissajousEdits[i]->setVisible(false);
            for (int i = 0; i < 2; ++i) if (circularEdits[i]) circularEdits[i]->setVisible(true);
        }
    }

    if (tracker) {
        tracker->setTrajectoryMode(index, config);
        if (index == 3) {
            // Initialize with current GUI values
            double acc[3], angVel[3];
            for (int i = 0; i < 3; ++i) {
                acc[i] = accEdits[i]->text().toDouble();
                angVel[i] = angVelEdits[i]->text().toDouble();
            }
            tracker->updateAcceleration(acc[0], acc[1], acc[2]);
            tracker->updateAngularVelocity(angVel[0], angVel[1], angVel[2]);
            displayTimer->start(50); // Update every 50ms for smoother integration
        } else if (index == 1 || index == 2) {
            displayTimer->start(50);
        } else {
            displayTimer->stop();
        }
        updatePose();
    } else {
        std::cerr << "Error: Tracker is null in modeChanged" << std::endl;
    }
}

void ViconGUI::updatePose() {
    int mode = modeCombo->currentIndex();
    
    if (mode == 0) {
        double pos[3], euler[3];
        for (int i = 0; i < 3; ++i) {
            pos[i] = posEdits[i]->text().toDouble();
            euler[i] = angleEdits[i]->text().toDouble();
        }
        tracker->updatePosition(pos[0], pos[1], pos[2]);
        auto quat = RotationUtils::eulerToQuaternion(euler[0], euler[1], euler[2]);
        std::vector<double> quat_vrpn = {quat[1], quat[2], quat[3], quat[0]};
        tracker->updateQuaternion(quat_vrpn[0], quat_vrpn[1], quat_vrpn[2], quat_vrpn[3]);
    } else if (mode == 1 || mode == 2) {
        Config newConfig = config;
        newConfig.lissajous_A = lissajousEdits[0]->text().toDouble();
        newConfig.lissajous_B = lissajousEdits[1]->text().toDouble();
        newConfig.lissajous_a = lissajousEdits[2]->text().toDouble();
        newConfig.lissajous_b = lissajousEdits[3]->text().toDouble();
        newConfig.lissajous_delta = lissajousEdits[4]->text().toDouble();
        newConfig.circular_radius = circularEdits[0]->text().toDouble();
        newConfig.circular_speed = circularEdits[1]->text().toDouble();
        tracker->setTrajectoryMode(mode, newConfig);
    } else if (mode == 3) {
        double acc[3], angVel[3];
        for (int i = 0; i < 3; ++i) {
            acc[i] = accEdits[i]->text().toDouble();
            angVel[i] = angVelEdits[i]->text().toDouble();
        }
        tracker->updateAcceleration(acc[0], acc[1], acc[2]);
        tracker->updateAngularVelocity(angVel[0], angVel[1], angVel[2]);
        tracker->setTrajectoryMode(3, config); // Ensure mode is set correctly
    }

    updateDisplay();
}

void ViconGUI::reset() {
    tracker->reset(config);
    
    // Update UI to match reset state
    for (int i = 0; i < 3; ++i) {
        posEdits[i]->setText(QString::number(config.pos[i]));
        angleEdits[i]->setText(QString::number(config.euler[i]));
        accEdits[i]->setText("0.0");
        angVelEdits[i]->setText("0.0");
    }
    
    // Reset trajectory parameters
    lissajousEdits[0]->setText(QString::number(config.lissajous_A));
    lissajousEdits[1]->setText(QString::number(config.lissajous_B));
    lissajousEdits[2]->setText(QString::number(config.lissajous_a));
    lissajousEdits[3]->setText(QString::number(config.lissajous_b));
    lissajousEdits[4]->setText(QString::number(config.lissajous_delta));
    circularEdits[0]->setText(QString::number(config.circular_radius));
    circularEdits[1]->setText(QString::number(config.circular_speed));
    
    updateDisplay(); // Only update display, don't trigger full pose update
}

void ViconGUI::updateDisplay() {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3); // For time and position
    
    oss << "Time: " << tracker->getTime() << "s";
    timeLabel->setText(QString::fromStdString(oss.str()));

    const double* pos = tracker->getPosition();
    oss.str("");
    oss << "Position (x, y, z): [" << pos[0] << ", " << pos[1] << ", " << pos[2] << "]";
    posLabel->setText(QString::fromStdString(oss.str()));

    const double* vel = tracker->getVelocity();
    oss.str("");
    oss << "Velocity (x, y, z): [" << vel[0] << ", " << vel[1] << ", " << vel[2] << "]";
    velLabel->setText(QString::fromStdString(oss.str()));

    // Use higher precision for acceleration and angular velocity
    oss.precision(6);
    const double* acc = tracker->getAcceleration();
    oss.str("");
    oss << "Acceleration (x, y, z): [" << acc[0] << ", " << acc[1] << ", " << acc[2] << "]";
    accLabel->setText(QString::fromStdString(oss.str()));

    const double* angVel = tracker->getAngularVelocity();
    oss.str("");
    oss << "Angular Vel (x, y, z): [" << angVel[0] << ", " << angVel[1] << ", " << angVel[2] << "]";
    angVelLabel->setText(QString::fromStdString(oss.str()));

    // Rest of the display code remains the same...
    oss.precision(3);
    const double* quat = tracker->getQuaternion();
    oss.str("");
    oss << "Quaternion (x, y, z, w): [" << quat[0] << ", " << quat[1] << ", " << quat[2] << ", " << quat[3] << "]";
    quatLabel->setText(QString::fromStdString(oss.str()));

    auto R = RotationUtils::quaternionToRotationMatrix(quat[3], quat[0], quat[1], quat[2]);
    oss.str("");
    oss << "Rotation Matrix:\n";
    for (int i = 0; i < 3; ++i) {
        oss << "[";
        for (int j = 0; j < 3; ++j) {
            oss << std::setw(8) << R[i][j];
            if (j < 2) oss << ", ";
        }
        oss << "]\n";
    }
    rotMatrixLabel->setText(QString::fromStdString(oss.str()));
}
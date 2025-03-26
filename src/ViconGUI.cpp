#include "ViconGUI.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <iomanip>
#include <sstream>

ViconGUI::ViconGUI(SimViconTracker* tracker, const Config& config, QWidget* parent)
    : QMainWindow(parent), tracker(tracker) {
    setupUI();

    // Set initial values from config
    posEdits[0]->setText(QString::number(config.pos[0]));
    posEdits[1]->setText(QString::number(config.pos[1]));
    posEdits[2]->setText(QString::number(config.pos[2]));
    angleEdits[0]->setText(QString::number(config.euler[0]));
    angleEdits[1]->setText(QString::number(config.euler[1]));
    angleEdits[2]->setText(QString::number(config.euler[2]));

    updatePose(); // Initial update
}

ViconGUI::~ViconGUI() {}

void ViconGUI::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Position Group
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
    mainLayout->addWidget(posGroup);

    // Attitude Group
    QGroupBox* attGroup = new QGroupBox("Attitude (deg)");
    QVBoxLayout* attLayout = new QVBoxLayout(attGroup);
    QString attLabels[3] = {"Yaw", "Pitch", "Roll"};
    for (int i = 0; i < 3; ++i) {
        QHBoxLayout* row = new QHBoxLayout();
        row->addWidget(new QLabel(attLabels[i] + ":"));
        angleEdits[i] = new QLineEdit(this);
        angleEdits[i]->setValidator(new QDoubleValidator(this));
        row->addWidget(angleEdits[i]);
        QPushButton* incBtn = new QPushButton("+", this);
        QPushButton* decBtn = new QPushButton("-", this);
        connect(incBtn, &QPushButton::clicked, [this, i]() { incrementValue(i, false); });
        connect(decBtn, &QPushButton::clicked, [this, i]() { decrementValue(i, false); });
        row->addWidget(incBtn);
        row->addWidget(decBtn);
        attLayout->addLayout(row);
    }
    mainLayout->addWidget(attGroup);

    // Display Group
    QGroupBox* displayGroup = new QGroupBox("Current Pose");
    QVBoxLayout* displayLayout = new QVBoxLayout(displayGroup);
    posLabel = new QLabel(this);
    quatLabel = new QLabel(this);
    rotMatrixLabel = new QLabel(this);
    displayLayout->addWidget(posLabel);
    displayLayout->addWidget(quatLabel);
    displayLayout->addWidget(rotMatrixLabel);
    mainLayout->addWidget(displayGroup);

    // Connect updates
    for (auto edit : posEdits) connect(edit, &QLineEdit::editingFinished, this, &ViconGUI::updatePose);
    for (auto edit : angleEdits) connect(edit, &QLineEdit::editingFinished, this, &ViconGUI::updatePose);

    // Apply dark theme
    setStyleSheet(
        "QMainWindow { background-color: #2b2b2b; }"
        "QGroupBox { background-color: #353535; border: 1px solid #555555; border-radius: 5px; margin-top: 1ex; color: #ffffff; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 3px; color: #ffffff; }"
        "QLabel { color: #ffffff; }"
        "QLineEdit { background-color: #404040; color: #ffffff; border: 1px solid #555555; padding: 2px; }"
        "QPushButton { background-color: #505050; color: #ffffff; border: 1px solid #555555; padding: 2px; }"
        "QPushButton:hover { background-color: #606060; }"
        "QPushButton:pressed { background-color: #404040; }"
    );

    setWindowTitle("Sim Vicon Tracker");
    resize(400, 500);
}

void ViconGUI::updatePose() {
    double pos[3], euler[3];
    for (int i = 0; i < 3; ++i) {
        pos[i] = posEdits[i]->text().toDouble();
        euler[i] = angleEdits[i]->text().toDouble();
    }

    tracker->updatePosition(pos[0], pos[1], pos[2]);
    auto quat = RotationUtils::eulerToQuaternion(euler[0], euler[1], euler[2]);
    std::vector<double> quat_vrpn = {quat[1], quat[2], quat[3], quat[0]};
    tracker->updateQuaternion(quat_vrpn[0], quat_vrpn[1], quat_vrpn[2], quat_vrpn[3]);

    updateDisplay();
}

void ViconGUI::incrementValue(int index, bool isPosition) {
    QLineEdit* edit = isPosition ? posEdits[index] : angleEdits[index];
    double value = edit->text().toDouble();
    value += isPosition ? 0.1 : 1.0; // 0.1m for position, 1° for angles
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

void ViconGUI::updateDisplay() {
    double pos[3];
    for (int i = 0; i < 3; ++i) pos[i] = posEdits[i]->text().toDouble();
    
    auto quat = RotationUtils::eulerToQuaternion(
        angleEdits[0]->text().toDouble(),
        angleEdits[1]->text().toDouble(),
        angleEdits[2]->text().toDouble());
    auto R = RotationUtils::quaternionToRotationMatrix(quat[0], quat[1], quat[2], quat[3]);
    std::vector<double> quat_vrpn = {quat[1], quat[2], quat[3], quat[0]};

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << "Position (x, y, z): [" << pos[0] << ", " << pos[1] << ", " << pos[2] << "]";
    posLabel->setText(QString::fromStdString(oss.str()));

    oss.str("");
    oss << "Quaternion (x, y, z, w): [" << quat_vrpn[0] << ", " << quat_vrpn[1] << ", "
        << quat_vrpn[2] << ", " << quat_vrpn[3] << "]";
    quatLabel->setText(QString::fromStdString(oss.str()));

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
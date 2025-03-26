#ifndef VICON_GUI_H
#define VICON_GUI_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDoubleValidator>
#include "SimViconTracker.h"
#include "RotationUtils.h"
#include "Config.h"

class ViconGUI : public QMainWindow {
    Q_OBJECT

public:
    ViconGUI(SimViconTracker* tracker, const Config& config, QWidget* parent = nullptr);
    ~ViconGUI();

private slots:
    void updatePose();
    void incrementValue(int index, bool isPosition);
    void decrementValue(int index, bool isPosition);

private:
    SimViconTracker* tracker;
    QLineEdit* posEdits[3];    // x, y, z
    QLineEdit* angleEdits[3];  // yaw, pitch, roll
    QLabel* posLabel;
    QLabel* quatLabel;
    QLabel* rotMatrixLabel;

    void setupUI();
    void updateDisplay();
};

#endif
// ViconGUI.h
#ifndef VICON_GUI_H
#define VICON_GUI_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QDoubleValidator>
#include <QTimer>
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
    void incrementAccValue(int index);  // New slot for acceleration
    void decrementAccValue(int index);  // New slot for acceleration
    void incrementAngVelValue(int index);  // New slot for angular velocity
    void decrementAngVelValue(int index);  // New slot for angular velocity
    void modeChanged(int index);
    void reset();

private:
    SimViconTracker* tracker;
    Config config;
    QComboBox* modeCombo;
    QLineEdit* posEdits[3];
    QLineEdit* angleEdits[3];
    QLineEdit* velEdits[3];
    QLineEdit* accEdits[3];
    QLineEdit* angVelEdits[3];
    QLineEdit* lissajousEdits[5];
    QLineEdit* circularEdits[2];
    QLabel* timeLabel;
    QLabel* posLabel;
    QLabel* velLabel;
    QLabel* accLabel;
    QLabel* angVelLabel;
    QLabel* quatLabel;
    QLabel* rotMatrixLabel;
    QTimer* displayTimer;

    void setupUI();
    void updateDisplay();
    
};

#endif
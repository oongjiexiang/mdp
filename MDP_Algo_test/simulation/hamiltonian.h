#ifndef HAMILTONIAN_H
#define HAMILTONIAN_H

#include "component.h"
#include "pathPlanning.h"
#include "action.h"
#include <vector>
#include <QDialog>
#include <QGroupBox>

using namespace std;

namespace Ui {
class Hamiltonian;
}

class Hamiltonian : public QDialog
{
    Q_OBJECT

public:
    explicit Hamiltonian(QWidget *parent = nullptr);
    ~Hamiltonian();

private:
    // Qt UI elements
    Ui::Hamiltonian *ui;
    QGroupBox *verticalGroupBox;
    QGroupBox *gridGroupBox;
    QGroupBox *horizontalGroupBox;
    vector<vector<QPushButton*>> buttons;
    vector<QPushButton*> controlButtons;

    // search variables
    vector<Obstacle*> obstacles;
    vector<ActionListPerObstacle> results;
    State* curState;
    State* nextState;
    int showingObstacle = 0;
    int showingStep = 0;

    // initial setup. Call once
    void setupSimulation();
    void setupMap();
    void setupRobotLocation();
    void setupControlPanel();

    // UI
    QPushButton* createPushButton(int row, int col);
    void obstacleInput();

    // When Hamiltonian path search begins
    void startHamiltonianCalculation();
    void displayRobotLocation();
    void displayNextStep();
    void simulate();
};

#endif // HAMILTONIAN_H

#ifndef HAMILTONIAN_H
#define HAMILTONIAN_H

#include "component.h"
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
    Ui::Hamiltonian *ui;
    enum { ROW_COUNT = 10, COLUMN_COUNT = 10 };
    vector<Obstacle*> obstacles;
    QGroupBox *verticalGroupBox;
    QGroupBox *gridGroupBox;
    QGroupBox *horizontalGroupBox;
    vector<vector<QPushButton*>> buttons;
    vector<QPushButton*> controlButtons;

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
    void simulate();
};

#endif // HAMILTONIAN_H

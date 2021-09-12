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
    void createHorizontalGroupBox();
    void createGridGroupBox();
    void createFormGroupBox();
    QPushButton* createPushButton(int row, int col);
    void obstacleInput();

    QGroupBox *horizontalGroupBox;
    QGroupBox *gridGroupBox;
    QGroupBox *formGroupBox;
    vector<vector<QPushButton*>> buttons;
};

#endif // HAMILTONIAN_H

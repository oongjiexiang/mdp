#include "hamiltonian.h"
#include "component.h"
#include "config.h"
#include "action.h"
#include "asearch.h"
#include "pathPlanning.h"
#include "simulator.h"
#include "ui_hamiltonian.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <iostream>

const int BUTTON_SIZE = 50;
static int obstacleId = 1;

Hamiltonian::Hamiltonian(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Hamiltonian)
{
//    ui->setupUi(this);
    setupSimulation();
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(verticalGroupBox);

    setLayout(mainLayout);
    setWindowTitle(tr("Simulation of Hamiltonian Path"));

}
void Hamiltonian::setupSimulation()
{
    verticalGroupBox = new QGroupBox(tr(""));
    QVBoxLayout *layout = new QVBoxLayout;

    setupMap();
    setupControlPanel();
    layout->addWidget(gridGroupBox);
    layout->addWidget(horizontalGroupBox);
    verticalGroupBox->setLayout(layout);
}
void Hamiltonian::setupMap()
{
    gridGroupBox = new QGroupBox(tr("Top Down Map"));
    buttons.resize(Y_GRID_COUNT, vector<QPushButton*>(X_GRID_COUNT));
    QGridLayout *gridLayout = new QGridLayout;
    for (int i = Y_GRID_COUNT - 1; i >= 0; i--) {
        for(int j = 0; j < X_GRID_COUNT; ++j){
            buttons[j][i] = createPushButton(j, i);
            gridLayout->addWidget(buttons[j][i], Y_GRID_COUNT - i - 1, j);
        }
    }
    setupRobotLocation();
    gridLayout->setSpacing(0);
    gridLayout->setColumnStretch(1, 10);

    gridGroupBox->setLayout(gridLayout);
}
void Hamiltonian::setupRobotLocation(){
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            // currently hard code initial position as (1, 1), will need to change later
            buttons[ROBOT_INIT_X_GRID + i][ROBOT_INIT_Y_GRID + j]->setEnabled(false);
            buttons[ROBOT_INIT_X_GRID + i][ROBOT_INIT_Y_GRID + j]->setStyleSheet("border: 1px solid black; margin: 1px; width: 20px; height: 20px; background: yellow");
        }
    }
}

QPushButton* Hamiltonian::createPushButton(int xGrid, int yGrid){
    QPushButton* button = new QPushButton("");
    button->setStyleSheet("border: 1px solid black; margin: 1px; width: 20px; height: 20px; background: white");
    button->setProperty("xGrid", xGrid);
    button->setProperty("yGrid", yGrid);
    connect(button, &QPushButton::clicked, this, &Hamiltonian::obstacleInput);
    return button;
}
void Hamiltonian::setupControlPanel()
{
    horizontalGroupBox = new QGroupBox(tr("Control Panel"));
    QHBoxLayout *hLayout = new QHBoxLayout;
    QPushButton* start = new QPushButton("Start Search");
    controlButtons.push_back(start);
    connect(start, &QPushButton::clicked, this, &Hamiltonian::startHamiltonianCalculation);

    hLayout->addWidget(start);
    horizontalGroupBox->setLayout(hLayout);
}
void Hamiltonian::obstacleInput(){
    int yGrid = sender()->property("yGrid").toInt();
    int xGrid = sender()->property("xGrid").toInt();
    qDebug().nospace() << qPrintable(QString::number(xGrid)) << " " << qPrintable(QString::number(yGrid));
    for(unsigned int i = 0; i < obstacles.size(); i++){
        if(obstacles[i]->yGrid == yGrid && obstacles[i]->xGrid == xGrid){
            qDebug() << qPrintable(QString::number((int)(obstacles[i]->face_direction)));
            switch((int)(obstacles[i]->face_direction)){
            case 90:
                buttons[xGrid][yGrid]->setText("W");
                obstacles[i]->face_direction = 180;
                break;
            case 180:
                buttons[xGrid][yGrid]->setText("S");
                obstacles[i]->face_direction = 270;
                break;
            case 0:
                buttons[xGrid][yGrid]->setText("N");
                obstacles[i]->face_direction = 90;
                break;
            default:    // remove obstacle after clicking four times
                obstacles.erase(obstacles.begin() + i);
                buttons[xGrid][yGrid]->setText("");
                buttons[xGrid][yGrid]->setStyleSheet("border: 1px solid black; margin: 1px; width: 20px; height: 20px; background: white");
            }
            return;
        }
    }
    // new obstacle
    Obstacle* obs = new Obstacle(obstacleId++, xGrid, yGrid, 0);    // obstacle id is temporary. Before obstacle vector is sent for A*, the obstacle id will be modified again. Need another function
    buttons[xGrid][yGrid]->setText("E");
    buttons[xGrid][yGrid]->setStyleSheet("border: 1px solid black; margin: 1px; width: 20px; height: 20px; background: gray");
    obstacles.push_back(obs);
}
void Hamiltonian::startHamiltonianCalculation(){
    int obstacleSize = obstacles.size();
    for(int i = 0; i < obstacleSize; i++){
        obstacles[i]->id = i + 1;
    }
    // call other file's Hamiltonian path search
    for(int i = 0; i < obstacleSize; i++){
        obstacles[i]->printObstacle();
    }
    simulate();
}
void Hamiltonian::simulate(){
    cout << "this function to simulate every second" << endl;
    vector<Obstacle> simulateObstacles;
    for(int i = 0; i < obstacles.size(); i++){
        simulateObstacles.push_back(*obstacles[i]);
    }
    ShortestPath sp(simulateObstacles);
    vector<ActionListPerObstacle> result = sp.hamiltonianPath();
    cout << result.size() << endl;
    cout << "result done. Sending to simulator" << endl;
    Simulator simulator;
    simulator.setModal(true);
    simulator.exec();
}
Hamiltonian::~Hamiltonian()
{
    delete ui;
}

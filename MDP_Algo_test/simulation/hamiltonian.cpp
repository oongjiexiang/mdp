#include "hamiltonian.h"
#include "component.h"
#include "config.h"
#include "action.h"
#include "asearch.h"
#include "pathPlanning.h"
#include "ui_hamiltonian.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <iostream>

const int BUTTON_SIZE = 50;
static int obstacleId = 1;

const QString ROBOT = "border: 1px solid black; margin: 1px; width: 20px; height: 20px; background: yellow";
const QString OBSTACLE = "border: 1px solid black; margin: 1px; width: 20px; height: 20px; background: gray";
const QString ROBOT_DIRECTION = "border: 1px solid black; margin: 1px; width: 20px; height: 20px; background-color: red";
const QString NORMAL = "border: 1px solid black; margin: 1px; width: 20px; height: 20px; background: white";

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
            buttons[ROBOT_INIT_X_GRID + i][ROBOT_INIT_Y_GRID + j]->setStyleSheet(ROBOT);
        }
    }
}

QPushButton* Hamiltonian::createPushButton(int xGrid, int yGrid){
    QPushButton* button = new QPushButton("");
    button->setStyleSheet(NORMAL);
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
    QPushButton* nextTarget = new QPushButton("Next");
    nextTarget->setEnabled(false);

    controlButtons.push_back(start);
    controlButtons.push_back(nextTarget);
    connect(start, &QPushButton::clicked, this, &Hamiltonian::startHamiltonianCalculation);
    connect(nextTarget, &QPushButton::clicked, this, &Hamiltonian::displayNextStep);

    hLayout->addWidget(start);
    hLayout->addWidget(nextTarget);
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
    buttons[xGrid][yGrid]->setStyleSheet(OBSTACLE);
    obstacles.push_back(obs);
}
void Hamiltonian::startHamiltonianCalculation(){
    int obstacleSize = obstacles.size();
    for(int i = 0; i < obstacleSize; i++){
        obstacles[i]->id = i + 1;
    }
    simulate();
}
void Hamiltonian::simulate(){
    controlButtons[1]->setEnabled(true);

    vector<Obstacle> simulateObstacles;
    for(int i = 0; i < obstacles.size(); i++){
        simulateObstacles.push_back(*obstacles[i]);
    }
    ShortestPath sp(simulateObstacles);
    results = sp.hamiltonianPath();
    
    vector<State*> states = results[showingObstacle].second;
    curState = states[showingStep++];
    nextState = states[showingStep];

    displayRobotLocation();
}

void Hamiltonian::displayNextStep(){
    if(showingStep >= results[showingObstacle].second.size()){
        if(showingObstacle == results.size() - 1){
            controlButtons[1]->setEnabled(false);
            return;
        }
        showingObstacle++;
        showingStep = 0;
        
        nextState = results[showingObstacle].second[showingStep++];
        displayRobotLocation();
        curState = nextState;
    }
    else{
        displayRobotLocation();
        curState = nextState;
        nextState = results[showingObstacle].second[showingStep++];
    }
}
void Hamiltonian::displayRobotLocation(){
    Vertex* initPosition = curState->position;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            buttons[initPosition->xGrid + i][initPosition->yGrid + j]->setStyleSheet(NORMAL);
        }
    }
    Vertex* nextPosition = nextState->position;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            buttons[nextPosition->xGrid + i][nextPosition->yGrid + j]->setStyleSheet(ROBOT);
        }
    }
    switch(nextState->face_direction){
        case 0:
            buttons[nextPosition->xGrid + 1][nextPosition->yGrid]->setStyleSheet(ROBOT_DIRECTION);
            break;
        case 90:
            buttons[nextPosition->xGrid][nextPosition->yGrid + 1]->setStyleSheet(ROBOT_DIRECTION);
            break;
        case 180:
            buttons[nextPosition->xGrid - 1][nextPosition->yGrid]->setStyleSheet(ROBOT_DIRECTION);
            break;
        case 270:
            buttons[nextPosition->xGrid][nextPosition->yGrid - 1]->setStyleSheet(ROBOT_DIRECTION);
            break;
    }
}

Hamiltonian::~Hamiltonian()
{
    delete ui;
}

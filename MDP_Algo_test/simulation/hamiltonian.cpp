#include "hamiltonian.h"
#include "ui_hamiltonian.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>

const int BUTTON_SIZE = 50;
static int obstacleId = 1;

Hamiltonian::Hamiltonian(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Hamiltonian)
{
//    ui->setupUi(this);
    createHorizontalGroupBox();
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(horizontalGroupBox);

    setLayout(mainLayout);
    setWindowTitle(tr("Simulation of Hamiltonian Path"));

}
void Hamiltonian::createHorizontalGroupBox()
{
    horizontalGroupBox = new QGroupBox(tr("Horizontal layout"));
    QHBoxLayout *layout = new QHBoxLayout;

    createGridGroupBox();
    createFormGroupBox();
    layout->addWidget(gridGroupBox);
    layout->addWidget(formGroupBox);
    horizontalGroupBox->setLayout(layout);
}
void Hamiltonian::createGridGroupBox()
{
    gridGroupBox = new QGroupBox(tr("Grid layout"));
    buttons.resize(ROW_COUNT, vector<QPushButton*>(COLUMN_COUNT));
    QGridLayout *gridLayout = new QGridLayout;
    for (int i = 0; i < ROW_COUNT; ++i) {
        for(int j = 0; j < COLUMN_COUNT; ++j){
            buttons[i][j] = createPushButton(i, j);
            gridLayout->addWidget(buttons[i][j], i, j);
        }
    }
    gridLayout->setSpacing(0);
    gridLayout->setColumnStretch(1, 10);


    gridGroupBox->setLayout(gridLayout);
}

QPushButton* Hamiltonian::createPushButton(int row, int col){
    QPushButton* button = new QPushButton("");
    button->setStyleSheet("border: 1px solid black; margin: 1px; width: 40px; height: 40px; background: white");
    button->setProperty("row", row);
    button->setProperty("col", col);
    connect(button, &QPushButton::clicked, this, &Hamiltonian::obstacleInput);
    return button;
}
void Hamiltonian::createFormGroupBox()
{
    formGroupBox = new QGroupBox(tr("Form layout"));
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Obstacle:")), new QComboBox);

    formGroupBox->setLayout(formLayout);
}
void Hamiltonian::obstacleInput(){
    int row = sender()->property("row").toInt();
    int col = sender()->property("col").toInt();
    qDebug().nospace() << qPrintable(QString::number(row)) << " " << qPrintable(QString::number(col));
    for(unsigned int i = 0; i < obstacles.size(); i++){
        if(obstacles[i]->row == row && obstacles[i]->column == col){
            qDebug() << qPrintable(QString::number((int)(obstacles[i]->face_direction)));
            switch((int)(obstacles[i]->face_direction)){
            case 90:
                buttons[row][col]->setText("W");
                obstacles[i]->face_direction = 180;
                break;
            case 180:
                buttons[row][col]->setText("S");
                obstacles[i]->face_direction = -90;
                break;
            case 0:
                buttons[row][col]->setText("N");
                obstacles[i]->face_direction = 90;
                break;
            default:    // remove obstacle after clicking four times
                obstacles.erase(obstacles.begin() + i);
                buttons[row][col]->setText("");
                buttons[row][col]->setStyleSheet("border: 1px solid black; margin: 1px; width: 40px; height: 40px; background: white");
            }
            return;
        }
    }
    // new obstacle
    Obstacle* obs = new Obstacle(obstacleId++, row, col, 0);    // obstacle id is temporary. Before obstacle vector is sent for A*, the obstacle id will be modified again. Need another function
    buttons[row][col]->setText("E");
    buttons[row][col]->setStyleSheet("border: 1px solid black; margin: 1px; width: 40px; height: 40px; background: gray");
    obstacles.push_back(obs);
}
Hamiltonian::~Hamiltonian()
{
    delete ui;
}

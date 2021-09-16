#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QDialog>

namespace Ui {
class Simulator;
}

class Simulator : public QDialog
{
    Q_OBJECT

public:
    explicit Simulator(QWidget *parent = nullptr);
    ~Simulator();

private:
    Ui::Simulator *ui;
};

#endif // SIMULATOR_H

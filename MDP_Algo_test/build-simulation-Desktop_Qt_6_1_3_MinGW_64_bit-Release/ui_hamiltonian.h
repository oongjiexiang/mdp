/********************************************************************************
** Form generated from reading UI file 'hamiltonian.ui'
**
** Created by: Qt User Interface Compiler version 6.1.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HAMILTONIAN_H
#define UI_HAMILTONIAN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_Hamiltonian
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Hamiltonian)
    {
        if (Hamiltonian->objectName().isEmpty())
            Hamiltonian->setObjectName(QString::fromUtf8("Hamiltonian"));
        Hamiltonian->resize(400, 300);
        buttonBox = new QDialogButtonBox(Hamiltonian);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(290, 20, 81, 241));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(Hamiltonian);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, Hamiltonian, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, Hamiltonian, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(Hamiltonian);
    } // setupUi

    void retranslateUi(QDialog *Hamiltonian)
    {
        Hamiltonian->setWindowTitle(QCoreApplication::translate("Hamiltonian", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Hamiltonian: public Ui_Hamiltonian {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HAMILTONIAN_H

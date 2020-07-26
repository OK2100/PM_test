/********************************************************************************
** Form generated from reading UI file 'zerolevelshistos.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZEROLEVELSHISTOS_H
#define UI_ZEROLEVELSHISTOS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ZeroLevelsHistos
{
public:
    QPushButton *pushButton;

    void setupUi(QWidget *ZeroLevelsHistos)
    {
        if (ZeroLevelsHistos->objectName().isEmpty())
            ZeroLevelsHistos->setObjectName(QString::fromUtf8("ZeroLevelsHistos"));
        ZeroLevelsHistos->resize(576, 409);
        pushButton = new QPushButton(ZeroLevelsHistos);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(130, 130, 251, 131));

        retranslateUi(ZeroLevelsHistos);

        QMetaObject::connectSlotsByName(ZeroLevelsHistos);
    } // setupUi

    void retranslateUi(QWidget *ZeroLevelsHistos)
    {
        ZeroLevelsHistos->setWindowTitle(QCoreApplication::translate("ZeroLevelsHistos", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("ZeroLevelsHistos", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZeroLevelsHistos: public Ui_ZeroLevelsHistos {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZEROLEVELSHISTOS_H

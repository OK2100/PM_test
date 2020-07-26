/********************************************************************************
** Form generated from reading UI file 'rawtdchistos.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAWTDCHISTOS_H
#define UI_RAWTDCHISTOS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "../../src/qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_RawTDChistos
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_4;
    QFrame *frame;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QSpinBox *spinBox;
    QPushButton *startBtn;
    QPushButton *stopBtn;
    QSpacerItem *verticalSpacer;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout;
    QFrame *frame_2;
    QGridLayout *gridLayout;
    QCustomPlot *h8;
    QCustomPlot *h11;
    QCustomPlot *h9;
    QCustomPlot *h3;
    QCustomPlot *h2;
    QCustomPlot *h12;
    QCustomPlot *h1;
    QCustomPlot *h4;
    QCustomPlot *h5;
    QCustomPlot *h10;
    QCustomPlot *h7;
    QCustomPlot *h6;
    QWidget *tab_2;
    QGridLayout *gridLayout_5;
    QFrame *frame_3;
    QGridLayout *gridLayout_2;
    QCustomPlot *h8_2;
    QCustomPlot *h11_2;
    QCustomPlot *h9_2;
    QCustomPlot *h3_2;
    QCustomPlot *h2_2;
    QCustomPlot *h12_2;
    QCustomPlot *h1_2;
    QCustomPlot *h4_2;
    QCustomPlot *h5_2;
    QCustomPlot *h10_2;
    QCustomPlot *h7_2;
    QCustomPlot *h6_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *RawTDChistos)
    {
        if (RawTDChistos->objectName().isEmpty())
            RawTDChistos->setObjectName(QString::fromUtf8("RawTDChistos"));
        RawTDChistos->resize(728, 480);
        centralwidget = new QWidget(RawTDChistos);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_4 = new QGridLayout(centralwidget);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        formLayout = new QFormLayout(frame);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        lineEdit = new QLineEdit(frame);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        spinBox = new QSpinBox(frame);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMinimum(2);
        spinBox->setMaximum(131072);
        spinBox->setSingleStep(50);
        spinBox->setValue(1024);

        formLayout->setWidget(1, QFormLayout::FieldRole, spinBox);

        startBtn = new QPushButton(frame);
        startBtn->setObjectName(QString::fromUtf8("startBtn"));

        formLayout->setWidget(2, QFormLayout::FieldRole, startBtn);

        stopBtn = new QPushButton(frame);
        stopBtn->setObjectName(QString::fromUtf8("stopBtn"));
        stopBtn->setEnabled(false);

        formLayout->setWidget(3, QFormLayout::FieldRole, stopBtn);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(4, QFormLayout::FieldRole, verticalSpacer);


        gridLayout_4->addWidget(frame, 0, 0, 1, 1);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout = new QVBoxLayout(tab);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame_2 = new QFrame(tab);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_2);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(1, 1, 1, 1);
        h8 = new QCustomPlot(frame_2);
        h8->setObjectName(QString::fromUtf8("h8"));

        gridLayout->addWidget(h8, 2, 4, 1, 1);

        h11 = new QCustomPlot(frame_2);
        h11->setObjectName(QString::fromUtf8("h11"));

        gridLayout->addWidget(h11, 3, 2, 1, 1);

        h9 = new QCustomPlot(frame_2);
        h9->setObjectName(QString::fromUtf8("h9"));

        gridLayout->addWidget(h9, 3, 0, 1, 1);

        h3 = new QCustomPlot(frame_2);
        h3->setObjectName(QString::fromUtf8("h3"));

        gridLayout->addWidget(h3, 1, 2, 1, 1);

        h2 = new QCustomPlot(frame_2);
        h2->setObjectName(QString::fromUtf8("h2"));

        gridLayout->addWidget(h2, 1, 1, 1, 1);

        h12 = new QCustomPlot(frame_2);
        h12->setObjectName(QString::fromUtf8("h12"));

        gridLayout->addWidget(h12, 3, 4, 1, 1);

        h1 = new QCustomPlot(frame_2);
        h1->setObjectName(QString::fromUtf8("h1"));

        gridLayout->addWidget(h1, 1, 0, 1, 1);

        h4 = new QCustomPlot(frame_2);
        h4->setObjectName(QString::fromUtf8("h4"));

        gridLayout->addWidget(h4, 1, 4, 1, 1);

        h5 = new QCustomPlot(frame_2);
        h5->setObjectName(QString::fromUtf8("h5"));

        gridLayout->addWidget(h5, 2, 0, 1, 1);

        h10 = new QCustomPlot(frame_2);
        h10->setObjectName(QString::fromUtf8("h10"));

        gridLayout->addWidget(h10, 3, 1, 1, 1);

        h7 = new QCustomPlot(frame_2);
        h7->setObjectName(QString::fromUtf8("h7"));

        gridLayout->addWidget(h7, 2, 2, 1, 1);

        h6 = new QCustomPlot(frame_2);
        h6->setObjectName(QString::fromUtf8("h6"));

        gridLayout->addWidget(h6, 2, 1, 1, 1);


        verticalLayout->addWidget(frame_2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_5 = new QGridLayout(tab_2);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        frame_3 = new QFrame(tab_2);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(frame_3);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(1, 1, 1, 1);
        h8_2 = new QCustomPlot(frame_3);
        h8_2->setObjectName(QString::fromUtf8("h8_2"));

        gridLayout_2->addWidget(h8_2, 2, 4, 1, 1);

        h11_2 = new QCustomPlot(frame_3);
        h11_2->setObjectName(QString::fromUtf8("h11_2"));

        gridLayout_2->addWidget(h11_2, 3, 2, 1, 1);

        h9_2 = new QCustomPlot(frame_3);
        h9_2->setObjectName(QString::fromUtf8("h9_2"));

        gridLayout_2->addWidget(h9_2, 3, 0, 1, 1);

        h3_2 = new QCustomPlot(frame_3);
        h3_2->setObjectName(QString::fromUtf8("h3_2"));

        gridLayout_2->addWidget(h3_2, 1, 2, 1, 1);

        h2_2 = new QCustomPlot(frame_3);
        h2_2->setObjectName(QString::fromUtf8("h2_2"));

        gridLayout_2->addWidget(h2_2, 1, 1, 1, 1);

        h12_2 = new QCustomPlot(frame_3);
        h12_2->setObjectName(QString::fromUtf8("h12_2"));

        gridLayout_2->addWidget(h12_2, 3, 4, 1, 1);

        h1_2 = new QCustomPlot(frame_3);
        h1_2->setObjectName(QString::fromUtf8("h1_2"));

        gridLayout_2->addWidget(h1_2, 1, 0, 1, 1);

        h4_2 = new QCustomPlot(frame_3);
        h4_2->setObjectName(QString::fromUtf8("h4_2"));

        gridLayout_2->addWidget(h4_2, 1, 4, 1, 1);

        h5_2 = new QCustomPlot(frame_3);
        h5_2->setObjectName(QString::fromUtf8("h5_2"));

        gridLayout_2->addWidget(h5_2, 2, 0, 1, 1);

        h10_2 = new QCustomPlot(frame_3);
        h10_2->setObjectName(QString::fromUtf8("h10_2"));

        gridLayout_2->addWidget(h10_2, 3, 1, 1, 1);

        h7_2 = new QCustomPlot(frame_3);
        h7_2->setObjectName(QString::fromUtf8("h7_2"));

        gridLayout_2->addWidget(h7_2, 2, 2, 1, 1);

        h6_2 = new QCustomPlot(frame_3);
        h6_2->setObjectName(QString::fromUtf8("h6_2"));

        gridLayout_2->addWidget(h6_2, 2, 1, 1, 1);


        gridLayout_5->addWidget(frame_3, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());

        gridLayout_4->addWidget(tabWidget, 0, 1, 1, 1);

        gridLayout_4->setColumnStretch(0, 1);
        gridLayout_4->setColumnStretch(1, 15);
        RawTDChistos->setCentralWidget(centralwidget);
        menubar = new QMenuBar(RawTDChistos);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 728, 20));
        RawTDChistos->setMenuBar(menubar);
        statusbar = new QStatusBar(RawTDChistos);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        RawTDChistos->setStatusBar(statusbar);

        retranslateUi(RawTDChistos);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(RawTDChistos);
    } // setupUi

    void retranslateUi(QMainWindow *RawTDChistos)
    {
        RawTDChistos->setWindowTitle(QCoreApplication::translate("RawTDChistos", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("RawTDChistos", "PM:", nullptr));
        lineEdit->setText(QCoreApplication::translate("RawTDChistos", "A0", nullptr));
        label_2->setText(QCoreApplication::translate("RawTDChistos", "N:", nullptr));
        startBtn->setText(QCoreApplication::translate("RawTDChistos", "Start", nullptr));
        stopBtn->setText(QCoreApplication::translate("RawTDChistos", "Stop", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("RawTDChistos", "TDC_RAW", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("RawTDChistos", "TDC_RAW (LS byte)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RawTDChistos: public Ui_RawTDChistos {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RAWTDCHISTOS_H

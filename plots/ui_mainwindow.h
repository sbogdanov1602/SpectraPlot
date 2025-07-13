/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <integraltableview.h>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionReset;
    QAction *actionCalculate_integral;
    QAction *actionCalculate_summ;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter_3;
    QSplitter *splitter;
    QCustomPlot *customPlot;
    QCustomPlot *customPlotH;
    QSplitter *splitter_2;
    QCustomPlot *customPlot2;
    IntegralTableView *tableViewH_2;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuTools;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1618, 968);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName("actionOpen");
        actionReset = new QAction(MainWindow);
        actionReset->setObjectName("actionReset");
        actionCalculate_integral = new QAction(MainWindow);
        actionCalculate_integral->setObjectName("actionCalculate_integral");
        actionCalculate_summ = new QAction(MainWindow);
        actionCalculate_summ->setObjectName("actionCalculate_summ");
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        splitter_3 = new QSplitter(centralWidget);
        splitter_3->setObjectName("splitter_3");
        splitter_3->setOrientation(Qt::Orientation::Vertical);
        splitter = new QSplitter(splitter_3);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        customPlot = new QCustomPlot(splitter);
        customPlot->setObjectName("customPlot");
        splitter->addWidget(customPlot);
        customPlotH = new QCustomPlot(splitter);
        customPlotH->setObjectName("customPlotH");
        splitter->addWidget(customPlotH);
        splitter_3->addWidget(splitter);
        splitter_2 = new QSplitter(splitter_3);
        splitter_2->setObjectName("splitter_2");
        splitter_2->setOrientation(Qt::Orientation::Horizontal);
        customPlot2 = new QCustomPlot(splitter_2);
        customPlot2->setObjectName("customPlot2");
        splitter_2->addWidget(customPlot2);
        tableViewH_2 = new IntegralTableView(splitter_2);
        tableViewH_2->setObjectName("tableViewH_2");
        splitter_2->addWidget(tableViewH_2);
        splitter_3->addWidget(splitter_2);

        verticalLayout->addWidget(splitter_3);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1618, 33));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        menuView = new QMenu(menuBar);
        menuView->setObjectName("menuView");
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName("menuTools");
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuFile->addAction(actionOpen);
        menuView->addAction(actionReset);
        menuTools->addAction(actionCalculate_integral);
        menuTools->addAction(actionCalculate_summ);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "SpectraPlot", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        actionReset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        actionCalculate_integral->setText(QCoreApplication::translate("MainWindow", "Calculate integral", nullptr));
        actionCalculate_summ->setText(QCoreApplication::translate("MainWindow", "Calculate 2D integral", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuTools->setTitle(QCoreApplication::translate("MainWindow", "Tools", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

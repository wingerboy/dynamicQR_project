/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *cameraLabel;
    QLabel *frameLabel;
    QLabel *resultLabel;
    QLabel *label_time;
    QPushButton *cameraButton;
    QRadioButton *captureChoose;
    QPushButton *grabButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1156, 625);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        cameraLabel = new QLabel(centralWidget);
        cameraLabel->setObjectName(QStringLiteral("cameraLabel"));
        cameraLabel->setGeometry(QRect(20, 20, 601, 451));
        cameraLabel->setScaledContents(true);
        frameLabel = new QLabel(centralWidget);
        frameLabel->setObjectName(QStringLiteral("frameLabel"));
        frameLabel->setGeometry(QRect(660, 20, 441, 331));
        resultLabel = new QLabel(centralWidget);
        resultLabel->setObjectName(QStringLiteral("resultLabel"));
        resultLabel->setGeometry(QRect(860, 480, 201, 31));
        QFont font;
        font.setFamily(QStringLiteral("Adobe Arabic"));
        font.setPointSize(12);
        resultLabel->setFont(font);
        label_time = new QLabel(centralWidget);
        label_time->setObjectName(QStringLiteral("label_time"));
        label_time->setGeometry(QRect(860, 400, 201, 31));
        label_time->setFont(font);
        cameraButton = new QPushButton(centralWidget);
        cameraButton->setObjectName(QStringLiteral("cameraButton"));
        cameraButton->setGeometry(QRect(490, 510, 75, 51));
        QFont font1;
        font1.setFamily(QStringLiteral("Adobe Arabic"));
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        cameraButton->setFont(font1);
        captureChoose = new QRadioButton(centralWidget);
        captureChoose->setObjectName(QStringLiteral("captureChoose"));
        captureChoose->setGeometry(QRect(680, 400, 89, 16));
        captureChoose->setFont(font);
        grabButton = new QPushButton(centralWidget);
        grabButton->setObjectName(QStringLiteral("grabButton"));
        grabButton->setGeometry(QRect(690, 490, 75, 51));
        grabButton->setFont(font1);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1156, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        cameraLabel->setText(QApplication::translate("MainWindow", "cameraLabel", 0));
        frameLabel->setText(QApplication::translate("MainWindow", "frameLabel", 0));
        resultLabel->setText(QApplication::translate("MainWindow", "\346\243\200\346\265\213\347\273\223\346\236\234", 0));
        label_time->setText(QApplication::translate("MainWindow", "\346\212\223\345\217\226\346\227\266\351\227\264", 0));
        cameraButton->setText(QApplication::translate("MainWindow", "\346\232\202\345\201\234", 0));
        captureChoose->setText(QApplication::translate("MainWindow", "\346\211\213\345\212\250\346\212\223\346\213\215", 0));
        grabButton->setText(QApplication::translate("MainWindow", "\346\212\223\346\213\215", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

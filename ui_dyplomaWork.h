/********************************************************************************
** Form generated from reading UI file 'dyplomaWork.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DYPLOMAWORK_H
#define UI_DYPLOMAWORK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dyplomaWorkClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *dyplomaWorkClass)
    {
        if (dyplomaWorkClass->objectName().isEmpty())
            dyplomaWorkClass->setObjectName(QStringLiteral("dyplomaWorkClass"));
        dyplomaWorkClass->resize(600, 400);
        centralWidget = new QWidget(dyplomaWorkClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        dyplomaWorkClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(dyplomaWorkClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 18));
        dyplomaWorkClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(dyplomaWorkClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        dyplomaWorkClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(dyplomaWorkClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        dyplomaWorkClass->setStatusBar(statusBar);

        retranslateUi(dyplomaWorkClass);

        QMetaObject::connectSlotsByName(dyplomaWorkClass);
    } // setupUi

    void retranslateUi(QMainWindow *dyplomaWorkClass)
    {
        dyplomaWorkClass->setWindowTitle(QApplication::translate("dyplomaWorkClass", "chipView", nullptr));
    } // retranslateUi

};

namespace Ui {
    class dyplomaWorkClass: public Ui_dyplomaWorkClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DYPLOMAWORK_H

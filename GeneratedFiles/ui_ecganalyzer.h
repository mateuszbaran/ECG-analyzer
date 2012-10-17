/********************************************************************************
** Form generated from reading UI file 'ecganalyzer.ui'
**
** Created: Wed 17. Oct 21:42:02 2012
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ECGANALYZER_H
#define UI_ECGANALYZER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ECGanalyzerClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ECGanalyzerClass)
    {
        if (ECGanalyzerClass->objectName().isEmpty())
            ECGanalyzerClass->setObjectName(QString::fromUtf8("ECGanalyzerClass"));
        ECGanalyzerClass->resize(600, 400);
        menuBar = new QMenuBar(ECGanalyzerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        ECGanalyzerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ECGanalyzerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ECGanalyzerClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ECGanalyzerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        ECGanalyzerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ECGanalyzerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ECGanalyzerClass->setStatusBar(statusBar);

        retranslateUi(ECGanalyzerClass);

        QMetaObject::connectSlotsByName(ECGanalyzerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ECGanalyzerClass)
    {
        ECGanalyzerClass->setWindowTitle(QApplication::translate("ECGanalyzerClass", "ECGanalyzer", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ECGanalyzerClass: public Ui_ECGanalyzerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ECGANALYZER_H

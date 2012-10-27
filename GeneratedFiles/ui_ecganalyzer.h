/********************************************************************************
** Form generated from reading UI file 'ecganalyzer.ui'
**
** Created: Sat 27. Oct 13:44:42 2012
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
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ECGanalyzerClass
{
public:
    QAction *actionO_Programie;
    QAction *actionO_Qt;
    QAction *actionOtw_rz;
    QAction *actionWyjd;
    QAction *actionZapisz;
    QAction *actionZapisz_jako;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuPlik;
    QMenu *menuPomoc;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ECGanalyzerClass)
    {
        if (ECGanalyzerClass->objectName().isEmpty())
            ECGanalyzerClass->setObjectName(QString::fromUtf8("ECGanalyzerClass"));
        ECGanalyzerClass->resize(817, 554);
        actionO_Programie = new QAction(ECGanalyzerClass);
        actionO_Programie->setObjectName(QString::fromUtf8("actionO_Programie"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ECGanalyzer/icons/info.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionO_Programie->setIcon(icon);
        actionO_Qt = new QAction(ECGanalyzerClass);
        actionO_Qt->setObjectName(QString::fromUtf8("actionO_Qt"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ECGanalyzer/icons/QtIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionO_Qt->setIcon(icon1);
        actionOtw_rz = new QAction(ECGanalyzerClass);
        actionOtw_rz->setObjectName(QString::fromUtf8("actionOtw_rz"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ECGanalyzer/icons/folder.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOtw_rz->setIcon(icon2);
        actionWyjd = new QAction(ECGanalyzerClass);
        actionWyjd->setObjectName(QString::fromUtf8("actionWyjd"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/ECGanalyzer/icons/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWyjd->setIcon(icon3);
        actionZapisz = new QAction(ECGanalyzerClass);
        actionZapisz->setObjectName(QString::fromUtf8("actionZapisz"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/ECGanalyzer/icons/3floppy_unmount.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionZapisz->setIcon(icon4);
        actionZapisz_jako = new QAction(ECGanalyzerClass);
        actionZapisz_jako->setObjectName(QString::fromUtf8("actionZapisz_jako"));
        actionZapisz_jako->setIcon(icon4);
        centralWidget = new QWidget(ECGanalyzerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        ECGanalyzerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ECGanalyzerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 817, 21));
        menuPlik = new QMenu(menuBar);
        menuPlik->setObjectName(QString::fromUtf8("menuPlik"));
        menuPomoc = new QMenu(menuBar);
        menuPomoc->setObjectName(QString::fromUtf8("menuPomoc"));
        ECGanalyzerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ECGanalyzerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ECGanalyzerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ECGanalyzerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ECGanalyzerClass->setStatusBar(statusBar);

        menuBar->addAction(menuPlik->menuAction());
        menuBar->addAction(menuPomoc->menuAction());
        menuPlik->addAction(actionOtw_rz);
        menuPlik->addSeparator();
        menuPlik->addAction(actionZapisz);
        menuPlik->addAction(actionZapisz_jako);
        menuPlik->addSeparator();
        menuPlik->addAction(actionWyjd);
        menuPomoc->addAction(actionO_Programie);
        menuPomoc->addSeparator();
        menuPomoc->addAction(actionO_Qt);

        retranslateUi(ECGanalyzerClass);

        QMetaObject::connectSlotsByName(ECGanalyzerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ECGanalyzerClass)
    {
        ECGanalyzerClass->setWindowTitle(QApplication::translate("ECGanalyzerClass", "ECGanalyzer", 0, QApplication::UnicodeUTF8));
        actionO_Programie->setText(QApplication::translate("ECGanalyzerClass", "O Programie...", 0, QApplication::UnicodeUTF8));
        actionO_Qt->setText(QApplication::translate("ECGanalyzerClass", "O Qt...", 0, QApplication::UnicodeUTF8));
        actionOtw_rz->setText(QApplication::translate("ECGanalyzerClass", "Otw\303\263rz", 0, QApplication::UnicodeUTF8));
        actionWyjd->setText(QApplication::translate("ECGanalyzerClass", "Wyjd\305\272", 0, QApplication::UnicodeUTF8));
        actionZapisz->setText(QApplication::translate("ECGanalyzerClass", "Zapisz", 0, QApplication::UnicodeUTF8));
        actionZapisz_jako->setText(QApplication::translate("ECGanalyzerClass", "Zapisz jako...", 0, QApplication::UnicodeUTF8));
        menuPlik->setTitle(QApplication::translate("ECGanalyzerClass", "Plik", 0, QApplication::UnicodeUTF8));
        menuPomoc->setTitle(QApplication::translate("ECGanalyzerClass", "Pomoc", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ECGanalyzerClass: public Ui_ECGanalyzerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ECGANALYZER_H

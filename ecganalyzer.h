#ifndef ECGANALYZER_H
#define ECGANALYZER_H

#include <QtGui/QMainWindow>
#include "ui_ecganalyzer.h"

class ECGanalyzer : public QMainWindow
{
	Q_OBJECT

public:
	ECGanalyzer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ECGanalyzer();

private slots:
    void on_actionO_Qt_triggered();

    void on_actionO_Programie_triggered();

    void on_actionWyjd_triggered();

private:
	Ui::ECGanalyzerClass ui;
};

#endif // ECGANALYZER_H

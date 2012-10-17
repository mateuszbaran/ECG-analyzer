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

private:
	Ui::ECGanalyzerClass ui;
};

#endif // ECGANALYZER_H

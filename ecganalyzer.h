#ifndef ECGANALYZER_H
#define ECGANALYZER_H

#include <QtGui/QMainWindow>
#include "ui_ecganalyzer.h"

#include "ECGController.h"

class ECGanalyzer : public QMainWindow
{
	Q_OBJECT

public:
	ECGanalyzer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ECGanalyzer();

private:
	Ui::ECGanalyzerClass ui;

  ECGController contr;
};

#endif // ECGANALYZER_H

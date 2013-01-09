#ifndef ECGANALYZER_H
#define ECGANALYZER_H

#include <QtGui/QMainWindow>
#ifdef WIN32
	#include "GeneratedFiles/ui_ecganalyzer.h"
	#include "GeneratedFiles/ui_AboutWindow.h"
#else
 #include "ui_ecganalyzer.h"
 #include "ui_AboutWindow.h"
#endif


#include "ECGController.h"
#include "AboutWindow.h"

class ECGanalyzer : public QMainWindow
{
	Q_OBJECT

public:
	ECGanalyzer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ECGanalyzer();

private slots:
    void on_actionO_Qt_triggered();

    void on_actionO_Programie_triggered();

    void on_actionWczytaj_plik_z_sygnalem_triggered();

    void on_actionWyjdz_triggered();

    void on_radioButtonMovingAverage_toggled(bool checked);

    void on_radioButtonButterworthFilter_toggled(bool checked);

    void on_checkBoxRPeaksDetectThresholdAutomatically_toggled(bool checked);

private:
	Ui::ECGanalyzerClass ui;
	AboutWindow aboutWindow;


	ECGController _ECGcontroller;
	//Ecg2Ch _ecg2ch;
};

#endif // ECGANALYZER_H

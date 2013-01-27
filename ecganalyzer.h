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
#include "plots/plottachogram.h"
#include "plots/plotpoincare.h"
#include "plots/plothrvtriangle.h"
#include "plots/plothrvfrequency.h"
#include "plots/plotdfa1.h"
#include "plots/plotdfa2.h"
#include "plots/plotqrs.h"
#include "plots/plothrt.h"
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

	void enableMovingAverageGUIControls(bool enable);

	void enableButterworthGUIControls(bool enable);

	void enableChebyschevGUIControls(bool enable);

    void on_radioButtonButterworthFilter_toggled(bool checked);

    void on_checkBoxRPeaksDetectThresholdAutomatically_toggled(bool checked);

    void on_actionPrzeprowadzPonownieAnalizeSygnalu_triggered();

    void on_actionZatrzymajPonownaAnalizeSygnalu_triggered();
    
    // For actions in ST interval module tab
    void on_run_st_analysis_button_clicked();
    void on_st_select_algorithm_currentIndexChanged(int value);    
    void on_st_intervals_table_cellDoubleClicked(int,int);
    void on_st_episodes_table_cellDoubleClicked(int,int);


    void on_radioButtonChebyschevFilter_toggled(bool checked);

    void on_doubleSpinBoxRPeaksHistogramSize_valueChanged(double arg1);

private:
	Ui::ECGanalyzerClass ui;
	AboutWindow aboutWindow;

	void updateRunButtons(bool analysisOngoing);

	void updateAnalysisStatus(std::string status);
    
    void setModulesParams();

	/**
     * Set params for R peaks module
     */
	void setRpeaksParams();
    
    /**
     * Set params for ST interval analysis module
     * 
     * Should be run *before* analysis
     */
    void setSTIntervalParams();
    
    /**
     * Updates ST interval Tab with data form ST analysis.
     * 
     * Should be run *only* after analysis completed!
     */
    void updateSTIntervalTab();

	 /**
     * Set params for HRV2 module
     */
    void setHRV2Params();

	ECGController _ECGcontroller;
	//Ecg2Ch _ecg2ch;
	
	PlotTachogram *plotTachogram;
	PlotHRVTriangle *plotHRVTriangle;
	PlotPoincare *plotPoincare;
	PlotHRVFrequency *plotHRVFrequency;
	PlotDFA1 *plotDFA1;
	PlotDFA2 *plotDFA2;
	PlotQRS *plotQRS;
	PlotHRT *plotHRT;
	
};

#endif // ECGANALYZER_H

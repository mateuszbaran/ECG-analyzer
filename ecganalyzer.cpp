#include "ecganalyzer.h"
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <QThread>
#include <functional>
#include <boost/foreach.hpp>

using namespace std;

ECGanalyzer::ECGanalyzer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

	ui.setupUi(this);
}

ECGanalyzer::~ECGanalyzer()
{

}

void ECGanalyzer::on_actionO_Qt_triggered()
{
    qApp->aboutQt();
}

void ECGanalyzer::on_actionO_Programie_triggered()
{
	aboutWindow.show();
}


/***********************************************************************
* Wywołuje modalne okienko wyboru pliku.
* Trzeba wskazać poprawny plik z sygnałem EKG,
* możliwy do wczytania przez bibliotekę WFDB,
* czyli jeden z trzech plików: .dat .hea .atr
* o współnym przedrostku zgodnym z jakimś id wewnątrz.
* Plik zostanie wczytany i wyświetlone zostaną dane
* z obu kanałów.
* Zaznaczenie fragmenu wykresu myszą przybliża go.
* Można wtedy przewijać wykres środkowym przyciskiem myszy.
* Oddala się prawym, ew. z wciśniętym klawiszem Ctrl (jest bug).
***********************************************************************/
void ECGanalyzer::on_actionWczytaj_plik_z_sygnalem_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if(!fileName.isNull() &&  !fileName.isEmpty())
    {
        if(!_ECGcontroller.readFile(fileName.toStdString()) ) // why?
		{
			QMessageBox::critical(NULL, "Błąd", "Nie udało się wczytać sygnału");
			return;//throw new exception("Nie udało się wczytać sygnału");
		}

        _ECGcontroller.runECGBaseline();
//        _ECGcontroller.runRPeaks();
        ui.rawPlotWidget->setSignal(&(_ECGcontroller.raw_signal), &(_ECGcontroller.ecg_info), &(_ECGcontroller.r_peaks_data));



		QTableWidgetItem *fileName = new QTableWidgetItem();
		fileName->setText(QString::fromStdString(_ECGcontroller.ecg_info.channel_one.filename) );
		ui.tableWidgetSignalInfo->setItem(0, 0, fileName);

		QTableWidgetItem *description = new QTableWidgetItem();
		description->setText(QString::fromStdString(_ECGcontroller.ecg_info.channel_one.description) );
		ui.tableWidgetSignalInfo->setItem(1, 0, description);

		QTableWidgetItem *signalResolution = new QTableWidgetItem();
		signalResolution->setText(QString().sprintf("%d bits", _ECGcontroller.ecg_info.channel_one.signal_resolution) );
		ui.tableWidgetSignalInfo->setItem(2, 0, signalResolution);

		QTableWidgetItem *zeroSignal = new QTableWidgetItem();
		zeroSignal->setText(QString().sprintf("%d", _ECGcontroller.ecg_info.channel_one.zero_signal) );
		ui.tableWidgetSignalInfo->setItem(3, 0, zeroSignal);

		QTableWidgetItem *gain = new QTableWidgetItem();
		gain->setText(QString().sprintf("%f", _ECGcontroller.ecg_info.channel_one.gain) );
		ui.tableWidgetSignalInfo->setItem(4, 0, gain);

		QTableWidgetItem *numberOfSamples = new QTableWidgetItem();
		numberOfSamples->setText(QString().sprintf("%d", _ECGcontroller.ecg_info.channel_one.nr_samples) );
		ui.tableWidgetSignalInfo->setItem(5, 0, numberOfSamples);

		QTableWidgetItem *frequency = new QTableWidgetItem();
		frequency->setText(QString().sprintf("%d Hz", _ECGcontroller.ecg_info.channel_one.frequecy) );
		ui.tableWidgetSignalInfo->setItem(6, 0, frequency);

		QTableWidgetItem *range = new QTableWidgetItem();
		range->setText(QString().sprintf("%d mV", _ECGcontroller.ecg_info.channel_one.range) );
		ui.tableWidgetSignalInfo->setItem(7, 0, range);
		
		/*
		_ECGcontroller.runHRT();

		QVBoxLayout *plotHARTLayout = new QVBoxLayout;
		PlotHRT *plotHRT = new PlotHRT(this);
		plotHARTLayout->addWidget(plotHRT);
		plotHRT->setData(_ECGcontroller.hrt_data);
		ui.groupBox->setLayout(plotHARTLayout);*/
	
		ECGHRV2 hrv2_data;
		// ...
		
		QVBoxLayout *plotHRVTriangleLayout = new QVBoxLayout;
		PlotHRVTriangle *plotHRVTriangle = new PlotHRVTriangle(this);
		plotHRVTriangleLayout->addWidget(plotHRVTriangle);
		//plotHRVTriangle->setData(hrv2_data);
		ui.tab_2->setLayout(plotHRVTriangleLayout);
		
		QVBoxLayout *plotPoincareLayout = new QVBoxLayout;
		PlotPoincare *plotPoincare = new PlotPoincare(this);
		plotPoincareLayout->addWidget(plotPoincare);
		//plotPoincare->setData(hrv2_data);
		ui.tab_3->setLayout(plotPoincareLayout);
		
		
		ui.actionPrzeprowadzPonownieAnalizeSygnalu->setEnabled(true);
		
    }
}

/**
 * Only for tests
 */
void ECGanalyzer::on_run_st_analysis_button_clicked()
{
  setSTIntervalParams();
  
  _ECGcontroller.runSTInterval();
  
  updateSTIntervalTab();
  updateAnalysisStatus("ST Analysis Ended!");
}

void ECGanalyzer::on_st_select_algorithm_currentIndexChanged(int value)
{
  switch(value) {
    case 1:
      this->ui.st_complex_params_box->setProperty("visible", true);
      this->ui.st_simple_params_box->setProperty("visible", false);
      break;
    case 0:
    default:
      this->ui.st_complex_params_box->setProperty("visible", false);
      this->ui.st_simple_params_box->setProperty("visible", true);
      break;    
  }
}

void ECGanalyzer::on_st_intervals_table_cellDoubleClicked(int row, int col)
{
  const auto & it = _ECGcontroller.st_data.getIntervalAt(row);
  ui.st_plot->showInterval(it);
}

void ECGanalyzer::on_st_episodes_table_cellDoubleClicked(int row, int col)
{
  const auto & ep = _ECGcontroller.st_data.getEpisodeAt(row);
  ui.st_plot->showEpisode(ep);

}



void ECGanalyzer::on_actionWyjdz_triggered()
{
    this->close();
}

void ECGanalyzer::on_radioButtonMovingAverage_toggled(bool checked)
{
    if(checked)
    {
		enableMovingAverageGUIControls(true);

		enableChebyschevGUIControls(false);
		enableButterworthGUIControls(false);
        //TODO: ustaw algorytm w ECGBaseline oraz przełącz zakładkę na GUI
    }
}

void ECGanalyzer::enableMovingAverageGUIControls(bool enable)
{
    ui.spinBoxMovingAverageSpan->setEnabled(enable);
}

void ECGanalyzer::enableChebyschevGUIControls(bool enable) 
{
	ui.spinBoxOrderChebyshev->setEnabled(enable);
	ui.spinBoxCutOffFrequencyChebyshev->setEnabled(enable);
	ui.doubleSpinBoxRippleChebyschev->setEnabled(enable);
	ui.spinBoxOrderChebyshev->setEnabled(enable);
}

void ECGanalyzer::enableButterworthGUIControls(bool enable)
{
	ui.doubleSpinBoxAttenuationButterworth->setEnabled(enable);
	ui.spinBoxCutOffFrequencyButterworth->setEnabled(enable);
	ui.spinBoxOrderButterworth->setEnabled(enable);
}


void ECGanalyzer::on_radioButtonButterworthFilter_toggled(bool checked)
{
    if(checked)
    {
		enableButterworthGUIControls(true);

		enableMovingAverageGUIControls(false);
		enableChebyschevGUIControls(false);
		

        //TODO: ustaw algorytm w ECGBaseline oraz przełącz zakładkę na GUI
    }
}

void ECGanalyzer::on_radioButtonChebyschevFilter_toggled(bool checked)
{
    if(checked)
    {
		enableChebyschevGUIControls(true);

		enableButterworthGUIControls(false);
		enableMovingAverageGUIControls(false);


        //TODO: ustaw algorytm w ECGBaseline oraz przełącz zakładkę na GUI
    }
}


void ECGanalyzer::on_checkBoxRPeaksDetectThresholdAutomatically_toggled(bool checked)
{
	ui.doubleSpinBoxRPeaksThreshold->setEnabled(!checked);
}

void ECGanalyzer::on_actionPrzeprowadzPonownieAnalizeSygnalu_triggered()
{
	using namespace std::placeholders;
	updateRunButtons(true);
    setModulesParams();
	//TODO: Tu ma być procedura analizy sygnału
	_ECGcontroller.rerunAnalysis(std::bind(&ECGanalyzer::updateAnalysisStatus, this, _1),
		std::bind(&ECGanalyzer::updateRunButtons, this, false));

}

void ECGanalyzer::on_actionZatrzymajPonownaAnalizeSygnalu_triggered()
{
	updateRunButtons(false);
	ui.actionZatrzymajPonownaAnalizeSygnalu->setEnabled(false);
	ui.actionPrzeprowadzPonownieAnalizeSygnalu->setEnabled(true);

	_ECGcontroller.rerunAnalysis(NULL, NULL);
}

void ECGanalyzer::updateAnalysisStatus(std::string status)
{
	QMetaObject::invokeMethod(ui.statusBar,
		"showMessage",
		Qt::QueuedConnection,
		Q_ARG(QString, QString::fromStdString(status)));
}

void ECGanalyzer::updateRunButtons( bool analysisOngoing )
{
	QMetaObject::invokeMethod(ui.actionZatrzymajPonownaAnalizeSygnalu,         // obj
		"setEnabled", // member
		Qt::QueuedConnection,     // connection type
		Q_ARG(bool, analysisOngoing));     // val1
	//ui.actionZatrzymajPonownaAnalizeSygnalu->setEnabled(analysisOngoing);
	QMetaObject::invokeMethod(ui.actionPrzeprowadzPonownieAnalizeSygnalu,         // obj
		"setEnabled", // member
		Qt::QueuedConnection,     // connection type
		Q_ARG(bool, !analysisOngoing));     // val1
	//ui.actionPrzeprowadzPonownieAnalizeSygnalu->setEnabled(!analysisOngoing);
}

void ECGanalyzer::setModulesParams()
{
  //TODO: Set params for other modules
  setBaselineParams();
  setRpeaksParams();
  setSTIntervalParams();
}


void ECGanalyzer::updateSTIntervalTab()
{
  const ECGST & st_data = _ECGcontroller.st_data;
  const ECGChannelInfo & info = _ECGcontroller.ecg_info.channel_two;
  std::vector<ECGST::Interval> intervals = st_data.getIntervals();
  std::vector<ECGST::Episode> episodes = st_data.getEpisodes();
  int i = 0;
  
  //Set Intervals table
  ui.st_intervals_table->clear();
  ui.st_intervals_table->setColumnCount(6);
  ui.st_intervals_table->setRowCount(intervals.size());  
  ui.st_intervals_table->setHorizontalHeaderItem(0, new QTableWidgetItem( "ST onset" ));
  ui.st_intervals_table->setHorizontalHeaderItem(1, new QTableWidgetItem( "ST end" ));
  ui.st_intervals_table->setHorizontalHeaderItem(2, new QTableWidgetItem( "Interval length [ms]" ));
  ui.st_intervals_table->setHorizontalHeaderItem(3, new QTableWidgetItem( "Slope" ));
  ui.st_intervals_table->setHorizontalHeaderItem(4, new QTableWidgetItem( "Offset" ));
  ui.st_intervals_table->setHorizontalHeaderItem(5, new QTableWidgetItem( "Description" ));
  
  i = 0;
  BOOST_FOREACH(const ECGST::Interval &it, intervals) {
    ui.st_intervals_table->setItem(i,0, new QTableWidgetItem( info.sampleToTime( it.jpoint ).c_str() ));
    ui.st_intervals_table->setItem(i,1, new QTableWidgetItem( info.sampleToTime( it.stpoint ).c_str() ) );
    ui.st_intervals_table->setItem(i,2, new QTableWidgetItem( QString::number(it.length()) ));
    ui.st_intervals_table->setItem(i,3, new QTableWidgetItem( QString::number(it.slope) ));
    ui.st_intervals_table->setItem(i,4, new QTableWidgetItem( QString::number(it.offset) ));
    ui.st_intervals_table->setItem(i,5, new QTableWidgetItem( it.description.c_str() ));
    ++i;
  }
  
  //Set Episodes table
  ui.st_episodes_table->clear();
  ui.st_episodes_table->setColumnCount(2);
  ui.st_episodes_table->setRowCount(episodes.size());
  ui.st_episodes_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Episode begin") );
  ui.st_episodes_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Episode end") );
  
  i = 0;
  BOOST_FOREACH(const ECGST::Episode &it, episodes) {
    ui.st_episodes_table->setItem(i,0, new QTableWidgetItem( info.sampleToTime( it.start ).c_str() ));
    ui.st_episodes_table->setItem(i,1, new QTableWidgetItem( info.sampleToTime(  it.end ).c_str() ));
    ++i;
  }
  
  ui.st_plot->setSignal(_ECGcontroller.filtered_signal, _ECGcontroller.ecg_info.channel_one, _ECGcontroller.st_data);
  ui.st_plot->showInterval(_ECGcontroller.st_data.getIntervalAt(0));
  
}

void ECGanalyzer::setBaselineParams()
{
	ParametersTypes params;
	if(ui.radioButtonMovingAverage->isChecked())
	{
		params["baseline_removal_method"] = MOVING_AVERAGE;
		params["span"] = ui.spinBoxMovingAverageSpan->value();
	}
	else if(ui.radioButtonButterworthFilter->isChecked())
	{
		params["baseline_removal_method"] = BUTTERWORTH;
		params["order"] = ui.spinBoxOrderButterworth->value();
		params["cutoff_frequency"] = ui.spinBoxCutOffFrequencyButterworth->value();
		params["attenuation"] = ui.doubleSpinBoxAttenuationButterworth->value();
	}
	else if(ui.radioButtonChebyschevFilter->isChecked())
	{
		params["baseline_removal_method"] = CHEBYSHEV;
		params["order"] = ui.spinBoxOrderChebyshev->value();
		params["cutoff_frequency"] = ui.spinBoxCutOffFrequencyChebyshev->value();
		params["ripple"] = ui.doubleSpinBoxRippleChebyschev->value();
	}

	_ECGcontroller.setParamsBaseline(params);
}

void ECGanalyzer::setRpeaksParams()
{
	ParametersTypes params;
	params["detection_method"] = ui.comboBoxRPeaksDetectionMethod->currentIndex();
	if(ui.comboBoxRPeaksDetectionMethod->currentIndex() == 0)
	{
		if(!ui.checkBoxRPeaksDetectThresholdAutomatically->isChecked())
		{
			params["threshold_size"] = ui.doubleSpinBoxRPeaksThreshold->value();
		}
		if(!ui.checkBoxRPeaksDetectWindowSizeAutomatically->isChecked())
		{
			params["window_size"] = ui.doubleSpinBoxRPeaksWindowSize->value();
		}
	}
	if(ui.comboBoxRPeaksDetectionMethod->currentIndex() == 1)
	{
		//TODO Hilbert parameters
	}
	_ECGcontroller.setParamsRPeaks(params);
}

void ECGanalyzer::setSTIntervalParams()
{
  ParametersTypes st_params;
  st_params["algorithm"] = (double) ui.st_select_algorithm->currentIndex();
  st_params["simple_thresh"] = ui.st_simple_thresh->value();
  st_params["complex_thresh"] = ui.st_complex_thresh->value();
  st_params["type_thresh"] = ui.st_complex_other->value();
  st_params["slope_thresh"] = ui.st_complex_slope->value();
  _ECGcontroller.setParamsSTInterval(st_params);
}






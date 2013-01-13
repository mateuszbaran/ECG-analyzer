#include "ecganalyzer.h"
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <QThread>
#include <functional>

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
   // QMessageBox::about(this, tr("About ECG Analyzer"),
   //         tr("<h2><div align=\"center\">ECG Analyzer</div></h2>"
			//"<br/><b>Moduły:</b>"
			//"<ul><li>ECG Baseline: Weronika Łabaj, Piotr Matuszkiewicz"
			//"R Peaks: Paweł Maślanka, Norbert Pabian<ul>"
			//"<li>Waves: Agata Sitnik, Łukasz Zieńkowski<ul>"
			//"<li>QRS class: Krzysztof Bębenek, Aleksander Steliga"
			//"<ul><li>HRT: Łukasz Kutrzuba, Mateusz Krasucki</li></ul>"
			//"</li>"
			//"<li>ST interval: Krzysiek Piekutowski, Bartłomiej Bułat</li>"
			//"<li>T Wave alt: Grzegorz Pietrzyk, Łukasz Krzyżek</li>"
			//"</ul>"
			//"</li>"
			//"<li>HRV 1: Łukasz Jaromi, Leszek Sosnowski</li>"
			//"<li>HRV 2: Krzysztof Farganus</li>"
			//"<li>HRV DFA: Mikołaj Rzepka, Szczepan Czaicki</li>"
			//"</ul>"
			//"</li></ul>"
			////"</li></ul>"
			//"<br/><div align=\"center\"><p>Wydział Elektrotechniki, Automatyki, "
   //         "Informatyki i Inżynierii Biomedycznej</p>"
   //         "<p>2012/2013 AGH Kraków</p></div>"));
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
		
		
		ECGHRT hrt_data;
		hrt_data.offset = 3;
		hrt_data.rr.push_back(QPointF(0.0, 2.0));
		hrt_data.rr.push_back(QPointF(1.0, 0.0));
		hrt_data.rr.push_back(QPointF(2.0, 5.0));
		hrt_data.rr.push_back(QPointF(3.0, 3.0));
		hrt_data.rr.push_back(QPointF(4.0, 10.0));
		hrt_data.rr.push_back(QPointF(5.0, 4.0));
		hrt_data.rr.push_back(QPointF(6.0, 9.0));
		hrt_data.rr.push_back(QPointF(7.0, 7.0));
		hrt_data.ts.setLine(2.0, 3.0, 5.0, 16.0);
		QVBoxLayout *plotHARTLayout = new QVBoxLayout;
		PlotHRT *plotHRT = new PlotHRT(this);
		plotHARTLayout->addWidget(plotHRT);
		plotHRT->setData(hrt_data);
		ui.groupBox->setLayout(plotHARTLayout);
		
		
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
  double dt = 1.0f / ( (double) _ECGcontroller.ecg_info.channel_one.frequecy);
  auto _section = _ECGcontroller.st_data.getIntervalBeginAndEnd(row);
  ui.st_plot->zoomX(_section.first, _section.second);
  
  auto interval = _ECGcontroller.st_data.getIntervals().at(row);
  ui.st_plot_info->clear();
  ui.st_plot_info->setRowCount(2);
  ui.st_plot_info->setColumnCount(1);
  ui.st_plot_info->setItem(0,0, new QTableWidgetItem(  QString::number(( (double) interval.jpoint*dt), 'f', 2) + " s" ) );
  ui.st_plot_info->setItem(1,0, new QTableWidgetItem(  QString::number(( (double) interval.stpoint*dt), 'f', 2) + " s" ) );
  ui.st_plot_info->setVerticalHeaderItem(0, new QTableWidgetItem( "ST onset" ));
  ui.st_plot_info->setVerticalHeaderItem(1, new QTableWidgetItem( "ST end" ));
}

void ECGanalyzer::on_st_episodes_table_cellDoubleClicked(int row, int col)
{
  auto _section = _ECGcontroller.st_data.getEpisodeBeginAndEnd(row);
  ui.st_plot->zoomX(_section.first, _section.second);
  ui.st_plot_info->clear();

}



void ECGanalyzer::on_actionWyjdz_triggered()
{
    this->close();
}

void ECGanalyzer::on_radioButtonMovingAverage_toggled(bool checked)
{
    if(checked)
    {
        //TODO: ustaw algorytm w ECGBaseline oraz przełącz zakładkę na GUI
    }
}

void ECGanalyzer::on_radioButtonButterworthFilter_toggled(bool checked)
{
    if(checked)
    {
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
	//TODO
	ui.statusBar->setStatusTip(QString("%s,status"));
}

void ECGanalyzer::updateRunButtons( bool analysisOngoing )
{
	ui.actionZatrzymajPonownaAnalizeSygnalu->setEnabled(analysisOngoing);
	ui.actionPrzeprowadzPonownieAnalizeSygnalu->setEnabled(!analysisOngoing);
}

void ECGanalyzer::setModulesParams()
{
  //TODO: Set params for other modules
  setSTIntervalParams();
}


void ECGanalyzer::updateSTIntervalTab()
{
  double dt = 1.0f / ( (double) _ECGcontroller.ecg_info.channel_one.frequecy);
  const ECGST & st_data = _ECGcontroller.st_data;
  
  std::vector<ECGST::Interval> intervals = st_data.getIntervals();
  ui.st_intervals_table->clear();
  ui.st_intervals_table->setColumnCount(4);
  ui.st_intervals_table->setRowCount(intervals.size());
  
  ui.st_intervals_table->setHorizontalHeaderItem(0, new QTableWidgetItem( "ST onset" ));
  ui.st_intervals_table->setHorizontalHeaderItem(1, new QTableWidgetItem( "ST end" ));
  ui.st_intervals_table->setHorizontalHeaderItem(2, new QTableWidgetItem( "Slope" ));
  ui.st_intervals_table->setHorizontalHeaderItem(3, new QTableWidgetItem( "Offset" ));
  
  auto it = intervals.begin();
  int i =0;
  for(; it != intervals.end(); ++it, ++i) {
    ui.st_intervals_table->setItem(i,0, new QTableWidgetItem( QString::number(( (double) it->jpoint*dt), 'f', 2) + " s" ) );
    ui.st_intervals_table->setItem(i,1, new QTableWidgetItem( QString::number(( (double) it->stpoint*dt), 'f', 2) + " s" ) );
    ui.st_intervals_table->setItem(i,2, new QTableWidgetItem( QString::number(it->slope) ));
    ui.st_intervals_table->setItem(i,3, new QTableWidgetItem( QString::number(it->offset) ));
  }
  
  auto interval = intervals.at(0);
  ui.st_plot_info->clear();
  ui.st_plot_info->setRowCount(2);
  ui.st_plot_info->setColumnCount(1);
  ui.st_plot_info->setItem(0,0, new QTableWidgetItem(  QString::number(( (double) interval.jpoint*dt), 'f', 2) + " s" ) );
  ui.st_plot_info->setItem(1,0, new QTableWidgetItem(  QString::number(( (double) interval.stpoint*dt), 'f', 2) + " s" ) );
  ui.st_plot_info->setVerticalHeaderItem(0, new QTableWidgetItem( "ST onset" ));
  ui.st_plot_info->setVerticalHeaderItem(1, new QTableWidgetItem( "ST end" ));
  
  
  std::vector<ECGST::Episode> episodes = st_data.getEpisodes();
  ui.st_episodes_table->clear();
  ui.st_episodes_table->setColumnCount(2);
  ui.st_episodes_table->setRowCount(episodes.size());
  ui.st_episodes_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Episode begin") );
  ui.st_episodes_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Episode end") );
  
  auto itt = episodes.begin();
  i = 0;
  for(; itt != episodes.end(); ++itt, ++i) {
    ui.st_episodes_table->setItem(i,0, new QTableWidgetItem( QString::number(( (double) itt->start)*dt, 'f', 2) + " s" ));
    ui.st_episodes_table->setItem(i,1, new QTableWidgetItem( QString::number( ( (double) itt->end)*dt, 'f', 2) + " s" ));
  }
  
  ui.st_plot->setSignal(_ECGcontroller.raw_signal.channel_one, _ECGcontroller.ecg_info.channel_one, _ECGcontroller.st_data);
  
  auto _section = st_data.getIntervalBeginAndEnd(0);
  ui.st_plot->zoomX(_section.first, _section.second);
  
}

void ECGanalyzer::setSTIntervalParams()
{
  ParametersTypes st_params;
  st_params["algorithm"] = (double) ui.st_select_algorithm->currentIndex();
  st_params["simple_thresh"] = ui.st_simple_thresh->value();
  st_params["complex_thresh"] = ui.st_complex_thresh->value();
  st_params["complex_other"] = ui.st_complex_other->value();
  _ECGcontroller.setParamsSTInterval(st_params);
}



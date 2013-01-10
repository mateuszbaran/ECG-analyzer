#include "ecganalyzer.h"
#include <qmessagebox.h>
#include <qtextcodec.h>

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
        _ECGcontroller.readFile(fileName.toStdString()); // why?
        _ECGcontroller.runECGBaseline();
        ui.rawPlotWidget->setSignal(&(_ECGcontroller.raw_signal), &(_ECGcontroller.ecg_info), &(_ECGcontroller.r_peaks_data));
		//_ECGcontroller.runRPeaks();


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
		gain->setText(QString().sprintf("%d", _ECGcontroller.ecg_info.channel_one.gain) );
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
		/*QVBoxLayout *plotHARTLayout = new QVBoxLayout;
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
		*/
		
		ui.actionPrzeprowadzPonownieAnalizeSygnalu->setEnabled(true);
		
    }
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

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
    QMessageBox::about(this, tr("About ECG Analyzer"),
            tr("<h2>ECG Analyzer</h2>"
               "<p>Wydział Elektrotechniki, Automatyki"
               "Informatyki i Inżynierii Biomedycznej</p>"
               "<p>2012/2013 AGH Kraków</p>"));
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
        ui.rawPlotWidget->setSignal(&(_ECGcontroller.raw_signal));
        ui.rawPlotWidget->redraw();



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

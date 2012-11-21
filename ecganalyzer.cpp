#include "ecganalyzer.h"
#include <qmessagebox.h>
#include <qtextcodec.h>

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
               "<p>Wydzia³ Elektrotechniki, Automatyki"
               "Informatyki i In¿ynierii Biomedycznej</p>"
               "<p>2012/2013 AGH Kraków</p>"));
}

void ECGanalyzer::on_actionWyjd_triggered()
{
    this->close();
}

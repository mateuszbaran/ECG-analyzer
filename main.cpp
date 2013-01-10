#include "ecganalyzer.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	ECGController c;
	//c.runRPeaks();
	//c.runHRV2();
	QApplication a(argc, argv);
	ECGanalyzer w;
	w.show();
	return a.exec();
	//return 0;
}

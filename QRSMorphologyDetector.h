#pragma once

#include "ModulesInterfaces.h"
#include "ModulesMethods.h"
#include <fstream>
#include <sstream>

#include <gsl/gsl_sort_vector.h>

using namespace std;


class QRSMorphologyDetector : public QRSClassModule
{
public:
	QRSMorphologyDetector(void);
	~QRSMorphologyDetector(void);
	
	void setParams(ParametersTypes &parameterTypes);
	void runModule (const ECGWaves &, const ECGSignalChannel &, const ECGInfo &, QRSClass &);

private:

	ECGSignalChannel filteredSignal;

	ECGWaves qrsPosition;

	QRSClass * qrsMorphology;

	bool detectQRSMorphology();
	friend double pole(ECGSignalChannel * signal, int forBegin, int forEnd);
	friend double dlugosc(ECGSignalChannel * signal, int forBegin, int forEnd);
	friend double stosunek(ECGSignalChannel * signal, int forBegin, int forEnd);
};
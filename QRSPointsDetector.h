#pragma once

#include "ModulesInterfaces.h"
#include "ModulesMethods.h"
#include <fstream>
#include <sstream>

#include <gsl/gsl_sort_vector.h>

//#define DEBUG
//#define USE_MOCKED_SIGNAL

using namespace std;

class QRSPointsDetector : public QRSClassModule
{
public:
	QRSPointsDetector(void);
	~QRSPointsDetector(void);

	void setParams(ParametersTypes &parameterTypes);
	void runModule (ECGWaves &, const ECGSignalChannel &, const ECGRs &, const ECGInfo &, ECGClasses &);

	ECGSignalChannel getMockedSignal();
    ECGRs getMockedRPeak();


private:

  ECGSignalChannel filteredSignal;

  ECGRs  rsPositions;

  ECGWaves *  qrsPoints;

  ECGSignalChannel mockedSignal;

  bool detectQRS();
  bool detectPT();

  ECGSignalChannel gradient(ECGSignalChannel *signal);
  ECGSignalChannel averageFilter(ECGSignalChannel *signal);
  friend double findMinimum (ECGSignalChannel *signal,int forBegin, int forEnd);
  friend double findMaximum (ECGSignalChannel *signal,int forBegin, int forEnd);

};


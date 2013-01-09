#pragma once

#include "ModulesInterfaces.h"

/**
 * @class Class for parameters created in HRV1 module
 */
class HRV1Analyzer : public HRV1Module {

public:
	HRV1Analyzer();
	~HRV1Analyzer();

	void runModule(const ECGRs &, ECGHRV1 &);
	void setParams(ParametersTypes &);

private:
	ECGRs rPeaksData;
	ECGHRV1 hrv1Data;
	void run();

};


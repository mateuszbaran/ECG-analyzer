#pragma once
#include "ModulesInterfaces.h"

class BaselineRemoval : ECGBaselineModule 
{
public:
	BaselineRemoval();
	~BaselineRemoval();

	void runModule(const ECGSignal &, ECGSignal &);
 	void setParams(ParametersTypes &);
};

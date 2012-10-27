#pragma once

#include <vector\gsl_vector.h>

class ECGSignal
{
public:
	ECGSignal(void);
	~ECGSignal(void);
private:
	gsl_vector singal;
};


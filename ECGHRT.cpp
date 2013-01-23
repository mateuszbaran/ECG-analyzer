#include "ECGHRT.h"

ECGHRT::ECGHRT (void) : rr(), ts(), SLENGTH(26),ALL_SIGNALS_LENGTH(1),x1_to(7) { 
	straightSignal = new double[SLENGTH];
	avgSignal = new double[SLENGTH];
	for(int j = 0; j < SLENGTH; j++)
	{
		avgSignal[j] = 0.0;
		straightSignal[j] = 0.0;
	}

}

ECGHRT::~ECGHRT (void) { }

void ECGHRT::setAllSignalsSize(int size)
{
	ALL_SIGNALS_LENGTH = size;

	allSignals = new double*[ALL_SIGNALS_LENGTH];
	for(int i = 0; i < ALL_SIGNALS_LENGTH; i++)
	{
		allSignals[i] = new double[SLENGTH];
		for(int j = 0; j < SLENGTH; j++)
		{
			allSignals[i][j]=0.0;
		}
	}
}

double ECGHRT::getMaxValue()
{
	double maxx = allSignals[0][0];
	for(int i = 0; i < ALL_SIGNALS_LENGTH; i++)
	{
		for(int j = 0; j < SLENGTH; j++)
		{
			if(allSignals[i][j] > maxx) maxx= allSignals[i][j];
		}
	}
	return maxx;
}

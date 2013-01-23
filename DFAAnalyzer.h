#pragma once
#include "ModulesInterfaces.h"

struct trend_coefs
{ 
	public:
		double a;
		double b;
};

class DFAAnalyzer : public HRVDFAModule
{
public:
	DFAAnalyzer();
	~DFAAnalyzer();
	void runModule (const ECGRs &, ECGHRVDFA &);
	void setParams(ParametersTypes &);

private:
	ECGRs rsPositions;
	ECGHRVDFA ecghrvdfa;
	void calcDFA();
	trend_coefs* calc_trend(double*, int, int);
	trend_coefs** calc_all_trends(double*, int);
	trend_coefs* calc_trends_for_window_length(double* rr_integrated, int length, int window_length);
};


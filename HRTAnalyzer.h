#pragma once
//#define USE_MOCKED_INTERVALS_SIGNAL
#define USE_MOCKED_QRS_CLASSIFICATION
#include "ModulesInterfaces.h"
#include "ECGHRT.h"
#include "ECGSignal.h"
#include "QRSClass.h"
#include "matrix/Matrix.h"
using namespace std;

// testowe
static double RRtest[] = {800,1425, 810, 800,1050, 800, 800, 800, 800, 800, 610, 960, 775, 777, 780, 785, 795, 797,790, 790, 795, 800, 810, 800, 810, 800, 810, 800, 810, 800, 800, 800, 800, 
	600, 950, 820, 780, 782, 810, 840, 870, 900, 790, 790, 792, 795, 795, 797, 800, 800, 805, 800, 800, 805, 800, 800, 800, 600, 685, 800, 825, 800, 825,
    800, 825, 825, 825, 800, 800, 800, 800, 605, 930, 825, 788, 775, 795, 800, 800, 800, 800, 800, 805, 805, 805, 800, 809, 800, 805, 800, 805, 800, 805, 800, 825, 800,
    825, 800, 825, 880, 825, 800, 825, 800, 825, 800, 825, 1000, 800, 1025, 800, 800, 1050, 800, 800, 800, 800, 800, 530, 1100,  775, 777,780, 785, 795, 797,790, 790, 795, 
	800, 810, 800, 810, 800, 810, 800, 810, 800, 800, 800, 805, 800, 805, 800, 805, 800, 800, 805, 795, 805, 805, 825, 800, 800, 800, 800, 600, 950, 800, 865, 800, 825, 800, 
	825, 299, 800, 825, 800, 825, 800, 825, 850, 800, 825, 800, 800, 825, 610, 695, 775, 777, 780, 785, 795, 797, 790, 790, 795, 800, 810, 800, 810, 800, 810, 800, 810, 800, 800, 
	800, 805, 800, 805, 800, 805, 800, 805, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 390, 825, 800, 825, 800, 825, 800, 825, 900, 800, 925, 800, 800, 1050,
    800, 600, 950, 800, 825, 800, 825, 800, 825, 850, 800, 825, 800, 825, 800, 825, 850, 800, 825, 850, 825, 875, 825, 800, 600, 970, 775, 777, 780, 785, 795, 797, 790, 
	790, 795, 800, 810, 800, 810, 800, 810, 800, 810, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 825, 800, 825, 850, 800, 825, 800, 825, 800, 825, 850, 800, 825, 800, 800, 
	825, 600, 585, 800, 825, 800, 825, 800, 825, 825, 825, 650, 1000,782, 779, 777, 780, 785, 795, 797, 790, 790, 795, 860, 810, 800, 810, 800, 820, 800, 810, 800, 800, 800, 800, 808, 
	809, 810, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 800, 825, 390, 825, 800, 825, 800, 
	825, 800, 700, 700, 700, 709, 700, 700, 825, 810, 810, 600, 800};


/**
 * @class Class for parameters created in HRV1 module
 */
class HRTAnalyzer : public HRTModule {

public:
	HRTAnalyzer();
	~HRTAnalyzer();
	void runModule(const ECGRs &, const QRSClass &,  const ECGInfo &, ECGHRT &);
	void run(ECGHRT &);
	void setParams(ParametersTypes &);

private:
	double* RRs;

	void calculateHrtParams(double *signal,  const QRSClass & qrsclass, int size, ECGHRT &);
	
	vector<int> findVpcOnsets(double *signal,  const QRSClass & qrsclass, int size);
	double* calculateAvgTach(double *signal, vector<int> vpc_list);

	// wyliczenie TO
	// Wzi¹³em nadal tê metodê, w której najpierw liczy siê wszystkie TO, a nastêpnie ich œredni¹
	double calculateTO(double * signal, int size, vector<int> vpc_list);

	//alternatywna metoda wylicznia TO (na uœrednionym tachogramie) 
	//double calculateTO_2(double * signal, int size, double* avgTach);

	void calculateTS(double * signal, int size, vector<int> vpc_list, double* avgTach, double to, ECGHRT&);

};


#include "ECGHRV1.h"

ECGHRV1::ECGHRV1() { }

ECGHRV1::~ECGHRV1() { }

void ECGHRV1::runModule(const ECGRs & r_peaks_data, ECGHRV1 & hrv1_data)
{
	this->run(r_peaks_data);
}

void ECGHRV1::run(const ECGRs & r_peaks_data) {

	gsl_vector_int* rs = r_peaks_data.GetRs();
	int sigSize = rs->size-1;
	double *rr_signal = new double[sigSize-1];
	for(int i = 0; i < sigSize; i++)
	{
		rr_signal[i] = (gsl_vector_int_get(rs, i+1) - gsl_vector_int_get(rs, i))/360.0;
		std::cout << rr_signal[i];
	}

}

void ECGHRV1::setParams(ParametersTypes &parameterTypes) { }

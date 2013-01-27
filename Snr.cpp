#include "Snr.h"
#include <math.h>

double Snr::snr(ECGSignalChannel &filtered_signal, ECGSignalChannel & raw_signal)
{
	double snr = 0;

	ECGSignalChannel noise = ECGSignalChannel(new WrappedVector);
	noise->signal = gsl_vector_alloc(filtered_signal->signal->size);

	this->extract_noise(filtered_signal, raw_signal, noise);

	snr = 20*log10(this->rms(filtered_signal)/this->rms(noise));
    return snr;
}

double Snr::psnr(ECGSignalChannel &filtered_signal, ECGSignalChannel & raw_signal)
{
	double k = gsl_vector_max(raw_signal->signal);
	double mse = this->mse(filtered_signal, raw_signal);
	double psnr = 10*log10(pow(k,2)/mse);
    return psnr;
}

/**
 * Mean-square-error calculation for PSNR
 */
double Snr::mse(ECGSignalChannel &filtered_signal, ECGSignalChannel & raw_signal)
{
	long n = filtered_signal->signal->size;
	double mse = 0;
	double f = 0;
	double f_prim = 0;

	for(int i = 0; i<n; i++)
	{
		f = gsl_vector_get(raw_signal->signal, i);
		f_prim = gsl_vector_get(filtered_signal->signal, i);

		mse += pow((f-f_prim),2);
	}

	return mse;
}

/**
 * Root-mean-square calculation for snr
 */
double Snr::rms(ECGSignalChannel &signal)
{
	double rms = 0;
	double sum = 0;
	double x;
	long n = signal->signal->size;

	for(int i=0; i<n; i++)
	{
		x = gsl_vector_get(signal->signal, i);
		sum += pow(x,2);
	}

	rms = sqrt(sum/n);

	return rms;
}

void Snr::extract_noise(ECGSignalChannel &filtered_signal, ECGSignalChannel & raw_signal, ECGSignalChannel & noise)
{
	int size = filtered_signal->signal->size;
	double raw, filtered;

	for(int i=0; i<size; i++)
	{
		raw = gsl_vector_get(raw_signal->signal, i);
		filtered = gsl_vector_get(filtered_signal->signal, i);

		gsl_vector_set(noise->signal, i, raw-filtered);
	}
}
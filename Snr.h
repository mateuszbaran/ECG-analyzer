#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class Snr
{
public:
	Snr(){};
	~Snr(){};

	double snr(ECGSignalChannel &filtered_signal, ECGSignalChannel & raw_signal);
	double psnr(ECGSignalChannel &filtered_signal, ECGSignalChannel & raw_signal);

private:

	double mse(ECGSignalChannel &filtered_signal, ECGSignalChannel & raw_signal);
	double rms(ECGSignalChannel &signal);
	void extract_noise(ECGSignalChannel &filtered_signal, ECGSignalChannel & raw_signal, ECGSignalChannel & noise);
};
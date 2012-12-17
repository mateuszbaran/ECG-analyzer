#include "HRV1Analyzer.h"
#include "ECGSignal.h"

HRV1Analyzer::HRV1Analyzer() { }

HRV1Analyzer::~HRV1Analyzer() { }

void HRV1Analyzer::runModule(const ECGRs & r_peaks_data, ECGHRV1 & hrv1_data) {
	this->rPeaksData = r_peaks_data;
	this->hrv1Data = hrv1_data;
	//this->hrv1Data = new ECGHRV1();

	this->run();
}

void HRV1Analyzer::run() {

	gsl_vector_int* rs = (*rPeaksData.GetRs())->signal;
	int signalSize = rs->size-1;

	double *sig = new double[signalSize-1];
	for(int i = 0; i < signalSize; i++)
	{
		sig[i] = (gsl_vector_int_get(rs, i+1)
				- gsl_vector_int_get(rs, i))/360.0;
	}


	/////////////////////RR
	double temp = 0;

	for(int i=0; i<signalSize; i++) {
		temp = temp + sig[i];
	}

	this->hrv1Data.RR_avg = (double)1/signalSize*temp;


	//////////////////////SDNN
	temp =0;
	for(int i=0; i<signalSize; i++) {
		temp = temp + (this->hrv1Data.RR_avg-sig[i])
						*(this->hrv1Data.RR_avg-sig[i]);
	}

	this->hrv1Data.SDNN = sqrt((double)1/(signalSize-1)*temp);


	//////////////////////RMSSD
	temp=0;

	for(int i=0; i<signalSize; i++) {
		temp = temp + (sig[i+1]-sig[i])*(sig[i+1]-sig[i]);
	}

	this->hrv1Data.RMSSD = sqrt((double)1/(signalSize)*temp);


	//////////////////////NN50
	temp=0;

	for(int i=0; i<signalSize; i++) {
		if (abs(sig[i+1]-sig[i]) > 50)
			temp++;
	}

	this->hrv1Data.NN50 = temp;


	/////////////////////pNN50
	temp=0;

	this->hrv1Data.pNN50 = this->hrv1Data.NN50/signalSize * 100;


	/////////////////////SDANN
	//TODO


	///////////////////SDANNi
	//TODO


	///////////////////SDSD
	//FIXME
	temp = 0;
	for(int i=0; i<signalSize; i++) {
		temp = temp + (this->hrv1Data.RR_avg-(sig[i+1]-sig[i]))
								*(this->hrv1Data.RR_avg-(sig[i+1]-sig[i]));
	}

	this->hrv1Data.SDSD = sqrt((double)1/(signalSize)*temp);

}

void HRV1Analyzer::setParams(ParametersTypes &parameterTypes) { }

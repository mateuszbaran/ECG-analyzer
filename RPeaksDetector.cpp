#include "RPeaksDetector.h"


RPeaksDetector::RPeaksDetector()
{
	this->detectionMethod = PAN_TOMPKINS;
}


RPeaksDetector::~RPeaksDetector()
{}

void RPeaksDetector::runModule(const ECGSignal &filteredSignal, ECGRs &ecgRs)
{
	this->filteredSignal = filteredSignal;
	this->rsPositions = ecgRs;

	bool success = this->detectRPeaks();

	//TODO What should I do if module cannot executed?
	if(!success)
		return;
}

void RPeaksDetector::setParams(ParametersTypes &parameterTypes)
{
	//TODO Allow to set detection method here. If no parameters then use Tompkins method
}

bool RPeaksDetector::areRsDetected()
{
	return this->rsDetected;
}

void RPeaksDetector::setDetectionMethod(R_PEAKS_DETECTION_METHOD detectionMethod)
{
	this->detectionMethod = detectionMethod;
}

R_PEAKS_DETECTION_METHOD RPeaksDetector::getDetectionMethod()
{
	return this->detectionMethod;
}

bool RPeaksDetector::detectRPeaks()
{
	if(detectionMethod == PAN_TOMPKINS)
	{
		panTompkinsRPeaksDetection(&filteredSignal);
		return true;
	}
	else if (detectionMethod == HILBERT)
	{
		hilbertRPeaksDetection(&filteredSignal);
		return true;
	}
	else
		return false;
}

void RPeaksDetector::panTompkinsRPeaksDetection(ECGSignal *signal)
{
	//TODO Body of Tompkins method
	rsDetected = true;
}

void RPeaksDetector::hilbertRPeaksDetection(ECGSignal *signal)
{
	//TODO Body of Hilbert method
	rsDetected = true;
}
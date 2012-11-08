#include "ECGRs.h"

ECGRs::ECGRs ()
{
	rsDetected = false;
	detectionMethod = PAN_TOMPKINS;
}

ECGRs::ECGRs (IntSignal filteredSignal)
{
	rsDetected = false;
	filteredSignal = filteredSignal;
	detectionMethod = PAN_TOMPKINS;
	detectRPeaks();
}

ECGRs::ECGRs (IntSignal filteredSignal, R_PEAKS_DETECTION_METHOD detectionMethod)
{
	rsDetected = false;
	filteredSignal = filteredSignal;
	detectionMethod = detectionMethod;
	detectRPeaks();
}

ECGRs::~ECGRs (void)
{}

bool ECGRs::isDetected()
{
	return rsDetected;
}

void ECGRs::setFilteredSignal(IntSignal filteredSignal)
{  
	filteredSignal = filteredSignal;
	rsDetected = false;
}

IntSignal ECGRs::getFilteredSignal()
{
	return filteredSignal;
}

void ECGRs::setDetectionMethod(R_PEAKS_DETECTION_METHOD detectionMethod)
{
	detectionMethod = detectionMethod;
}

R_PEAKS_DETECTION_METHOD ECGRs::getDetectionMethod()
{
	return detectionMethod;
}

bool ECGRs::detectRPeaks()
{
	if(detectionMethod == PAN_TOMPKINS)
		panTompkinsRPeaksDetection(&filteredSignal);
	else if (detectionMethod == HILBERT)
		hilbertRPeaksDetection(&filteredSignal);
}

void ECGRs::panTompkinsRPeaksDetection(IntSignal *signal)
{
	rsDetected = true;
}

void ECGRs::hilbertRPeaksDetection(IntSignal *signal)
{
	rsDetected = true;
}
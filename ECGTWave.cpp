#include "ECGTWave.h"

ECGTWave::ECGTWave (void) { 
	numberOfWinDetected = 0;
	percentageOfWinDetected = 0;
}

ECGTWave::~ECGTWave (void) { }

void ECGTWave::setTWaveAlt(IntSignal twa)
{
	this->twa = twa;        
}
#pragma once

#include "ModulesInterfaces.h"


class TWaveAltDetector : public TWaveAltModule {

public:
	TWaveAltDetector(void);
	~TWaveAltDetector(void);

	void setParams(ParametersTypes &parameterTypes);
	void runModule (const ECGWaves &, const ECGSignalChannel &, ECGInfo &, ECGTWave &); 
	//void runModule(const ECGRs &, ECGHRV1 &);


	//detecting alternans
	bool detectTWaveAlt();
	//(ECGWaves &ecg_waves, ECGSignal &ecg_signal, ECGTWave &ecg_twave, ECGInfo &ecg_info)



private:
	ECGSignalChannel filteredSignal;
	ECGWaves wavesData;
	ECGTWave tWaveAltData;
	void run();

};

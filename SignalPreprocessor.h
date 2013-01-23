#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class SignalPreprocessor
{
public:
	SignalPreprocessor();
	~SignalPreprocessor();

	int evaluateSignalChannels(const ECGSignal &inputSignal);
	void normalize(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal);
};
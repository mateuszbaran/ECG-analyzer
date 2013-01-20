#pragma once
#include "ModulesMethods.h"
#include "ModulesInterfaces.h"

class SignalPreprocessor
{
public:
	SignalPreprocessor();
	~SignalPreprocessor();

	void evaluateSignalChannels(const ECGSignal &inputSignal, ECGSignalChannel &betterChannel);
	void normalize(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal);
};
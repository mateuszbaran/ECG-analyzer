#include "SignalPreprocessor.h"

SignalPreprocessor::SignalPreprocessor(){}
SignalPreprocessor::~SignalPreprocessor(){}

void SignalPreprocessor::evaluateSignalChannels(const ECGSignal &inputSignal, ECGSignalChannel &betterChannel)
{
	//This method will evaluate which channel is better
	//For the time being it just returns first channel
	auto size = inputSignal.channel_one->signal->size;
	betterChannel->signal = gsl_vector_alloc(size);
	for(int i=0; i<size; i++)
	{
		auto value = gsl_vector_get(inputSignal.channel_one->signal, i);
		gsl_vector_set(betterChannel->signal, i, value);
	}
}

void SignalPreprocessor::normalize(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal)
{
	double input_min = 0;
	double input_max = 0;
	double output_min = 0;
	double output_max = 1;
	
	auto size = inputSignal->signal->size;

	for(int i=0; i<size; i++)
	{
		auto current_value = gsl_vector_get(inputSignal->signal, i);

		if(current_value > input_max)
		{
			input_max = current_value;
		}

		if(current_value < input_min)
		{
			input_min = current_value;
		}
	}

	double normalized_value;

	for(int j=0; j<size; j++)
	{
		auto current_value = gsl_vector_get(inputSignal->signal, j);
		normalized_value = (current_value-input_min) / ((input_max-input_min)*(output_max-output_min)) + output_min;
		gsl_vector_set(outputSignal->signal, j, normalized_value);
	}
}
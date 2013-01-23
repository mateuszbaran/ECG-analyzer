#include "SignalPreprocessor.h"
#include "MovingAverage.h"
#include "Snr.h"

SignalPreprocessor::SignalPreprocessor(){}
SignalPreprocessor::~SignalPreprocessor(){}

int SignalPreprocessor::evaluateSignalChannels(const ECGSignal &inputSignal)
{
	//This method will evaluate which channel is better
	//For the time being it just returns first channel
	//auto size = inputSignal.channel_one->signal->size;
	//betterChannel->signal = gsl_vector_alloc(size);
	//for(int i=0; i<size; i++)
	//{
	//	auto value = gsl_vector_get(inputSignal.channel_one->signal, i);
	//	gsl_vector_set(betterChannel->signal, i, value);
	//}

	ECGSignalChannel channel_one_sample = ECGSignalChannel(new WrappedVector);
	ECGSignalChannel filtered_channel_one_sample = ECGSignalChannel(new WrappedVector);
	ECGSignalChannel channel_two_sample = ECGSignalChannel(new WrappedVector);
	ECGSignalChannel filtered_channel_two_sample = ECGSignalChannel(new WrappedVector);
	channel_one_sample->signal = gsl_vector_alloc(1000);
	filtered_channel_one_sample->signal = gsl_vector_alloc(1000);
	channel_two_sample->signal = gsl_vector_alloc(1000);
	filtered_channel_two_sample->signal = gsl_vector_alloc(1000);

	double ch_one, ch_two;

	for(int i=0; i<1000; i++)
	{
		ch_one = gsl_vector_get(inputSignal.channel_one->signal,i);
		ch_two = gsl_vector_get(inputSignal.channel_two->signal,i);
		gsl_vector_set(channel_one_sample->signal,i,ch_one);
		gsl_vector_set(channel_two_sample->signal,i,ch_two);
	}

	MovingAverage *ma = new MovingAverage();
	ma->removeBaseline(channel_one_sample, filtered_channel_one_sample, 5);
	ma->removeBaseline(channel_two_sample, filtered_channel_two_sample, 5);

	Snr * snr = new Snr();
	double snr_ch_one = snr->snr(filtered_channel_one_sample, channel_one_sample);
	double snr_ch_two = snr->snr(filtered_channel_two_sample, channel_two_sample);

	if(snr_ch_one>snr_ch_two)
		return 1;
	else
		return 2;
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
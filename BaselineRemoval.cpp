#include "BaselineRemoval.h"
#include "Butterworth.h"
#include "Filter.h"

BaselineRemoval::BaselineRemoval(){}

BaselineRemoval::~BaselineRemoval(){}

void BaselineRemoval::runModule(const ECGSignal &inputSignal, const ECGInfo & ecgi, ECGSignalChannel &outputSignal)
{
	
	switch(baselineRemovalMethod){
		case MOVING_AVERAGE:
			movingAverageBaselineRemoval(inputSignal, outputSignal, 5);
			break;
		case BUTTERWORTH_2D:
			butterworthBaselineRemoval(inputSignal, outputSignal, ecgi, 5);
			break;
	}


	if(baselineRemovalMethod == MOVING_AVERAGE)
	{
		movingAverageBaselineRemoval(inputSignal, outputSignal, 5);
	}
}

/**
 Sets selected algorithm for baseline removal.
*/
void BaselineRemoval::setParams(ParametersTypes &params)
{
	this -> baselineRemovalMethod = MOVING_AVERAGE;
}

void BaselineRemoval::evaluateSignalChannels(const ECGSignal &inputSignal, ECGSignalChannel &betterChannel)
{
	//This method will evaluate which channel is better
	//For the time being it just returns first channel
	betterChannel->signal = inputSignal.channel_one->signal;
}

/**
	Moving Average algorithm - we assume that 4 neighbours are considered 
	while calculating average value for any given point,
	(--->>> it can be parametrized and number of neighbours might be passed from GUI)
	for edge points the output values are copies of the corresponding input values
*/
void BaselineRemoval::movingAverageBaselineRemoval(const ECGSignal &inputSignal, ECGSignalChannel &outputSignal, int span)
{
	ECGSignalChannel betterChannel;
	evaluateSignalChannels(inputSignal, betterChannel);

	auto signalLength = outputSignal-> signal -> size;
	for(int index = 0; index<signalLength; index++)
	{
		if(index < span/2 || index > signalLength - span/2) 
		{
			auto inputValue = gsl_vector_get (betterChannel -> signal, index);
			gsl_vector_set(outputSignal -> signal, index, inputValue);
		}
		else
		{
			auto avgValue = calculateAvgValueOfNeighbours(betterChannel -> signal, index, span);		
			gsl_vector_set (outputSignal -> signal, index, avgValue);
		}
	}
}

double BaselineRemoval::calculateAvgValueOfNeighbours(gsl_vector *signal, int currentIndex, int span)
{
	double sum = 0.00;
	for(int i = 1; i <= span/2; i++)
	{
		sum += gsl_vector_get (signal, currentIndex - i);
		sum += gsl_vector_get (signal, currentIndex + i);
	}
	return sum/span;
}

void BaselineRemoval::butterworthBaselineRemoval(const ECGSignal &inputSignal, ECGSignalChannel &outputSignal, const ECGInfo &ecgInfo, int cutoffFreq)
{
	ECGSignalChannel betterChannel;
	evaluateSignalChannels(inputSignal, betterChannel);

	int sampleFreq = ecgInfo.channel_one.frequecy;
	Butterworth * butterworth = new Butterworth();
	std::vector<std::vector<double>> bcCoefficients = butterworth->filterDesign(2, cutoffFreq, sampleFreq, 0);
	
	Filter * filter = new Filter();
	filter->zeroPhase(bcCoefficients[0], bcCoefficients[1], betterChannel, outputSignal, 2);
}
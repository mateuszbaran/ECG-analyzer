#include "BaselineRemoval.h"

BaselineRemoval::BaselineRemoval(){}

BaselineRemoval::~BaselineRemoval(){}

void BaselineRemoval::runModule(const ECGSignal &inputSignal, const ECGInfo & ecgi, ECGSignal &outputSignal)
{
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

/**
	Moving Average algorithm - we assume that 4 neighbours are considered 
	while calculating average value for any given point,
	(--->>> it can be parametrized and number of neighbours might be passed from GUI)
	for edge points the output values are copies of the corresponding input values
*/
void BaselineRemoval::movingAverageBaselineRemoval(const ECGSignal &inputSignal, ECGSignal &outputSignal, int span)
{
	auto signalLength = outputSignal.channel_one -> signal -> size;
	for(int index = 0; index<signalLength; index++)
	{
		if(index < span/2 || index > signalLength - span/2) 
		{
			auto inputValueChannelOne = gsl_vector_get (inputSignal.channel_one -> signal, index);
			auto inputValueChannelTwo = gsl_vector_get (inputSignal.channel_two -> signal, index);

			gsl_vector_set(outputSignal.channel_one -> signal, index, inputValueChannelOne);
			gsl_vector_set(outputSignal.channel_two -> signal, index, inputValueChannelTwo);
		}
		else
		{
			auto avgValueChannelOne = calculateAvgValueOfNeighbours(inputSignal.channel_one -> signal, index, span);
			auto avgValueCahnnelTwo = calculateAvgValueOfNeighbours(inputSignal.channel_two -> signal, index, span);
			
			gsl_vector_set (outputSignal.channel_one -> signal, index, avgValueChannelOne) ;
			gsl_vector_set (outputSignal.channel_two -> signal, index, avgValueCahnnelTwo) ;
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
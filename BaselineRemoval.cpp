#include "BaselineRemoval.h"
#include "Butterworth.h"
#include "Filter.h"
#include "SignalPreprocessor.h"
#include "math.h"

BaselineRemoval::BaselineRemoval(){}

BaselineRemoval::~BaselineRemoval(){}

void BaselineRemoval::runModule(const ECGSignal &inputSignal, const ECGInfo & ecgi, ECGSignalChannel &outputSignal)
{
	//For testing purpose
	baselineRemovalMethod = MOVING_AVERAGE;

	SignalPreprocessor * preprocessor = new SignalPreprocessor();	
	ECGSignalChannel betterChannel = ECGSignalChannel(new WrappedVector);
	ECGSignalChannel filteredSignal = ECGSignalChannel(new WrappedVector);
	filteredSignal->signal = gsl_vector_alloc(outputSignal->signal->size);
	
	preprocessor->evaluateSignalChannels(inputSignal, betterChannel);

	switch(baselineRemovalMethod){
		case MOVING_AVERAGE:
			movingAverageBaselineRemoval(betterChannel, filteredSignal, 5);
			break;
		case BUTTERWORTH_2D:
			butterworthBaselineRemoval(betterChannel, filteredSignal, ecgi, 5);
			break;
	}

	preprocessor->normalize(filteredSignal, outputSignal);
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
void BaselineRemoval::movingAverageBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, int span)
{
	auto signalLength = outputSignal-> signal -> size;

	for(long index = 0; index < signalLength; index++)
	{
		auto edge = ceil((float)span/2);

		if(index < edge || index > signalLength - edge) 
		{
			auto inputValue = gsl_vector_get (inputSignal -> signal, index);
			gsl_vector_set(outputSignal -> signal, index, inputValue);
		}
		else
		{
			auto avgValue = calculateAvgValueOfNeighbours(inputSignal -> signal, index, span);		
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

void BaselineRemoval::butterworthBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, const ECGInfo &ecgInfo, int cutoffFreq)
{
	int sampleFreq = ecgInfo.channel_one.frequecy;
	Butterworth * butterworth = new Butterworth();
	std::vector<std::vector<double>> bcCoefficients = butterworth->filterDesign(2, cutoffFreq, sampleFreq, 0);
	
	Filter * filter = new Filter();
	filter->zeroPhase(bcCoefficients[0], bcCoefficients[1], inputSignal, outputSignal, 2);
}
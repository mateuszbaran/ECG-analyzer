#include "ECGBaselineRemoval.h"
#include "Butterworth.h"
#include "Filter.h"
#include "SignalPreprocessor.h"
#include "math.h"

ECGBaselineRemoval::ECGBaselineRemoval(){}

ECGBaselineRemoval::~ECGBaselineRemoval(){}

void ECGBaselineRemoval::runModule(const ECGSignal &inputSignal, const ECGInfo & ecgi, ECGSignalChannel &outputSignal)
{

	SignalPreprocessor * preprocessor = new SignalPreprocessor();	
	ECGSignalChannel betterChannel = ECGSignalChannel(new WrappedVector);
	ECGSignalChannel filteredSignal = ECGSignalChannel(new WrappedVector);
	filteredSignal->signal = gsl_vector_alloc(outputSignal->signal->size);
	
	preprocessor->evaluateSignalChannels(inputSignal, betterChannel);

	switch(baselineRemovalMethod){
		case MOVING_AVERAGE:
			movingAverageBaselineRemoval(betterChannel, filteredSignal, this->span);
			break;
		case BUTTERWORTH:
			butterworthBaselineRemoval(betterChannel, filteredSignal, ecgi, this->order, this->cutoff_frequency, this->attenuation);
			break;
		case CHEBYSHEV:
			chebyshevBaselineRemoval(betterChannel, filteredSignal, ecgi, this->order, this->cutoff_frequency, this->ripple);
			break;
		default:
			movingAverageBaselineRemoval(betterChannel, filteredSignal, 5);
			break;
	}

	preprocessor->normalize(filteredSignal, outputSignal);
}

/**
 Sets selected algorithm for baseline removal.
*/
void ECGBaselineRemoval::setParams(ParametersTypes &params)
{
	this -> baselineRemovalMethod = (BASELINE_REMOVAL_METHOD)params["baseline_removal_method"];

	switch(this->baselineRemovalMethod)
	{
		case MOVING_AVERAGE:
			this->span = params["span"];
			break;
		case BUTTERWORTH:
			this->order = params["order"];
			this->cutoff_frequency = params["cutoff_frequency"];
			this->attenuation = params["attenuation"];
			break;
		case CHEBYSHEV:
			this->order = params["order"];
			this->cutoff_frequency = params["cutoff_frequency"];
			this->ripple = params["ripple"];
			break;
	}
}

/**
	Moving Average algorithm - we assume that variable (default = 5) neighbours are considered 
	while calculating average value for any given point,
	for edge points the output values are copies of the corresponding input values
*/
void ECGBaselineRemoval::movingAverageBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, int span)
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

double ECGBaselineRemoval::calculateAvgValueOfNeighbours(gsl_vector *signal, int currentIndex, int span)
{
	double sum = 0.00;
	for(int i = 1; i <= span/2; i++)
	{
		sum += gsl_vector_get (signal, currentIndex - i);
		sum += gsl_vector_get (signal, currentIndex + i);
	}

	return sum/span;
}

void ECGBaselineRemoval::butterworthBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal,
													const ECGInfo &ecgInfo, int order, double cutoffFrequency, double attenuation)
{
	int sampleFreq = ecgInfo.channel_one.frequecy;
	Butterworth * butterworth = new Butterworth();
	std::vector<std::vector<double>> bcCoefficients = butterworth->filterDesign(2, cutoffFrequency, sampleFreq, 0);
	
	Filter * filter = new Filter();
	filter->zeroPhase(bcCoefficients[0], bcCoefficients[1], inputSignal, outputSignal, 2);
}

void ECGBaselineRemoval::chebyshevBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, 
												  const ECGInfo &ecgInfo, int order, double cutoffFrequency, double ripple)
{
}
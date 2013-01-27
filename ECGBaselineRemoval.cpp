#include "ECGBaselineRemoval.h"
#include "MovingAverage.h"
#include "Butterworth.h"
#include "Chebyshev.h"
#include "Filter.h"
#include "SignalPreprocessor.h"
#include "math.h"

ECGBaselineRemoval::ECGBaselineRemoval(){}

ECGBaselineRemoval::~ECGBaselineRemoval(){}

void ECGBaselineRemoval::runModule(const ECGSignal &inputSignal, const ECGInfo & ecgi, ECGSignalChannel &outputSignal)
{

	SignalPreprocessor * preprocessor = new SignalPreprocessor();	
	ECGSignalChannel betterChannel;

	ECGSignalChannel filteredSignal = ECGSignalChannel(new WrappedVector);
	filteredSignal->signal = gsl_vector_alloc(outputSignal->signal->size);
	
	if(preprocessor->evaluateSignalChannels(inputSignal) == 1)
		betterChannel = inputSignal.channel_one;
	else
		betterChannel = inputSignal.channel_two;

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
	if(params["baseline_removal_method"] == 1)
		this->baselineRemovalMethod = MOVING_AVERAGE;
	else if(params["baseline_removal_method"] == 2)
		this->baselineRemovalMethod = BUTTERWORTH;
	else if(params["baseline_removal_method"] == 3)
		this->baselineRemovalMethod = CHEBYSHEV;

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
	MovingAverage * ma = new MovingAverage();
	ma->removeBaseline(inputSignal, outputSignal, span);
}

void ECGBaselineRemoval::butterworthBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal,
													const ECGInfo &ecgInfo, int order, double cutoffFrequency, double attenuation)
{
	int sampleFreq = ecgInfo.channel_one.frequecy;
	Butterworth * butterworth = new Butterworth();
	std::vector<std::vector<double>> bcCoefficients = butterworth->filterDesign(2, attenuation, cutoffFrequency, sampleFreq, 0);
	
	Filter * filter = new Filter();
	filter->zeroPhase(bcCoefficients[0], bcCoefficients[1], inputSignal, outputSignal, 2);
}

void ECGBaselineRemoval::chebyshevBaselineRemoval(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, 
												  const ECGInfo &ecgInfo, int order, double cutoffFrequency, double ripple)
{
	int sampleFreq = ecgInfo.channel_one.frequecy;
	Chebyshev * chebyshev = new Chebyshev();
	std::vector<std::vector<double>> cbCoefficients = chebyshev->filterDesign(2, ripple, cutoffFrequency, sampleFreq, 0);

	Filter * filter = new Filter();
	filter->zeroPhase(cbCoefficients[0], cbCoefficients[1], inputSignal, outputSignal, 2);
}
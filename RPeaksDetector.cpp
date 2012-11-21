#include "RPeaksDetector.h"


RPeaksDetector::RPeaksDetector()
{
	this->detectionMethod = PAN_TOMPKINS;
	this->panTompkinsMovinghWindowLenght = 25;
}


RPeaksDetector::~RPeaksDetector()
{}

void RPeaksDetector::runModule(const ECGSignal &filteredSignal, ECGRs &ecgRs)
{
	this->filteredSignal = filteredSignal;
	this->rsPositions = ecgRs;

	bool success = this->detectRPeaks();
}

void RPeaksDetector::setParams(ParametersTypes &parameterTypes)
{
	//TODO Allow to set detection method here. If no parameters then use Tompkins method
	//TODo Allot to set moving window size if PanTompkins method
	this->detectionMethod = PAN_TOMPKINS;
	this->panTompkinsMovinghWindowLenght = 25;
}

bool RPeaksDetector::detectRPeaks()
{
	if(detectionMethod == PAN_TOMPKINS)
	{
		panTompkinsRPeaksDetection(&filteredSignal);
		return true;
	}
	else if (detectionMethod == HILBERT)
	{
		hilbertRPeaksDetection(&filteredSignal);
		return true;
	}
	else
		return false;
}

void RPeaksDetector::panTompkinsRPeaksDetection(ECGSignal *signal)
{
	//Convolution [-0.125 -0.25 0.25 0.125] (Tutaj gubimy 4 probki sygna³u)
	ECGSignal diffSig;
	auto sigSize = signal->channel_one->signal->size;
	double filter[] = {-0.125, -0.25, 0.25, 0.125};
	int filterSize = 4;
	for(int i = 0; i < sigSize - filterSize; i++)
	{
		auto tmpSumChannelOne = 0;
		auto tmpSumChannelTwo = 0;
		for(int j = i; j < filterSize; i++)
		{
			auto inputValueChannelOne = gsl_vector_get (signal->channel_one->signal, i);			
			auto inputValueChannelTwo = gsl_vector_get (signal->channel_two->signal, i);
			tmpSumChannelOne += inputValueChannelOne * filter[j];
			tmpSumChannelTwo += inputValueChannelTwo * filter[j];
		}
		gsl_vector_set(diffSig.channel_one->signal, i, tmpSumChannelOne);
		gsl_vector_set(diffSig.channel_two->signal, i, tmpSumChannelTwo);
	}
	
	//Exponentiation
	ECGSignal powSig;
	sigSize = diffSig.channel_one->signal->size;
	for(int i = 0; i < sigSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (diffSig.channel_one->signal, i);			
		auto inputValueChannelTwo = gsl_vector_get (diffSig.channel_two->signal, i);
		gsl_vector_set(powSig.channel_one->signal, i, pow(inputValueChannelOne, 2));
		gsl_vector_set(powSig.channel_two->signal, i, pow(inputValueChannelTwo, 2));
	}

	//Moving window integration (tutaj gubimy 'movinghWindowLenght' próbek sygna³u)
	ECGSignal integrSig;
	sigSize = powSig.channel_one->signal->size;
	auto movinghWindowLenght = 25;
	auto tmpSumChannelOne = 0;
	auto tmpSumChannelTwo = 0;

	for(int i = movinghWindowLenght; i < sigSize; i++)
	{
		for(int j = 0; j < movinghWindowLenght; i++)
		{
			auto inputValueChannelOne = gsl_vector_get (powSig.channel_one->signal, i - j);			
			auto inputValueChannelTwo = gsl_vector_get (powSig.channel_two->signal, i - j);
			tmpSumChannelOne = tmpSumChannelOne + inputValueChannelOne;
			tmpSumChannelTwo = tmpSumChannelTwo + inputValueChannelTwo;
		}
		gsl_vector_set(integrSig.channel_one->signal, i, ((1/movinghWindowLenght) * tmpSumChannelOne));
		gsl_vector_set(integrSig.channel_two->signal, i, ((1/movinghWindowLenght) * tmpSumChannelTwo));
		tmpSumChannelOne = 0;
		tmpSumChannelTwo = 0;
	}

	//Calculating detection thersold
	//TODO (Not important now) Try to find another way to calcutale thersold position, maybe dynamic thersold?
	sigSize = integrSig.channel_one->signal->size;
	auto sigMaxValCHannelOne = 0;
	auto sigMaxValCHannelTwo = 0;
	auto meanChannelOne = 0;
	auto meanChannelTwo = 0;
	for(int i = 0; i < sigSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (integrSig.channel_one->signal, i);			
		auto inputValueChannelTwo = gsl_vector_get (integrSig.channel_two->signal, i);
		if(inputValueChannelOne > sigMaxValCHannelOne)
		{
			sigMaxValCHannelOne = inputValueChannelOne;
		}
		if(inputValueChannelTwo > sigMaxValCHannelTwo)
		{
			sigMaxValCHannelTwo = inputValueChannelTwo;
		}
		meanChannelOne = meanChannelOne + inputValueChannelOne;
		meanChannelTwo = meanChannelTwo + inputValueChannelTwo;
	}
	
	meanChannelOne = meanChannelOne/sigSize;
	meanChannelTwo = meanChannelTwo/sigSize;

	// Try use callculated thersold if not working use constant values;
	auto thresholdCHannelOne = meanChannelOne;
	auto thresholdCHannelTwo = meanChannelTwo;
	//auto thresholdCHannelOne = 0.2;
	//auto thresholdCHannelTwo = 0.2;

	//Looking for points over thersold
	ECGSignal overThersold;
	for(int i = 0; i < sigSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (integrSig.channel_one->signal, i);			
		auto inputValueChannelTwo = gsl_vector_get (integrSig.channel_two->signal, i);
		if(inputValueChannelOne > thresholdCHannelOne * sigMaxValCHannelOne)
		{
			gsl_vector_set(overThersold.channel_one->signal, i, inputValueChannelOne);
		}
		else
		{
			gsl_vector_set(overThersold.channel_one->signal, i, 0);
		}
		if(inputValueChannelTwo > thresholdCHannelOne * sigMaxValCHannelTwo)
		{
			gsl_vector_set(overThersold.channel_one->signal, i, inputValueChannelTwo);
		}
		else
		{
			gsl_vector_set(overThersold.channel_one->signal, i, 0);
		}
	}

	ECGSignal leftPoints;
	ECGSignal rightPoints;
	sigSize = overThersold.channel_one->signal->size;
	int leftPointsCountChannelOne = 0;
	int rightPointsCountChannelOne = 0;
	int leftPointsCountChannelTwo = 0;
	int rightPointsCountChannelTwo = 0;

	for(int i = 0; i < sigSize - 1; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (overThersold.channel_one->signal, i);
		auto inputValueChannelOneIndexPlus = gsl_vector_get (overThersold.channel_one->signal, i + 1);	

		auto inputValueChannelTwo = gsl_vector_get (overThersold.channel_two->signal, i);				
		auto inputValueChannelTwoIndexPlus = gsl_vector_get (overThersold.channel_two->signal, i + 1);
		
		//TODO Translate matlab equation to c
		//leftPoints;// = find(diff([0 overThersold])==1);
		//rightPoints;// = find(diff([overThersold 0])==-1);

		// Channel one
		if(true/*diff([0 overThersold])==1*/)
		{
			gsl_vector_set(leftPoints.channel_one->signal, leftPointsCountChannelOne, i);
			leftPointsCountChannelOne++;
		}
		if(true/*diff([overThersold 0])==-1*/)
		{
			gsl_vector_set(rightPoints.channel_one->signal, rightPointsCountChannelOne, i);
			rightPointsCountChannelOne++;
		}

		// Channel two
		if(true/*diff([0 overThersold])==1*/)
		{
			gsl_vector_set(leftPoints.channel_two->signal, leftPointsCountChannelTwo, i);
			leftPointsCountChannelTwo++;
		}
		if(true/*diff([overThersold 0])==-1*/)
		{
			gsl_vector_set(rightPoints.channel_two->signal, rightPointsCountChannelTwo, i);
			rightPointsCountChannelTwo++;
		}
	}

	
	//R peaks detection
	IntSignal rPosChanOne;
	IntSignal rPosChanTwo;
	sigSize = leftPoints.channel_one->signal->size;
	int partLength;
	double tmpMax;
	int tmpMaxIndex;
	for(int i = 0; i < sigSize; i++)
	{
		// Chanel one
		partLength = gsl_vector_get (rightPoints.channel_one->signal, i) - gsl_vector_get (leftPoints.channel_one->signal, i);
		tmpMax = 0;
		tmpMaxIndex = 0;
		for(int j = 0; j < partLength; j++)
		{
			int sigIndex = gsl_vector_get (leftPoints.channel_one->signal, i) + j;
			auto sigVal = gsl_vector_get(signal->channel_one->signal, sigIndex);
			if(sigVal > tmpMax)
			{
				tmpMax = sigVal;
				tmpMaxIndex = sigIndex;
			}

		}
		gsl_vector_int_set(rPosChanOne->signal, i, tmpMaxIndex);

		//Channel two
		partLength = gsl_vector_get(rightPoints.channel_two->signal, i) - gsl_vector_get(leftPoints.channel_two->signal, i);
		tmpMax = 0;
		tmpMaxIndex = 0;
		for(int j = 0; j < partLength; j++)
		{
			int sigIndex = gsl_vector_get (leftPoints.channel_two->signal, i) + j;
			auto sigVal = gsl_vector_get(signal->channel_two->signal, sigIndex);
			if(sigVal > tmpMax)
			{
				tmpMax = sigVal;
				tmpMaxIndex = sigIndex;
			}

		}
		gsl_vector_int_set(rPosChanTwo->signal, i, tmpMaxIndex);
	}

	rsPositions.setRsChannelOne(rPosChanOne);
	rsPositions.setRsChannelTwo(rPosChanTwo);
	rsDetected = true;
}

void RPeaksDetector::hilbertRPeaksDetection(ECGSignal *signal)
{
	//TODO Body of Hilbert method
	rsDetected = true;
}
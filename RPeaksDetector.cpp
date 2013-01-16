#include "RPeaksDetector.h"
#include "tri_logger.hpp"
#define LOG_END TRI_LOG_STR("END: " << __FUNCTION__);

RPeaksDetector::RPeaksDetector()
{
	this->detectionMethod = PAN_TOMPKINS;
	this->panTompkinsMovinghWindowLenght = 25;
	this->panTompkinsThersold = 0.1;
	this->customParameters = false;
}

RPeaksDetector::~RPeaksDetector()
{
	#ifdef USE_MOCKED_SIGNAL
		gsl_vector_free(mockedSignal->signal);
	#endif
}

void RPeaksDetector::runModule(const ECGSignalChannel &filteredSignal, const ECGInfo & ecgi, ECGRs &ecgRs)
{
	try{
		this->filteredSignal = filteredSignal;
		#ifdef USE_MOCKED_SIGNAL
			this->filteredSignal = getMockedSignal();
		#endif
		this->rsPositions = &ecgRs;
	
		bool success = this->detectRPeaks();
		if(!success)
		{
			#ifdef DEBUG
				cout << "R peaks cetedtion module fails" << endl;
			#endif
			throw new RPeaksDetectionException("R peaks detection method fails");
		}
	} catch (...) {
		#ifdef DEBUG
			cout << "R peaks cetedtion module fails" << endl;
		#endif
		throw new RPeaksDetectionException("Unknown exception during execution R peaks detection method");
	}
}

void RPeaksDetector::setParams(ParametersTypes &parameterTypes)
{
	TRI_LOG_STR(__FUNCTION__);
	if(parameterTypes.find("detection_method") != parameterTypes.end())
	{
		int method = (int)parameterTypes["detection_method"];
		if(method == 0)
		{
			this->detectionMethod = PAN_TOMPKINS;
		}
		else if(method == 1)
		{
			this->detectionMethod = HILBERT;
		}
		else
		{
			#ifdef DEBUG
				cout << "Unknown detection method" << endl;
			#endif
			TRI_LOG_STR("Unknown detection method.");
			throw new RPeaksDetectionException("Unknown detection method.");
		}
	}
	else {
		#ifdef DEBUG
			cout << "Parameter: detection method not found" << endl;
		#endif
		TRI_LOG_STR("Parameter: detection method not found");
		throw new RPeaksDetectionException("Parameter: detection method not found");
	}
	
	if(parameterTypes.find("window_size") != parameterTypes.end())
	{
		this->panTompkinsMovinghWindowLenght = (int)parameterTypes["window_size"];
	}
	else {
		#ifdef DEBUG
			cout << "Window size not found, use default falue 25" << endl;
		#endif
	}

	if(parameterTypes.find("thersold_size") != parameterTypes.end())
	{
		panTompkinsMovinghWindowLenght = parameterTypes["thersold_size"];
	}
	else {
		#ifdef DEBUG
			cout << "Thersold size not found, use default falue 0.1" << endl;
		#endif
	}
	this->customParameters = true;

	#ifdef DEBUG
			cout << "Input parameters for R peaks module:" << endl;
			if(this->detectionMethod == PAN_TOMPKINS)
			{
				cout << "Detection method: PanTompkins" << endl
						  << "Moving window size: " << this->panTompkinsMovinghWindowLenght << endl
						  << "Thersold size: " << panTompkinsThersold << endl;
			} 
			else if (this->detectionMethod == HILBERT)
			{
				cout << "Detection method: Hilbert" << endl;
			}
			else
			{
				cout << "Unknown detection method" << endl;
			}
	#endif
	LOG_END;
}

bool RPeaksDetector::detectRPeaks()
{
	TRI_LOG_STR(__FUNCTION__);
	#ifdef DEBUG
		if(this->customParameters)
		{
			cout << "Running module with custom parameters" << endl;
		}
		else
		{
			cout << "Running module with default parameters" << endl;
		}
	#endif

	if(detectionMethod == PAN_TOMPKINS)
	{
		return panTompkinsRPeaksDetection(&filteredSignal);
	}
	else if (detectionMethod == HILBERT)
	{
		return hilbertRPeaksDetection(&filteredSignal);
	}
	else
	LOG_END;
	return false;
}

bool RPeaksDetector::panTompkinsRPeaksDetection(ECGSignalChannel *signal)
{
	TRI_LOG_STR(__FUNCTION__);
	ECGSignalChannel sig;
	sig = *signal;
	int sigSize = 0;
	if(sig->signal->size == NULL || sig->signal->size == 0)
	{
		#ifdef DEBUG
			cout << "Input signal size is 0" << endl;
		#endif
		TRI_LOG_STR("Input signal size is 0");
		return false;
	}
	else
	{
		sigSize = sig->signal->size;
	}

	// UNECESSARY This part probably is unecessary, should be done in Baseline module
	#ifdef DEBUG
		cout << "DC cancelation and normalization" << endl;
	#endif
	double sigSumVal = 0;
	double sigMaxVal = 0;
	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (sig->signal, i);			
		sigSumVal += inputValue;
		if(abs(inputValue) > sigMaxVal)
		{
			sigMaxVal = abs(inputValue);
			#ifdef DEBUG_SIGNAL
				cout << "New max signal value for channel one: " << inputValueChannelOne << endl;
			#endif
		}
	}
	#ifdef DEBUG
		cout << "Signal sum: " << sigSumVal << endl
		     << "Final signal max value: " <<  sigMaxVal << endl;
	#endif
	
	ECGSignalChannel normSig;
	normSig = ECGSignalChannel(new WrappedVector);
	normSig->signal = gsl_vector_alloc(sigSize);
	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (sig->signal, i);				
		double chanOne = inputValue - (sigSumVal / sigSize);	
		#ifdef DEBUG_SIGNAL
			cout << "DC cancel value: " << chanOne << endl;
		#endif
		chanOne = chanOne / sigMaxVal;	
		gsl_vector_set(normSig->signal, i, chanOne);
		#ifdef DEBUG_SIGNAL
			cout << "Normalized value: " << chanOne << endl;
		#endif
	}
	//END OF UNECESSARY

	//Convolution [-0.125 -0.25 0.25 0.125] (Here we lose 4 signal samples)	
	#ifdef DEBUG
		cout << "Convolution [-0.125 -0.25 0.25 0.125]" << endl << "Orginal signal size: " << sigSize << endl;
	#endif
	int newSigSize = 0;
	ECGSignalChannel diffSig;
	diffSig = ECGSignalChannel(new WrappedVector);
	diffSig->signal = gsl_vector_alloc(sigSize);
	double filter[] = {-0.125, -0.25, 0.25, 0.125};
	int filterSize = 4;
	for(int i = 0; i < sigSize - filterSize; i++)
	{
		double tmpSum = 0;
		for(int j = 0; j < filterSize; j++)
		{
			double inputValue = gsl_vector_get (normSig->signal, i + j);			
			tmpSum += inputValue * filter[j];
			#ifdef DEBUG_SIGNAL_DETAILS
				cout << "Signal: " << inputValue << " Filter: " << filter[j] << " Sum: " << tmpSum << endl;
			#endif
		}
		#ifdef DEBUG_SIGNAL
			cout << "Final val: " << tmpSum << " at index: " << i << endl;
		#endif
		gsl_vector_set(diffSig->signal, i, tmpSum);
		newSigSize++;
	}
	
	//Exponentiation
	sigSize = newSigSize;
	#ifdef DEBUG
		cout << "Exponentiation ^2" << endl << "Signal size after convolution: " << sigSize << endl;
	#endif
	ECGSignalChannel powSig;
	powSig = ECGSignalChannel(new WrappedVector);
	powSig->signal = gsl_vector_alloc(sigSize);
	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (diffSig->signal, i);			
		double powVal = pow(inputValue, 2);
		gsl_vector_set(powSig->signal, i, powVal);
		#ifdef DEBUG_SIGNAL
				cout << " Pow: "<< powVal << " at index: " << i  << endl;
		#endif
	}

	//Moving window integration (Here we lose "movinghWindowLenght" signal samples)	
	#ifdef DEBUG
		cout << "Moving window integration" << endl << "Window size: " << panTompkinsMovinghWindowLenght << endl
				  << "Signal size after exponentiation: " << sigSize << endl;
	#endif
	ECGSignalChannel integrSig;
	integrSig = ECGSignalChannel(new WrappedVector);
	integrSig->signal = gsl_vector_alloc(sigSize);
	newSigSize = 0;
	int movinghWindowLenght = panTompkinsMovinghWindowLenght;
	double tmpSum = 0;

	for(int i = movinghWindowLenght; i < sigSize; i++)
	{
		for(int j = movinghWindowLenght - 1; j >= 0 ; j--)
		{
			double inputValue = gsl_vector_get (powSig->signal, i - j);			
			tmpSum += inputValue;
			#ifdef DEBUG_SIGNAL_DETAILS
				cout << "Signal: " << inputValue << " Sum: " << tmpSum << endl;
			#endif
		}
		int index = i - movinghWindowLenght;
		// TODO Why this is not working? (To small values and all are save as zero)
		//double mwico = (1/movinghWindowLenght) * tmpSum;
		double mwico =  tmpSum;
		#ifdef DEBUG_SIGNAL
			cout << "Final val: " << mwico << " at index: " << index << endl;
		#endif
		gsl_vector_set(integrSig->signal, index, mwico);
		tmpSum = 0;
		newSigSize++;
	}

	//Calculating detection thersold
	//TODO (Not important now) Try to find another way to calcutale thersold position, maybe dynamic thersold?
	sigSize = newSigSize;
	#ifdef DEBUG
		cout << "Calculating detection thersold" << endl << "After moving window integration signal size: " << sigSize << endl;
	#endif
	sigMaxVal = 0;
	double meanVal = 0;

	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (integrSig->signal, i);					
		if(inputValue > sigMaxVal)
		{
			sigMaxVal = inputValue;
			#ifdef DEBUG_SIGNAL
				cout << "New max signal value: " << inputValue << endl;
			#endif
		}
		meanVal += inputValue;
	}
	meanVal = meanVal / sigSize;

	#ifdef DEBUG
		cout << "Final max value for channel one: " << sigMaxVal << endl 
	         << "Final mean value: " << meanVal << endl;

	#endif

	// Select automatic or manual thersold
	double threshold = 0;
	if( this->panTompkinsThersold == 0)
	{
		threshold = meanVal;
	}
	else
	{
		threshold = this->panTompkinsThersold;
	}

	//Looking for points over thersold
	#ifdef DEBUG
		cout << "Looking for points over thersold" << endl;
	#endif
	ECGSignalChannel overThersold;
	overThersold = ECGSignalChannel(new WrappedVector);
	overThersold->signal = gsl_vector_alloc(sigSize);
	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (integrSig->signal, i);			
		if(inputValue > threshold * sigMaxVal)
		{
			gsl_vector_set(overThersold->signal, i, 1);
			#ifdef DEBUG_SIGNAL
				cout << "Value over thersold at index: " << i << endl;
			#endif
		}
		else
		{
			gsl_vector_set(overThersold->signal, i, 0);
		}
	}
	#ifdef DEBUG_SIGNAL
		cout << "Signal with points over thersold" << endl;
		for(int i = 0; i < sigSize; i++)
		{
			cout << gsl_vector_get(overThersold->signal, i);
		}
	#endif
	#ifdef DEBUG
		cout << "Detect begin and end of QRS complex" << endl;
	#endif
	ECGSignalChannel leftPoints;
	ECGSignalChannel tmpRightPoints;
	leftPoints = ECGSignalChannel(new WrappedVector);
	tmpRightPoints = ECGSignalChannel(new WrappedVector);
	leftPoints->signal = gsl_vector_alloc(sigSize);
	tmpRightPoints->signal = gsl_vector_alloc(sigSize);
	int leftPointsCount = 0;
	int rightPointsCount = 0;

	gsl_vector* copiedSig = gsl_vector_calloc(sigSize);
	gsl_vector_memcpy(copiedSig, overThersold->signal);

	// Boundary values
	if(gsl_vector_get (copiedSig, 0) == 1)
	{
		gsl_vector_set(leftPoints->signal, leftPointsCount, 0);
		leftPointsCount++;
		#ifdef DEBUG_SIGNAL
			cout << "QRS complex left point at index: " << 0 << endl;
		#endif
	}
	
	if(gsl_vector_get (copiedSig, sigSize - 1) == 1)
	{
		gsl_vector_set(tmpRightPoints->signal, rightPointsCount, sigSize - 1);
		rightPointsCount++;
		#ifdef DEBUG_SIGNAL
			cout << "QRS complex right point at index: " << sigSize - 1 << endl;
		#endif
	}

	// Left points of QRS complex
	for(int i = 0; i < sigSize - 1; i++)
	{
		double inputValue = gsl_vector_get (copiedSig, i);
		double inputValueIndexPlus = gsl_vector_get (copiedSig, i + 1);
		if((inputValueIndexPlus - inputValue) == 1)
		{
			gsl_vector_set(leftPoints->signal, leftPointsCount, i);
			leftPointsCount++;
			#ifdef DEBUG_SIGNAL
				cout << "QRS complex left point at index: " << i << endl;
			#endif
		}
	}

	// Rights points of QRS complex
	for(int i = sigSize - 1; i > 0; i--)
	{
		double reversedInput = gsl_vector_get(copiedSig, i);
		double reversedInputIndexMinus = gsl_vector_get (copiedSig, i - 1);
		if((reversedInputIndexMinus - reversedInput) == 1)
		{
			gsl_vector_set(tmpRightPoints->signal, rightPointsCount, i);
			rightPointsCount++;
			#ifdef DEBUG_SIGNAL
				cout << "QRS complex right at index: " << i << endl;
			#endif
		}
	}

	#ifdef DEBUG_SIGNAL
		cout << "Vector with left points:" << endl;
		for(int i = 0; i < leftPointsCount; i++)
		{
			cout << gsl_vector_get(leftPoints->signal, i) << ' ';
		}
		cout << endl << "Vector with right points:" << endl;
		for(int i = 0; i < rightPointsCount; i++)
		{
			cout << gsl_vector_get(tmpRightPoints->signal, i) << ' ';
		}
		cout << endl;
	#endif
	// Invert vector with rightPoints
	ECGSignalChannel rightPoints;
	rightPoints = ECGSignalChannel(new WrappedVector);
	rightPoints->signal = gsl_vector_alloc(sigSize);
	for(int i = 0; i < rightPointsCount; i++)
	{
		double tmp = gsl_vector_get(tmpRightPoints->signal, rightPointsCount - i - 1);
		gsl_vector_set(rightPoints->signal, i, tmp );
	}
	for(int i = 0; i < rightPointsCount; i++)
	{
		double tmp = gsl_vector_get(tmpRightPoints->signal, rightPointsCount - i - 1);
		gsl_vector_set(rightPoints->signal, i, tmp );
	}
	#ifdef DEBUG_SIGNAL
		cout << "After vector invertion" << endl;
		cout << "Vector with left points:" << endl;
		for(int i = 0; i < leftPointsCount; i++)
		{
			cout << gsl_vector_get(leftPoints->signal, i) << ' ';
		}
		cout << endl << "Vector with right points:" << endl;
		for(int i = 0; i < rightPointsCount; i++)
		{
			cout << gsl_vector_get(rightPoints->signal, i) << ' ';
		}
		cout << endl;
	#endif
	#ifdef DEBUG
		cout << "Number of left points: " << leftPointsCount << endl
			 << "Number of right points: " << rightPointsCount << endl;
	#endif

	//Final R peaks detection
	#ifdef DEBUG
		cout << "Final R peaks detection" << endl;
	#endif

	int partLength;
	double tmpMax;
	int tmpMaxIndex;
	IntSignal rs;

	if(leftPointsCount > 0 )
	{
		rs = IntSignal(new WrappedVectorInt);
		rs->signal = gsl_vector_int_alloc(leftPointsCount);
		for(int i = 0; i < leftPointsCount; i++)
		{		
			partLength = gsl_vector_get (rightPoints->signal, i) - gsl_vector_get(leftPoints->signal, i);
			tmpMax = 0;
			tmpMaxIndex = 0;
			for(int j = 0; j < partLength; j++)
			{
				int sigIndex = gsl_vector_get (leftPoints->signal, i) + j;
				double sigVal = gsl_vector_get(sig->signal, sigIndex);
				if(sigVal > tmpMax)
				{
					tmpMax = sigVal;
					tmpMaxIndex = sigIndex;
				}

			}
			gsl_vector_int_set(rs->signal, i, tmpMaxIndex);
			#ifdef DEBUG_SIGNAL
				cout << "R point at index: " << tmpMaxIndex 
					<< " signal value: " << gsl_vector_get(sig->signal, tmpMaxIndex) << endl;
			#endif
		}
		rsPositions->setRs(rs);
	}
	else
	{
		#ifdef DEBUG
			cout << "R peaks not detected. Check input signal." << endl;
		#endif
		TRI_LOG_STR("R peaks not detected. Check input signal.");
		return false;
	}
	gsl_vector_free(copiedSig);

	rsDetected = true;
	#ifdef DEBUG
		cout << "Done" << endl;
	#endif
	LOG_END;
	return true;
}

bool RPeaksDetector::hilbertRPeaksDetection(ECGSignalChannel *signal)
{
	TRI_LOG_STR(__FUNCTION__);

	//TODO Body of Hilbert method

	rsDetected = true;
	LOG_END;
	return true;
}

ECGSignalChannel RPeaksDetector::getMockedSignal()
{
	TRI_LOG_STR(__FUNCTION__);

	int length = 600000; // MAX 650000
	mockedSignal = ECGSignalChannel(new WrappedVector);
	mockedSignal->signal = gsl_vector_alloc(length);

	string line;
    ifstream myfile("FilteredSignal.txt");
    if (myfile.is_open())
    {
		for(int i; i < length; i++)
        {
            getline (myfile,line);
			istringstream stm;
			stm.str(line);
			double d;
			stm >> d;
			gsl_vector_set(mockedSignal->signal, i, d);
        }
		myfile.close();
    }
	else
	{
		#ifdef DEBUG
			cout << "Cannot read mocked signal." << endl;
		#endif
		TRI_LOG_STR("Cannot read mocked signal.");
	}
	LOG_END;
	return mockedSignal;
}  

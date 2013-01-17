#include "RPeaksDetector.h"
#include<QDebug>
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
				qDebug() << "R peaks cetedtion module fails";
			#endif
			throw new RPeaksDetectionException("R peaks detection method fails");
		}
	} catch (...) {
		#ifdef DEBUG
			qDebug() << "R peaks cetedtion module fails";
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
				qDebug() << "Unknown detection method";
			#endif
			TRI_LOG_STR("Unknown detection method.");
			throw new RPeaksDetectionException("Unknown detection method.");
		}
	}
	else {
		#ifdef DEBUG
			qDebug() << "Parameter: detection method not found";
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
			qDebug() << "Window size not found, use default falue 25";
		#endif
	}

	if(parameterTypes.find("thersold_size") != parameterTypes.end())
	{
		panTompkinsMovinghWindowLenght = parameterTypes["thersold_size"];
	}
	else {
		#ifdef DEBUG
			qDebug() << "Thersold size not found, use default falue 0.1";
		#endif
	}
	this->customParameters = true;

	#ifdef DEBUG
			cout << "Input parameters for R peaks module:" << endl;
			if(this->detectionMethod == PAN_TOMPKINS)
			{
				qDebug()  << "Detection method: PanTompkins" << endl
						  << "Moving window size: " << this->panTompkinsMovinghWindowLenght << endl
						  << "Thersold size: " << panTompkinsThersold;
			} 
			else if (this->detectionMethod == HILBERT)
			{
				qDebug() << "Detection method: Hilbert";
			}
			else
			{
				qDebug() << "Unknown detection method";
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
			qDebug() << "Running module with custom parameters";
		}
		else
		{
			qDebug() << "Running module with default parameters";
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
			qDebug() << "Input signal size is 0";
		#endif
		TRI_LOG_STR("Input signal size is 0");
		return false;
	}
	else
	{
		sigSize = sig->signal->size;
		#ifdef DEBUG_SIGNAL
			qDebug() << "Input signal";	
			for(int i = 0; i < sigSize; i++)
			{
				double inputValue = gsl_vector_get (sig->signal, i);	
				qDebug() << inputValue;	
			}
		#endif
	}

	//Convolution [-0.125 -0.25 0.25 0.125] (Here we lose 4 signal samples)	
	#ifdef DEBUG
		qDebug() << "Convolution [-0.125 -0.25 0.25 0.125]" << endl << "Orginal signal size: " << sigSize;
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
			double inputValue = gsl_vector_get (sig->signal, i + j);			
			tmpSum += inputValue * filter[j];
			#ifdef DEBUG_SIGNAL_DETAILS
				qDebug() << "Signal: " << inputValue << " Filter: " << filter[j] << " Sum: " << tmpSum;
			#endif
		}
		#ifdef DEBUG_SIGNAL
			qDebug() << "Final val: " << tmpSum << " at index: " << i;
		#endif
		gsl_vector_set(diffSig->signal, i, tmpSum);
		newSigSize++;
	}
	
	//Exponentiation
	sigSize = newSigSize;
	#ifdef DEBUG
		qDebug() << "Exponentiation ^2" << endl << "Signal size after convolution: " << sigSize;
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
				qDebug() << " Pow: "<< powVal << " at index: " << i;
		#endif
	}

	//Moving window integration (Here we lose "movinghWindowLenght" signal samples)	
	#ifdef DEBUG
		qDebug() << "Moving window integration" << endl << "Window size: " << panTompkinsMovinghWindowLenght << endl
				  << "Signal size after exponentiation: " << sigSize;
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
				qDebug() << "Signal: " << inputValue << " Sum: " << tmpSum;
			#endif
		}
		int index = i - movinghWindowLenght;
		// TODO Why this is not working? (To small values and all are save as zero)
		//double mwico = (1/movinghWindowLenght) * tmpSum;
		double mwico =  tmpSum;
		#ifdef DEBUG_SIGNAL
			qDebug() << "Final val: " << mwico << " at index: " << index;
		#endif
		gsl_vector_set(integrSig->signal, index, mwico);
		tmpSum = 0;
		newSigSize++;
	}

	//Calculating detection thersold
	//TODO (Not important now) Try to find another way to calcutale thersold position, maybe dynamic thersold?
	sigSize = newSigSize;
	#ifdef DEBUG
		qDebug() << "Calculating detection thersold" << endl << "After moving window integration signal size: " << sigSize;
	#endif
	double sigMaxVal = 0;
	double meanVal = 0;

	for(int i = 0; i < sigSize; i++)
	{
		double inputValue = gsl_vector_get (integrSig->signal, i);					
		if(inputValue > sigMaxVal)
		{
			sigMaxVal = inputValue;
			#ifdef DEBUG_SIGNAL
				qDebug() << "New max signal value: " << inputValue;
			#endif
		}
		meanVal += inputValue;
	}
	meanVal = meanVal / sigSize;

	#ifdef DEBUG
		qDebug() << "Final max value for channel one: " << sigMaxVal << endl
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
		qDebug() << "Looking for points over thersold";
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
				qDebug() << "Value over thersold at index: " << i;
			#endif
		}
		else
		{
			gsl_vector_set(overThersold->signal, i, 0);
		}
	}
	#ifdef DEBUG_SIGNAL
		qDebug() << "Signal with points over thersold";
		for(int i = 0; i < sigSize; i++)
		{
			qDebug() << gsl_vector_get(overThersold->signal, i);
		}
	#endif
	#ifdef DEBUG
		qDebug() << "Detect begin and end of QRS complex";
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
			qDebug() << "QRS complex left point at index: " << 0;
		#endif
	}
	
	if(gsl_vector_get (copiedSig, sigSize - 1) == 1)
	{
		gsl_vector_set(tmpRightPoints->signal, rightPointsCount, sigSize - 1);
		rightPointsCount++;
		#ifdef DEBUG_SIGNAL
			qDebug() << "QRS complex right point at index: " << sigSize - 1;
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
				qDebug() << "QRS complex left point at index: " << i;
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
				qDebug() << "QRS complex right at index: " << i;
			#endif
		}
	}

	#ifdef DEBUG_SIGNAL
		cout << "Vector with left points:" << endl;
		for(int i = 0; i < leftPointsCount; i++)
		{
			qDebug() << gsl_vector_get(leftPoints->signal, i);
		}
		qDebug() << endl << "Vector with right points:";
		for(int i = 0; i < rightPointsCount; i++)
		{
			qDebug() << gsl_vector_get(tmpRightPoints->signal, i);
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
		qDebug() << "After vector invertion" << endl;
		qDebug() << "Vector with left points:" << endl;
		for(int i = 0; i < leftPointsCount; i++)
		{
			qDebug() << gsl_vector_get(leftPoints->signal, i);
		}
		qDebug() << endl << "Vector with right points:" << endl;
		for(int i = 0; i < rightPointsCount; i++)
		{
			qDebug() << gsl_vector_get(rightPoints->signal, i);
		}
	#endif
	#ifdef DEBUG
		qDebug() << "Number of left points: " << leftPointsCount << endl
			     << "Number of right points: " << rightPointsCount;
	#endif

	//Final R peaks detection
	#ifdef DEBUG
		qDebug() << "Final R peaks detection";
	#endif

	int partLength;
	double tmpMax;
	int tmpMaxIndex;
	IntSignal rs;
	int numberRs = 0;

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
			numberRs++;
			#ifdef DEBUG_SIGNAL
				qDebug() << "R point at index: " << tmpMaxIndex 
					     << " signal value: " << gsl_vector_get(sig->signal, tmpMaxIndex);
			#endif
		}
		rsPositions->setRs(rs);
		#ifdef DEBUG
			qDebug() << "Number of detected R-peaks:" << numberRs;
		#endif
	}
	else
	{
		#ifdef DEBUG
			qDebug() << "R peaks not detected. Check input signal.";
		#endif
		TRI_LOG_STR("R peaks not detected. Check input signal.");
		return false;
	}
	gsl_vector_free(copiedSig);

	rsDetected = true;
	#ifdef DEBUG
		qDebug() << "Done";
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

	string line = "";
	ifstream myfile;
	myfile.open("FilteredSignal.txt");
	if (myfile.is_open())
    {
		int i = 0;
		while (!myfile.eof())
		{
			getline(myfile, line);
			std::istringstream stm;
			stm.str(line);
			double d;
			stm >>d;
			gsl_vector_set(mockedSignal->signal, i, d);
			i++;
			if(i == length)
				break;
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

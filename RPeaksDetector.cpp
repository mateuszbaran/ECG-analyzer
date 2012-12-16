#include "RPeaksDetector.h"


RPeaksDetector::RPeaksDetector()
{
	this->detectionMethod = PAN_TOMPKINS;
	this->panTompkinsMovinghWindowLenght = 25;
	this->panTompkinsThersold = 0.1;
	this->customParameters = false;
}

RPeaksDetector::~RPeaksDetector()
{}

void RPeaksDetector::runModule(const ECGSignalChannel &filteredSignal, const ECGInfo & ecgi, ECGRs &ecgRs)
{
	try{
		this->filteredSignal = filteredSignal;
		#ifdef USE_MOCKED_SIGNAL
			this->filteredSignal = getMockedSignal();
		#endif
		this->rsPositions = ecgRs;
	
		bool success = this->detectRPeaks();
		if(!success)
		{
			#ifdef DEBUG
				cout << "R peaks detection fails" << endl;
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
			throw new RPeaksDetectionException("Unknown detection method.");
		}
	}
	else {
		#ifdef DEBUG
			cout << "Unknown detection method" << endl;
		#endif
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
		this->panTompkinsMovinghWindowLenght = 25;
	}

	if(parameterTypes.find("thersold_size") != parameterTypes.end())
	{
		panTompkinsMovinghWindowLenght = parameterTypes["thersold_size"];
	}
	else {
		#ifdef DEBUG
			cout << "Thersold size not found, use default falue 0.1" << endl;
		#endif
		this->panTompkinsThersold = 0.1;
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
}

bool RPeaksDetector::detectRPeaks()
{
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

	return false;
}

bool RPeaksDetector::panTompkinsRPeaksDetection(ECGSignalChannel *signal)
{
	ECGSignalChannel sig;
	sig = *signal;
	int sigSize = 0;
	sigSize = sig->signal->size;
	if(sigSize > 0)
	{
		#ifdef DEBUG
			cout << "Input signal size is 0" << endl;
		#endif
		return false;
	}

	// UNECESSARY This part probably is unecessary 
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
				cout << "New max signal value: " << inputValue << endl;
			#endif
		}
	}
	#ifdef DEBUG
		cout << "Signal sum: " << sigSumVal << endl
				  << "Final signal max value: " <<  sigMaxVal << endl;
	#endif
				
	ECGSignalChannel normSig;
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

	// Other values
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
	gsl_vector_free(copiedSig);

	//R peaks detection
	#ifdef DEBUG
		cout << "Final R peaks detection" << endl;
	#endif

	int partLength;
	double tmpMax;
	int tmpMaxIndex;
	IntSignal rs;

	// Chanel one
	if(leftPointsCount > 0 )
	{
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
		rsPositions.setRs(&rs);
	}
	rsDetected = true;
	#ifdef DEBUG
		cout << "Done" << endl;
	#endif
	return true;
}

bool RPeaksDetector::hilbertRPeaksDetection(ECGSignalChannel *signal)
{
	//TODO Body of Hilbert method
	rsDetected = true;
	return true;
}

ECGSignalChannel RPeaksDetector::getMockedSignal()
{
	double signal[] = {
   -0.0000,
   -0.0001,
   -0.0003,
   -0.0006,
   -0.0010,
   -0.0015,
   -0.0023,
   -0.0031,
   -0.0040,
   -0.0050,
   -0.0061,
   -0.0071,
   -0.0082,
   -0.0093,
   -0.0104,
   -0.0115,
   -0.0117,
   -0.0109,
   -0.0091,
   -0.0063,
   -0.0025,
    0.0022,
    0.0062,
    0.0095,
    0.0121,
    0.0144,
    0.0160,
    0.0167,
    0.0173,
    0.0178,
    0.0179,
    0.0168,
    0.0147,
    0.0121,
    0.0091,
    0.0055,
    0.0015,
   -0.0019,
   -0.0046,
   -0.0071,
   -0.0091,
   -0.0103,
   -0.0107,
   -0.0104,
   -0.0094,
   -0.0069,
   -0.0035,
    0.0004,
    0.0042,
    0.0074,
    0.0095,
    0.0100,
    0.0090,
    0.0068,
    0.0038,
    0.0006,
   -0.0022,
   -0.0043,
   -0.0060,
   -0.0068,
   -0.0067,
   -0.0061,
   -0.0055,
   -0.0052,
   -0.0047,
   -0.0043,
   -0.0044,
   -0.0044,
   -0.0041,
   -0.0029,
   -0.0017,
   -0.0002,
    0.0014,
    0.0029,
    0.0038,
    0.0040,
    0.0038,
    0.0032,
    0.0026,
    0.0019,
    0.0016,
    0.0018,
    0.0021,
    0.0026,
    0.0025,
    0.0022,
    0.0014,
   -0.0004,
   -0.0032,
   -0.0065,
   -0.0096,
   -0.0126,
   -0.0151,
   -0.0167,
   -0.0168,
   -0.0157,
   -0.0139,
   -0.0116,
   -0.0088,
   -0.0061,
   -0.0037,
   -0.0015,
    0.0008,
    0.0036,
    0.0062,
    0.0085,
    0.0100,
    0.0102,
    0.0084,
    0.0048,
    0.0000,
   -0.0048,
   -0.0093,
   -0.0123,
   -0.0133,
   -0.0135,
   -0.0136,
   -0.0147,
   -0.0179,
   -0.0243,
   -0.0348,
   -0.0485,
   -0.0648,
   -0.0824,
   -0.1002,
   -0.1166,
   -0.1301,
   -0.1396,
   -0.1435,
   -0.1380,
   -0.1174,
   -0.0779,
   -0.0220,
    0.0429,
    0.1090,
    0.1660,
    0.2007,
    0.2042,
    0.1792,
    0.1378,
    0.0910,
    0.0472,
    0.0135,
   -0.0057,
   -0.0113,
   -0.0094,
   -0.0046,
    0.0014,
    0.0091,
    0.0196,
    0.0332,
    0.0505,
    0.0711,
    0.0937,
    0.1162,
    0.1361,
    0.1518,
    0.1613,
    0.1638,
    0.1592,
    0.1479,
    0.1299,
    0.1049,
    0.0736,
    0.0369,
   -0.0033,
   -0.0436,
   -0.0805,
   -0.1105,
   -0.1309,
   -0.1412,
   -0.1428,
   -0.1389,
   -0.1320,
   -0.1237,
   -0.1154,
   -0.1071,
   -0.0977,
   -0.0858,
   -0.0712,
   -0.0549,
   -0.0381,
   -0.0220,
   -0.0082,
    0.0020,
    0.0079,
    0.0100,
    0.0092,
    0.0066,
    0.0030,
   -0.0000,
   -0.0024,
   -0.0033,
   -0.0031,
   -0.0023,
   -0.0009,
    0.0006,
    0.0027,
    0.0046,
    0.0064,
    0.0080,
    0.0094,
    0.0102,
    0.0099,
    0.0089,
    0.0074,
    0.0059,
    0.0039,
    0.0020,
    0.0003,
   -0.0009,
   -0.0020,
   -0.0032,
   -0.0038,
   -0.0040,
   -0.0037,
   -0.0032,
   -0.0024,
   -0.0015,
   -0.0011,
   -0.0011,
   -0.0017,
   -0.0027,
   -0.0040,
   -0.0051,
   -0.0057,
   -0.0058,
   -0.0052,
   -0.0043,
   -0.0028,
   -0.0014,
   -0.0002,
    0.0008,
    0.0017,
    0.0026,
    0.0034,
    0.0047,
    0.0060,
    0.0076,
    0.0089,
    0.0102,
    0.0113,
    0.0118,
    0.0125,
    0.0131,
    0.0140,
    0.0146,
    0.0151,
    0.0158,
    0.0161,
    0.0162,
    0.0162,
    0.0163,
    0.0164,
    0.0161,
    0.0155,
    0.0147,
    0.0135,
    0.0121,
    0.0105,
    0.0088,
    0.0072,
    0.0056,
    0.0038,
    0.0022,
    0.0005,
   -0.0011,
   -0.0025,
   -0.0036,
   -0.0043,
   -0.0047,
   -0.0050,
   -0.0048,
   -0.0049,
   -0.0052,
   -0.0055,
   -0.0059,
   -0.0063,
   -0.0068,
   -0.0066,
   -0.0061,
   -0.0058,
   -0.0057,
   -0.0054,
   -0.0050,
   -0.0050,
   -0.0052,
   -0.0051,
   -0.0047,
   -0.0044,
   -0.0045,
   -0.0046,
   -0.0042,
   -0.0041,
   -0.0038,
   -0.0037,
   -0.0034,
   -0.0030,
   -0.0032,
   -0.0034,
   -0.0040,
   -0.0044,
   -0.0045,
   -0.0047,
   -0.0045,
   -0.0042,
   -0.0034,
   -0.0025,
   -0.0017,
   -0.0006,
    0.0004,
    0.0015,
    0.0028,
    0.0038,
    0.0044,
    0.0049,
    0.0054,
    0.0057,
    0.0056,
    0.0056,
    0.0058,
    0.0057,
    0.0056,
    0.0052,
    0.0048,
    0.0042,
    0.0035,
    0.0028,
    0.0021,
    0.0016,
    0.0010,
    0.0005,
    0.0002,
    0.0003,
    0.0003,
    0.0005,
    0.0009,
    0.0013,
    0.0015,
    0.0014,
    0.0014,
    0.0013,
    0.0011,
    0.0007,
    0.0003,
    0.0002,
   -0.0001,
   -0.0005,
   -0.0012,
   -0.0016,
   -0.0019,
   -0.0022,
   -0.0020,
   -0.0018,
   -0.0010,
   -0.0001,
    0.0006,
    0.0011,
    0.0008,
    0.0005,
   -0.0005,
   -0.0017,
   -0.0028,
   -0.0036,
   -0.0038,
   -0.0038,
   -0.0033,
   -0.0024,
   -0.0016,
   -0.0006,
   -0.0001,
    0.0008,
    0.0014,
    0.0014,
    0.0017,
    0.0015,
    0.0014,
    0.0010,
    0.0003,
   -0.0002,
   -0.0007,
   -0.0014,
   -0.0022,
   -0.0028,
   -0.0031,
   -0.0035,
   -0.0037,
   -0.0037,
   -0.0033,
   -0.0031,
   -0.0029,
   -0.0023,
   -0.0019,
   -0.0015,
   -0.0011,
   -0.0002,
    0.0007,
    0.0016,
    0.0022,
    0.0027,
    0.0027,
    0.0023,
    0.0017,
    0.0005,
   -0.0006,
   -0.0022,
   -0.0044,
   -0.0073,
   -0.0117,
   -0.0175,
   -0.0256,
   -0.0357,
   -0.0476,
   -0.0614,
   -0.0760,
   -0.0908,
   -0.1048,
   -0.1174,
   -0.1283,
   -0.1366,
   -0.1424,
   -0.1447,
   -0.1423,
   -0.1309,
   -0.1038,
   -0.0570,
    0.0078,
    0.0829,
    0.1589,
    0.2248,
    0.2658,
    0.2723,
    0.2460,
    0.1994,
    0.1450,
    0.0919,
    0.0484,
    0.0198,
    0.0062,
    0.0020,
    0.0026,
    0.0063,
    0.0132,
    0.0228,
    0.0345,
    0.0479,
    0.0613,
    0.0733,
    0.0818,
    0.0862,
    0.0859,
    0.0802,
    0.0698,
    0.0555,
    0.0375,
    0.0163,
   -0.0079,
   -0.0337,
   -0.0599,
   -0.0842,
   -0.1032,
   -0.1144,
   -0.1162,
   -0.1087,
   -0.0936,
   -0.0749,
   -0.0562,
   -0.0406,
   -0.0292,
   -0.0215,
   -0.0163,
   -0.0112,
   -0.0054,
    0.0012,
    0.0078,
    0.0127,
    0.0149,
    0.0139,
    0.0097,
    0.0040,
   -0.0021,
   -0.0076,
   -0.0118,
   -0.0149,
   -0.0163,
   -0.0169,
   -0.0168,
   -0.0161,
   -0.0146,
   -0.0122,
   -0.0095,
   -0.0064,
   -0.0034,
   -0.0008,
    0.0010,
    0.0022,
    0.0025,
    0.0024,
    0.0020,
    0.0014,
    0.0010,
    0.0003,
   -0.0002,
   -0.0008,
   -0.0014,
   -0.0017,
   -0.0021,
   -0.0023,
   -0.0026,
   -0.0027,
   -0.0029,
   -0.0033,
   -0.0039,
   -0.0049,
   -0.0057,
   -0.0066,
   -0.0075,
   -0.0080,
   -0.0081,
   -0.0076,
   -0.0073,
   -0.0066,
   -0.0057,
   -0.0047,
   -0.0035,
   -0.0022,
   -0.0002,
    0.0018,
    0.0036,
    0.0054,
    0.0071,
    0.0087,
    0.0099,
    0.0107,
    0.0117,
    0.0125,
    0.0130,
    0.0131,
    0.0129,
    0.0125,
    0.0117,
    0.0109,
    0.0101,
    0.0095,
    0.0091,
    0.0089,
    0.0090,
    0.0090,
    0.0089,
    0.0087,
    0.0083,
    0.0081,
    0.0077,
    0.0073,
    0.0069,
    0.0064,
    0.0059,
    0.0049,
    0.0036,
    0.0020,
    0.0003,
   -0.0011,
   -0.0024,
   -0.0032,
   -0.0038,
   -0.0040,
   -0.0043,
   -0.0045,
   -0.0048,
   -0.0053,
   -0.0057,
   -0.0060,
   -0.0059,
   -0.0058,
   -0.0053,
   -0.0048,
   -0.0044,
   -0.0041,
   -0.0040,
   -0.0041,
   -0.0046,
   -0.0050,
   -0.0051,
   -0.0049,
   -0.0046,
   -0.0039,
   -0.0031,
   -0.0023,
   -0.0014,
   -0.0006,
   -0.0001,
    0.0003,
    0.0006,
    0.0010,
    0.0011,
    0.0010,
    0.0011,
    0.0013,
    0.0015,
    0.0015,
    0.0013,
    0.0014,
    0.0011,
    0.0008,
    0.0007,
    0.0008,
    0.0012,
    0.0015,
    0.0022,
    0.0030,
    0.0036,
    0.0041,
    0.0044,
    0.0044,
    0.0040,
    0.0033,
    0.0025,
    0.0015,
    0.0005,
    0.0001,
   -0.0001,
   -0.0002,
   -0.0001,
    0.0001,
    0.0004,
    0.0004,
    0.0004,
    0.0005,
    0.0006,
    0.0009,
    0.0010,
    0.0013,
    0.0015,
    0.0013,
    0.0012,
    0.0008,
    0.0006,
    0.0001,
   -0.0004,
   -0.0005,
   -0.0007,
   -0.0009,
   -0.0014,
   -0.0016,
   -0.0018,
   -0.0020,
   -0.0021,
   -0.0020,
   -0.0014,
   -0.0010,
   -0.0006,
   -0.0003,
    0.0002,
    0.0006,
    0.0010,
    0.0015,
    0.0018,
    0.0017,
    0.0014,
    0.0008,
   -0.0001,
   -0.0011,
   -0.0020,
   -0.0025,
   -0.0029,
   -0.0032,
   -0.0033,
   -0.0033,
   -0.0035,
   -0.0035,
   -0.0032,
   -0.0024,
   -0.0016,
   -0.0009,
    0.0001,
    0.0008,
    0.0012,
    0.0012,
    0.0009,
    0.0006,
    0.0003,
    0.0000,
   -0.0005,
   -0.0010,
   -0.0013,
   -0.0018,
   -0.0021,
   -0.0022,
   -0.0018,
   -0.0015,
   -0.0013,
   -0.0012,
   -0.0016,
   -0.0027,
   -0.0050,
   -0.0082,
   -0.0128,
   -0.0192,
   -0.0280,
   -0.0388,
   -0.0511,
   -0.0649,
   -0.0791,
   -0.0928,
   -0.1049,
   -0.1154,
   -0.1238,
   -0.1295,
   -0.1327,
   -0.1319,
   -0.1238,
   -0.1019,
   -0.0606,
   -0.0013,
    0.0692,
    0.1419,
    0.2068,
    0.2505,
    0.2606,
    0.2381,
    0.1952,
    0.1447,
    0.0955,
    0.0545,
    0.0279,
    0.0158,
    0.0127,
    0.0135,
    0.0161,
    0.0211,
    0.0288,
    0.0390,
    0.0512,
    0.0639,
    0.0757,
    0.0841,
    0.0876,
    0.0850,
    0.0761,
    0.0615,
    0.0417,
    0.0175,
   -0.0103,
   -0.0403,
   -0.0711,
   -0.1004,
   -0.1256,
   -0.1436,
   -0.1517,
   -0.1490,
   -0.1368,
   -0.1179,
   -0.0949,
   -0.0711,
   -0.0488,
   -0.0294,
   -0.0130,
    0.0018,
    0.0153,
    0.0277,
    0.0381,
    0.0458,
    0.0503,
    0.0502,
    0.0456,
    0.0372,
    0.0268,
    0.0161,
    0.0064,
   -0.0009,
   -0.0060,
   -0.0085,
   -0.0090,
   -0.0084,
   -0.0076,
   -0.0068,
   -0.0057,
   -0.0047,
   -0.0038,
   -0.0027,
   -0.0015,
   -0.0005,
   -0.0001,
    0.0001,
   -0.0004,
   -0.0014,
   -0.0029,
   -0.0044,
   -0.0057,
   -0.0071,
   -0.0079,
   -0.0085,
   -0.0089,
   -0.0090,
   -0.0091,
   -0.0090,
   -0.0091,
   -0.0091,
   -0.0089,
   -0.0088,
   -0.0086,
   -0.0086,
   -0.0082,
   -0.0081,
   -0.0084,
   -0.0086,
   -0.0086,
   -0.0087,
   -0.0092,
   -0.0091,
   -0.0087,
   -0.0084,
   -0.0081,
   -0.0075,
   -0.0063,
   -0.0053,
   -0.0038,
   -0.0021,
    0.0000,
    0.0024,
    0.0047,
    0.0068,
    0.0088,
    0.0105,
    0.0119,
    0.0129,
    0.0136,
    0.0141,
    0.0141,
    0.0139,
    0.0133,
    0.0125,
    0.0114,
    0.0105,
    0.0096,
    0.0088,
    0.0086,
    0.0087,
    0.0093,
    0.0097,
    0.0106,
    0.0113,
    0.0115,
    0.0119,
    0.0118,
    0.0119,
    0.0116,
    0.0110,
    0.0103,
    0.0088,
    0.0071,
    0.0048,
    0.0024,
    0.0003,
   -0.0017,
   -0.0031,
   -0.0041,
   -0.0046,
   -0.0052,
   -0.0058,
   -0.0064,
   -0.0069,
   -0.0073,
   -0.0077,
   -0.0073,
   -0.0064,
   -0.0055,
   -0.0047,
   -0.0040,
   -0.0034,
   -0.0031,
   -0.0029,
   -0.0025,
   -0.0016,
   -0.0005,
    0.0006,
    0.0017,
    0.0024,
    0.0027,
    0.0025,
    0.0017,
    0.0009,
   -0.0002,
   -0.0012,
   -0.0020,
   -0.0025,
   -0.0027,
   -0.0027,
   -0.0020,
   -0.0009,
    0.0001,
    0.0008,
    0.0010,
    0.0005,
   -0.0005,
   -0.0017,
   -0.0024,
   -0.0024,
   -0.0012,
    0.0010,
    0.0037,
    0.0061,
    0.0078,
    0.0086,
    0.0082,
    0.0066,
    0.0045,
    0.0022,
    0.0003,
   -0.0010,
   -0.0016,
   -0.0015,
   -0.0013,
   -0.0012,
   -0.0013,
   -0.0017,
   -0.0022,
   -0.0026,
   -0.0026,
   -0.0017,
   -0.0009,
    0.0000,
    0.0006,
    0.0007,
    0.0001,
   -0.0013,
   -0.0024,
   -0.0036,
   -0.0042,
   -0.0044,
   -0.0040,
   -0.0031,
   -0.0024,
   -0.0018,
   -0.0016,
   -0.0016,
   -0.0017,
   -0.0018,
   -0.0017,
   -0.0009,
   -0.0000,
    0.0008,
    0.0015,
    0.0018,
    0.0017,
    0.0011,
    0.0007,
    0.0001,
   -0.0004,
   -0.0005,
   -0.0005,
   -0.0004,
   -0.0005,
   -0.0004,
   -0.0002,
   -0.0002,
   -0.0002,
   -0.0001,
    0.0004,
    0.0008,
    0.0010,
    0.0013,
    0.0012,
    0.0008,
   -0.0003,
   -0.0013,
   -0.0023,
   -0.0035,
   -0.0043,
   -0.0053,
   -0.0057,
   -0.0062,
   -0.0064,
   -0.0061,
   -0.0053,
   -0.0038,
   -0.0020,
    0.0003,
    0.0027,
    0.0045,
    0.0054,
    0.0054,
    0.0048,
    0.0030,
    0.0004,
   -0.0028,
   -0.0064,
   -0.0108,
   -0.0172,
   -0.0253,
   -0.0354,
   -0.0476,
   -0.0616,
   -0.0764,
   -0.0901,
   -0.1024,
   -0.1127,
   -0.1210,
   -0.1267,
   -0.1299,
   };

	ECGSignalChannel mockedSignal;
	int length = 1000;
	mockedSignal->signal = gsl_vector_alloc(length);

	for(int i = 0; i < length; i++)
	{
		gsl_vector_set(mockedSignal->signal, i, signal[i]);
	}

	return mockedSignal;
}  


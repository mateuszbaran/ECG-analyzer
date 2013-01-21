#include "RPeaksDetector.h"
#include<QDebug>
#include "tri_logger.hpp"
#define LOG_END TRI_LOG_STR("END: " << __FUNCTION__);

RPeaksDetector::RPeaksDetector()
{
	this->detectionMethod = PAN_TOMPKINS;
	this->panTompkinsMovinghWindowLenght = 0;
	this->panTompkinsThersold = 0;
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
	#ifdef DEBUG
		qDebug() << "R peaks module started";
	#endif
	try{
		this->filteredSignal = filteredSignal;
		this->signalFrequency = ecgi.channel_one.frequecy;
		#ifdef USE_MOCKED_SIGNAL
			qDebug() << "Use mocked signal for R-peaks module.";
			this->filteredSignal = getMockedSignal();
		#endif
		this->rsPositions = &ecgRs;
	
		bool success = this->detectRPeaks();
		if(!success)
		{
			#ifdef DEBUG
				qDebug() << "R peaks detection module fails";
			#endif
			throw new RPeaksDetectionException("R peaks detection method fails");
		}
	} catch (...) {
		#ifdef DEBUG
			qDebug() << "R peaks detection module fails";
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
			qDebug() << "Window size not found, use automatic calculated value";
		#endif
	}

	if(parameterTypes.find("threshold_size") != parameterTypes.end())
	{
		panTompkinsMovinghWindowLenght = parameterTypes["threshold_size"];
	}
	else {
		#ifdef DEBUG
			qDebug() << "Thersold size not found, use automatic calculated value";
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

	//Calculae moving window lenght or use custom value
	// N=30 for f=200Hz - from literature
	// N=24 for f=360Hz - from literature and tests
	// Linear function for calculating window lenght
	// wl = -0.0375 * fs + 37.5
	if(panTompkinsMovinghWindowLenght == 0)
	{
		panTompkinsMovinghWindowLenght = -0.0375 * signalFrequency + 37.5;
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
		threshold = sigMaxVal * 0.25;
	}
	else
	{
		threshold = this->panTompkinsThersold;
	}

	//Looking for points over thersold
	#ifdef DEBUG
		qDebug() << "Current thresold value: " << threshold << endl << "Looking for points over thersold";
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

	ECGSignalChannel sig = *signal;

	if(sig->signal->size == NULL || sig->signal->size == 0)
	{
		#ifdef DEBUG
				qDebug() << "Input signal size is 0";
		#endif
		TRI_LOG_STR("Input signal size is 0");
		return false;
	}
	int n = sig->signal->size;

	std::vector<double> sygnal(n);
	int i = 0;

	for ( ; i < n ; ++i)
		sygnal[i] = gsl_vector_get (sig->signal, i);

	zmien_sygnal_na_abs(sygnal);

	std::vector<int> pozycje;
	int czestotliwosc = 360;

	detekcja_r_hilbert(sygnal, czestotliwosc, pozycje);

	IntSignal rs;
	rs = IntSignal(new WrappedVectorInt);

	n = pozycje.size();

	rs->signal = gsl_vector_int_alloc(n);

	for (i = 0; i < n; ++i)
		gsl_vector_int_set(rs->signal, i, pozycje.at(i));

	rsPositions->setRs(rs);

	#ifdef DEBUG
		qDebug() << "Number of detected R-peaks:" << n;
	#endif

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
	myfile.open("mocks/FilteredSignal.txt");
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



// ----------------------------------------------------------------------------------------------

/*
Copyright (c) 2003-2010, Mark Borgerding

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the author nor the names of any contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/* The guts header contains all the multiplication and addition macros that are defined for
fixed or floating point complex numbers.  It also delares the kf_ internal functions.
*/

static void kf_bfly2(
	kiss_fft_cpx * Fout,
	const size_t fstride,
	const kiss_fft_cfg st,
	int m
	)
{
	kiss_fft_cpx * Fout2;
	kiss_fft_cpx * tw1 = st->twiddles;
	kiss_fft_cpx t;
	Fout2 = Fout + m;
	do{
		C_FIXDIV(*Fout,2); C_FIXDIV(*Fout2,2);

		C_MUL (t,  *Fout2 , *tw1);
		tw1 += fstride;
		C_SUB( *Fout2 ,  *Fout , t );
		C_ADDTO( *Fout ,  t );
		++Fout2;
		++Fout;
	}while (--m);
}

static void kf_bfly4(
	kiss_fft_cpx * Fout,
	const size_t fstride,
	const kiss_fft_cfg st,
	const size_t m
	)
{
	kiss_fft_cpx *tw1,*tw2,*tw3;
	kiss_fft_cpx scratch[6];
	size_t k=m;
	const size_t m2=2*m;
	const size_t m3=3*m;


	tw3 = tw2 = tw1 = st->twiddles;

	do {
		C_FIXDIV(*Fout,4); C_FIXDIV(Fout[m],4); C_FIXDIV(Fout[m2],4); C_FIXDIV(Fout[m3],4);

		C_MUL(scratch[0],Fout[m] , *tw1 );
		C_MUL(scratch[1],Fout[m2] , *tw2 );
		C_MUL(scratch[2],Fout[m3] , *tw3 );

		C_SUB( scratch[5] , *Fout, scratch[1] );
		C_ADDTO(*Fout, scratch[1]);
		C_ADD( scratch[3] , scratch[0] , scratch[2] );
		C_SUB( scratch[4] , scratch[0] , scratch[2] );
		C_SUB( Fout[m2], *Fout, scratch[3] );
		tw1 += fstride;
		tw2 += fstride*2;
		tw3 += fstride*3;
		C_ADDTO( *Fout , scratch[3] );

		if(st->inverse) {
			Fout[m].r = scratch[5].r - scratch[4].i;
			Fout[m].i = scratch[5].i + scratch[4].r;
			Fout[m3].r = scratch[5].r + scratch[4].i;
			Fout[m3].i = scratch[5].i - scratch[4].r;
		}else{
			Fout[m].r = scratch[5].r + scratch[4].i;
			Fout[m].i = scratch[5].i - scratch[4].r;
			Fout[m3].r = scratch[5].r - scratch[4].i;
			Fout[m3].i = scratch[5].i + scratch[4].r;
		}
		++Fout;
	}while(--k);
}

static void kf_bfly3(
	kiss_fft_cpx * Fout,
	const size_t fstride,
	const kiss_fft_cfg st,
	size_t m
	)
{
	size_t k=m;
	const size_t m2 = 2*m;
	kiss_fft_cpx *tw1,*tw2;
	kiss_fft_cpx scratch[5];
	kiss_fft_cpx epi3;
	epi3 = st->twiddles[fstride*m];

	tw1=tw2=st->twiddles;

	do{
		C_FIXDIV(*Fout,3); C_FIXDIV(Fout[m],3); C_FIXDIV(Fout[m2],3);

		C_MUL(scratch[1],Fout[m] , *tw1);
		C_MUL(scratch[2],Fout[m2] , *tw2);

		C_ADD(scratch[3],scratch[1],scratch[2]);
		C_SUB(scratch[0],scratch[1],scratch[2]);
		tw1 += fstride;
		tw2 += fstride*2;

		Fout[m].r = Fout->r - HALF_OF(scratch[3].r);
		Fout[m].i = Fout->i - HALF_OF(scratch[3].i);

		C_MULBYSCALAR( scratch[0] , epi3.i );

		C_ADDTO(*Fout,scratch[3]);

		Fout[m2].r = Fout[m].r + scratch[0].i;
		Fout[m2].i = Fout[m].i - scratch[0].r;

		Fout[m].r -= scratch[0].i;
		Fout[m].i += scratch[0].r;

		++Fout;
	}while(--k);
}

static void kf_bfly5(
	kiss_fft_cpx * Fout,
	const size_t fstride,
	const kiss_fft_cfg st,
	int m
	)
{
	kiss_fft_cpx *Fout0,*Fout1,*Fout2,*Fout3,*Fout4;
	int u;
	kiss_fft_cpx scratch[13];
	kiss_fft_cpx * twiddles = st->twiddles;
	kiss_fft_cpx *tw;
	kiss_fft_cpx ya,yb;
	ya = twiddles[fstride*m];
	yb = twiddles[fstride*2*m];

	Fout0=Fout;
	Fout1=Fout0+m;
	Fout2=Fout0+2*m;
	Fout3=Fout0+3*m;
	Fout4=Fout0+4*m;

	tw=st->twiddles;
	for ( u=0; u<m; ++u ) {
		C_FIXDIV( *Fout0,5); C_FIXDIV( *Fout1,5); C_FIXDIV( *Fout2,5); C_FIXDIV( *Fout3,5); C_FIXDIV( *Fout4,5);
		scratch[0] = *Fout0;

		C_MUL(scratch[1] ,*Fout1, tw[u*fstride]);
		C_MUL(scratch[2] ,*Fout2, tw[2*u*fstride]);
		C_MUL(scratch[3] ,*Fout3, tw[3*u*fstride]);
		C_MUL(scratch[4] ,*Fout4, tw[4*u*fstride]);

		C_ADD( scratch[7],scratch[1],scratch[4]);
		C_SUB( scratch[10],scratch[1],scratch[4]);
		C_ADD( scratch[8],scratch[2],scratch[3]);
		C_SUB( scratch[9],scratch[2],scratch[3]);

		Fout0->r += scratch[7].r + scratch[8].r;
		Fout0->i += scratch[7].i + scratch[8].i;

		scratch[5].r = scratch[0].r + S_MUL(scratch[7].r,ya.r) + S_MUL(scratch[8].r,yb.r);
		scratch[5].i = scratch[0].i + S_MUL(scratch[7].i,ya.r) + S_MUL(scratch[8].i,yb.r);

		scratch[6].r =  S_MUL(scratch[10].i,ya.i) + S_MUL(scratch[9].i,yb.i);
		scratch[6].i = -S_MUL(scratch[10].r,ya.i) - S_MUL(scratch[9].r,yb.i);

		C_SUB(*Fout1,scratch[5],scratch[6]);
		C_ADD(*Fout4,scratch[5],scratch[6]);

		scratch[11].r = scratch[0].r + S_MUL(scratch[7].r,yb.r) + S_MUL(scratch[8].r,ya.r);
		scratch[11].i = scratch[0].i + S_MUL(scratch[7].i,yb.r) + S_MUL(scratch[8].i,ya.r);
		scratch[12].r = - S_MUL(scratch[10].i,yb.i) + S_MUL(scratch[9].i,ya.i);
		scratch[12].i = S_MUL(scratch[10].r,yb.i) - S_MUL(scratch[9].r,ya.i);

		C_ADD(*Fout2,scratch[11],scratch[12]);
		C_SUB(*Fout3,scratch[11],scratch[12]);

		++Fout0;++Fout1;++Fout2;++Fout3;++Fout4;
	}
}

/* perform the butterfly for one stage of a mixed radix FFT */
static void kf_bfly_generic(
	kiss_fft_cpx * Fout,
	const size_t fstride,
	const kiss_fft_cfg st,
	int m,
	int p
	)
{
	int u,k,q1,q;
	kiss_fft_cpx * twiddles = st->twiddles;
	kiss_fft_cpx t;
	int Norig = st->nfft;

	kiss_fft_cpx * scratch = (kiss_fft_cpx*)KISS_FFT_TMP_ALLOC(sizeof(kiss_fft_cpx)*p);

	for ( u=0; u<m; ++u ) {
		k=u;
		for ( q1=0 ; q1<p ; ++q1 ) {
			scratch[q1] = Fout[ k  ];
			C_FIXDIV(scratch[q1],p);
			k += m;
		}

		k=u;
		for ( q1=0 ; q1<p ; ++q1 ) {
			int twidx=0;
			Fout[ k ] = scratch[0];
			for (q=1;q<p;++q ) {
				twidx += fstride * k;
				if (twidx>=Norig) twidx-=Norig;
				C_MUL(t,scratch[q] , twiddles[twidx] );
				C_ADDTO( Fout[ k ] ,t);
			}
			k += m;
		}
	}
	KISS_FFT_TMP_FREE(scratch);
}

static
	void kf_work(
	kiss_fft_cpx * Fout,
	const kiss_fft_cpx * f,
	const size_t fstride,
	int in_stride,
	int * factors,
	const kiss_fft_cfg st
	)
{
	kiss_fft_cpx * Fout_beg=Fout;
	const int p=*factors++; /* the radix  */
	const int m=*factors++; /* stage's fft length/p */
	const kiss_fft_cpx * Fout_end = Fout + p*m;

#ifdef _OPENMP
	// use openmp extensions at the 
	// top-level (not recursive)
	if (fstride==1 && p<=5)
	{
		int k;

		// execute the p different work units in different threads
#       pragma omp parallel for
		for (k=0;k<p;++k) 
			kf_work( Fout +k*m, f+ fstride*in_stride*k,fstride*p,in_stride,factors,st);
		// all threads have joined by this point

		switch (p) {
		case 2: kf_bfly2(Fout,fstride,st,m); break;
		case 3: kf_bfly3(Fout,fstride,st,m); break; 
		case 4: kf_bfly4(Fout,fstride,st,m); break;
		case 5: kf_bfly5(Fout,fstride,st,m); break; 
		default: kf_bfly_generic(Fout,fstride,st,m,p); break;
		}
		return;
	}
#endif

	if (m==1) {
		do{
			*Fout = *f;
			f += fstride*in_stride;
		}while(++Fout != Fout_end );
	}else{
		do{
			// recursive call:
			// DFT of size m*p performed by doing
			// p instances of smaller DFTs of size m, 
			// each one takes a decimated version of the input
			kf_work( Fout , f, fstride*p, in_stride, factors,st);
			f += fstride*in_stride;
		}while( (Fout += m) != Fout_end );
	}

	Fout=Fout_beg;

	// recombine the p smaller DFTs 
	switch (p) {
	case 2: kf_bfly2(Fout,fstride,st,m); break;
	case 3: kf_bfly3(Fout,fstride,st,m); break; 
	case 4: kf_bfly4(Fout,fstride,st,m); break;
	case 5: kf_bfly5(Fout,fstride,st,m); break; 
	default: kf_bfly_generic(Fout,fstride,st,m,p); break;
	}
}

/*  facbuf is populated by p1,m1,p2,m2, ...
where 
p[i] * m[i] = m[i-1]
m0 = n                  */
static 
	void kf_factor(int n,int * facbuf)
{
	int p=4;
	double floor_sqrt;
	floor_sqrt = floor( sqrt((double)n) );

	/*factor out powers of 4, powers of 2, then any remaining primes */
	do {
		while (n % p) {
			switch (p) {
			case 4: p = 2; break;
			case 2: p = 3; break;
			default: p += 2; break;
			}
			if (p > floor_sqrt)
				p = n;          /* no more factors, skip to end */
		}
		n /= p;
		*facbuf++ = p;
		*facbuf++ = n;
	} while (n > 1);
}

/*
*
* User-callable function to allocate all necessary storage space for the fft.
*
* The return value is a contiguous block of memory, allocated with malloc.  As such,
* It can be freed with free(), rather than a kiss_fft-specific function.
* */
kiss_fft_cfg kiss_fft_alloc(int nfft,int inverse_fft,void * mem,size_t * lenmem )
{
	kiss_fft_cfg st=NULL;
	size_t memneeded = sizeof(struct kiss_fft_state)
		+ sizeof(kiss_fft_cpx)*(nfft-1); /* twiddle factors*/

	if ( lenmem==NULL ) {
		st = ( kiss_fft_cfg)KISS_FFT_MALLOC( memneeded );
	}else{
		if (mem != NULL && *lenmem >= memneeded)
			st = (kiss_fft_cfg)mem;
		*lenmem = memneeded;
	}
	if (st) {
		int i;
		st->nfft=nfft;
		st->inverse = inverse_fft;

		for (i=0;i<nfft;++i) {
			const double pi=3.141592653589793238462643383279502884197169399375105820974944;
			double phase = -2*pi*i / nfft;
			if (st->inverse)
				phase *= -1;
			kf_cexp(st->twiddles+i, phase );
		}

		kf_factor(nfft,st->factors);
	}
	return st;
}


void kiss_fft_stride(kiss_fft_cfg st,const kiss_fft_cpx *fin,kiss_fft_cpx *fout,int in_stride)
{
	if (fin == fout) {
		//NOTE: this is not really an in-place FFT algorithm.
		//It just performs an out-of-place FFT into a temp buffer
		kiss_fft_cpx * tmpbuf = (kiss_fft_cpx*)KISS_FFT_TMP_ALLOC( sizeof(kiss_fft_cpx)*st->nfft);
		kf_work(tmpbuf,fin,1,in_stride, st->factors,st);
		memcpy(fout,tmpbuf,sizeof(kiss_fft_cpx)*st->nfft);
		KISS_FFT_TMP_FREE(tmpbuf);
	}else{
		kf_work( fout, fin, 1,in_stride, st->factors,st );
	}
}

void kiss_fft(kiss_fft_cfg cfg,const kiss_fft_cpx *fin,kiss_fft_cpx *fout)
{
	kiss_fft_stride(cfg,fin,fout,1);
}


void kiss_fft_cleanup(void)
{
	// nothing needed any more
}

int kiss_fft_next_fast_size(int n)
{
	while(1) {
		int m=n;
		while ( (m%2) == 0 ) m/=2;
		while ( (m%3) == 0 ) m/=3;
		while ( (m%5) == 0 ) m/=5;
		if (m<=1)
			break; /* n is completely factorable by twos, threes, and fives */
		n++;
	}
	return n;
}
// --------------------------------------------------------------------------------------------------

void double2kiss(const std::vector<double> &f, 
	kiss_fft_cpx * g) {
		for (int i=0; i<f.size(); i++) {
			g[i].r = f[i];
			g[i].i = 0.0;
		};
}
void kiss2double(int n, kiss_fft_cpx * f, std::vector<double> &g) {
	g.resize(n);
	for (int i=0; i<n; i++) {
		g[i] = f[i].r;
	};
}

void fft(const std::vector<double> & f, kiss_fft_cpx * g) {
	// z rzeczywistego wektora f utworzy jego transformate g
	kiss_fft_cpx * dane_dla_kiss = new kiss_fft_cpx[f.size()];
	double2kiss(f, dane_dla_kiss);
	kiss_fft_cfg cfg = kiss_fft_alloc(f.size(), false, NULL, 0);
	kiss_fft( cfg, dane_dla_kiss, g);
	kiss_fft_free(cfg);
	kiss_fft_cleanup();
	delete dane_dla_kiss;
};
void ifft(int n, kiss_fft_cpx * f, std::vector<double> & g) {
	// z zespolonego wektora f utworzy jego transofrmatê odwrotn¹, i
	// jej czêœæ rzeczywist¹ wpisze do g
	kiss_fft_cpx * wyniki_kiss = new kiss_fft_cpx[n];
	kiss_fft_cfg cfg = kiss_fft_alloc(n, true, NULL, 0);
	kiss_fft( cfg, f, wyniki_kiss);
	kiss_fft_free(cfg);
	kiss_fft_cleanup();
	kiss2double(n,wyniki_kiss,g);
	delete wyniki_kiss;
};

void przemnoz_przez_transformate_1ppix( int n, kiss_fft_cpx * f ) {
	// pierwsza po³ówka przez j, druga po³ówka przez -j, œrodek
	// przez 0
	f[ 0 ].r = 0; f[ 0 ].i = 0;
	f[ n/2 ].r = 0; f[ n/2 ].i = 0;
	for (int i=1; i< n/2; i++) {
		kiss_fft_cpx nowy;
		nowy.r = f[i].i;
		nowy.i = -f[i].r;
		f[i] = nowy;
	}
	for (int i=n/2+1; i< n; i++) {
		kiss_fft_cpx nowy;
		nowy.r = -f[i].i;
		nowy.i = f[i].r;
		f[i] = nowy;
	}
};


void hilbert(const std::vector<double> & f, std::vector<double> & g) {
	const int n = f.size();
	kiss_fft_cpx * fft_f = new kiss_fft_cpx[n];
	fft ( f, fft_f );
	przemnoz_przez_transformate_1ppix(n,  fft_f );
	ifft ( n, fft_f, g);
	delete fft_f;
	for (int i=0; i<n;i++)
		g[i] /= n; 
}

//---------------------------------------------------------------------------------------------------

// *************************************************************
// funkcja oblicza RMS ci¹gu "w"
// *************************************************************
double oblicz_rms (const std::vector<double> & w) { 
	ASSERT( oblicz_rms, w.size() > 0);
	double s=0;
	for (int i=0; i<w.size(); i++)
		s += w[i]*w[i];
	return sqrt(s/w.size());
}       

// *************************************************************
// funkcja oblicza maksimum z abs(w)
// *************************************************************
double oblicz_max_abs (const std::vector<double> & w) { 
	double mx=w[1];
	for (int i=1; i<w.size(); i++) {
		double wart = fabs(w[i]);
		if (wart > mx)
			mx = wart;
	}
	return mx;
}       


// sygnal - dany sygnal
// czestotliwosc - jego czetotliwosc
// y - wektor w którym umieszczone s¹ wyniki dzia³ania funkcji
// *************************************************************
// funkcja oblicza pochodn¹ sygna³u o zadanej czêtotliwoœci
// *************************************************************
void rozniczkuj(const std::vector<double> &sygnal, int czestotliwosc, 
	std::vector<double> &y) { 
		y.resize( sygnal.size() );
		double s = (sygnal[0] + sygnal[ sygnal.size() - 1 ])/2;
		double dt = 2.0/czestotliwosc;

		y[0] = (sygnal[1]-s)/dt;
		y[ y.size()-1 ] = (s- sygnal[ y.size() - 2]) /dt;

		for (int i=1; i< y.size() - 1; i++)
			y[i] = (sygnal[i+1] - sygnal[i-1]) / dt;
};


// *******************************************************************
// **   oblicza œredni¹ wartoœæ wektora
// *******************************************************************
double oblicz_srednia(const std::vector<double> & w) {
	double suma = 0.0;
	for (int i=0; i< w.size(); i++)
		suma += w[i];

	return suma / w.size();
}

// sygnal - zmieniany sygnal
// *****************************************************************
//   fukcja odejmuje od sygnalu jego sredni¹ wartoœæ a nastêpnie 
//   ka¿dy element wektoa zamienia na jego wartoœæ bezwzglêdn¹.
//   Po co to? Ano po to, by póŸnejsza naliza transformacji pochodnej 
//   bra³a pod uwagê tak¿e "dolne" R-peaki
// *****************************************************************
void zmien_sygnal_na_abs(std::vector<double> &s) {
	double srednia = oblicz_srednia(s);
	for (int i=0; i<s.size(); i++)
		s[i] = fabs(s[i] - srednia);
}

//-------------------------------------------------------------------------------------------------------




namespace parametry_hilbert {

	using namespace std;
	// rozmiar odcinka dla którego przeprowadzona jest
	// transformacja hilberta (w artykule 1024), bierz potêgê 2
	const unsigned int rozmiar_okna = 1024; // 1024;
	// promieñ s¹siedztwa (w milisekundach) do przezukiwania w
	// okolicy punktu podejrzanego (w artykule 1/36)
	const double sasiedztwo = 2.0/36.0;
	// odleg³oœæ czasowa miêdzy punktami w których wykryto R-peaki,
	// któr¹ uznajemy za nieprawdopodobn¹ i powoduje ona, z
	// zdecyujemy siê na jeden tylko z tych punktów
	const double  za_bliskie = 1.0/5.0;
	// je¿eli mamy dwa punkty R, które s¹ za bliskie, to mo¿emy
	// chcieæ wybraæ ten, który lepiej pasuje do œrednich odleg³oœci
	// "dotychczasowych", ale do tego trzeba jakiejœ sensownej
	// liczby poprzednio wykrytych punktów; poni¿sza liczba to
	// w³aœnie taka liczba
	const int min_liczba_do_sr_odl = 10; // absolutnie powy¿ej 3, ale 
	// lepiej wiecej, moze 10
	// je¿eli mamy dwa zbyt bliskie sobie punkty R, i chcemy wybraæ
	// ten w³aœciwy, to o ile to mo¿liwe) obliczamy œredni¹ z
	// odleg³oœci pomêdzy ju¿ odkrtymi punktami R i badamy jaka
	// by³aby ró¿nica miêdzy odgleg³oœci¹ pierwszego z tych spornych
	// punktów do ostatniego "zatwierdzonego" a t¹ œredni¹. Oznaczmy
	// ta wielkoœæ R1. Analogicznie obliczymy R2. Teraz, je¿eli R1
	// stanowi ustalony procent R2 (w³aœnie procent ustalony
	// poni¿ej) to za w³aœciwy przyjmujê punkt pierwszy. Je¿eli
	// natomiast R2 stanowi ustalony procent R1, to za w³aœciwy
	// przymujemy punkt drugi. W pozosta³ych przypadkach roztrzyga
	// to, w którym punkcie wartoœæ ygna³u góruje. No, chyba, ze
	// wartoœæ ta bêdzie identyczna. Wtedy wracamy do porównywania
	// odleg³oœci (ju¿ bez zabawy w procenty). Jeœ³i i tu bêdzie
	// identycznie - przyjmujemy pierwszy. 	
	const double procent_mniejszej_odleglosci = 0.8;
	// je¿eli dla danego odcinka obliczê transformatê hilberta, to
	// rozwa¿am stosunek RMS ci¹gu do MAX tego ci¹gu. Je¿eli ten
	// stounek jest poni¿ej tego progu - ci¹g ten ma ma³y szumm. W
	// ARTYKULE by³a to wielkoœæ 0.18
	const double prog_stosunku_rms_do_max = 0.18;
	// w niektórych sytuacjach próg do filtrowania wyznacza siê
	// w proporcji do MAX. poni¿sza sta³a ustala t¹ proporcjê
	const double stosunek_prog_do_max = 0.39;
	// w niektórych sytuacjach próg do filtrowania wyznacza siê
	// w proporcji do RMS. poni¿sza sta³a ustala t¹ proporcjê
	const double stosunek_prog_do_rms = 2.6;
	// gdy porównujê ze sob¹ dwa zbyt bliskie piki (bliskie w sensie
	// czasowym) to najpierw sprawdzê, czy sa mniej wiêcej równe). A
	// mniej wiêcej równe bêdzie znaczy³o, ¿e ró¿nica miêdzy nim
	// stanowi malutki procent ich œrednije wartoœci, poni¿szy
	// parametr ten w³aœnie procent definiuje
	const double zaniedbywalna_roznica = 0.02;
};


#include <cmath>
//***************************************************************
// ** stwierdza czy liczby s¹ "prawie równe" (przy porównywaniu
// ** podejrzanie bliskich szczytów
//***************************************************************
bool prawie_rowne(double x, double y) {
	double s = (x+y)/2;
	double blad = fabs(x-y)/s;
	// [TODO] Check that blad is infinite number
	// bool isInfinite = std::numeric_limits<double>::infinity() == blad;
	// ASSERT(prawie_rowne, isInfinite);
	return blad<parametry_hilbert::zaniedbywalna_roznica;
}

double round( double liczba)
{
	if( liczba - floor(liczba) < 0.5 )
	{
		return floor(liczba);
	} else
	{
		return ceil(liczba);
	}
}


// wyszukuje nr najwiekszego sygnalu w otoczeniu miejsca "nr", probka o
// czestotliwoœci "czestotliwoœæ" (do wyznaczania sasiedztwa)
int wyszukaj_max( int nr, const std::vector<double> &sygnal, int czestotliwosc) { 
	ASSERT(wyszukaj_max, (nr<sygnal.size()) );
	ASSERT(wyszukaj_max, (nr >=0 ));
	int sasiedztwo = round(czestotliwosc * parametry_hilbert::sasiedztwo);
	int start = std::max(0, nr-sasiedztwo);
	int stop = std::min((int)sygnal.size()-1, nr+sasiedztwo);
	int max_nr = start;
	for (int i=start+1; i<= stop; i++)
		if (sygnal[i] > sygnal[max_nr])
			max_nr = i;
	return max_nr;
};

// czestotliwosc 	- czestotliwosc probki
// sygnal		- badany sygnal
// propozycje		- lisa punktow podejrzanych o bycie R punktami
// ********************************************************************
//    funkcja funkcja dla kazdego z punktow sprawdza maksimum w jego
//    okolicy i zatepuje go przez prawdziwe maksimum; ewentualne punkty
//    z tego samego sasiedztwa na tej liscie -- usuwa
// ********************************************************************
void rzeczywiste_maksima(int czestotliwosc, const std::vector<double> &sygnal, 
	std::list<int> &propozycje) {  
		for (std::list<int>::iterator p=propozycje.begin();
			p != propozycje.end(); p++) {
				int nr = wyszukaj_max( *p, sygnal , czestotliwosc);
				*p = nr;
		}
		propozycje.unique();	
}


// **********************************************************************
//     do wektora "wykryte" dopisuje elementy listy "propozycje"
// **********************************************************************
void dopisz_do_wykrytych( const std::list<int> &propozycje,
	std::vector<int> &wykryte) {    
		for (std::list<int>::const_iterator i = propozycje.begin(); i!= propozycje.end(); i++) {
			wykryte.push_back( *i );
		}
};






// *************************************************************
// funkcja do wektora "propozycje" wpisze numery
// tych elementów z wektora h, które s¹ wiêksze ni¿ "threshold"
// przy dopisywaniu powiêksza dopisywany numer o liczbê "zalatwione"
// *************************************************************
void wyfiltruj_propozycje(int czestotliwosc, const std::vector<double> &h, 
	double threshold, 
	std::list<int> & propozycje,
	int przesuniecie) {   
		propozycje.resize(0);
		const int za_bliskie = round( parametry_hilbert::za_bliskie * czestotliwosc );
		for (int i=0+za_bliskie; i<h.size()-za_bliskie; i++)
			if (h[i] > threshold)
				propozycje.push_back(i+przesuniecie);
}



// czestotliwosc - liczba próbek w sekundzie
// sygnal - sygnal dla którego wyszukujemy R-peaków
// wykryte - wektor numerów wykrytych maksimów, które pasuj¹ do punktów
// wskazanych przez du¿e wartoœci transformaty hilberta
// R - wektro do którego wpiszemy wszystkie z wykrytych, no chyba, ¿e
// bêd¹ któreœ zbyt blisko siebie, wtey (z tych bliskich) wybierzemy
// tylko ten "w³aœciwy"
void usun_zbyt_bliskie(int czestotliwosc, const std::vector<double> & sygnal,
	const std::vector<int> &wykryte, 
	std::vector<int> & R) { 
		R.resize(0);
		if (wykryte.size() == 0)
			return;
		R.push_back(wykryte[0]);
		ASSERT(usun_zbyt_bliskie, wykryte[0] < sygnal.size() );

		const int za_bliskie = 
			round(parametry_hilbert::za_bliskie * czestotliwosc);

		for (int i=1; i<wykryte.size(); i++) {
			ASSERT(usun_zbyt_bliskie, wykryte[i] < sygnal.size() );
			if ((sygnal.size()-wykryte[i]) <= za_bliskie)
				break;
			if (wykryte[i] - R.back() <= za_bliskie ) {
				// sa zbyt blisko;
				bool rozstrzygniete = false;
				if (prawie_rowne(sygnal[R.back()],sygnal[wykryte[i]])) {
					if (R.size() >= parametry_hilbert::min_liczba_do_sr_odl) {
						double srednia = (R.back() - R.front())/
							double(R.size() - 1);
						int odl1 = R[R.size()-1]-R[R.size()-2];
						int odl2 = wykryte[i] - R[R.size()-2];

						double roz1 = fabs(odl1 - srednia);
						double roz2 = fabs(odl2 - srednia);
						if (roz1 < parametry_hilbert::procent_mniejszej_odleglosci*roz2)  {
							// bierzemy 1;
							rozstrzygniete = true;
						}

						if (roz2 < parametry_hilbert::procent_mniejszej_odleglosci*roz1)  {
							// bierzemy 2;
							rozstrzygniete = true;
							R.back() = wykryte[i];
						}
						if (!rozstrzygniete) {
							if (sygnal[R.back()] > sygnal[wykryte[i]])
								// pierwszy
								;
							if (sygnal[R.back()] < sygnal[wykryte[i]])
								// drugi
								R.back() = wykryte[i];
							if (sygnal[R.back()] == sygnal[wykryte[i]]) {
								// jednak wg
								// bliskosci
								if (roz2 < roz1)
									R.back() = wykryte[i];
							};
							rozstrzygniete = true;
						}
					};
				};
				if (!rozstrzygniete) {
					// s¹ za blisko, ale nie mamy jeszcze
					// œrednich odleg³oœci
					if (sygnal[R.back()] < sygnal[wykryte[i]])
						R.back() = wykryte[i];
				}
			} else {
				R.push_back(wykryte[i]);
			}
		}
}	




// sygnal - to przefiltrowany, gotowy do dalszej obrobki sygnal
// y - zró¿niczkowany sygnal
// czestootliwosc - ile probek odpowiada jednej sekundzie
// wykryte - wekto numerow probek oznaczajacych miejsca wykrycia probki
// *************************************************************
// funkcja przetwarza jedno okno zrozniczkowanego sygnalu
// *************************************************************
void rozwaz_okno(const std::vector<double> &sygnal,
	int start, int rozmiar, 
	const std::vector<double> &y,
	int czestotliwosc,	
	std::vector<int> &wykryte) {

		static double poprzedni_max_abs = -1;
		// wartoœæ max|y| dla poprzedniego segmentu, przechowywana z
		// wywo³ania na wywo³anie 

		std::vector<double> odcinek( y.begin()+start,
			y.begin()+start+rozmiar );
		std::vector<double> h;
		hilbert(odcinek, h);



		double rms = oblicz_rms(h);
		double max_abs = oblicz_max_abs(h);
		double threshold;

		if (rms >= parametry_hilbert::prog_stosunku_rms_do_max*max_abs)
			threshold = parametry_hilbert::stosunek_prog_do_max*max_abs;
		else
			threshold = parametry_hilbert::stosunek_prog_do_rms*rms;
		if (poprzedni_max_abs != -1 ) // by³o ju¿ poprzednie okno
			if (max_abs > 2*poprzedni_max_abs)
				threshold = std::max(parametry_hilbert::stosunek_prog_do_max*poprzedni_max_abs, threshold);
		poprzedni_max_abs = max_abs;

		std::list<int> propozycje;
		wyfiltruj_propozycje(czestotliwosc,h, threshold, propozycje, start);
		rzeczywiste_maksima(czestotliwosc, sygnal, propozycje);
		dopisz_do_wykrytych(propozycje, wykryte);
};



// sygnal - to przefiltrowany, gotowy do dalszej obrobki sygnal
// czestootliwosc - ile probek odpowiada jednej sekundzie
// wykryte - wekto numerow probek oznaczajacych miejsca wykrycia probki
// *************************************************************
// funkcja organizuje ca³y algorytm detekcji R
// *************************************************************
void detekcja_r_hilbert(const std::vector<double> & sygnal,
	int czestotliwosc,
	std::vector<int> & numery_R) { 
		const int za_bliskie = round(parametry_hilbert::za_bliskie * czestotliwosc);
		std::vector<int> wykryte;
		int zalatwione = 0;
		std::vector<double> y;
		rozniczkuj(sygnal, czestotliwosc, y);	
		while (zalatwione < sygnal.size() ) {
			zalatwione = std::max(0,zalatwione-za_bliskie);
			int rozmiar = std::min(parametry_hilbert::rozmiar_okna, sygnal.size()-zalatwione);
			int stara_liczba_wykrytych = wykryte.size();
			rozwaz_okno(sygnal, zalatwione, rozmiar, y, 
				czestotliwosc, wykryte);
			bool znalezione = wykryte.size() > stara_liczba_wykrytych;
			// czy zosta³ wykryty za³amek R
			// w ostatniej próbce?
			if ((rozmiar < parametry_hilbert::rozmiar_okna) || !znalezione) 
				zalatwione += rozmiar;
			else 
				zalatwione = wykryte.back();
		}
		//sort(wykryte.begin(), wykryte.end());
		usun_zbyt_bliskie(czestotliwosc, sygnal, wykryte, numery_R);
}

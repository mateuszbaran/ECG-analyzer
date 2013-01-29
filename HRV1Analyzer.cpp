#include "HRV1Analyzer.h"

#ifndef DEV // 4 testing
    #include <QDebug>
#endif

HRV1Analyzer::HRV1Analyzer() {
    signalSize=0;
    signalSampling=0;
    sizeFftIndex=0;
}

HRV1Analyzer::~HRV1Analyzer() {
	delete[] sig;
	delete[] sigAbsolute;
}

void HRV1Analyzer::runModule(const ECGInfo &info, const ECGRs & r_peaks_data, ECGHRV1 & hrv1_data) {

	this->rPeaksData = r_peaks_data;
	this->hrv1Data = &hrv1_data;
	this->signalSampling = (int) info.channel_one.frequecy;

	prepareSignal();
	prepareSigAbsolute();
	calculateParameters();
}

/**
 * Przeksztalcenie tablicy z numerami probek na tablice zawierajaca zbior interwalow
 * pomiedzy kolejnymi zespolami RR
 */
void HRV1Analyzer::prepareSignal() {

		#ifdef DEBUG
			qDebug() << "Preparing RR signal data.";
		#endif

		 //wlasciwe przygotowanie sygnalu wejsciowego (double *sig)
		#ifdef DEV
            std::cout << "Preparing RR example signal data.\n";

	        ExampleSignal es = ExampleSignal();

	        sig = es.getSignal();
	        this->signalSize = es.getLength();
	        signalSampling = 500;

			std::cout << "ExampleSignal loaded.\n";

		#else
            this->signalSize = (unsigned int) rPeaksData.GetRs()->signal->size*1;
			sig = new double[this->signalSize];

			double scalingValue = (double)(1000.0/signalSampling);
			int v1, v2;

			for(int i = 0; i < this->signalSize-1; i++) {
				v1 = gsl_vector_int_get (rPeaksData.GetRs()->signal, i);
				v2 = gsl_vector_int_get (rPeaksData.GetRs()->signal, i+1);

				sig[i] = (double) abs(v2-v1) * scalingValue;
			}
			//po przygotowaniu wielkosc sygnalu RR ulega zmniejszeniu
			this->signalSize = this->signalSize -1;

		#endif

		#ifdef DEBUG
			qDebug() << "Preparing RR signal data has been completed.";
		#endif

	}

void HRV1Analyzer::prepareSigAbsolute() {
	sigAbsolute = new double[this->signalSize];

	sigAbsolute[0] = sig[0];

	for(int i=1; i<this->signalSize; i++) {
		sigAbsolute[i] = sig[i] + sigAbsolute[i-1];
		#ifdef DEBUG_SIG
			qDebug() << "sig: " << sig[i] << " sigA: " << sigAbsolute[i];
		#endif
	}
	
}

/**
 * Metoda odpowiadajaca za wyliczenie wszystkich parametrow ilosciowych oraz czestotliwosciowych,
 * ktore skladaja sie na analize, ktora ma przeprowadzic modul HRV1
 */
void HRV1Analyzer::calculateParameters() {

	#ifdef DEBUG
		qDebug() << "HRV1 calculateParameters method started";
	#endif

    double temp=0;

	/////////////////////RR_avg
	//OK: accuracy (vs Matlab 2010b) 0.00005

	this->hrv1Data->RR_avg = mean(sig,0,this->signalSize);

	#ifdef DEBUG
		qDebug() << "RR_avg:" << this->hrv1Data->RR_avg;
	#endif

	/////////////////////RR_stddev
    //OK: accuracy 0.006

	this->hrv1Data->RR_stddev = std(sig,0,this->signalSize);

	#ifdef DEBUG
		qDebug() << "RR_stddev:" << this->hrv1Data->RR_stddev;
	#endif


	//////////////////////SDNN
	//OK: accuracy 0.00005

	temp=0;

	for(int i=0; i<this->signalSize; i++) {
		temp += (this->hrv1Data->RR_avg-sig[i])
						*(this->hrv1Data->RR_avg-sig[i]);
	}

	this->hrv1Data->SDNN = sqrt( (double)temp/this->signalSize-1 );

	#ifdef DEBUG
		qDebug() << "SDNN:" << this->hrv1Data->SDNN;
	#endif

	//////////////////////RMSSD
	//OK: accuracy 0.00005

	temp=0;

	for(int i=0; i<this->signalSize-1; i++) {
		temp += (sig[i+1]-sig[i])*(sig[i+1]-sig[i]);
	}

	this->hrv1Data->RMSSD = sqrt( (double)temp/(this->signalSize-1) );

	#ifdef DEBUG
		qDebug() << "RMSSD:" << this->hrv1Data->RMSSD;
	#endif


	//////////////////////NN50
	//OK: accuracy 0.0

	temp=0;
	double thresholdNN50  = 50;

	for(int i=0; i<this->signalSize-1; i++) {
		if (abs(sig[i+1]-sig[i]) > thresholdNN50)
			temp++;
	}

	this->hrv1Data->NN50 = temp;

	#ifdef DEBUG
		qDebug() << "NN50:" << this->hrv1Data->NN50;
	#endif


	/////////////////////pNN50
	//OK: accuracy 0.00005

	this->hrv1Data->pNN50 = this->hrv1Data->NN50*100/(this->signalSize-1);

	#ifdef DEBUG
		qDebug() << "pNN50:" << this->hrv1Data->pNN50;
	#endif

	/////////////////////SDANN & SDANNi
	//SDANN: accuracy 0.5
	//SDANNi OK: accuracy 0.05

	temp=0;

    //temp variables
    int windowSize = 1000*60*5; /* 60 seconds*5minutes */
	long numberOfSteps = std::floor( sigAbsolute[this->signalSize-1]/windowSize );

	//zabezpieczenie na wypadek smieci w ostatniej probce
	if (numberOfSteps < 0) {
		qDebug() << "Dopasowano sygnal!";
		this->signalSize = this->signalSize -1;
		long numberOfSteps = std::floor( sigAbsolute[this->signalSize-1]/windowSize );
	}

    double * mRRI = new double[numberOfSteps];
    double * stdRR5 = new double[numberOfSteps];

    int windowStartTime, windowEndTime, windowStartIndex, windowEndIndex;

    //wyliczanie wartosci w 5 minutowym oknie czasowym
	for(int step=1;step<=numberOfSteps;step++) {
	    windowStartTime = (step-1) * windowSize;
        windowEndTime = step * windowSize;

        windowStartIndex = 0;
        windowEndIndex = 0;

	    for(int i=0; i<this->signalSize; i++) {
	        if(windowStartTime<=sigAbsolute[i]) {
                windowStartIndex=i;
                break;
	        }
	    }
	    for(int i=0; i<this->signalSize; i++) {
	        if(windowEndTime<=sigAbsolute[i]) {
                windowEndIndex=i;
                break;
	        }
	    }

	    //mRRI i stdRR5 w oknie czasowym
	    mRRI[step-1] = mean(sig, windowStartIndex, windowEndIndex+1);
	    stdRR5[step-1] = std(sig, windowStartIndex, windowEndIndex+1);

		#ifdef DEBUG_WINDOWS
			qDebug() << "#windowStartIndex=" << windowStartIndex << " windowEndIndex=" << windowEndIndex;
			qDebug() << "#windowStartTime=" << windowStartTime << " windowEndTime=" << windowEndTime;
			qDebug() << "#mRRI=" << mRRI[step-1] << " stdRR5=" << stdRR5[step-1];
		#endif

	}

	//zapis SDANN i SDANNi
	this->hrv1Data->SDANN = std(mRRI, 0, numberOfSteps);
	this->hrv1Data->SDANN_index = mean(stdRR5, 0, numberOfSteps);

	#ifdef DEBUG
		qDebug() << "SDANN:" << this->hrv1Data->SDANN;
		qDebug() << "SDANNi:" << this->hrv1Data->SDANN_index;
	#endif


	///////////////////SDSD
	//OK: accuracy 0.01

    double* tmpSig = new double[this->signalSize-1];

    for(int i=0; i<this->signalSize-1; i++) {
        tmpSig[i] = sig[i+1] - sig[i];
    }

    this->hrv1Data->SDSD = std(tmpSig,0,this->signalSize-1);

	#ifdef DEBUG
		qDebug() << "SDSD:" << this->hrv1Data->SDSD;
	#endif

    delete[] tmpSig;


	////////////////////FFT

	double* sigAfterSpline = cubicSpline(sigAbsolute, sig, this->signalSize);

	int sigAfterSplineSize = (int)(sigAbsolute[this->signalSize-1]/(FREQUENCY_FFT))-1;

	double* fftMagnitude = doFFT(sigAfterSpline, sigAfterSplineSize);


	double freqq = (double)signalSampling/sigAfterSplineSize;

	//wlasciwa analiza czestotliwosciowa
    for(int i = 1; i< sizeFftIndex; i++) {
        if(0.003>i*freqq) {
            this->hrv1Data->ULF += fftMagnitude[i];
        }
        else if(0.003<i*freqq && 0.04>=i*freqq) {
            this->hrv1Data->VLF += fftMagnitude[i];
        }
        else if(0.04<i*freqq && 0.15>=i*freqq) {
            this->hrv1Data->LF += fftMagnitude[i];
        }
        else if(0.15<i*freqq && 0.4>=i*freqq) {
            this->hrv1Data->HF += fftMagnitude[i];
        }
    }
    this->hrv1Data->TP = this->hrv1Data->ULF + this->hrv1Data->VLF + this->hrv1Data->LF + this->hrv1Data->HF;
    this->hrv1Data->LFHF = this->hrv1Data->LF / this->hrv1Data->HF;
	
	#ifdef DEBUG
		qDebug() << "ULF:" << this->hrv1Data->ULF;
		qDebug() << "VLF:" << this->hrv1Data->VLF;
		qDebug() << "LF:" << this->hrv1Data->LF;
		qDebug() << "HF:" << this->hrv1Data->HF;
		qDebug() << "TP:" << this->hrv1Data->TP;
		qDebug() << "LFHF:" << this->hrv1Data->LFHF;
	#endif

    ////////////////////power & frequency plot

    #ifndef DEV
        this->hrv1Data->freqency = OtherSignal(new WrappedVector);
		this->hrv1Data->freqency->signal = gsl_vector_alloc(sizeFftIndex);
        this->hrv1Data->power = OtherSignal(new WrappedVector);
		this->hrv1Data->power->signal = gsl_vector_alloc(sizeFftIndex);

        for(int i=0; i<sizeFftIndex; i++) {
            this->hrv1Data->freqency->set(i, i);
            this->hrv1Data->power->set(i, fftMagnitude[i]);
        }
	#endif


    //sprzatanie pamieci
	delete[] mRRI;
	delete[] stdRR5;
	delete[] fftMagnitude;
	delete[] sigAfterSpline;
}

#ifndef DEV
    void HRV1Analyzer::setParams(ParametersTypes &parameterTypes) { }
#endif

/**
 * Funkcja przeprowadzajaca analize fft zinterpolowanego sygnalu
 * z wykorzystaniem biblioteki kiss fft
 */
double* HRV1Analyzer::doFFT(double* sigAfterSpline, int size) {

	// ustalenie uwagi na interesujacy nas fragment sygnalu 
	int sizeFftSig = 2000; //nieco ponad 1,6Hz

	#ifdef DEBUG
		qDebug() << "Method doFFT() started";
	#endif

	#ifdef DEV
        std::cout << "Method doFFT() started\n";
    #endif
    
	size = (size>2000)?sizeFftSig:(int)(size*3/4); //nieco ponad 1,6Hz


	kiss_fft_cpx * out_cpx = new kiss_fft_cpx[size], * out = new kiss_fft_cpx[size], *cpx_buf;

	kiss_fft_cfg fft = kiss_fft_alloc(size, false, NULL,0);
	cpx_buf = copycpx(sigAfterSpline,size);

	kiss_fft(fft,cpx_buf, out_cpx);

	//zwalnianie pamieci
	kiss_fft_cleanup();
	free(fft);

	sizeFftIndex = (size+(size%2))/2;
	double * fftMagnitude = new double[sizeFftIndex];

	#ifdef DEV
        std::cout << "fftMagnitude[] length: " << sizeFftIndex << "\n";
  #endif

	for(int i=0; i<sizeFftIndex; i++) {
		fftMagnitude[i] = 2*(out_cpx[i].r*out_cpx[i].r)/(size*size); //=(abs(out_cpx[i].r)/size)*(abs(out_cpx[i].r)/size)
	}

	// wziecie tylko polowy wartosci fft (bo jest symetryczne)
	fftMagnitude[0] = fftMagnitude[0]/2;
	if(size%2==0) {
		fftMagnitude[sizeFftIndex-1] = fftMagnitude[sizeFftIndex-1]/2;
	}

	#ifdef DEBUG_FFT
		for(i=0;i<sizeFftIndex;i++) {
			qDebug() << "fft#"<< i << ": " << fftMagnitude[i];
		}
	#endif

	delete[] out_cpx;
	delete[] out;
	delete cpx_buf;

	#ifdef DEBUG
		qDebug() << "Method doFFT() finished";
	#endif
	#ifdef DEV
		std::cout << "Method doFFT() finished\n";
	#endif

	return fftMagnitude;
}

/**
 * Metoda zamienia tablice double* o dlugosci nframe na strukturze uzywana
 * przez biblioteke kiss fft
 */
kiss_fft_cpx* HRV1Analyzer::copycpx(double *mat, int nframe) {

	int i;
	kiss_fft_cpx *mat2;

	mat2=(kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*nframe);
    kiss_fft_scalar zero;
    memset(&zero,0,sizeof(zero) );

	for(i=0; i<nframe ; i++) {
		mat2[i].r = mat[i];
		mat2[i].i = zero;
	}
	return mat2;
}

/**
 * Interpolacja funkcji o wektorach x i y.
 * Dlugosc wektorow x i y jest taka sama i wynosi nframe
 */
double* HRV1Analyzer::cubicSpline(double* x, double* y, int nframe) {

    alglib::spline1dinterpolant s;
    alglib::ae_int_t natural_bound_type = 2;

	alglib::real_1d_array *rx = new alglib::real_1d_array();
	rx->setcontent(nframe, x);
	alglib::real_1d_array *ry = new alglib::real_1d_array();
	ry->setcontent(nframe, y);

	alglib::spline1dbuildcubic(
                            *const_cast<const alglib::real_1d_array*>(rx),
                            *const_cast<const alglib::real_1d_array*>(ry),
                            s);

	//wlasciwe interpolowanie funkcji
    long sizeAfterSpline = (long)(x[this->signalSize-1]/FREQUENCY_FFT) +1;
	double* sigI = new double[sizeAfterSpline];

	#ifdef DEBUG
        qDebug() << "cubicSpline sizeAfterSpline= " << sizeAfterSpline << "\n";
    #endif

    for(int i=0; i<sizeAfterSpline; i++) {
        sigI[i] = alglib::spline1dcalc(s, i*FREQUENCY_FFT);
    }

    return sigI;
}

/**
 * Oblicza srednia z wartosci przechowywanych w tablicy tab pomiedzy indeksami start i end
 */
double HRV1Analyzer::mean(double *tab, int start, int end) {
    double temp=0;

    for(int i=start; i<end; i++) {
		temp += tab[i];
	}
    return temp/(end-start);
}

/**
 * Oblicza odchylenie standardowe z wartosci przechowywanych w tablicy tab
 * pomiedzy indeksami start i end
 */
double HRV1Analyzer::std(double *tab, int start, int end) {
    double temp=0;
    double avg = mean(tab, start, end);

    for(int i=start; i<end; i++) {
        temp += std::pow( (tab[i]-avg), 2);
    }
    return std::sqrt(temp/(end-start));
}


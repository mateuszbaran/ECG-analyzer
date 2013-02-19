#include "QRSMorphologyDetector.h"
#include <fstream>



double dlugosc(ECGSignalChannel * signal, int forBegin, int forEnd){

	ECGSignalChannel sig;
	sig = *signal;

	double result = 0;

	double tmp1 = gsl_vector_get(sig->signal,forBegin);
	double tmp2;

	for(int i=forBegin+1;i<forEnd;i++){
		tmp2 = tmp1;
		tmp1 = gsl_vector_get(sig->signal,i);
		result += sqrt(1 + (tmp1-tmp2)*(tmp1-tmp2));
	}

	return result;
}

double pole(ECGSignalChannel * signal, int forBegin, int forEnd){

	ECGSignalChannel sig;
	sig = *signal;

	double result = 0;
	double temp;

	for(int i=forBegin;i<forEnd;i++){
		temp = gsl_vector_get(sig->signal,i);
		if (temp > 0 ) result += temp;
		else result -= temp;
	}

	return result;
}

double stosunek(ECGSignalChannel * signal, int forBegin, int forEnd)
{
	ECGSignalChannel sig;
	sig = *signal;

	double tmp1 = 0;
	double tmp2 = 0;

	for(int i=forBegin;i<forEnd;i++){
		if(gsl_vector_get(sig->signal,i)>0) tmp1 += gsl_vector_get(sig->signal,i);
		else tmp2 -= gsl_vector_get(sig->signal,i);
	}
	if (tmp2 == 0) return 0;
	return tmp1/tmp2;
}

double szybkosc(ECGSignalChannel * signal, int forBegin, int forEnd)
{
	ECGSignalChannel sig;
	sig = *signal;

	double tmp1 = 0;
	double tmp2,tmp3=0;

	for(int i=forBegin;i<forEnd-1;i++){
		tmp2 = gsl_vector_get(sig->signal,i+1) - gsl_vector_get(sig->signal,i);
		if (tmp2<0) tmp2 = -tmp2;
		if(tmp2>tmp1) tmp1 = tmp2;
	}
	return tmp1;

	for(int i=forBegin;i<forEnd-1;i++){
		tmp2 = gsl_vector_get(sig->signal,i+1) - gsl_vector_get(sig->signal,i);
		if (tmp2>0.33*tmp1) tmp3++;
	}

	return tmp3/(forEnd-forBegin);
}

QRSMorphologyDetector::QRSMorphologyDetector(void)
{
}

QRSMorphologyDetector::~QRSMorphologyDetector(void)
{
}

void QRSMorphologyDetector::setParams(ParametersTypes &parameterTypes)
{
}

void QRSMorphologyDetector::runModule(const ECGWaves & waves, const ECGSignalChannel & filteredSignal, const ECGInfo & ecgi, QRSClass & qrsClass)
{
	double inputValue = gsl_vector_get (filteredSignal->signal,1);
	this->filteredSignal = filteredSignal;
	this->qrsPosition = waves;
	this->qrsMorphology = &qrsClass;
	gsl_vector_int *v = qrsPosition.GetQRS_onset()->signal;
	int size = int(v->size);
	this->qrsMorphology->setSize(size);

	detectQRSMorphology();
}

bool QRSMorphologyDetector::detectQRSMorphology()
{
	fstream plik("test_qrs.txt", ios::out);
	auto signalSize  = filteredSignal->signal->size;
	gsl_vector_int *QRS_onset = qrsPosition.GetQRS_onset()->signal;
	gsl_vector_int *QRS_end = qrsPosition.GetQRS_end()->signal;
	int size = int(QRS_onset->size);
	IntSignal tmpSig;
	tmpSig = IntSignal(new WrappedVectorInt);
	tmpSig->signal = gsl_vector_int_alloc(size);

	double maxr,minr,mins,maxs,maxv,minv;

	double * rms;
	double * dodatnieDoUjemnych;
	double * szybkosc1;

	rms = (double*) malloc(sizeof(double)*size);
	dodatnieDoUjemnych = (double*) malloc(sizeof(double)*size);
	szybkosc1 = (double*) malloc(sizeof(double)*size);

	int start = gsl_vector_int_get(QRS_onset,0);
	int stop = gsl_vector_int_get(QRS_end,0);
		
	double poleV = pole(&filteredSignal,start,stop);
	double dlugoscV = dlugosc(&filteredSignal,start,stop);
	rms[0] = (dlugoscV/2*sqrt(3.14*poleV))-1;
	minr=rms[0];
	maxr=minr;
	dodatnieDoUjemnych[0] = stosunek(&filteredSignal,start,stop);
	mins=dodatnieDoUjemnych[0];
	maxs=mins;
	szybkosc1[0] = szybkosc(&filteredSignal,start,stop);
	minv=szybkosc1[0];
	maxv=minv;

	for(int i=1;i<size;i++){

		int start = gsl_vector_int_get(QRS_onset,i);
		int stop = gsl_vector_int_get(QRS_end,i);
		
		double poleV = pole(&filteredSignal,start,stop);
		double dlugoscV = dlugosc(&filteredSignal,start,stop);
		rms[i] = (dlugoscV/2*sqrt(3.14*poleV))-1;

		if (rms[i]>maxr) maxr = rms[i];
		if (rms[i]<minr) minr = rms[i];
		
		dodatnieDoUjemnych[i] = stosunek(&filteredSignal,start,stop);

		if (dodatnieDoUjemnych[i]>maxs) maxs = dodatnieDoUjemnych[i];
		if (dodatnieDoUjemnych[i]<mins) mins = dodatnieDoUjemnych[i];

		szybkosc1[i] = szybkosc(&filteredSignal,start,stop);

		if (szybkosc1[i]>maxv) maxv = szybkosc1[i];
		if (szybkosc1[i]<minv) minv = szybkosc1[i];

		//dokonczyc ocene rodzaju pobudzenia

	//	if (rm>65 && rm<80 && szybkoscV>0.075) gsl_vector_int_set(tmpSig->signal,i,SUPRACENTRICULAR);
	//	else gsl_vector_int_set(tmpSig->signal,i,VENTRICULUS);

	}

	double avgr = maxr-minr;
	double avgs = maxs-mins;
	double avgv = maxv-minv;

	for(int i=0;i<size;i++){
		if(rms[i]>=avgr && szybkosc1[i]>=avgv) {
			gsl_vector_int_set(tmpSig->signal,i,CLASS1);
			plik << CLASS1 << '\n';
		}
		else if(rms[i]>=avgr && szybkosc1[i]<avgv) {
			gsl_vector_int_set(tmpSig->signal,i,CLASS2);
			plik << CLASS2 << '\n';
		}
		else if(rms[i]<avgr && szybkosc1[i]>=avgv) {
			gsl_vector_int_set(tmpSig->signal,i,CLASS3);
			plik << CLASS3 << '\n';
		}
		else {
			gsl_vector_int_set(tmpSig->signal,i,CLASS4);
			plik << CLASS4 << '\n';
		}
	}


	qrsMorphology->setQrsMorphology(tmpSig);
	plik.flush();
	plik.close();
	return 1;
}

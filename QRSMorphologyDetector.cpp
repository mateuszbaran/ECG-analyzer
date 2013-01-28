#include "QRSMorphologyDetector.h"
#include <QDebug>


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
	double tmp2;

	for(int i=forBegin;i<forEnd-5;i++){
		tmp2 = gsl_vector_get(sig->signal,i+5) - gsl_vector_get(sig->signal,i);
		if (tmp2<0) tmp2 = -tmp2;
		if(tmp2>tmp1) tmp1 = tmp2;
	}

	return tmp1;
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

	auto signalSize  = filteredSignal->signal->size;
	gsl_vector_int *QRS_onset = qrsPosition.GetQRS_onset()->signal;
	gsl_vector_int *QRS_end = qrsPosition.GetQRS_end()->signal;
	int size = int(QRS_onset->size);
	IntSignal tmpSig;
	tmpSig = IntSignal(new WrappedVectorInt);
	tmpSig->signal = gsl_vector_int_alloc(size);

	for(int i=0;i<size;i++){

		int start = gsl_vector_int_get(QRS_onset,i);
		int stop = gsl_vector_int_get(QRS_end,i);
		
		double poleV = pole(&filteredSignal,start,stop);

		double dlugoscV = dlugosc(&filteredSignal,start,stop);
		double rm = (dlugoscV/2*sqrt(3.14*poleV))-1;
		double stosunekV = stosunek(&filteredSignal,start,stop);
		double szybkoscV = szybkosc(&filteredSignal,start,stop);

		//dokonczyc ocene rodzaju pobudzenia

		if (rm>65 && rm<80 && szybkoscV>0.075) gsl_vector_int_set(tmpSig->signal,i,SUPRACENTRICULAR);
		else gsl_vector_int_set(tmpSig->signal,i,VENTRICULUS);

	}

    qrsMorphology->setQrsMorphology(tmpSig);
	return 1;
}

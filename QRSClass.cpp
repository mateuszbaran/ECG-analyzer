#include "QRSClass.h"

QRSClass::QRSClass (void)
{}

QRSClass::~QRSClass (void)
{}

bool QRSClass::classQRS(ECGSignal &signal,ECGWaves &waves){

}

ECGSignal QRSClass::derivates(ECGSignal * signal){
	
	auto size = signal->channel_one->signal->size;

	ECGSignal tmpSig;
	tmpSig.setSize(size);
	auto currnetValue1 = gsl_vector_get (signal->channel_one->signal, 0);
	auto currnetValue2 = gsl_vector_get (signal->channel_two->signal, 0);
	for(int i = 1; i< size-1; i++){
		auto prevValue1 = currnetValue1;
		auto prevValue2 = currnetValue2;
		auto currnetValue1 = gsl_vector_get (signal->channel_one->signal, i);
		auto currnetValue2 = gsl_vector_get (signal->channel_two->signal, i);
		gsl_vector_set(tmpSig.channel_one->signal, i-1,currnetValue1 / prevValue1);
		gsl_vector_set(tmpSig.channel_two->signal, i-1,currnetValue2 / prevValue2);
	}

	return tmpSig;
}

ECGSignal QRSClass::norm(ECGSignal * signal){

	auto size = signal->channel_one->signal->size;

	ECGSignal tmpSig;
	tmpSig.setSize(size);
	double* min = findMinimum(signal,0,size-1);
	double* max = findMaximum(signal,0,size-1);
	double tmp1,tmp2;
	if (-min[0] > max[0]) tmp1 = -min[0];
	else tmp1 = max[0];
	if (-min[1] > max[1]) tmp2 = -min[1];
	else tmp2 = max[1];
	for(int i=0;i<size-1;i++){
		auto currnetValue1 = gsl_vector_get (signal->channel_one->signal, i);
		auto currnetValue2 = gsl_vector_get (signal->channel_two->signal, i);
		gsl_vector_set(tmpSig.channel_one->signal, i, currnetValue1/tmp1);
		gsl_vector_set(tmpSig.channel_two->signal, i, currnetValue2/tmp2);
	}

	return tmpSig;

}

ECGSignal QRSClass::quan(ECGSignal * signal){

	auto size = signal->channel_one->signal->size;
	int level = 50;
	ECGSignal tmpSig;
	tmpSig.setSize(size);
	double* min = findMinimum(signal,0,size-1);
	double* max = findMaximum(signal,0,size-1);
	for(int i=0;i<size-1;i++){
		auto qS1 = ((max[0] - gsl_vector_get (signal->channel_one->signal, i))/(max[0]-min[0])) * 100 % level;
		auto qS2 = ((max[1] - gsl_vector_get (signal->channel_two->signal, i))/(max[1]-min[1])) * 100 % level;
		gsl_vector_set(tmpSig.channel_one->signal,i,qS1);
		gsl_vector_set(tmpSig.channel_two->signal,i,qS2);
	}

	return tmpSig;
}

ECGSignal QRSClass::hist(ECGSignal * signal){

	auto size = signal->channel_one->signal->size;
	int level = 50;
	ECGSignal tmpSig;
	tmpSig.setSize(level);

	for (int i=0;i<level;i++){
		int one = 0;
		int two = 0;
		for(int j=0;j<size;j++){
			if(gsl_vector_get(signal->channel_one->signal,j)==i) one++;
			if(gsl_vector_get(signal->channel_two->signal,j)==i) two++;
		}
		gsl_vector_set(tmpSig.channel_one->signal,i,one);
		gsl_vector_set(tmpSig.channel_two->signal,i,two);
	}

	return tmpSig;

}

ECGSignal QRSClass::pca(ECGSignal * signal){

	auto size = signal->channel_one->signal->size;
	int level = 50;
	ECGSignal tmpSig;
	tmpSig.setSize(level);

	for (int i=0;i<level;i++){
		int one = 0;
		int two = 0;
		for(int j=0;j<size;j++){
			if(gsl_vector_get(signal->channel_one->signal,j)==i) one++;
			if(gsl_vector_get(signal->channel_two->signal,j)==i) two++;
		}
		gsl_vector_set(tmpSig.channel_one->signal,i,one);
		gsl_vector_set(tmpSig.channel_two->signal,i,two);
	}

	return tmpSig;

}


double* findMinimum (ECGSignal *signal,int forBegin, int forEnd) {


	auto minChannelOne = gsl_vector_get (signal->channel_one->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (signal->channel_one->signal, i);
		if (currentValue<minChannelOne) minChannelOne=currentValue;
	}

	auto minChannelTwo = gsl_vector_get (signal->channel_two->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (signal->channel_two->signal, i);
		if (currentValue<minChannelTwo) minChannelTwo=currentValue;
	}

	double* result = new double[2];
	result[0] = minChannelOne;
	result[1] = minChannelTwo;

	return result;
}

double* findMaximum (ECGSignal *signal,int forBegin, int forEnd) {


	auto maxChannelOne = gsl_vector_get (signal->channel_one->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (signal->channel_one->signal, i);
		if (currentValue>maxChannelOne) maxChannelOne=currentValue;
	}

	auto maxChannelTwo = gsl_vector_get (signal->channel_two->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (signal->channel_two->signal, i);
		if (currentValue>maxChannelTwo) maxChannelTwo=currentValue;
	}

	double* result = new double[2];
	result[0] = maxChannelOne;
	result[1] = maxChannelTwo;

	return result;
}
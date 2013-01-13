#include "ECGWaves.h"

double* findMinimum (ECGSignal *signal,int forBegin, int forEnd);
double* findMaximum (ECGSignal *signal,int forBegin, int forEnd);

ECGWaves::ECGWaves (void)
{}

ECGWaves::~ECGWaves (void)
{}

bool ECGWaves::detectQRS(ECGSignal *signal,ECGRs * rPeak){
	auto signalSize = signal->channel_one->signal->size;

	//power
	ECGSignal powerSig;

	for(int i = 0; i < signalSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (signal->channel_one->signal, i);			
		auto inputValueChannelTwo = gsl_vector_get (signal->channel_two->signal, i);
		gsl_vector_set(powerSig.channel_one->signal, i, pow(inputValueChannelOne, 2));
		gsl_vector_set(powerSig.channel_two->signal, i, pow(inputValueChannelTwo, 2));
	}

	//gradient
	ECGSignal gradSig =gradient(&powerSig);

	//average
	ECGSignal fg1Sig = averageFilter(&gradSig);

	//exp
	ECGSignal expSig;

	for(int i = 0; i < signalSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (gradSig.channel_one->signal, i);			
		auto inputValueChannelTwo = gsl_vector_get (gradSig.channel_two->signal, i);
		gsl_vector_set(expSig.channel_one->signal, i, 1-(2/ exp(2*inputValueChannelOne)+1));
		gsl_vector_set(expSig.channel_two->signal, i, 1-(2/ exp(2*inputValueChannelTwo)+1));
	}

	//gradient
	gradSig =gradient(&expSig);

	//average
	ECGSignal fg2Sig = averageFilter(&gradSig);

	ECGSignal ts3Sig;

	for(int i = 0; i < signalSize; i++)
	{
		auto originalValueChannelOne = gsl_vector_get (signal->channel_one->signal, i);			
		auto originaltValueChannelTwo = gsl_vector_get (signal->channel_two->signal, i);
		auto inputValueChannelOne = gsl_vector_get (fg2Sig.channel_one->signal, i);			
		auto inputValueChannelTwo = gsl_vector_get (fg2Sig.channel_two->signal, i);
		gsl_vector_set(ts3Sig.channel_one->signal, i, originalValueChannelOne * inputValueChannelOne);
		gsl_vector_set(ts3Sig.channel_two->signal, i, originaltValueChannelTwo * inputValueChannelTwo );
	}

	//gradient
    gradSig =gradient(&expSig);

	//average
	ECGSignal fg3 =averageFilter(&gradSig);;

	ECGSignal ts4Sig;

	for(int i = 0; i < signalSize; i++)
	{
		auto fg1ValueChannelOne = gsl_vector_get (fg1Sig.channel_one->signal, i);			
		auto fg1tValueChannelTwo = gsl_vector_get (fg1Sig.channel_two->signal, i);
		auto fg3ValueChannelOne = gsl_vector_get (fg3.channel_one->signal, i);			
		auto fg3ValueChannelTwo = gsl_vector_get (fg3.channel_two->signal, i);
		gsl_vector_set(ts4Sig.channel_one->signal, i, fg1ValueChannelOne + fg3ValueChannelOne);
		gsl_vector_set(ts4Sig.channel_two->signal, i, fg1tValueChannelTwo + fg3ValueChannelTwo );
	}

	//normalizacja
	ECGSignal pre_fq;
	double max_absoluteC1 =gsl_vector_get (ts4Sig.channel_one->signal, 0);
	double max_absoluteC2 =gsl_vector_get (ts4Sig.channel_two->signal, 0);

	for(int i = 0; i < signalSize; i++)
	{
		auto ValueChannelOne = gsl_vector_get (ts4Sig.channel_one->signal, i);			
		auto ValueChannelTwo = gsl_vector_get (ts4Sig.channel_two->signal, i);
		if(abs(ValueChannelOne)> abs(max_absoluteC1)) max_absoluteC1 = ValueChannelOne;
		if(abs(ValueChannelTwo)> abs(max_absoluteC2)) max_absoluteC2 = ValueChannelTwo;
	}

	
	for(int i = 0; i < signalSize; i++)
	{
		auto ValueChannelOne = gsl_vector_get (ts4Sig.channel_one->signal, i);			
		auto ValueChannelTwo = gsl_vector_get (ts4Sig.channel_two->signal, i);
		gsl_vector_set(pre_fq.channel_one->signal, i, ValueChannelOne / max_absoluteC1);
		gsl_vector_set(pre_fq.channel_two->signal, i, ValueChannelTwo /  max_absoluteC2);
	}


	ECGSignal OISig;
	for(int i = 0; i < signalSize; i++)
	{
		auto pre_fqValueChannelOne = gsl_vector_get (pre_fq.channel_one->signal, i);			
		auto pre_fqtValueChannelTwo = gsl_vector_get (pre_fq.channel_two->signal, i);
		gsl_vector_set(ts4Sig.channel_one->signal, i, (pre_fqValueChannelOne>0.05)?pre_fqValueChannelOne:0);
		gsl_vector_set(ts4Sig.channel_two->signal, i, (pre_fqtValueChannelTwo>0.05)?pre_fqtValueChannelTwo:0 );
	}

	gsl_vector_int *v = rPeak->GetRs()->signal;
    int size = int(v->size);
	for(int ithRpeak = 0;ithRpeak<size;ithRpeak++){

		int rPeak = gsl_vector_int_get (v, ithRpeak);

		for(int j = rPeak;j>rPeak-100;j--){
			auto value = gsl_vector_get (ts4Sig.channel_one->signal, j);
			if(j==0){
				//we have QRS_onset
				gsl_vector_int_set(QRS_onset->signal,ithRpeak,j);
				break;
			}
		}
		for(int j = rPeak;j<rPeak+100;j++){
			auto value = gsl_vector_get (ts4Sig.channel_one->signal, j);
			if(j==0){
				//we have QRS_end
				gsl_vector_int_set(QRS_end->signal,ithRpeak,j);
				break;
			}
		}
	}
	return true;
}

bool ECGWaves::detectPT(ECGSignal *signal){

	double qrsCount = QRS_onset->signal->size;
	ECGSignal allPTSig;

	for(int i = 0; i < qrsCount-2; i++)
	{	
		auto qrsOnset = gsl_vector_int_get (QRS_onset->signal, i+1);
		auto qrsEnd = gsl_vector_int_get (QRS_end->signal, i);
	    auto qrsOnset2 = gsl_vector_int_get (QRS_onset->signal, i+1);
		auto qrsEnd_2 = gsl_vector_int_get (QRS_end->signal, i+1);
		double cycleSize = qrsOnset2 - qrsEnd;


		double min,max = 0;
		min = gsl_vector_get (signal->channel_one->signal, qrsEnd);

		ECGSignal cycleSig;
		for(int j = qrsEnd; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (signal->channel_one->signal, j);
			max = (max>value)? max:value;
			min = (min<value)? min:value;
		}

		for(int j = qrsOnset; j < qrsEnd; j++)
		{
			gsl_vector_set(allPTSig.channel_one->signal, j, 0);
		}
		for(int j = qrsEnd;j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (signal->channel_one->signal, j);
			value = (value-min)/max;
			gsl_vector_set(allPTSig.channel_one->signal, j, value);
			gsl_vector_set(cycleSig.channel_one->signal, j-qrsEnd, value);
		}

		//finding median 
		gsl_sort_vector(cycleSig.channel_one->signal);
		double median = gsl_vector_get (cycleSig.channel_one->signal, cycleSize/2);

		for(int j = qrsEnd; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (signal->channel_one->signal, j);
			gsl_vector_set(allPTSig.channel_one->signal, j, value-median);
		}


		for(int j = qrsEnd; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (allPTSig.channel_one->signal, j);			
			gsl_vector_set(allPTSig.channel_one->signal, j, (value>0.07)?1:0);
		}

		//finding points 
		gsl_vector_int_set(T_end->signal,i,qrsEnd);
		double p1=0,p2=0;
		//T is in first 75%
		for(int j = qrsEnd; j < qrsOnset2*3/4; j++)
		{
			auto value = gsl_vector_get (allPTSig.channel_one->signal, j);	
			if(p1==0){
				if(value>0){
					p1=value;
				}
			}else if(value <1){
				p2 = value;
			}

			if(p2-p1>90){
				gsl_vector_int_set(T_end->signal,i,j);
				break;
			}else{
			p1 = p2 =0;
			}
		}
		//p in last 23%
		gsl_vector_int_set(P_onset->signal,i,qrsOnset2);
		gsl_vector_int_set(P_end->signal,i,qrsOnset2);
		for(int j = qrsEnd*3/4; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (allPTSig.channel_one->signal, j);	
			if(p1==0){
				if(value>0){
					p1=value;
				}
			}else if(value <1){
				p2 = value;
			}

			if(p2-p1>9){
				gsl_vector_int_set(P_onset->signal,i,p1);
				gsl_vector_int_set(P_end->signal,i,p2);
				break;
			}else{
			p1 = p2 =0;
			}
		}

	}
	return true;
}

ECGSignal ECGWaves::gradient(ECGSignal * signal){
	
	ECGSignal tmpSig;
	auto signalSize = signal->channel_one->signal->size;
	double* min;
	double* max;
	int limit=signalSize-5;
	
	for (int i=5;i<limit;i++){
		min=findMinimum(signal,i-5,i+5);
		max=findMaximum(signal,i-5,i+5);
		gsl_vector_set(tmpSig.channel_two->signal, i, max[0]-min[0]);
		gsl_vector_set(tmpSig.channel_two->signal, i, max[1]-min[1]);
	}
	auto vValueC1 = gsl_vector_get (signal->channel_one->signal, 5);
	auto vValueC2 = gsl_vector_get (signal->channel_two->signal, 5);
	auto lastValueC1 = gsl_vector_get (signal->channel_one->signal, signalSize-6);
	auto lastValueC2 = gsl_vector_get (signal->channel_two->signal, signalSize-6);

	gsl_vector_set(tmpSig.channel_one->signal, 0, vValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, 0, vValueC2);
	gsl_vector_set(tmpSig.channel_one->signal, 1, vValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, 1, vValueC2);
	gsl_vector_set(tmpSig.channel_one->signal, 2, vValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, 2, vValueC2);
	gsl_vector_set(tmpSig.channel_one->signal, 3, vValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, 3, vValueC2);
	gsl_vector_set(tmpSig.channel_one->signal, 4, vValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, 4, vValueC2);

	gsl_vector_set(tmpSig.channel_one->signal, signalSize-1, lastValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-1, lastValueC2);
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-2, lastValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-2, lastValueC2);
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-3, lastValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-3, lastValueC2);
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-4, lastValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-4, lastValueC2);
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-5, lastValueC1);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-5, lastValueC2);

	return tmpSig;
}

ECGSignal ECGWaves::averageFilter(ECGSignal * signal){
	
	ECGSignal tmpSig;
	auto signalSize = signal->channel_one->signal->size;
	int limit=signalSize-5;
	double sumC1=0;
	double sumC2=0;
	for (int j=0;j<11;j++){
		sumC1=sumC1+ gsl_vector_get (signal->channel_one->signal, j);
		sumC2=sumC2+ gsl_vector_get (signal->channel_two->signal, j);
	}
	gsl_vector_set(tmpSig.channel_one->signal, 0, sumC1/11);
	gsl_vector_set(tmpSig.channel_one->signal, 1, sumC1/11);
	gsl_vector_set(tmpSig.channel_one->signal, 2, sumC1/11);
	gsl_vector_set(tmpSig.channel_one->signal, 3, sumC1/11);
	gsl_vector_set(tmpSig.channel_one->signal, 4, sumC1/11);
	gsl_vector_set(tmpSig.channel_one->signal, 5, sumC1/11);

	gsl_vector_set(tmpSig.channel_two->signal, 0, sumC2/11);
	gsl_vector_set(tmpSig.channel_two->signal, 1, sumC2/11);
	gsl_vector_set(tmpSig.channel_two->signal, 2, sumC2/11);
	gsl_vector_set(tmpSig.channel_two->signal, 3, sumC2/11);
	gsl_vector_set(tmpSig.channel_two->signal, 4, sumC2/11);
	gsl_vector_set(tmpSig.channel_two->signal, 5, sumC2/11);

	for (int i=6;i<limit;i++){
		sumC1 = sumC1 - gsl_vector_get (signal->channel_one->signal, i-6) + gsl_vector_get (signal->channel_one->signal, i+5); 
		gsl_vector_set(tmpSig.channel_one->signal, 5, sumC1/11);					 
		
		sumC2 = sumC2 - gsl_vector_get (signal->channel_two->signal, i-6) + gsl_vector_get (signal->channel_two->signal, i+5); 
		gsl_vector_set(tmpSig.channel_two->signal, 5, sumC2/11);
	}
 	
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-1, sumC1/11);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-1, sumC2/11);
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-2, sumC1/11);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-2, sumC2/11);
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-3, sumC1/11);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-3, sumC2/11);
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-4, sumC1/11);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-4, sumC2/11);
	gsl_vector_set(tmpSig.channel_one->signal, signalSize-5, sumC1/11);
	gsl_vector_set(tmpSig.channel_two->signal, signalSize-5, sumC2/11);

	return tmpSig;
}

double* findMinimum (ECGSignal *signal,int forBegin, int forEnd) {


	auto minChannelOne = gsl_vector_get (signal->channel_one->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (signal->channel_one->signal, forBegin);
		if (currentValue<minChannelOne) minChannelOne=currentValue;
	}

	auto minChannelTwo = gsl_vector_get (signal->channel_two->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (signal->channel_two->signal, forBegin);
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
		auto currentValue = gsl_vector_get (signal->channel_one->signal, forBegin);
		if (currentValue<maxChannelOne) maxChannelOne=currentValue;
	}

	auto maxChannelTwo = gsl_vector_get (signal->channel_two->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (signal->channel_two->signal, forBegin);
		if (currentValue<maxChannelTwo) maxChannelTwo=currentValue;
	}

	double* result = new double[2];
	result[0] = maxChannelOne;
	result[1] = maxChannelTwo;

	return result;
}

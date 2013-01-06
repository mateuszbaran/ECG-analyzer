#include "ECGWaves.h"


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

	//mediana
	ECGSignal ts4mSig = ts4Sig;

	//normalizacja
	ECGSignal pre_fq;
	double max_absoluteC1 =gsl_vector_get (ts4mSig.channel_one->signal, 0);
	double max_absoluteC2 =gsl_vector_get (ts4mSig.channel_two->signal, 0);

	for(int i = 0; i < signalSize; i++)
	{
		auto ValueChannelOne = gsl_vector_get (ts4mSig.channel_one->signal, i);			
		auto ValueChannelTwo = gsl_vector_get (ts4mSig.channel_two->signal, i);
		if(abs(ValueChannelOne)> abs(max_absoluteC1)) max_absoluteC1 = ValueChannelOne;
		if(abs(ValueChannelTwo)> abs(max_absoluteC2)) max_absoluteC2 = ValueChannelTwo;
	}

	
	for(int i = 0; i < signalSize; i++)
	{
		auto ValueChannelOne = gsl_vector_get (ts4mSig.channel_one->signal, i);			
		auto ValueChannelTwo = gsl_vector_get (ts4mSig.channel_two->signal, i);
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

	rPeak->GetRs();
	//TODO
	int rPeakCount = 10;
	for(int i = 0;i<rPeakCount;i++){
		//TODI
		int rPeak = 1;
		for(int j = i;j>i-100;j--){
			auto value = gsl_vector_get (ts4Sig.channel_one->signal, j);
			if(j==0){
				//we have QRS_onset
				break;
			}
		}
		for(int j = i;j<i+100;j++){
			auto value = gsl_vector_get (ts4Sig.channel_one->signal, j);
			if(j==0){
				//we have QRS_end
				break;
			}
		}
	}
	return true;
}

bool ECGWaves::detectPT(ECGSignal *signal){

	double qrsCount = 350;//QRS_onset.get
	ECGSignal allPTSig;

	for(int i = 0; i < qrsCount-1; i++)
	{
		auto qrsOnset = 1;// gsl_vector_get (QRS_onset->signal, i);			
		auto qrsEnd = 12;//gsl_vector_get (QRS_end, i);
		auto qrsEnd_2 = 14;//gsl_vector_get (QRS_end, i+1);

		double sum1 = 0,sum2,mean1,mean2 = 0;
		ECGSignal tmpPTSig;

		for(int i = qrsOnset; i < qrsEnd_2; i++)
		{
			sum1 += gsl_vector_get (signal->channel_one->signal, i);
			gsl_vector_set(tmpPTSig.channel_one->signal, i, mean1);
		}

		//TODO mean2
		mean1 = sum1/(qrsEnd_2-qrsEnd);
		mean2 = sum1/(qrsEnd_2-qrsEnd);

		for(int i = 0; i < qrsCount; i++)
		{
		gsl_vector_set(tmpPTSig.channel_one->signal, i, mean1);
		gsl_vector_set(tmpPTSig.channel_two->signal, i, mean2);
		}


		double minimal = gsl_vector_get (tmpPTSig.channel_one->signal, qrsOnset);;
		//normalizacja
		for(int i = qrsOnset+1; i < qrsEnd_2; i++)
		{
			auto tmp = gsl_vector_get (tmpPTSig.channel_one->signal, i);
			if(tmp<minimal){
				minimal = tmp;
			}
		}
		if(minimal<0){
			minimal = -1*minimal;
		}

		for(int i = qrsOnset+1; i < qrsEnd_2; i++)
		{
			auto tmp = gsl_vector_get (tmpPTSig.channel_one->signal, i);
			gsl_vector_set(tmpPTSig.channel_one->signal, i, tmp+minimal);
		}
		
		double maximal = gsl_vector_get (tmpPTSig.channel_one->signal, qrsOnset);;
		for(int i = qrsOnset+1; i < qrsEnd_2; i++)
		{
			auto tmp = gsl_vector_get (tmpPTSig.channel_one->signal, i);
			if(tmp<maximal){
				maximal = tmp;
			}
		}
		for(int i = qrsOnset+1; i < qrsEnd_2; i++)
		{
			auto tmp = gsl_vector_get (tmpPTSig.channel_one->signal, i);
			gsl_vector_set(tmpPTSig.channel_one->signal, i, tmp/maximal);
		}

		//znormalizowany sygnal
		for(int i = 0; i < qrsEnd_2; i++)
		{
			auto pre_fqValueChannelOne = gsl_vector_get (tmpPTSig.channel_one->signal, i);			
			auto pre_fqtValueChannelTwo = gsl_vector_get (tmpPTSig.channel_one->signal, i);
			gsl_vector_set(allPTSig.channel_one->signal, i, (pre_fqValueChannelOne>0.05)?1:0);
			gsl_vector_set(allPTSig.channel_two->signal, i, (pre_fqtValueChannelTwo>0.05)?1:0 );
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
	//okno o rozmiarze 11, zoptymalizowany czas obliczeñ z 10 na 2 operacje przy ka¿dym przejœciu pêtli
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
		sumC1 = sumC1 - gsl_vector_get (signal->channel_one->signal, i-6) + gsl_vector_get (signal->channel_one->signal, i+5); //od sumy któr¹ obliczy³ w poprzednim przejœciu odejmuje piewszy z tamtej kolejki i dodaje nowy (ostatni), 
		gsl_vector_set(tmpSig.channel_one->signal, 5, sumC1/11);					 //dziêki temy nie liczy zowu sumy od 0 do 11 tylko korzyta z sumy dla 10 œrodkowych liczb z poprzedniego przejœcia pêtli
		
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

float findMedian(float * table, int size){
	
	float median;
	for (int i=0;i<size-2;i++)
		for( int j=size-1;j>0;j--) 
				if (table[j-1]>table[j]){
					float aa=table[j];
					table[j]=table[j-1];
					table[j-1]=aa;
				}
       
	 if ( (size/2) % 2) median = ((table[size/2]) + (table[size/2 + 1])) / 2;  
	 else median= table[size/2];

	 return median;
}

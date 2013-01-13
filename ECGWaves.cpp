#include "ECGWaves.h"

double* findMinimum (ECGSignal *signal,int forBegin, int forEnd);
double* findMaximum (ECGSignal *signal,int forBegin, int forEnd);

ECGWaves::ECGWaves (void)
{}

ECGWaves::~ECGWaves (void)
{}

bool ECGWaves::detectQRS(ECGSignal &signal,ECGRs &rPeak){

	auto signalSize = signal.channel_one->signal->size;

	//power
	ECGSignal powerSig;
	powerSig.setSize(signalSize);
	for(int i = 0; i < signalSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (signal.channel_one->signal, i);			
		auto inputValueChannelTwo = gsl_vector_get (signal.channel_two->signal, i);
		double pp = inputValueChannelOne*inputValueChannelOne;
		gsl_vector_set(powerSig.channel_one->signal, i, inputValueChannelOne*inputValueChannelOne);
		gsl_vector_set(powerSig.channel_two->signal, i, inputValueChannelTwo*inputValueChannelTwo);
	}

	//gradient
	ECGSignal gradSig;
	gradSig.setSize(signalSize);
	gradSig=gradient(&powerSig);

	//average
	ECGSignal fg1Sig;
	fg1Sig.setSize(signalSize);
	fg1Sig = averageFilter(&gradSig);

	//exp
	ECGSignal expSig;
	expSig.setSize(signalSize);

	for(int i = 0; i < signalSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (gradSig.channel_one->signal, i);			
		auto inputValueChannelTwo = gsl_vector_get (gradSig.channel_two->signal, i);
		double expp = 1-(2/ exp(2*inputValueChannelOne)+1);
		gsl_vector_set(expSig.channel_one->signal, i, 1-(2/ exp(2*inputValueChannelOne)+1));
		gsl_vector_set(expSig.channel_two->signal, i, 1-(2/ exp(2*inputValueChannelTwo)+1));
	}

	//gradient
	gradSig =gradient(&expSig);

	//average
	ECGSignal fg2Sig = averageFilter(&gradSig);

	ECGSignal ts3Sig;
	ts3Sig.setSize(signalSize);

	for(int i = 0; i < signalSize; i++)
	{
		auto originalValueChannelOne = gsl_vector_get (signal.channel_one->signal, i);			
		auto originaltValueChannelTwo = gsl_vector_get (signal.channel_two->signal, i);
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
	ts4Sig.setSize(signalSize);

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
	pre_fq.setSize(signalSize);

	double max_absoluteC1 = gsl_vector_get (ts4Sig.channel_one->signal, 10);
	double max_absoluteC2 =gsl_vector_get (ts4Sig.channel_two->signal, 10);
	double min_C1 = gsl_vector_get (ts4Sig.channel_one->signal, 10);
	double min_C2 =gsl_vector_get (ts4Sig.channel_two->signal, 10);

	for(int i = 10; i < signalSize-100; i++)
	{
		auto ValueChannelOne = gsl_vector_get (ts4Sig.channel_one->signal, i);			
		auto ValueChannelTwo = gsl_vector_get (ts4Sig.channel_two->signal, i);
		if(ValueChannelOne> max_absoluteC1) max_absoluteC1 = ValueChannelOne;
		if(ValueChannelTwo> max_absoluteC2) max_absoluteC2 = ValueChannelTwo;

		if(ValueChannelOne< min_C1) min_C1 = ValueChannelOne;
		if(ValueChannelTwo< min_C2) min_C1 = ValueChannelTwo;
	}

	
	for(int i = 0; i < signalSize; i++)
	{
		auto ValueChannelOne = gsl_vector_get (ts4Sig.channel_one->signal, i);			
		auto ValueChannelTwo = gsl_vector_get (ts4Sig.channel_two->signal, i);
		double pprr = (ValueChannelOne-min_C1) / (max_absoluteC1-min_C1);
		gsl_vector_set(pre_fq.channel_one->signal, i, (ValueChannelOne-min_C1) / (max_absoluteC1-min_C1));
		gsl_vector_set(pre_fq.channel_two->signal, i, (ValueChannelTwo-min_C2) /  (max_absoluteC2-min_C2));
	}


	ECGSignal OISig;
	OISig.setSize(signalSize);

	for(int i = 0; i < signalSize; i++)
	{
		auto pre_fqValueChannelOne = gsl_vector_get (pre_fq.channel_one->signal, i);			
		auto pre_fqtValueChannelTwo = gsl_vector_get (pre_fq.channel_two->signal, i);
		gsl_vector_set(ts4Sig.channel_one->signal, i, (pre_fqValueChannelOne>0.05)?pre_fqValueChannelOne:0);
		gsl_vector_set(ts4Sig.channel_two->signal, i, (pre_fqtValueChannelTwo>0.05)?pre_fqtValueChannelTwo:0 );
	}

	gsl_vector_int *v = rPeak.GetRs()->signal;
    int size = int(v->size);
	QRS_onset = IntSignal(new WrappedVectorInt);
	QRS_end = IntSignal(new WrappedVectorInt);
	QRS_onset->signal = gsl_vector_int_alloc(size);
	QRS_end->signal = gsl_vector_int_alloc(size);

	for(int ithRpeak = 0;ithRpeak<size;ithRpeak++){

		int rPeak = gsl_vector_int_get (v, ithRpeak);
		gsl_vector_int_set(QRS_onset->signal,ithRpeak,rPeak);
		gsl_vector_int_set(QRS_end->signal,ithRpeak,rPeak);

		for(int j = rPeak;j>rPeak-100;j--){
			auto value = gsl_vector_get (ts4Sig.channel_one->signal, j);
			if(value==0){
				//we have QRS_onset
				gsl_vector_int_set(QRS_onset->signal,ithRpeak,j);
				break;
			}
		}
		for(int j = rPeak;j<rPeak+100;j++){
			auto value = gsl_vector_get (ts4Sig.channel_one->signal, j);
			if(value==0){
				//we have QRS_end
				gsl_vector_int_set(QRS_end->signal,ithRpeak,j);
				break;
			}
		}
	}
	return true;
}

bool ECGWaves::detectPT(ECGSignal &signal){
	double qrsCount = QRS_onset->signal->size;
	auto signalSize = signal.channel_one->signal->size;

	T_end = IntSignal(new WrappedVectorInt);
	P_onset = IntSignal(new WrappedVectorInt);
	P_end = IntSignal(new WrappedVectorInt);

	T_end->signal = gsl_vector_int_alloc(qrsCount);
	P_onset->signal = gsl_vector_int_alloc(qrsCount);
	P_end->signal = gsl_vector_int_alloc(qrsCount);

	
	ECGSignal allPTSig;
	allPTSig.setSize(signalSize);

	for(int i = 0; i < qrsCount-2; i++)
	{	
		auto qrsOnset = gsl_vector_int_get (QRS_onset->signal, i+1);
		auto qrsEnd = gsl_vector_int_get (QRS_end->signal, i);
	    auto qrsOnset2 = gsl_vector_int_get (QRS_onset->signal, i+1);
		auto qrsEnd_2 = gsl_vector_int_get (QRS_end->signal, i+1);
		double cycleSize = qrsOnset2 - qrsEnd;


		double min,max = 0;
		min = gsl_vector_get (signal.channel_one->signal, qrsEnd);

		ECGSignal cycleSig;
		cycleSig.setSize(signalSize);
		for(int j = qrsEnd; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (signal.channel_one->signal, j);
			max = (max>value)? max:value;
			min = (min<value)? min:value;
		}

		for(int j = qrsOnset; j < qrsEnd; j++)
		{
			gsl_vector_set(allPTSig.channel_one->signal, j, 0);
		}
		for(int j = qrsEnd;j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (signal.channel_one->signal, j);
			value = (value-min)/max;
			gsl_vector_set(allPTSig.channel_one->signal, j, value);
			gsl_vector_set(cycleSig.channel_one->signal, j-qrsEnd, value);
		}

		//finding median 
		gsl_sort_vector(cycleSig.channel_one->signal);
		double median = gsl_vector_get (cycleSig.channel_one->signal, cycleSize/2);

		for(int j = qrsEnd; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (signal.channel_one->signal, j);
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
	
	auto size = signal->channel_one->signal->size;

	ECGSignal tmpSig;
	tmpSig.setSize(size);
	auto signalSize = signal->channel_one->signal->size;
	double* min;
	double* max;
	int limit=signalSize-5;
	
	for (int i=5;i<limit;i++){
		min=findMinimum(signal,i-5,i+5);
		max=findMaximum(signal,i-5,i+5);
		double aa =max[0]-min[0];
		gsl_vector_set(tmpSig.channel_one->signal, i, max[0]-min[0]);
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
	auto size = signal->channel_one->signal->size;

	ECGSignal tmpSig;
	tmpSig.setSize(size);

	auto signalSize = signal->channel_one->signal->size;
	int limit=signalSize-5;
	double sumC1=0;
	double sumC2=0;
	for (int j=0;j<11;j++){
		//double one = gsl_vector_get (signal->channel_one->signal, j);
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
		gsl_vector_set(tmpSig.channel_one->signal, i-1, sumC1/11);					 
		
		sumC2 = sumC2 - gsl_vector_get (signal->channel_two->signal, i-6) + gsl_vector_get (signal->channel_two->signal, i+5); 
		gsl_vector_set(tmpSig.channel_two->signal, i-1, sumC2/11);
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

ECGRs ECGWaves::getMockedRPeak(){

	ECGRs mockedSignal;

	IntSignal rct = IntSignal(new WrappedVectorInt);
	rct->signal = gsl_vector_int_alloc(3);

	gsl_vector_int_set(rct->signal, 0, 137);
	gsl_vector_int_set(rct->signal, 1, 431);
	gsl_vector_int_set(rct->signal, 2, 719);

	mockedSignal.setRs(rct);
	return mockedSignal;
}

ECGSignal ECGWaves::getMockedSignal()
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

	ECGSignal mockedSignal;
	int length = 1000;
	mockedSignal.setSize(length);

	for(int i = 0; i < length; i++)
	{
		double s = signal[i];
		double ss = s*s;
		double sss = signal[i]* signal[i];
		gsl_vector_set(mockedSignal.channel_one->signal, i, signal[i]);
		gsl_vector_set(mockedSignal.channel_two->signal, i, signal[i]);
	}

	return mockedSignal;
}  


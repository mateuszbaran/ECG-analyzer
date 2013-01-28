#include "QRSPointsDetector.h"

#include "tri_logger.hpp"
#define LOG_END TRI_LOG_STR("END: " << __FUNCTION__);

double findMinimum (ECGSignalChannel *signal,int forBegin, int forEnd) {

	ECGSignalChannel sig;
	sig = *signal;
	auto sigSize = sig->signal->size;

	auto minChannelOne = gsl_vector_get (sig->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (sig->signal, i);
		if (currentValue<minChannelOne) minChannelOne=currentValue;
	}

	return minChannelOne;
}

double findMaximum (ECGSignalChannel *signal,int forBegin, int forEnd) {


	ECGSignalChannel sig;
	sig = *signal;
	auto sigSize = sig->signal->size;

	auto maxChannelOne = gsl_vector_get (sig->signal, forBegin);
	for (int i = forBegin; i < forEnd; i++) {
		auto currentValue = gsl_vector_get (sig->signal, i);
		if (currentValue>maxChannelOne) maxChannelOne=currentValue;
	}

	return maxChannelOne;
}


QRSPointsDetector::QRSPointsDetector(void)
{
}


QRSPointsDetector::~QRSPointsDetector(void)
{
}


void QRSPointsDetector::setParams(ParametersTypes &parameterTypes){}


void QRSPointsDetector::runModule (ECGWaves &ecgWaves, const ECGSignalChannel &filteredSignal, const ECGRs &ecgRs, const ECGInfo &ecgi, ECGClasses &classes){
	double inputValue = gsl_vector_get (filteredSignal->signal, 1);
	try{
		this->filteredSignal = filteredSignal;
#ifdef USE_MOCKED_SIGNAL
		this->filteredSignal = getMockedSignal();
#endif
		this->rsPositions = ecgRs;

		this->qrsPoints = &ecgWaves;

		gsl_vector_int *v = rsPositions.GetRs()->signal;
		int size = int(v->size);
		this->qrsPoints->setSize(size);

		detectQRS();
		detectPT();

	}catch (...) {
#ifdef DEBUG
		cout << "modules failed" << endl;
#endif
	}
}


bool QRSPointsDetector::detectQRS()
{
#ifdef DEBUG
	cout << "Detecting qrs-onset and qrs-end" << endl;
#endif

	auto signalSize = filteredSignal->signal->size;

	gsl_vector_int *v = rsPositions.GetRs()->signal;
	int rPeaksize = int(v->size);

	//power
	ECGSignalChannel powerSig;
	powerSig = ECGSignalChannel(new WrappedVector);
	powerSig->signal = gsl_vector_alloc(signalSize);
	for(int i = 0; i < signalSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (filteredSignal->signal, i);	
		double pp = inputValueChannelOne*inputValueChannelOne;
		gsl_vector_set(powerSig->signal, i, inputValueChannelOne*inputValueChannelOne);
	}

	//gradient
	ECGSignalChannel gradSig;
	gradSig = ECGSignalChannel(new WrappedVector);
	gradSig->signal = gsl_vector_alloc(signalSize);
	gradSig=gradient(&powerSig);

	//average
	ECGSignalChannel fg1Sig;
	fg1Sig = ECGSignalChannel(new WrappedVector);
	fg1Sig->signal = gsl_vector_alloc(signalSize);
	fg1Sig = averageFilter(&gradSig);

	//exp
	ECGSignalChannel expSig;
	expSig = ECGSignalChannel(new WrappedVector);
	expSig->signal = gsl_vector_alloc(signalSize);

	for(int i = 0; i < signalSize; i++)
	{
		auto inputValueChannelOne = gsl_vector_get (gradSig->signal, i);			
		double expp = 1-(2/ exp(2*inputValueChannelOne)+1);
		gsl_vector_set(expSig->signal, i, 1-(2/ exp(2*inputValueChannelOne)+1));
	}

	//gradient
	gradSig =gradient(&expSig);

	//average
	ECGSignalChannel fg2Sig;
	fg2Sig = ECGSignalChannel(new WrappedVector);
	fg2Sig->signal = gsl_vector_alloc(signalSize);
	fg2Sig = averageFilter(&gradSig);

	ECGSignalChannel ts3Sig;
	ts3Sig = ECGSignalChannel(new WrappedVector);
	ts3Sig->signal = gsl_vector_alloc(signalSize);

	for(int i = 0; i < signalSize; i++)
	{
		auto originalValueChannelOne = gsl_vector_get (filteredSignal->signal, i);		
		auto inputValueChannelOne = gsl_vector_get (fg2Sig->signal, i);			
		gsl_vector_set(ts3Sig->signal, i, originalValueChannelOne * inputValueChannelOne);
	}

	//gradient
	gradSig =gradient(&expSig);

	//average
	ECGSignalChannel fg3;
	fg3 = ECGSignalChannel(new WrappedVector);
	fg3->signal = gsl_vector_alloc(signalSize);
	fg3 =averageFilter(&gradSig);;

	ECGSignalChannel ts4Sig;
	ts4Sig = ECGSignalChannel(new WrappedVector);
	ts4Sig->signal = gsl_vector_alloc(signalSize);

	for(int i = 0; i < signalSize; i++)
	{
		auto fg1ValueChannelOne = gsl_vector_get (fg1Sig->signal, i);			
		auto fg3ValueChannelOne = gsl_vector_get (fg3->signal, i);			
		gsl_vector_set(ts4Sig->signal, i, fg1ValueChannelOne + fg3ValueChannelOne);
	}

	//normalizacja

	ECGSignalChannel pre_fq;
	pre_fq = ECGSignalChannel(new WrappedVector);
	pre_fq->signal = gsl_vector_alloc(signalSize);

	double min_C1 = gsl_vector_get (ts4Sig->signal, 0);

	for(int i = 0; i < signalSize-100; i++)
	{
		auto ValueChannelOne = gsl_vector_get (ts4Sig->signal, i);		
		if(ValueChannelOne< min_C1) min_C1 = ValueChannelOne;
	}

	double max_absoluteC1 = min_C1;
	for(int ithRpeak = 0;ithRpeak<rPeaksize;ithRpeak++){
		int rPeak = gsl_vector_int_get (v, ithRpeak);
		auto value = gsl_vector_get (ts4Sig->signal, rPeak);
		if(value> max_absoluteC1) max_absoluteC1 = value;
	}
#ifdef DEBUG
	cout << "Minimal value "<< min_C1 << endl;
	cout << "Macimal value "<< max_absoluteC1 << endl;	
#endif

	for(int i = 0; i < signalSize; i++)
	{
		auto ValueChannelOne = gsl_vector_get (ts4Sig->signal, i);			
		double pprr = (ValueChannelOne-min_C1) / (max_absoluteC1-min_C1);
		gsl_vector_set(pre_fq->signal, i, (ValueChannelOne-min_C1) / (max_absoluteC1-min_C1));
	}


	ECGSignalChannel OISig;
	OISig = ECGSignalChannel(new WrappedVector);
	OISig->signal = gsl_vector_alloc(signalSize);

	for(int i = 0; i < signalSize; i++)
	{
		auto pre_fqValueChannelOne = gsl_vector_get (pre_fq->signal, i);			
		double rr = (pre_fqValueChannelOne>0.05)?pre_fqValueChannelOne:0;
		gsl_vector_set(ts4Sig->signal, i, (pre_fqValueChannelOne>0.05)?pre_fqValueChannelOne:0);
	}

	IntSignal qrsOnset;
	qrsOnset = IntSignal(new WrappedVectorInt);
	qrsOnset->signal = gsl_vector_int_alloc(rPeaksize);
	IntSignal qrsEnd;
	qrsEnd = IntSignal(new WrappedVectorInt);
	qrsEnd->signal = gsl_vector_int_alloc(rPeaksize);

	for(int ithRpeak = 0;ithRpeak<rPeaksize;ithRpeak++){
		int rPeak = gsl_vector_int_get (v, ithRpeak);
		gsl_vector_int_set(qrsOnset->signal,ithRpeak,rPeak);
		gsl_vector_int_set(qrsEnd->signal,ithRpeak,rPeak);
#ifdef DEBUG
		cout << "Rpeak no "<<ithRpeak <<" value :" << rPeak  << endl;	
#endif

		for(int j = rPeak;j>rPeak-100;j--){
			auto value = gsl_vector_get (ts4Sig->signal, j);
			if(value==0){
				//we have QRS_onset
#ifdef DEBUG
				cout << "QRS_onset " << j  << endl;	
#endif
				gsl_vector_int_set(qrsOnset->signal,ithRpeak,j);
				break;
			}
		}
		for(int j = rPeak;j<rPeak+100;j++){
			auto value = gsl_vector_get (ts4Sig->signal, j);
			if(value==0){
				//we have QRS_end
#ifdef DEBUG
				cout << "QRS_end " << j  << endl;	
#endif
				gsl_vector_int_set(qrsEnd->signal,ithRpeak,j);
				break;
			}
		}
	}
	qrsPoints->setQrsOnset(qrsOnset);
	qrsPoints->setQrsEnd(qrsEnd);
	return true;
}

bool QRSPointsDetector::detectPT(){
	double qrsCount = qrsPoints->GetQRS_onset()->signal->size;


	auto signalSize = filteredSignal->signal->size;

	ECGSignalChannel allPTSig;
	allPTSig = ECGSignalChannel(new WrappedVector);
	allPTSig->signal = gsl_vector_alloc(signalSize);

#ifdef DEBUG
	cout << "Number of cycles " << qrsCount-1  << endl;	
#endif

	IntSignal tEnd;
	tEnd = IntSignal(new WrappedVectorInt);
	tEnd->signal = gsl_vector_int_alloc(qrsCount);

	IntSignal pOnset;
	pOnset = IntSignal(new WrappedVectorInt);
	pOnset->signal = gsl_vector_int_alloc(qrsCount);
	IntSignal pEnd;
	pEnd = IntSignal(new WrappedVectorInt);
	pEnd->signal = gsl_vector_int_alloc(qrsCount);

	auto qrsOnset = gsl_vector_int_get (qrsPoints->GetQRS_onset()->signal, 0);

	gsl_vector_int_set(pOnset->signal,0,(qrsOnset-20<0?0:qrsOnset-20));
	gsl_vector_int_set(pEnd->signal,0,(qrsOnset-10<0?1:qrsOnset-10));

	for(int i = 0; i < qrsCount-1; i++)
	{	
		auto qrsOnset = gsl_vector_int_get (qrsPoints->GetQRS_onset()->signal, i);
		auto qrsEnd = gsl_vector_int_get (qrsPoints->GetQRS_end()->signal, i);
		auto qrsOnset2 = gsl_vector_int_get (qrsPoints->GetQRS_onset()->signal, i+1);
		auto qrsEnd_2 = gsl_vector_int_get (qrsPoints->GetQRS_end()->signal, i+1);
		double cycleSize = qrsOnset2 - qrsEnd;
		
		gsl_vector_int_set(tEnd->signal,i,qrsEnd);
		gsl_vector_int_set(pOnset->signal,i+1,qrsOnset2);
		gsl_vector_int_set(pEnd->signal,i+1,qrsOnset2);

		if(cycleSize<1)
			continue;

#ifdef DEBUG
		cout << "Cycle     :  " << i << endl;
		cout << "qrsEnd    :  " << qrsEnd << endl;
		cout << "qrsOnset  :  " << qrsOnset << endl;
		cout << "qrsOnset2 :  " << qrsOnset2 << endl;
		cout << "qrsEnd_2  :  " << qrsEnd_2 << endl;

#endif

		double min,max = 0;
		min = gsl_vector_get (filteredSignal->signal, qrsEnd);
		
		ECGSignalChannel cycleSig;
		cycleSig = ECGSignalChannel(new WrappedVector);
		cycleSig->signal = gsl_vector_alloc(cycleSize);
		
		
		for(int j = qrsEnd+10; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (filteredSignal->signal, j);
			max = (max>value)? max:value;
			min = (min<value)? min:value;
		}
		
#ifdef DEBUG
		cout << "Maximal value in cycle " << max  << endl;
		cout << "Minimal value in cycle " << min  << endl;
#endif

		for(int j = qrsOnset; j < qrsEnd; j++)
		{
			gsl_vector_set(allPTSig->signal, j, 0);
		}
		for(int j = qrsEnd;j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (filteredSignal->signal, j);
			value = (value-min)/(max-min);
			gsl_vector_set(allPTSig->signal, j, value);
			gsl_vector_set(cycleSig->signal, j-qrsEnd, value);
		}

		//finding median 
		gsl_sort_vector(cycleSig->signal);
		double median = gsl_vector_get (cycleSig->signal, cycleSize/2);
#ifdef DEBUG
		cout << "Median in cycle " << median  << endl;
#endif
		for(int j = qrsEnd; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (allPTSig->signal, j);
			gsl_vector_set(allPTSig->signal, j, value-median);
		}


		for(int j = qrsEnd; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (allPTSig->signal, j);	
			double afterTresholding = (value>0.05)?1:0;
			gsl_vector_set(allPTSig->signal, j, afterTresholding);
		}

		//finding points 
		double p1=0,p2=0;
		//T is in first 75%
		for(int j = qrsEnd; j < qrsEnd+cycleSize*3/4; j++)
		{
			auto value = gsl_vector_get (allPTSig->signal, j);	
			if(p1==0){
				if(value>0){
					p1=j;
				}
			}else if(value <1){
				p2 = j;

#ifdef DEBUG
				cout << "Candidate for T start " << p1  << endl;
				cout << "Candidate for T_end " << p2  << endl;
#endif

				if(p2-p1>cycleSize/100*15){
					gsl_vector_int_set(tEnd->signal,i,j);
#ifdef DEBUG
					cout << "We have next T_end: " << p2  << endl;
#endif
					break;
				}else{
					p1 = p2 =0;
				}
			}
		}
		//p in last 23%

		p1 = p2 =0;

		for(int j = qrsEnd+cycleSize*3/4; j < qrsOnset2; j++)
		{
			auto value = gsl_vector_get (allPTSig->signal, j);	
			if(p1==0){
				if(value>0){
					p1=j;
				}
			}else if(value <1){
				p2 = j;


				if(p2-p1>cycleSize/100*3){
					gsl_vector_int_set(pOnset->signal,i,p1);
					gsl_vector_int_set(pEnd->signal,i,p2);
#ifdef DEBUG
					cout << "We have next P_onset: " << p1  << endl;
					cout << "We have next P_end  : " << p2  << endl;
#endif
					break;
				}else{
					p1 = p2 =0;
				}
			}
		}

	}

	auto qrsEnd = gsl_vector_int_get (qrsPoints->GetQRS_end()->signal, qrsCount-1);
	gsl_vector_int_set(tEnd->signal,qrsCount-1,(qrsEnd+20>signalSize?signalSize:qrsEnd+20));
	qrsPoints->setTend(tEnd);
	qrsPoints->setPonset(pOnset);
	qrsPoints->setPend(pEnd);
	return true;
}





ECGSignalChannel QRSPointsDetector::gradient(ECGSignalChannel * signal){

	ECGSignalChannel sig;
	sig = *signal;
	auto sigSize = sig->signal->size;

	ECGSignalChannel tmpSig;
	tmpSig = ECGSignalChannel(new WrappedVector);
	tmpSig->signal = gsl_vector_alloc(sigSize);
	auto signalSize = sig->signal->size;
	double min;
	double max;
	int limit=signalSize-5;

	for (int i=5;i<limit;i++){
		min=findMinimum(&sig,i-5,i+5);
		max=findMaximum(&sig,i-5,i+5);
		gsl_vector_set(tmpSig->signal, i, max-min);
		//gsl_vector_set(tmpSig->signal, i, max[1]-min[1]);
	}
	auto vValueC1 = gsl_vector_get (sig->signal, 5);
	//auto vValueC2 = gsl_vector_get (sig->signal, 5);
	auto lastValueC1 = gsl_vector_get (sig->signal, signalSize-6);
	//auto lastValueC2 = gsl_vector_get (sig->signal, signalSize-6);

	gsl_vector_set(tmpSig->signal, 0, vValueC1);
	////gsl_vector_set(tmpSig->signal, 0, vValueC2);
	gsl_vector_set(tmpSig->signal, 1, vValueC1);
	//gsl_vector_set(tmpSig->signal, 1, vValueC2);
	gsl_vector_set(tmpSig->signal, 2, vValueC1);
	//gsl_vector_set(tmpSig->signal, 2, vValueC2);
	gsl_vector_set(tmpSig->signal, 3, vValueC1);
	//gsl_vector_set(tmpSig->signal, 3, vValueC2);
	gsl_vector_set(tmpSig->signal, 4, vValueC1);
	//gsl_vector_set(tmpSig->signal, 4, vValueC2);

	gsl_vector_set(tmpSig->signal, signalSize-1, lastValueC1);
	//gsl_vector_set(tmpSig->signal, signalSize-1, lastValueC2);
	gsl_vector_set(tmpSig->signal, signalSize-2, lastValueC1);
	//gsl_vector_set(tmpSig->signal, signalSize-2, lastValueC2);
	gsl_vector_set(tmpSig->signal, signalSize-3, lastValueC1);
	//gsl_vector_set(tmpSig->signal, signalSize-3, lastValueC2);
	gsl_vector_set(tmpSig->signal, signalSize-4, lastValueC1);
	//gsl_vector_set(tmpSig->signal, signalSize-4, lastValueC2);
	gsl_vector_set(tmpSig->signal, signalSize-5, lastValueC1);
	//gsl_vector_set(tmpSig->signal, signalSize-5, lastValuesC2);

	return tmpSig;
}

ECGSignalChannel QRSPointsDetector::averageFilter(ECGSignalChannel * signal){
	ECGSignalChannel sig;
	sig = *signal;
	auto sigSize = sig->signal->size;

	ECGSignalChannel tmpSig;
	tmpSig = ECGSignalChannel(new WrappedVector);
	tmpSig->signal = gsl_vector_alloc(sigSize);
	auto signalSize = sig->signal->size;
	int limit=signalSize-5;
	double sumC1=0;
	//double sumC2=0;
	for (int j=0;j<11;j++){
		//double one = gsl_vector_get (signal->channel_one->signal, j);
		sumC1=sumC1+ gsl_vector_get (sig->signal, j);
		//sumC2=sumC2+ gsl_vector_get (sig->signal, j);
	}
	gsl_vector_set(tmpSig->signal, 0, sumC1/11);
	gsl_vector_set(tmpSig->signal, 1, sumC1/11);
	gsl_vector_set(tmpSig->signal, 2, sumC1/11);
	gsl_vector_set(tmpSig->signal, 3, sumC1/11);
	gsl_vector_set(tmpSig->signal, 4, sumC1/11);
	gsl_vector_set(tmpSig->signal, 5, sumC1/11);


	for (int i=6;i<limit;i++){
		sumC1 = sumC1 - gsl_vector_get (sig->signal, i-6) + gsl_vector_get (sig->signal, i+5); 
		gsl_vector_set(tmpSig->signal, i-1, sumC1/11);
	}

	gsl_vector_set(tmpSig->signal, signalSize-1, sumC1/11);
	//gsl_vector_set(tmpSig->signal, signalSize-1, sumC2/11);
	gsl_vector_set(tmpSig->signal, signalSize-2, sumC1/11);
	//gsl_vector_set(tmpSig->signal, signalSize-2, sumC2/11);
	gsl_vector_set(tmpSig->signal, signalSize-3, sumC1/11);
	//gsl_vector_set(tmpSig->signal, signalSize-3, sumC2/11);
	gsl_vector_set(tmpSig->signal, signalSize-4, sumC1/11);
	//gsl_vector_set(tmpSig->signal, signalSize-4, sumC2/11);
	gsl_vector_set(tmpSig->signal, signalSize-5, sumC1/11);
	//gsl_vector_set(tmpSig->signal, signalSize-5, sumC2/11);

	return tmpSig;
}

ECGRs QRSPointsDetector::getMockedRPeak(){

	ECGRs mockedSignal;

	IntSignal rct = IntSignal(new WrappedVectorInt);
	rct->signal = gsl_vector_int_alloc(3);

	gsl_vector_int_set(rct->signal, 0, 137);
	gsl_vector_int_set(rct->signal, 1, 431);
	gsl_vector_int_set(rct->signal, 2, 719);

	mockedSignal.setRs(rct);
	return mockedSignal;
}

ECGSignalChannel QRSPointsDetector::getMockedSignal()
{
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
	return mockedSignal;
}  
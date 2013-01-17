#include "Filter.h"

Filter::Filter(){}
Filter::~Filter(){}

void Filter::zeroPhase(std::vector<double> b, std::vector<double> a, ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, int order)
{
	this->filter(b, a, inputSignal, outputSignal, order);

	auto signalLength = inputSignal->signal->size;

	ECGSignalChannel reverseSignal;
	reverseSignal->signal = gsl_vector_alloc(signalLength);


	for (int i=0; i<signalLength; i++)
	{ 
		auto y1 = gsl_vector_get(outputSignal -> signal, signalLength-i-1);
		//auto y2 = gsl_vector_get(outputSignal.channel_two->signal, signalLength-i-1);

		gsl_vector_set (reverseSignal -> signal, i, y1);
		//gsl_vector_set (reverseSignal.channel_two -> signal, i, y2);
		//x[i]=y[NP-i-1];
	}

	this->filter(b, a, reverseSignal, outputSignal, order);

	for (int i=0; i<signalLength; i++)
	{ 
		auto y1 = gsl_vector_get(outputSignal -> signal, signalLength-i-1);
		//auto y2 = gsl_vector_get(outputSignal.channel_two->signal, signalLength-i-1);

		gsl_vector_set (reverseSignal -> signal, i, y1);
		//gsl_vector_set (reverseSignal.channel_two -> signal, i, y2);
		//x[i]=y[NP-i-1];
	}
	
	for (int i=0; i<signalLength; i++)
	{ 
		auto y1 = gsl_vector_get(reverseSignal-> signal, i);
		//auto y2 = gsl_vector_get(reverseSignal.channel_two->signal, i);

		gsl_vector_set (outputSignal -> signal, i, y1);
		//gsl_vector_set (outputSignal.channel_two -> signal, i, y2);
		//y[i]=x[i];
	}	
}

void Filter::filter(std::vector<double> b, std::vector<double> a, ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, int order)
{
	auto signalLength = inputSignal -> signal->size;

	//y[0]=b[0]*x[0];
	auto x1 = gsl_vector_get(inputSignal -> signal, 0);
	//auto x2 = gsl_vector_get(inputSignal.channel_two->signal, 0);

	gsl_vector_set (outputSignal -> signal, 0, x1*b[0]);
	//gsl_vector_set (outputSignal.channel_two -> signal, i, x2*b[0]);

	for (int i=1; i<order+1; i++)
	{
		//y[i]=0.0;	
		gsl_vector_set (outputSignal -> signal, i, 0.0);
		//gsl_vector_set (outputSignal.channel_two -> signal, i, 0.0);

		for (int j=0; j<i+1; j++)
		{
        	//y[i]=y[i]+b[j]*x[i-j];
			auto y1 = gsl_vector_get(outputSignal -> signal, i);
			//auto y2 = gsl_vector_get(outputSignal.channel_two->signal, i);
			auto x1 = gsl_vector_get(inputSignal -> signal, i-j);
			//auto x2 = gsl_vector_get(inputSignal.channel_two->signal, i-j);

			gsl_vector_set (outputSignal -> signal, i, y1+b[j]*x1);
			//gsl_vector_set (outputSignal.channel_two -> signal, i, y2+b[j]*x2);
		}

		for (int j=0; j<i; j++)
		{
        	//y[i]=y[i]-a[j+1]*y[i-j-1];
			auto y_i1 = gsl_vector_get(outputSignal -> signal, i);
			//auto y_i2 = gsl_vector_get(outputSignal.channel_two->signal, i);
			auto y1 = gsl_vector_get(outputSignal -> signal, i-j-1);
			//auto y2 = gsl_vector_get(outputSignal.channel_two->signal, i-j-1);

			gsl_vector_set (outputSignal -> signal, i, y_i1-a[j+1]*y1);
			//gsl_vector_set (outputSignal.channel_two -> signal, i, y_i2-a[j+1]*y2);
		}
	}

	for (int i=order+1;i<signalLength+1;i++)
	{
		//y[i]=0.0;
		gsl_vector_set (outputSignal -> signal, i, 0.0);
		//gsl_vector_set (outputSignal.channel_two -> signal, i, 0.0);
		
		for (int j=0; j<order+1; j++)
		{
			//y[i]=y[i]+b[j]*x[i-j];
			auto y1 = gsl_vector_get(outputSignal -> signal, i);
			//auto y2 = gsl_vector_get(outputSignal.channel_two->signal, i);
			auto x1 = gsl_vector_get(inputSignal -> signal, i-j);
			//auto x2 = gsl_vector_get(inputSignal.channel_two->signal, i-j);

			gsl_vector_set (outputSignal -> signal, i, y1+b[j]*x1);
			//gsl_vector_set (outputSignal.channel_two -> signal, i, y2+b[j]*x2);
		}

		for (int j=0; j<order; j++)
		{
			//y[i]=y[i]-a[j+1]*y[i-j-1];
			auto y_i1 = gsl_vector_get(outputSignal -> signal, i);
			//auto y_i2 = gsl_vector_get(outputSignal.channel_two->signal, i);
			auto y1 = gsl_vector_get(outputSignal -> signal, i-j-1);
			//auto y2 = gsl_vector_get(outputSignal.channel_two->signal, i-j-1);

			gsl_vector_set (outputSignal -> signal, i, y_i1-a[j+1]*y1);
			//gsl_vector_set (outputSignal.channel_two -> signal, i, y_i2-a[j+1]*y2);
		}
	}
}
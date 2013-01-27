#include "MovingAverage.h"

void MovingAverage::removeBaseline(ECGSignalChannel &inputSignal, ECGSignalChannel &outputSignal, int span)
{
	auto signalLength = outputSignal-> signal -> size;

	for(long index = 0; index < signalLength; index++)
	{
		auto edge = ceil((float)span/2);

		auto inputValue = gsl_vector_get (inputSignal -> signal, index);

		if(index < edge || index > signalLength - edge) 
		{
			gsl_vector_set(outputSignal -> signal, index, inputValue);
		}
		else
		{
			auto avgValue = this->calculateAvgValueOfNeighbours(inputSignal -> signal, index, span);		
			gsl_vector_set (outputSignal -> signal, index, -(inputValue-avgValue));
		}
	}
}

double MovingAverage::calculateAvgValueOfNeighbours(gsl_vector *signal, int currentIndex, int span)
{
	double sum = 0.00;
	for(int i = 1; i <= span/2; i++)
	{
		sum += gsl_vector_get (signal, currentIndex - i);
		sum += gsl_vector_get (signal, currentIndex + i);
	}

	return sum/span;
}
#include "ECGSignal.h"

WrappedVector::WrappedVector (gsl_vector * _signal)
	: signal(_signal)
{}

WrappedVector::~WrappedVector ()
{
  if (signal) gsl_vector_free (signal);
}

WrappedVectorInt::WrappedVectorInt( gsl_vector_int * _signal /*= NULL*/ )
	: signal(_signal)
{}

WrappedVectorInt::~WrappedVectorInt()
{
	if (signal) gsl_vector_int_free (signal);
}

void ECGSignal::setSize(int n)
{
  channel_one = ECGSignalChannel(new WrappedVector);
  channel_two = ECGSignalChannel(new WrappedVector);
}

#include "ECGSignal.h"

WrappedVector::WrappedVector (gsl_vector * _signal)
	: signal(_signal)
{}

WrappedVector::~WrappedVector ()
{
  if (signal) gsl_vector_free (signal);
}

double WrappedVector::get(size_t it)
{
  return gsl_vector_get(signal, it);
}

void WrappedVector::set(size_t it, double value)
{
  gsl_vector_set(signal, it, value);
}


WrappedVectorInt::WrappedVectorInt( gsl_vector_int * _signal /*= NULL*/ )
	: signal(_signal)
{}

WrappedVectorInt::~WrappedVectorInt()
{
	if (signal) gsl_vector_int_free (signal);
}

int WrappedVectorInt::get(size_t it)
{
  return gsl_vector_int_get(signal, it);
}

void WrappedVectorInt::set(size_t it, int value)
{
  gsl_vector_int_set(signal, it, value);
}



void ECGSignal::setSize(size_t n)
{
  channel_one = ECGSignalChannel(new WrappedVector);
  channel_one->signal = gsl_vector_alloc(n);
  channel_two = ECGSignalChannel(new WrappedVector);
  channel_two->signal = gsl_vector_alloc(n);
}

#include "ECGSignal.h"

ECGSignal::ECGSignal (void)
{
  signal = 0;
}

ECGSignal::~ECGSignal (void)
{
  if (signal) gsl_vector_free(signal);
}

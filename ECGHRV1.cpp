#include "ECGHRV1.h"

ECGHRV1::ECGHRV1 (void)
{
  freqency = power = 0;
}

ECGHRV1::~ECGHRV1 (void)
{
  if (freqency) gsl_vector_free (freqency);
  if (power) gsl_vector_free (power);
}

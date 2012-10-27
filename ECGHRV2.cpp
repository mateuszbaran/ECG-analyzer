#include "ECGHRV2.h"

ECGHRV2::ECGHRV2 (void)
{
  histogram_x = poincare_x = histogram_y = poincare_y = 0;
  triple_index_x = triple_index_y = 0;
}

ECGHRV2::~ECGHRV2 (void)
{
  if (histogram_x) gsl_vector_int_free (histogram_x);
  if (histogram_y) gsl_vector_int_free (histogram_y);
  if (poincare_x) gsl_vector_int_free (poincare_x);
  if (poincare_y) gsl_vector_int_free (poincare_y);
  if (triple_index_x) gsl_vector_free (triple_index_x);
  if (triple_index_y) gsl_vector_free (triple_index_y);
}

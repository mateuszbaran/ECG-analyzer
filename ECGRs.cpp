#include "ECGRs.h"

ECGRs::ECGRs (void)
{
  rs = 0;
}

ECGRs::~ECGRs (void)
{
  if (rs) gsl_vector_int_free (rs);
}

#include "ECGWaves.h"

ECGWaves::ECGWaves (void)
{
  T_end = QRS_onset = QRS_end = P_onset = P_end = 0;
}

ECGWaves::~ECGWaves (void)
{
  if (T_end) gsl_vector_int_free (T_end);
  if (QRS_onset) gsl_vector_int_free (QRS_onset);
  if (QRS_end) gsl_vector_int_free (QRS_end);
  if (P_onset) gsl_vector_int_free (P_onset);
  if (P_end) gsl_vector_int_free (P_end);
}

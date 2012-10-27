#include "ECGWaves.h"

ECGWaves::ECGWaves (void)
{
  T_end = QRS_onset = QRS_end = P_onset = P_end = 0;
}

ECGWaves::~ECGWaves (void)
{
  if (T_end) delete T_end;
  if (QRS_onset) delete QRS_onset;
  if (QRS_end) delete QRS_end;
  if (P_onset) delete P_onset;
  if (P_end) delete P_end;
}

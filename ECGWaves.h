#pragma once

#include <vector\gsl_vector_int.h>

class ECGWaves
{
public:
  ECGWaves (void);
  ~ECGWaves (void);
private:
  gsl_vector_int * QRS_onset;
  gsl_vector_int * QRS_end;
  gsl_vector_int * T_end;
  gsl_vector_int * P_onset;
  gsl_vector_int * P_end;
};


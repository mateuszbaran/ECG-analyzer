#pragma once

#include <vector\gsl_vector_int.h>

/**
 * @class Class for storing QRS parameters.
 */
class ECGWaves
{
public:
  ECGWaves (void);
  ~ECGWaves (void);

  gsl_vector_int* GetP_end () const
  {
    return P_end;
  }

  gsl_vector_int* GetP_onset () const
  {
    return P_onset;
  }

  gsl_vector_int* GetQRS_end () const
  {
    return QRS_end;
  }

  gsl_vector_int* GetQRS_onset () const
  {
    return QRS_onset;
  }

  gsl_vector_int* GetT_end () const
  {
    return T_end;
  }

private:
  gsl_vector_int * QRS_onset;
  gsl_vector_int * QRS_end;
  gsl_vector_int * T_end;
  gsl_vector_int * P_onset;
  gsl_vector_int * P_end;
};


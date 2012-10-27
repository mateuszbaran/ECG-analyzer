#pragma once

#include <vector\gsl_vector_int.h>

/**
 * @class R peaks.
 */
class ECGRs
{
public:
  ECGRs (void);
  ~ECGRs (void);

  gsl_vector_int* GetRs () const
  {
    return rs;
  }

private:
  gsl_vector_int * rs;
};


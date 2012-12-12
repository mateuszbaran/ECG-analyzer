#pragma once

#include "ECGSignal.h"

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
    return rsco;
  }

  void setRsChannelOne(gsl_vector_int* rs);
  void setRsChannelTwo(gsl_vector_int* rs);

private:
  gsl_vector_int* rsco;
  gsl_vector_int* rsct;
};
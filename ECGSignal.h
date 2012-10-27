#pragma once

#include <vector\gsl_vector.h>

/**
 * @class Raw signal ECG.
 */
class ECGSignal
{
public:
  ECGSignal (void);
  ~ECGSignal (void);

  /**
   * Get signal.
   * @return signal as pointer to gsl_vector.
   */
  gsl_vector * getSignal () const;
private:
  //signal
  gsl_vector* signal;
};


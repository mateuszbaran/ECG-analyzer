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

  IntSignal GetRs () const
  {
    return rs;
  }

private:
  IntSignal rs;
};


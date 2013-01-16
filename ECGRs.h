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

  void setRs(IntSignal rs);

private:
  IntSignal rs;
};
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
    return rsco;
  }

  void setRsChannelOne(IntSignal rs);
  void setRsChannelTwo(IntSignal rs);

private:
  IntSignal rsco;
  IntSignal rsct;
};
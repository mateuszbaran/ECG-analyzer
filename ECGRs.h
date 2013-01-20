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
  
  size_t count() const;

private:
  IntSignal rs;
};
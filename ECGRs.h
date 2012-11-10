#pragma once

#include "ECGSignal.h"
#include "ModulesMethods.h"

/**
 * @class R peaks.
 */
class ECGRs
{
public:

  ECGRs ();
  ~ECGRs ();

  IntSignal GetRs () const
  {
    return rs;
  }

private:

  IntSignal rs;

};


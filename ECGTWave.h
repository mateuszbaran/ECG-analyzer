#pragma once

#include "ECGSignal.h"

/**
 * @class Class for parameters created in TWaveAlt module
 */
class ECGTWave
{
public:
  ECGTWave (void);
  ~ECGTWave (void);

  IntSignal getTWaveAlt () const
  {
    return twa;
  }

  void setTWaveAlt(IntSignal twa);

  //quality parameters
  double numberOfWinDetected;
  double percentageOfWinDetected;

private:
  //data for plotting t-alternans
  IntSignal twa;

};

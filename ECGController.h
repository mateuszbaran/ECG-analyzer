#pragma once

#include "ECGSignal.h"
#include "ECGRs.h"
#include "ECGWaves.h"
#include "ECGHRV1.h"
#include "ECGHRV2.h"
#include "ECGHRVDFA.h"
#include "ECGClasses.h"
#include "ECGHRT.h"
#include "ECGST.h"
#include "ECGTWave.h"

/**
 * @class Class which controls executing all modules.
 */
class ECGController
{
public:
  ECGController (void);
  ~ECGController (void);
private:
  ECGSignal raw_signal;
  ECGSignal filtered_signal;
  ECGRs r_peaks;
  ECGWaves waves;
  ECGHRV1 hrv1;
  ECGHRV2 hrv2;
  ECGHRVDFA ecghrvdfa;
  ECGClasses ecgclasses;
  ECGST ecgst;
  ECGTWave ecgtwave;
  ECGHRT ecghrt;
};


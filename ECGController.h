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

#include "ModulesInterfaces.h"
#include "ModulesMethods.h"

/**
 * @class Class which controls executing all modules.
 */
class ECGController
{
public:
  ECGController (void);
  ~ECGController (void);

  void runECGBaseline(ParametersTypes & params);
  void runRPeaks(ParametersTypes & params);
  void runWaves(ParametersTypes & params);
  void runHRV1(ParametersTypes & params);
  void runHRV2(ParametersTypes & params);
  void runHRVDFA(ParametersTypes & params);
  void runQRSClass(ParametersTypes & params);
  void runSTInterval(ParametersTypes & params);
  void runTwaveAlt(ParametersTypes & params);
  void runHRT(ParametersTypes & params);

private:
  //data
  ECGSignal raw_signal;
  ECGSignal filtered_signal;
  ECGRs r_peaks_data;
  ECGWaves waves_data;
  ECGHRV1 hrv1_data;
  ECGHRV2 hrv2_data;
  ECGHRVDFA hrv_dfa_data;
  ECGClasses classes_data;
  ECGST st_data;
  ECGTWave twave_data;
  ECGHRT hrt_data;

  //modules
  ECGBaselineModule *ecg_baseline_module;
  RPeaksModule *rpeaks_module;
  WavesModule *waves_module;
  HRV1Module *hrv1_module;
  HRV2Module *hrv2_module;
  HRVDFAModule *hrv_dfa_module;
  QRSClassModule *qrs_class_module;
  STIntervalModule *st_interval_module;
  TWaveAltModule *t_wave_alt_module;
  HRT *hrt_module;
};


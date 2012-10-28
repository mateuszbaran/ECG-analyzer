#include "ECGController.h"

#include "wfdb\wfdb.h"

ECGController::ECGController (void)
{
  //TODO: create modules objects
}

ECGController::~ECGController (void) { }

void ECGController::setParamsECGBaseline (ParametersTypes & params)
{
  ecg_baseline_module->setParams(params);
  setECGBaselineNotRunned();
}

void ECGController::setParamsRPeaks (ParametersTypes & params)
{
  rpeaks_module->setParams(params);
  setRPeaksNotRunned();
}

void ECGController::setParamsnWaves (ParametersTypes & params)
{
  waves_module->setParams(params);
  setWavesNotRunned();
}

void ECGController::setParamsHRV1 (ParametersTypes & params)
{
  hrv1_module->setParams(params);
  setHRV1NotRunned();
}

void ECGController::setParamsHRV2 (ParametersTypes & params)
{
  hrv2_module->setParams(params);
  setHRV2NotRunned();
}

void ECGController::setParamsHRVDFA (ParametersTypes & params)
{
  hrv_dfa_module->setParams(params);
  setHRVDFANotRunned();
}

void ECGController::setParamsQRSClass (ParametersTypes & params)
{
  qrs_class_module->setParams(params);
  setQRSClassNotRunned();
}

void ECGController::setParamsSTInterval (ParametersTypes & params)
{
  st_interval_module->setParams(params);
  setSTIntervalNotRunned();
}

void ECGController::setParamsTwaveAlt (ParametersTypes & params)
{
  t_wave_alt_module->setParams(params);
  setTwaveAltNotRunned();
}

void ECGController::setParamsHRT (ParametersTypes & params)
{
  hrt_module->setParams(params);
  setHRTNotRunned();
}

void ECGController::runECGBaseline ()
{
  ecg_baseline_module->runModule(raw_signal, filtered_signal);
  ecg_baseline_module->runned = true;
}

void ECGController::runRPeaks ()
{
  if (!ecg_baseline_module->runned)
  {
    runECGBaseline();
  }
  rpeaks_module->runModule(filtered_signal, r_peaks_data);
}

void ECGController::runWaves ()
{
  if (!ecg_baseline_module->runned)
  {
    runECGBaseline();
  }
  if (!rpeaks_module->runned) 
  {
    runRPeaks();
  }
  waves_module->runModule(filtered_signal, r_peaks_data, waves_data);
}

void ECGController::runHRV1 ()
{
  if (!rpeaks_module->runned)
  {
    runRPeaks();
  }
  hrv1_module->runModule(r_peaks_data, hrv1_data);
}

void ECGController::runHRV2 ()
{
  if (!rpeaks_module->runned)
  {
    runRPeaks();
  }
  hrv2_module->runModule(r_peaks_data, hrv2_data);
}

void ECGController::runHRVDFA ()
{
  if (!rpeaks_module->runned)
  {
    runRPeaks();
  }
  hrv_dfa_module->runModule(r_peaks_data, hrv_dfa_data);
}

void ECGController::runQRSClass ()
{
  if (!ecg_baseline_module->runned)
  {
    runECGBaseline();
  }
  if (!waves_module->runned)
  {
    runWaves();
  }
  qrs_class_module->runModule(waves_data, filtered_signal, classes_data);
}

void ECGController::runSTInterval ()
{
  if (!ecg_baseline_module->runned)
  {
    runECGBaseline();
  }
  if (!waves_module->runned)
  {
    runWaves();
  }
  st_interval_module->runModule(waves_data, filtered_signal, st_data);
}

void ECGController::runTwaveAlt ()
{
  if (!ecg_baseline_module->runned)
  {
    runECGBaseline();
  }
  if (!waves_module->runned)
  {
    runWaves();
  }
  t_wave_alt_module->runModule(waves_data, filtered_signal, twave_data);
}

void ECGController::runHRT ()
{
  if (!ecg_baseline_module->runned)
  {
    runECGBaseline();
  }
  if (!waves_module->runned)
  {
    runWaves();
  }
  if (!qrs_class_module->runned)
  {
    runQRSClass();
  }
  hrt_module->runModule(waves_data, r_peaks_data, filtered_signal, hrt_data);
}

void ECGController::setECGBaselineNotRunned()
{
  ecg_baseline_module->runned = false;
  setRPeaksNotRunned();
  setQRSClassNotRunned();
  setSTIntervalNotRunned();
  setTwaveAltNotRunned();
  setHRTNotRunned();
  setWavesNotRunned();
}

void ECGController::setRPeaksNotRunned()
{
  rpeaks_module->runned = false;
  setHRTNotRunned();
  setWavesNotRunned();
  setHRV1NotRunned();
  setHRV2NotRunned();
  setHRVDFANotRunned();
}

void ECGController::setWavesNotRunned()
{
  waves_module->runned = false;
  setQRSClassNotRunned();
  setSTIntervalNotRunned();
  setTwaveAltNotRunned();
  setHRTNotRunned();
}

void ECGController::setHRV1NotRunned()
{
  hrv1_module->runned = false;
}

void ECGController::setHRV2NotRunned()
{
  hrv2_module->runned = false;
}

void ECGController::setHRVDFANotRunned()
{
  hrv_dfa_module->runned = false;
}

void ECGController::setQRSClassNotRunned()
{
  qrs_class_module->runned = false;
  setHRTNotRunned();
}

void ECGController::setSTIntervalNotRunned()
{
  st_interval_module->runned = false;
}

void ECGController::setTwaveAltNotRunned()
{
  t_wave_alt_module->runned = false;
}

void ECGController::setHRTNotRunned()
{
  hrt_module->runned = false;
}

bool ECGController::readFile(std::string filename)
{
  /*
  int i;
  WFDB_Sample v[2];
  WFDB_Siginfo s[2];
  if (isigopen("100s", s, 2) < 2)
    exit(1);
  for (i = 0; i < 10; i++) {
    if (getvec(v) < 0)
      break;
    printf("%d\t%d\n", v[0], v[1]);
  }*/
  return true;
}
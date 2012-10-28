#include "ECGController.h"
#include <Windows.h>

ECGController::ECGController (void) {
  //int i = isigopen("100s", NULL, 0);
}

ECGController::~ECGController (void) { }

void ECGController::setParamsECGBaseline (ParametersTypes & params)
{
  ecg_baseline_module->setParams(params);
}

void ECGController::setParamsRPeaks (ParametersTypes & params)
{
  rpeaks_module->setParams(params);
}

void ECGController::setParamsnWaves (ParametersTypes & params)
{
  waves_module->setParams(params);
}

void ECGController::setParamsHRV1 (ParametersTypes & params)
{
  hrv1_module->setParams(params);
}

void ECGController::setParamsHRV2 (ParametersTypes & params)
{
  hrv2_module->setParams(params);
}

void ECGController::setParamsHRVDFA (ParametersTypes & params)
{
  hrv_dfa_module->setParams(params);
}

void ECGController::setParamsQRSClass (ParametersTypes & params)
{
  qrs_class_module->setParams(params);
}

void ECGController::setParamsSTInterval (ParametersTypes & params)
{
  st_interval_module->setParams(params);
}

void ECGController::setParamsTwaveAlt (ParametersTypes & params)
{
  t_wave_alt_module->setParams(params);
}

void ECGController::setParamsHRT (ParametersTypes & params)
{
  hrt_module->setParams(params);
}

void ECGController::runECGBaseline ()
{
  ecg_baseline_module->runModule(raw_signal, filtered_signal);
}

void ECGController::runRPeaks ()
{
  rpeaks_module->runModule(filtered_signal, r_peaks_data);
}

void ECGController::runWaves ()
{
  waves_module->runModule(filtered_signal, r_peaks_data, waves_data);
}

void ECGController::runHRV1 ()
{
  hrv1_module->runModule(r_peaks_data, hrv1_data);
}

void ECGController::runHRV2 ()
{
  hrv2_module->runModule(r_peaks_data, hrv2_data);
}

void ECGController::runHRVDFA ()
{
  hrv_dfa_module->runModule(r_peaks_data, hrv_dfa_data);
}

void ECGController::runQRSClass ()
{
  qrs_class_module->runModule(waves_data, filtered_signal, classes_data);
}

void ECGController::runSTInterval ()
{
  st_interval_module->runModule(waves_data, filtered_signal, st_data);
}

void ECGController::runTwaveAlt ()
{
  t_wave_alt_module->runModule(waves_data, filtered_signal, twave_data);
}

void ECGController::runHRT ()
{
  hrt_module->runModule(waves_data, r_peaks_data, filtered_signal, hrt_data);
}
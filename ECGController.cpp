#include "ECGController.h"
#include <Windows.h>

ECGController::ECGController (void) {
  //int i = isigopen("100s", NULL, 0);
}

ECGController::~ECGController (void) { }

void ECGController::runECGBaseline (ParametersTypes & params)
{
  ecg_baseline_module->setParams(params);
  ecg_baseline_module->runModule(raw_signal, filtered_signal);
}

void ECGController::runRPeaks (ParametersTypes & params)
{
  rpeaks_module->setParams(params);
  rpeaks_module->runModule(filtered_signal, r_peaks_data);
}

void ECGController::runWaves (ParametersTypes & params)
{
  waves_module->setParams(params);
  waves_module->runModule(filtered_signal, r_peaks_data, waves_data);
}

void ECGController::runHRV1 (ParametersTypes & params)
{
  hrv1_module->setParams(params);
  hrv1_module->runModule(r_peaks_data, hrv1_data);
}

void ECGController::runHRV2 (ParametersTypes & params)
{
  hrv2_module->setParams(params);
  hrv2_module->runModule(r_peaks_data, hrv2_data);
}

void ECGController::runHRVDFA (ParametersTypes & params)
{
  hrv_dfa_module->setParams(params);
  hrv_dfa_module->runModule(r_peaks_data, hrv_dfa_data);
}

void ECGController::runQRSClass (ParametersTypes & params)
{
  qrs_class_module->setParams(params);
  qrs_class_module->runModule(waves_data, filtered_signal, classes_data);
}

void ECGController::runSTInterval (ParametersTypes & params)
{
  st_interval_module->setParams(params);
  st_interval_module->runModule(waves_data, filtered_signal, st_data);
}

void ECGController::runTwaveAlt (ParametersTypes & params)
{
  t_wave_alt_module->setParams(params);
  t_wave_alt_module->runModule(waves_data, filtered_signal, twave_data);
}

void ECGController::runHRT (ParametersTypes & params)
{
  hrt_module->setParams(params);
  hrt_module->runModule(waves_data, r_peaks_data, filtered_signal, hrt_data);
}
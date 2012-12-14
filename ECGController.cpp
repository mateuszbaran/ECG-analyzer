#include "ECGController.h"

#include "BaselineRemoval.h"
#include "RPeaksDetector.h"

#include "wfdb/wfdb.h"

#include "tri_logger.hpp"

ECGController::ECGController (void) :
  ecg_baseline_module(new BaselineRemoval()),
  rpeaks_module(new RPeaksDetector())
{
  TRI_LOG_STR("ECGController created");
  //TODO: create modules objects
}

ECGController::~ECGController (void)
{
  TRI_LOG_STR("ECGController destroyed");
}

void ECGController::setParamsECGBaseline (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  ecg_baseline_module->setParams(params);
  setECGBaselineNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsRPeaks (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  rpeaks_module->setParams(params);
  setRPeaksNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsnWaves (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  waves_module->setParams(params);
  setWavesNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsHRV1 (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  hrv1_module->setParams(params);
  setHRV1NotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsHRV2 (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  hrv2_module->setParams(params);
  setHRV2NotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsHRVDFA (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  hrv_dfa_module->setParams(params);
  setHRVDFANotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsQRSClass (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  qrs_class_module->setParams(params);
  setQRSClassNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsSTInterval (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  st_interval_module->setParams(params);
  setSTIntervalNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsTwaveAlt (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  t_wave_alt_module->setParams(params);
  setTwaveAltNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setParamsHRT (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  hrt_module->setParams(params);
  setHRTNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runECGBaseline ()
{
  TRI_LOG_STR(__FUNCTION__);
  ecg_baseline_module->runModule(raw_signal, ecg_info, filtered_signal);
  ecg_baseline_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runRPeaks ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  rpeaks_module->runModule(filtered_signal, ecg_info, r_peaks_data);
  rpeaks_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runWaves ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!rpeaks_module->run_) 
  {
    runRPeaks();
  }
  waves_module->runModule(filtered_signal, ecg_info, r_peaks_data, waves_data);
  waves_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runHRV1 ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!rpeaks_module->run_)
  {
    runRPeaks();
  }
  hrv1_module->runModule(r_peaks_data, hrv1_data);
  hrv1_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runHRV2 ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!rpeaks_module->run_)
  {
    runRPeaks();
  }
  hrv2_module->runModule(r_peaks_data, hrv2_data);
  hrv2_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runHRVDFA ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!rpeaks_module->run_)
  {
    runRPeaks();
  }
  hrv_dfa_module->runModule(r_peaks_data, hrv_dfa_data);
  hrv_dfa_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runQRSClass ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!waves_module->run_)
  {
    runWaves();
  }
  qrs_class_module->runModule(waves_data, filtered_signal, ecg_info, classes_data);
  qrs_class_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runSTInterval ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!waves_module->run_)
  {
    runWaves();
  }
  st_interval_module->runModule(waves_data, filtered_signal, ecg_info, st_data);
  st_interval_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runTwaveAlt ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!waves_module->run_)
  {
    runWaves();
  }
  t_wave_alt_module->runModule(waves_data, filtered_signal, ecg_info, twave_data);
  t_wave_alt_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::runHRT ()
{
  TRI_LOG_STR(__FUNCTION__);
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!waves_module->run_)
  {
    runWaves();
  }
  if (!qrs_class_module->run_)
  {
    runQRSClass();
  }
  hrt_module->runModule(waves_data, r_peaks_data, filtered_signal, ecg_info, hrt_data);
  hrt_module->run_ = true;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setECGBaselineNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  ecg_baseline_module->run_ = false;
  setRPeaksNotRun();
  setQRSClassNotRun();
  setSTIntervalNotRun();
  setTwaveAltNotRun();
  setHRTNotRun();
  setWavesNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setRPeaksNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  rpeaks_module->run_ = false;
  setHRTNotRun();
  setWavesNotRun();
  setHRV1NotRun();
  setHRV2NotRun();
  setHRVDFANotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setWavesNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  waves_module->run_ = false;
  setQRSClassNotRun();
  setSTIntervalNotRun();
  setTwaveAltNotRun();
  setHRTNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setHRV1NotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  hrv1_module->run_ = false;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setHRV2NotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  hrv2_module->run_ = false;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setHRVDFANotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  hrv_dfa_module->run_ = false;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setQRSClassNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  qrs_class_module->run_ = false;
  setHRTNotRun();
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setSTIntervalNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  st_interval_module->run_ = false;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setTwaveAltNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  t_wave_alt_module->run_ = false;
  TRI_LOG_STR("END: "__FUNCTION__);
}

void ECGController::setHRTNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  hrt_module->run_ = false;
  TRI_LOG_STR("END: "__FUNCTION__);
}

bool ECGController::readFile(std::string filename)
{
  TRI_LOG_STR(__FUNCTION__);
  //check filename to be correct with WFDB
  int pos;
  if ((pos = filename.find('.')) != std::string::npos)
  {
    filename = filename.substr(0, pos);
  }

  int nr_samples;

  size_t i;
  WFDB_Sample v[2];
  WFDB_Siginfo s[2];
  if (isigopen(const_cast<char*> (filename.c_str()), s, 2) < 2)
  {
    TRI_LOG_STR("File");
    TRI_LOG(filename);
    TRI_LOG_STR("Not loaded");
    TRI_LOG_STR("END: "__FUNCTION__);
    return false;
  }
  TRI_LOG_STR("File: ");
  TRI_LOG(filename);
  TRI_LOG_STR("loaded");
  TRI_LOG(s[0].nsamp);

  //read channels info
  ecg_info.channel_one.filename = s[0].fname;
  ecg_info.channel_two.description = s[0].desc;

  ecg_info.channel_one.filename = s[1].fname;
  ecg_info.channel_two.description = s[1].desc;

  ecg_info.channel_one.frequecy = 360; //from mit-bih description
  ecg_info.channel_two.frequecy = 360; //from mit-bih description

  ecg_info.channel_one.gain = s[0].gain;
  ecg_info.channel_two.gain = s[1].gain;

  ecg_info.channel_one.nr_samples = s[0].nsamp;
  ecg_info.channel_two.nr_samples = s[1].nsamp;

  ecg_info.channel_one.range = 10;
  ecg_info.channel_two.range = 10;

  ecg_info.channel_one.signal_resolution = s[0].adcres;
  ecg_info.channel_two.signal_resolution = s[1].adcres;
  
  ecg_info.channel_one.zero_signal = s[0].adczero;
  ecg_info.channel_two.zero_signal = s[1].adczero;
 
  //set signal
  nr_samples = s[0].nsamp; //we assume that both channels are equal.
  raw_signal.setSize(nr_samples);

  //alocate memory for filtered signal
  filtered_signal.setSize(nr_samples);

  //read signals
  for (i = 0; i < nr_samples; i++)
  {
    if (getvec(v) < 0) //error
    {
      TRI_LOG_STR("Error during reading signal");
      return false;
    }
    gsl_vector_set(raw_signal.channel_one->signal, i, (double)v[0]);
    gsl_vector_set(raw_signal.channel_two->signal, i, (double)v[1]);
  }
  TRI_LOG_STR("END: "__FUNCTION__);
  TRI_LOG_STR(" returns true");
  return true;
}

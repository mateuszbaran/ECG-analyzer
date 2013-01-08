#include "ECGController.h"

#include "BaselineRemoval.h"
#include "RPeaksDetector.h"
#include "HRV1Analyzer.h"

#include "wfdb/wfdb.h"

#include "tri_logger.hpp"

#define LOG_END TRI_LOG_STR("END: " << __FUNCTION__);

ECGController::ECGController (void) :
  ecg_baseline_module(new BaselineRemoval()),
  rpeaks_module(new RPeaksDetector()),
  hrv1_module(new HRV1Analyzer())
{
  TRI_LOG_STR("ECGController created, 20:51 17-12-2012");
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
  LOG_END
}

void ECGController::setParamsRPeaks (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  rpeaks_module->setParams(params);
  setRPeaksNotRun();
  LOG_END
}

void ECGController::setParamsnWaves (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  waves_module->setParams(params);
  setWavesNotRun();
  LOG_END
}

void ECGController::setParamsHRV1 (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  hrv1_module->setParams(params);
  setHRV1NotRun();
  LOG_END
}

void ECGController::setParamsHRV2 (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  hrv2_module->setParams(params);
  setHRV2NotRun();
  LOG_END
}

void ECGController::setParamsHRVDFA (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  hrv_dfa_module->setParams(params);
  setHRVDFANotRun();
  LOG_END
}

void ECGController::setParamsQRSClass (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  qrs_class_module->setParams(params);
  setQRSClassNotRun();
  LOG_END
}

void ECGController::setParamsSTInterval (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  st_interval_module->setParams(params);
  setSTIntervalNotRun();
  LOG_END
}

void ECGController::setParamsTwaveAlt (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  t_wave_alt_module->setParams(params);
  setTwaveAltNotRun();
  LOG_END
}

void ECGController::setParamsHRT (ParametersTypes & params)
{
  TRI_LOG_STR(__FUNCTION__);
  hrt_module->setParams(params);
  setHRTNotRun();
  LOG_END
}

void ECGController::runECGBaseline ()
{
  TRI_LOG_STR(__FUNCTION__);
  ecg_baseline_module->runModule(raw_signal, ecg_info, filtered_signal);
  ecg_baseline_module->run_ = true;
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
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
  LOG_END
}

void ECGController::setWavesNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  waves_module->run_ = false;
  setQRSClassNotRun();
  setSTIntervalNotRun();
  setTwaveAltNotRun();
  setHRTNotRun();
  LOG_END
}

void ECGController::setHRV1NotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  hrv1_module->run_ = false;
  LOG_END
}

void ECGController::setHRV2NotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  hrv2_module->run_ = false;
  LOG_END
}

void ECGController::setHRVDFANotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  hrv_dfa_module->run_ = false;
  LOG_END
}

void ECGController::setQRSClassNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  qrs_class_module->run_ = false;
  setHRTNotRun();
  LOG_END
}

void ECGController::setSTIntervalNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  st_interval_module->run_ = false;
  LOG_END
}

void ECGController::setTwaveAltNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  t_wave_alt_module->run_ = false;
  LOG_END
}

void ECGController::setHRTNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  hrt_module->run_ = false;
  LOG_END
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
    LOG_END
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
  LOG_END
  TRI_LOG_STR(" returns true");
  return true;
}

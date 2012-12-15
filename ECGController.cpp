#include "ECGController.h"

#include "BaselineRemoval.h"
#include "RPeaksDetector.h"
#include "HRV1Analyzer.h"

#include "wfdb/wfdb.h"

ECGController::ECGController (void) :
  ecg_baseline_module(new BaselineRemoval()),
  rpeaks_module(new RPeaksDetector()),
  hrv1_module(new HRV1Analyzer())
{
  //TODO: create modules objects
}

ECGController::~ECGController (void) { }

void ECGController::setParamsECGBaseline (ParametersTypes & params)
{
  ecg_baseline_module->setParams(params);
  setECGBaselineNotRun();
}

void ECGController::setParamsRPeaks (ParametersTypes & params)
{
  rpeaks_module->setParams(params);
  setRPeaksNotRun();
}

void ECGController::setParamsnWaves (ParametersTypes & params)
{
  waves_module->setParams(params);
  setWavesNotRun();
}

void ECGController::setParamsHRV1 (ParametersTypes & params)
{
  hrv1_module->setParams(params);
  setHRV1NotRun();
}

void ECGController::setParamsHRV2 (ParametersTypes & params)
{
  hrv2_module->setParams(params);
  setHRV2NotRun();
}

void ECGController::setParamsHRVDFA (ParametersTypes & params)
{
  hrv_dfa_module->setParams(params);
  setHRVDFANotRun();
}

void ECGController::setParamsQRSClass (ParametersTypes & params)
{
  qrs_class_module->setParams(params);
  setQRSClassNotRun();
}

void ECGController::setParamsSTInterval (ParametersTypes & params)
{
  st_interval_module->setParams(params);
  setSTIntervalNotRun();
}

void ECGController::setParamsTwaveAlt (ParametersTypes & params)
{
  t_wave_alt_module->setParams(params);
  setTwaveAltNotRun();
}

void ECGController::setParamsHRT (ParametersTypes & params)
{
  hrt_module->setParams(params);
  setHRTNotRun();
}

void ECGController::runECGBaseline ()
{
  ecg_baseline_module->runModule(raw_signal, ecg_info, filtered_signal);
  ecg_baseline_module->run_ = true;
}

void ECGController::runRPeaks ()
{
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  rpeaks_module->runModule(filtered_signal, ecg_info, r_peaks_data);
}

void ECGController::runWaves ()
{
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!rpeaks_module->run_) 
  {
    runRPeaks();
  }
  waves_module->runModule(filtered_signal, ecg_info, r_peaks_data, waves_data);
}

void ECGController::runHRV1 ()
{
  if (!rpeaks_module->run_)
  {
    runRPeaks();
  }
  hrv1_module->runModule(r_peaks_data, hrv1_data);
}

void ECGController::runHRV2 ()
{
  if (!rpeaks_module->run_)
  {
    runRPeaks();
  }
  hrv2_module->runModule(r_peaks_data, hrv2_data);
}

void ECGController::runHRVDFA ()
{
  if (!rpeaks_module->run_)
  {
    runRPeaks();
  }
  hrv_dfa_module->runModule(r_peaks_data, hrv_dfa_data);
}

void ECGController::runQRSClass ()
{
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!waves_module->run_)
  {
    runWaves();
  }
  qrs_class_module->runModule(waves_data, filtered_signal, ecg_info, classes_data);
}

void ECGController::runSTInterval ()
{
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!waves_module->run_)
  {
    runWaves();
  }
  st_interval_module->runModule(waves_data, filtered_signal, ecg_info, st_data);
}

void ECGController::runTwaveAlt ()
{
  if (!ecg_baseline_module->run_)
  {
    runECGBaseline();
  }
  if (!waves_module->run_)
  {
    runWaves();
  }
  t_wave_alt_module->runModule(waves_data, filtered_signal, ecg_info, twave_data);
}

void ECGController::runHRT ()
{
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
}

void ECGController::setECGBaselineNotRun()
{
  ecg_baseline_module->run_ = false;
  setRPeaksNotRun();
  setQRSClassNotRun();
  setSTIntervalNotRun();
  setTwaveAltNotRun();
  setHRTNotRun();
  setWavesNotRun();
}

void ECGController::setRPeaksNotRun()
{
  rpeaks_module->run_ = false;
  setHRTNotRun();
  setWavesNotRun();
  setHRV1NotRun();
  setHRV2NotRun();
  setHRVDFANotRun();
}

void ECGController::setWavesNotRun()
{
  waves_module->run_ = false;
  setQRSClassNotRun();
  setSTIntervalNotRun();
  setTwaveAltNotRun();
  setHRTNotRun();
}

void ECGController::setHRV1NotRun()
{
  hrv1_module->run_ = false;
}

void ECGController::setHRV2NotRun()
{
  hrv2_module->run_ = false;
}

void ECGController::setHRVDFANotRun()
{
  hrv_dfa_module->run_ = false;
}

void ECGController::setQRSClassNotRun()
{
  qrs_class_module->run_ = false;
  setHRTNotRun();
}

void ECGController::setSTIntervalNotRun()
{
  st_interval_module->run_ = false;
}

void ECGController::setTwaveAltNotRun()
{
  t_wave_alt_module->run_ = false;
}

void ECGController::setHRTNotRun()
{
  hrt_module->run_ = false;
}

bool ECGController::readFile(std::string filename)
{
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
    return false;
  }

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
      return false;
    }
    gsl_vector_set(raw_signal.channel_one->signal, i, (double)v[0]);
    gsl_vector_set(raw_signal.channel_two->signal, i, (double)v[1]);
  }
  return true;
}

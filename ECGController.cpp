#include "ECGController.h"

#include "wfdb/wfdb.h"

ECGController::ECGController (void) :
  raw_signal(ECGSignal(new WrappedVector())),
  filtered_signal(ECGSignal(new WrappedVector())),
  original_signal_channel_one(ECGSignal(new WrappedVector())),
  original_signal_channel_two(ECGSignal(new WrappedVector()))
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
  channel_one_info.filename = s[0].fname;
  channel_one_info.description = s[0].desc;

  channel_two_info.filename = s[1].fname;
  channel_two_info.description = s[1].desc;
 
  //set signal
  nr_samples = s[0].nsamp; //we assume that both channels are equal.
  original_signal_channel_one->signal = gsl_vector_alloc(nr_samples);
  original_signal_channel_two->signal = gsl_vector_alloc(nr_samples);

  //alocate memory for filtered signal
  filtered_signal->signal = gsl_vector_alloc(nr_samples);

  //read signals
  for (i = 0; i < nr_samples; i++)
  {
    if (getvec(v) < 0) //error
    {
      return false;
    }
    gsl_vector_set(original_signal_channel_one->signal, i, (double)v[0]);
    gsl_vector_set(original_signal_channel_two->signal, i, (double)v[1]);
  }
  return true;
}

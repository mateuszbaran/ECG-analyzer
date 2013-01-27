#include "ECGController.h"

#include "ECGBaselineRemoval.h"
#include "HRV1Analyzer.h"
#include "HRTAnalyzer.h"
#include "DFAAnalyzer.h"
#include "RPeaksDetector.h"
#include "STAnalysis.h"
#include "QRSPointsDetector.h"
#include "DFAAnalyzer.h"
#include "GeometricAnalysis.h"
#include "TWaveAltDetector.h"
#include "QRSPointsDetector.h"

#include "wfdb/wfdb.h"

#include "tri_logger.hpp"

#include <boost/thread.hpp>

#define LOG_END TRI_LOG_STR("END: " << __FUNCTION__);

ECGController::ECGController (void) :
  ecg_baseline_module(new ECGBaselineRemoval()),
  rpeaks_module(new RPeaksDetector()),
  hrv_dfa_module(new DFAAnalyzer()),
  hrt_module(new HRTAnalyzer()),
  waves_module(NULL),
  qrs_class_module(new QRSPointsDetector()),
  t_wave_alt_module(new TWaveAltDetector()),
  hrv1_module(new HRV1Analyzer()),
  hrv2_module(new GeometricAnalysis()),
  st_interval_module(new STAnalysis()),
  computation(NULL),
  analysisCompl(false)
{
  TRI_LOG_STR("ECGController created, 20:51 17-12-2012");
  //TODO: create modules objects
}

ECGController::~ECGController (void)
{
  TRI_LOG_STR("ECGController destroyed");
  delete computation;
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
  hrv1_module->runModule(ecg_info, r_peaks_data, hrv1_data);
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
  hrv2_module->runModule(ecg_info,r_peaks_data, hrv2_data);
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
  if (waves_module && !waves_module->run_)
  {
    runWaves();
  }
  qrs_class_module->runModule(waves_data, filtered_signal, r_peaks_data, ecg_info, classes_data);
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
  if (!rpeaks_module->run_)
  {
    runRPeaks();
  }
  if (waves_module && !waves_module->run_)
  {
    runWaves();
  }
  st_interval_module->runModule(r_peaks_data, waves_data, filtered_signal, ecg_info, st_data);
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
  if (waves_module && !waves_module->run_)
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
  if (!rpeaks_module->run_)
  {
    runRPeaks();
  }
  if (!qrs_class_module->run_)
  {
    runQRSClass();
  }
  hrt_module->runModule(r_peaks_data, qrsclass_data, ecg_info, hrt_data);
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
  if(waves_module)
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
  if(hrv2_module)
	hrv2_module->run_ = false;
  LOG_END
}

void ECGController::setHRVDFANotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  if(hrv_dfa_module)
	hrv_dfa_module->run_ = false;
  LOG_END
}

void ECGController::setQRSClassNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  if(qrs_class_module)
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
  if(t_wave_alt_module)
	t_wave_alt_module->run_ = false;
  LOG_END
}

void ECGController::setHRTNotRun()
{
  TRI_LOG_STR(__FUNCTION__);
  if(hrt_module)
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
  filtered_signal = ECGSignalChannel(new WrappedVector);
  filtered_signal->signal = gsl_vector_alloc(nr_samples);

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

void ECGController::rerunAnalysis( std::function<void(std::string)> statusUpdate, std::function<void()> analysisComplete )
{
	TRI_LOG_STR(__FUNCTION__);

	//cleaning after previous computation/stopping it
	if(computation)
	{
		computation->interrupt();
		computation->join();
		delete computation;
		computation = NULL;
	}
	if(statusUpdate && analysisComplete)
	{

#define HANDLE_INTERRUPTION							\
	try {											\
		boost::this_thread::interruption_point(); } \
	catch(...) {									\
		return;										\
	}

		analysisCompl = false;
		computation = new boost::thread([=](){
			statusUpdate("Analysis begins");
			runECGBaseline();
			HANDLE_INTERRUPTION
			statusUpdate("Baseline removal completed; R peaks detection ongoing.");
			runRPeaks();
			HANDLE_INTERRUPTION
			statusUpdate("R peaks detection completed; HRV1 analysis ongoing.");
			//runHRV1();
			HANDLE_INTERRUPTION
			statusUpdate("HRV1 analysis completed; HRV2 analysis ongoing.");
			runHRV2();
			HANDLE_INTERRUPTION
			statusUpdate("HRV2 analysis completed; waves analysis ongoing.");
			//runWaves();
			HANDLE_INTERRUPTION
			statusUpdate("ST segment analysis completed; QRS analysis ongoing.");
			runQRSClass();
			HANDLE_INTERRUPTION
			statusUpdate("QRS analysis completed; ST interval analysis ongoing.");
			runSTInterval();
			HANDLE_INTERRUPTION
			statusUpdate("ST interval analysis completed; HRVDFA analysis ongoing.");
			runHRVDFA();
			HANDLE_INTERRUPTION
			statusUpdate("HRVDFA analysis completed; T wave alternans analysis ongoing.");
			//runTwaveAlt();
			HANDLE_INTERRUPTION
			statusUpdate("T wave alternans analysis completed; HRT analysis ongoing.");
			runHRT();
			statusUpdate("Analysis complete!");
			analysisComplete();
			analysisCompl = true;
		});
	}
	
#undef HANDLE_INTERRUPTION
}

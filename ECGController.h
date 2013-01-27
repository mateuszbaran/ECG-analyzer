#pragma once

#include "ECGSignal.h"
#include "ECGRs.h"
#include "ECGWaves.h"
#include "ECGHRV1.h"
#include "ECGHRV2.h"
#include "ECGHRVDFA.h"
#include "QRSClass.h"
#include "ECGHRT.h"
#include "ECGST.h"
#include "ECGTWave.h"
#include "ECGChannelInfo.h"
#include "ModulesInterfaces.h"
#include "ModulesMethods.h"

#include <boost/scoped_ptr.hpp>

namespace boost
{
	class thread;
}

/**
 * @class Class which controls executing all modules.
 */
class ECGController
{
public:
  ECGController (void);
  ~ECGController (void);

  bool readFile(std::string filename);

  //methods for setting parameters
  void setParamsECGBaseline(ParametersTypes & params);
  void setParamsRPeaks(ParametersTypes & params);
  void setParamsnWaves(ParametersTypes & params);
  void setParamsHRV1(ParametersTypes & params);
  void setParamsHRV2(ParametersTypes & params);
  void setParamsHRVDFA(ParametersTypes & params);
  void setParamsQRSClass(ParametersTypes & params);
  void setParamsSTInterval(ParametersTypes & params);
  void setParamsTwaveAlt(ParametersTypes & params);
  void setParamsHRT(ParametersTypes & params);
  
  void rerunAnalysis(std::function<void(std::string)> statusUpdate, std::function<void()> analysisComplete); //if analysis is running, then it's interrupted
  //methods for running modules
  void runECGBaseline();
  void runRPeaks();
  void runWaves();
  void runHRV1();
  void runHRV2();
  void runHRVDFA();
  void runQRSClass();
  void runSTInterval();
  void runTwaveAlt();
  void runHRT();

  //data
  ECGInfo ecg_info;
  ECGSignal raw_signal;
  ECGSignalChannel filtered_signal;
  ECGRs r_peaks_data;
  ECGWaves waves_data;
  ECGHRV1 hrv1_data;
  ECGHRV2 hrv2_data;
  ECGHRVDFA hrv_dfa_data;
  ECGClasses classes_data;
  QRSClass qrsclass_data;
  ECGST st_data;
  ECGTWave twave_data;
  ECGHRT hrt_data;

private:
  //method for setting not run
  void setECGBaselineNotRun();
  void setRPeaksNotRun();
  void setWavesNotRun();
  void setHRV1NotRun();
  void setHRV2NotRun();
  void setHRVDFANotRun();
  void setQRSClassNotRun();
  void setSTIntervalNotRun();
  void setTwaveAltNotRun();
  void setHRTNotRun();

  //modules
  boost::scoped_ptr<ECGBaselineModule> ecg_baseline_module;
  boost::scoped_ptr<RPeaksModule> rpeaks_module;
  boost::scoped_ptr<WavesModule> waves_module;
  boost::scoped_ptr<HRV1Module> hrv1_module;
  boost::scoped_ptr<HRV2Module> hrv2_module;
  boost::scoped_ptr<HRVDFAModule> hrv_dfa_module;
  boost::scoped_ptr<QRSClassModule> qrs_class_module;
  boost::scoped_ptr<STIntervalModule> st_interval_module;
  boost::scoped_ptr<TWaveAltModule> t_wave_alt_module;
  boost::scoped_ptr<HRTModule> hrt_module;

  //computing thread
  boost::thread * computation;
  bool analysisCompl;
};


#include "STAnalysis.h"
#include "wfdb/wfdb.h"
#include "wfdb/ecgcodes.h"
#include "ECGST.h"
#include "ECGSignal.h"
#include <cstdio>
#include <cstdlib>

STAnalysis::STAnalysis() :
  analizator(nullptr)
{
  setAnalizator(AlgorithmType::Simple);
}


STAnalysis::~STAnalysis()
{
  if (analizator) delete analizator;
}

void STAnalysis::runModule(const ECGRs & rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGInfo& ecg_info, ECGST& output)
{ 
  //For tests: ECGRs my_rpeaks = read_normal_r_peaks("ecgSignals", ecg_info.channel_one.filename);
  
  //For tests: int N = my_rpeaks.GetRs()->signal->size;
  int N = rpeaks.GetRs()->signal->size;
  
  for(int i=0; i < N; i++) {
    //For tests: ECGST::Interval interval = analizator->analyse(i, my_rpeaks, waves, signal, ecg_info);
    ECGST::Interval interval = analizator->analyse(i, rpeaks, waves, signal, ecg_info);
    output.addInterval(interval);
  }
  
  output.episodeAnalysis(ecg_info.channel_one.frequecy, thresh);
}


ECGST::Interval STAnalysis::SimpleAnalizator::analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGInfo& info)
{
  ECGST::Interval interval;
  
  double invgain = 1.0 / float(info.channel_one.gain);
  int rpeak = rpeaks.GetRs()->get(it);
  int _45ms_in_samles = static_cast<int>(info.channel_one.frequecy*45.0f/1000.0f);
  int _60ms_in_samles = static_cast<int>(info.channel_one.frequecy*60.0f/1000.0f);
  
  int isopoint = rpeak - _45ms_in_samles;
  interval.jpoint = rpeak + _45ms_in_samles;
  interval.stpoint = interval.jpoint + _60ms_in_samles;
  
  if ( interval.stpoint > signal->signal->size) {
    
  } else {
    interval.offset = (signal->get(isopoint) - signal->get(interval.stpoint))*invgain;
    double diff = signal->get(interval.stpoint) - signal->get(interval.jpoint);
    double invdist = 1/( ( (double) interval.stpoint ) - ( (double) interval.jpoint ) );
    interval.slope = diff*invdist*invgain;
  }
  
  return interval;

}

void STAnalysis::setParams(ParametersTypes& p)
{
  auto algorithm = p.find("algorithm");
  if (algorithm != p.end()) {
    AlgorithmType atype = algorithmTypeFromInt((int) algorithm->second);
    setAnalizator(atype);
  }
  
  thresh = ECGST::Interval::thresh;
  auto thresh_it = p.find("simple_thresh");
  if (thresh_it != p.end()) {
    thresh = thresh_it->second;
  }
}

ECGRs STAnalysis::read_normal_r_peaks(std::string path, std::string filename)
{
  WFDB_anninfo info;
  WFDB_Annotation ann;
  int err;
  ECGRs rs;
  IntSignal rpeaks(new WrappedVectorInt());
  
  size_t dot_pos = filename.rfind('.');
  std::string file = path + "/" + filename.substr(0, dot_pos);
  
  info.name = "atr";
  info.stat = WFDB_READ;
  if( (err = annopen(const_cast<char*>(file.c_str()), &info, 1)) < 0 ) {
    printf("ANNOPEN error %d\n", err);
    return rs;
  }
  int  rpeaks_size = 0;
  while (getann(0, &ann) == 0) {
    if (ann.anntyp == NORMAL) rpeaks_size++;
  }
  
  if( (err = annopen(const_cast<char*>(file.c_str()), &info, 1)) < 0 ) {
    printf("ANNOPEN error %d\n", err);
    return rs;
  }
  
  rpeaks->signal = gsl_vector_int_alloc(rpeaks_size);
  int it = 0;
  while (getann(0, &ann) == 0) {
    if (ann.anntyp == NORMAL) {
      rpeaks->set(it, ann.time);
      it++;
    }
  }
  rs.setRs(rpeaks);
  return rs;
}

void STAnalysis::setAnalizator(STAnalysis::AbstractAnalizator * a)
{
  if (analizator) {
    delete analizator;
    
  }
  analizator = a;
}

void STAnalysis::setAnalizator(STAnalysis::AlgorithmType atype)
{
  switch(atype) {
    case AlgorithmType::Complex:
    case AlgorithmType::Simple:
    default:
      setAnalizator(new SimpleAnalizator()); break;
  }
}

STAnalysis::AlgorithmType STAnalysis::algorithmTypeFromInt(int value) const
{
  switch(value) {
    case 1:
      return AlgorithmType::Complex;
    case 0:
    default:
      return AlgorithmType::Simple;
  }
}


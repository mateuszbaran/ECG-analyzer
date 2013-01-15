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

void STAnalysis::runModule(const ECGRs & rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGInfo& info, ECGST& output)
{ 
#ifdef DEVELOPMENT
  ECGRs my_rpeaks = read_normal_r_peaks("ecgSignals", info.channel_one.filename); //For tests
  int N = my_rpeaks.count(); //For tests
#else
  int N = rpeaks.count(); //For real
#endif
  
  for(int i=0; i < N; i++) {
#ifdef DEVELOPMENT
    analizator->analyse(i, my_rpeaks, waves, signal, info.channel_one, output); // For tests
#else
    analizator->analyse(i, rpeaks, waves, signal, info.channel_one, output); //For real
#endif

  }
}

STAnalysis::SimpleAnalizator::SimpleAnalizator() :
  thresh(0.05), start(0), during_episode(false) {}



void STAnalysis::SimpleAnalizator::analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST& output)
{
  ECGST::Interval interval;
  ECGST::Episode ep;
  
  double invgain = 1.0 / float(info.gain);
  
  int _60s_in_samples = info.frequecy*60;
  int _45ms_in_samples = static_cast<int>(info.frequecy*45.0f/1000.0f);
  int _60ms_in_samples = static_cast<int>(info.frequecy*60.0f/1000.0f);
  
  int rpeak = rpeaks.GetRs()->get(it);
  
  interval.rpoint = rpeak;
#ifdef DEVELOPMENT
  interval.isopoint = rpeak - _45ms_in_samples;
  interval.jpoint = rpeak + _45ms_in_samples;
#else
  interval.isopoint = waves.GetQRS_onset()->get(it);
  interval.jpoint = waves.GetQRS_end()->get(it);
#endif
  interval.stpoint = interval.jpoint + _60ms_in_samples;
  
  if ( interval.stpoint <= signal->signal->size) {
    interval.offset = (signal->get(interval.isopoint) - signal->get(interval.stpoint))*invgain;
    double diff = signal->get(interval.stpoint) - signal->get(interval.jpoint);
    double invdist = 1/( ( (double) interval.stpoint ) - ( (double) interval.jpoint ) );
    interval.slope = diff*invdist*invgain;
    output.addInterval(interval);
  }
  
  if (interval.normal(thresh))
  {
    interval.description = string("normal");
  } 
  else if (interval.higher(thresh))
  {
    interval.description = string("higher");
  }
  else if (interval.lower(thresh))
  { 
    interval.description = string("lower");
  }
  
  if (during_episode && interval.normal(thresh)) {
      if (interval.stpoint - start > _60s_in_samples) {
        ep.start = start;
        ep.end = interval.stpoint;
        output.addEpisode(ep);
      }
      during_episode = false;
    }
    
    if (!during_episode && ! interval.normal()) {
      start = interval.jpoint;
      during_episode = true;
    }

}

void STAnalysis::AbstractAnalizator::setParams(ParametersTypes& p)
{
  params = p;
}

void STAnalysis::SimpleAnalizator::setParams(ParametersTypes& p)
{
   STAnalysis::AbstractAnalizator::setParams(p);
   auto it = params.find("simple_thresh");
   if (it != params.end()) {
    thresh = it->second;
   }
}

STAnalysis::ComplexAnalizator::ComplexAnalizator() :
  thresh(0.05), start(0), during_episode(false) {}
  
void STAnalysis::ComplexAnalizator::analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST& output)
{
  ECGST::Interval interval;
  ECGST::Episode ep;
  
  double invgain = 1.0 / float(info.gain);
  
  int _60s_in_samples = info.frequecy*60;
  int _45ms_in_samples = static_cast<int>(info.frequecy*45.0f/1000.0f);
  int _60ms_in_samples = static_cast<int>(info.frequecy*60.0f/1000.0f);
  
  int rpeak = rpeaks.GetRs()->get(it);
  
#ifdef DEVELOPMENT
  interval.isopoint = rpeak - _45ms_in_samples;
  interval.jpoint = rpeak + _45ms_in_samples;
#else
  interval.isopoint = waves.GetQRS_onset()->get(it);
  interval.jpoint = waves.GetQRS_end()->get(it);
#endif
  interval.stpoint = interval.jpoint + _60ms_in_samples;
  
  if ( interval.stpoint <= signal->signal->size) {
    interval.offset = (signal->get(interval.isopoint) - signal->get(interval.stpoint))*invgain;
    double diff = signal->get(interval.stpoint) - signal->get(interval.jpoint);
    double invdist = 1/( ( (double) interval.stpoint ) - ( (double) interval.jpoint ) );
    interval.slope = diff*invdist*invgain;
    output.addInterval(interval);
  }

  if (interval.normal(thresh))
  {
    interval.description = string("normal");
  } 
  else if (interval.higher(thresh))
  {
    interval.description = string("higher");
  }
  else if (interval.lower(thresh))
  { 
    interval.description = string("lower");
  }
  
  if (during_episode && interval.normal(thresh)) {
    if (interval.stpoint - start > _60s_in_samples) {
      ep.start = start;
      ep.end = interval.stpoint;
      output.addEpisode(ep);
    }
    during_episode = false;
  }
  
  if (!during_episode && ! interval.normal()) {
    start = interval.jpoint;
    during_episode = true;
  }
}

void STAnalysis::ComplexAnalizator::setParams(ParametersTypes& p)
{
  STAnalysis::AbstractAnalizator::setParams(p);
  auto it = params.find("complex_thresh");
  if (it != params.end()) {
    thresh = it->second;
  }
}

void STAnalysis::setParams(ParametersTypes& p)
{
  auto algorithm = p.find("algorithm");
  if (algorithm != p.end()) {
    AlgorithmType atype = algorithmTypeFromInt((int) algorithm->second);
    setAnalizator(atype);
    p.erase(algorithm);
  }
  
  analizator->setParams(p);
}

#ifdef DEVELOPMENT

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

#endif

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
      setAnalizator(new ComplexAnalizator()); break;
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


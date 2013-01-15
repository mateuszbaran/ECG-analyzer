#include "STAnalysis.h"
#include "wfdb/wfdb.h"
#include "wfdb/ecgcodes.h"
#include "ECGST.h"
#include "ECGSignal.h"
#include <cstdio>
#include <cstdlib>
//#include <gsl/gsl_vector.h>
//#include <gsl/gsl_vector_int.h>

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
  printf("ST ANALYSIS END");
  
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
 
    if (interval.normal(thresh))
    {
      interval.description = std::string("normal");
    } 
    else if (interval.higher(thresh))
    {
      interval.description = std::string("higher");
    }
    else if (interval.lower(thresh))
    { 
      interval.description = std::string("lower");
    }
  
    output.addInterval(interval);
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
  thresh(0.1), start(0), during_episode(false), type_thresh(0.15), slope_thresh(0.15) {}
  
std::pair<int, double> STAnalysis::ComplexAnalizator::maxDistanceSample(const OtherSignal& sig, int from, int to)
{
  int indexMax = from;
  double distMax = 0.0;
  
  int size = to - from;
  double first = (sig->get(from)*double(to) - sig->get(to)*double(from))/double(size),
    A = (sig->get(from) - sig->get(to))/double(size),
    ABsqrt = std::sqrt(A*A+1);
  
  for (int i = 0; i < size; i++)
  {
    double d = (A * (from + i)  + sig->get(from + i) + first)/ABsqrt;
    if (d > distMax)
    {
      distMax = d;
      indexMax = from + i;
    }
  }
  
  return std::pair<int, double>(indexMax, distMax);
}

std::pair< int, int > STAnalysis::ComplexAnalizator::overBelowSamples(const OtherSignal& sig, int from, int to)
{
  int over = 0, below = 0;
  
  int size = to - from;
  double first = (sig->get(from)*double(to) - sig->get(to)*double(from))/double(size),
    A = (sig->get(from) - sig->get(to))/double(size);
  
  for (int i = 0; i < size; i++)
  {
    double d = A * (from + i) + first;
    if (sig->get(from+1) > d) over++;
    else below++;
  }
  
  return std::pair<int, int>(over, below);
}


void STAnalysis::ComplexAnalizator::analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignalChannel& signal, const ECGChannelInfo& info, ECGST& output)
{
  ECGST::Interval interval;
  ECGST::Episode ep;
  
  double invgain = 1.0 / float(info.gain);
  
  int _60s_in_samples = info.frequecy*60;
  int _20ms_in_samples = static_cast<int>(info.frequecy*20.0f/1000.0f);
  int _45ms_in_samples = static_cast<int>(info.frequecy*45.0f/1000.0f);
  int _60ms_in_samples = static_cast<int>(info.frequecy*60.0f/1000.0f);
  
  int rpeak = rpeaks.GetRs()->get(it);
  
#ifdef DEVELOPMENT
  interval.isopoint = rpeak - _45ms_in_samples;
  interval.jpoint = rpeak + _45ms_in_samples;
  int tend = rpeak + 2*_60ms_in_samples;
#else
  interval.isopoint = waves.GetQRS_onset()->get(it);
  interval.jpoint = waves.GetQRS_end()->get(it);
  int tend = waves.GetT_end()->get(it);
#endif
  
  if ( tend <= signal->signal->size) {
    int tpeak = getTPeak(signal, interval.jpoint, tend);
    auto max_dist = maxDistanceSample(signal, interval.jpoint + _20ms_in_samples, tpeak);
    
    interval.stpoint = max_dist.first;
  
    interval.offset = (signal->get(interval.isopoint) - signal->get(interval.stpoint))*invgain;
    
    int tepoint = 0;
    bool straight = true;
    
    if (interval.higher(thresh)) {
      interval.description = std::string("higher");
      tepoint = tpeak;
    } else { 
      if (interval.lower(thresh)) interval.description = std::string("lower");
      else interval.description = std::string("normal");
      tepoint = interval.stpoint;
      max_dist = maxDistanceSample(signal, interval.jpoint + _20ms_in_samples, interval.stpoint);
    }
    straight = max_dist.second < type_thresh;
    
    double diff = signal->get(tepoint) - signal->get(interval.jpoint+ _20ms_in_samples);
    double invdist = 1/( ( (double) tepoint ) - ( (double) interval.jpoint+ _20ms_in_samples ) );
    interval.slope = diff*invdist*invgain;
    
    if (straight) {
      interval.description += " straight";
      
      if (interval.slope > slope_thresh) interval.description += " upward";
      else if (interval.slope < -1*slope_thresh) interval.description += " downward";
      else interval.description += " horizon";
    } else {
      interval.description += " curve";
      auto over_below = overBelowSamples(signal, interval.jpoint + _20ms_in_samples, tepoint);
      double span = tepoint - (interval.jpoint + _20ms_in_samples);
      if (double(over_below.first)/span > 0.7) {
        interval.description += " convex";
      } else if (double(over_below.second)/span > 0.7) {
        interval.description += " concave";
      }
    }
    
    output.addInterval(interval);
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

int STAnalysis::ComplexAnalizator::getTPeak(const OtherSignal& sig, int from, int to)
{
  //TODO: Wymyśłić coś mądrzejszego
  return to - 4;
}


void STAnalysis::ComplexAnalizator::setParams(ParametersTypes& p)
{
  STAnalysis::AbstractAnalizator::setParams(p);
  auto it = params.find("complex_thresh");
  if (it != params.end()) {
    thresh = it->second;
  }
  it = params.find("type_thresh");
  if (it != params.end()) {
    type_thresh = it->second;
  }
  it = params.find("slope_thresh");
  if (it != params.end()) {
    slope_thresh = it->second;
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


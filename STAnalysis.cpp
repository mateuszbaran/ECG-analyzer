#include "STAnalysis.h"
#include "wfdb/wfdb.h"
#include "wfdb/ecgcodes.h"
#include "ECGST.h"
#include <cstdio>
#include <cstdlib>

STAnalysis::~STAnalysis()
{
  setAnalizator();
}

void STAnalysis::runModule(const ECGRs & rpeaks, const ECGWaves& waves, const ECGSignal& signal, const ECGInfo& ecg_info, ECGST& output)
{
  printf("RUN ST MODULE\n");
  
  printf("%s\n", ecg_info.channel_one.filename.c_str());
  IntSignal rs = read_normal_r_peaks("ecgSignals/100");
  ECGRs my_rpeaks;
  my_rpeaks.setRs(rs);
  
  int N = my_rpeaks.GetRs()->signal->size;
  //Should be: int N = rpeaks.GetRs()->signal->size;
  
  for(int i=0; i < N; i++) {
    ECGST::Interval interval = analizator->analyse(i, my_rpeaks, waves, signal, ecg_info);
    //Shoud be: ECGST::Interval interval = analizator->analyse(i, rpeaks, waves, signal, ecg_info);
    output.addInterval(interval);
  }
  
  output.episodeAnalysis(ecg_info.channel_one.frequecy);
}


ECGST::Interval STAnalysis::SimpleAnalizator::analyse(const int it, const ECGRs& rpeaks, const ECGWaves& waves, const ECGSignal& signal, const ECGInfo& info)
{
  ECGST::Interval interval;
  
  double invgain = 1.0 / float(info.channel_one.gain);
  int rpeak = gsl_vector_int_get(rpeaks.GetRs()->signal, it);
  int _45ms_in_samles = static_cast<int>(info.channel_one.frequecy*45.0f/1000.0f);
  int _60ms_in_samles = static_cast<int>(info.channel_one.frequecy*60.0f/1000.0f);
  
  int isopoint = rpeak - _45ms_in_samles;
  interval.jpoint = rpeak + _45ms_in_samles;
  interval.stpoint = interval.jpoint + _60ms_in_samles;
  
  if ( interval.stpoint > signal.channel_one->signal->size) {
    
  } else {
    interval.offset = (gsl_vector_get(signal.channel_one->signal, isopoint) - gsl_vector_get(signal.channel_one->signal, interval.stpoint))*invgain;
    double diff = gsl_vector_get(signal.channel_one->signal, interval.stpoint) - gsl_vector_get(signal.channel_one->signal, interval.jpoint);
    double invdist = 1/( ( (double) interval.stpoint ) - ( (double) interval.jpoint ) );
    interval.slope = diff*invdist*invgain;
  }
  
  return interval;

}

void STAnalysis::setParams(ParametersTypes& )
{
  setAnalizator(new SimpleAnalizator());
}

IntSignal STAnalysis::read_normal_r_peaks(char* filename)
{
  WFDB_anninfo info;
  WFDB_Annotation ann;
  int err; 
  IntSignal rpeaks(new WrappedVectorInt());
 
  info.name = "atr";
  info.stat = WFDB_READ;
  if( (err = annopen(filename, &info, 1)) < 0 ) {
    printf("ANNOPEN error %d\n", err);
    return rpeaks;
  }
  int  rpeaks_size = 0;
  while (getann(0, &ann) == 0) {
    if (ann.anntyp == NORMAL) rpeaks_size++;
  }
  
  if( (err = annopen(filename, &info, 1)) < 0 ) {
    printf("ANNOPEN error %d\n", err);
    return rpeaks;
  }
  
  rpeaks->signal = gsl_vector_int_alloc(rpeaks_size);
  int it = 0;
  while (getann(0, &ann) == 0) {
    if (ann.anntyp == NORMAL) {
      gsl_vector_int_set(rpeaks->signal, it, ann.time);
      it++;
    }
  }
  return rpeaks;
}

void STAnalysis::setAnalizator(STAnalysis::AbstractAnalizator * a)
{
  if (analizator) delete analizator;
  analizator = a;
}




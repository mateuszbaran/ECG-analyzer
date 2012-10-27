#pragma once

#include <vector\gsl_vector.h>

class ECGHRV1
{
public:
  ECGHRV1 (void);
  ~ECGHRV1 (void);
private:
  //time domain parameters
  double RR_avg;
  double RR_stddev;
  double RMSSD;
  double NN50;
  double pNN50;
  double SDANN;
  double SDANN_index;
  double SDSD;
  //frequency domain parameters
  double TP;
  double HF;
  double LF;
  double VLF;
  double ULF;
  double LFHF;
  //vectors are needed to plot frequency figure
  gsl_vector * freqency;
  gsl_vector * power;
};


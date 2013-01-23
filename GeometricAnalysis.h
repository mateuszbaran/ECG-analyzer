#pragma once
#include "ModulesInterfaces.h"
#include "ModulesMethods.h"
#include "ECGSignal.h"
#include "ECGRs.h"
#include "ECGHRV2.h"
#include "ECGChannelInfo.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_statistics.h>


//#define DEVELOPMENT
/**
 * @class Class for parameters created in HRV2 module
 */
class GeometricAnalysis : public HRV2Module
{
public:
  GeometricAnalysis (void);
  ~GeometricAnalysis (void);

  void runModule( const ECGInfo &, const ECGRs &, ECGHRV2 &);
  

  void setParams(ParametersTypes &);

  void PrepareRRSignal();
  void MakeHistogramAndGeometricalParams();
  void MakePoincareAndSDParams();
  void SetHRV2Params(ECGHRV2 &hrv2);
  ECGRs MakeRRsignal();

  private:

  //SD parameters [ms]
  double SD1;
  double SD2;

  //TINN parameter [ms]
  double TINN;

  // begin point for base of a triangle approximating the NN (TINN) on histogram (x-axis)
  double M;

  // end point for base of a triangle approximating the NN (TINN) on histogram (x-axis)
  double N;

  //HRV__Triangular_Index parameter
  double HRVTriangularIndex;

  // maximum value on histogram (y-axis)
  double Y; 

  // position of Y on x-axis
  double X;

  // length of histogram bin (1/128 second)
  double HistogramBinLength; // [ms]

  //data for plotting histogram 
  IntSignal histogram_x;
  IntSignal histogram_y;
  
  //data for plotting Poincare [ms]
  IntSignal poincare_x;
  IntSignal poincare_y;

   /**
  *  Filtered signal from 'ECG_BASALINE'
  */
  ECGSignal signal;

  /**
  *  R peaks vector
  */
  ECGRs rpeaks;

   /**
  *  HRV2 output
  */
  //ECGHRV2 hrv2;

  /**
  *  RR intervals vector
  */
  OtherSignal RR_intervals;

  /**
  *  RR SamplingInterval [Hz]
  */
  double SamplingInterval; 

};


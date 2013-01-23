#pragma once

#include "ECGSignal.h"
#include "ECGRs.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_statistics.h>

/**
 * @class Class for parameters created in HRV2 module
 */
class ECGHRV2
{
public:
  ECGHRV2 (void);
  ~ECGHRV2 (void);

  IntSignal GetHistogram_x () ;
  IntSignal GetHistogram_y () ;
  IntSignal GetPoincare_x () ;
  IntSignal GetPoincare_y () ;

  void SetHistogram_x (IntSignal histogram_x) ;
  void SetHistogram_y (IntSignal histogram_y) ;
  void SetPoincare_x (IntSignal poincare_x) ;
  void SetPoincare_y (IntSignal poincare_y) ;
  
  double GetSD1();
  double GetSD2();
  double GetTINN();
  double GetM();
  double GetN();
  double GetHRVTriangularIndex();
  double GetY();
  double GetX();
  double GetHistogramBinLength();

  void SetSD1(double SD1);
  void SetSD2(double SD2);
  void SetTINN(double TINN);
  void SetM(double M);
  void SetN(double N);
  void SetHRVTriangularIndex(double HRVTriangularIndex);
  void SetY(double Y);
  void SetX(double X);
  void SetHistogramBinLength(double HistogramBinLength);

  //SD parameters [ms]
  double SD1;
  double SD2;

  private:

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
  
  //data for plotting Poincare
  IntSignal poincare_x;
  IntSignal poincare_y;

};


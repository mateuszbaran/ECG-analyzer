#pragma once

#include "ECGSignal.h"

/**
 * @class Class for parameters created in HRV2 module
 */
class ECGHRV2
{
public:
  ECGHRV2 (void);
  ~ECGHRV2 (void);

  IntSignal GetHistogram_x () const
  {
    return histogram_x;
  }

  IntSignal GetHistogram_y () const
  {
    return histogram_y;
  }

  IntSignal GetPoincare_x () const
  {
    return poincare_x;
  }

  IntSignal GetPoincare_y () const
  {
    return poincare_y;
  }

  double GetTriple_index_x () const
  {
    return triple_index_x;
  }

  double GetTriple_index_y () const
  {
    return triple_index_y;
  }

  //SD parameters
  double SD1;
  double SD2;
  //TINN
  double TINN;

private:
  //data for plotting histogram
  IntSignal histogram_x;
  IntSignal histogram_y;
  //data for plotting triple index (?)
  double triple_index_x;
  double triple_index_y;
  //data for plotting Poincare
  IntSignal poincare_x;
  IntSignal poincare_y;

};


#pragma once

#include <vector\gsl_vector_int.h>
#include <vector\gsl_vector.h>

/**
 * @class Class for parameters created in HRV2 module
 */
class ECGHRV2
{
public:
  ECGHRV2 (void);
  ~ECGHRV2 (void);
  
  double GetSD1 () const
  {
    return SD1;
  }

  double GetSD2 () const
  {
    return SD2;
  }

  double GetTINN () const
  {
    return TINN;
  }

  gsl_vector_int* GetHistogram_x () const
  {
    return histogram_x;
  }

  gsl_vector_int* GetHistogram_y () const
  {
    return histogram_y;
  }

  gsl_vector_int* GetPoincare_x () const
  {
    return poincare_x;
  }

  gsl_vector_int* GetPoincare_y () const
  {
    return poincare_y;
  }

  gsl_vector* GetTriple_index_x () const
  {
    return triple_index_x;
  }

  gsl_vector* GetTriple_index_y () const
  {
    return triple_index_y;
  }

  void SetSD1 (double SD1)
  {
    this->SD1 = SD1;
  }

  void SetSD2 (double SD2)
  {
    this->SD2 = SD2;
  }

  void SetTINN (double TINN)
  {
    this->TINN = TINN;
  }
  
private:
  //data for plotting histogram
  gsl_vector_int * histogram_x;
  gsl_vector_int * histogram_y;
  //TINN
  double TINN;
  //data for plotting triple index (?)
  gsl_vector * triple_index_x;
  gsl_vector * triple_index_y;
  //data for plotting Poincare
  gsl_vector_int * poincare_x;
  gsl_vector_int * poincare_y;
  //SD paramters
  double SD1;
  double SD2;
};


#pragma once

#include "ECGSignal.h"
#include "ModulesMethods.h"

/**
 * @class R peaks.
 */
class ECGRs
{
public:

  ECGRs ();
  ~ECGRs ();

  ECGRsSignal GetRs () const
  {
    return rs;
  }

  void setRChannelOne(int index, int value)
  {
	  gsl_vector_set(rs.rsChannelOne, index, value);
  }

   void setRChannelTwo(int index, int value)
  {
	  gsl_vector_set(rs.rsChannelTwo, index, value);
  }

private:

  ECGRsSignal rs;

};

struct ECGRsSignal
{
public:
  void setSize(size_t n);
  IntSignal rsChannelOne;
  IntSignal rsChannelTwo;
};

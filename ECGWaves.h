#pragma once

#include <gsl/gsl_sort_vector.h>
#include "ECGSignal.h"
#include "ECGRs.h"

using namespace std;

/**
 * @class Class for storing QRS parameters.
 */
class ECGWaves
{
public:
  ECGWaves (void);
  ~ECGWaves (void);

  bool detectQRS(ECGSignal &signal,ECGRs &rPeak);
  bool detectPT(ECGSignal &signal);
  ECGSignal gradient(ECGSignal *signal);
  ECGSignal averageFilter(ECGSignal *signal);
  friend double* findMinimum (ECGSignal *signal,int forBegin, int forEnd);
  friend double* findMaximum (ECGSignal *signal,int forBegin, int forEnd);

  ECGSignal getMockedSignal();
  ECGRs getMockedRPeak();

  IntSignal GetP_end () const
  {
    return P_end;
  }

  IntSignal GetP_onset () const
  {
    return P_onset;
  }

  IntSignal GetQRS_end () const
  {
    return QRS_end;
  }

  IntSignal GetQRS_onset () const
  {
    return QRS_onset;
  }

  IntSignal GetT_end () const
  {
    return T_end;
  }

private:
  IntSignal QRS_onset;
  IntSignal QRS_end;
  IntSignal T_end;
  IntSignal P_onset;
  IntSignal P_end;
};


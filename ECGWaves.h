#pragma once

#include <gsl/gsl_sort_vector.h>
#include "ECGSignal.h"
#include "ECGRs.h"

#define DEBUG

using namespace std;

/**
 * @class Class for storing QRS parameters.
 */
class ECGWaves
{
public:
  ECGWaves (void);
  ~ECGWaves (void);

  void setSize(size_t n);
  void setQrsOnset(IntSignal rs);
  void setQrsEnd(IntSignal rs);
  void setTend(IntSignal rs);
  void setPonset(IntSignal rs);
  void setPend(IntSignal rs);


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


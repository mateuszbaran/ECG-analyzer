#pragma once

#include "ECGSignal.h"
#include <vector>

/**
 * @class Class for storing DFA results.
 */
class ECGHRVDFA
{
public:
  ECGHRVDFA (void);
  ~ECGHRVDFA (void);

  OtherSignal GetDfa_x () const
  {
    return dfa_x;
  }

  OtherSignal GetDfa_y () const
  {
    return dfa_y;
  }

  OtherSignal GetRR_integrated () const
  {
    return rr_integrated;
  }

  double getDFA_scaling_factor() const
  {
    return DFA_scaling_factor;
  }

  std::vector<std::vector<std::string >> &getTable ()
  {
    return table;
  }
	
  void setDFA_scaling_factor(double DFA_scaling_factor);

  void setDFA_x(OtherSignal dfa_x);

  void setDFA_y(OtherSignal dfa_y);

  void setRR_integrated(OtherSignal rr_integrated);

private:
  //for plotting
  OtherSignal dfa_x;
  OtherSignal dfa_y;
  OtherSignal rr_integrated;
  //table
  double DFA_scaling_factor;
  std::vector<std::vector<std::string >> table;
};


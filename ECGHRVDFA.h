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

  std::vector<std::vector<std::string >> &getTable ()
  {
    return table;
  }
	
  void setDFA_scaling_factor(double DFA_scaling_factor);

private:
  //for plotting
  OtherSignal dfa_x;
  OtherSignal dfa_y;
  //table
  double DFA_scaling_factor;
  std::vector<std::vector<std::string >> table;
};


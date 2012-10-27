#pragma once

#include <vector\gsl_vector.h>
#include <vector>

/**
 * @class Class for storing DFA results.
 */
class ECGHRVDFA
{
public:
  ECGHRVDFA (void);
  ~ECGHRVDFA (void);

  gsl_vector* GetDfa_x () const
  {
    return dfa_x;
  }

  gsl_vector* GetDfa_y () const
  {
    return dfa_y;
  }

  std::vector<std::vector<std::string >> &getTable ()
  {
    return table;
  }

private:
  //for plotting
  gsl_vector * dfa_x;
  gsl_vector * dfa_y;
  //table
  std::vector<std::vector<std::string >> table;
};


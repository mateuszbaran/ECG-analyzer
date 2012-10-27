#include "ECGHRVDFA.h"

ECGHRVDFA::ECGHRVDFA (void)
{
  dfa_y = dfa_x = 0;
}

ECGHRVDFA::~ECGHRVDFA (void)
{
  if (dfa_y) gsl_vector_free (dfa_y);
  if (dfa_x) gsl_vector_free (dfa_x);
}

#include "ECGRs.h"

ECGRs::ECGRs (void)
{
  rs = 0;
}

ECGRs::~ECGRs (void)
{
  if (rs) delete rs;
}

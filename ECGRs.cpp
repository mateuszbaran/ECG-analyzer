#include "ECGRs.h"

ECGRs::ECGRs ()
{}

ECGRs::~ECGRs (void)
{}

void ECGRs::setRs(IntSignal rs)
{
	this->rs = rs;
}

size_t ECGRs::count() const
{
  return rs->signal->size;
}

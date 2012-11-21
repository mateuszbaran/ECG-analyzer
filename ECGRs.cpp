#include "ECGRs.h"

ECGRs::ECGRs ()
{}

ECGRs::~ECGRs (void)
{}

void ECGRs::setRsChannelOne(IntSignal rs)
{
	rsco = rs;
}

void ECGRs::setRsChannelTwo(IntSignal rs)
{
	rsct = rs;
}
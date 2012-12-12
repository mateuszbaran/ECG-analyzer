#include "ECGRs.h"

ECGRs::ECGRs ()
{}

ECGRs::~ECGRs (void)
{}

void ECGRs::setRsChannelOne(gsl_vector_int* rs)
{
	rsco = rs;
}

void ECGRs::setRsChannelTwo(gsl_vector_int* rs)
{
	rsct = rs;
}
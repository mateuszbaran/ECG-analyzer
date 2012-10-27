#include "ECGSignal.h"


ECGSignal::ECGSignal(void)
{
	signal =0;
}


ECGSignal::~ECGSignal(void)
{
	if (signal) delete signal;
}

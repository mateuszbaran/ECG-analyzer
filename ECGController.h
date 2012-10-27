#pragma once

#include "ECGSignal.h"

/**
 * @class 
 */
class ECGController
{
public:
  ECGController (void);
  ~ECGController (void);
private:
  ECGSignal raw_signal;
  ECGSignal filtered_signal;

};


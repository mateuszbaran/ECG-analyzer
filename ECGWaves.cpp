#include "ECGWaves.h"

ECGWaves::ECGWaves (void)
{}

ECGWaves::~ECGWaves (void)
{}

void ECGWaves::setQrsOnset(IntSignal QRS_onset)
{
	this->QRS_onset = QRS_onset;
}

void ECGWaves::setQrsEnd(IntSignal QRS_end)
{
	this->QRS_end = QRS_end;
}

void ECGWaves::setTend(IntSignal T_end)
{
	this->T_end = T_end;
}

void ECGWaves::setPonset(IntSignal P_onset)
{
	this->P_onset = P_onset;
}

void ECGWaves::setPend(IntSignal P_end)
{
	this->P_end = P_end;
}

void ECGWaves::setSize(size_t n)
{
  this->QRS_onset = IntSignal(new WrappedVectorInt);
  this->QRS_onset->signal = gsl_vector_int_alloc(n);
  
  this->QRS_end = IntSignal(new WrappedVectorInt);
  this->QRS_end->signal = gsl_vector_int_alloc(n);
  
  this->T_end = IntSignal(new WrappedVectorInt);
  this->T_end->signal = gsl_vector_int_alloc(n-1);
  
  this->P_onset = IntSignal(new WrappedVectorInt);
  this->P_onset->signal = gsl_vector_int_alloc(n-1);
  
  this->P_end = IntSignal(new WrappedVectorInt);
  this->P_end->signal = gsl_vector_int_alloc(n-1);

}
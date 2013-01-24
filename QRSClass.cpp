#include "QRSClass.h"

QRSClass::QRSClass (void)
{}

QRSClass::~QRSClass (void)
{}

void QRSClass::setQrsMorphology(IntSignal QRS_morphology)
{
	this->QRS_morphology = QRS_morphology;
}

void QRSClass::setSize(size_t n)
{
	this->QRS_morphology = IntSignal(new WrappedVectorInt);
	this->QRS_morphology->signal = gsl_vector_int_alloc(n);
}
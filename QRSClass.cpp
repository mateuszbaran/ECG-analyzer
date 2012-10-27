#include "QRSClass.h"

QRSClass::QRSClass (void)
{
  example = 0;
}

QRSClass::~QRSClass (void)
{
  if (example) gsl_vector_free (example);
}

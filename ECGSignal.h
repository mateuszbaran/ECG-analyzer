#pragma once

#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_int.h>
#include <boost/shared_ptr.hpp>

/**
 * @class Wrapped gsl_vector
 */
class WrappedVector
{
public:
  WrappedVector (gsl_vector * _signal = NULL);
  ~WrappedVector ();

  gsl_vector* signal;
};

/**
 * @class Wrapped gsl_vector_int
 */
class WrappedVectorInt
{
public:
  WrappedVectorInt (gsl_vector_int * _signal = NULL);
  ~WrappedVectorInt ();

  gsl_vector_int* signal;
};

typedef boost::shared_ptr<WrappedVector> ECGSignal;
typedef boost::shared_ptr<WrappedVector> OtherSignal;
typedef boost::shared_ptr<WrappedVectorInt> IntSignal;


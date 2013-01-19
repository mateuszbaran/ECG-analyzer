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
  
  double get(size_t it);
  void set(size_t it, double value);
  
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
  
  int get(size_t it);
  void set(size_t it, int value);

  gsl_vector_int* signal;
};

typedef boost::shared_ptr<WrappedVector> ECGSignalChannel;
typedef boost::shared_ptr<WrappedVector> OtherSignal;
typedef boost::shared_ptr<WrappedVectorInt> IntSignal;

struct ECGSignal
{
public:
  void setSize(size_t n);
  ECGSignalChannel channel_one;
  ECGSignalChannel channel_two;
};



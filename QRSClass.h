#pragma once

#include <gsl/gsl_sort_vector.h>
#include "ECGSignal.h"
#include "ECGWaves.h"

using namespace std;

/**
 * QRS Types.
 */
/*
enum QRSType
{
  VENTRICULUS,
  SUPRACENTRICULAR
};*/

/**
 * @class Represents single QRS Class.
 */
class QRSClass
{
public:
  QRSClass (void);
  ~QRSClass (void);

  bool QRSClass::classQRS(ECGSignal &signal,ECGWaves &waves);
  double * QRSClass::pole(ECGSignal * signal);
  double * QRSClass::dlugosc(ECGSignal * signal);

  IntSignal GetQRS_class() const
  {
	  return QRS_class;
  }


  void SetId (int id)
  {
    this->id = id;
  }

  void SetType (QRSType type)
  {
    this->type = type;
  }

  OtherSignal GetExample () const
  {
    return example;
  }

  int GetId () const
  {
    return id;
  }

  QRSType GetType () const
  {
    return type;
  }


private:
  int id;
  IntSignal QRS_class;
  QRSType type;
  OtherSignal example;
};


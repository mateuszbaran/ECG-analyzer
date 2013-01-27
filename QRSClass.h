#pragma once

#include <gsl/gsl_sort_vector.h>
#include "ECGSignal.h"
#include "ECGWaves.h"

using namespace std;

/**
 * QRS Types.
 */

enum QRSType
{
  VENTRICULUS,
  SUPRACENTRICULAR
};

/**
 * @class Represents single QRS Class.
 */
class QRSClass
{
public:
  QRSClass (void);
  ~QRSClass (void);

  void setQrsMorphology(IntSignal QRS_morphology);
  void setSize(size_t n);

  IntSignal GetQRS_morphology() const
  {
	  return QRS_morphology;
  }


  void SetId (int id)
  {
    this->id = id;
  }

  int GetId () const
  {
    return id;
  }

private:
  int id;
  IntSignal QRS_morphology;
};

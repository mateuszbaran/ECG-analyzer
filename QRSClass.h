#pragma once

#include <ECGSignal.h>

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
  QRSType type;
  OtherSignal example;
};


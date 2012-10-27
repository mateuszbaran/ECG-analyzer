#pragma once

#include "QRSClass.h"

#include <list>

/**
 * @class Container for QRS Classes. Also result of QRSCLASS module.
 */
class ECGClasses
{
public:
  ECGClasses (void);
  ~ECGClasses (void);

  std::list<QRSClass*> & getClasses ()
  {
    return classes;
  }
private:
  std::list<QRSClass*> classes;
};


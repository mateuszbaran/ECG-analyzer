#pragma once

#include "QRSClass.h"

#include <vector>

/**
 * @class Container for QRS Classes. Also result of QRSCLASS module.
 */
class ECGClasses
{
public:
  ECGClasses (void);
  ~ECGClasses (void);

  std::vector<QRSClass*> & getClasses ()
  {
    return classes;
  }
private:
  std::vector<QRSClass*> classes;
};


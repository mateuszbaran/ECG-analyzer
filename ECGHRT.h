#pragma once

#include<QPointF>
#include<QVector>
#include<QLine>

class ECGHRT
{
public:
  ECGHRT (void);
  ~ECGHRT (void);
  int offset;
  QVector<QPointF> rr;
  QLine ts;
private:
  //TODO: znalezienie co tu ma by�
};


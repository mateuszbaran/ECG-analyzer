#ifndef STPLOT_H
#define STPLOT_H

#include "ecgch.h"
#include <ECGST.h>
#include <boost/concept_check.hpp>


class StPlot : public QwtPlot
{
  Q_OBJECT
public:
  StPlot(QWidget* parent = 0);
  virtual ~StPlot();
  void zoomX(int from, int to, bool vscale = true);
  void setSignal(const ECGSignalChannel&, const ECGChannelInfo&, const ECGST&);
private:
  double invgain, dt;
  QwtPlotCurve* curve;
  QwtPointSeriesData* data;
  QVector<QPointF>* samples;
  QwtPlotZoomer * zoomer;
  std::pair< double, double > minMaxValueIn(int from, int to);
};

#endif // ECGCHEXTENDED_H

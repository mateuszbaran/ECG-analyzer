#include "stplot.h"
#include "timescaledraw.h"
#include "gainsscaledraw.h"

#ifdef WIN32
#include <qwt6/qwt_symbol.h>
#include <qwt6/qwt_legend.h>
#else
#include <qwt_symbol.h>
#include <qwt_legend.h>
#endif
#include <QResizeEvent>

StPlot::StPlot(QWidget* parent): 
  QwtPlot(parent),
  viewFactor(-1.0)
{
  setMinimumHeight(10);
  setMinimumWidth(10);
  QwtPlotGrid *grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->attach(this);
  setAxisTitle(QwtPlot::xBottom, "Czas [mm:ss.ms]");
  setAxisTitle(QwtPlot::yLeft, "Amplituda [mv]");
  setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(360));
  curve = new QwtPlotCurve("Filtered signal");
  curve->setYAxis(QwtPlot::yLeft);
  curve->attach(this);

  ISOPoints = new QwtPlotCurve("ISO");
  ISOPoints->setStyle(QwtPlotCurve::NoCurve);
  ISOPoints->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::green), QColor(Qt::green), QSize(5, 5)));
  ISOPoints->setYAxis(QwtPlot::yLeft);
  ISOPoints->attach(this);
  
  JPoints = new QwtPlotCurve("J");
  JPoints->setStyle(QwtPlotCurve::NoCurve);
  JPoints->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::blue), QColor(Qt::blue), QSize(5, 5)));
  JPoints->setYAxis(QwtPlot::yLeft);
  JPoints->attach(this);
  
  STPoints = new QwtPlotCurve("ST");
  STPoints->setStyle(QwtPlotCurve::NoCurve);
  STPoints->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::red), QColor(Qt::red), QSize(5, 5)));
  STPoints->setYAxis(QwtPlot::yLeft);
  STPoints->attach(this);

  RPoints = new QwtPlotCurve("R");
  RPoints->setStyle(QwtPlotCurve::NoCurve);
  RPoints->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::yellow), QColor(Qt::yellow), QSize(5, 5)));
  RPoints->setYAxis(QwtPlot::yLeft);
  //RPoints->attach(this);

  legend = new QwtLegend();
  legend->setItemMode(QwtLegend::ReadOnlyItem);
  legend->setWhatsThis("Click on an item to show/hide the plot");
  this->insertLegend(legend, QwtPlot::RightLegend);
  
  samples = new QVector<QPointF>;
  data = new QwtPointSeriesData;
  replot();
  
  zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas());
  zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect4, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect5, Qt::NoButton);
  zoomer->setMousePattern(QwtEventPattern::MouseSelect6, Qt::NoButton);
}

StPlot::~StPlot()
{
  if(zoomer) delete zoomer;
  delete ISOPoints;
  delete JPoints;
  delete STPoints;
  //delete RPoints;
}

void StPlot::setSignal(const ECGSignalChannel& signal, const ECGChannelInfo& info, const ECGST& stdata)
{
  gsl_vector *v = signal->signal;
  int size = int(v->size);
  samples->clear();
  
  for (int i = 0; i < size; i++)
    samples->push_back(QPointF(float(i), float(v->data[i*v->stride])));
  
  QVector<QPointF>* ISOVector = new QVector<QPointF>;
  QVector<QPointF>* JVector = new QVector<QPointF>;
  QVector<QPointF>* STVector = new QVector<QPointF>;
  //QVector<QPointF>* RVector = new QVector<QPointF>;
  const std::vector<ECGST::Interval> intervals = stdata.getIntervals();
  for (std::vector<ECGST::Interval>::const_iterator it = intervals.begin() ; it != intervals.end(); ++it)
  {
    ISOVector->push_back(QPointF(float(it->isopoint), float(v->data[(it->isopoint)*v->stride])));
    JVector->push_back(QPointF(float(it->jpoint), float(v->data[(it->jpoint)*v->stride])));
    STVector->push_back(QPointF(float(it->stpoint), float(v->data[(it->stpoint)*v->stride])));
    //RVector->push_back(QPointF(float(it->rpoint)*dt, float(v->data[(it->rpoint)*v->stride]*invgain)));
  }
  ISOPoints->setSamples(*ISOVector);
  JPoints->setSamples(*JVector);
  STPoints->setSamples(*STVector);
  //RPoints->setSamples(*RVector);
  setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(info.frequecy));
  qDebug() << info.gain;
  setAxisScaleDraw(QwtPlot::yLeft, new GainScaleDraw(info.gain));
  data->setSamples(*samples);
  curve->setData(data);
  zoomer->setZoomBase();
  replot();
}

void StPlot::showEpisode(const ECGST::Episode& e)
{
  ISOPoints->hide();
  JPoints->hide();
  STPoints->hide();
  legend->hide();
  zoomX(e.start - 10, e.end + 10, true);
  
  
}

void StPlot::showInterval(const ECGST::Interval& it)
{
  ISOPoints->show();
  JPoints->show();
  STPoints->show();
  legend->show();
  auto span = it.span();
  zoomX(span.first, span.second, true);
}



void StPlot::zoomX(int from, int to, bool vscale)
{
  QRectF rect = zoomer->zoomBase();
  
  rect.setLeft(from);
  rect.setRight(to);
  viewFactor = rect.width()/canvas()->width();
  
  if (vscale) {
    auto _minmax = minMaxValueIn(from, to);
    rect.setTop(_minmax.second);
    rect.setBottom(_minmax.first);
  }
  
  zoomer->zoom(rect);
  replot();
}

std::pair<double, double> StPlot::minMaxValueIn(int from, int to)
{
  from = from < 0 ? 0 : from;
  to = to > (data->size() - 1) ? data->size() : to;
  QVector<QPointF> roi(to - from);
  std::copy(samples->begin()+from, samples->begin()+to, roi.begin());
  auto _minmax = std::minmax_element(roi.begin(), roi.end(), [](const QPointF& a, const QPointF& b) {
    return a.y() < b.y();
  });
  return std::make_pair(_minmax.first->y(), _minmax.second->y());
}

void StPlot::resizeEvent(QResizeEvent* e)
{
  QwtPlot::resizeEvent(e);
  if (e->oldSize().width() < 0) return;
  if (viewFactor > 0) {
    double diff = (e->size().width() - e->oldSize().width())*viewFactor;
    auto zoom_rect = zoomer->zoomRect();
    double nleft = zoom_rect.left() - diff/2.0f;
    double nright = zoom_rect.right() + diff/2.0f;
    zoom_rect.setLeft(nleft);
    zoom_rect.setRight(nright);
    zoomer->zoom(zoom_rect);
  }
  replot();
}

#include "synczoomer.h"

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas):
        QwtPlotZoomer(xAxis, yAxis, canvas)
    {
        setTrackerMode(QwtPicker::AlwaysOff);
        setRubberBand(QwtPicker::NoRubberBand);
        setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
        setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
        setRubberBand(QwtPicker::RectRubberBand);
        setRubberBandPen(QColor(Qt::green));
        setTrackerMode(QwtPicker::ActiveOnly);
        setTrackerPen(QColor(Qt::white));
    }
};

SyncZoomer::SyncZoomer(QwtPlotCanvas *canvas1, QwtPlotCanvas *canvas2)
{
    zoomer1 = new Zoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas1);
    zoomer2 = new Zoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas2);
}

void SyncZoomer::setZoomBase()
{
    zoomer1->setZoomBase();
    zoomer2->setZoomBase();
}

void SyncZoomer::enableSync(bool enable)
{
    if (enable)
    {
        connect(zoomer1, SIGNAL(zoomed(QRectF)), zoomer2, SLOT(zoom(QRectF)));
        connect(zoomer2, SIGNAL(zoomed(QRectF)), zoomer1, SLOT(zoom(QRectF)));
    }
    else
    {
        disconnect(zoomer1, 0, zoomer2, 0);
        disconnect(zoomer2, 0, zoomer1, 0);
    }
}

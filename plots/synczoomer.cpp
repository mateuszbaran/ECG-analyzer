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

PlotControl::PlotControl(QwtPlot *_plot1, QwtPlot *_plot2)
{
    plot1 = _plot1;
    plot2 = _plot2;
    QwtPlotCanvas *canvas1 = plot1->canvas();
    QwtPlotCanvas *canvas2 = plot2->canvas();
    zoomer1 = new Zoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas1);
    zoomer2 = new Zoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas2);
    panner1 = new QwtPlotPanner(canvas1);
    panner2 = new QwtPlotPanner(canvas2);
    panner1->setMouseButton(Qt::MidButton);
    panner2->setMouseButton(Qt::MidButton);
}

void PlotControl::setZoomBase()
{
    zoomer1->setZoomBase();
    zoomer2->setZoomBase();
}

void PlotControl::enableSync(bool enable)
{
    if (enable)
    {
//        plot1->enableAxis(QwtPlot::xBottom, false); // TODO! wymiarowanie się psuje
        connect(zoomer1, SIGNAL(zoomed(QRectF)), zoomer2, SLOT(zoom(QRectF)));
        connect(zoomer2, SIGNAL(zoomed(QRectF)), zoomer1, SLOT(zoom(QRectF)));
        connect(panner1, SIGNAL(panned(int,int)), panner2, SLOT(moveCanvas(int,int)));
        connect(panner2, SIGNAL(panned(int,int)), panner1, SLOT(moveCanvas(int,int)));
    }
    else
    {
        plot1->enableAxis(QwtPlot::xBottom, true);
        disconnect(zoomer1, 0, zoomer2, 0);
        disconnect(zoomer2, 0, zoomer1, 0);
        disconnect(panner1, 0, panner2, 0);
        disconnect(panner2, 0, panner1, 0);
    }
}

#include "ecgch.h"

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas):
        QwtPlotZoomer(xAxis, yAxis, canvas)
    {
        setTrackerMode(QwtPicker::AlwaysOff);
        setRubberBand(QwtPicker::NoRubberBand);

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

        setMousePattern(QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier);
        setMousePattern(QwtEventPattern::MouseSelect3,
            Qt::RightButton);
    }
};

EcgCh::EcgCh(QWidget *parent) :
    QwtPlot(parent)
{
     setMinimumHeight(50);
     setMinimumWidth(50);
     QwtPlotGrid *grid = new QwtPlotGrid;
     grid->enableXMin(true);
     grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
     grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
     grid->attach(this);
     zoomer[0] = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft, canvas());
     zoomer[0]->setRubberBand(QwtPicker::RectRubberBand);
     zoomer[0]->setRubberBandPen(QColor(Qt::green));
     zoomer[0]->setTrackerMode(QwtPicker::ActiveOnly);
     zoomer[0]->setTrackerPen(QColor(Qt::white));
     zoomer[1] = new Zoomer(QwtPlot::xTop, QwtPlot::yRight, canvas());
     panner = new QwtPlotPanner(canvas());
     panner->setMouseButton(Qt::MidButton);
     picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas());
     picker->setStateMachine(new QwtPickerDragPointMachine());
     picker->setRubberBandPen(QColor(Qt::green));
     picker->setRubberBand(QwtPicker::CrossRubberBand);
     picker->setTrackerPen(QColor(Qt::white));
     curve = new QwtPlotCurve("signal");
     curve->attach(this);
     samples = new QVector<QPointF>;
     data = new QwtPointSeriesData;
     replot();
}

void EcgCh::setSignal(ECGSignalChannel signal)
{
    gsl_vector *v = signal->signal;
    int size = int(v->size);
    samples->clear();
    for (int i = 0; i < size; i++)
        samples->push_back(QPointF(float(i), float(v->data[i*v->stride])));
    data->setSamples(*samples);
    curve->setData(data);
}

void EcgCh::redraw()
{
    replot();
}

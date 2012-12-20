#include "ecgch.h"

EcgCh::EcgCh(QWidget *parent) :
    QwtPlot(parent)
{
     setMinimumHeight(10);
     setMinimumWidth(10);
     QwtPlotGrid *grid = new QwtPlotGrid;
     grid->enableXMin(true);
     grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
     grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
     grid->attach(this);
     setAxisTitle(QwtPlot::xBottom, "Czas [s]");
     setAxisTitle(QwtPlot::yLeft, "Amplituda [mv]");
//     picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas());
//     picker->setStateMachine(new QwtPickerDragPointMachine());
//     picker->setRubberBandPen(QColor(Qt::green));
//     picker->setRubberBand(QwtPicker::CrossRubberBand);
//     picker->setTrackerPen(QColor(Qt::white));
     curve = new QwtPlotCurve("signal");
     curve->setYAxis(QwtPlot::yLeft);
     curve->attach(this);
     samples = new QVector<QPointF>;
     data = new QwtPointSeriesData;
     replot();
}

void EcgCh::setSignal(ECGSignalChannel signal, ECGChannelInfo info)
{
    gsl_vector *v = signal->signal;
    float invgain = 1.0 / float(info.gain);
    float dt = 1.0 / float(info.frequecy);
    int size = int(v->size);
    samples->clear();
    for (int i = 0; i < size; i++)
        samples->push_back(QPointF(float(i)*dt, float(v->data[i*v->stride])*invgain));
    data->setSamples(*samples);
    curve->setData(data);
}

void EcgCh::redraw()
{
    replot();
}

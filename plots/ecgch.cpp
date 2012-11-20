#include "ecgch.h"

EcgCh::EcgCh(QWidget *parent) :
    QwtPlot(parent)
{
     setMinimumHeight(50);
     setMinimumWidth(50);
     //setAxisTitle(xBottom, "sample");
     //setAxisTitle(yLeft, "amplitude");
     samples = new QVector<QPointF>;
     data = new QwtPointSeriesData;
     curve = new QwtPlotCurve("signal");
     curve->attach(this);
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

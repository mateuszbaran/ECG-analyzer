#include "ecgch.h"
#include "timescaledraw.h"
#include "gainsscaledraw.h"

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
     setAxisTitle(QwtPlot::xBottom, "Czas [hh:mm:ss.ms]");
     setAxisTitle(QwtPlot::yLeft, "Amplituda [mV]");
//     picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas());
//     picker->setStateMachine(new QwtPickerDragPointMachine());
//     picker->setRubberBandPen(QColor(Qt::green));
//     picker->setRubberBand(QwtPicker::CrossRubberBand);
//     picker->setTrackerPen(QColor(Qt::white));
     curve = new QwtPlotCurve("signal");
     curve->setYAxis(QwtPlot::yLeft);
     curve->attach(this);
     peaksCurve = new QwtPlotCurve("signal");
     peaksCurve->setYAxis(QwtPlot::yLeft);
     peaksCurve->setStyle(QwtPlotCurve::CurveStyle::Dots);
     peaksCurve->setPen(QPen(Qt::red, 5));
     peaksCurve->attach(this);
     samples = new QVector<QPointF>;
     data = new QwtPointSeriesData;
     peaksSamples = new QVector<QPointF>;
     peaksData = new QwtPointSeriesData;
     replot();
}

void EcgCh::setSignal(ECGSignalChannel signal, ECGChannelInfo info)
{
    gsl_vector *v = signal->signal;

    int size = int(v->size);
    samples->clear();
    for (int i = 0; i < size; i++)
    {
        if ((float)(v->data[i*v->stride]) != 0.0)
            samples->push_back(QPointF(float(i), float(v->data[i*v->stride])));
    }

    data->setSamples(*samples);
    curve->setData(data);
    setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(info.frequecy));
    setAxisScaleDraw(QwtPlot::yLeft, new GainScaleDraw(info.gain));
	replot();
}

void EcgCh::setSignal(ECGSignalChannel signal, ECGChannelInfo info, IntSignal peaks)
{
    this->setSignal(signal, info);
//    gsl_vector_int *v = peaks->signal;

//    int size = int(v->size);
//    peaksSamples->clear();
//    for (int i = 0; i < size; i++)
//        peaksSamples->push_back(data->sample(i));
//    peaksData->setSamples(*peaksSamples);
//    peaksCurve->setData(data);
//    replot();
}

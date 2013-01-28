#include "ecgch.h"
#include "timescaledraw.h"
#include "gainsscaledraw.h"
#include <QDebug>

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
     setAxisTitle(QwtPlot::xBottom, "Czas [mm:ss.ms]");
     setAxisTitle(QwtPlot::yLeft, "Amplituda [mV]");

     signalCurve = new QwtPlotCurve("signal");
     signalCurve->setYAxis(QwtPlot::yLeft);
     signalCurve->attach(this);
     peaksCurve = new QwtPlotCurve("r-peaks");
     peaksCurve->setYAxis(QwtPlot::yLeft);
     peaksCurve->setStyle(QwtPlotCurve::NoCurve);
     peaksCurve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(Qt::red), QColor(Qt::red), QSize(6, 6)));
     peaksCurve->setStyle(QwtPlotCurve::CurveStyle::Dots);
     peaksCurve->setPen(QPen(Qt::red, 5));
     peaksCurve->attach(this);

     qrsOnSetCurve = new QwtPlotCurve("qrsOnSet");
     qrsOnSetCurve->setYAxis(QwtPlot::yLeft);
     qrsOnSetCurve->setStyle(QwtPlotCurve::NoCurve);
     qrsOnSetCurve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(0,255,0), QColor(0,255,0), QSize(6, 6)));
     qrsOnSetCurve->attach(this);

     qrsEndSetCurve = new QwtPlotCurve("qrsEndSet");
     qrsEndSetCurve->setYAxis(QwtPlot::yLeft);
     qrsEndSetCurve->setStyle(QwtPlotCurve::NoCurve);
     qrsEndSetCurve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(0,150,0), QColor(0,150,0), QSize(6, 6)));
     qrsEndSetCurve->attach(this);

     pOnSetCurve = new QwtPlotCurve("pOnSet");
     pOnSetCurve->setYAxis(QwtPlot::yLeft);
     pOnSetCurve->setStyle(QwtPlotCurve::NoCurve);
     pOnSetCurve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(0,0,255), QColor(0,0,255), QSize(6, 6)));
     pOnSetCurve->attach(this);

     pEndSetCurve = new QwtPlotCurve("pEndSet");
     pEndSetCurve->setYAxis(QwtPlot::yLeft);
     pEndSetCurve->setStyle(QwtPlotCurve::NoCurve);
     pEndSetCurve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(0,94,185), QColor(0,94,185), QSize(6, 6)));
     pEndSetCurve->attach(this);

     tEndSetCurve = new QwtPlotCurve("tEndSet");
     tEndSetCurve->setYAxis(QwtPlot::yLeft);
     tEndSetCurve->setStyle(QwtPlotCurve::NoCurve);
     tEndSetCurve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QColor(116,11,11), QColor(116,11,11), QSize(6, 6)));
     tEndSetCurve->attach(this);

     setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(360.0));

     replot();
}

void EcgCh::addIntSignal(QwtPlotCurve *curve, IntSignal signal)
{
    gsl_vector_int *v = signal->signal;

    QVector<QPointF> samples;
    for (long int i = 0; i < v->size; i++)
    {
        samples.push_back(signalCurve->sample(v->data[i*v->stride]));
    }

    curve->setSamples(samples);
}

void EcgCh::setSignal(ECGSignalChannel signal, ECGChannelInfo info)
{
    gsl_vector *v = signal->signal;

    QVector<QPointF> samples;
    for (long int i = 0; i < v->size; i++) {
        samples.push_back(QPointF(float(i), float(v->data[i*v->stride])));
    }

    signalCurve->setSamples(samples);
    setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(info.frequecy));
    setAxisScaleDraw(QwtPlot::yLeft, new GainScaleDraw(info.gain));
    replot();
}

 void EcgCh::setSignal(ECGSignalChannel signal, ECGChannelInfo info, IntSignal peaks, ECGWaves* waves)
 {
    setSignal(signal, info);
    if (peaks)
    {
        addIntSignal(peaksCurve, peaks);
        emit rPeaksEnabled(true);
    } else emit rPeaksEnabled(false);

    if (waves)
    {
        if (waves->GetQRS_onset())
        {
            addIntSignal(qrsOnSetCurve, waves->GetQRS_onset());
            emit qrsOnEnabled(true);
        } else emit qrsOnEnabled(false);
        if (waves->GetQRS_end())
        {
            addIntSignal(qrsEndSetCurve, waves->GetQRS_end());
            emit qrsOffEnabled(true);
        } else emit qrsOffEnabled(false);
        if (waves->GetP_onset())
        {
            addIntSignal(pOnSetCurve, waves->GetP_onset());
            emit pOnEnabled(true);
        } else emit pOnEnabled(false);
        if (waves->GetP_end())
        {
            addIntSignal(pEndSetCurve, waves->GetP_end());
            emit pOffEnabled(true);
        } else emit pOffEnabled(false);
        if (waves->GetT_end())
        {
            addIntSignal(tEndSetCurve, waves->GetT_end());
            emit tOffEnabled(true);
        } else emit tOffEnabled(false);
    }
    else {
        emit qrsOnEnabled(false);
        emit qrsOffEnabled(false);
        emit pOnEnabled(false);
        emit pOffEnabled(false);
        emit tOffEnabled(false);
    }
 }

 void EcgCh::toggleRPeaks(bool toggle)
 {
     peaksCurve->setVisible(toggle);
     replot();
 }

 void EcgCh::toggleQrsOn(bool toggle)
 {
     qrsOnSetCurve->setVisible(toggle);
     replot();
 }

 void EcgCh::toggleQrsOff(bool toggle)
 {
     qrsEndSetCurve->setVisible(toggle);
     replot();
 }

 void EcgCh::togglePOn(bool toggle)
 {
     pOnSetCurve->setVisible(toggle);
     replot();
 }

 void EcgCh::togglePOff(bool toggle)
 {
     pEndSetCurve->setVisible(toggle);
     replot();
 }

 void EcgCh::toggleTOff(bool toggle)
 {
     tEndSetCurve->setVisible(toggle);
     replot();
 }


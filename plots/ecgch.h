#ifndef ECGCH_H
#define ECGCH_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>

class EcgCh : public QwtPlot
{
    Q_OBJECT

public:
    explicit EcgCh(QWidget *parent = 0);
    
signals:
    
public slots:
    void setSignal(double *x, int size); // -> ECGSignal
    void redraw();

private:
    QwtPlotCurve* curve;
    QwtPointSeriesData* data;
    QVector<QPointF>* samples;
    
};

#endif // ECGCH_H

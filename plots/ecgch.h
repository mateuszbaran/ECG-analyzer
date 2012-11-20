#ifndef ECGCH_H
#define ECGCH_H

#include "../ECGSignal.h"
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>

class EcgCh : public QwtPlot
{
    Q_OBJECT

public:
    explicit EcgCh(QWidget *parent = 0);
    
signals:
    
public slots:
    void setSignal(ECGSignalChannel signal);
    void redraw();

private:
    QwtPlotCurve* curve;
    QwtPointSeriesData* data;
    QVector<QPointF>* samples;
    QwtPlotZoomer *zoomer[2];
    QwtPlotPicker *picker;
    QwtPlotPanner *panner;
    
};

#endif // ECGCH_H

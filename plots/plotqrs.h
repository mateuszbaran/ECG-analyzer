#ifndef PLOTQRS_H
#define PLOTQRS_H

#include <QWidget>

#include "../ECGSignal.h"

#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_symbol.h>
#include <qwt6/qwt_plot_marker.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_marker.h>
#endif

class PlotQRS : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotQRS(QWidget *parent = 0);
	~PlotQRS();
    
signals:
    
public slots:
    void setData();

private:
    QwtPlotCurve *signal;
    QwtPlotMarker *qrs_onset;
    QwtPlotMarker *qrs_end;
    QwtPlotMarker *t_end;
    QwtPlotMarker *p_onset;
    QwtPlotMarker *p_end;
    QwtPlotMarker *t_wave_alt;
};

#endif // PLOTQRS_H

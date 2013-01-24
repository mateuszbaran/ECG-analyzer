#ifndef PLOTHRVFREQUENCY_H
#define PLOTHRVFREQUENCY_H

#include <QWidget>

#include "../ECGHRV1.h"
#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_plot_marker.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#endif

class PlotHRVFrequency : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotHRVFrequency(QWidget *parent = 0);
	~PlotHRVFrequency();
    
signals:
    
public slots:
    void setData(ECGHRV1 &data);

private:
    QwtPlotCurve *power;
    QwtPlotMarker *ulf;
    QwtPlotMarker *vlf;
    QwtPlotMarker *lf;
    QwtPlotMarker *hf;
};

#endif // PLOTHRVFREQUENCY_H

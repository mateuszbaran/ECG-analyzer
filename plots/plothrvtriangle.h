#ifndef PLOTHRVTRIANGLE_H
#define PLOTHRVTRIANGLE_H

#include <QWidget>

#include "../ECGHRV2.h"
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

class PlotHRVTriangle : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotHRVTriangle(QWidget *parent = 0);
	~PlotHRVTriangle();
    
signals:
    
public slots:
    void setData(ECGHRV2 &data);

private:
    QwtPlotCurve *rr;
    QwtPlotCurve *xn;
    QwtPlotCurve *xm;
    QwtPlotMarker *mx;
    QwtPlotMarker *my;
    QwtPlotMarker *mn;
    QwtPlotMarker *mm;
};

#endif // PLOTHRVTRIANGLE_H

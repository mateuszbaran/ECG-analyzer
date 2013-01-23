#ifndef PLOTHRT_H
#define PLOTHRT_H

#include <QWidget>

#include "../ECGHRT.h"
#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_grid.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_scale_draw.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#endif

class PlotHRT : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotHRT(QWidget *parent = 0);
	~PlotHRT();
    
signals:
    
public slots:
    void setData(ECGHRT &data);

private:
	int offset;
    QwtPlotCurve* rr;
    QwtPlotCurve* ts;
    
};

#endif // PLOTHRT_H

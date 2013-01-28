#ifndef PLOTPOINCARE_H
#define PLOTPOINCARE_H

#include <QWidget>

#include "../ECGHRV2.h"
#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_symbol.h>
#include <qwt6/qwt_plot_canvas.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_canvas.h>
#endif

class PlotPoincare : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotPoincare(QWidget *parent = 0);
	~PlotPoincare();
    
signals:
    
public slots:
    void setData(ECGHRV2 &data);
    void toggleSD(bool checked);

private:
    QwtPlotCurve *rr;
    QwtPlotCurve *sd1;
    QwtPlotCurve *sd2;
};

#endif // PLOTPOINCARE_H

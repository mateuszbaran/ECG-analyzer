#ifndef PLOTDFA1_H
#define PLOTDFA1_H

#include <QWidget>

#include "../ECGHRVDFA.h"
#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_symbol.h>
#include <qwt6/qwt_scale_engine.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_scale_engine.h>
#endif

class PlotDFA1 : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotDFA1(QWidget *parent = 0);
	~PlotDFA1();
    
signals:
    
public slots:
    void setData(ECGHRVDFA &data);

private:
    QwtPlotCurve *curve;
};

#endif // PLOTDFA1_H

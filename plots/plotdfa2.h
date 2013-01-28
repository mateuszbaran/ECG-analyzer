#ifndef PLOTDFA2_H
#define PLOTDFA2_H

#include <QWidget>

#include "../ECGHRVDFA.h"
#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_legend.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#endif

class PlotDFA2 : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotDFA2(QWidget *parent = 0);
	~PlotDFA2();
    
signals:
    
public slots:
    void setData(ECGHRVDFA &data);

private:
    QwtPlotCurve *rrint;
};

#endif // PLOTDFA1_H

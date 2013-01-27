#ifndef PLOTTACHOGRAM_H
#define PLOTTACHOGRAM_H

#include <QWidget>

#include "../ECGHRV1.h"
#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#else
#include <qwt_plot.h>
#endif

class PlotTachogram : public QwtPlot
{
    Q_OBJECT

public:
    explicit PlotTachogram(QWidget *parent = 0);
	~PlotTachogram();
    
signals:
    
public slots:
    void setData(ECGHRV1 &data);

private:
};

#endif // PLOTTACHOGRAM_H

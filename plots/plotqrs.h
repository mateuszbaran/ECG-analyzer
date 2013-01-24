#ifndef PLOTQRS_H
#define PLOTQRS_H

#include <QWidget>

#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#else
#include <qwt_plot.h>
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
};

#endif // PLOTQRS_H

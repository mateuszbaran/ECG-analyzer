#ifndef SYNCZOOMER_H
#define SYNCZOOMER_H

#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_grid.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_series_data.h>
#include <qwt6/qwt_picker_machine.h>
#include <qwt6/qwt_plot_zoomer.h>
#include <qwt6/qwt_plot_panner.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#endif

class PlotControl : public QObject
{
    Q_OBJECT
public:
    PlotControl(QwtPlot *plot1, QwtPlot *plot2);
    void setZoomBase();
public slots:
    void enableSync(bool enable);
private:
    QwtPlot *plot1;
    QwtPlot *plot2;
    QwtPlotZoomer *zoomer1;
    QwtPlotZoomer *zoomer2;
    QwtPlotPanner *panner1;
    QwtPlotPanner *panner2;
};

#endif // SYNCZOOMER_H

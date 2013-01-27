#ifndef SYNCZOOMER_H
#define SYNCZOOMER_H

#include "scrollzoomer.h"

#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_grid.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_series_data.h>
#include <qwt6/qwt_picker_machine.h>
#include <qwt6/qwt_plot_zoomer.h>
#include <qwt6/qwt_plot_panner.h>
#include <qwt6/qwt_plot_magnifier.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#endif

class Zoomer;

class PlotControl : public QObject
{
    Q_OBJECT
public:
    PlotControl(QwtPlot *plot1, QwtPlot *plot2);
    void setZoomBase(QRectF &base, QRectF &current);
    void resize(QResizeEvent* e);

public slots:
    void enableSync(bool enable);
    void zoomIn(Zoomer* zoomer);
    void zoomInFirst();
    void zoomInSecond();
    void zoomOut(Zoomer* zoomer);
    void zoomOutFirst();
    void zoomOutSecond();
    void zoomReset(Zoomer* zoomer);
    void zoomResetFirst();
    void zoomResetSecond();


private:
    QwtPlot *plot1;
    QwtPlot *plot2;
    Zoomer *zoomer1;
    Zoomer *zoomer2;
    bool isSyncEnabled;
    QRectF defaultRect;
};

#endif // SYNCZOOMER_H

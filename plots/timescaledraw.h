#ifndef TIMESCALEDRAW_H
#define TIMESCALEDRAW_H

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

class TimeScaleDraw : public QwtScaleDraw
{
public:
    TimeScaleDraw(int);
    ~TimeScaleDraw();
    QwtText label(double) const;
private:
    int frequency;
};

#endif // TIMESCALEDRAW_H

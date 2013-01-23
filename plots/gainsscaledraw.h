#ifndef GAINSSCALEDRAW_H
#define GAINSSCALEDRAW_H

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

class GainScaleDraw : public QwtScaleDraw
{
public:
    GainScaleDraw(float);
    ~GainScaleDraw();
    QwtText label(double) const;
private:
    float invgain;
};

#endif // GAINSSCALEDRAW_H

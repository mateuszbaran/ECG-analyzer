#include "synczoomer.h"
#include "scrollzoomer.h"
#include "qglobal.h"

#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot_grid.h>
#include <qwt6/qwt_plot_canvas.h>
#include <qwt6/qwt_plot_layout.h>
#include <qwt6/qwt_scale_widget.h>
#include <qwt6/qwt_scale_draw.h>
#include <qwt6/qwt_plot_magnifier.h>
#else
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_magnifier.h>
#endif

const unsigned int c_rangeMax = 1000;

class Zoomer: public ScrollZoomer
{
public:
    Zoomer( QwtPlotCanvas *canvas ):
            ScrollZoomer( canvas )
    {
#if 0
        setRubberBandPen( QPen( Qt::red, 2, Qt::DotLine ) );
#else
        setRubberBandPen( QPen( Qt::red ) );
#endif
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );

        QwtText text = QwtPlotZoomer::trackerTextF( pos );
        text.setBackgroundBrush( QBrush( bg ) );
        return text;
    }

    virtual void Zoomer::widgetMousePressEvent(QMouseEvent *e)
    {
    }

    virtual void Zoomer::widgetMouseReleaseEvent(QMouseEvent *)
    {

    }

    virtual void rescale()
    {
        QwtScaleWidget *scaleWidget = plot()->axisWidget( yAxis() );
        QwtScaleDraw *sd = scaleWidget->scaleDraw();

        double minExtent = 0.0;
        if ( zoomRectIndex() > 0 )
        {
            // When scrolling in vertical direction
            // the plot is jumping in horizontal direction
            // because of the different widths of the labels
            // So we better use a fixed extent.

            minExtent = sd->spacing() + sd->maxTickLength() + 1;
            minExtent += sd->labelSize(
                        scaleWidget->font(), c_rangeMax ).width();
        }

        sd->setMinimumExtent( minExtent );

        ScrollZoomer::rescale();
    }
};


PlotControl::PlotControl(QwtPlot *_plot1, QwtPlot *_plot2)
{
    plot1 = _plot1;
    plot2 = _plot2;
    QwtPlotCanvas *canvas1 = plot1->canvas();
    QwtPlotCanvas *canvas2 = plot2->canvas();
    zoomer1 = new Zoomer(canvas1);
    zoomer2 = new Zoomer(canvas2);
}

void PlotControl::setZoomBase(QRectF& rect)
{
    zoomer1->setZoomBase();
    zoomer2->setZoomBase();
    zoomer1->zoom(rect);
    zoomer2->zoom(rect);
}

void PlotControl::enableSync(bool enable)
{
    if (enable)
    {
        connect(zoomer1, SIGNAL(zoomed(QRectF)), zoomer2, SLOT(zoom(QRectF)));
        connect(zoomer2, SIGNAL(zoomed(QRectF)), zoomer1, SLOT(zoom(QRectF)));
    }
    else
    {
        plot1->enableAxis(QwtPlot::xBottom, true);
        disconnect(zoomer1, 0, zoomer2, 0);
        disconnect(zoomer2, 0, zoomer1, 0);
    }
}

void PlotControl::zoomOutFirst()
{
    QRectF rect = zoomer1->zoomStack().last();
    QPointF topLeft(rect.topLeft().x() - rect.width() * 0.2, rect.topLeft().y() - rect.height() * 0.2);
    QPointF bottomRight(rect.bottomRight().x() + rect.width() * 0.2, rect.bottomRight().y() + rect.height() * 0.2);
    QRectF newRect(topLeft, bottomRight);
    zoomer1->zoom(newRect);
}

void PlotControl::zoomInSecond()
{

}

void PlotControl::zoomInFirst()
{
    QRectF rect = zoomer1->zoomStack().last();
    QPointF topLeft(rect.topLeft().x() + rect.width() * 0.24, rect.topLeft().y() + rect.height() * 0.24);
    QPointF bottomRight(rect.bottomRight().x() - rect.width() * 0.24, rect.bottomRight().y() - rect.height() * 0.24);
    QRectF newRect(topLeft, bottomRight);
    zoomer1->zoom(newRect);
}

void PlotControl::zoomOutSecond()
{

}

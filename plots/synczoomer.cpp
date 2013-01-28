#include "synczoomer.h"
#include "scrollzoomer.h"
#include "qglobal.h"
#include "stdlib.h"
#include <QResizeEvent>
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

#include "qdebug.h"

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

    virtual void widgetMousePressEvent(QMouseEvent *e)
    {
    }

    virtual void widgetMouseReleaseEvent(QMouseEvent *)
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
    isSyncEnabled = true;
}

void PlotControl::setZoomBase(QRectF& base, QRectF& current)
{
    this->defaultRect = current;
    zoomer1->zoom(base);
    zoomer2->zoom(base);
    zoomer1->setZoomBase();
    zoomer2->setZoomBase();
    zoomer1->zoom(current);
    zoomer2->zoom(current);
}

void PlotControl::enableSync(bool enable)
{
    isSyncEnabled = enable;
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

void PlotControl::zoomIn(Zoomer *zoomer)
{
    QRectF rect = zoomer->zoomStack().last();
    QPointF topLeft(rect.topLeft().x() + rect.width() * 0.0816, rect.topLeft().y() + rect.height() * 0.0816);
    QPointF bottomRight(rect.bottomRight().x() - rect.width() * 0.0816, rect.bottomRight().y() - rect.height() * 0.0816);
    QRectF newRect(topLeft, bottomRight);
    zoomer->zoom(newRect);
}

void PlotControl::zoomOut(Zoomer *zoomer)
{
    QRectF rect = zoomer->zoomStack().last();
    double dimensionFactor = rect.height() / rect.width();

    float newWidth = 1.2 * rect.width();
    float x_offset = (newWidth - rect.width()) * 0.5;

    float newLeftX = std::max((float)zoomer->zoomBase().left(), (float)rect.left() - x_offset);
    float newRightX = std::min(newLeftX + newWidth, (float)zoomer->zoomBase().right());
    newWidth = newRightX - newLeftX;

    float newHeight = std::min((float)(dimensionFactor * newWidth), (float)zoomer->zoomBase().height());
    newWidth = newHeight / dimensionFactor;
    x_offset = (newWidth - rect.width()) * 0.5;

    newLeftX = std::max((float)zoomer->zoomBase().left(), (float)rect.left() - x_offset);

    float y_offset = (newHeight - rect.height()) * 0.5;

    QPointF topLeft(newLeftX, rect.top() - y_offset);
    QPointF bottomRight(newLeftX + newWidth, rect.top() - y_offset + newHeight);

    QRectF newRect(topLeft, bottomRight);
    zoomer->zoom(newRect);
}

void PlotControl::zoomReset(Zoomer *zoomer)
{
    QRectF rect = zoomer->zoomStack().last();

    if (rect.width() > defaultRect.width())
    {
        QPointF topLeft(rect.center().x() - defaultRect.width() * 0.5, rect.center().y() - defaultRect.height() * 0.5);
        QPointF bottomRight(rect.center().x() + defaultRect.width() * 0.5, rect.center().y() + defaultRect.height() * 0.5);
        QRectF newRect(topLeft, bottomRight);
        zoomer->zoom(newRect);
    }
    else
    {
        QPointF center = rect.center();
        if ((center.x() - zoomer->zoomBase().left()) < (defaultRect.width() * 0.5))
        {
            center.setX(zoomer->zoomBase().left() + (defaultRect.width() * 0.5));
        }
        else if ((zoomer->zoomBase().right() - center.x()) < (defaultRect.width() * 0.5))
        {
            center.setX(zoomer->zoomBase().right() - (defaultRect.width() * 0.5));
        }
        QPointF topLeft(center.x() - defaultRect.width() * 0.5, center.y() - defaultRect.height() * 0.5);
        QPointF bottomRight(center.x() + defaultRect.width() * 0.5, center.y() + defaultRect.height() * 0.5);
        QRectF newRect(topLeft, bottomRight);
        zoomer->zoom(newRect);
    }
}

void PlotControl::zoomInFirst()
{
    this->zoomIn(zoomer1);
}

void PlotControl::zoomOutFirst()
{
    this->zoomOut(zoomer1);
}

void PlotControl::zoomResetFirst()
{
    this->zoomReset(zoomer1);
}

void PlotControl::zoomInSecond()
{
    this->zoomIn(zoomer2);
}

void PlotControl::zoomOutSecond()
{
    this->zoomOut(zoomer2);
}

void PlotControl::zoomResetSecond()
{
    this->zoomReset(zoomer2);
}

void PlotControl::resize(QResizeEvent *e)
{
    double diff = (e->size().width() - e->oldSize().width()) * (defaultRect.width() / zoomer1->canvas()->width());

    if (defaultRect.width())
    {
        double nleft = defaultRect.left() - diff/2.0f;
        double nright = defaultRect.right() + diff/2.0f;
        defaultRect.setLeft(nleft);
        defaultRect.setRight(nright);
    }

    if (diff < 0)
    {

        QRectF zoom_rect1 = zoomer1->zoomRect();
        double nleft = zoom_rect1.left() - diff/2.0f;
        double nright = zoom_rect1.right() + diff/2.0f;
        zoom_rect1.setLeft(nleft);
        zoom_rect1.setRight(nright);
        zoomer1->zoom(zoom_rect1);

        if (!isSyncEnabled)
        {
            QRectF zoom_rect2 = zoomer2->zoomRect();
            nleft = zoom_rect2.left() - diff/2.0f;
            nright = zoom_rect2.right() + diff/2.0f;
            zoom_rect2.setLeft(nleft);
            zoom_rect2.setRight(nright);
            zoomer2->zoom(zoom_rect2);
        }
    }
    else
    {
        QRectF zoom_rect1 = zoomer1->zoomRect();
        double nleft = std::max(zoom_rect1.left() - diff/2.0f, 0.0);
        double nright = std::min(zoom_rect1.right() + diff/2.0f, (double)zoomer1->zoomBase().right());
        if (zoom_rect1.left() < diff/2.0f)
        {
            nright += diff/2.0f - zoom_rect1.left();
        }
        if ((double)zoomer1->zoomBase().right() - zoom_rect1.right() < diff/2.0f)
        {
            nleft -= diff/2.0f - ((double)zoomer1->zoomBase().right() - zoom_rect1.right());
        }
        zoom_rect1.setLeft(nleft);
        zoom_rect1.setRight(nright);
        zoomer1->zoom(zoom_rect1);

        QRectF zoom_rect2 = zoomer2->zoomRect();
        if (!isSyncEnabled)
        {
            nleft = std::max(zoom_rect2.left() - diff/2.0f, 0.0);
            nright = std::min(zoom_rect2.right() + diff/2.0f, (double)zoomer2->zoomBase().right());
            if (zoom_rect2.left() < diff/2.0f)
            {
                nright += diff/2.0f - zoom_rect2.left();
            }
            if ((double)zoomer2->zoomBase().right() - zoom_rect2.right() < diff/2.0f)
            {
                nleft -= diff/2.0f - ((double)zoomer2->zoomBase().right() - zoom_rect2.right());
            }
            zoom_rect2.setLeft(nleft);
            zoom_rect2.setRight(nright);
            zoomer2->zoom(zoom_rect2);
        }
    }

    zoomer1->plot()->replot();
    zoomer2->plot()->replot();
}

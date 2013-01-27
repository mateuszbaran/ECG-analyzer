#include "plottachogram.h"

PlotTachogram::PlotTachogram(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	replot();
}

PlotTachogram::~PlotTachogram()
{
}

void PlotTachogram::setData(ECGHRV1 &data)
{
	replot();
}

#include "plotqrs.h"

PlotQRS::PlotQRS(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	replot();
}

PlotQRS::~PlotQRS()
{
}

void PlotQRS::setData()
{
	replot();
}

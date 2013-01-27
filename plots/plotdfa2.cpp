#include "plotdfa2.h"

PlotDFA2::PlotDFA2(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	
	setAxisTitle(QwtPlot::xBottom, "Ilość RR, k");
	setAxisTitle(QwtPlot::yLeft, "y(k)");
	
	rrint = new QwtPlotCurve("RR integrated");
	rrint->attach(this);
	
	replot();
}

PlotDFA2::~PlotDFA2()
{
	delete rrint;
}

void PlotDFA2::setData(ECGHRVDFA &data)
{
	OtherSignal got = data.GetRR_integrated();
	QVector<QPointF> points;
	size_t size = got->signal->size;
	for (size_t i = 0; i < size; ++i)
	{
		points.push_back(QPointF(float(i), float(got->get(i))));
	}
	rrint->setSamples(points);
	replot();
}

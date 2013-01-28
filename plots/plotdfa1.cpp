#include "plotdfa1.h"

PlotDFA1::PlotDFA1(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	
	setAxisTitle(QwtPlot::xBottom, "F(n)");
	setAxisTitle(QwtPlot::yLeft, "n");
	
    setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
    setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
	
	curve = new QwtPlotCurve("Curve");
	curve->setStyle(QwtPlotCurve::NoCurve);
	curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));
	curve->attach(this);
	
	replot();
}

PlotDFA1::~PlotDFA1()
{
	delete curve;
}

void PlotDFA1::setData(ECGHRVDFA &data)
{
	OtherSignal x = data.GetDfa_x();
	OtherSignal y = data.GetDfa_y();
	QVector<QPointF> points;
    if (x && y)
    {
        size_t size = x->signal->size;
        for (size_t i = 0; i < size; ++i)
        {
            points.push_back(QPointF(float(x->get(i)), float(y->get(i))));
        }
        curve->setSamples(points);
    }
	replot();
}

#include "plotpoincare.h"

PlotPoincare::PlotPoincare(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	setAxisTitle(QwtPlot::xBottom, "RR(j) [ms]");
	setAxisTitle(QwtPlot::yLeft, "RR(j+1) [ms]");
	rr = new QwtPlotCurve("Poincare");
	sd1 = new QwtPlotCurve("S1");
	sd2 = new QwtPlotCurve("S2");
	rr->setStyle(QwtPlotCurve::NoCurve);
	rr->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, Qt::NoBrush, QPen(Qt::blue), QSize(5, 5)));
	sd1->setPen(QPen(Qt::red, 5));
	sd2->setPen(QPen(Qt::green, 5));
	rr->attach(this);
	sd1->attach(this);
	sd2->attach(this);
	replot();
}

PlotPoincare::~PlotPoincare()
{
	delete rr;
	delete sd1;
	delete sd2;
}

void PlotPoincare::setData(ECGHRV2 &data)
{
	gsl_vector_int *px = data.GetPoincare_x()->signal;
	gsl_vector_int *py = data.GetPoincare_y()->signal;
	double x = data.GetX();
	double y = data.GetY();
	double s1 = data.GetSD1();
	double s2 = data.GetSD2();
	int size = int(px->size); // == py.size
	QVector<QPointF> points;
	for (int i = 0; i < size; ++i)
	{
		points.push_back(QPointF(float(px->data[i*px->stride]), float(py->data[i*py->stride])));
	}
	rr->setSamples(points);
	QVector<QPointF> l1;
	l1.push_back(QPointF(x,y));
	l1.push_back(QPointF(x-s1,y+s1));
	QVector<QPointF> l2;
	l2.push_back(QPointF(x,y));
	l2.push_back(QPointF(x+s2,y+s2));
	sd1->setSamples(l1);
	sd1->setSamples(l2);
	replot();
}
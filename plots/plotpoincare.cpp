#include "plotpoincare.h"

PlotPoincare::PlotPoincare(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	setAxisTitle(QwtPlot::xBottom, "RR(j) [ms]");
    setAxisTitle(QwtPlot::yLeft, "RR(j+1) [ms]");
    setAxisAutoScale(QwtPlot::xBottom, false);
    setAxisAutoScale(QwtPlot::yLeft, false);
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
	IntSignal px = data.GetPoincare_x();
	IntSignal py = data.GetPoincare_y();
	float x = 0.0;
	float y = 0.0;
	float ax = 0.0;
	float ay = 0.0;
	float s1 = data.GetSD1() * 1.414;
	float s2 = data.GetSD2() * 1.414;
	size_t size = px->signal->size;
    int minX, maxX, minY, maxY;
    gsl_vector_int_minmax(px->signal, &minX, &maxX);
    gsl_vector_int_minmax(py->signal, &minY, &maxY);
    QVector<QPointF> points;
	for (int i = 0; i < size; ++i)
	{
		x = float(px->get(i));
		y = float(py->get(i));
		ax += x;
		ay += y;
		points.push_back(QPointF(x, y));
	}
	rr->setSamples(points);

	ax /= float(size);
	ay /= float(size);
	QVector<QPointF> l1;
	l1.push_back(QPointF(ax,ay));
	l1.push_back(QPointF(ax-s1,ay+s1));
	QVector<QPointF> l2;
	l2.push_back(QPointF(ax,ay));
	l2.push_back(QPointF(ax+s2,ay+s2));
	sd1->setSamples(l1);
    sd2->setSamples(l2);

    double start = std::min((double)minX, (double)minY);
    double end = std::max((double)maxX, (double)maxY);
    double x_end = start + (canvas()->width() / canvas()->height()) * (end - start);
    setAxisScale(QwtPlot::yLeft, start, end);
    setAxisScale(QwtPlot::xBottom, start, x_end);

    replot();
}

void PlotPoincare::toggleSD(bool checked)
{
    sd1->setVisible(checked);
    sd2->setVisible(checked);
    replot();
}

#include "plothrvtriangle.h"

PlotHRVTriangle::PlotHRVTriangle(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	
	setAxisTitle(QwtPlot::xBottom, "TINN [s]");
	setAxisTitle(QwtPlot::yLeft, "Liczba wszystkich odstępów RR");
	
	rr = new QwtPlotCurve("RR");
	rr->setStyle(QwtPlotCurve::Sticks);
	rr->setOrientation(Qt::Orientation::Vertical);
	rr->attach(this);
	
	xn = new QwtPlotCurve("X-N");
	xn->setPen(QPen(Qt::darkGray, 0, Qt::DashLine));
	xn->attach(this);
	
	xm = new QwtPlotCurve("X-M");
	xm->setPen(QPen(Qt::darkGray, 0, Qt::DashLine));
	xm->attach(this);
	
	mx = new QwtPlotMarker();
	mx->setLineStyle(QwtPlotMarker::VLine);
	mx->setLabel(tr("X"));
	mx->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	mx->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	mx->attach(this);

	my = new QwtPlotMarker();
	mx->setLineStyle(QwtPlotMarker::HLine);
	mx->setLabel(tr("Y"));
	mx->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
	mx->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	my->attach(this);

	mn = new QwtPlotMarker();
	mx->setLineStyle(QwtPlotMarker::VLine);
	mx->setLabel(tr("X"));
	mx->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
	mx->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	mn->attach(this);

	mm = new QwtPlotMarker();
	mx->setLineStyle(QwtPlotMarker::VLine);
	mx->setLabel(tr("X"));
	mx->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
	mx->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	mm->attach(this);
	
}

PlotHRVTriangle::~PlotHRVTriangle()
{
	delete rr;
	delete xn;
	delete xm;
	delete mx;
	delete my;
	delete mn;
	delete mm;
}

void PlotHRVTriangle::setData(ECGHRV2 &data)
{
	gsl_vector_int *hx = data.GetHistogram_x()->signal;
	gsl_vector_int *hy = data.GetHistogram_y()->signal;
	double x = data.GetX();
	double y = data.GetY();
	double n = data.GetN();
	double m = data.GetM();
	int size = int(hx->size); // == hy.size
	
	QVector<QPointF> peaks;
	for (int i = 0; i < size; ++i)
	{
		peaks.push_back(QPointF(float(hx->data[i*hx->stride]), float(hy->data[i*hy->stride])));
	}
	rr->setSamples(peaks);
	
	QVector<QPointF> vxn;
	vxn.push_back(QPointF(x, y));
	vxn.push_back(QPointF(n, 0.0));
	QVector<QPointF> vxm;
	vxm.push_back(QPointF(x, y));
	vxm.push_back(QPointF(m, 0.0));
	xn->setSamples(vxn);
	xm->setSamples(vxm);
    mx->setValue(x, y);
    my->setValue(m, y);
    mn->setValue(n, 0.0);
    mm->setValue(m, 0.0);
	replot();
}
#include "plothrvtriangle.h"
#include "boost/format.hpp"

PlotHRVTriangle::PlotHRVTriangle(QWidget *parent) :
	QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	
    setAxisTitle(QwtPlot::xBottom, "Numer przedziału");
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
	mx->setLabel(tr("N"));
	mx->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
	mx->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	mn->attach(this);

	mm = new QwtPlotMarker();
	mx->setLineStyle(QwtPlotMarker::VLine);
	mx->setLabel(tr("M"));
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
	IntSignal hx = data.GetHistogram_x();
	IntSignal hy = data.GetHistogram_y();
	double x = data.GetX();
	double y = data.GetY();
	double n = data.GetN();
	double m = data.GetM();
	size_t size = hx->signal->size;
	QVector<QPointF> peaks;
	for (int i = 0; i < size; ++i)
	{
		peaks.push_back(QPointF(float(hx->get(i)), float(hy->get(i))));
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
    std::string title = (boost::format("Numer przedziału (długość: %f ms)") % data.GetHistogramBinLength()).str();
    setAxisTitle(QwtPlot::xBottom, title.c_str());
    replot();
}


void PlotHRVTriangle::toggleTriangle(bool checked)
{
    xn->setVisible(checked);
    xm->setVisible(checked);
    mx->setVisible(checked);
    my->setVisible(checked);
    mn->setVisible(checked);
    mm->setVisible(checked);
    replot();
}

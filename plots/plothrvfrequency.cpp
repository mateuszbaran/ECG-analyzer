#include "plothrvfrequency.h"

PlotHRVFrequency::PlotHRVFrequency(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	
	setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
	setAxisTitle(QwtPlot::yLeft, "Power msec sq");
	
	power = new QwtPlotCurve("Power");
	power->setStyle(QwtPlotCurve::Sticks);
	power->setOrientation(Qt::Orientation::Vertical);
	power->attach(this);
	
	ulf = new QwtPlotMarker();
	ulf->setLineStyle(QwtPlotMarker::VLine);
	ulf->setLabel(tr("ULF"));
	ulf->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	ulf->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	ulf->attach(this);
	
	vlf = new QwtPlotMarker();
	vlf->setLineStyle(QwtPlotMarker::VLine);
	vlf->setLabel(tr("VLF"));
	vlf->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	vlf->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	vlf->attach(this);
	
	lf = new QwtPlotMarker();
	lf->setLineStyle(QwtPlotMarker::VLine);
	lf->setLabel(tr("LF"));
	lf->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	lf->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	lf->attach(this);
	
	hf = new QwtPlotMarker();
	hf->setLineStyle(QwtPlotMarker::VLine);
	hf->setLabel(tr("HF"));
	hf->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	hf->setLinePen(QPen(Qt::gray, 0, Qt::DashLine));
	hf->attach(this);
	
	replot();
}

PlotHRVFrequency::~PlotHRVFrequency()
{
	delete power;
	delete ulf;
	delete vlf;
	delete lf;
	delete hf;
}

void PlotHRVFrequency::setData(ECGHRV1 &data)
{
	OtherSignal freq = data.freqency;
	OtherSignal pow = data.power;
	QVector<QPointF> peaks;
	size_t size = freq->signal->size;
	for (size_t i = 0; i < size; ++i)
	{
		peaks.push_back(QPointF(float(freq->get(i)), float(pow->get(i))));
	}
	power->setSamples(peaks);
	
	// TODO
	ulf->setValue(1.0, 0.0);
	vlf->setValue(2.0, 0.0);
	lf->setValue(3.0, 0.0);
	hf->setValue(4.0, 0.0);
	replot();
}

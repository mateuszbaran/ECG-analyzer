#include "plothrt.h"

class LabeledScaleDraw : public QwtScaleDraw
{
public:
	LabeledScaleDraw(int);
	~LabeledScaleDraw();
	QwtText label(double) const;
private:
	int offset;
};

LabeledScaleDraw::LabeledScaleDraw(int _offset) : QwtScaleDraw(), offset(_offset) { setLabelRotation(330); setLabelAlignment(Qt::AlignLeft);}
LabeledScaleDraw::~LabeledScaleDraw() { }

QwtText LabeledScaleDraw::label(double value) const
{
	int index = int(value + 0.5);
	if (index < offset)
	{
		return QLocale::system().toString(index - offset);
	}
	else if (index == offset)
	{
		return QObject::tr("VPC");
	}
	else if (index == offset + 1)
	{
		return QObject::tr("PAUSE");
	}
	else
	{
        return QLocale::system().toString(index - offset);
	}
}

PlotHRT::PlotHRT(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableXMin(true);
	grid->setMajPen(QPen(Qt::white, 0, Qt::DotLine));
	grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
	grid->attach(this);

	setAxisTitle(QwtPlot::xBottom, "relative interval number");
	setAxisTitle(QwtPlot::yLeft, "RR [ms]");

    QwtLegend *legend = new QwtLegend();
    legend->setItemMode(QwtLegend::ReadOnlyItem);
    this->insertLegend(legend, QwtPlot::RightLegend);

    setAxisMaxMajor(QwtPlot::xBottom, 30);
	rr = new QwtPlotCurve("RR");
    ts = new QwtPlotCurve("Odcinek TS");
	ts->setPen(QPen(Qt::red, 2));
	rr->setYAxis(QwtPlot::yLeft);
	ts->setYAxis(QwtPlot::yLeft);
	rr->attach(this);
	ts->attach(this);
	replot();
}

PlotHRT::~PlotHRT()
{
	delete rr;
	delete ts;
}

void PlotHRT::setData(ECGHRT &data)
{
	offset = data.offset;
	setAxisScaleDraw(QwtPlot::xBottom, new LabeledScaleDraw(offset));
	rr->setSamples(data.rr);
	QVector<QPointF> data_ts;
	data_ts.push_back(data.ts.p1());
	data_ts.push_back(data.ts.p2());

	ts->setSamples(data_ts);
	replot();
}

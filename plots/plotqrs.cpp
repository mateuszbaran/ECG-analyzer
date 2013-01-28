#include "plotqrs.h"

PlotQRS::PlotQRS(QWidget *parent) :
    QwtPlot(parent)
{
	setMinimumHeight(10);
	setMinimumWidth(10);
	
	setAxisTitle(QwtPlot::xBottom, "n");
	setAxisTitle(QwtPlot::yLeft, "Amplituda [mV]");
	
	signal = new QwtPlotCurve("Signal");
	signal->attach(this);
	
	qrs_onset = new QwtPlotMarker();
	qrs_onset->setLineStyle(QwtPlotMarker::NoLine);
	qrs_onset->setSymbol(new QwtSymbol(QwtSymbol::Ellipse));
	qrs_onset->setLabel(tr("QRS onset"));
	qrs_onset->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	qrs_onset->attach(this);
	
	qrs_end = new QwtPlotMarker();
	qrs_end->setLineStyle(QwtPlotMarker::NoLine);
	qrs_end->setSymbol(new QwtSymbol(QwtSymbol::Ellipse));
	qrs_end->setLabel(tr("QRS end"));
	qrs_end->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	qrs_end->attach(this);
	
	t_end = new QwtPlotMarker();
	t_end->setLineStyle(QwtPlotMarker::NoLine);
	t_end->setSymbol(new QwtSymbol(QwtSymbol::Ellipse));
	t_end->setLabel(tr("T end"));
	t_end->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	t_end->attach(this);
	
	p_onset = new QwtPlotMarker();
	p_onset->setLineStyle(QwtPlotMarker::NoLine);
	p_onset->setSymbol(new QwtSymbol(QwtSymbol::Ellipse));
	p_onset->setLabel(tr("P onset"));
	p_onset->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	p_onset->attach(this);
	
	p_end = new QwtPlotMarker();
	p_end->setLineStyle(QwtPlotMarker::NoLine);
	p_end->setSymbol(new QwtSymbol(QwtSymbol::Ellipse));
	p_end->setLabel(tr("P end"));
	p_end->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	p_end->attach(this);
	
	t_wave_alt = new QwtPlotMarker();
	t_wave_alt->setLineStyle(QwtPlotMarker::NoLine);
	t_wave_alt->setSymbol(new QwtSymbol(QwtSymbol::Ellipse));
	t_wave_alt->setLabel(tr("T Wave Alt"));
	t_wave_alt->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
	t_wave_alt->attach(this);
	
	replot();
}

PlotQRS::~PlotQRS()
{
    delete signal;
    delete qrs_onset;
    delete qrs_end;
    delete t_end;
    delete p_onset;
    delete p_end;
    delete t_wave_alt;
}

void PlotQRS::setData()
{
	OtherSignal _signal;
	int _qrs_onset;
	int _qrs_end;
	int _t_end;
	int _p_onset;
	int _p_end;
	int _t_wave_alt;
	size_t size = _signal->signal->size;
	QVector<QPointF> points;
	for (int i = 0; i < size; ++i)
	{
		points.push_back(QPointF(float(i), float(_signal->get(i))));
	}
	signal    ->setSamples(points);
	qrs_onset ->setValue(float(_qrs_onset),  float(_signal->get(_qrs_onset)));
	qrs_end   ->setValue(float(_qrs_end),    float(_signal->get(_qrs_end)));
	t_end     ->setValue(float(_t_end),      float(_signal->get(_t_end)));
	p_onset   ->setValue(float(_p_onset),    float(_signal->get(_p_onset)));
	p_end     ->setValue(float(_p_end),      float(_signal->get(_p_end)));
	t_wave_alt->setValue(float(_t_wave_alt), float(_signal->get(_t_wave_alt)));
	replot();
}

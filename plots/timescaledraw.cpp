#include "timescaledraw.h"
#include "ECGChannelInfo.h"

TimeScaleDraw::TimeScaleDraw(int _frequency) : QwtScaleDraw(), frequency(_frequency) { };
TimeScaleDraw::~TimeScaleDraw() { };

QwtText TimeScaleDraw::label(double value) const
{
    return QString::fromStdString(ECGChannelInfo::sampleToTime((int)value,this->frequency));
}


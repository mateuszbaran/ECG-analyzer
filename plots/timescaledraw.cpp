#include "timescaledraw.h"
#include "boost/format.hpp"
#include <math.h>
#include "ECGChannelInfo.h"

TimeScaleDraw::TimeScaleDraw() : QwtScaleDraw() { };
TimeScaleDraw::~TimeScaleDraw() { };

QwtText TimeScaleDraw::label(double value) const
{
//#ifdef VERBOSE_TIME_FORMAT
    return QObject::tr("NIC");
//#else

//#endif
}


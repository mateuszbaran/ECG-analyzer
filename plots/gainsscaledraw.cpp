#include "gainsscaledraw.h"

GainScaleDraw::GainScaleDraw(float gain) : QwtScaleDraw(), invgain(1.0 / gain) { };
GainScaleDraw::~GainScaleDraw() { };

QwtText GainScaleDraw::label(double value) const
{
    return QLocale::system().toString(value * invgain);
}

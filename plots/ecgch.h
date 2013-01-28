#ifndef ECGCH_H
#define ECGCH_H

#include "../ECGSignal.h"
#include "../ECGChannelInfo.h"
#include "../ECGWaves.h"

#ifdef WIN32
#define QWT_DLL
#include <qwt6/qwt_plot.h>
#include <qwt6/qwt_plot_grid.h>
#include <qwt6/qwt_plot_curve.h>
#include <qwt6/qwt_series_data.h>
#include <qwt6/qwt_picker_machine.h>
#include <qwt6/qwt_plot_zoomer.h>
#include <qwt6/qwt_plot_panner.h>
#include <qwt6/qwt_symbol.h>
#else
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_symbol.h>
#endif

class EcgCh : public QwtPlot
{
    Q_OBJECT

public:
    explicit EcgCh(QWidget *parent = 0);
signals:
    void rPeaksEnabled(bool enabled);
    void qrsOnEnabled(bool enabled);
    void qrsOffEnabled(bool enabled);
    void pOnEnabled(bool enabled);
    void pOffEnabled(bool enabled);
    void tOffEnabled(bool enabled);

public slots:
    void setSignal(ECGSignalChannel signal, ECGChannelInfo info);
    void setSignal(ECGSignalChannel signal, ECGChannelInfo info, IntSignal peaks, ECGWaves* waves);
    void toggleRPeaks(bool toggle);
    void toggleQrsOn(bool toggle);
    void toggleQrsOff(bool toggle);
    void togglePOn(bool toggle);
    void togglePOff(bool toggle);
    void toggleTOff(bool toggle);
private:
    void addIntSignal(QwtPlotCurve* curve, IntSignal signal);
    QwtPlotCurve* signalCurve;
    QwtPlotCurve* peaksCurve;
    QwtPlotCurve* qrsOnSetCurve;
    QwtPlotCurve* qrsEndSetCurve;
    QwtPlotCurve* pOnSetCurve;
    QwtPlotCurve* pEndSetCurve;
    QwtPlotCurve* tEndSetCurve;
    
};

#endif // ECGCH_H

#ifndef ECG2CH_H
#define ECG2CH_H

#include <QWidget>

#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include "ecgch.h"
#include "../ECGController.h"
#include "synczoomer.h"

class Ecg2Ch : public QWidget
{
    Q_OBJECT

public:
    explicit Ecg2Ch(QWidget *parent = 0);
    ~Ecg2Ch();
    
public slots:
    void setSignal(ECGSignal *signal, ECGInfo *info, ECGRs *peaks, ECGWaves *waves);
    void syncToggled(bool);
    void setLegendRPeaksEnabled(bool enabled);
    void setLegendQrsOnEnabled(bool enabled);
    void setLegendQrsOffEnabled(bool enabled);
    void setLegendPOnEnabled(bool enabled);
    void setLegendPOffEnabled(bool enabled);
    void setLegendTOffEnabled(bool enabled);

    virtual void resizeEvent(QResizeEvent* e);
private:
    EcgCh *ch1;
    EcgCh *ch2;
    PlotControl *control;

    QToolButton *syncButton, *zoomResetFirstButton, *zoomInFirstButton, *zoomOutFirstButton, *zoomResetSecondButton, *zoomInSecondButton, *zoomOutSecondButton, *legendButton;
    QMenu *legendMenu;
    QAction *legendRpeaks, *legendQrsOn, *legendQrsOff, *legendPOn, *legendPOff, *legendTOff;

};

#endif // ECG2CH_H

#ifndef ECG2CH_H
#define ECG2CH_H

#include <QWidget>

#include <QFileDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include "ecgch.h"
#include "../ECGController.h"
#include "synczoomer.h"

class Ecg2Ch : public QWidget
{
    Q_OBJECT

public:
    explicit Ecg2Ch(QWidget *parent = 0);
    ~Ecg2Ch();
    
signals:
    
public slots:
    void setSignal(ECGSignal *signal, ECGInfo *info);
    void setSignal(ECGSignal *signal, ECGInfo *info, ECGRs *peaks);

private:
    EcgCh *ch1;
    EcgCh *ch2;
    PlotControl *control;

};

#endif // ECG2CH_H

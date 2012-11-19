#ifndef ECG2CH_H
#define ECG2CH_H

#include <QWidget>

#include <QVBoxLayout>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include "ecgch.h"

class Ecg2Ch : public QWidget
{
    Q_OBJECT

public:
    explicit Ecg2Ch(QWidget *parent = 0);
    ~Ecg2Ch();
    
signals:
    
public slots:
    void setSignal(double *x, int size); // -> ECGSignal
    void redraw();
    void test();

private:
    EcgCh *ch1;
    EcgCh *ch2;

};

#endif // ECG2CH_H

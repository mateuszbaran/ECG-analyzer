#include "ecg2ch.h"

Ecg2Ch::Ecg2Ch(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QToolBar *toolBar = new QToolBar(this);
    QToolButton *toolButton = new QToolButton(toolBar);
    toolButton->setText("test()");
    toolBar->addWidget(toolButton);
    mainLayout->addWidget(toolBar);

    QWidget *plotsWidget = new QWidget(this);
    QVBoxLayout *plotsLayout = new QVBoxLayout(plotsWidget);
    EcgCh *plot1 = new EcgCh(plotsWidget);
    EcgCh *plot2 = new EcgCh(plotsWidget);
    plotsLayout->addWidget(plot1);
    plotsLayout->addWidget(plot2);
    plotsWidget->setLayout(plotsLayout);
    mainLayout->addWidget(plotsWidget);

    setLayout(mainLayout);

    ch1 = plot1;
    ch2 = plot2;

    connect(toolButton, SIGNAL(clicked()), SLOT(test()));
}

Ecg2Ch::~Ecg2Ch()
{
    delete ch1;
    delete ch2;
}

void Ecg2Ch::setSignal(double *x, int size)
{
    ch1->setSignal(x, size);
    ch2->setSignal(x, size);
    return;
}

void Ecg2Ch::redraw()
{
    ch1->redraw();
    ch2->redraw();
    return;
}

void Ecg2Ch::test()
{
    const int size = 500;
    double *x = new double[size];
    for (int i = 0; i < size; i++)
        x[i] = 2.0 * sin((3.0 * 3.14 / double(size)) * double(i));
    setSignal(x, size);
    redraw();
    delete[] x;
}

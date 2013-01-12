#include "ecg2ch.h"

Ecg2Ch::Ecg2Ch(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QToolBar *toolBar = new QToolBar(this);
    QToolButton *syncButton = new QToolButton(toolBar);
        syncButton->setText("sync");
        syncButton->setCheckable(true);
        toolBar->addWidget(syncButton);
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
    control = new PlotControl(plot1, plot2);

    connect(syncButton, SIGNAL(toggled(bool)), control, SLOT(enableSync(bool)));
}

Ecg2Ch::~Ecg2Ch()
{
    delete ch1;
    delete ch2;
}

void Ecg2Ch::setSignal(ECGSignal *signal, ECGInfo *info)
{
    ch1->setSignal(signal->channel_one, info->channel_one);
    ch2->setSignal(signal->channel_two, info->channel_two);
    control->setZoomBase();
    return;
}

void Ecg2Ch::setSignal(ECGSignal *signal, ECGInfo *info, ECGRs *peaks)
{
    ch1->setSignal(signal->channel_one, info->channel_one, peaks->GetRs());
    ch2->setSignal(signal->channel_two, info->channel_two, peaks->GetRs());
    control->setZoomBase();
    return;
}


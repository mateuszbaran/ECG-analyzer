#include "ecg2ch.h"
#include "math.h"

void gsl_vector_minmax_to_index(const gsl_vector *v, double* min_val, double* max_val, int index) {
    *min_val = gsl_vector_max(v);
    *max_val = gsl_vector_min(v);

    int size = min(int(v->size), index);

    for (int i = 0; i < size; i++)
    {
        *min_val = min(*min_val, v->data[i*v->stride]);
        *max_val = max(*max_val, v->data[i*v->stride]);
    }
}

Ecg2Ch::Ecg2Ch(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QToolBar *toolBar = new QToolBar(this);
    syncButton = new QToolButton(toolBar);
    syncButton->setIcon(QIcon(":/ECGanalyzer/icons/locked.png"));
    syncButton->setCheckable(true);
    syncButton->setChecked(true);
    toolBar->addWidget(syncButton);

    toolBar->addSeparator();

    zoomResetFirstButton = new QToolButton(toolBar);
    zoomResetFirstButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_one.png"));
    toolBar->addWidget(zoomResetFirstButton);
    zoomInFirstButton = new QToolButton(toolBar);
    zoomInFirstButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_plus.png"));
    toolBar->addWidget(zoomInFirstButton);
    zoomOutFirstButton = new QToolButton(toolBar);
    zoomOutFirstButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_minus.png"));
    toolBar->addWidget(zoomOutFirstButton);

    toolBar->addSeparator();

    zoomResetSecondButton = new QToolButton(toolBar);
    zoomResetSecondButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_one.png"));
    zoomResetSecondButton->setEnabled(false);
    toolBar->addWidget(zoomResetSecondButton);
    zoomInSecondButton = new QToolButton(toolBar);
    zoomInSecondButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_plus.png"));
    zoomInSecondButton->setEnabled(false);
    toolBar->addWidget(zoomInSecondButton);
    zoomOutSecondButton = new QToolButton(toolBar);
    zoomOutSecondButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_minus.png"));
    zoomOutSecondButton->setEnabled(false);
    toolBar->addWidget(zoomOutSecondButton);


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
    control->enableSync(true);
    connect(syncButton, SIGNAL(toggled(bool)), this, SLOT(syncToggled(bool)));
    connect(zoomResetFirstButton, SIGNAL(clicked()), control, SLOT(zoomResetFirst()));
    connect(zoomInFirstButton, SIGNAL(clicked()), control, SLOT(zoomInFirst()));
    connect(zoomOutFirstButton, SIGNAL(clicked()), control, SLOT(zoomOutFirst()));

    connect(zoomResetSecondButton, SIGNAL(clicked()), control, SLOT(zoomResetSecond()));
    connect(zoomInSecondButton, SIGNAL(clicked()), control, SLOT(zoomInSecond()));
    connect(zoomOutSecondButton, SIGNAL(clicked()), control, SLOT(zoomOutSecond()));
}

Ecg2Ch::~Ecg2Ch()
{
    delete ch1;
    delete ch2;
}

void Ecg2Ch::setSignal(ECGSignal *signal, ECGInfo *info)
{
//    double max_value_one, min_value_one, max_value_two, min_value_two;
//    gsl_vector_minmax_to_index(signal->channel_one->signal, &min_value_one, &max_value_one, 1000);
//    gsl_vector_minmax_to_index(signal->channel_two->signal, &min_value_two, &max_value_two, 1000);
//    float max_value = max(max_value_one, max_value_two);
//    float min_value = min(min_value_one, min_value_two);

//    ch1->setSignal(signal->channel_one, info->channel_one);
//    ch2->setSignal(signal->channel_two, info->channel_two);
//    QRectF rect(QPointF(0, max_value), QPointF(1000, min_value));
//    control->setZoomBase(rect);
}

void Ecg2Ch::setSignal(ECGSignal *signal, ECGInfo *info, ECGRs *peaks)
{
    double max_value_one, min_value_one, max_value_two, min_value_two;

    gsl_vector_minmax_to_index(signal->channel_one->signal, &min_value_one, &max_value_one, 1000);
    gsl_vector_minmax_to_index(signal->channel_two->signal, &min_value_two, &max_value_two, 1000);
    float max_value = max(max_value_one, max_value_two);
    float min_value = min(min_value_one, min_value_two);


    ch1->setSignal(signal->channel_one, info->channel_one, peaks->GetRs());
    ch2->setSignal(signal->channel_two, info->channel_two, peaks->GetRs());

    QRectF defaultRect(QPointF(0, max_value), QPointF((float)min(abs((int)(max_value * 1.3)), (int)signal->channel_one->signal->size), min_value));

    float min_global = min(gsl_vector_min(signal->channel_one->signal), gsl_vector_min(signal->channel_two->signal));
    float max_global = max(gsl_vector_max(signal->channel_one->signal), gsl_vector_max(signal->channel_two->signal));
    float last_x = (float)max(signal->channel_one->signal->size, signal->channel_two->signal->size);

    QRectF baseRect(QPointF(0.0, max_global), QPointF(last_x, min_global));

    control->setZoomBase(baseRect, defaultRect);
}

void Ecg2Ch::setSignal(ECGSignalChannel filtered_signal, ECGSignalChannel raw_signal, ECGInfo *info, ECGRs *peaks)
{
    double max_value_one, min_value_one, max_value_two, min_value_two;

    gsl_vector_minmax_to_index(filtered_signal->signal, &min_value_one, &max_value_one, 1000);
    gsl_vector_minmax_to_index(raw_signal->signal, &min_value_two, &max_value_two, 1000);
    float max_value = max(max_value_one, max_value_two);
    float min_value = min(min_value_one, min_value_two);

    ch1->setSignal(filtered_signal, info->channel_one, peaks->GetRs());
    ch2->setSignal(raw_signal, info->channel_one, peaks->GetRs());

    QRectF defaultRect(QPointF(0, max_value), QPointF((float)min(abs((int)(max_value * 1.3)), (int)raw_signal->signal->size), min_value));

    float min_global = min(gsl_vector_min(raw_signal->signal), gsl_vector_min(filtered_signal->signal));
    float max_global = max(gsl_vector_max(raw_signal->signal), gsl_vector_max(filtered_signal->signal));
    float last_x = (float)max(raw_signal->signal->size, filtered_signal->signal->size);

    QRectF baseRect(QPointF(0.0, max_global), QPointF(last_x, min_global));

    control->setZoomBase(baseRect, defaultRect);
}

void Ecg2Ch::syncToggled(bool toggle)
{
    if (toggle)
    {
        syncButton->setIcon(QIcon(":/ECGanalyzer/icons/locked.png"));
        zoomResetSecondButton->setEnabled(false);
        zoomInSecondButton->setEnabled(false);
        zoomOutSecondButton->setEnabled(false);
    }
    else
    {
        syncButton->setIcon(QIcon(":/ECGanalyzer/icons/unlocked.png"));
        zoomResetSecondButton->setEnabled(true);
        zoomInSecondButton->setEnabled(true);
        zoomOutSecondButton->setEnabled(true);
    }
    control->enableSync(toggle);
}

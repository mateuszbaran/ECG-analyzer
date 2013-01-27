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
    syncButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    syncButton->setText("Synchronizacja");
    syncButton->setCheckable(true);
    syncButton->setChecked(true);
    syncButton->setEnabled(false);
    toolBar->addWidget(syncButton);

    toolBar->addSeparator();

    zoomResetFirstButton = new QToolButton(toolBar);
    zoomResetFirstButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_one.png"));
    zoomResetFirstButton->setEnabled(false);
    toolBar->addWidget(zoomResetFirstButton);
    zoomInFirstButton = new QToolButton(toolBar);
    zoomInFirstButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_plus.png"));
    zoomInFirstButton->setEnabled(false);
    toolBar->addWidget(zoomInFirstButton);
    zoomOutFirstButton = new QToolButton(toolBar);
    zoomOutFirstButton->setIcon(QIcon(":/ECGanalyzer/icons/magnifier_minus.png"));
    zoomOutFirstButton->setEnabled(false);
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

    toolBar->addSeparator();

    legendMenu = new QMenu(toolBar);

    legendRpeaks = new QAction(QIcon(":/ECGanalyzer/icons/legend_red.png"), "Załamek R", legendMenu);
    legendRpeaks->setIconVisibleInMenu(true);
    legendRpeaks->setCheckable(true);
    legendMenu->setEnabled(false);
    legendMenu->addAction(legendRpeaks);

    legendQrsOn = new QAction(QIcon(":/ECGanalyzer/icons/legend_green.png"), "Początek QRS", legendMenu);
    legendQrsOn->setIconVisibleInMenu(true);
    legendQrsOn->setCheckable(true);
    legendMenu->addAction(legendQrsOn);

    legendQrsOff = new QAction(QIcon(":/ECGanalyzer/icons/legend_dark_green.png"), "Koniec QRS", legendMenu);
    legendQrsOff->setIconVisibleInMenu(true);
    legendQrsOff->setCheckable(true);
    legendMenu->addAction(legendQrsOff);

    legendPOn = new QAction(QIcon(":/ECGanalyzer/icons/legend_blue.png"), "Początek P", legendMenu);
    legendPOn->setIconVisibleInMenu(true);
    legendPOn->setCheckable(true);
    legendMenu->addAction(legendPOn);

    legendPOff = new QAction(QIcon(":/ECGanalyzer/icons/legend_dark_blue.png"), "Koniec P", legendMenu);
    legendPOff->setIconVisibleInMenu(true);
    legendPOff->setCheckable(true);
    legendMenu->addAction(legendPOff);

    legendTOff = new QAction(QIcon(":/ECGanalyzer/icons/legend_brown.png"), "Koniec T", legendMenu);
    legendTOff->setIconVisibleInMenu(true);
    legendTOff->setCheckable(true);
    legendMenu->addAction(legendTOff);

    setLegendRPeaksEnabled(false);
    setLegendPOffEnabled(false);
    setLegendPOnEnabled(false);
    setLegendQrsOffEnabled(false);
    setLegendQrsOnEnabled(false);
    setLegendTOffEnabled(false);

    legendButton = new QToolButton(toolBar);
    legendButton->setIcon(QIcon(":/ECGanalyzer/icons/legend.png"));
    legendButton->setText("Legenda");
    legendButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    legendButton->setMenu(legendMenu);
    legendButton->setPopupMode(QToolButton::InstantPopup);
    legendButton->setEnabled(false);
    legendButton->setCheckable(true);

    toolBar->addWidget(legendButton);
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

    connect(legendRpeaks, SIGNAL(toggled(bool)), ch1, SLOT(toggleRPeaks(bool)));
    connect(legendQrsOn, SIGNAL(toggled(bool)), ch1, SLOT(toggleQrsOn(bool)));
    connect(legendQrsOff, SIGNAL(toggled(bool)), ch1, SLOT(toggleQrsOff(bool)));
    connect(legendPOff, SIGNAL(toggled(bool)), ch1, SLOT(togglePOff(bool)));
    connect(legendPOn, SIGNAL(toggled(bool)), ch1, SLOT(togglePOn(bool)));
    connect(legendTOff, SIGNAL(toggled(bool)), ch1, SLOT(toggleTOff(bool)));

    connect(ch1, SIGNAL(rPeaksEnabled(bool)), this, SLOT(setLegendRPeaksEnabled(bool)));
    connect(ch1, SIGNAL(qrsOnEnabled(bool)), this, SLOT(setLegendQrsOnEnabled(bool)));
    connect(ch1, SIGNAL(qrsOffEnabled(bool)), this, SLOT(setLegendQrsOffEnabled(bool)));
    connect(ch1, SIGNAL(pOnEnabled(bool)), this, SLOT(setLegendPOnEnabled(bool)));
    connect(ch1, SIGNAL(pOffEnabled(bool)), this, SLOT(setLegendPOffEnabled(bool)));
    connect(ch1, SIGNAL(tOffEnabled(bool)), this, SLOT(setLegendTOffEnabled(bool)));
}

Ecg2Ch::~Ecg2Ch()
{
    delete ch1;
    delete ch2;
}

void Ecg2Ch::setSignal(ECGSignal *signal, ECGInfo *info, ECGRs *peaks, ECGWaves *waves)
{
    double max_value_one, min_value_one, max_value_two, min_value_two;

    gsl_vector_minmax_to_index(signal->channel_one->signal, &min_value_one, &max_value_one, 1000);
    gsl_vector_minmax_to_index(signal->channel_two->signal, &min_value_two, &max_value_two, 1000);
    float max_value = max(max_value_one, max_value_two);
    float min_value = min(min_value_one, min_value_two);


    ch1->setSignal(signal->channel_one, info->channel_one, peaks->GetRs(), waves);
    ch2->setSignal(signal->channel_two, info->channel_two);

    QRectF defaultRect(QPointF(0, max_value), QPointF((float)min(abs((int)(max_value * 1.3)), (int)signal->channel_one->signal->size), min_value));

    float min_global = min(gsl_vector_min(signal->channel_one->signal), gsl_vector_min(signal->channel_two->signal));
    float max_global = max(gsl_vector_max(signal->channel_one->signal), gsl_vector_max(signal->channel_two->signal));
    float last_x = (float)max(signal->channel_one->signal->size, signal->channel_two->signal->size);

    QRectF baseRect(QPointF(0.0, max_global), QPointF(last_x, min_global));

    control->setZoomBase(baseRect, defaultRect);

    zoomInFirstButton->setEnabled(true);
    zoomResetFirstButton->setEnabled(true);
    zoomOutFirstButton->setEnabled(true);
    syncButton->setEnabled(true);
    legendButton->setEnabled(true);
    legendMenu->setEnabled(true);
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

void Ecg2Ch::setLegendRPeaksEnabled(bool enabled) {
    legendRpeaks->setEnabled(enabled);
    legendRpeaks->setChecked(enabled);
}

void Ecg2Ch::setLegendQrsOnEnabled(bool enabled) {
    legendQrsOn->setEnabled(enabled);
    legendQrsOn->setChecked(enabled);
}

void Ecg2Ch::setLegendQrsOffEnabled(bool enabled) {
    legendQrsOff->setEnabled(enabled);
    legendQrsOff->setChecked(enabled);
}
void Ecg2Ch::setLegendPOnEnabled(bool enabled) {
    legendPOn->setEnabled(enabled);
    legendPOn->setChecked(enabled);
}
void Ecg2Ch::setLegendPOffEnabled(bool enabled) {
    legendPOff->setEnabled(enabled);
    legendPOff->setChecked(enabled);
}
void Ecg2Ch::setLegendTOffEnabled(bool enabled) {
    legendTOff->setEnabled(enabled);
    legendTOff->setChecked(enabled);
}

void Ecg2Ch::resizeEvent(QResizeEvent *e) {
    control->resize(e);
}




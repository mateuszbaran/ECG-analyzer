#include "ecg2ch.h"

Ecg2Ch::Ecg2Ch(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QToolBar *toolBar = new QToolBar(this);
    QToolButton *testButton = new QToolButton(toolBar);
        testButton->setText("test()");
        toolBar->addWidget(testButton);
    QToolButton *syncButton = new QToolButton(toolBar);
        syncButton->setText("sync");
        syncButton->setCheckable(true);
        toolBar->addWidget(syncButton);
    mainLayout->addWidget(toolBar);

    connect(testButton, SIGNAL(clicked()), SLOT(test()));
    connect(syncButton, SIGNAL(toggled(bool)), SLOT(enableSync(bool)));

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
    zoomer = new SyncZoomer(plot1->canvas(), plot2->canvas());


}

Ecg2Ch::~Ecg2Ch()
{
    delete ch1;
    delete ch2;
}

void Ecg2Ch::setSignal(ECGSignal *signal)
{
    ch1->setSignal(signal->channel_one);
    ch2->setSignal(signal->channel_two);
    zoomer->setZoomBase();
    return;
}

void Ecg2Ch::redraw()
{
    ch1->redraw();
    ch2->redraw();
    return;
}

void Ecg2Ch::enableSync(bool enable)
{
    zoomer->enableSync(enable);
}

/* Funkcja testująca funkcjonalność widżetu.
 * Wywoływana przyciskiem "test()".
 * Wywołuje modealne okienko wyboru pliku.
 * Trzeba wskazać poprawny plik z sygnałem EKG,
 * możliwy do wczytania przez bibliotekę WFDB,
 * czyli jeden z trzech plików: .dat .hea .atr
 * o współnym przedrostku zgodnym z jakimś id wewnątrz.
 * Plik zostanie wczytany i wyświetlone zostaną dane
 * z obu kanałów.
 * Zaznaczenie fragmenu wykresu myszą przybliża go.
 * Można wtedy przewijać wykres środkowym przyciskiem myszy.
 * Oddala się prawym, ew. z wciśniętym klawiszem Ctrl (jest bug).
 */
void Ecg2Ch::test()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    ECGController *controller = new ECGController;
    controller->readFile(fileName.toStdString()); // why?
    setSignal(&(controller->raw_signal));
    redraw();
    delete controller;
}

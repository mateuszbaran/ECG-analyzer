#include "AboutWindow.h"

#ifdef WIN32
 #include "GeneratedFiles/ui_AboutWindow.h"
#else
 #include "ui_AboutWindow.h"
#endif

AboutWindow::AboutWindow(QWidget *parent) :
	QDialog(parent),
		ui(new Ui::AboutWindow)
	{
		ui->setupUi(this);
	}

	AboutWindow::~AboutWindow()
	{
		delete ui;
	}


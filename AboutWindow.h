#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
	class AboutWindow;
}

class AboutWindow : public QDialog
{
	Q_OBJECT

public:
	explicit AboutWindow(QWidget *parent = 0);
	~AboutWindow();

private:
	Ui::AboutWindow *ui;
};

#endif // DIALOG_H
#ifndef DRAGPROXYWINDOW_H
#define DRAGPROXYWINDOW_H

#include <QtWidgets/QDialog>
#include "ui_DragProxyWindow.h"

class DragProxyWindow : public QDialog
{
	Q_OBJECT

public:
	DragProxyWindow(QWidget *parent = 0);
	~DragProxyWindow();

private:
	Ui::DragProxyWindowClass ui;
};

#endif // DRAGPROXYWINDOW_H

#include "DragProxyWindow.h"
#include "DragProxy.h"

DragProxyWindow::DragProxyWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint);  
	setMinimumSize(50, 50);  
	setMaximumSize(800, 800);  

	DragProxy* dragProxy = new DragProxy(this);  
	dragProxy->SetBorderWidth(8, 8, 8, 8);  
}

DragProxyWindow::~DragProxyWindow()
{

}

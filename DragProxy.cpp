#include "DragProxy.h"
#include <QMouseEvent>  
#include <QTimerEvent>  
#include <QCursor>  

DragProxy::DragProxy(QWidget *parent)  
	: QObject((QObject*)parent)  
{  
	m_proxyWidget = parent;  
	m_top = m_right = m_bottom = m_left = 0;  

	m_proxyWidget->setMouseTracking(true);  
	m_proxyWidget->installEventFilter(this); // 代理窗体事件  

	m_mousePressed = false;  
	m_regionPressed = Unknown;  

	m_cursorTimerId = 0;  
}  

DragProxy::~DragProxy()  
{  
}  

void DragProxy::SetBorderWidth(int top, int right, int bottom, int left)  
{  
	m_top = top;  
	m_right = right;  
	m_bottom = bottom;  
	m_left = left;  

	MakeRegions();  
}  

void DragProxy::UpdateGeometry(int x, int y, int w, int h)  
{  
	int minWidth = m_proxyWidget->minimumWidth();  
	int minHeight = m_proxyWidget->minimumHeight();  
	int maxWidth = m_proxyWidget->maximumWidth();  
	int maxHeight = m_proxyWidget->maximumHeight();  

	if (w < minWidth || w > maxWidth || h < minHeight || h > maxHeight)  
	{  
		return;  
	}  

	m_proxyWidget->setGeometry(x, y, w, h);  
}  

bool DragProxy::eventFilter(QObject* obj, QEvent* event)  
{  
	QEvent::Type eventType = event->type();  
	if (eventType == QEvent::MouseMove)  
	{  
		QMouseEvent* mouseEvent = (QMouseEvent*)event;  
		QPoint curPosLocal = mouseEvent->pos();  
		DragProxy::WidgetRegion regionType = HitTest(curPosLocal);  

		QPoint curPosGlobal = m_proxyWidget->mapToGlobal(curPosLocal);  

		if (!m_mousePressed)    // 鼠标未按下  
		{  
			switch (regionType)  
			{  
			case Top:  
			case Bottom:  
				m_proxyWidget->setCursor(Qt::SizeVerCursor);  
				break;  
			case TopRight:  
			case LeftBottom:  
				m_proxyWidget->setCursor(Qt::SizeBDiagCursor);  
				break;  
			case Right:  
			case Left:  
				m_proxyWidget->setCursor(Qt::SizeHorCursor);  
				break;  
			case RightBottom:  
			case LeftTop:  
				m_proxyWidget->setCursor(Qt::SizeFDiagCursor);  
				break;  
			default:  
				m_proxyWidget->setCursor(Qt::ArrowCursor);  
				break;  
			}  

			StartCursorTimer();  
		}  
		else    // 鼠标已按下  
		{  
			QRect geo = m_proxyWidget->geometry();  

			if (m_regionPressed == Inner)     
			{  
				m_proxyWidget->move(m_originGeo.topLeft() + curPosGlobal - m_originPosGlobal);  
			}  
			else if (m_regionPressed == Top)  
			{  
				int dY = curPosGlobal.y() - m_originPosGlobal.y();  
				UpdateGeometry(m_originGeo.x(), m_originGeo.y() + dY, m_originGeo.width(), m_originGeo.height() - dY);  
			}  
			else if (m_regionPressed == TopRight)  
			{  
				QPoint dXY = curPosGlobal - m_originPosGlobal;  
				UpdateGeometry(m_originGeo.x(), m_originGeo.y() + dXY.y(), m_originGeo.width() + dXY.x(), m_originGeo.height() - dXY.y());  
			}  
			else if (m_regionPressed == Right)  
			{  
				int dX = curPosGlobal.x() - m_originPosGlobal.x();  
				UpdateGeometry(m_originGeo.x(), m_originGeo.y(), m_originGeo.width() + dX, m_originGeo.height());  
			}  
			else if (m_regionPressed == RightBottom)  
			{  
				QPoint dXY = curPosGlobal - m_originPosGlobal;  
				UpdateGeometry(m_originGeo.x(), m_originGeo.y(), m_originGeo.width() + dXY.x(), m_originGeo.height() + dXY.y());  
			}  
			else if (m_regionPressed == Bottom)  
			{  
				int dY = curPosGlobal.y() - m_originPosGlobal.y();  
				UpdateGeometry(m_originGeo.x(), m_originGeo.y(), m_originGeo.width(), m_originGeo.height() + dY);  
			}  
			else if (m_regionPressed == LeftBottom)  
			{  
				QPoint dXY = curPosGlobal - m_originPosGlobal;  
				UpdateGeometry(m_originGeo.x() + dXY.x(), m_originGeo.y(), m_originGeo.width() - dXY.x(), m_originGeo.height() + dXY.y());  
			}  
			else if (m_regionPressed == Left)  
			{  
				int dX = curPosGlobal.x() - m_originPosGlobal.x();  
				UpdateGeometry(m_originGeo.x() + dX, m_originGeo.y(), m_originGeo.width() - dX, m_originGeo.height());  
			}  
			else if (m_regionPressed == LeftTop)  
			{  
				QPoint dXY = curPosGlobal - m_originPosGlobal;  
				UpdateGeometry(m_originGeo.x() + dXY.x(), m_originGeo.y() + dXY.y(), m_originGeo.width() - dXY.x(), m_originGeo.height() - dXY.y());  
			}  
		}  
	}  
	else if (eventType == QEvent::MouseButtonPress)  
	{  
		QMouseEvent* mouseEvent = (QMouseEvent*)event;  
		if (mouseEvent->button() == Qt::LeftButton)  
		{  
			m_mousePressed = true;  

			QPoint curPos = mouseEvent->pos();  
			m_regionPressed = HitTest(curPos);  

			m_originPosGlobal = m_proxyWidget->mapToGlobal(curPos);  
			m_originGeo = m_proxyWidget->geometry();  

			StopCursorTimer();  
		}  
	}  
	else if (eventType == QEvent::MouseButtonRelease)  
	{  
		m_mousePressed = false;  
		m_regionPressed = Unknown;  

		m_proxyWidget->setCursor(Qt::ArrowCursor);  
	}  
	else if (eventType == QEvent::Resize)  
	{  
		MakeRegions();  
	}  
	else if (eventType == QEvent::Leave)  
	{  
		m_proxyWidget->setCursor(Qt::ArrowCursor);  
		StopCursorTimer();  
	}  
	else if (eventType == QEvent::Timer)  
	{  
		QTimerEvent* timerEvent = (QTimerEvent*)event;  
		if (timerEvent->timerId() == m_cursorTimerId)  
		{  
			if (m_regions[Inner].contains(m_proxyWidget->mapFromGlobal(QCursor::pos())))  
			{  
				m_proxyWidget->setCursor(Qt::ArrowCursor);  
				StopCursorTimer();  
			}  
		}  
	}  

	return QObject::eventFilter(obj, event);  
}  

void DragProxy::StartCursorTimer()  
{  
	StopCursorTimer();  
	m_cursorTimerId = m_proxyWidget->startTimer(50);  
}  

void DragProxy::StopCursorTimer()  
{  
	if (m_cursorTimerId != 0)  
	{  
		m_proxyWidget->killTimer(m_cursorTimerId);  
		m_cursorTimerId = 0;  
	}  
}  

void DragProxy::MakeRegions()  
{  
	int width = m_proxyWidget->width();  
	int height = m_proxyWidget->height();  

	m_regions[Top]          = QRect(m_left, 0, width - m_left - m_right, m_top);  
	m_regions[TopRight]     = QRect(width - m_right, 0, m_right, m_top);  
	m_regions[Right]        = QRect(width - m_right, m_top, m_right, height - m_top - m_bottom);  
	m_regions[RightBottom]  = QRect(width - m_right, height - m_bottom, m_right, m_bottom);  
	m_regions[Bottom]       = QRect(m_left, height - m_bottom, width - m_left - m_right, m_bottom);  
	m_regions[LeftBottom]   = QRect(0, height - m_bottom, m_left, m_bottom);  
	m_regions[Left]         = QRect(0, m_top, m_left, height - m_top - m_bottom);  
	m_regions[LeftTop]      = QRect(0, 0, m_left, m_top);  
	m_regions[Inner]        = QRect(m_left, m_top, width - m_left - m_right, height - m_top - m_bottom);  
}  

DragProxy::WidgetRegion DragProxy::HitTest(const QPoint& pos)  
{  
	for (int i = 0; i < 9; i++)  
	{  
		const QRect rect = m_regions[i];  
		if (rect.contains(pos))  
		{  
			return DragProxy::WidgetRegion(i);  
		}  
	}  

	return Unknown;  
}  


#ifndef DRAGPROXY_H
#define DRAGPROXY_H

#include <QObject>
#include <QWidget>  
#include <QEvent>  
#include <QRect>  
#include <QPoint>

class DragProxy : public QObject
{
	Q_OBJECT

public:
	DragProxy(QWidget *parent);
	~DragProxy();

protected:  
	enum WidgetRegion  
	{  
		Top = 0,  
		TopRight,  
		Right,  
		RightBottom,  
		Bottom,  
		LeftBottom,  
		Left,  
		LeftTop,  
		Inner,  
		Unknown  
	};  

public:  
	// 设置四周边框宽度  
	void SetBorderWidth(int top, int right, int bottom, int left);  

protected:  
	virtual bool eventFilter(QObject* obj, QEvent* event);  

	void MakeRegions();  
	WidgetRegion HitTest(const QPoint& pos);  
	void UpdateGeometry(int x, int y, int w, int h);  

	// 鼠标从边框快速移到窗体内子控件上，可能会造成鼠标样式未改变，这里使用计时器监控  
	void StartCursorTimer();  
	void StopCursorTimer();  

protected:
	QWidget* m_proxyWidget;     // 代理的窗体  
	int m_top, m_right, m_bottom, m_left;   // 四周宽度  
	QRect m_regions[9];     // 九宫格，对应9个区域  

	QPoint m_originPosGlobal;   // 拖拽前鼠标位置  
	QRect m_originGeo;          // 拖拽前窗体位置和大小  

	bool m_mousePressed;        // 鼠标是否按下  
	WidgetRegion m_regionPressed;   // 记录鼠标按下时所点击的区域  

	int m_cursorTimerId;  
};

#endif // DRAGPROXY_H

#pragma once

#include <QLabel>
#include <QPainter>
#include <opencv2\core.hpp>
#include <vector>
#include "ui_DrawWindow.h"

class DrawWindow : public QLabel
{
	Q_OBJECT

public:
	DrawWindow(QWidget *parent = Q_NULLPTR);
	~DrawWindow();
	std::vector<cv::Mat> getImages() const noexcept;

public slots:
	void clearWindow();

protected:

	void mouseMoveEvent		(QMouseEvent *ev) final;
	void mousePressEvent	(QMouseEvent *ev) final;
	void mouseReleaseEvent	(QMouseEvent *ev) final;
	void paintEvent			(QPaintEvent *ev) final;

private:
	Ui::DrawWindow ui;
	QImage m_defaulTexture;
	QPen m_pen;
	QPainter m_painter;
	QPoint m_prevPoint;
	QPoint m_currentPoint;
};

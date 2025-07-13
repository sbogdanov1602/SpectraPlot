#pragma once
#include "qcustomplot.h"

using namespace Qt;

class Line
{
public: 
	Line();
	void PlotLine(QCustomPlot* plot, QPointF beg, QPointF end, QPen pen = QPen(Qt::black));
	void ClearLine();
	bool Exists();
	QCPItemLine& ItemLine() { return *m_line; }
protected:
	QCPItemLine* m_line;
	QCustomPlot* m_plot;
};

class Cursor : public Line
{
public:
	Cursor();
	void CreateVCursor(QCustomPlot* plot, QPointF beg, QPointF end, QPointF textData);
	void CreateHCursor(QCustomPlot* plot, QPointF beg, QPointF end, QPointF textData);
	void ClearCursor();
protected:
	QCPItemText* m_textLabel;
};



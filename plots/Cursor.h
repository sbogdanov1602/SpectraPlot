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

class BaseCursor;

class Cursor : public Line
{
public:
	Cursor();
	void CreateVCursor(QCustomPlot* plot, QPointF beg, QPointF end, QPointF textData, BaseCursor* baseCursor, QPen pen = QPen(Qt::black));
	void CreateHCursor(QCustomPlot* plot, QPointF beg, QPointF end, QPointF textData, QPen pen = QPen(Qt::black));
	void ClearCursor();
protected:
	QCPItemText* m_textLabel;
};

class BaseCursor : public Cursor
{
public: 
	BaseCursor();
	void Create(QCustomPlot* plot, QCPItemLine* hLine, std::vector<std::vector<double>>* lstSpecData, int iSpecIndx);
	void Clear();
	bool Exists() { return m_bExists; }
	double GetPosition() { return m_dPeakPos; }
	int GetIndex() {return m_iPeakIndx; }
protected:
	int m_iPeakIndx;
	double m_dPeakPos;
	QCustomPlot* m_plot;
	bool m_bExists;
	double m_dStartx_key;
	double m_dEndx_key;

	void ClearData();
};



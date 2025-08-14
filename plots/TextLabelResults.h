#pragma once

class QCPItemText;
class QCustomPlot;
class CalcResult;

class CTextLabelResults
{
public:
	CTextLabelResults();
	void ClearLabelResults();
	void FillLabelResults(QCustomPlot* plot, CalcResult* integral, CalcResult* integral2D);
protected:
	QCPItemText* m_textLabel;
	QCustomPlot* m_plot;
	CalcResult* m_integral;
	CalcResult* m_2Dintegral;
};


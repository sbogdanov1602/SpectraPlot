#include "IntegralSummCalc.h"
#include "qcustomplot.h"
#include "IntegralCalc.h"
#include "CPlotData.h"

CIntegralSummCalc::CIntegralSummCalc(QCPItemLine& vLine, QCPItemLine& hLine, std::vector<std::vector<double>>& lstSpecData)
	: m_LstSpecData(lstSpecData)
	, m_vLine(vLine)
	, m_hLine(hLine)
{
}

double CIntegralSummCalc::Calculate()
{
	double mean = 0.0;
	QCPItemPosition* end = m_vLine.end;
	QCPItemPosition* start = m_vLine.start;
	double start_val = start->value();
	double end_val = end->value();
	int startIdx = (int)(start_val / gPlotData.GetPlotData()->MeasurementStep());
	int endIdx = (int)(end_val / gPlotData.GetPlotData()->MeasurementStep());
	double result = 0.0;
	int k = 0;
	for (int i = startIdx; i <= endIdx; i++) {
		CIntegralCalc integralCalc(m_hLine, m_LstSpecData[i]);
		result = integralCalc.Calculate();
		mean = ((double)k / (k + 1)) * mean + result / (k + 1);
		k++;
	}
	int m = endIdx - startIdx + 1;
	return mean;
}

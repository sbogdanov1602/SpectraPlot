#pragma once
#include <vector>

class QCPItemLine; 

class CIntegralSummCalc
{
public:
	CIntegralSummCalc(QCPItemLine& vLine, QCPItemLine& hLine, std::vector<std::vector<double>>& lstSpecData);
	double Calculate();
protected:
	QCPItemLine& m_vLine;
	QCPItemLine& m_hLine;
	std::vector<std::vector<double>>& m_LstSpecData;
};


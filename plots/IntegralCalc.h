#pragma once
#include <vector>

class QCPItemLine;

class CIntegralCalc
{
public:
	CIntegralCalc(QCPItemLine& pline, std::vector<double>& lstSpecData);
	double Calculate();
protected:
	QCPItemLine& m_Line;
	std::vector<double>& m_LstSpecData;
};


#pragma once
#include <vector>
#include "pd_alg/pd_data.h"
#include "Cursor.h"
#include "common.h"

class QCPItemLine;

class C2DIntegralCalc
{

public:
	C2DIntegralCalc(QCPItemLine& vLine, QCPItemLine& hLine
		, std::vector<std::vector<double>>& lstSpecData
		, std::vector<std::vector<double>>& lstSpecDataT
		, Cursor* vCursor = nullptr
		, Cursor* hCursor = nullptr);

	int2Dresult Calculate();
protected:
	QCPItemLine& m_vLine;
	QCPItemLine& m_hLine;
	int m_startx_idx;
	int m_endx_idx;
	int m_startIdx;
	int m_endIdx;

	int m_leftX;
	int m_rightX;
	int m_leftY;
	int m_rightY;

	std::vector<std::vector<double>>& m_LstSpecData;
	std::vector<std::vector<double>>& m_LstSpecDataT;
	Cursor* m_vCursor;
	Cursor* m_hCursor;
	void CalculateVInterval(int& leftV, int& rightV, int& avgNoise);
	void CalculateHInterval(int& leftH, int& rightH, int& avgNoise);
	void CalculateMaxPeakInterval(std::vector<TPeak>& peaks, int& left, int& right);
};


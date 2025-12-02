#pragma once
#include <vector>
#include "pd_alg/pd_data.h"
#include "Cursor.h"
#include "common.h"

class QCPItemLine;

class C2DIntegralCalc
{

public:

	struct max_peak
	{
		int max_peakHeight;
		int max_point_idx;
		int max_spectr_idx;
		int max_left;
		int max_right;

		max_peak() :
			max_peakHeight(0)
			,max_point_idx(0)
			,max_spectr_idx(0)
			,max_left(0)
			,max_right(0)
		{
		}
	};

	C2DIntegralCalc(QCPItemLine& vLine, QCPItemLine& hLine
		, std::vector<std::vector<double>>& lstSpecData
		, std::vector<std::vector<double>>& lstSpecDataT
		, Cursor* vCursor = nullptr
		, Cursor* hCursor = nullptr);

	Result Calculate();
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
	void CalculateVInterval(int point_idx, int& leftV, int& rightV, int& avgNoise);
	max_peak CalculateHInterval(int& leftH, int& rightH, int& avgNoise);
	void CalculateMaxPeakInterval(std::vector<TPeak>& peaks, int& left, int& right, int& max_height, int& max_height_idx);
};


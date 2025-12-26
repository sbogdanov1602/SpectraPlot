#include "C2DIntegralCalc.h"
#include "qcustomplot.h"
#include "CPlotData.h"
#include "Settings.h"
#include "pd_alg/pd_alg.h"

C2DIntegralCalc::C2DIntegralCalc(QCPItemLine& vLine, QCPItemLine& hLine
	, std::vector<std::vector<double>>& lstSpecData
	, std::vector<std::vector<double>>& lstSpecDataT
	, Cursor* vCursor /* = nullptr*/
	, Cursor* hCursor /* = nullptr*/)
	: m_LstSpecData(lstSpecData)
	, m_LstSpecDataT(lstSpecDataT)
	, m_vLine(vLine)
	, m_hLine(hLine)
	, m_startx_idx(0)
	, m_endx_idx(0)
	, m_startIdx(0)
	, m_endIdx(0)
	, m_vCursor(vCursor)
	, m_hCursor(hCursor)
	, m_maxIterationNum(gSettings.GetMaxIterationNum())
	, m_changeCoeff(gSettings.GetChangeCoeff())

{
}

void C2DIntegralCalc::PrepareIntervals(int iterationNum, double stepH, double stepV)
{
	QCPItemPosition* end = m_vLine.end;
	QCPItemPosition* start = m_vLine.start;
	double start_val = start->value();
	double end_val = end->value();

	QCPItemPosition* endx = m_hLine.end;
	QCPItemPosition* startx = m_hLine.start;
	double startx_key = startx->key();
	double endx_key = endx->key();

	if (iterationNum > 0) {
		double dh = (endx_key - startx_key) * iterationNum * m_changeCoeff;
		startx_key -= dh;
		if (startx_key < 0.0) {
			startx_key = 0.0;
		}
		endx_key += dh;
	}

	m_leftX = m_startx_idx = (int)(startx_key / stepH);
	m_rightX = m_endx_idx = (int)(endx_key / stepH);
	m_leftY = m_startIdx = (int)(start_val / stepV);
	m_rightY = m_endIdx = (int)(end_val / stepV);
}

Result C2DIntegralCalc::Calculate()
{
	Result ret;
	double stepV = gPlotData.GetPlotData()->MeasurementStep();
	double stepH = gPlotData.GetPlotData()->PointScale() / gPlotData.GetPlotData()->PointsNum();

	int iteration = 0;
	bool peaksExist = false;
	
	while (!peaksExist && (iteration <= m_maxIterationNum)) {
		PrepareIntervals(iteration, stepH, stepV);
		int avgNoiseH = 0;
		auto max_peak = CalculateHInterval(peaksExist, avgNoiseH);
		if (peaksExist) {
			int leftV = m_leftY, rightV = m_rightY, avgNoiseV = 0;
			CalculateVInterval(max_peak.max_point_idx, leftV, rightV, avgNoiseV);

			double avgNoise = (avgNoiseH + avgNoiseV) * 0.5 * gPlotData.GetPlotData()->SignalCoeff();

			int leftH = max_peak.max_left;
			int rightH = max_peak.max_right;

			double beg_h = leftH * stepH;
			double end_h = rightH * stepH;
			double beg_v = leftV * stepV;
			double end_v = rightV * stepV;

			double result = 0.0;
			for (int i = leftV; i <= rightV; i++) {
				for (int j = leftH; j < rightH; j++) {
					double addVal = (m_LstSpecData[i][j] + m_LstSpecData[i + 1][j] + m_LstSpecData[i][j + 1] + m_LstSpecData[i + 1][j + 1] - 4.0 * avgNoise) / 4.0 * stepH * stepV;
					// - 4.0 * avgNoise        - the noise part of integral
					result += addVal > 0.0 ? addVal : 0.0;
				}
			}
			ret.piakHight = max_peak.max_peakHeight * gPlotData.GetPlotData()->SignalCoeff();
			ret.peakPosX = max_peak.max_point_idx * stepH;
			ret.peakPosY = max_peak.max_spectr_idx * stepV;
			ret.value = result;
			ret.leftX = beg_h;
			ret.rightX = end_h;
			ret.leftY = beg_v;
			ret.rightY = end_v;
		}
		iteration++;
	}
	ret.time = QTime::currentTime().toString();
	ret.date = QDate::currentDate().toString();
	ret.description = VALUE_COLUMN_NAME;
	return ret;
}

C2DIntegralCalc::max_peak C2DIntegralCalc::CalculateHInterval(bool& peaksExist, int& avgNoise)
{
	auto algParams = TPDAlgParams();
	algParams.bAutoNoise = true;
	algParams.bAllData = false;
	algParams.iLeftLim = m_leftX;
	algParams.iRightLim = m_rightX;
	//algParams.iMaxPeakCount = 1;
	algParams.iNoiseRate = gSettings.GetNoiseRate();

	size_t count = m_LstSpecData.at(0).size();

	max_peak max_peak_data;
	peaksExist = false;
	for (int i = m_startIdx; i < m_endIdx; i++) {
		int* pData = new int[count];
		for (int k = 0; k < count; k++) {
			pData[k] = (int)(m_LstSpecData[i][k] / gPlotData.GetPlotData()->SignalCoeff());
		}
		auto vecPeaks = PDAlg::PeaksDetecting(pData, count, 1.0, &algParams);
		delete pData;
		bool b = (vecPeaks.size() > 0);
		if (b) {
			avgNoise = algParams.iNoiseLevel / gSettings.GetNoiseRate();
			int max_height = 0, max_height_idx = 0;
			int _leftH = 0, _rightH = 0;
			CalculateMaxPeakInterval(vecPeaks, _leftH, _rightH, max_height, max_height_idx);
			if (max_height > max_peak_data.max_peakHeight) {
				max_peak_data.max_peakHeight = max_height;
				max_peak_data.max_point_idx = max_height_idx;
				max_peak_data.max_spectr_idx = i;
				max_peak_data.max_left = _leftH;
				max_peak_data.max_right = _rightH;
			}
		}
		peaksExist = peaksExist || b;
	}
	return max_peak_data;
}

void C2DIntegralCalc::CalculateMaxPeakInterval(std::vector<TPeak>& vecPeaks, int& left, int& right, int& max_height, int& max_height_idx)
{
	if (vecPeaks.size() > 0) {
		int max = 0, max_idx = 0, idx = 0;
		for (auto it = vecPeaks.begin(); it != vecPeaks.end(); it++) {
			if (it->Height > max) {
				max = it->Height;
				max_idx = idx;
			}
			idx++;
		}
		max_height = max;
		max_height_idx = vecPeaks[max_idx].Extrem;
		left = vecPeaks[max_idx].LeftBound;
		right = vecPeaks[max_idx].RightBound;
	}
	else {
		int k = 0;
	}
}

void C2DIntegralCalc::CalculateVInterval(int point_idx, int& leftV, int& rightV, int& avgNoise)
{
	auto algParams = TPDAlgParams();
	algParams.bAutoNoise = true;
	algParams.bAllData = false;
	algParams.iLeftLim = m_leftY;
	algParams.iRightLim = m_rightY;
	//algParams.iMaxPeakCount = 1;
	algParams.iNoiseRate = gSettings.GetNoiseRate();

	size_t spCount = m_LstSpecDataT.at(point_idx).size();

	int* pData = new int[spCount];
	for (int j = 0; j < spCount; j++) {
		pData[j] = (int)(m_LstSpecDataT[point_idx][j] / gPlotData.GetPlotData()->SignalCoeff());
	}
	auto vecPeaks = PDAlg::PeaksDetecting(pData, spCount, 1.0, &algParams);
	delete pData;
	avgNoise = algParams.iNoiseLevel / gSettings.GetNoiseRate();
	int max_height = 0, max_height_idx = 0;
	CalculateMaxPeakInterval(vecPeaks, leftV, rightV, max_height, max_height_idx);
}

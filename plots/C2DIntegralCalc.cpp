#include "C2DIntegralCalc.h"
#include "qcustomplot.h"
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
{
}

Result C2DIntegralCalc::Calculate()
{
	QCPItemPosition* end = m_vLine.end;
	QCPItemPosition* start = m_vLine.start;
	double start_val = start->value();
	double end_val = end->value();
	double stepV = gSettings.GetMeasurementStep();
	m_leftY = m_startIdx = (int)(start_val / stepV);
	m_rightY = m_endIdx = (int)(end_val / stepV);

	QCPItemPosition* endx = m_hLine.end;
	QCPItemPosition* startx = m_hLine.start;
	double startx_key = startx->key();
	double endx_key = endx->key();
	double stepH = gSettings.GetPointScale() / gSettings.GetPointsNum();
	m_leftX = m_startx_idx = (int)(startx_key / stepH);
	m_rightX = m_endx_idx = (int)(endx_key / stepH);

	size_t spCount = m_LstSpecDataT.at(0).size();	

//	CalculateVInterval(leftV, rightV, avgNoiseV);

	size_t count = m_LstSpecData.at(0).size();
	int leftH = count, rightH = 0, avgNoiseH = 0; 
	auto max_peak = CalculateHInterval(leftH, rightH, avgNoiseH);

	int leftV = m_leftY, rightV = m_rightY, avgNoiseV = 0;
	CalculateVInterval(max_peak.max_point_idx, leftV, rightV, avgNoiseV);

	double avgNoise = (avgNoiseH + avgNoiseV) * 0.5 * gSettings.GetSignalCoeff();

	leftH = max_peak.max_left;
	rightH = max_peak.max_right;

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
	Result ret;
	ret.piakHight = max_peak.max_peakHeight * gSettings.GetSignalCoeff();
	ret.peakPosX = max_peak.max_point_idx * stepH;
	ret.peakPosY = max_peak.max_spectr_idx * stepV;
	ret.value = result;
	ret.leftX = beg_h;
	ret.rightX = end_h;
	ret.leftY = beg_v;
	ret.rightY = end_v;
	ret.time = QTime::currentTime().toString();
	ret.date = QDate::currentDate().toString();
	ret.description = VALUE_COLUMN_NAME;
	return ret;
}

C2DIntegralCalc::max_peak C2DIntegralCalc::CalculateHInterval(int& leftH, int& rightH, int& avgNoise)
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
	for (int i = m_startIdx; i < m_endIdx; i++) {
		int* pData = new int[count];
		for (int k = 0; k < count; k++) {
			pData[k] = (int)(m_LstSpecData[i][k] / gSettings.GetSignalCoeff());
		}
		auto vecPeaks = PDAlg::PeaksDetecting(pData, count, 1.0, &algParams);
		delete pData;
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
		pData[j] = (int)(m_LstSpecDataT[point_idx][j] / gSettings.GetSignalCoeff());
	}
	auto vecPeaks = PDAlg::PeaksDetecting(pData, spCount, 1.0, &algParams);
	delete pData;
	avgNoise = algParams.iNoiseLevel / gSettings.GetNoiseRate();
	int max_height = 0, max_height_idx = 0;
	CalculateMaxPeakInterval(vecPeaks, leftV, rightV, max_height, max_height_idx);
}
/*
void C2DIntegralCalc::CalculateVInterval_Old(int& leftV, int& rightV, int& avgNoise)
{
	auto algParams = TPDAlgParams();
	algParams.bAutoNoise = true;
	algParams.bAllData = false;
	algParams.iLeftLim = m_leftY;
	algParams.iRightLim = m_rightY;
	//algParams.iMaxPeakCount = 1;
	algParams.iNoiseRate = gSettings.GetNoiseRate();

	size_t spCount = m_LstSpecDataT.at(0).size();

	for (int i = m_startx_idx; i < m_endx_idx; i++) {
		int* pData = new int[spCount];
		for (int j = 0; j < spCount; j++) {
			pData[j] = (int)(m_LstSpecDataT[i][j] / gSettings.GetSignalCoeff());
		}
		auto vecPeaks = PDAlg::PeaksDetecting(pData, spCount, 1.0, &algParams);
		delete pData;
		avgNoise = algParams.iNoiseLevel / gSettings.GetNoiseRate();
		int max_height = 0, max_height_idx = 0;
		CalculateMaxPeakInterval(vecPeaks, leftV, rightV, max_height, max_height_idx);
	}
}

Result C2DIntegralCalc::Calculate_Old()
{
	QCPItemPosition* end = m_vLine.end;
	QCPItemPosition* start = m_vLine.start;
	double start_val = start->value();
	double end_val = end->value();
	double stepV = gSettings.GetMeasurementStep();
	m_leftY = m_startIdx = (int)(start_val / stepV);
	m_rightY = m_endIdx = (int)(end_val / stepV);
	QCPItemPosition* endx = m_hLine.end;
	QCPItemPosition* startx = m_hLine.start;
	double startx_key = startx->key();
	double endx_key = endx->key();
	double stepH = gSettings.GetPointScale() / gSettings.GetPointsNum();
	m_leftX = m_startx_idx = (int)(startx_key / stepH);
	m_rightX = m_endx_idx = (int)(endx_key / stepH);
	size_t spCount = m_LstSpecDataT.at(0).size();
	int leftV = spCount, rightV = m_rightY, avgNoiseV = 0;
	//sb dbg CalculateVInterval(leftV, rightV, avgNoiseV);

	size_t count = m_LstSpecData.at(0).size();
	int leftH = count, rightH = 0, avgNoiseH = 0;
	auto max_peak = CalculateHInterval(leftH, rightH, avgNoiseH);

	double avgNoise = (avgNoiseH + avgNoiseV) * 0.5 * gSettings.GetSignalCoeff();

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
	Result ret;
	ret.piakHight = max_peak.max_peakHeight * gSettings.GetSignalCoeff();
	ret.peakPosX = max_peak.max_point_idx * stepH;
	ret.peakPosY = max_peak.max_spectr_idx * stepV;
	ret.value = result;
	ret.leftX = beg_h;
	ret.rightX = end_h;
	ret.leftY = beg_v;
	ret.rightY = end_v;
	ret.time = QTime::currentTime().toString();
	ret.date = QDate::currentDate().toString();
	ret.description = VALUE_COLUMN_NAME;
	return ret;
}
*/


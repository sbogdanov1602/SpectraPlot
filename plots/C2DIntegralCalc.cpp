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

int2Dresult C2DIntegralCalc::Calculate()
{
	QCPItemPosition* end = m_vLine.end;
	QCPItemPosition* start = m_vLine.start;
	double start_val = start->value();
	double end_val = end->value();
	double stepV = gSettings.GetMeasurementStep();
	m_leftY = m_startIdx = (int)(start_val / stepV);
	m_rightY = m_endIdx = (int)(end_val / stepV);
/*
	if (m_hCursor != nullptr) {
		start_val = m_hCursor->ItemLine().start->value();
		m_startIdx = (int)(start_val / stepV);
		m_endIdx = m_startIdx + 1;
		int m = 0;
	}
*/
	QCPItemPosition* endx = m_hLine.end;
	QCPItemPosition* startx = m_hLine.start;
	double startx_key = startx->key();
	double endx_key = endx->key();
	double stepH = gSettings.GetPointScale() / gSettings.GetPointsNum();
	m_leftX = m_startx_idx = (int)(startx_key / stepH);
	m_rightX = m_endx_idx = (int)(endx_key / stepH);
/*
	if (m_vCursor != nullptr) {
		startx_key = m_vCursor->ItemLine().start->key();
		m_startx_idx = (int)(startx_key / stepH);
		m_endx_idx = m_startx_idx + 1;
		int m = 0;
	}
*/
	size_t spCount = m_LstSpecDataT.at(0).size();	
	int leftV = spCount, rightV = m_rightY, avgNoiseV = 0;
	CalculateVInterval(leftV, rightV, avgNoiseV);

	size_t count = m_LstSpecData.at(0).size();
	int leftH = count, rightH = 0, avgNoiseH = 0; 
	CalculateHInterval(leftH, rightH, avgNoiseH);

	double avgNoise = (avgNoiseH + avgNoiseV) * 0.5 * gSettings.GetSignalCoeff();

	double beg_h = leftH * stepH;
	double end_h = rightH * stepH;
	double beg_v = leftV * stepV;
	double end_v = rightV * stepV;

	double result = 0.0;
	for (int i = leftV; i <= rightV; i++) {
		for (int j = leftH; j < rightH; j++) {
			result += (m_LstSpecData[i][j] + m_LstSpecData[i + 1][j] + m_LstSpecData[i][j + 1] + m_LstSpecData[i + 1][j + 1] /* - 4.0 * avgNoise*/) / 4.0 * stepH * stepV;
			// - 4.0 * avgNoise        - the noise part of integral
		}
	}
	int2Dresult ret;
	ret.value = result;
	ret.leftX = beg_h;
	ret.rightX = end_h;
	ret.leftY = beg_v;
	ret.rightY = end_v;
	ret.time = QTime::currentTime().toString();
	ret.date = QDate::currentDate().toString();
	return ret;
}

void C2DIntegralCalc::CalculateVInterval(int& leftV, int& rightV, int& avgNoise)
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
		CalculateMaxPeakInterval(vecPeaks, leftV, rightV);
	}
}

void C2DIntegralCalc::CalculateHInterval(int& leftH, int& rightH, int& avgNoise)
{
	auto algParams = TPDAlgParams();
	algParams.bAutoNoise = true;
	algParams.bAllData = false;
	algParams.iLeftLim = m_leftX;
	algParams.iRightLim = m_rightX;
	//algParams.iMaxPeakCount = 1;
	algParams.iNoiseRate = gSettings.GetNoiseRate();
	
	size_t count = m_LstSpecData.at(0).size();

	for (int i = m_startIdx; i < m_endIdx; i++) {
		int* pData = new int[count];
		for (int k = 0; k < count; k++) {
			pData[k] = (int)(m_LstSpecData[i][k] / gSettings.GetSignalCoeff());
		}
		auto vecPeaks = PDAlg::PeaksDetecting(pData, count, 1.0, &algParams);
		delete pData;
		avgNoise = algParams.iNoiseLevel / gSettings.GetNoiseRate();
		CalculateMaxPeakInterval(vecPeaks, leftH, rightH);
	}
}

void C2DIntegralCalc::CalculateMaxPeakInterval(std::vector<TPeak>& vecPeaks, int& left, int& right)
{
	int beg_idx2 = 0;
	int end_idx2 = 0;

	if (vecPeaks.size() > 0) {
		int max = 0, max_idx = 0, idx = 0;
		for (auto it = vecPeaks.begin(); it != vecPeaks.end(); it++) {
			if (it->Height > max) {
				max = it->Height;
				max_idx = idx;
			}
			idx++;
		}
		beg_idx2 = (int)vecPeaks[max_idx].LeftBound;
		end_idx2 = (int)vecPeaks[max_idx].RightBound;
		if (beg_idx2 < left) {
			left = beg_idx2;
		}
		if (right < end_idx2) {
			right = end_idx2;
		}
	}
}



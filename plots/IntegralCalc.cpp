#include "qcustomplot.h"
#include "IntegralCalc.h"
#include "Settings.h"
#include "pd_alg/pd_alg.h"
#include "pd_alg/pd_data.h"

CIntegralCalc::CIntegralCalc(QCPItemLine& pline, std::vector<double>& lstSpecData) 
	: m_LstSpecData(lstSpecData)
	, m_Line(pline)
{
	
}

double CIntegralCalc::Calculate()
{
	double result = 0.0;
	double result1 = 0.0;
	double beg_v = 0.0;
	double end_v = 0.0;
	double value = 0.0;
	int beg_idx2 = 0;
	int end_idx2 = 0;
	double half = 0.0;

	QCPItemPosition* end = m_Line.end;
	QCPItemPosition* start = m_Line.start;
	double start_key = start->key();
	double end_key = end->key();
	double step = gSettings.GetPointScale() / gSettings.GetPointsNum();
	int start_idx = (int)(start_key / step);
	int end_idx = (int)(end_key / step);

	auto algParams = TPDAlgParams();
	algParams.bAutoNoise = true;
	algParams.bAllData = false;
	algParams.iLeftLim = start_idx;
	algParams.iRightLim = end_idx;
	algParams.iMaxPeakCount = 1;
	algParams.iNoiseRate = gSettings.GetNoiseRate();

	size_t count = m_LstSpecData.size();
	int* pData = new int[count];
	for (int k = 0; k < count; k++) {
		pData[k] = (int)(m_LstSpecData[k] / gSettings.GetSignalCoeff());
	}
	auto vecPeaks = PDAlg::PeaksDetecting(pData, count, 1.0, &algParams);
	delete pData;
	if (vecPeaks.size() > 0) {
		int max = 0, max_idx = 0, idx = 0;
		for (auto it = vecPeaks.begin(); it != vecPeaks.end(); it++) {
			if (it->Value > max) {
				max = it->Value;
				max_idx = idx;
			}
			idx++;
		}
		half = 0.0;
		beg_idx2 = (int)vecPeaks[max_idx].LeftBound;
		end_idx2 = (int)vecPeaks[max_idx].RightBound;
		beg_v = beg_idx2 * step;
		end_v = end_idx2 * step;
		value = vecPeaks[max_idx].Value * gSettings.GetSignalCoeff() * step;
		for (int i = beg_idx2; i < end_idx2; i++) {
			half = (m_LstSpecData[i] + m_LstSpecData[i + 1]) * 0.5;
			result1 += half * step;
		}
	}
	return result1;
}
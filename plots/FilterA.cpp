#include "FilterA.h"
#include "Settings.h"

FilterA::FilterA(const std::vector<double>& data)
	: m_data(data)
	, m_mean(0.0)
	, m_sigma(0.0)
	, m_beginIdx(0)
	, m_meanCnt(100)
	, m_seriaCnt(50)
	, m_epsilon(1)
{
}

std::vector<double> FilterA::Calculate()
{
	FindBaseSeria();
	CalculateMeanAndSigma(m_mean, m_sigma, m_beginIdx, m_meanCnt);
	SetDataBaseLevel(m_beginIdx - 2, m_meanCnt + 2);

	double mean = 0.0;
	double sigma = 0.0;
	int startIdx = m_beginIdx + m_meanCnt;

	while (startIdx < m_data.size()) {
		CalculateMeanAndSigma(mean, sigma, startIdx, m_seriaCnt);

		bool b1 = (2.0 * abs(m_mean - mean) / abs(mean + m_mean)) < m_epsilon;
		bool b2 = true;// (2.0 * abs(m_sigma - sigma) / abs(sigma + m_sigma)) < m_epsilon;
		
		if (b1 && b2) {
			SetDataBaseLevel(startIdx, m_seriaCnt);
		}
		startIdx += m_seriaCnt;
	}

	return m_data;
}

void FilterA::FindBaseSeria() {
	int i = 0;
	bool b = false;
	bool b1 = false;
	for (; i < m_data.size() - 2; i++) {
		b = abs(m_data[i] - m_data[i + 1]) > std::numeric_limits<double>::epsilon();
		b1 = abs(m_data[i + 1] - m_data[i + 2]) > std::numeric_limits<double>::epsilon();
		if (b && b1) {
			break;
		}
	}
	if (b && b1) {
		m_beginIdx = i + 2;
	}
}

void FilterA::CalculateMeanAndSigma(double& mean, double& sigma, int beginIdx, int meanCnt)
{
	mean = 0.0;
	sigma = 0.0;
	int n = 0;
	for (int j = beginIdx; j < (meanCnt - 1 + beginIdx) && j < m_data.size(); j++) {
		mean = (n * 1.0) / (n + 1) * mean + m_data[j] / (n + 1);
		n++;
	}
	n = 0;
	for (int j = beginIdx; j < (meanCnt - 1 + beginIdx) && j < m_data.size(); j++) {
		sigma = (n * 1.0) / (n + 1) * sigma + (m_data[j] - mean) * (m_data[j] - mean) / (n + 1);
		n++;
	}
	sigma = sqrt(sigma);
}

void FilterA::SetDataBaseLevel(int beginIdx, int meanCnt)
{
	for (int j = beginIdx; j < (meanCnt - 1 + beginIdx) && j < m_data.size(); j++) {
		m_data[j] -= m_mean + 3 * m_sigma;
		if (m_data[j] < 0.0)
		{
			m_data[j] = 0.0;
		}
	}
}

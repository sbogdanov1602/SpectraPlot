#pragma once
#include <vector>


class FilterA
{
public:
	FilterA(const std::vector<double>& data);
	std::vector<double> Calculate();
	void CalculateMeanAndSigma(double& mean, double& sigma, int beginIdx, int meanCnt);
	void FindBaseSeria();
	void SetDataBaseLevel(int beginIdx, int meanCnt);
	double GetMean() const { return m_mean; }
	double GetSigma() const { return m_sigma; }
protected:
	double m_mean;
	double m_sigma;
	int m_beginIdx;
	int m_meanCnt;
	int m_seriaCnt;
	double m_epsilon;
	std::vector<double> m_data;
};


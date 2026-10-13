#pragma once
#include <qmetatype.h>

class CSettings
{
public:
	CSettings();
	~CSettings();
	bool IsUseSpectrionData() {	return m_bUseSpectrionData;	}
	QString SpectrionDir() { return m_sSpectrionDir; }
	int GetNoiseRate() { return m_NoiseRate; }
	void SetNoiseRate(int value) { m_NoiseRate = value; }

	int GetMaxIterationNum() { return m_MaxIterationNum; }
	void SetMaxIterationNum(int value) { m_MaxIterationNum = value; }

	double GetChangeCoeff() { return m_ChangeCoeff; }
	void SetChangeCoeff(double value) { m_ChangeCoeff = value; }

	void Load();
protected:
	bool m_bUseSpectrionData;
	QString m_sSpectrionDir;
	int m_NoiseRate;
	int m_MaxIterationNum;
	double m_ChangeCoeff;
private:
	QString m_FileName;
};

extern CSettings gSettings;


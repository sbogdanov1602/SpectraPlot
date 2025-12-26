#pragma once
#include <qmetatype.h>

class CSettings
{
public:
	CSettings();
	~CSettings();
	/*
	float GetPointScale() { return m_PointScale; }
	void  SetPointScale(float value) { m_PointScale = value; }

	float GetMeasurementStep() { return m_MeasurementStep; }
	void  SetMeasurementStep(float value) { m_MeasurementStep = value; }
	
	double GetSignalCoeff() { return m_SignalCoeff; }
	void SetSignalCoeff(double value) { m_SignalCoeff = value; }

	int GetPointsNum() { return m_PointsNum; }
	void SetPointaNum(int value) { m_PointsNum = value; }
	*/

	int GetNoiseRate() { return m_NoiseRate; }
	void SetNoiseRate(int value) { m_NoiseRate = value; }

	int GetMaxIterationNum() { return m_MaxIterationNum; }
	void SetMaxIterationNum(int value) { m_MaxIterationNum = value; }

	double GetChangeCoeff() { return m_ChangeCoeff; }
	void SetChangeCoeff(double value) { m_ChangeCoeff = value; }

	void Load();
protected:
	//float m_PointScale; //ms
	//float m_MeasurementStep; //sec
	//double m_SignalCoeff; // to V
//	int    m_PointsNum;
	int m_NoiseRate;
	int m_MaxIterationNum;
	double m_ChangeCoeff;

private:
	QString m_FileName;
};

extern CSettings gSettings;


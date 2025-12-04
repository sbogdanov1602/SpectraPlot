#pragma once
#include "CommonLib.h"

class CchData : public IPlotData
{
public:
	CchData();
	// Inherited via IPlotData
	float PointScale() override;
	float MeasurementStep() override;
	double SignalCoeff() override;
	int Load(std::string filePath, std::function<void(int)> setProgressDlgValue, std::function<void(int)>  setMaximum, bool loadOneFileOnly) override;
	std::string MapXAxisLabel() override;
	std::string MapYAxisLabel() override;
	std::string SignalAxisLabel() override;
protected:
	int m_iMeaningAreaBeg;
	int m_iMeaningAreaLength;
	float m_fPointScale;
	float m_fMeasurementStep;
	double m_dSignalCoeff;
	std::string m_sMapXAxisLabel;
	std::string m_sMapYAxisLabel;
	std::string m_sSignalAxisLabel;

	void LoadSettings();
};


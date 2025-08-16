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
};


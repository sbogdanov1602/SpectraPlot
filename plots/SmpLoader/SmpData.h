#pragma once
#include "CommonLib.h"

class SmpData : public IPlotData
{
public:
	SmpData();
	int Load(std::string filePath, std::function<void(int)>  setProgressDlgValue, std::function<void(int)>  setMaximum, bool loadOneFileOnly) override;
	// Inherited via IPlotData
	float PointScale() override;
	float MeasurementStep() override;
	double SignalCoeff() override;
	std::string MapXAxisLabel() override;
	std::string MapYAxisLabel() override;
	std::string SignalAxisLabel() override;
};


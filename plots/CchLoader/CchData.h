#pragma once
#include "CommonLib.h"

class CchData : public IPlotData
{
public:
	CchData();
	// Inherited via IPlotData
	std::vector<std::vector<double>>& LstSpecData() override;
	std::vector<std::vector<double>>& LstSpecDataT() override;
	float PointScale() override;
	float MeasurementStep() override;
	double SignalCoeff() override;
	int Load(std::string filePath, std::function<void(int)> setProgressDlgValue, std::function<void(int)>  setMaximum, bool loadOneFileOnly) override;
	std::string MapXAxisLabel() override;
	std::string MapYAxisLabel() override;
	std::string HXAxisLabel() override;
	std::string HYAxisLabel() override;
	std::string VXAxisLabel() override;
	std::string VYAxisLabel() override;
};


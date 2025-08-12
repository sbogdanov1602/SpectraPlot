#pragma once
#include "CommonLib.h"
#include "smploader_global.h"

class SMPLOADER_EXPORT SmpData : public IPlotData
{
public:
	SmpData();
	int Load(std::string filePath, std::function<void(int)>  setProgressDlgValue/*void(*setProgressDlgValue)(int)*/, void (*closeProgressDlg)()) override;
	// Inherited via IPlotData
	std::vector<std::vector<double>>& LstSpecData() override;
	std::vector<std::vector<double>>& LstSpecDataT() override;
	float PointScale() override;
	float MeasurementStep() override;
	double SignalCoeff() override;
	std::string MapXAxisLabel() override;
	std::string MapYAxisLabel() override;
	std::string HXAxisLabel() override;
	std::string HYAxisLabel() override;
	std::string VXAxisLabel() override;
	std::string VYAxisLabel() override;
};


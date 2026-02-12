#pragma once
#include "CommonLib/CommonLib.h"

class CPlotData
{
public:
	CPlotData();
	~CPlotData();
	IPlotData* GetPlotData() { return m_pPlotData; }
	void SetPlotData(IPlotData* pPlotData);
private:
	IPlotData* m_pPlotData;
};

extern CPlotData gPlotData;


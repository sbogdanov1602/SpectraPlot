#include "CPlotData.h"


CPlotData::CPlotData()
: m_pPlotData(nullptr)
{
}

CPlotData::	~CPlotData() 
{
	if (m_pPlotData != nullptr) {
//		delete m_pPlotData;
		m_pPlotData = nullptr;
	}
}

void CPlotData::SetPlotData(IPlotData* pPlotData)
{
	if (m_pPlotData != nullptr) {
//		delete m_pPlotData;
		m_pPlotData = nullptr;
	}
	m_pPlotData = pPlotData;
}

CPlotData gPlotData;


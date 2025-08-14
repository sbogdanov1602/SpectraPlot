// CommonLib.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "CommonLib.h"


// This is an example of an exported variable
COMMONLIB_API int nCommonLib=0;

// This is an example of an exported function.
COMMONLIB_API int fnCommonLib(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CCommonLib::CCommonLib()
{
    return;
}

IPlotData::IPlotData()
{
    m_maxSignal = 0.0;
    m_loadIsCanceled = false;
    size_t s1 = m_lstSpecData.size();
    size_t s2 = m_lstSpecDataT.size();
}

void IPlotData::ClearData()
{
    for (int i = 0; i < m_lstSpecData.size(); i++) {
        auto vec = m_lstSpecData[i];
        vec.clear();
    }
    m_lstSpecData.clear();

    for (int i = 0; i < m_lstSpecDataT.size(); i++) {
        auto vec = m_lstSpecDataT[i];
        vec.clear();
    }
    m_lstSpecDataT.clear();
}

int IPlotData::PointsNum()
{
    size_t s = m_lstSpecData.size();
    return (s > 0) ? m_lstSpecData[0].size() : 0; 
}

int IPlotData::SpectraNum() 
{
    size_t s = m_lstSpecDataT.size();
    return (s > 0) ? m_lstSpecDataT[0].size() : 0;
}



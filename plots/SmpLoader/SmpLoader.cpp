#include "SmpLoader.h"
#include "CommonLib.h"
#include "SmpData.h"

SmpLoader::SmpLoader()
{
    m_pPlotData = nullptr;
}

SmpLoader::~SmpLoader()
{
    if (m_pPlotData != nullptr) {
        delete m_pPlotData;
        m_pPlotData = nullptr;
    }
}

IPlotData* SmpLoader::PlotData()
{
    if (m_pPlotData != nullptr) {
        m_pPlotData = new SmpData();

    }
    return m_pPlotData;
}

namespace smp 
{
    IPlotData* GetIPlotData(void)
    {
        return (IPlotData*)(new SmpData());
    }

}

SmpLoader SmpLoaderInstance;


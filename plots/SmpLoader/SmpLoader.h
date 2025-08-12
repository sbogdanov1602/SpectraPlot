#pragma once

#include "smploader_global.h"
class IPlotData;

class SmpLoader
{
public:
    SmpLoader();
    ~SmpLoader();
    IPlotData* PlotData();
private:
    IPlotData* m_pPlotData;
};

extern SmpLoader SmpLoaderInstance;

namespace smp
{
    SMPLOADER_EXPORT IPlotData* GetIPlotData(void);
}


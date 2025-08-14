#pragma once
#include "smploader_global.h"
#include "SmpData.h"

class IPlotData;

extern SmpData SmpDataInstance;

namespace smp
{
    SMPLOADER_EXPORT IPlotData* GetIPlotData(void);
}


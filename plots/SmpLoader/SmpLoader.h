#pragma once
#include "smploader_global.h"
#include "SmpData.h"

class IPlotData;

extern SmpData SmpDataInstance;

extern "C" SMPLOADER_EXPORT IPlotData* GetIPlotData(void)
{
    return &SmpDataInstance;
}


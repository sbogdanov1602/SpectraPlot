#pragma once
#include "cchloader_global.h"
#include "CchData.h"

class IPlotData;

extern CchData CchDataInstance;

extern "C" CCHLOADER_EXPORT IPlotData* GetIPlotData(void)
{
    return &CchDataInstance;
}


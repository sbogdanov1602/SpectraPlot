#pragma once
#include "cchloader_global.h"
#include "CchData.h"

class IPlotData;

extern CchData CchDataInstance;

namespace cch
{
    CCHLOADER_EXPORT IPlotData* GetIPlotData(void);
}


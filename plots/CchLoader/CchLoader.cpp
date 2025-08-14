#pragma once
#include "CchLoader.h"
#include "CommonLib.h"

namespace cch
{
    IPlotData* GetIPlotData(void)
    {
        return &CchDataInstance;
    }

}

CchData CchDataInstance;


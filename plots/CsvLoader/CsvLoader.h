#pragma once

#include "csvloader_global.h"
#include "CsvData.h"

class IPlotData;

extern CsvData CsvDataInstance;

extern "C" CSVLOADER_EXPORT IPlotData* GetIPlotData(void)
{
    return &CsvDataInstance;
}

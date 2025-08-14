#include "SmpLoader.h"
#include "CommonLib.h"
#include "SmpData.h"

namespace smp 
{
    IPlotData* GetIPlotData(void)
    {
        return &SmpDataInstance;
    }

}

SmpData SmpDataInstance;


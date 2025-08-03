#pragma once

#include "i_map.h"
#include "i_calib.h"
#include <windows.h>


class IFileMap: public IMap
{
public:
        virtual int SetNewMax(int iNewMaxVal, bool bCheck)=0;
        virtual int UpDateLineNum(int iLineNum)=0;
        virtual ICalib* GetCalib() = 0;
        virtual HANDLE GetFileHandle()const = 0;
};

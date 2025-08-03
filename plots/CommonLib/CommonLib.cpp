// CommonLib.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "CommonLib.h"


// This is an example of an exported variable
COMMONLIB_API int nCommonLib=0;

// This is an example of an exported function.
COMMONLIB_API int fnCommonLib(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CCommonLib::CCommonLib()
{
    return;
}

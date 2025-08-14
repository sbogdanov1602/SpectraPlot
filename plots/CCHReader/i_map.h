#pragma once

struct IMap
{
	virtual int* __cdecl GetData(int &NX, int &NY, int &Max) = 0;
	virtual void __cdecl GetXBounds(double &min, double &max) = 0;
	virtual void __cdecl GetYBounds(double &min, double &max) = 0;

	virtual void** __cdecl getClustersPointer() = 0;
	virtual int __cdecl getClustersCount() = 0;

	virtual int __cdecl GetMax() = 0;
	virtual int __cdecl GetLastLine() = 0;
};

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the COMMONLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// COMMONLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once
#include <vector>
#include <string>
#include <functional>
#ifdef COMMONLIB_EXPORTS
#define COMMONLIB_API __declspec(dllexport)
#else
#define COMMONLIB_API __declspec(dllimport)
#endif

// This class is exported from the dll
class COMMONLIB_API CCommonLib {
public:
	CCommonLib(void);
	// TODO: add your methods here.
};

class COMMONLIB_API IPlotData
{
public:
	IPlotData();
	virtual std::vector<std::vector<double>>& LstSpecData() = 0;

	virtual float PointScale() = 0;
	virtual float MeasurementStep() = 0;
	virtual double SignalCoeff() = 0;

	virtual int Load(std::string filePath, std::function<void (int)>  setProgressDlgValue, std::function<void(int)>  setProgressDlgMaximum,  bool loadOneFileOnly) = 0;

	virtual std::string MapXAxisLabel() = 0;
	virtual std::string MapYAxisLabel() = 0;
	virtual std::string HXAxisLabel() = 0;
	virtual std::string HYAxisLabel() = 0;
	virtual std::string VXAxisLabel() = 0;
	virtual std::string VYAxisLabel() = 0;

	double GetMaxSignal() { return m_maxSignal; }
	void SetMaxSignal(double value) { m_maxSignal = value; }
	void CancelLoad() { m_loadIsCanceled = true; }

	int PointsNum();
	int SpectraNum();
	void ClearData();
protected:

	std::vector<std::vector<double>> m_lstSpecData;
	double m_maxSignal;
	bool m_loadIsCanceled;
};

extern COMMONLIB_API int nCommonLib;

COMMONLIB_API int fnCommonLib(void);

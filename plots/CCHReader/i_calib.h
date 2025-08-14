#pragma once
#include <memory>
/// <summary>
/// Интерфейс описания применения время-пролетной и других типов калибровки сигнала
/// Присяч С.С.
/// </summary>
class ICalib;
typedef std::shared_ptr<ICalib> CalibPointer;
class ICalib
{
public:
	virtual double calibrate(double) = 0;
    virtual double decalibrate(double) = 0;
//	virtual QString description() const = 0;
};
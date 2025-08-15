#pragma once
#include <qmetatype.h>
#include <wtypes.h>
#include <qmap.h>

struct jsonLib 
{
	int id;
	QString ext;
	QString libName;
	QString funcName;
	bool loadOneFileOnly;
	HMODULE hmodule;
};

class Libraries
{
public:
	Libraries();
	~Libraries();
	void Load();
	jsonLib* GetLib(QString& ext);
private:
	QString m_FileName;
	QMap<QString, jsonLib> m_Libs;
};

extern Libraries Dlls;


#include "Settings.h"
#include <qfile.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QApplication>

CSettings::CSettings()
{
	m_PointScale = 24.0f;
	m_MeasurementStep = 1.0f;
	m_FileName = "SpectraPlot.json";
	m_SignalCoeff = 0.0000625;// 4.096 / 65536 
	m_PointsNum = 4000;
	m_NoiseRate = 3;
	m_MaxIterationNum = 5;
	m_ChangeCoeff = 0.5;
}

CSettings::~CSettings()
{
}

void CSettings::Load()
{
	QString val;
	QFile file;
	file.setFileName(m_FileName);
	if (file.exists()) {
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		val = file.readAll();
		file.close();

		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8(), &error);

		if (doc.isObject())
		{
			QJsonObject json = doc.object();
			m_PointScale = (float)json["PointScale"].toDouble();
			m_MeasurementStep = (float)json["MeasurementStep"].toDouble();
			m_SignalCoeff = json["SignalCoeff"].toDouble();
			m_PointsNum = json["PointsNum"].toInteger();
			m_NoiseRate = json["NoiseRate"].toInteger();
			m_MaxIterationNum = json["MaxIterationNum"].toInteger();
			m_ChangeCoeff = json["ChangeIntervalCoeff"].toDouble();;
		}
	}
}

CSettings gSettings;

#include "Libraries.h"
#include <qfile.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QApplication>
#include <qmessagebox.h>

Libraries::Libraries()
{
	m_FileName = "Libraries.json";
}

Libraries::~Libraries()
{
	for (const jsonLib& lib : m_Libs) {
		if (lib.hmodule) {
			FreeLibrary(lib.hmodule);
		}
	}
}

void Libraries::Load()
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

		if (doc.isArray())
		{
			QJsonArray jsonArray = doc.array();
			for (const QJsonValue& value : jsonArray) {
				if (value.isObject()) {
					QJsonObject jsonObj = value.toObject();
					jsonLib dll;
					dll.id = jsonObj["id"].toInt();
					dll.ext = jsonObj["ext"].toString();
					dll.libName = jsonObj["libName"].toString();
					dll.funcName = jsonObj["funcName"].toString();
					dll.loadOneFileOnly = jsonObj["loadOneFileOnly"].toBool();
					auto str = dll.libName.toStdString();
					auto pC = &(str[0]);
					dll.hmodule = ::LoadLibraryA(pC);
					if (dll.hmodule) {
						m_Libs[dll.ext] = dll;
						m_FileFilter += " *." + dll.ext;
					}
					else {
						QMessageBox::warning(nullptr, "SpectraPlot", QObject::tr("Cannot find a library '%1'.").arg(dll.libName));
					}
				}
			}
			if (!m_FileFilter.isEmpty()) {
				m_FileFilter = "(" + m_FileFilter.mid(1) + ")";
			}
		}
	}
	else {
		QMessageBox::warning(nullptr, "SpectraPlot", QObject::tr("Cannot find file '%1'.").arg(m_FileName));
	}
}

jsonLib* Libraries::GetLib(QString& ext)
{
	auto it = m_Libs.find(ext);
	if (it != m_Libs.end()) {
		return &(it.value());
	}
	return nullptr;
}

Libraries Dlls;


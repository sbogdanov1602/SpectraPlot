#include "SmpData.h"
#include <qmetatype.h>
#include <qdir.h>
#include <qsettings.h>
#include <vector>

SmpData::SmpData():IPlotData()
{
    m_iMeaningAreaBeg = 172;
    m_iMeaningAreaLength = 101;
    LoadSettings();
}

void SmpData::LoadSettings()
{
    auto settingsFile = QDir::currentPath() + "/smp_data.ini";
    if (QFile::exists(settingsFile)) {
        QSettings settings(settingsFile, QSettings::IniFormat);
        m_iMeaningAreaBeg = settings.value("MeaningAreaBeg").toInt();
        m_iMeaningAreaLength = settings.value("MeaningAreaLength").toInt();
        m_fPointScale = settings.value("PointScale", 0.0f).toFloat();
        m_fMeasurementStep = settings.value("MeasurementStep", 0.0f).toFloat();
        m_dSignalCoeff = settings.value("SignalCoeff", 0.0f).toDouble();
        m_sMapXAxisLabel = settings.value("MapXAxisLabel", "").toString().toStdString();
        m_sMapYAxisLabel = settings.value("MapYAxisLabel", "").toString().toStdString();
        m_sSignalAxisLabel = settings.value("SignalAxisLabel", "").toString().toStdString();
    }
}

int SmpData::Load(std::string inFilePath, std::function<void(int)>  setProgressDlgValue, std::function<void(int)>  setMaximum, bool loadOneFileOnly)
{
    ClearData();
    double dBaseLine = 0.0;

    QString fname(&(inFilePath[0]));
    QString dir;
    QStringList fileList;

    if (!loadOneFileOnly) {
        auto pos = fname.lastIndexOf('/');
        if (pos >= 0) {
            dir = fname.left(pos);
            QDir directory(dir);
            fileList = directory.entryList(QStringList() << "*.smp" << "*.SMP", QDir::Files);
            std::sort(fileList.begin(), fileList.end());
        }

        if (dir.isEmpty())
            return 0;
    }
    else {
        fileList.push_back(fname);
    }

    size_t nFiles = 0;
    size_t nSpecs = 0;

    int specthumLength = 0;

    int numFiles = fileList.size();
    m_maxSignal = 0.0;
    int pointsNum = 0;
    m_loadIsCanceled = false;
    
    if (setMaximum != nullptr) {
        setMaximum(numFiles);
    }
    for (auto&& filePath : fileList)
    {
        //progress.setValue(nFiles);
        if (setProgressDlgValue != nullptr)
        {
            setProgressDlgValue(nFiles);
        }
        if (m_loadIsCanceled/*progress.wasCanceled()*/)
            break;

        QFile file(dir + "/" + filePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream in(&file);
        int i = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.contains("-"))
            {
                if (pointsNum < i) {
                    pointsNum = i;
                }
                m_lstSpecData.push_back(std::vector<double>());
                dBaseLine = 0.0;
                i = 0;
                continue;
            }
            if (m_lstSpecData.empty())
                continue;

            auto&& vec = m_lstSpecData.back();

            bool ok = false;
            double val = line.toInt(&ok);

            if (!ok)
                break;

            val *= SignalCoeff();

            if (vec.size() == (m_iMeaningAreaBeg + m_iMeaningAreaLength + 1)) {
                dBaseLine = CalculateBaseLine(vec, m_iMeaningAreaBeg, m_iMeaningAreaLength);
                CorrectDataToBase(vec, dBaseLine);
            }
            
            val -= dBaseLine;
            if (val < 0.0) {
                val = 0.0;
            }

            if (m_maxSignal < val) {
                m_maxSignal = val;
            }
            vec.push_back(val);
            i++;
        }
        in.flush();
        file.close();
        if (pointsNum < i) {
            pointsNum = i;
        }
        ++nFiles;

        if (loadOneFileOnly) {
            break;
        }
    }

    if (setProgressDlgValue != nullptr)
    {
        setProgressDlgValue(nFiles);
    }

    return nFiles;
}

float SmpData::PointScale()
{
    return m_fPointScale;
}

float SmpData::MeasurementStep()
{
    return m_fMeasurementStep;
}

double SmpData::SignalCoeff()
{
    return m_dSignalCoeff;//0.0000625;// 4.096 / 65536 ;
}

std::string SmpData::MapXAxisLabel()
{
    return m_sMapXAxisLabel;//"t (ms)";
}

std::string SmpData::MapYAxisLabel()
{
    return m_sMapYAxisLabel;// "t (sec)";
}

std::string SmpData::SignalAxisLabel()
{
    return m_sSignalAxisLabel;// "signal (V)";
}

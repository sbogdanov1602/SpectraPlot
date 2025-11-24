#include "SmpData.h"
#include <qmetatype.h>
#include <qdir.h>

SmpData::SmpData():IPlotData()
{
    m_dBaseLine = 0.0;
    m_iMeanValIndex = 100;
    m_iMeaningAreaBeg = 171;
    m_iMeaningAreaLength = 100;
}

int SmpData::Load(std::string inFilePath, std::function<void(int)>  setProgressDlgValue, std::function<void(int)>  setMaximum, bool loadOneFileOnly)
{
    ClearData();

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
            if (nFiles == 0 && i == m_iMeanValIndex) {
                m_dBaseLine = val;
                CorrectDataToBase(vec);
            }
            val -= m_dBaseLine;
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

void SmpData::CalculateBaseLine(std::vector<double>& data)
{
    int m_dBaseLine = 0.0;
/*
    for (int i = m_iMeaningAreaBeg; (i < m_iMeaningAreaBeg + m_iMeaningAreaLength) && i < data.size(); i++)
    {
        if (i != m_iMeaningAreaBeg)
        {
            int n = i - m_iMeaningAreaBeg;
            m_dBaseLine = (m_dBaseLine * n + data[i]) / (n + 1);
        }
        else
        {
            m_dBaseLine = data[i];
        }
    }
*/
}

void SmpData::CorrectDataToBase(std::vector<double>& data)
{
    for (int i = data.size() - 1; i >= 2; i--)
    {
        data[i] = data[i] - m_dBaseLine;
    }
}

float SmpData::PointScale()
{
    return 24.0f;
}

float SmpData::MeasurementStep()
{
    return 1.0f;
}

double SmpData::SignalCoeff()
{
    return 0.0000625;// 4.096 / 65536 ;
}

std::string SmpData::MapXAxisLabel()
{
    return "t (ms)";
}

std::string SmpData::MapYAxisLabel()
{
    return "t (sec)";
}

std::string SmpData::SignalAxisLabel()
{
    return "signal (V)";
}

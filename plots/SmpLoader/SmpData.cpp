#include "SmpData.h"
#include <qmetatype.h>
#include <qdir.h>

SmpData::SmpData():IPlotData()
{
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

std::vector<std::vector<double>>& SmpData::LstSpecData()
{
    return m_lstSpecData;
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

std::string SmpData::HXAxisLabel()
{
    return "t (ms)";
}

std::string SmpData::HYAxisLabel()
{
    return "signal (V)";
}

std::string SmpData::VYAxisLabel()
{
    return "t (sec)";
}

std::string SmpData::VXAxisLabel()
{
    return "signal (V)";
}

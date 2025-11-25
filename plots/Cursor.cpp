#include "Cursor.h"
#include <Settings.h>
#include "pd_alg/pd_alg.h"
#include <Common.h>

Line::Line() {
    m_line = nullptr;
    m_plot = nullptr;
}

void Line::PlotLine(QCustomPlot* plot, QPointF beg, QPointF end, QPen pen /* = QPen(Qt::black)*/)
{
    if (m_line == nullptr) {
        m_plot = plot;
        m_line = new QCPItemLine(plot);
    }
    m_line->setPen(pen);
    m_line->start->setCoords(beg.x(), beg.y());
    m_line->end->setCoords(end.x(), end.y());
}

void Line::ClearLine() 
{
    if (m_line != nullptr) {
        m_plot->removeItem(m_line);
        m_line = nullptr;
    }
}

bool Line::Exists()
{
    return (m_line != nullptr);
}

Cursor::Cursor() : Line()
{
    m_textLabel = nullptr;
}

BaseCursor::BaseCursor() : Cursor() 
{
    ClearData();
}

void Cursor::CreateVCursor(QCustomPlot* plot, QPointF beg, QPointF end, QPointF textData, BaseCursor* baseCursor, QPen pen /*= QPen(Qt::black)*/)
{
    if (m_textLabel == nullptr) {
        m_plot = plot;
        m_textLabel = new QCPItemText(plot);
    }
    m_textLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    m_textLabel->position->setCoords(end.x(), end.y() * 0.997); // place position at center/top of axis rect
    QString s;
    if (baseCursor->Exists()) {
        s = QString::asprintf(" t (ms): %7.2f \n ratio to base: %6.3f \n signal (V): %6.3f ", textData.x(), textData.x() / baseCursor->GetPosition(), textData.y());   /*%10.2f*/
    }
    else {
        s = QString::asprintf(" t (ms): %7.2f \n signal (V): %6.3f ", textData.x(), textData.y());   /*%10.2f*/
    }
    m_textLabel->setText(s);
    m_textLabel->setFont(QFont(plot->font().family(), 9)); // make font a bit larger
    m_textLabel->setPen(pen); // show black border around text

    PlotLine(plot, beg, end, pen);
}

void Cursor::CreateHCursor(QCustomPlot* plot, QPointF beg, QPointF end, QPointF textData, QPen pen /*= QPen(Qt::black)*/)
{
    if (m_textLabel == nullptr) {
        m_plot = plot;
        m_textLabel = new QCPItemText(plot);
    }
    m_textLabel->setPositionAlignment(Qt::AlignRight | Qt::AlignBottom);
    m_textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    m_textLabel->position->setCoords(end.x()*0.997, end.y()); // place position at center/top of axis rect
    auto s = QString::asprintf(" t (sec): %d \n signal (V): %6.3f ", (int)textData.x(), textData.y());   /*%10.2f*/
    m_textLabel->setText(s);
    m_textLabel->setFont(QFont(plot->font().family(), 9)); // make font a bit larger
    m_textLabel->setPen(pen); // show black border around text

	PlotLine(plot, beg, end, pen);
}

void Cursor::ClearCursor()
{
    ClearLine();
    if (m_textLabel != nullptr) {
        m_plot->removeItem(m_textLabel);
        m_textLabel = nullptr;
    }
}

void BaseCursor::Clear()
{
    ClearCursor();
    ClearData();
}

void BaseCursor::ClearData() 
{
    m_iPeakIndx = 0;
    m_dPeakPos = 0.0;
    m_dStartx_key = 0.0;
    m_dEndx_key = 0.0;
    m_bExists = false;
}


Result  BaseCursor::Create(QCustomPlot* plot, QCPItemLine* hLine, std::vector<std::vector<double>>* lstSpecData, int iSpecIndx)
{
    m_plot = plot;
    if (!m_bExists) {
        QCPItemPosition* endx = hLine->end;
        QCPItemPosition* startx = hLine->start;
        m_dStartx_key = startx->key();
        m_dEndx_key = endx->key();
    }
    double stepH = gSettings.GetPointScale() / gSettings.GetPointsNum();
    int leftX  = (int)(m_dStartx_key / stepH);
    int rightX = (int)(m_dEndx_key / stepH);

    size_t count = lstSpecData->at(iSpecIndx).size();

    auto algParams = TPDAlgParams();
    algParams.bAutoNoise = true;
    algParams.bAllData = false;
    algParams.iLeftLim = leftX;
    algParams.iRightLim = rightX;
    //algParams.iMaxPeakCount = 1;
    algParams.iNoiseRate = gSettings.GetNoiseRate();

    int* pData = new int[count];
    for (int k = 0; k < count; k++) {
        pData[k] = (int)((*lstSpecData)[iSpecIndx][k] / gSettings.GetSignalCoeff());
    }
    auto vecPeaks = PDAlg::PeaksDetecting(pData, count, 1.0, &algParams);
    delete pData;

    Result ret;
    ret.description = BASE_PEAK_NAME;
    ret.time = QTime::currentTime().toString();
    ret.date = QDate::currentDate().toString();
    ret.id = BASE_CURSOR_ID;

    if (vecPeaks.size() > 0) {
        int max = 0, max_idx = 0;
        for (int idx = 0; idx < vecPeaks.size(); idx++) {
            auto peak = vecPeaks[idx];
            if (peak.Height > max) {
                max = peak.Height;
                max_idx = peak.Extrem;
            }
        }
        m_iPeakIndx = max_idx;
        m_dPeakPos = max_idx * stepH;
        auto signalValue = max * gSettings.GetSignalCoeff();

        ret.piakHight = signalValue;
        ret.peakPosX = m_dPeakPos;
        ret.peakPosY = iSpecIndx * gSettings.GetMeasurementStep();
        ret.ratio = 1.0;
        ret.leftX = m_dStartx_key;
        ret.rightX = m_dEndx_key;

        m_bExists = true;
        auto range = m_plot->graph(0)->valueAxis()->range();
        CreateVCursor(m_plot, QPointF(m_dPeakPos, range.lower), QPointF(m_dPeakPos, range.upper), QPointF(m_dPeakPos, signalValue), this, QPen(Qt::red));
    }
    return ret;
}

#include "Cursor.h"

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

void Cursor::CreateVCursor(QCustomPlot* plot, QPointF beg, QPointF end, QPointF textData)
{
    if (m_textLabel == nullptr) {
        m_plot = plot;
        m_textLabel = new QCPItemText(plot);
    }
    m_textLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    m_textLabel->position->setCoords(end.x(), end.y() * 0.997); // place position at center/top of axis rect
    auto s = QString::asprintf(" t (ms): %7.2f \n signal (V): %6.3f ", textData.x(), textData.y());   /*%10.2f*/
    m_textLabel->setText(s);
    m_textLabel->setFont(QFont(plot->font().family(), 9)); // make font a bit larger
    m_textLabel->setPen(QPen(Qt::black)); // show black border around text

    PlotLine(plot, beg, end);
}

void Cursor::CreateHCursor(QCustomPlot* plot, QPointF beg, QPointF end, QPointF textData)
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
    m_textLabel->setPen(QPen(Qt::black)); // show black border around text

	PlotLine(plot, beg, end);
}

void Cursor::ClearCursor()
{
    ClearLine();
    if (m_textLabel != nullptr) {
        m_plot->removeItem(m_textLabel);
        m_textLabel = nullptr;
    }
}


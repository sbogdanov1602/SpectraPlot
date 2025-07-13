#include "TextLabelResults.h"
#include "qcustomplot.h"
#include "Common.h"

CTextLabelResults::CTextLabelResults()
{
	m_textLabel = nullptr;
	m_plot = nullptr;
	m_integral = nullptr;
	m_2Dintegral = nullptr;
}

void CTextLabelResults::ClearLabelResults()
{
	if (m_textLabel != nullptr) {
		m_plot->removeItem(m_textLabel);
		m_textLabel = nullptr;
	}
}

void CTextLabelResults::FillLabelResults(QCustomPlot* plot, CalcResult* integral, CalcResult* integral2D)
{
	if (m_textLabel == nullptr) {
		m_plot = plot;
		m_integral = integral;
		m_2Dintegral = integral2D;
		m_textLabel = new QCPItemText(plot);
	}
	m_textLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
	m_textLabel->position->setType(QCPItemPosition::ptViewportRatio);
	m_textLabel->position->setCoords(0.5, 0.5); // place position at center/top of axis rect
	QString s = "";
	if (m_integral->actual && m_2Dintegral->actual) {
		s = QString::asprintf(" integral value: %8.4f \n 2D integral value: %8.4f ", m_integral->value, m_2Dintegral->value);   /*%10.2f*/
	}
	else if (m_integral->actual) {
		s = QString::asprintf(" integral value: %8.4f ", m_integral->value);   /*%10.2f*/
	}else if (m_2Dintegral->actual) {
		s = QString::asprintf(" 2D integral value: %8.4f ", m_2Dintegral->value);   /*%10.2f*/
	}
	m_textLabel->setText(s);
	m_textLabel->setFont(QFont(plot->font().family(), 10)); // make font a bit larger
	m_textLabel->setPen(QPen(Qt::black)); // show black border around text

}


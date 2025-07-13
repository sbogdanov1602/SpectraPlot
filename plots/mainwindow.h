/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2022 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: https://www.qcustomplot.com/                         **
**             Date: 06.11.22                                             **
**          Version: 2.1.1                                                **
****************************************************************************/

/************************************************************************************************************
**                                                                                                         **
**  This is the example code for QCustomPlot.                                                              **
**                                                                                                         **
**  It demonstrates basic and some advanced capabilities of the widget. The interesting code is inside     **
**  the "setup(...)Demo" functions of MainWindow.                                                          **
**                                                                                                         **
**  In order to see a demo in action, call the respective "setup(...)Demo" function inside the             **
**  MainWindow constructor. Alternatively you may call setupDemo(i) where i is the index of the demo       **
**  you want (for those, see MainWindow constructor comments). All other functions here are merely a       **
**  way to easily create screenshots of all demos for the website. I.e. a timer is set to successively     **
**  setup all the demos and make a screenshot of the window area and save it in the ./screenshots          **
**  directory.                                                                                             **
**                                                                                                         **
*************************************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.
#include <vector>
#include "Common.h"
#include "Cursor.h"
//#include <TextLabelResults.h>
#include "IntegralModel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
  void setupColorDataMap(QCustomPlot* customPlot);

public slots:
	void onOpenAction();
	void onPlotDblClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event);
	void onPlotClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event);
	void onPlotHClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event);
	void onPlotVClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event);
	void onResetAction();
	void onCalculateIntegralAction();
	void onCalculateSummAction();
private:
  Ui::MainWindow *ui;
  QString demoName;
  QTimer dataTimer;
  QCPItemTracer *itemDemoPhaseTracer;
  int currentDemoIndex;

  QGroupBox* groupBox;
  QHBoxLayout* horizontalLayout;
  QWidget* bottomWidget;
  QString m_sAppSettingsFile;
  QSettings* m_pSettings;
  IntegralModel* m_pIntegralModel;

  QString m_dir;
  QString m_LastFolder;
  QStringList m_fileList;
  std::vector<std::vector<double>> m_lstSpecData;
  std::vector<std::vector<double>> m_lstSpecDataT;
  double m_spectraNum;
  double m_maxSignal;
  double m_pointsNum;
  double m_pointStep;
  double m_measurementStep;
  double m_SignalCoeff;
  int m_NMeaning;
  int m_spectrumIdx;
  int m_pointIdx;
  CalcResult m_integral;
  CalcResult m_2Dintegral;

  Cursor m_HPlotCursor;
  Cursor m_VPlotCursor;
  Cursor m_HeatMapHCursor;
  Cursor m_HeatMapVCursor;

  Line m_HSelectMap;
  Line m_HSelectMap2;
  Line m_VSelectMap;
  Line m_VSelectMap2;
  Line m_HSelect;
  Line m_VSelect;

  //CTextLabelResults m_LabelResults;

  QCPColorMap* m_colorMap;
  QCPColorScale* m_colorScale;
  QCPMarginGroup* m_marginGroup;

  void LoadSpectra();
  void plotOneGraph(QCustomPlot* customPlot, std::vector<double> in);
  void plotTwoGraph(QCustomPlot* customPlot, std::vector<double> in);
  void restoreWidgetsGeometry();
  void storeWidgetsGeometry();

  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void ClearLines();
  void ClearResults();
  void ClearCursors();

private slots:
  void selectionChanged();
  void selectionHChanged();

};

#endif // MAINWINDOW_H

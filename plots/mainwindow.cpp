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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#  include <QDesktopWidget>
#endif
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <QProgressDialog>
#include "Settings.h"
#include "IntegralCalc.h"
#include "IntegralSummCalc.h"
#include "TextLabelResults.h"
#include <C2DIntegralCalc.h>
#include "pd_alg/pd_alg.h"
#include "pd_alg/pd_data.h"
//#include "SmpLoader/SmpLoader.h"
//#include "CchLoader/CchLoader.h"
#include "Libraries.h"

Ui_MainWindow;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  m_pPlotData = nullptr;
  m_pProgressDlg = nullptr;
  m_dir = QApplication::applicationDirPath();
  /*
  m_spectraNum = 0.0;
  m_maxSignal = 0.0;
  m_pointsNum = 0.0;
  m_NMeaning = 0;
  m_pointStep = 24.0/4000;
  m_measurementStep = 1.0;
  m_SignalCoeff = 1.0;
  */
  m_spectrumIdx = 0;
  m_pointIdx = 0;

  m_colorMap = nullptr;
  m_colorScale = nullptr;
  m_marginGroup = nullptr;
  
  ui->setupUi(this);

//  ui->customPlotH_2->xAxis->setVisible(false);
//  ui->customPlotH_2->yAxis->setVisible(false);

  setGeometry(400, 250, 1200, 700);
  QObject::connect(ui->actionOpen, &QAction::triggered, this, qOverload<>(&MainWindow::onOpenAction));
  QObject::connect(ui->actionReset, &QAction::triggered, this, qOverload<>(&MainWindow::onResetAction));
  QObject::connect(ui->actionCalculate_integral, &QAction::triggered, this, qOverload<>(&MainWindow::onCalculateIntegralAction));
  QObject::connect(ui->actionCalculate_summ, &QAction::triggered, this, qOverload<>(&MainWindow::onCalculateSummAction));

  QObject::connect(ui->customPlot, &QCustomPlot::plottableDoubleClick, this, qOverload<QCPAbstractPlottable*, int, QMouseEvent*>(&MainWindow::onPlotDblClick));//plottableClick
  QObject::connect(ui->customPlot, &QCustomPlot::plottableClick, this, qOverload<QCPAbstractPlottable*, int, QMouseEvent*>(&MainWindow::onPlotClick));
  QObject::connect(ui->customPlotH, &QCustomPlot::plottableClick, this, qOverload<QCPAbstractPlottable*, int, QMouseEvent*>(&MainWindow::onPlotHClick));
  QObject::connect(ui->customPlot2, &QCustomPlot::plottableClick, this, qOverload<QCPAbstractPlottable*, int, QMouseEvent*>(&MainWindow::onPlotVClick));
  
  //QObject::connect(ui->customPlot2, &QCustomPlot::selectionChangedByUser, this, qOverload<>(&MainWindow::selectionChanged/*SLOT(selectionChanged()*/));
  connect(ui->customPlot2, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
  connect(ui->customPlotH, SIGNAL(selectionChangedByUser()), this, SLOT(selectionHChanged()));

  QMetaObject::connectSlotsByName(this);

  gSettings.Load();
  /*
  m_pointStep = gSettings.GetPointScale() / gSettings.GetPointsNum();
  m_measurementStep = gSettings.GetMeasurementStep();
  m_SignalCoeff = gSettings.GetSignalCoeff();
  */
  m_pIntegralModel = new IntegralModel();
  ui->tableViewH_2->setModel(m_pIntegralModel);
  ui->tableViewH_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableViewH_2->verticalHeader()->hide();
  ui->tableViewH_2->setShowGrid(true);
  ui->tableViewH_2->show();

  m_sAppSettingsFile = QApplication::applicationDirPath() + "/appsettings.ini";
  m_pSettings = new QSettings(m_sAppSettingsFile, QSettings::IniFormat);
  if (QFile::exists(m_sAppSettingsFile)) {
      restoreWidgetsGeometry();
  }
  else {
      QScreen* screen = QGuiApplication::primaryScreen();
      resize(screen->availableSize() * 0.8);
  }
  Dlls.Load();
}

void MainWindow::restoreWidgetsGeometry()
{
    QByteArray geometryData = m_pSettings->value("windowGeometry").toByteArray();
    if (geometryData.length() > 0) {
        restoreGeometry(geometryData);
    }
    geometryData = m_pSettings->value("splitter_3Geometry").toByteArray();
    if (geometryData.length() > 0) {
        ui->splitter_3->restoreGeometry(geometryData);
    }
    geometryData = m_pSettings->value("splitterGeometry").toByteArray();
    if (geometryData.length() > 0) {
        ui->splitter->restoreGeometry(geometryData);
    }
    geometryData = m_pSettings->value("splitter_2Geometry").toByteArray();
    if (geometryData.length() > 0) {
        ui->splitter_2->restoreGeometry(geometryData);
    }
    geometryData = m_pSettings->value("customPlotGeometry").toByteArray();
    if (geometryData.length() > 0) {
        ui->customPlot->restoreGeometry(geometryData);
    }
    geometryData = m_pSettings->value("customPlotHGeometry").toByteArray();
    if (geometryData.length() > 0) {
        ui->customPlotH->restoreGeometry(geometryData);
    }
    geometryData = m_pSettings->value("customPlot2Geometry").toByteArray();
    if (geometryData.length() > 0) {
        ui->customPlot2->restoreGeometry(geometryData);
    }
    geometryData = m_pSettings->value("customPlotH_2Geometry").toByteArray();
    if (geometryData.length() > 0) {
        ui->tableViewH_2->restoreGeometry(geometryData);
    }
}

void MainWindow::plotOneGraph(QCustomPlot* customPlot, std::vector<double> in)
{
    demoName = "Spectrum";
    int size = in.size();
    QVector<double> x, y;
    double v;
    double max = 0.0;
    for (int i = 0; i < size; ++i)
    {
        x.push_back(i * m_pointStep);
        v = in[i];  // let's plot a quadratic function
        if (v > max) {
            max = v;
        }
        y.push_back(v);
    }
    // create graph and assign data to it:
    if (!(customPlot->graphCount() > 0)) {
        customPlot->addGraph();
    }
    else {
        customPlot->graph(0)->data()->clear();
    }
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->xAxis->setLabel(QString(&(m_pPlotData->HXAxisLabel()[0]))/*"t (ms)"*/);
    customPlot->yAxis->setLabel(QString(&(m_pPlotData->HYAxisLabel()[0]))/*"signal (V)"*/);
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, m_pointStep * (m_pPlotData->PointsNum() - 1));
    customPlot->yAxis->setRange(0, m_pPlotData->GetMaxSignal());
    //customPlot->graph(0)->data()->keyRange(QCPRange(0, 24.0));
    customPlot->axisRect()->setupFullAxesBox(true);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);// | QCP::iSelectPlottables);
    //customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);

 //   customPlot->replot();
}

void MainWindow::plotTwoGraph(QCustomPlot* customPlot, std::vector<double> in)
{
    demoName = "";
    int size = in.size();
    QVector<double> x, y;
    double v;
    double max = 0.0;
    for (int i = 0; i < size; ++i)
    {
        x.push_back(i * m_pPlotData->MeasurementStep()/*m_measurementStep*/);
        v = in[i];  // let's plot a quadratic function
        if (v > max) {
            max = v;
        }
        y.push_back(v);
    }
    // create graph and assign data to it:
    //customPlot->axisRect()->addAxis(QCPAxis::atRight);
    if (!(customPlot->graphCount() > 0)) {
        customPlot->addGraph(customPlot->axisRect()->axis(QCPAxis::atLeft, 0), customPlot->axisRect()->axis(QCPAxis::atBottom, 0));
    }
    else {
        customPlot->graph(0)->data()->clear();
    }

    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->yAxis->setLabel(QString(&(m_pPlotData->VYAxisLabel()[0]))/*"t (sec)"*/);
    customPlot->xAxis->setLabel(QString(&(m_pPlotData->VXAxisLabel()[0]))/*"signal (V)"*/);
    // set axes ranges, so we see all data:
    customPlot->yAxis->setRange(0, (m_pPlotData->SpectraNum() - 1) * m_pPlotData->MeasurementStep());
    customPlot->xAxis->setRange(0, m_pPlotData->GetMaxSignal());
    //customPlot->graph(0)->data()->keyRange(QCPRange(0, 24.0));
    customPlot->axisRect()->setupFullAxesBox(true);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);// | QCP::iSelectPlottables);

 //   customPlot->replot();
}

void MainWindow::onPlotDblClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event)
{
    auto point = event->pos();
    auto locPoint = event->localPos();
    auto x = event->x();
    auto y = event->y();
    double key, value;
    plottable->pixelsToCoords(x, y, key, value);
    auto keyAxis = plottable->keyAxis();
    auto valueAxis = plottable->valueAxis();
    bool b;
    auto keyRange = plottable->getKeyRange(b);
    bool b1;
    auto valueRange = plottable->getValueRange(b1);
    //ui->customPlot->
}

void MainWindow::onPlotClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event)
{
    auto point = event->pos();
    auto x = event->x();
    auto y = event->y();
    double key, value;
    plottable->pixelsToCoords(x, y, key, value);
    bool b;
    auto keyRange = plottable->getKeyRange(b);
    bool b1;
    auto valueRange = plottable->getValueRange(b1);

    m_spectrumIdx = (int)(value / m_pPlotData->MeasurementStep());
    plotOneGraph(ui->customPlot2, m_pPlotData->LstSpecData()[m_spectrumIdx]);
    m_pointIdx = (int)(key / m_pointStep);
    plotTwoGraph(ui->customPlotH, LstSpecDataT()[m_pointIdx]);

    auto signalValue = m_pPlotData->LstSpecData()[m_spectrumIdx].at(m_pointIdx);

    m_HeatMapHCursor.CreateHCursor(ui->customPlot, QPointF(keyRange.lower, value), QPointF(keyRange.upper, value), QPointF(value, signalValue));
    m_HeatMapVCursor.CreateVCursor(ui->customPlot, QPointF(key, valueRange.lower), QPointF(key, valueRange.upper), QPointF(key, signalValue));

    auto range = ui->customPlotH->graph(0)->valueAxis()->range();
    m_HPlotCursor.CreateHCursor(ui->customPlotH, QPointF(range.lower, value), QPointF(range.upper, value), QPointF(value, signalValue));

    range = ui->customPlot2->graph(0)->valueAxis()->range();
    m_VPlotCursor.CreateVCursor(ui->customPlot2, QPointF(key, range.lower), QPointF(key, range.upper), QPointF(key, signalValue));

    ui->customPlotH->replot();
    ui->customPlot2->replot();
    ui->customPlot->replot();
}

void MainWindow::onPlotHClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event)
{
    auto x = event->x();
    auto y = event->y();
    double key, value;
    plottable->pixelsToCoords(x, y, key, value);    
    bool b;
    auto keyRange = plottable->getKeyRange(b);
    bool b1;
    auto valueRange = plottable->getValueRange(b1);

    auto range = ui->customPlot->xAxis->range();
    m_HeatMapHCursor.CreateHCursor(ui->customPlot, QPointF(range.lower, key), QPointF(range.upper, key), QPointF(key, value));

    range = ui->customPlotH->graph(0)->valueAxis()->range();
    m_HPlotCursor.CreateHCursor(ui->customPlotH, QPointF(range.lower, key), QPointF(range.upper, key), QPointF(key, value));

    ui->customPlotH->replot();
    ui->customPlot->replot();
}

void MainWindow::onPlotVClick(QCPAbstractPlottable* plottable, int dataIndex, QMouseEvent* event)
{
    auto x = event->x();
    auto y = event->y();
    double key, value;
    plottable->pixelsToCoords(x, y, key, value);
    bool b;
    auto keyRange = plottable->getKeyRange(b);
    bool b1;
    auto valueRange = plottable->getValueRange(b1);

    auto range = ui->customPlot->yAxis->range();
    m_HeatMapVCursor.CreateVCursor(ui->customPlot, QPointF(key, range.lower), QPointF(key, range.upper), QPointF(key, value));

    range = ui->customPlot2->graph(0)->valueAxis()->range();
    m_VPlotCursor.CreateVCursor(ui->customPlot2, QPointF(key, range.lower), QPointF(key, range.upper), QPointF(key, value));

    ui->customPlot2->replot();
    ui->customPlot->replot();
}

void MainWindow::onOpenAction()
{
    int retNFiles = 0;
    auto curDir = QDir::current();
    QString filter("*.smp *.cch");
    auto fname = QFileDialog::getOpenFileName(this, tr("Open Directory"),
        m_dir.isEmpty() ? curDir.dirName() : m_dir, "(*.smp *.cch)", &filter, QFileDialog::Option::ReadOnly);

    auto pos = fname.lastIndexOf('/');
    setWindowTitle(QCoreApplication::translate("MainWindow", "SpectraPlot", nullptr));
    if (pos >= 0)
    {
        m_dir = fname.left(pos);
        pos = m_dir.lastIndexOf('/');
        if (pos >= 0) 
        {
            m_LastFolder = m_dir.mid(pos + 1);
            QString s = "SpectraPlot: " + m_LastFolder;
            setWindowTitle(QCoreApplication::translate("MainWindow", &(s.toStdString()[0]), nullptr));
        }
        
        QDir directory(m_dir);
        pos = fname.lastIndexOf('.');
        if (pos >= 0) {
            QString ext = fname.mid(pos + 1);

            m_fileList = directory.entryList(QStringList() << "*." + ext << "*." + ext.toUpper(), QDir::Files);

            if (m_pPlotData != nullptr)
            {
                //delete m_pPlotData;
                m_pPlotData = nullptr;
            }
            bool loadOneFileOnly = false;
            
            clearPlots();

            ext = ext.toLower();

            auto dllItem = Dlls.GetLib(ext);

            if (dllItem != nullptr && dllItem->hmodule) {
                
                typedef IPlotData*(__cdecl* MYFUNCTION)(); /* __cdecl* __stdcall* */

                auto s = dllItem->funcName.toStdString();
                auto pStr = &(s[0]);

                MYFUNCTION pFunction = (MYFUNCTION)GetProcAddress(dllItem->hmodule, pStr);
                if (pFunction) {
                    m_pPlotData = pFunction();
                    if (m_pPlotData) {

                        if (m_pProgressDlg != nullptr) {
                            delete m_pProgressDlg;
                            m_pProgressDlg = nullptr;
                        }
                        int fileCount = (loadOneFileOnly) ? 1 : m_fileList.size();
                        m_pProgressDlg = new QProgressDialog(tr("Loading files..."), tr("Stop"), 0, fileCount, this);
                        m_pProgressDlg->setWindowModality(Qt::WindowModal);
                        QObject::connect(m_pProgressDlg, &QProgressDialog::canceled, this, qOverload<>(&MainWindow::progressDlgWasCanceled));
                        m_pProgressDlg->show();
                        m_pProgressDlg->setValue(0);

                        auto ptrToSetValue = std::bind(&QProgressDialog::setValue, m_pProgressDlg, std::placeholders::_1);
                        auto ptrToSetMaximum = std::bind(&QProgressDialog::setMaximum, m_pProgressDlg, std::placeholders::_1);

                        retNFiles = m_pPlotData->Load(fname.toStdString(), ptrToSetValue, ptrToSetMaximum, loadOneFileOnly);
                        makeSpecDataT();

                        QObject::disconnect(m_pProgressDlg, &QProgressDialog::canceled, this, qOverload<>(&MainWindow::progressDlgWasCanceled));
                        m_pProgressDlg->close();
                        delete m_pProgressDlg;
                        m_pProgressDlg = nullptr;

                        QMessageBox::information(
                            this,
                            "SMP files is loaded",
                            QStringLiteral("Files: ") + QString::number(retNFiles) + ", Spectra: " + QString::number(m_pPlotData->SpectraNum()),
                            QMessageBox::StandardButton::Ok);

                        setupColorDataMap(ui->customPlot);
                    }
                }
                else
                {
                    FreeLibrary(dllItem->hmodule); // Unload the DLL if function not found
                    dllItem->hmodule = nullptr;
                    QMessageBox::warning(this, "SpectraPlot", tr("Cannot find a function '%1' in module '%2'.").arg(dllItem->funcName).arg(dllItem->libName));
                }
            }
            else {
                if (dllItem == nullptr) {
                    QMessageBox::warning(this, "SpectraPlot", tr("Cannot find a record about library for extension '%1'.").arg(ext));
                }
                else if (!dllItem->hmodule) {
                    QMessageBox::warning(this, "SpectraPlot", tr("The library for extension '%1' isn't loaded.").arg(ext));
                }
            }
        }
    }
}

void MainWindow::ClearCursors()
{
    m_HPlotCursor.ClearCursor();
    m_VPlotCursor.ClearCursor();
    m_HeatMapHCursor.ClearCursor();
    m_HeatMapVCursor.ClearCursor();
}

void MainWindow::onResetAction()
{
    if (m_pPlotData != nullptr && m_pPlotData->LstSpecData().size() > 0)
    {        
        ClearLines();
        ClearResults();
        ClearCursors();

        ui->customPlot->rescaleAxes();
        plotOneGraph(ui->customPlot2, m_pPlotData->LstSpecData().at(0));
        plotTwoGraph(ui->customPlotH, LstSpecDataT().at(0));
        
        ui->customPlotH->replot();
        ui->customPlot2->replot();
        ui->customPlot->replot();
 //       ui->customPlotH_2->replot();
    }
}

void MainWindow::clearPlots()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->clearPlottables();
    ui->customPlot->clearItems();
    ui->customPlotH->clearGraphs();
    ui->customPlotH->clearPlottables();
    ui->customPlotH->clearItems();
    ui->customPlot2->clearGraphs();
    ui->customPlot2->clearPlottables();
    ui->customPlot2->clearItems();
    
    ui->customPlot->replot();
    ui->customPlotH->replot();
    ui->customPlot2->replot();
}

void MainWindow::setupColorDataMap(QCustomPlot* customPlot)
{
    demoName = "Color Data Map";
    if(m_pPlotData != nullptr && m_pPlotData->LstSpecData().size() > 0)
    {
        ClearLines();
        ClearResults();
        ClearCursors();
        clearPlots();
        // configure axis rect:
        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
        customPlot->axisRect()->setupFullAxesBox(true);
        customPlot->xAxis->setLabel(QString(&(m_pPlotData->MapXAxisLabel()[0])));
        customPlot->yAxis->setLabel(QString(&(m_pPlotData->MapYAxisLabel()[0])));

        // set up the QCPColorMap:
        m_colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
        int nx = m_pPlotData->LstSpecData().at(0).size();
        int ny = m_pPlotData->LstSpecData().size();
        m_colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
        double xMax = gSettings.GetPointScale();
        m_pointStep = xMax / (nx - 1);
        //m_measurementStep = gSettings.GetMeasurementStep();
        double yMax = (ny - 1) * m_pPlotData->MeasurementStep();
        m_colorMap->data()->setRange(QCPRange(0, xMax), QCPRange(0, yMax)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
        // now we assign some data, by accessing the QCPColorMapData instance of the color map:
        double x, y, z;
        for (int yIndex = 0; yIndex < ny; ++yIndex)
        {
            auto v = m_pPlotData->LstSpecData().at(yIndex);
            for (int xIndex = 0; xIndex < nx; ++xIndex)
            {
                m_colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
                z = v.at(xIndex);
                m_colorMap->data()->setCell(xIndex, yIndex, z);
            }
        }

        // add a color scale:
        m_colorScale = new QCPColorScale(customPlot);
        customPlot->plotLayout()->addElement(0, 1, m_colorScale); // add it to the right of the main axis rect
        m_colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
        m_colorMap->setColorScale(m_colorScale); // associate the color map with the color scale
        m_colorScale->axis()->setLabel("Magnetic Field Strength");
        m_colorScale->setRangeZoom(false);
        m_colorScale->setRangeDrag(false);

        // set the color gradient of the color map to one of the presets:
        m_colorMap->setGradient(QCPColorGradient::gpPolar);
        // we could have also created a QCPColorGradient instance and added own colors to
        // the gradient, see the documentation of QCPColorGradient for what's possible.

        // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
        m_colorMap->rescaleDataRange();

        // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
        m_marginGroup = new QCPMarginGroup(customPlot);
        customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, m_marginGroup);
        m_colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, m_marginGroup);

        // rescale the key (x) and value (y) axes so the whole color map is visible:
        customPlot->rescaleAxes();
        plotOneGraph(ui->customPlot2, m_pPlotData->LstSpecData().at(0));
        plotTwoGraph(ui->customPlotH, LstSpecDataT().at(0));
        ui->customPlotH->replot();
        ui->customPlot2->replot();

        //setWindowTitle("QCustomPlot: " + demoName);
        statusBar()->clearMessage();
        ui->customPlot->replot();
    }
}

void MainWindow::storeWidgetsGeometry()
{
    QByteArray geometryData = saveGeometry();//ui->centralWidget->saveGeometry();
    m_pSettings->setValue("windowGeometry", geometryData);
    QByteArray splitter_3Geometry = ui->splitter_3->saveGeometry();
    m_pSettings->setValue("splitter_3Geometry", splitter_3Geometry);
    QByteArray splitterGeometry = ui->splitter->saveGeometry();
    m_pSettings->setValue("splitterGeometry", splitterGeometry);
    QByteArray splitter_2Geometry = ui->splitter_2->saveGeometry();
    m_pSettings->setValue("splitter_2Geometry", splitter_2Geometry);
    QByteArray customPlotGeometry = ui->customPlot->saveGeometry();
    m_pSettings->setValue("customPlotGeometry", customPlotGeometry);
    QByteArray customPlotHGeometry = ui->customPlotH->saveGeometry();
    m_pSettings->setValue("customPlotHGeometry", customPlotHGeometry);
    QByteArray customPlot2Geometry = ui->customPlot2->saveGeometry();
    m_pSettings->setValue("customPlot2Geometry", customPlot2Geometry);
    QByteArray tableViewH_2Geometry = ui->tableViewH_2->saveGeometry();
    m_pSettings->setValue("customPlotH_2Geometry", tableViewH_2Geometry);
}

MainWindow::~MainWindow()
{
    storeWidgetsGeometry();

    delete m_pSettings;
    if (m_pPlotData != nullptr) {
        //delete m_pPlotData;
        m_pPlotData = nullptr;
    }
    delete ui;
}

void MainWindow::selectionChanged()
{
    int graphCount = ui->customPlot2->graphCount();
    for (int i = 0; i < graphCount; ++i)
    {
        QCPGraph* graph = ui->customPlot2->graph(i);
        if (graph->selected())
        {
            QCPDataSelection selection = graph->selection();
            int rangesCnt = selection.dataRanges().count();
            auto beg = selection.dataRanges().first().begin();
            auto end = selection.dataRanges().back().end();
            QCPGraphDataContainer::const_iterator beg_min = graph->data()->at(beg);
            QCPGraphDataContainer::const_iterator end_max = graph->data()->at(end);
            bool b;
//            auto valRange0 = graph->getValueRange(b);
//            auto valRange = graph->valueAxis()->range();
            auto s = QString::asprintf("%7.3f - %7.3f", beg_min->key, end_max->key);
            ui->statusBar->showMessage(s);

            m_HSelectMap.ClearLine();
            m_HSelectMap2.ClearLine();
            m_VSelectMap2.ClearLine();
            m_HSelect.ClearLine();
            ClearResults();

            auto val = m_spectrumIdx * m_pPlotData->MeasurementStep();//m_measurementStep;
            if (m_VSelectMap.Exists()) {
                auto start = m_VSelectMap.ItemLine().start;
                auto end = m_VSelectMap.ItemLine().end;
                double start_key = start->key();
                double start_val = start->value();
                double end_key = end->key();
                double end_val = end->value();
                m_HSelectMap.PlotLine(ui->customPlot, QPointF(beg_min->key, start_val), QPointF(end_max->key, start_val), QPen(Qt::red));
                m_HSelectMap2.PlotLine(ui->customPlot, QPointF(beg_min->key, end_val), QPointF(end_max->key, end_val), QPen(Qt::red));
                m_VSelectMap.ClearLine();
                m_VSelectMap.PlotLine(ui->customPlot, QPointF(beg_min->key, start_val), QPointF(beg_min->key, end_val), QPen(Qt::red));
                m_VSelectMap2.PlotLine(ui->customPlot, QPointF(end_max->key, start_val), QPointF(end_max->key, end_val), QPen(Qt::red));
            }
            else {
                m_HSelectMap.PlotLine(ui->customPlot, QPointF(beg_min->key, val), QPointF(end_max->key, val), QPen(Qt::red));
            }
            m_HSelect.PlotLine(ui->customPlot2, QPointF(beg_min->key, beg_min->value), QPointF(end_max->key, beg_min->value), QPen(Qt::red));
            //m_HSelect.PlotLine(ui->customPlot2, QPointF(beg_min->key, valRange.lower), QPointF(end_max->key, valRange.lower), QPen(Qt::red));
            ui->customPlot2->replot();
            ui->customPlot->replot();
        }
    }
}

void MainWindow::selectionHChanged()
{
    int graphCount = ui->customPlotH->graphCount();
    for (int i = 0; i < graphCount; ++i)
    {
        QCPGraph* graph = ui->customPlotH->graph(i);
        bool b = graph->selected();
        if (b)
        {
            QCPDataSelection selection = graph->selection();
            int rangesCnt = selection.dataRanges().count();
            if (rangesCnt > 0) {
                auto beg = selection.dataRanges().first().begin();
                auto end = selection.dataRanges().back().end();
                QCPGraphDataContainer::const_iterator beg_min = graph->data()->at(beg);
                QCPGraphDataContainer::const_iterator end_max = graph->data()->at(end);                
                auto s = QString::asprintf("%7.3f - %7.3f", beg_min->key, end_max->key);
                ui->statusBar->showMessage(s);

                m_VSelectMap.ClearLine();
                m_VSelectMap2.ClearLine();
                m_HSelectMap2.ClearLine();
                m_VSelect.ClearLine();

                m_2Dintegral.actual = false;
                m_2Dintegral.value = 0.0;
                auto key = m_pointIdx * m_pointStep;
                if (m_HSelectMap.Exists()) {
                    auto start = m_HSelectMap.ItemLine().start;
                    auto end = m_HSelectMap.ItemLine().end;
                    double start_key = start->key();
                    //double start_val = start->value();
                    double end_key = end->key();
                    //double end_val = end->value();
                    m_VSelectMap.PlotLine(ui->customPlot, QPointF(start_key, beg_min->key), QPointF(start_key, end_max->key), QPen(Qt::red));
                    m_VSelectMap2.PlotLine(ui->customPlot, QPointF(end_key, beg_min->key), QPointF(end_key, end_max->key), QPen(Qt::red));
                    m_HSelectMap.ClearLine();
                    m_HSelectMap.PlotLine(ui->customPlot, QPointF(start_key, beg_min->key), QPointF(end_key, beg_min->key), QPen(Qt::red));
                    m_HSelectMap2.PlotLine(ui->customPlot, QPointF(start_key, end_max->key), QPointF(end_key, end_max->key), QPen(Qt::red));
                }
                else {
                    m_VSelectMap.PlotLine(ui->customPlot, QPointF(key, beg_min->key), QPointF(key, end_max->key), QPen(Qt::red));
                }
                m_VSelect.PlotLine(ui->customPlotH, QPointF(beg_min->value, beg_min->key), QPointF(beg_min->value, end_max->key), QPen(Qt::red));
                ui->customPlotH->replot();
                ui->customPlot->replot();
                if (m_integral.actual) {
                    //m_LabelResults.ClearLabelResults();
                    //m_LabelResults.FillLabelResults(ui->tableViewH_2, &m_integral, &m_2Dintegral);
                    //ui->customPlotH_2->replot();
                }
            }
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        ui->statusBar->showMessage("Ctrl key is pressed");
        if(ui->customPlot2->selectionRectMode() != QCP::SelectionRectMode::srmSelect) {
            ui->customPlot2->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
            ui->customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        }
        if (ui->customPlotH->selectionRectMode() != QCP::SelectionRectMode::srmSelect) {
            ui->customPlotH->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
            ui->customPlotH->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        }
    }
    else {
        ui->statusBar->showMessage("Press Escape key");
        if (event->key() == Qt::Key_Escape) {
            ClearLines();
            ClearResults();
            ui->customPlot2->replot();
            ui->customPlotH->replot();
            ui->customPlot->replot();
            //ui->customPlotH_2->replot();
        }
    }
}

void MainWindow::ClearLines()
{
    m_HSelect.ClearLine();
    m_HSelectMap.ClearLine();
    m_HSelectMap2.ClearLine();
    m_VSelect.ClearLine();
    m_VSelectMap.ClearLine();
    m_VSelectMap2.ClearLine();
    ui->customPlot2->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    ui->customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->customPlotH->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    ui->customPlotH->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void MainWindow::ClearResults()
{
    m_integral.actual = false;
    m_integral.value = 0.0;
    m_2Dintegral.actual = false;
    m_2Dintegral.value = 0.0;
    //m_LabelResults.ClearLabelResults();
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        ui->statusBar->showMessage("Ctrl key released");
        ui->customPlot2->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        ui->customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);// | QCP::iSelectPlottables);
        ui->customPlotH->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        ui->customPlotH->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);// | QCP::iSelectPlottables);
    }
}

void MainWindow::onCalculateIntegralAction()
{
    if (m_HSelect.Exists()) {
        CIntegralCalc integralCalc(m_HSelect.ItemLine(), m_pPlotData->LstSpecData()[m_spectrumIdx]);
        m_integral.value = integralCalc.Calculate();
        m_integral.actual = true;
        //m_LabelResults.FillLabelResults(ui->customPlotH_2, &m_integral, &m_2Dintegral);
        //ui->customPlotH_2->replot();
    }
    else {
        QMessageBox::warning(this, tr("SpectraPlot"), tr("Select horizontal calculation interval."));
    }
}

void MainWindow::onCalculateSummAction()
{
    if (m_HSelectMap.Exists() && m_VSelectMap.Exists() && m_VSelectMap2.Exists()) {
        C2DIntegralCalc integral2DCalc(m_VSelect.ItemLine(), m_HSelect.ItemLine(), m_pPlotData->LstSpecData(), LstSpecDataT(), &m_VPlotCursor, &m_HPlotCursor);
        auto ret = integral2DCalc.Calculate();
        m_pIntegralModel->addNewData(ret);

        double y = m_HSelect.ItemLine().start->value();
        double x = m_VSelect.ItemLine().start->key();

        QKeyEvent event(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
        keyPressEvent(&event);

        m_2Dintegral.value = ret.value;
        m_2Dintegral.actual = true;

        m_HSelect.PlotLine(ui->customPlot2, QPointF(ret.leftX, y), QPointF(ret.rightX, y), QPen(Qt::red));
        ui->customPlot2->replot();

        m_VSelect.PlotLine(ui->customPlotH, QPointF(x, ret.leftY), QPointF(x, ret.rightY), QPen(Qt::red));
        ui->customPlotH->replot();

        m_VSelectMap.PlotLine(ui->customPlot, QPointF(ret.leftX, ret.leftY), QPointF(ret.leftX, ret.rightY), QPen(Qt::red));
        m_VSelectMap2.PlotLine(ui->customPlot, QPointF(ret.rightX, ret.leftY), QPointF(ret.rightX, ret.rightY), QPen(Qt::red));

        m_HSelectMap.PlotLine(ui->customPlot, QPointF(ret.leftX, ret.leftY), QPointF(ret.rightX, ret.leftY), QPen(Qt::red));
        m_HSelectMap2.PlotLine(ui->customPlot, QPointF(ret.leftX, ret.rightY), QPointF(ret.rightX, ret.rightY), QPen(Qt::red));
        ui->customPlot->replot();
    }
    else {
        QMessageBox::warning(this, tr("SpectraPlot"), tr("Select horizontal and vertical calculation intervals."));
    }
}

void MainWindow::setProgressDlgValue(int value)
{
    if (m_pProgressDlg != nullptr) {
        m_pProgressDlg->setValue(value);
    }
}

void MainWindow::closeProgressDlg()
{
    if (m_pProgressDlg != nullptr) {
        m_pProgressDlg->close();
        delete m_pProgressDlg;
        m_pProgressDlg = nullptr;
    }
}

void MainWindow::progressDlgWasCanceled()
{
    if (m_pPlotData != nullptr) {
        m_pPlotData->CancelLoad();
    }
    if (m_pProgressDlg != nullptr) {
    }
}

void MainWindow::makeSpecDataT()
{
    if (m_pPlotData != nullptr) {
        clearSpecDataT();
        size_t nSpecs = m_pPlotData->LstSpecData().size();
        if (nSpecs > 0) {
            size_t size = m_pPlotData->LstSpecData().at(0).size();
            m_lstSpecDataT.resize(size);
            for (int i = 0; i < nSpecs; i++) {
                for (int j = 0; j < size; j++) {
                    auto v = m_pPlotData->LstSpecData()[i][j];
                    m_lstSpecDataT[j].push_back(v);
                }
            }
        }
    }
}

void MainWindow::clearSpecDataT()
{
    for (int i = 0; i < m_lstSpecDataT.size(); i++) {
        auto vec = m_lstSpecDataT[i];
        vec.clear();
    }
    m_lstSpecDataT.clear();
}











































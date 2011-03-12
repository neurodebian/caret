
/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <algorithm>
#include <iostream>
#include <limits>

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#ifdef HAVE_QWT
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>
#endif // HAVE_QWT

#include "GuiGraphWidget.h"
#include <QDoubleSpinBox>

#ifdef HAVE_QWT
/**
 * Constructor.
 */
GuiQwtGraph::GuiQwtGraph(QWidget* parent, 
                         const QString& title,
                         const char* /*name*/)
   : QwtPlot(parent)

{
   setTitle(title);

   scaleMinimumX = std::numeric_limits<double>::max();
   scaleMaximumX = -std::numeric_limits<double>::max();
   scaleMinimumY = std::numeric_limits<double>::max();
   scaleMaximumY = -std::numeric_limits<double>::max();
   
   //setAxisScale(xBottom, 0.0, 5000.0);
   //setAxisScale(yLeft, 0.0, 5000.0);

   minPeakMarker = NULL;
   maxPeakMarker = NULL;
   
   replot();
}

/**
 * destructor.
 */
GuiQwtGraph::~GuiQwtGraph()
{
}
      
/**
 * set the scale.
 */
void 
GuiQwtGraph::setScale(const double xMin, 
                      const double xMax,
                      const double yMin,
                      const double yMax)
{
   scaleMinimumX = xMin;
   scaleMaximumX = xMax,
   scaleMinimumY = yMin;
   scaleMaximumY = yMax;
   replot();
}
                    
/**
 * get the scale.
 */
void 
GuiQwtGraph::getScale(double& xMin, 
                      double& xMax,
                      double& yMin,
                      double& yMax)
{
   xMin = scaleMinimumX;
   xMax = scaleMaximumX;
   yMin = scaleMinimumY;
   yMax = scaleMaximumY;
}
                   
/**
 * set the minimum for the X-scale.
 */
void 
GuiQwtGraph::setScaleXMinimum(double val)
{
   scaleMinimumX = val;
   replot();
}

/**
 * set the maximum for the X-scale.
 */
void 
GuiQwtGraph::setScaleXMaximum(double val)
{
   scaleMaximumX = val;
   replot();
}

/**
 * set the minimum for the Y-scale.
 */
void 
GuiQwtGraph::setScaleYMinimum(double val)
{
   scaleMinimumY = val;
   replot();
}

/**
 * set the maximum for the Y-scale.
 */
void 
GuiQwtGraph::setScaleYMaximum(double val)
{
   scaleMaximumY = val;
   replot();
}
      
/**
 * replot the graph.
 */
void
GuiQwtGraph::replot()
{
   if (scaleMaximumX > scaleMinimumX) {
      setAxisScale(xBottom, scaleMinimumX, scaleMaximumX);
   }
   if (scaleMaximumY > scaleMinimumY) {
      setAxisScale(yLeft,   scaleMinimumY, scaleMaximumY);
   }
   
   if (topLegend.isEmpty() == false) setAxisTitle(xTop, topLegend);
   if (bottomLegend.isEmpty() == false) setAxisTitle(xBottom, bottomLegend);
   if (leftLegend.isEmpty() == false) setAxisTitle(yLeft, leftLegend);
   if (rightLegend.isEmpty() == false) setAxisTitle(yRight, rightLegend);
   
   QwtPlot::replot();
}
      
/**
 * set the minimum peak.
 */
void 
GuiQwtGraph::slotSetMinimumPeak(int val)
{
   if (minPeakMarker == NULL) {
       //
       // Use marker lines for min peak
       //
       minPeakMarker = new QwtPlotMarker();
       minPeakMarker->attach(this);
       //insertLineMarker("", xBottom);
       minPeakMarker->setLineStyle(QwtPlotMarker::VLine);
       minPeakMarker->setLinePen(QPen(Qt::blue, 3));
   }
   if (minPeakMarker != NULL) {
      minPeakMarker->show();
      minPeakMarker->setXValue(val);
   }
   else {
      minPeakMarker->hide();
   }
   replot();
}

/**
 * set the maximum peak.
 */
void 
GuiQwtGraph::slotSetMaximumPeak(int val)
{
   if (maxPeakMarker == NULL) {
      //
      // Use maker for max peak
      //
      maxPeakMarker = new QwtPlotMarker();
      maxPeakMarker->attach(this);
      maxPeakMarker->setLineStyle(QwtPlotMarker::VLine);
      maxPeakMarker->setLinePen(QPen(Qt::green, 3));
   }
   if (maxPeakMarker != NULL) {
      maxPeakMarker->show();
      maxPeakMarker->setXValue(val);
   }
   else {
      maxPeakMarker->hide();
   }
   replot();
}

/**
 * add data to the graph.  Returns 0 if resulting curve is invalid.
 */
QwtPlotCurve*
GuiQwtGraph::addData(const std::vector<double>& dataX, 
                     const std::vector<double>& dataY,
                     const QColor& color,
                     const DRAW_DATA_TYPE ddt)
{
   const unsigned int num = dataX.size();
   if (num > 0) {
      //
      // Create a new curve
      //
      QwtPlotCurve* newCurve = new QwtPlotCurve("histogram");
      newCurve->attach(this);
      
      //
      // Set the style of the curve
      //
      newCurve->setPen(QPen(color));
      switch (ddt) {
         case DRAW_DATA_TYPE_LINES:
            break;
         case DRAW_DATA_TYPE_BARS:
            newCurve->setBrush(QBrush(color, Qt::SolidPattern));    
            break;
      }

      //
      // Set the data for the curve
      //
      newCurve->setData(&dataX[0], &dataY[0], num);
       
      const double minValX = *std::min_element(dataX.begin(), dataX.end());
      const double maxValX = *std::max_element(dataX.begin(), dataX.end());
      //const double percentX = (maxValX - minValX) * 0.05;

      scaleMinimumX = std::min(scaleMinimumX, minValX); //(minValX - percentX));
      scaleMaximumX = std::max(scaleMaximumX, maxValX); //(maxValX + percentX));
      
      const double minValY = *std::min_element(dataY.begin(), dataY.end());
      const double maxValY = *std::max_element(dataY.begin(), dataY.end());
      //const double percentY = (maxValY - minValY) * 0.01;

      scaleMinimumY = std::min(scaleMinimumY, minValY); //(minValY - percentY));
      scaleMaximumY = std::max(scaleMaximumY, maxValY); //(maxValY + percentY));
      
      replot();
      
      return newCurve;
   }
   
   return 0;
}                   

/**
 * set the legends.
 */
void 
GuiQwtGraph::setLegends(const QString& topLegendIn,
                        const QString& bottomLegendIn,
                        const QString& leftLegendIn,
                        const QString& rightLegendIn)
{
   topLegend = topLegendIn;
   bottomLegend = bottomLegendIn;
   leftLegend = leftLegendIn;
   rightLegend = rightLegendIn;
}

#endif // HAVE_QWT
//=============================================================================

/**
 * constructor.
 */
GuiGraphWidget::GuiGraphWidget(QWidget* parent, 
                  const QString& title)
   : QWidget(parent)
{
#ifdef HAVE_QWT
   xDataMinimum = std::numeric_limits<double>::max();
   xDataMaximum = -std::numeric_limits<double>::max();
   
   QVBoxLayout* widgetLayout = new QVBoxLayout(this);
   
   //
   // Box for widget and scale control
   //
   QHBoxLayout* graphBoxLayout = new QHBoxLayout;
   graphBoxLayout->setSpacing(5);
   widgetLayout->addLayout(graphBoxLayout);
   
   //
   // The graph widget inside a group box
   //
   QGroupBox* graphGroupBox = new QGroupBox(title);
   graphBoxLayout->addWidget(graphGroupBox);
   graphBoxLayout->setStretchFactor(graphGroupBox, 1000);
   QVBoxLayout* graphGroupBoxLayout = new QVBoxLayout(graphGroupBox);
   graphWidget = new GuiQwtGraph(graphGroupBox, "");
   graphGroupBoxLayout->addWidget(graphWidget);
   
   //
   // Picker for when graph is clicked with mouse
   //
   QwtPlotPicker* plotPicker = new QwtPlotPicker(QwtPlot::xBottom, 
                                                 QwtPlot::yLeft, 
                                                 graphWidget->canvas());
   plotPicker->setSelectionFlags(QwtPicker::PointSelection |
                                 QwtPicker::ClickSelection);
   QObject::connect(plotPicker, SIGNAL(selected(const QwtDoublePoint&)),
                     this, SLOT(slotPointPicked(const QwtDoublePoint&)));
                     
   //
   // Vertical box for scale control and mouse coordinates
   //
   QVBoxLayout* scaleAndMouseVBoxLayout = new QVBoxLayout;
   graphBoxLayout->addLayout(scaleAndMouseVBoxLayout);
   graphBoxLayout->setStretchFactor(scaleAndMouseVBoxLayout, 0);
   
   //
   // Axis controls
   //
   QGroupBox* axisGroupBox = new QGroupBox("Scale Control");
   QVBoxLayout* axisGroupBoxLayout = new QVBoxLayout(axisGroupBox);
   scaleAndMouseVBoxLayout->addWidget(axisGroupBox);
   QGridLayout* axisGridLayout = new QGridLayout;
   axisGroupBoxLayout->addLayout(axisGridLayout);
   
   const int minSpinWidth = 120;
   
   //
   // Y-Max
   //
   axisGridLayout->addWidget(new QLabel("Y-Max "), 0, 0);
   yMaximumDoubleSpinBox = new QDoubleSpinBox;
   yMaximumDoubleSpinBox->setMinimum(-1.0);
   yMaximumDoubleSpinBox->setMaximum(1.0);
   yMaximumDoubleSpinBox->setSingleStep(1.0);
   yMaximumDoubleSpinBox->setDecimals(6);
   axisGridLayout->addWidget(yMaximumDoubleSpinBox, 0, 1);
   yMaximumDoubleSpinBox->setMinimumWidth(minSpinWidth);
   QObject::connect(yMaximumDoubleSpinBox, SIGNAL(valueChanged(double)),
                    graphWidget, SLOT(setScaleYMaximum(double)));
   
   //
   // Y-Min
   //
   axisGridLayout->addWidget(new QLabel("Y-Min "), 1, 0);
   yMinimumDoubleSpinBox = new QDoubleSpinBox;
   yMinimumDoubleSpinBox->setMinimum(-1.0);
   yMinimumDoubleSpinBox->setMaximum(1.0);
   yMinimumDoubleSpinBox->setSingleStep(1.0);
   yMinimumDoubleSpinBox->setDecimals(6);
   axisGridLayout->addWidget(yMinimumDoubleSpinBox, 1, 1);
   yMinimumDoubleSpinBox->setMinimumWidth(minSpinWidth);
   QObject::connect(yMinimumDoubleSpinBox, SIGNAL(valueChanged(double)),
                    graphWidget, SLOT(setScaleYMinimum(double)));
   
   //
   // X-Max
   //
   axisGridLayout->addWidget(new QLabel("X-Max "), 2, 0);
   xMaximumDoubleSpinBox = new QDoubleSpinBox;
   xMaximumDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   xMaximumDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   xMaximumDoubleSpinBox->setSingleStep(1.0);
   xMaximumDoubleSpinBox->setDecimals(6);
   axisGridLayout->addWidget(xMaximumDoubleSpinBox, 2, 1);
   xMaximumDoubleSpinBox->setMinimumWidth(minSpinWidth);
   QObject::connect(xMaximumDoubleSpinBox, SIGNAL(valueChanged(double)),
                    graphWidget, SLOT(setScaleXMaximum(double)));
   
   //
   // X-Min
   //
   axisGridLayout->addWidget(new QLabel("X-Min "), 3, 0);
   xMinimumDoubleSpinBox = new QDoubleSpinBox;
   xMinimumDoubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
   xMinimumDoubleSpinBox->setMaximum(std::numeric_limits<double>::max());
   xMinimumDoubleSpinBox->setSingleStep(1.0);
   xMinimumDoubleSpinBox->setDecimals(6);
   axisGridLayout->addWidget(xMinimumDoubleSpinBox, 3, 1);
   xMinimumDoubleSpinBox->setMinimumWidth(minSpinWidth);
   QObject::connect(xMinimumDoubleSpinBox, SIGNAL(valueChanged(double)),
                    graphWidget, SLOT(setScaleXMinimum(double)));
   
   //
   // Apply push button
   //
   QPushButton* applyPushButton = new QPushButton("Apply");
   axisGroupBoxLayout->addWidget(applyPushButton);
   applyPushButton->setAutoDefault(false);
   QObject::connect(applyPushButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyScalePushButton()));
                    
   //
   // Reset push button
   //
   QPushButton* resetPushButton = new QPushButton("Reset");
   axisGroupBoxLayout->addWidget(resetPushButton);
   resetPushButton->setAutoDefault(false);
   QObject::connect(resetPushButton, SIGNAL(clicked()),
                    this, SLOT(slotResetScalePushButton()));
                    
   axisGroupBox->setFixedHeight(axisGroupBox->sizeHint().height());
   

   //
   // X and Y pick label
   //
   QGroupBox* labelGroupBox = new QGroupBox("Mouse Coordinates");
   QVBoxLayout* labelGroupBoxLayout = new QVBoxLayout(labelGroupBox);
   scaleAndMouseVBoxLayout->addWidget(labelGroupBox);
   pickXLabel = new QLabel("Click mouse on graph");
   labelGroupBoxLayout->addWidget(pickXLabel);
   pickYLabel = new QLabel("to get coordinates.");
   labelGroupBoxLayout->addWidget(pickYLabel);
   labelGroupBox->setFixedHeight(labelGroupBox->sizeHint().height());
#else  // HAVE_QWT
   //
   // Warning Label
   //
   QLabel* warningLabel = new QLabel(
      "<HTML><B>Graphs are not available.<br>"
      "Caret was built without the QWT Graph Library.</B></HTML>");

   QVBoxLayout* widgetLayout = new QVBoxLayout(this);
   widgetLayout->addWidget(warningLabel);
#endif // HAVE_QWT
}

/**
 * destructor.
 */
GuiGraphWidget::~GuiGraphWidget()
{
}

/**
 * apply scale push button.
 */
void 
GuiGraphWidget::slotApplyScalePushButton()
{
#ifdef HAVE_QWT
   graphWidget->setScale(xMinimumDoubleSpinBox->value(),
                         xMaximumDoubleSpinBox->value(),
                         yMinimumDoubleSpinBox->value(),
                         yMaximumDoubleSpinBox->value());
#endif HAVE_QWT
}
      
/**
 * reset scale push button.
 */
void 
GuiGraphWidget::slotResetScalePushButton()
{
   xMinimumDoubleSpinBox->setValue(xDataMinimum); // xMinimumDoubleSpinBox->minimum());
   xMaximumDoubleSpinBox->setValue(xDataMaximum); // xMaximumDoubleSpinBox->maximum());
   yMinimumDoubleSpinBox->setValue(yMinimumDoubleSpinBox->minimum());
   yMaximumDoubleSpinBox->setValue(yMaximumDoubleSpinBox->maximum());
}

/**
 * get the graph min/max.
 */
void 
GuiGraphWidget::getGraphMinMax(double& xMin, 
                               double& xMax,
                               double& yMin,
                               double& yMax)
{
#ifdef HAVE_QWT
   graphWidget->getScale(xMin, xMax, yMin, yMax);
#endif
}
             
/**
 * set the graph min/max.
 */
void 
GuiGraphWidget::setGraphMinMax(const double xMin, 
                               const double xMax,
                               const double yMin,
                               const double yMax)
{
#ifdef HAVE_QWT
   xMinimumDoubleSpinBox->setValue(xMin);
   xMaximumDoubleSpinBox->setValue(xMax);
   yMinimumDoubleSpinBox->setValue(yMin);
   yMaximumDoubleSpinBox->setValue(yMax);
   slotApplyScalePushButton();
#endif  // HAVE_QWT
}
                   
/**
 * add data to the graph widget.
 */
int 
GuiGraphWidget::addData(const std::vector<double>& dataX, 
                           const std::vector<double>& dataY,
                           const QColor& color,
                           const DRAW_DATA_TYPE ddtIn)
{
#ifdef HAVE_QWT
   GuiQwtGraph::DRAW_DATA_TYPE ddt = GuiQwtGraph::DRAW_DATA_TYPE_LINES;
   switch (ddtIn) {
      case DRAW_DATA_TYPE_LINES:
         ddt = GuiQwtGraph::DRAW_DATA_TYPE_LINES;
         break;
      case DRAW_DATA_TYPE_BARS:
         ddt = GuiQwtGraph::DRAW_DATA_TYPE_BARS;
         break;
   }
   
   QwtPlotCurve* curve = graphWidget->addData(dataX, dataY, color, ddt);
   if (curve > 0) {
      curvesInGraphWidget.push_back(curve);

      xDataMinimum = std::min(xDataMinimum, *std::min_element(dataX.begin(), dataX.end()));
      xDataMaximum = std::max(xDataMaximum, *std::max_element(dataX.begin(), dataX.end()));
      xMinimumDoubleSpinBox->setValue(xDataMinimum);
      xMaximumDoubleSpinBox->setValue(xDataMaximum);
/*
      double minX = *std::min_element(dataX.begin(), dataX.end());
      double maxX = *std::max_element(dataX.begin(), dataX.end());
      if (curvesInGraphWidget.size() > 1) {
         minX = std::min(minX, xMinimumDoubleSpinBox->minimum());
         maxX = std::max(maxX, xMinimumDoubleSpinBox->maximum());
      }
      
      xMinimumDoubleSpinBox->setMinimum(minX);
      xMinimumDoubleSpinBox->setMaximum(maxX);
      xMaximumDoubleSpinBox->setMinimum(minX);
      xMaximumDoubleSpinBox->setMaximum(maxX);
      xMinimumDoubleSpinBox->setValue(minX);
      xMaximumDoubleSpinBox->setValue(maxX);
*/
      double minY = *std::min_element(dataY.begin(), dataY.end());
      double maxY = *std::max_element(dataY.begin(), dataY.end());
      if (curvesInGraphWidget.size() > 1) {
         minY = std::min(minY, yMinimumDoubleSpinBox->minimum());
         maxY = std::max(maxY, yMinimumDoubleSpinBox->maximum());
      }
      yMinimumDoubleSpinBox->setMinimum(minY);
      yMinimumDoubleSpinBox->setMaximum(maxY);
      yMaximumDoubleSpinBox->setMinimum(minY);
      yMaximumDoubleSpinBox->setMaximum(maxY);
      yMinimumDoubleSpinBox->setValue(minY);
      yMaximumDoubleSpinBox->setValue(maxY);

      return curvesInGraphWidget.size() - 1;
   }
#endif // HAVE_QWT
   return -1;
}
  
/**
 * called when a point is picked.
 */
#ifdef HAVE_QWT
void 
GuiGraphWidget::slotPointPicked(const QwtDoublePoint& dp)
{
   pickXLabel->setText("X: " + QString::number(dp.x(), 'f', 6));
   pickYLabel->setText("Y: " + QString::number(dp.y(), 'f', 6));
}
#endif // HAVE_QWT

/**
 * set the minimum peak.
 */
void 
GuiGraphWidget::slotSetMinimumPeak(int val)
{
#ifdef HAVE_QWT
   graphWidget->slotSetMinimumPeak(val);
#endif  // HAVE_QWT
}

/**
 * set the maximum peak.
 */
void 
GuiGraphWidget::slotSetMaximumPeak(int val)
{
#ifdef HAVE_QWT
   graphWidget->slotSetMaximumPeak(val);
#endif  // HAVE_QWT
}

/**
 * set the minimum for the X-scale.
 */
void 
GuiGraphWidget::setScaleXMinimum(double val)
{
#ifdef HAVE_QWT
   xMinimumDoubleSpinBox->setValue(val);
   graphWidget->setScaleXMinimum(val);
#endif  // HAVE_QWT
}

/**
 * set the maximum for the X-scale.
 */
void 
GuiGraphWidget::setScaleXMaximum(double val)
{
#ifdef HAVE_QWT
   xMaximumDoubleSpinBox->setValue(val);
   graphWidget->setScaleXMaximum(val);
#endif  // HAVE_QWT
}

/**
 * set the minimum for the Y-scale.
 */
void 
GuiGraphWidget::setScaleYMinimum(double val)
{
#ifdef HAVE_QWT
   yMinimumDoubleSpinBox->setValue(val);
   graphWidget->setScaleYMinimum(val);
#endif  // HAVE_QWT
}

/**
 * set the maximum for the Y-scale.
 */
void 
GuiGraphWidget::setScaleYMaximum(double val)
{
#ifdef HAVE_QWT
   yMaximumDoubleSpinBox->setValue(val);
   graphWidget->setScaleYMaximum(val);
#endif  // HAVE_QWT
}

/**
 * enable/disable display of data in graph widget.
 */
void 
GuiGraphWidget::setDataDisplayed(const int dataIndex,
                                    const bool displayIt)
{
#ifdef HAVE_QWT
   if (dataIndex < static_cast<int>(curvesInGraphWidget.size())) {
      if (displayIt) {
         curvesInGraphWidget[dataIndex]->setStyle(QwtPlotCurve::Lines);
      }
      else {
         curvesInGraphWidget[dataIndex]->setStyle(QwtPlotCurve::NoCurve);
      }
      graphWidget->replot();
   }
#endif  // HAVE_QWT
}                            

/**
 * remove data .
 */
void 
GuiGraphWidget::removeData(const int dataNumber)
{
#ifdef HAVE_QWT
   if ((dataNumber >= 0) && (dataNumber < static_cast<int>(curvesInGraphWidget.size()))) {
      //graphWidget->removeCurve(curvesInGraphWidget[dataNumber]);
      curvesInGraphWidget[dataNumber]->detach();
   }
#endif  // HAVE_QWT
}

/**
 * remove all data from graph.
 */
void 
GuiGraphWidget::removeAllData()
{
#ifdef HAVE_QWT
   for (int i = 0; i < static_cast<int>(curvesInGraphWidget.size()); i++) {
      curvesInGraphWidget[i]->detach();
   }
#endif  // HAVE_QWT
}
      
/**
 * set the legends.
 */
void 
GuiGraphWidget::setLegends(const QString& topLegend,
                           const QString& bottomLegend,
                           const QString& leftLegend,
                           const QString& rightLegend)
{
#ifdef HAVE_QWT
   graphWidget->setLegends(topLegend, bottomLegend, leftLegend, rightLegend);
#endif  // HAVE_QWT
}


#ifndef __GUI_GRAPH_WIDGET_H__
#define __GUI_GRAPH_WIDGET_H__

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

#include <vector>

#include <QWidget>

#ifdef HAVE_QWT
#include <qwt_plot.h>
#endif // HAVE_QWT
#include <QLabel>

class QDoubleSpinBox;
class QwtPlotCurve;
class QwtPlotMarker;

#ifdef HAVE_QWT
/// graph based upon Qwt Graph
class GuiQwtGraph : public QwtPlot
{
   Q_OBJECT

   public:
      /// how to draw data
      enum DRAW_DATA_TYPE {
         /// draw data as lines
         DRAW_DATA_TYPE_LINES,
         
         /// draw data as bars
         DRAW_DATA_TYPE_BARS
      };
      
      // constructor
      GuiQwtGraph(QWidget* parent, 
                  const QString& title,
                  const char* name = 0);

      // destructor
      ~GuiQwtGraph();
      
      // add data to the graph (returns key of curve)
      QwtPlotCurve* addData(const std::vector<double>& dataX, 
                   const std::vector<double>& dataY,
                   const QColor& color,
                   const DRAW_DATA_TYPE ddt);
                   
      // set the minimum peak
      void slotSetMinimumPeak(int val);
      
      // set the maximum peak
      void slotSetMaximumPeak(int val);

      // replot the graph
      virtual void replot();
      
      // get the scale
      void getScale(double& xMin, 
                    double& xMax,
                    double& yMin,
                    double& yMax);
                   
      // set the scale
      void setScale(const double xMin, 
                    const double xMax,
                    const double yMin,
                    const double yMax);
                   
      // set the legends.
      void setLegends(const QString& topLegendIn,
                      const QString& bottomLegendIn,
                      const QString& leftLegendIn,
                      const QString& rightLegendIn);
                      
   public slots:
      // set the minimum for the X-scale
      void setScaleXMinimum(double val);
      
      // set the maximum for the X-scale
      void setScaleXMaximum(double val);
      
      // set the minimum for the Y-scale
      void setScaleYMinimum(double val);
      
      // set the maximum for the Y-scale
      void setScaleYMaximum(double val);
      
   protected:
      /// marker for minimum peak
      QwtPlotMarker* minPeakMarker;
      
      /// marker for maximum peak
      QwtPlotMarker* maxPeakMarker;
      
      /// the scale minimum for Y
      double scaleMinimumX;
      
      /// the scale maximum for Y
      double scaleMaximumX;
      
      /// the scale minimum for Y
      double scaleMinimumY;
      
      /// the scale maximum for Y
      double scaleMaximumY;      
      
      /// legend
      QString topLegend;
      
      /// legend
      QString bottomLegend;
      
      /// legend
      QString leftLegend;
      
      /// legend
      QString rightLegend;
                        
};
#endif // HAVE_QWT

/// the graph widget
class GuiGraphWidget : public QWidget {
   Q_OBJECT
   
   public:
      /// how to draw data
      enum DRAW_DATA_TYPE {
         /// draw data as lines
         DRAW_DATA_TYPE_LINES,
         
         /// draw data as bars
         DRAW_DATA_TYPE_BARS
      };
      
      // constructor
      GuiGraphWidget(QWidget* parent, 
                        const QString& title);
      
      // destructor
      ~GuiGraphWidget();
      
      // add data to the graph widget (returns -1 if invalid data else index of curve)
      int addData(const std::vector<double>& dataX, 
                   const std::vector<double>& dataY,
                   const QColor& color,
                   const DRAW_DATA_TYPE ddt);
        
      // enable/disable display of data in graph widget
      void setDataDisplayed(const int dataIndex,
                            const bool displayIt);
          
      // remove data 
      void removeData(const int dataNumber);
      
      // remove all data from graph
      void removeAllData();
      
      // get the graph min/max
      void getGraphMinMax(double& xMin, 
                          double& xMax,
                          double& yMin,
                          double& yMax);
                   
      // set the graph min/max
      void setGraphMinMax(const double xMin, 
                          const double xMax,
                          const double yMin,
                          const double yMax);
                   
      // set the legends
      void setLegends(const QString& topLegend,
                      const QString& bottomLegend,
                      const QString& leftLegend,
                      const QString& rightLegend);
   public slots:
      // set the minimum peak
      void slotSetMinimumPeak(int val);
      
      // set the maximum peak
      void slotSetMaximumPeak(int val);
      
      // set the minimum for the X-scale
      void setScaleXMinimum(double val);
      
      // set the maximum for the X-scale
      void setScaleXMaximum(double val);
      
      // set the minimum for the Y-scale
      void setScaleYMinimum(double val);
      
      // set the maximum for the Y-scale
      void setScaleYMaximum(double val);
      
   protected slots:
#ifdef HAVE_QWT
      // called when a point is picked
      void slotPointPicked(const QwtDoublePoint&);
#endif // HAVE_QWT

      // apply scale push button
      void slotApplyScalePushButton();
      
      // reset scale push button
      void slotResetScalePushButton();
      
   protected:
#ifdef HAVE_QWT
      /// the graph
      GuiQwtGraph* graphWidget;
#endif // HAVE_QWT

      /// pick label showing X coordinates
      QLabel* pickXLabel;
      
      /// pick label showing Y coordinates
      QLabel* pickYLabel;
      
#ifdef HAVE_QWT
      /// the curves in the graph widget
      std::vector<QwtPlotCurve*> curvesInGraphWidget;
#endif // HAVE_QWT

      /// x-minimum float spin box
      QDoubleSpinBox* xMinimumDoubleSpinBox;
      
      /// x-maximum float spin box
      QDoubleSpinBox* xMaximumDoubleSpinBox;
      
      /// x-minimum float spin box
      QDoubleSpinBox* yMinimumDoubleSpinBox;
      
      /// y-maximum float spin box
      QDoubleSpinBox* yMaximumDoubleSpinBox;
      
      /// x-data minimum value
      double xDataMinimum;
      
      /// x-data maximum value
      double xDataMaximum;
};


#endif // __GUI_GRAPH_WIDGET_H__


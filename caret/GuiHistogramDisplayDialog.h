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

#ifndef __GUI_HISTOGRAM_DISPLAY_DIALOG_H__
#define __GUI_HISTOGRAM_DISPLAY_DIALOG_H__

#include <vector>

#include "WuQDialog.h"

class GuiGraphWidget;
class StatisticHistogram;
class QComboBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QSlider;

/// Dialog for displaying a histogram
class GuiHistogramDisplayDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor (dialog deleted when closed if non-modal)
      GuiHistogramDisplayDialog(QWidget* parent, 
                         const QString& titleCaption,
                         const std::vector<float>& values,
                         const bool showGrayWhitePeaks,
                         Qt::WindowFlags f = 0);
      
      /// Destructor
      ~GuiHistogramDisplayDialog();
      
      /// get the peaks if "CHOOSE_PEAKS" mode
      void getPeaks(float& minPeak, float& maxPeak);
      
   private slots:
      /// called when apply hit (only if non-modal)
      void slotApply();
      
      /// called when display mode changed
      void slotDisplayModeComboBox(int value);
      
      ///Called when non-modal dialog is closed.
      void slotCloseButton();

      /// called when show/hide stats push button is pressed
      void slotShowHideStatsPushButton();
      
      /// called when smooth histogram button pressed
      void slotSmoothPushButton();
      
   protected:
      enum HISTO_SELECTION {
         HISTO_SELECTION_ALL,
         HISTO_SELECTION_MIDDLE_96
      };
      
      /// called when dialog OK/Cancel used if modal
      void done(int r);
      
      /// load the data into graph widget
      void loadDataIntoGraph(const HISTO_SELECTION selectedHistogram);
      
      /// load gray/white peak information
      void loadGrayWhitePeakInfo(const HISTO_SELECTION selectedHistogram);
      
      /// current histogram selection
      HISTO_SELECTION histogramSelection;
      
      /// the histogram widget
      GuiGraphWidget* histogramWidget;
      
      /// combo box for selecting all or middle 96%
      QComboBox* displayModeComboBox;
      
      /// group box for stats
      QGroupBox* statsGroupBox;
      
      /// show/hide stats push button
      QPushButton* showHideStatsPushButton;
      
      /// label
      QLabel* meanLabel;
      
      /// label
      QLabel* mean96Label;
      
      /// label
      QLabel* stdDevLabel;
      
      /// label
      QLabel* stdDev96Label;
      
      /// label
      QLabel* minLabel;
      
      /// label
      QLabel* min96Label;
      
      /// label
      QLabel* maxLabel;
      
      /// label
      QLabel* max96Label;
      
      /// label
      QLabel* rangeLabel;
      
      /// label
      QLabel* range96Label;
      
      /// label
      QLabel* csfPeakLabel;
      
      /// label
      QLabel* grayMinLabel;
      
      /// label
      QLabel* grayPeakLabel;
      
      /// label
      QLabel* grayWhiteLabel;
      
      /// label
      QLabel* whitePeakLabel;
      
      /// label
      QLabel* whiteMaxLabel;
      
      /// widget containing the histogram
      //QWidget* histogramLayoutWidget;
      
      /// the normal histogram
      StatisticHistogram* histogram;
      
      /// the 2% to 98% histogram
      StatisticHistogram* histogram96;
      
      /// index of histogram in graph
      int histogramGraphIndex;
      
      /// index of histogram 96% in graph
      int histogram96GraphIndex;
};

#endif // __GUI_HISTOGRAM_DISPLAY_DIALOG_H__



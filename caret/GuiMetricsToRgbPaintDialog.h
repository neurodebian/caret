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


#ifndef __GUI_METRICS_TO_RGB_PAINT_DIALOG_H__
#define __GUI_METRICS_TO_RGB_PAINT_DIALOG_H__

#include "WuQDialog.h"

class GuiNodeAttributeColumnSelectionComboBox;
class QLineEdit;

/// This dialog is used to enter parameters for converting a metric file to RGB paint
class GuiMetricsToRgbPaintDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMetricsToRgbPaintDialog(QWidget* parent);
      
      /// Destructor
      ~GuiMetricsToRgbPaintDialog();
      
      /// Update the dialog if metrics or rgb paint change
      void updateDialog();
      
   private slots:
      /// called when an Rgb paint column is selected
      void slotRgbColumnSelected(int column);
      
      /// Called when apply button pressed.
      void slotApplyButton();
      
      /// Called when red metric column is selected
      void slotRedMetricSelected(int item);
      
      /// Called when green metric column is selected
      void slotGreenMetricSelected(int item);
      
      /// Called when blue metric column is selected
      void slotBlueMetricSelected(int item);
      
      /// Called when red threshold column is selected
      void slotRedThresholdSelected(int item);
      
      /// Called when green threshold column is selected
      void slotGreenThresholdSelected(int item);
      
      /// Called when blue threshold column is selected
      void slotBlueThresholdSelected(int item);
      
   private:
      /// inhibit update of dislay
      bool inhibitUpdate;
      
      /// Create the metric section
      QWidget* createMetricSection();
      
      /// Create the threshold section
      QWidget* createThresholdSection();
      
      /// Create the rgb paint section
      QWidget* createRgbSection();
      
      /// load metric line edits
      void loadMetricMinMaxLineEdits(const int metricColumn,
                                     QLineEdit* minLineEdit,
                                     QLineEdit* maxLineEdit);
                                     
      /// line edit for comment
      QLineEdit* columnCommentLineEdit;
      
      ///  column name line edit
      QLineEdit* columnNameLineEdit;
      
      /// rgb paint column selection;
      GuiNodeAttributeColumnSelectionComboBox* rgbColumnComboBox;
      
      /// red metric combo box
      GuiNodeAttributeColumnSelectionComboBox* redMetricComboBox;
      
      /// green metric combo box
      GuiNodeAttributeColumnSelectionComboBox* greenMetricComboBox;
      
      /// blue metric combo box
      GuiNodeAttributeColumnSelectionComboBox* blueMetricComboBox;
      
      // red metric neg max line edit
      QLineEdit* redMetricNegMaxLineEdit;
      
      // green metric neg max line edit
      QLineEdit* greenMetricNegMaxLineEdit;
      
      // blue metric neg max line edit
      QLineEdit* blueMetricNegMaxLineEdit;
      
      // red metric pos max line edit
      QLineEdit* redMetricPosMaxLineEdit;
      
      // green metric pos max line edit
      QLineEdit* greenMetricPosMaxLineEdit;
      
      // blue metric pos max line edit
      QLineEdit* blueMetricPosMaxLineEdit;
      
      /// red threshold combo box
      GuiNodeAttributeColumnSelectionComboBox* redThresholdComboBox;
      
      /// green threshold combo box
      GuiNodeAttributeColumnSelectionComboBox* greenThresholdComboBox;
      
      /// blue threshold combo box
      GuiNodeAttributeColumnSelectionComboBox* blueThresholdComboBox;
      
      // red threshold neg max line edit
      QLineEdit* redThresholdNegLineEdit;
      
      // green threshold neg max line edit
      QLineEdit* greenThresholdNegLineEdit;
      
      // blue threshold neg max line edit
      QLineEdit* blueThresholdNegLineEdit;
      
      // red threshold pos max line edit
      QLineEdit* redThresholdPosLineEdit;
      
      // green threshold pos max line edit
      QLineEdit* greenThresholdPosLineEdit;
      
      // blue threshold pos max line edit
      QLineEdit* blueThresholdPosLineEdit;
      
};

#endif  // __GUI_METRICS_TO_RGB_PAINT_DIALOG_H__


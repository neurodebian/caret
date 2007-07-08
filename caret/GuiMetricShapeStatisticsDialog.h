
#ifndef __GUI_METRIC_SHAPE_STATISTICS_DIALOG_H__
#define __GUI_METRIC_SHAPE_STATISTICS_DIALOG_H__

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

#include "QtDialog.h"

class QCheckBox;
class QGroupBox;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;

/// dialog for metric and shape statistical operations
class GuiMetricShapeStatisticsDialog : public QtDialog {
   Q_OBJECT
   
   public:      
      /// dialog mode
      enum DIALOG_MODE {
         /// computing Levene-map
         DIALOG_MODE_LEVENE,
         /// computing Z-map
         DIALOG_MODE_ZMAP,
         /// computing T-map
         DIALOG_MODE_TMAP,
         /// computing subtract group average
         DIALOG_MODE_SUBTRACT_GROUP_AVERAGE,
         /// computeing shuffled columns t-map
         DIALOG_MODE_SHUFFLED_TMAP,
         /// normalize all columns
         DIALOG_MODE_NORMALIZE,
         /// shuffle cross-correlation maps
         DIALOG_MODE_SHUFF_CROSS_CORRELATION,
         /// root means square
         DIALOG_MODE_ROOT_MEAN_SQUARE
      };
      
      // constructor
      GuiMetricShapeStatisticsDialog(QWidget* parent, 
                                     const DIALOG_MODE dialogModeIn);
      
      // destructor
      ~GuiMetricShapeStatisticsDialog();
      
   protected slots:
      // called when output name 1 push button pressed
      void slotOutputFile1PushButton();
      
      // called when output name 2 push button pressed
      void slotOutputFile2PushButton();
      
      // called when input name 1 push button pressed
      void slotInputFile1PushButton();
      
      // called when input name 2 push button pressed
      void slotInputFile2PushButton();
      
      // called when help button pressed
      void slotHelpButton();
      
   protected:
      // called when OK or Cancel button pressed
      void done(int r);
      
      /// get a file using a file selection dialog
      QString getFile(const QString& caption,
                      const bool fileMustExist);
      
      /// dialog mode
      DIALOG_MODE dialogMode;
      
      /// output name 1 line edit
      QLineEdit* outputName1LineEdit;
      
      /// output name 2 line edit
      QLineEdit* outputName2LineEdit;
      
      /// input name 1 line edit
      QLineEdit* inputName1LineEdit;
      
      /// input name 2 line edit
      QLineEdit* inputName2LineEdit;
      
      /// repetitions spin box
      QSpinBox* repetitionsSpinBox;
      
      /// mean float spin box
      QDoubleSpinBox* meanDoubleSpinBox;
      
      /// deviation float spin box
      QDoubleSpinBox* deviationDoubleSpinBox;   
      
      /// size of group 1 spin box
      QSpinBox* sizeOfGroup1SpinBox;
      
      /// tmap degrees of freedom check box
      QCheckBox* tMapDOFCheckBox;
      
      /// tmap p-value check box
      QCheckBox* tMapPValueCheckBox;
      
      /// group box for variance smoothing options
      QGroupBox* varianceSmoothingGroupBox;
      
      /// variance smoothing topology file line edit
      QLineEdit* varianceSmoothingTopologyFileLineEdit;
      
      /// variance smoothing iterations spin box
      QSpinBox* varianceSmoothingIterationsSpinBox;
      
      /// variance smoothing strength spin box
      QDoubleSpinBox* varianceSmoothingStrengthSpinBox;
      
      /// false discovery rate q-value spin box
      QDoubleSpinBox* falseDiscoveryRateQValueDoubleSpinBox;
      
      /// false discovery rate group box
      QGroupBox* falseDiscoveryRateGroupBox;
      
      /// the help page for the current mode
      QString helpPage;
};

#endif // __GUI_METRIC_SHAPE_STATISTICS_DIALOG_H__


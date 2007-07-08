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


#ifndef __GUI_PREFERENCES_DIALOG_H__
#define __GUI_PREFERENCES_DIALOG_H__

#include "QtDialog.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QSpinBox;

/// Dialog used to set user preferences
class GuiPreferencesDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiPreferencesDialog(QWidget* parent);
      
      /// Destructor
      ~GuiPreferencesDialog();
      
   private slots:
      /// called when apply button is pressed
      void applyButtonSlot();
      
      /// called by reset display lists push button
      void slotResetDisplayListsPushButton();
      
   private:
      /// load preferences into the dialog
      void loadPreferences();
      
      /// surface background color red spin box
      QSpinBox* surfaceBackgroundRedSpinBox;
      
      /// surface background color green spin box
      QSpinBox* surfaceBackgroundGreenSpinBox;
      
      /// surface background color blue spin box
      QSpinBox* surfaceBackgroundBlueSpinBox;
      
      /// surface foreground color red spin box
      QSpinBox* surfaceForegroundRedSpinBox;
      
      /// surface foreground color green spin box
      QSpinBox* surfaceForegroundGreenSpinBox;
      
      /// surface foreground color blue spin box
      QSpinBox* surfaceForegroundBlueSpinBox;
      
      /// light position X line edit
      QLineEdit* lightPositionLineEditX;
      
      /// light position Y line edit
      QLineEdit* lightPositionLineEditY;
      
      /// light position Z line edit
      QLineEdit* lightPositionLineEditZ;
      
      /// mouse speed line edit
      QLineEdit* mouseSpeedLineEdit;
      
      /// iterative update check box
      QSpinBox* iterUpdateSpinBox;
      
      /// debug on checkbox
      QCheckBox* debugOnCheckBox;
      
      /// debug node number
      QSpinBox* debugNodeSpinBox;
      
      /// web browser line edit
      QLineEdit* webBrowserLineEdit;
      
      /// image capture type combo box
      QComboBox* imageCaptureTypeComboBox;
      
      /// number of threads spin box
      QSpinBox* numberOfThreadsSpinBox;
      
      /// speech enabled check box
      QComboBox* speechComboBox;
      
      /// floating point precision spin box
      QSpinBox* floatPrecisionSpinBox;
      
      /// file write data type combo box
      QComboBox* fileWriteDataTypeComboBox;
      
      /// SuMS hosts line edit
      QLineEdit* sumsHostsLineEdit;
      
      /// display lists enabled check box
      QCheckBox* displayListsOnCheckBox;
      
      /// random seed check box
      QCheckBox* randomSeedCheckBox;
      
      /// random seed spin box
      QSpinBox* randomSeedSpinBox;
      
      /// loading preferences flag
      bool loadingPreferencesFile;
};

#endif


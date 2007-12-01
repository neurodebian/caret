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

#include <vector>

#include "QtDialog.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;
class WuQWidgetGroup;

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
      
      // called to set background color
      void slotSetBackgroundColor();
      
      // called to set foreground color
      void slotSetForegroundColor();
      
   private:
      // load preferences into the dialog
      void loadPreferences();
      
      // create the debug section
      QWidget* createDebugSection();
      
      // create the file section
      QWidget* createFileSection();
      
      // create the misc section
      QWidget* createMiscSection();
      
      // create the opengl section
      QWidget* createOpenglSection();
      
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
      
      /// light position X double spin box
      QDoubleSpinBox* lightPositionXDoubleSpinBox;
      
      /// light position Y double spin box
      QDoubleSpinBox* lightPositionYDoubleSpinBox;
      
      /// light position Z double spin box
      QDoubleSpinBox* lightPositionZDoubleSpinBox;
      
      /// mouse speed double spin box
      QDoubleSpinBox* mouseSpeedDoubleSpinBox;
      
      /// iterative update check box
      QSpinBox* iterUpdateSpinBox;
      
      /// debug on checkbox
      QCheckBox* debugOnCheckBox;
      
      /// debug node number
      QSpinBox* debugNodeSpinBox;
      
      /// test flag 1 on checkbox
      QCheckBox* testFlag1CheckBox;
      
      /// test flag 2 on checkbox
      QCheckBox* testFlag2CheckBox;
      
      /// web browser line edit
      QLineEdit* webBrowserLineEdit;
      
      /// image capture type combo box
      QComboBox* imageCaptureTypeComboBox;
      
      /// number of threads spin box
      QSpinBox* numberOfThreadsSpinBox;
      
      /// number of spec file read thread
      QSpinBox* numberOfSpecFileReadThreadsSpinBox;
      
      /// floating point precision spin box
      QSpinBox* floatPrecisionSpinBox;
      
      /// file write data type combo box
      std::vector<QComboBox*> fileWriteDataTypeComboBox;
      
      /// SuMS hosts line edit
      QLineEdit* sumsHostsLineEdit;
      
      /// display lists enabled check box
      QCheckBox* displayListsOnCheckBox;
      
      /// random seed check box
      QCheckBox* randomSeedCheckBox;
      
      /// random seed spin box
      QSpinBox* randomSeedSpinBox;
      
      /// widget group for all widgets
      WuQWidgetGroup* allWidgetsGroup;
      
      /// OpenGL debugging check box
      QCheckBox* openGLDebugCheckBox;
};

#endif


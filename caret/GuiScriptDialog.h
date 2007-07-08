#ifndef __GUI_SCRIPT_DIALOG_H__
#define __GUI_SCRIPT_DIALOG_H__

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

#include <vector>

#include "QtDialog.h"

class GuiScriptOperation;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QProcess;
class QStackedWidget;

/// class for dialog that is used to build scripts
class GuiScriptDialog : public QtDialog {
   Q_OBJECT

   public:
      // constructor
      GuiScriptDialog(QWidget* parent,
                      const QString& caretHomeDirectory);

      // destructor
      ~GuiScriptDialog();

   protected slots:
      // called when apply button is pressed
      void slotApplyButton();
      
      // called when an operation is selected
      void slotOperationsListBox(int item);
      
      // called when mode selected
      void slotModeComboBox(int item);
      
   protected:
      /// script building mode
      enum MODE {
         /// create script only
         MODE_CREATE_SCRIPT,
         /// create script and execute commands
         MODE_CREATE_SCRIPT_AND_EXECUTE_COMMAND,
         /// execute command only
         MODE_EXECUTE_COMMAND
      };
      
      // add an operation
      void addOperation(GuiScriptOperation* op);
      
      // create the deformation map path update page
      void createDeformationMapPathUpdate();
      
      // create the spec file create page
      void createSpecFileCreatePage();
      
      // create the spec file add page
      void createSpecFileAddPage();
      
      // create the blue page
      void createBlurPage();
      
      // create the classify intensities page
      void createClassifyIntensitiesPage();
      
      // create the copy vec mag to volume  page
      void createReplaceVolumeVoxelsWithVectorMagnitude();
      
      // create the convert vector file to volume file page
      void createConvertVectorToVolumePage();
      
      // copy a volume page
      void createCopyVolumePage();
      
      // create a volume page
      void createCreateVolumePage();
      
      // create the dilate page
      void createDilatePage();
      
      // create the dilate and erode page
      void createDilateErodePage();
      
      // create the erode page
      void createErodePage();
      
      // create the fill biggest object page
      void createFillBiggestObjectPage();
      
      // create the fill holes page
      void createFillHolesPage();
      
      // create the fill slice page
      void createFillSlicePage();
      
      // create the find limits page
      void createFindLimitsPage();
      
      // create the flood fill page
      void createFloodFillPage();
      
      // create the gradient page
      void createGradientPage();
      
      // create the volume information page
      void createInformationPage();
      
      // create the intersect volume with surface page
      void createIntersectVolumeWithSurfacePage();
      
      // create the make plane page
      void createMakePlanePage();
      
      // create the make shell page
      void createMakeShellPage();
      
      // create the make sphere page
      void createMakeSpherePage();
      
      // create the math page
      void createMathPage();
      
      // create the math unary page
      void createMathUnaryPage();
      
      // create the mask volume page
      void createMaskVolumePage();
      
      // create the near to plane page
      void createNearToPlanePage();
      
      // create the pad volume page
      void createPadVolumePage();
      
      // create the remove islands page
      void createRemoveIslandsPage();
      
      // create the resample page
      void createResamplePage();
      
      // create the rescale voxels page
      void createRescaleVoxelsPage();
      
      // create the scale voxels 0 to 255 page
      void createScaleVoxels0255Page();
      
      // create the sculpt page
      void createSculptPage();
      
      // create the set orientation page
      void createSetOrientationPage();
      
      // create the set origin page
      void createSetOriginPage();
      
      // create the set spacing page
      void createSetSpacingPage();
      
      // create the shell command page
      void createShellCommandPage();
      
      // create the shift axis page
      void createShiftAxisPage();
      
      // create the smear axis page
      void createSmearAxisPage();
      
      // create the threshold page
      void createThresholdPage();
      
      // create the threshold dual page
      void createThresholdDualPage();
      
      // create the threshold inverse page
      void createThresholdInversePage();
      
      // create the combine vector files page
      void createVectorCombinePage();
      
      // create the replace vector magnitude with volume page
      void createVectorMagReplaceWithVolumePage();
      
      /// operations list box
      QListWidget* operationsListBox;
      
      /// operations description label
      QLabel* operationsDescriptionLabel;
      
      /// the operations widget stack
      QStackedWidget* operationsWidgetStack;
      
      /// the operations objects 
      std::vector<GuiScriptOperation*> operations;
      
      /// path and name of caret_volume program
      QString caretVolumeProgramName;
      
      /// mode combo box
      QComboBox* modeComboBox;
      
      /// script file name line edit
      QLineEdit* scriptFileNameLineEdit;
      
      /// log file name line edit
      QLineEdit* logFileNameLineEdit;
      
      /// log file check box
      QCheckBox* logFileCheckBox;
      
      /// QProcess used for running command
      QProcess* process;
      
      /// first command flag
      bool firstCommandFlag;
      
};

#endif // __GUI_SCRIPT_DIALOG_H__

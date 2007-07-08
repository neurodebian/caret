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

#ifndef __GUI_APPLY_DEFORMATION_MAP_DIALOG_H__
#define __GUI_APPLY_DEFORMATION_MAP_DIALOG_H__

#include <QString>

#include "QtDialog.h"

#include "DeformationMapFile.h"

class BrainSet;
class DeformationMapFile;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QDoubleSpinBox;

/// Class for dialog that applies a deformation map
class GuiApplyDeformationMapDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiApplyDeformationMapDialog(QWidget* parent, Qt::WFlags = 0);
      
      /// Destructor
      ~GuiApplyDeformationMapDialog();
      
   private slots:
      /// called when apply button is pressed
      void slotApplyButton();
      
      /// called when deformation map button is pressed
      void slotDeformationMapButton();
      
      /// called when file pushbutton is pressed
      void slotFileButton();
      
      /// called when deformed file pushbutton is pressed
      void slotDeformedFileButton();
   
      /// called when source directory pushbutton is pressed
      void slotSourceDirectoryButton();
      
      /// called when target directory pushbutton is pressed
      void slotTargetDirectoryButton();
      
      /// called when atlas topo file button is pressed
      void slotAtlasTopoFileButton();
      
      /// called when iniv topo file button is pressed
      void slotIndivTopoFileButton();
      
      /// called when deformed topo file button is pressed
      void slotDeformedTopoFileButton();
      
      /// called when a file type is selected
      void slotFileTypeComboBox(int item);
      
   private:
      enum FILE_DIALOG_TYPE {
         FILE_DIALOG_ATLAS_TOPO_FILE,
         FILE_DIALOG_DATA_FILE,
         FILE_DIALOG_DEFORMED_FILE,
         FILE_DIALOG_INDIV_TOPO_FILE,
         FILE_DIALOG_DEFORMED_TOPO_FILE
      };
      
      /// data file dialog for data file and deformed file
      void dataFileDialog(const FILE_DIALOG_TYPE fdt);
      
      /// set the deformed file name
      void setDeformedFileName(const FILE_DIALOG_TYPE fdt);
      
      /// read the spec files
      bool readBrains(QString& errorMessage);
      
      /// deformation map file name line edit
      QLineEdit* deformationMapFileLineEdit;
      
      /// deformed column name prefix line edit
      QLineEdit* deformedColumnNamePrefixLineEdit;
      
      /// date file name line edit
      QLineEdit* dataFileNameLineEdit;
      
      /// deformed file name line edit
      QLineEdit* deformedFileNameLineEdit;
      
      /// file type combo box
      QComboBox* fileTypeComboBox;
      
      /// the File push button
      QPushButton* filePushButton;
      
      /// the deformed file push button
      QPushButton* deformedFilePushButton;
      
      /// the source brain set needed for some deformations
      BrainSet* sourceBrainSet;
      
      /// the target brain set needed for some deformations
      BrainSet* targetBrainSet;
      
      /// the deformation map file
      DeformationMapFile deformationMapFile;
      
      /// source directory pushbutton
      QPushButton* sourceDirectoryPushButton;
      
      /// target directory pushbutton
      QPushButton* targetDirectoryPushButton;
      
      /// directory line edit;
      QLineEdit* sourceDirectoryLineEdit;
      
      /// directory line edit;
      QLineEdit* targetDirectoryLineEdit;
      
      /// atlas topo file push button
      QPushButton* atlasTopoFilePushButton;
      
      /// atlas topo file line edit
      QLineEdit* atlasTopoFileLineEdit;
      
      /// indiv topo file push button
      QPushButton* indivTopoFilePushButton;
      
      /// indiv topo file line edit
      QLineEdit* indivTopoFileLineEdit;
      
      /// deformed topo file push button
      QPushButton* deformedTopoFilePushButton;
      
      /// deformed topo file line edit
      QLineEdit* deformedTopoFileLineEdit;
      
      /// flat coord max edge length
      QDoubleSpinBox* flatCoordMaxEdgeLengthDoubleSpinBox;
  
      /// flat coord max edge length label
      QLabel* flatCoordMaxEdgeLengthLabel;    

      /// metric nearest node radio button
      QRadioButton* metricNearestNodeRadioButton;

      /// metric average tile nodes radio button
      QRadioButton* metricAverageTileNodesRadioButton;      
      
      /// smooth deformed coord files
      QCheckBox* smoothCoordsOneIterationCheckBox;
};

#endif // __GUI_APPLY_DEFORMATION_MAP_DIALOG_H__


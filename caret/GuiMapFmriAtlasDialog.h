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

#ifndef __GUI_MAP_FMRI_ATLAS_DIALOG_H__
#define __GUI_MAP_FMRI_ATLAS_DIALOG_H__

#include <QString>
#include <vector>

#include "MapFmriAtlasSpecFileInfo.h"
#include "WuQDialog.h"

class PreferencesFile;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QGroupBox;

/// Class for selecting coordinate files from an atlas spec file
class GuiMapFmriAtlasDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMapFmriAtlasDialog(QWidget* parent,
                            std::vector<MapFmriAtlasSpecFileInfo>* atlasesIn,
                            PreferencesFile* pf,
                            const QString& speciesIn,
                            const QString& structureNameIn,
                            const bool showOutputSpecFileSelectionIn,
                            const bool enableMetricFiducialOptionsIn,
                            const bool enablePaintFiducialOptionsIn);
                                 
      /// Destructor
      ~GuiMapFmriAtlasDialog();
      
      /// get the output spec file 
      QString getOutputSpecFileName() const;
      
      /// get the selected atlas information.
      void getSelectedMetricAtlasData(QString& atlasPath,
                                      QString& topoFileName,
                                      QString& description,
                                      std::vector<QString>& coordFileNames,
                                      QString& averageCoordFile,
                                      QString& metricNameHint,
                                      QString& structureName,
                                      bool& mapToAvgCoordFileFlag,
                                      bool& mapToAvgOfAllFlag,
                                      bool& mapToStdDevOfAllFlag,
                                      bool& mapToStdErrorOfAllFlag,
                                      bool& mapToMinOfAllFlag,
                                      bool& mapToMaxOfAllFlag,
                                      bool& mapToAllCasesFlag) const;
      
      /// get the selected paint atlas information.
      void getSelectedPaintAtlasData(QString& atlasPath,
                                      QString& topoFileName,
                                      QString& description,
                                      std::vector<QString>& coordFileNames,
                                      QString& averageCoordFile,
                                      QString& paintNameHint,
                                      QString& structureName,
                                      bool& mapToAvgCoordFileFlag,
                                      bool& mapToMostCommonOfAllFlag,
                                      bool& mapToMostCommonExcludeUnidentifiedOfAllFlag,
                                      bool& mapToAllCasesFlag) const;
      
   protected slots:
      /// called when select spec file pushbutton pressed.
      void slotSelectSpecPushButton();

      /// called when an atlas space is selected
      void slotAtlasSpaceComboBox(int item);
      
      /// called when an atlas is selected
      void slotAtlasComboBox(int item);
      
   protected:
      /// overrides parent's version
      void done(int r);
      
      /// load atlas list box
      void loadAtlasListBox();

      /// Load the atlas space combo box.
      void loadAtlasSpaceComboBox();

      /// atlas space combo box
      QComboBox* atlasSpaceComboBox;
      
      /// atlas combo box
      QComboBox* atlasComboBox;
      
      /// the metric multi-fiducial group box
      QGroupBox* metricMultiFiducialGroupBox;
      
      /// metric multi-fid show map to average fiducial check box
      QCheckBox* metricMultiFidAvgFidCheckBox;
      
      /// metric multi-fid show average of all cases check box
      QCheckBox* metricMultiFidAvgAllCasesCheckBox;
      
      /// metric multi-fid show std dev of all cases check box
      QCheckBox* metricMultiFidStdDevAllCheckBox;
      
      /// metric multi-fid show std error of all cases check box
      QCheckBox* metricMultiFidStdErrorAllCheckBox;
      
      /// metric multi-fid show minimum of all cases check box
      QCheckBox* metricMultiFidMinAllCheckBox;
      
      /// metric multi-fid show maximum of all cases check box
      QCheckBox* metricMultiFidMaxAllCheckBox;
      
      /// metric multi-fid show all indiv cases check box
      QCheckBox* metricMultiFidAllCasesCheckBox;
      
      /// the paint multi-fiducial group box
      QGroupBox* paintMultiFiducialGroupBox;
      
      /// paint multi-fid show map to average fiducial check box
      QCheckBox* paintMultiFidAvgFidCheckBox;
      
      /// paint multi-fid show all indiv cases check box
      QCheckBox* paintMultiFidAllCasesCheckBox;
      
      /// paint multi-fid most common of all cases check box
      QCheckBox* paintMultiFidMostCommonCheckBox;
      
      /// paint multi-fid most common exclude unidentified of all cases check box
      QCheckBox* paintMultiFidMostCommonExcludeUnidentifiedCheckBox;
      
      /// the atlases that match species/hem
      std::vector<MapFmriAtlasSpecFileInfo> matchingAtlases;
      
      /// all of the atlases
      std::vector<MapFmriAtlasSpecFileInfo>* allAtlases;
      
      /// the spec file name line edit
      QLineEdit* specFileLineEdit;
      
      /// the preferences file
      PreferencesFile* preferencesFile;
      
      /// the desired species
      QString species;
      
      /// the desired structure
      QString structureName;
      
      /// the desired space
      QString space;
      
};

#endif // __GUI_MAP_FMRI_ATLAS_DIALOG_H__


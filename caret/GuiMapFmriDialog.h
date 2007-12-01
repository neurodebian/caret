#ifndef __GUI_MAP_FMRI_DIALOG_H__
#define __GUI_MAP_FMRI_DIALOG_H__

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

class BrainModelSurface;
class BrainSet;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QProgressDialog;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTableWidget;
class QTextBrowser;
class QTextEdit;
class QStackedWidget;

class QDoubleSpinBox;

#include "BrainModelVolumeToSurfaceMapperAlgorithmParameters.h"
#include "MapFmriAtlasSpecFileInfo.h"
#include "GuiMapFmriMappingSet.h"
#include "GuiMapFmriVolume.h"
#include "QtDialog.h"


/// Dialog for fmri mapping
class GuiMapFmriDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMapFmriDialog(QWidget* parent,
                       const bool runningAsPartOfCaretIn,
                       bool modal = false,
                       Qt::WFlags f = 0);
                       
      /// Destructor
      ~GuiMapFmriDialog();
     
      // Initialize the dialog (resets to first page and clears everything)
      void initializeDialog();
      
   signals:
      // signal that mapping is complete
      void signalMappingComplete();
      
   protected slots:
      /// Set the pages that are valid for viewing according to the current selections.
      void slotSetAppropriatePages();

      /// Called to select the next page (overrides parent method)
      void slotNext();

      /// Called to select the previous page (overrides parent method)
      void slotBack();

      /// Called when close/cancel button is pressed.
      void slotCloseOrCancelButton();
      
      /// Called when Select Volumes From Disk pushbutton pressed.
      void slotDiskVolumesPushButton();

      /// Called when Select Loaded Volumes pushbutton pressed.
      void slotLoadedVolumesPushButton();

      /// Called when Remove Volumes pushbutton pressed.
      void slotRemoveVolumesPushButton();

      /// Called when add caret surfaces pushbutton pressed.
      void slotAddCaretSurfacesPushButton();
      
      /// Called when add spec files pushbutton pressed.
      void slotAddSpecFilesPushButton();
      
      /// Called when add spec file map with atlas pushbutton pressed.
      void slotAddSpecAtlasPushButton();
      
      /// Called when add caret map with spec pushbutton is pressed.
      void slotAddCaretMapWithAtlasPushButton();
      
      /// Called when remove mapping sets (caret surfaces/spec files) pushbutton pressed.
      void slotRemoveMappingSetsPushButton();
      
      /// Called when an algorithm is selected.
      void slotAlgorithmComboBox(int item);

      /// load the metric naming page
      void slotLoadMetricNamingPage();

      /// read the metric naming page
      void slotReadMetricNamingPage();
      
      /// called when metric file name select button pressed
      void slotMetricFilePushButton();
      
      /// called when a data mapping type is selected
      void slotDataMappingTypeButtGroup(int item);
      
      /// called when a cell in the metric naming table is double clicked
      void slotMetricNamingTableCellDoubleClicked(int row, int col);
      
   protected:
      /// type of data being mapped
      enum DATA_MAPPING_TYPE {
         DATA_MAPPING_TYPE_NONE,
         DATA_MAPPING_TYPE_METRIC,
         DATA_MAPPING_TYPE_PAINT
      };
      
      /// override's parent's done method
      void done(int r);
      
      /// add a page to the dialog
      void addPage(QWidget* pageIn, const QString& pageTitleIn);
      
      /// Adds help information in a read only text widget
      QWidget* addPageInformation(const QString& title, const QString& text,
                                  QTextBrowser* textBrowserIn = NULL);
      
      /// map functional volumes to surfaces.
      void mapVolumesToSurfaces();

      /// Load the volumes list box.
      void loadVolumesListBox();

      /// Prepare some pages when they are about to be shown
      void showPage(QWidget* page);
      
      /// create the data mapping type page
      QWidget* createDataMappingTypePage();
      
      /// create the volume selection page
      QWidget* createVolumeSelectionPage();
      
      /// create the spec file and surface selection page
      QWidget* createSpecFileAndSurfaceSelectionPage();
      
      /// create the metric naming page
      QWidget* createMetricNamingPage();
      
      /// create the algorithm page
      QWidget* createAlgorithmPage();
      
      /// create the summary page
      QWidget* createSummaryPage();
      
      /// Get the Caret fiducial surfaces.
      void getFiducialSurfaces(std::vector<BrainModelSurface*>& surfaces,
                               bool& haveAnySurface) const;

      /// load the mapping sets list box
      void loadMappingSetsListBox();
      
      /// reset (erase) the mapping sets
      void resetMappingSets();
      
      /// Load the algorithm parameters from the preferences file.
      void loadAlgorithmParametersFromPreferences();

      /// Load the metric naming combo box
      void loadMetricNamingComboBox();
      
      /// Load the summary text edit
      void loadSummaryTextEdit();

      /// Read the atlases
      void readAtlases();

      /// see if atlases are available
      bool haveAtlases() const { return (atlasSpecFileInfo.empty() == false); }
      
      /// map the volumes to metric files
      void mapDataToMetricFiles(QProgressDialog& progressDialog);
      
      /// map the volumes to paint files
      void mapDataToPaintFiles(QProgressDialog& progressDialog);
      
      /// the mapping parameters
      BrainModelVolumeToSurfaceMapperAlgorithmParameters mappingParameters;
   
      /// type of data being mapped
      DATA_MAPPING_TYPE dataMappingType;
      
      /// running as part of caret flag
      bool runningAsPartOfCaret;
      
      /// push button for selecting caret surfaces
      QPushButton* addCaretSurfacesPushButton;
      
      /// push button for mapping to caret with an atlas
      QPushButton* addCaretMapWithAtlasPushButton;
      
      /// push button for map to spec file with atlas
      QPushButton* addSpecAtlasPushButton;
      
      /// the data mapping type page
      QWidget* pageDataMappingType;
      
      /// the volume selection page
      QWidget* pageVolumeSelection;
      
      /// the spec file and surface selection page
      QWidget* pageSpecFileAndSurfaceSelection;
      
      /// the metric naming page
      QWidget* pageMetricNaming;
      
      /// the algorithm page
      QWidget* pageAlgorithm;
      
      /// the summary page
      QWidget* pageSummary;
      
      /// mapping type functional radio button
      QRadioButton* mappingTypeFunctionalRadioButton;
      
      /// mapping type paint radio button
      QRadioButton* mappingTypePaintRadioButton;
      
      /// functional volumes list box
      QListWidget* volumeListBox;

      /// volumes that are to be mapped
      std::vector<GuiMapFmriVolume*> volumesToBeMapped;
      
      /// list box for mapping sets
      QListWidget* mappingSetListBox;
      
      /// surfaces/spec files for mapping
      std::vector<GuiMapFmriMappingSet> mappingSets;
      
      /// metric file name push button
      QPushButton* metricFilePushButton;
      
      /// metric file name line edit
      QLineEdit* metricFileNameLineEdit;
      
      /// the metric naming table
      QTableWidget* metricNamingTable;
      
      /// the metric naming mapping set combo box
      QComboBox* metricMappingSetComboBox;
      
      /// metric naming columns
      enum {
         METRIC_NAMING_COLUMN_NAME       = 0,
         METRIC_NAMING_COLUMN_COMMENT    = 1,
         METRIC_NAMING_COLUMN_SURFACE    = 2,
         METRIC_NAMING_COLUMN_VOLUME     = 3,
         METRIC_NAMING_COLUMN_SUB_VOLUME = 4,
         METRIC_NAMING_COLUMN_NEG_THRESH = 5,
         METRIC_NAMING_COLUMN_POS_THRESH = 6,
         METRIC_NAMING_NUMBER_OF_COLUMNS = 7  // MUST BE LAST
      };
      
      /// algorithm selection combo box
      QComboBox* algorithmComboBox;
      
      /// algorithm information text edit
      QTextBrowser* algorithmInformationTextEdit;

      /// algorithm parameters widget stack
      QStackedWidget* algorithmParameterWidgetStack;
      
      /// algorithm average nodes parameters widget
      QWidget* algorithmMetricAverageNodesParametersBox;
      
      /// algorithm average voxel parameters widget
      QWidget* algorithmMetricAverageVoxelParametersBox;
      
      /// algorithm enclosing voxel parameters widget
      QWidget* algorithmMetricEnclosingVoxelParametersBox;
      
      /// algorithm gaussian parameters widget
      QWidget* algorithmMetricGaussianParametersBox;
      
      /// algorithm interpolated voxel parameters widget
      QWidget* algorithmMetricInterpolatedVoxelParametersBox;
      
      /// algorithm maximum voxel parameters widget
      QWidget* algorithmMetricMaximumVoxelParametersBox;
      
      /// algorithm mcw brain fish parameters widget
      QWidget* algorithmMetricMcwBrainFishParametersBox;
      
      /// algorithm strongest voxel parameters widget
      QWidget* algorithmMetricStrongestVoxelParametersBox;
      
      /// algorithm paint enclosing voxel parameters widget
      QWidget* algorithmPaintEnclosingVoxelParametersBox;
      
      /// algorithm average voxel neighbor size float spin box
      QDoubleSpinBox* algorithmAverageVoxelNeighborDoubleSpinBox;
      
      /// algorithm gaussian neighbor size float spin box
      QDoubleSpinBox* algorithmGaussianNeighborDoubleSpinBox;
      
      /// algorithm maximum voxel neighbor size float spin box
      QDoubleSpinBox* algorithmMaximumVoxelNeighborDoubleSpinBox;
      
      /// algorithm gaussian sigma norm float spin box
      QDoubleSpinBox* algorithmGaussianSigmaNormDoubleSpinBox;
      
      /// algorithm gaussian sigma tang float spin box
      QDoubleSpinBox* algorithmGaussianSigmaTangDoubleSpinBox;
      
      /// algorithm gaussian norm below float spin box
      QDoubleSpinBox* algorithmGaussianNormBelowDoubleSpinBox;
      
      /// algorithm gaussian norm above float spin box
      QDoubleSpinBox* algorithmGaussianNormAboveDoubleSpinBox;
      
      /// algorithm gaussian tang cutoff float spin box
      QDoubleSpinBox* algorithmGaussianTangCutoffDoubleSpinBox;
      
      /// algorithm brain fish max distance float spin box
      QDoubleSpinBox* algorithmBrainFishMaxDistanceDoubleSpinBox;
      
      /// algorithm brain fish splat factor spin box
      QSpinBox* algorithmBrainFishSplatFactorSpinBox;
      
      /// algorithm strongest voxel neighbor size float spin box
      QDoubleSpinBox* algorithmStrongestVoxelNeighborDoubleSpinBox;
      
      /// the summary page text edit
      QTextEdit* summaryTextEdit;
      
      /// directory containing the atlas files
      QString atlasFilesDirectory;
      
      /// info about atlas spec files
      std::vector<MapFmriAtlasSpecFileInfo> atlasSpecFileInfo;
      
      /// extension for output data file
      QString outputDataFileExtension;
      
      /// the loaded volumes push button
      QPushButton* loadedVolumesPushButton;
      
      /// enable entry of volume thresholds check box
      QCheckBox* volumeEnterThreshCheckBox;
      
      /// widget stack for pages in dialog
      QStackedWidget* pagesStackedWidget;
      
      /// next push button
      QPushButton* nextPushButton;
      
      /// label for title of current page
      QLabel* currentPageTitleLabel;
      
      /// titles for pages
      std::vector<QString> pageTitles;
};

#endif // __GUI_MAP_FMRI_DIALOG_H__


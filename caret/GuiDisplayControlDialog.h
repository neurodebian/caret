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

#ifndef __GUI_DISPLAY_CONTROL_DIALOG_H__
#define __GUI_DISPLAY_CONTROL_DIALOG_H__

#include <vector>


#include <QSize>

#include "DisplaySettingsVolume.h"

#include "QtDialog.h"
// forward declarations help avoid lots of include files
class QAction;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QRadioButton;
class QScrollArea;
class QSlider;
class QSpinBox;
class QStackedWidget;
class QTextEdit;
class QToolButton;
class QVBoxLayout;

class GuiBrainModelSelectionComboBox;
class GuiNodeAttributeColumnSelectionComboBox;
class GuiTransformationMatrixSelectionControl;
class QDoubleSpinBox;
class QtTextEditDialog;
class WuQWidgetGroup;

/// Dialog for controlling display of data
class GuiDisplayControlDialog : public QtDialog {
   
   Q_OBJECT
   
   public:
      /// constructor
      GuiDisplayControlDialog(QWidget* parent);
      
      /// destructor
      ~GuiDisplayControlDialog();
      
      /// show the scene page
      void showScenePage();
      
      /// Called when a new spec file is loaded
      void newSpecFileLoaded();

      /// Update all items in the dialog
      void updateAllItemsInDialog(const bool filesChanged /* = false*/,
                                  const bool updateResultOfSceneChange);
      
      /// Update the toggles and combo boxes based upon overlay/underlay selections
      void updateOverlayUnderlayItems();
      
      /// update the surface and volume selections
      void updateSurfaceAndVolumeItems();
      
      /// update the volume selections
      void updateVolumeItems();
      
      /// update the volume tooltips
      void updateVolumeToolTips();
      
      /// update all border items in dialog.  If the number of borders has changed
      /// pass true.
      void updateBorderItems(const bool bordersWereChanged = false);
      
      /// update border main page
      void updateBorderMainPage();
      
      /// update border colors page
      void updateBorderColorPage(const bool filesChanged);
      
      /// update border name page
      void updateBorderNamePage(const bool filesChanged);
      
      /// update all cell items in dialog.  If the number of cells has changed
      /// pass true.
      void updateCellItems(const bool cellsWereChanged = false);
      
      /// upate cell main page
      void updateCellMainPage();
      
      /// update cell class page
      void updateCellClassPage(const bool cellsWereChanged);
      
      /// update cell color page
      void updateCellColorPage(const bool cellsWereChanged);
      
      /// update all cocomac items in dialog.
      void updateCocomacItems();
      
      /// update the cocomac display page
      void updateCocomacDisplayPage();
      
      /// update the cocomac information page
      void updateCocomacInformationPage();
      
      /// update all contour items in dialog.
      void updateContourItems(const bool filesChanged = false);
      
      /// update all contour main page in dialog.
      void updateContourMainPage();
      
      /// update all contour class page in dialog.
      void updateContourClassPage(const bool filesChanged);
      
      /// update all contour color page in dialog.
      void updateContourColorPage(const bool filesChanged);
      
      /// update the deformation field page
      void updateDeformationFieldPage();
      
      /// update foci main page
      void updateFociMainPage();
      
      /// update foci class page
      void updateFociClassPage(const bool filesChanged);
      
      /// update foci color page
      void updateFociColorPage(const bool filesChanged);
      
      /// update foci keyword page
      void updateFociKeywordPage(const bool filesChanged);
      
      /// update foci name page
      void updateFociNamePage(const bool filesChanged);
      
      /// update foci page
      void updateFociTablePage(const bool filesChanged);
      
      /// update all foci items in dialog.  If the number of foci has changed
      /// pass true.
      void updateFociItems(const bool fociWereChanged = false);
      
      /// update lat lon items
      void updateLatLonItems();
      
      /// update geodesic items
      void updateGeodesicItems();
      
      /// update the image items
      void updateImagesItems();
      
      /// update all metric items in dialog
      void updateMetricItems();
      
      /// update metric overlay/underlay selection
      void updateMetricOverlayUnderlaySelection();
      
      /// update the metric selection page
      void updateMetricSelectionPage();
      
      /// update the metric settings page
      void updateMetricSettingsPage();
      
      /// update the misc items
      void updateMiscItems();
      
      /// update model main page
      void updateModelMainPage();
      
      /// update model settings page
      void updateModelSettingsPage();
      
      /// update the model items in the dialog
      void updateModelItems();
      
      /// update paint overlay/underlay selection
      void updatePaintOverlayUnderlaySelection();

      /// update the paint items in the dialog
      void updatePaintItems();
      
      /// update paint column name page 
      void updatePaintColumnPage();
      
      /// update paint name page
      void updatePaintNamePage();
      
      /// update prob atlas surface overlay/underlay selection
      void updateProbAtlasSurfaceOverlayUnderlaySelection();

      /// update all prob atlas surface items in dialog
      void updateProbAtlasSurfaceItems(const bool filesWereChanged = false);
      
      /// update prob atlas surface main page
      void updateProbAtlasSurfaceMainPage();
      
      /// update prob atlas surface channel page
      void updateProbAtlasSurfaceChannelPage(const bool filesWereChanged);
      
      /// update prob atlas surface area page
      void updateProbAtlasSurfaceAreaPage(const bool filesWereChanged);
      
      /// update all prob atlas volume main page
      void updateProbAtlasVolumeMainPage();
      
      /// update all prob atlas volume area page
      void updateProbAtlasVolumeAreaPage(const bool filesChanged);
      
      /// update all prob atlas volume channel page
      void updateProbAtlasVolumeChannelPage(const bool filesChanged);
      
      /// update all prob atlas volume items in dialog
      void updateProbAtlasVolumeItems(const bool filesWereChanged = false);
      
      /// update region items
      void updateRegionItems();
      
      /// update scene items
      void updateSceneItems();
      
      /// update rgb paint overlay/underlay selections
      void updateRgbPaintOverlayUnderlaySelection();
      
      /// update all rgb paint items in dialog
      void updateRgbPaintItems();
            
      /// update shape overlay/underlay selection
      void updateShapeOverlayUnderlaySelection();

      /// update all surface shape selections in dialog
      void updateShapeSelections();
      
      /// update the shape items
      void updateShapeItems();
      
      /// update all surface shape settings in dialog
      void updateShapeSettings();
      
      /// update surface vector items in dialog
      void updateSurfaceVectorItems();
      
      /// update surface vector selection page
      void updateSurfaceVectorSelectionPage();
      
      /// update surface vector settings page
      void updateSurfaceVectorSettingsPage();
      
      /// update topography overlay/underlay selection
      void updateTopographyOverlayUnderlaySelection();

      /// update all topography items in dialog
      void updateTopographyItems();
      
      /// update areal estimation items.
      void updateArealEstimationItems();

      /// print the size of the pages
      void printPageSizes();

   private slots:
      /// called by OK (relabeled "Apply") button press.  Overriding qtabdialog's
      /// accept prevents Ok button from closing the dialog.
      //void accept() {}
      
      /// called when surface model combo box is changed
      void slotSurfaceModelIndexComboBox(int item);
      
      /// called when page combo box selection is made
      void pageComboBoxSelection(int item);
      
      /// called when page back tool button pressed
      void slotPageBackToolButtonPressed(QAction*);
      
      /// called when page forward tool button pressed
      void slotPageForwardToolButtonPressed(QAction*);
      
      /// called when popup menu selection is made
      void popupMenuSelection(QAction*);
      
      /// called when return is pressed in a line edit and by other methods to update graphics
      void applySelected();
 
      /// called when help button pressed
      void slotHelpButton();
      
      /// called to show a scene
      void slotShowScenePushButton();
      
      /// called to append a new scene
      void slotAppendScenePushButton();
      
      /// called to insert a new scene
      void slotInsertScenePushButton();
      
      /// called to replace a scene
      void slotReplaceScenePushButton();
      
      /// called to delete a scene
      void slotDeleteScenePushButton();
      
      /// called when a scene is selected
      void slotSceneListBox(int item);
      
      /// called when a scene is selected
      void slotSceneListBox(QListWidgetItem* item);
      
      /// called to check all scenes
      void slotCheckAllScenesPushButton();
      
      /// called to create a spec file from files used in selected scenes
      void slotCreateSpecFromSelectedScenesPushButton();
      
      /// called to display a histogram of the selecteed anatomy volume
      void slotAnatomyVolumeHistogram();
      
      /// called when a volume primary overlay is selected
      void volumePrimaryOverlaySelection(int n);
      
      /// called when a volume secondary overlay is selected
      void volumeSecondaryOverlaySelection(int n);
      
      /// called when a volume underlay is selected
      void volumeUnderlaySelection(int n);
      
      /// called when a anatomy volume column is selected
      void volumeAnatomySelection(int n);
      
      /// called when a functional view volume viewing column is selected
      void volumeFunctionalViewSelection(int n);
      
      /// called when a volume threshold column is selected
      void volumeFunctionalThresholdSelection(int n);
      
      /// called when a paint volume column is selected
      void volumePaintSelection(int n);
      
      /// called when an rgb volume column is selected
      void volumeRgbSelection(int n);
      
      /// called when a segmentation volume column is selected
      void volumeSegmentationSelection(int n);
      
      /// called when a vector volume column is selected
      void volumeVectorSelection(int n);
      
      /// called to display comment info for volume
      void volumeAnatomyInfoPushButtonSelection();
      
      /// called to display comment info for volume
      void volumeFunctionalViewInfoPushButtonSelection();
      
      /// called to display comment info for volume
      void volumeFunctionalThreshInfoPushButtonSelection();
      
      /// called to display comment info for volume
      void volumePaintInfoPushButtonSelection();
      
      /// called to display comment info for volume
      void volumeProbAtlasInfoPushButtonSelection();
      
      /// called to set prob atlas volume study metadata link
      void volumeProbAtlasVolumeStudyMetaDataPushButton();
      
      /// called to display comment info for volume
      void volumeRgbInfoPushButtonSelection();
      
      /// called to display comment info for volume
      void volumeSegmentationInfoPushButtonSelection();
      
      /// called to display comment info for volume
      void volumeVectorInfoPushButtonSelection();
      
      /// called to display metadata for anatomy volume
      void volumeAnatomyMetaDataPushButtonSelection();
      
      /// called to display metadata for functional view volume
      void volumeFunctionalViewMetaDataPushButtonSelection();
      
      /// called to display metadata for functional thresh volume
      void volumeFunctionalThreshMetaDataPushButtonSelection();
      
      /// called to display metadata for paint volume
      void volumePaintMetaDataPushButtonSelection();
      
      /// called to display metadata for rgb volume
      void volumeRgbMetaDataPushButtonSelection();
      
      /// called to display metadata for segmentation volume
      void volumeSegmentationMetaDataPushButtonSelection();
      
      /// called to display metadata for vector volume
      void volumeVectorMetaDataPushButtonSelection();
      
      /// called to start the volume animation
      void slotVolumeAnimateStartPushButton();
      
      /// called to stop the volume animation
      void slotVolumeAnimateStopPushButton();
      
      /// called to set default brightness/contrast
      void slotDefaultVolumeContrastBrightness();
      
      /// called when a primary overlay is selected
      void primaryOverlaySelection(int n);
      
      /// called when a secondary overlay is selected
      void secondaryOverlaySelection(int n);
      
      /// called when an underlay is selected
      void underlaySelection(int n);
      
      /// Called to display comment information about an areal estimation column.
      void arealEstimationCommentColumnSelection(int column);

      /// Called to display metadata information about an areal estimation column.
      void arealEstimationMetaDataColumnSelection(int column);

      /// read the areal estimation page.
      void readArealEstimationSelections();

      /// called when a areal est file column is selected
      void arealEstFileSelection(int col);
      
      /// called when areal est's ? button is clicked
      void arealEstInfoPushButtonSelection();
      
      /// called when a metric file column is selected
      void metricDisplayColumnSelection(int col);
      
      /// called when metric's ? button is clicked
      void metricInfoPushButtonSelection();
      
      /// called when a metric file column is selected
      void metricThresholdColumnSelection(int col);
      
      /// called when a metric file column comment is selected
      void metricCommentColumnSelection(int col);
      
      /// called when a metric metadata column is selected
      void metricMetaDataColumnSelection(int col);
      
      /// called when a metric palette is selected 
      void metricPaletteSelection(int itemNum);
      
      /// called when animate push button is selected
      void metricAnimatePushButtonSelection();
      
      /// called when metric view column histogram pushbutton is pressed
      void metricViewHistogram();
      
      /// called when metric thresh column histogram pushbutton is pressed
      void metricThreshHistogram();
      
      /// called when a paint file column is selected
      void paintColumnSelection(int col);
      
      /// called when paint's ? button is clicked
      void paintInfoPushButtonSelection();
      
      /// called when rgb paint's ? button is clicked
      void rgbPaintInfoPushButtonSelection();
      
      /// called when prob atlas ? button is clicked
      void probAtlasSurfaceInfoPushButtonSelection();
      
      /// called when a paint page ? button is clicked
      void paintCommentColumnSelection(int num);
      
      /// called when a paint page meta data button is clicked
      void paintMetaDataColumnSelection(int num);
      
      /// called when a lat lon page ? button is clicked
      void latLonCommentColumnSelection(int num);
      
      /// called when a geodesic page ? button is clicked
      void geodesicCommentColumnSelection(int num);
      
   //   /// called when prob atlas file selected
   //   void probAtlasFileSelection();
      
      /// called when an rgb paint file column is selected
      void rgbPaintFileSelection(int col);
      
      /// called when a "?" on rgb tab page is selected
      void rgbPaintCommentSelection(int buttonNum);
           
      /// called when an RGB Paint display mode selected
      void rgbDisplayModeSelection(int itemNumber);

      /// called when a surface shape file column is selected
      void shapeColumnSelection(int col);
      
      /// called when a surface shape column ? is selected
      void surfaceShapeCommentColumnSelection(int item);
      
      /// called when a surface shape column metadata M is selected
      void surfaceShapeMetaDataColumnSelection(int item);
      
      /// called when surface shape's ? button is clicked
      void shapeInfoPushButtonSelection();
      
      /// called when primary overlay light checkbox is toggled
      void primaryOverlayLightSelection();
      
      /// called when secondary overlay light checkbox is toggled
      void secondaryOverlayLightSelection();
      
      /// called when underlay light checkbox is toggled
      void underlayLightSelection();
      
      /// reads the opacity and geography blending lineedits
      void readOverlayUnderlaySelections();
      
      /// reads the volume selections
      void readVolumeSelections();
      
      /// reads the volume settings
      void readVolumeSettings();
      
      /// reads the volume surface outline
      void readVolumeSurfaceOutline();
      
      /// read the cocomac display page
      void readCocomacDisplayPage();
   
      /// read the contour main page
      void readContourMainPage();
      
      /// read the contour class page
      void readContourClassPage();
      
      /// read the contour color page
      void readContourColorPage();
      
      /// slot to turn all contour cell classes on
      void contourCellClassAllOn();
      
      /// slot to turn all contour cell classes off
      void contourCellClassAllOff();
      
      /// slot to turn all contour cell colors on
      void contourCellColorAllOn();
      
      /// slot to turn all contour cell colors off
      void contourCellColorAllOff();
      
      /// read the deformation field page
      void readDeformationFieldPage();
      
      /// called when borders selected on borders or overlay/underlay surface page
      void showBordersToggleSlot(bool b);
      
      /// read border main page
      void readBorderMainPage();
      
      /// read border color page
      void readBorderColorPage();
      
      /// read border name page
      void readBorderNamePage();
      
      /// called when border colors All On button is pressed
      void borderColorAllOn();
      
      /// called when border colors All Off button is pressed
      void borderColorAllOff();
      
      /// called when border name All On button is pressed
      void borderNameAllOn();
      
      /// called when border name All Off button is pressed
      void borderNameAllOff();
      
      /// called when cells selected on cells page or overlay/underlay surface page
      void showCellsToggleSlot(bool b);
      
      /// read the cell main page
      void readCellMainPage();
      
      /// read the cell color page
      void readCellColorPage();
      
      /// read the cell class page
      void readCellClassPage();
      
      /// read the misc selections
      void readMiscSelections();
      
      /// read the surface and volume selections
      void readSurfaceAndVolumeSelections();
      
      /// called when cell class All On button is pressed
      void cellClassAllOn();
      
      /// called when cell class All Off button is pressed
      void cellClassAllOff();
      
      /// called when cell colors All On button is pressed
      void cellColorAllOn();
      
      /// called when cell colors All Off button is pressed
      void cellColorAllOff();
      
      /// called when foci selected on foci page or overlay/underlay surface page
      void showFociToggleSlot(bool b);
      
      /// called when foci color mode changed
      void slotFociColorModeComboBox(int i);
      
      /// read the foci main page
      void readFociMainPage(const bool updateDisplay = true);
      
      /// read the foci class page
      void readFociClassPage(const bool updateDisplay = true);
      
      /// read the foci color page
      void readFociColorPage(const bool updateDisplay = true);
      
      /// read the foci keyword page
      void readFociKeywordPage(const bool updateDisplay = true);
      
      /// read the foci name page
      void readFociNamePage(const bool updateDisplay = true);
      
      /// read the foci table page
      void readFociTablePage(const bool updateDisplay = true);
      
      /// called when foci class All On button is pressed
      void fociClassAllOn();
      
      /// called when foci class All Off button is pressed
      void fociClassAllOff();
      
      /// called when foci colors All On button is pressed
      void fociColorAllOn();
      
      /// called when foci colors All Off button is pressed
      void fociColorAllOff();
      
      /// called when foci names All On button is pressed
      void fociNamesAllOn();
      
      /// called when foci names All Off button is pressed
      void fociNamesAllOff();
      
      /// called when foci keywords All On button is pressed
      void fociKeywordsAllOn();
      
      /// called when foci keywords All Off button is pressed
      void fociKeywordsAllOff();
      
      /// called when foci keywords or tables Update button is pressed
      void fociKeywordsAndTablesUpdate();
      
      /// called when foci tables All On button is pressed
      void fociTablesAllOn();
      
      /// called when foci tables All Off button is pressed
      void fociTablesAllOff();
      
      /// read the geodesic page
      void readGeodesicSelections();
      
      /// read the lat lon page
      void readLatLonSelections();
      
      /// read the metric selection page
      void readMetricSelectionPage();
      
      /// read metric L-to-L, R-to-R
      void readMetricL2LR2R();
      
      /// read the metric settings page
      void readMetricSettingsPage();
      
      /// read the region selections
      void readRegionSelections();
      
      /// read rgb paint L-to-L, R-to-R
      void readRgbPaintL2LR2R();
      
      /// read the rgb paint selections
      void readRgbPaintSelections();
      
      /// read the surface shape selections
      void readShapeSelections();
      
      /// read the surface shape settings
      void readShapeSettings();
      
      /// read the shape L-to-L, R-to-R
      void readShapeL2LR2R();
      
      /// read the scene selections
      void readSceneSelections();
      
      /// called when surface shape color map selected
      void shapeColorMapSelection(int mapNumber);
      
      /// called when surface shape histogram button pressed
      void surfaceShapeHistogram();
      
      /// create and update the surface shape selections
      void createAndUpdateSurfaceShapeSelections();

      /// read the topography selections
      void readTopographySelections();
      
      /// called when a topography file is selected
      void topographyFileSelection(int fileNumber);
      
      /// called when a type of topography is selected
      void topographyTypeSelection(int typeSelected);
      
      /// read the surface vector selection page
      void readSurfaceVectorSelectionPage();
      
      /// read the surface vector settings page
      void readSurfaceVectorSettingsPage();
      
      /// called when surface vector comment button pressed
      void slotSurfaceVectorCommentPushButton(int item);
      
      /// read the probabilistic atlas surface main page
      void readProbAtlasSurfaceMainPage();
      
      /// read the probabilistic atlas surface channel page
      void readProbAtlasSurfaceChannelPage();
      
      /// called to set prob atlas surface study metadata link
      void volumeProbAtlasSurfaceStudyMetaDataPushButton();
      
      /// read the probabilistic atlas surface area page
      void readProbAtlasSurfaceAreaPage();
      
      /// read the prob atlas L-to-L, R-to-R
      void readProbAtlasSurfaceL2LR2R();
      
      /// called when a prob atlas surface display mode selection is made
      void probAtlasSurfaceModeSelection(int num);
      
      ///  called when prob atlas surface all on channel button is pressed
      void probAtlasSurfaceChannelAllOn();
      
      ///  called when prob atlas surface all off channel button is pressed
      void probAtlasSurfaceChannelAllOff();
      
      ///  called when prob atlas surface all on areas button is pressed
      void probAtlasSurfaceAreasAllOn();
      
      ///  called when prob atlas surface all off areas button is pressed
      void probAtlasSurfaceAreasAllOff();
      
      /// read the probabilistic Volume main page
      void readProbAtlasVolumeMainPage();
      
      /// read the probabilistic Volume area page
      void readProbAtlasVolumeAreaPage();
      
      /// read the probabilistic Volumechannel page
      void readProbAtlasVolumeChannelPage();
      
      /// called when a prob atlas Volume display mode selection is made
      void probAtlasVolumeModeSelection(int num);
      
      ///  called when prob atlas Volume all on channel button is pressed
      void probAtlasVolumeChannelAllOn();
      
      ///  called when prob atlas Volume all off channel button is pressed
      void probAtlasVolumeChannelAllOff();
      
      ///  called when prob atlas Volume all on areas button is pressed
      void probAtlasVolumeAreasAllOn();
      
      ///  called when prob atlas volume all off areas button is pressed
      void probAtlasVolumeAreasAllOff();
      
      /// read the image selections
      void readImagesSelections();
      
      /// called to read model main page items from dialog
      void readModelMainPage();

      /// called to read model main page items from dialog
      void readModelSettingsPage();

      /// called when all models on pushbutton pressed
      void slotModelsAllOn();
            
      /// called when all models off pushbutton pressed
      void slotModelsAllOff();

      /// called to read paint items in the dialog
      void readPaintColumnSelections();  
                
      /// called to read paint L-to-L, R-to-R
      void readPaintL2LR2R();  
                
      /// called to read paint name selections
      void readPaintNameSelections();
      
      /// called when paint name all on button is pressed
      void slotPaintNamesAllOnPushButton();
      
      /// called when paint name all off button is pressed
      void slotPaintNamesAllOffPushButton();
      
   private:
      /// volume animate direction
      enum VOLUME_ANIMATE_DIRECTION {
         /// increment slices
         VOLUME_ANIMATE_DIRECTION_INCREMENT,
         /// decrement slices
         VOLUME_ANIMATE_DIRECTION_DECREMENT
      };
      
      /// override of parent method
      virtual void resizeEvent(QResizeEvent* re);
      
      /// context menu event
      void contextMenuEvent(QContextMenuEvent* e);
      
      /// override of sizeHint (limits width of dialog but user can stretch)
      //virtual QSize sizeHint() const;
      
      /// update the data validity flag
      void updateDataValidityFlags();
      
      /// update volume selection page
      void updateVolumeSelectionPage();
      
      /// update volume settings page
      void updateVolumeSettingsPage();
      
      /// update volume surface outline page
      void updateVolumeSurfaceOutlinePage();
      
      /// Update the page selection combo box based upon enabled pages.
      void updatePageSelectionComboBox();

      /// create overlay/underlay surface page
      void createOverlayUnderlaySurfacePage();
      
      /// create the cocomac display sub page
      void createCocomacDisplayPage();
      
      /// create the cocomac file info sub page
      void createCocomacFileInformationPage();
      
      /// create the contour main page
      void createContourMainPage();
      
      /// create the contour class page
      void createContourClassPage();
      
      /// create the contour color page
      void createContourColorPage();
      
      /// create and update contour cell class buttons
      void createAndUpdateContourCellClassCheckBoxes();
      
      /// create and update contour color class buttons
      void createAndUpdateContourCellColorCheckBoxes();
      
      /// create the surface and volume page
      void createSurfaceAndVolumePage();
      
      /// create the surface misc page
      void createSurfaceMiscPage();
      
      /// create the probabilistic atlas surface main sub page
      void createProbAtlasSurfaceMainPage();
      
      /// create the probabilistic atlas surface area sub page
      void createProbAtlasSurfaceAreaPage();
      
      /// create the probabilistic atlas surface channel sub page
      void createProbAtlasSurfaceChannelPage();
      
      /// create and update the check boxes for prob atlas surface channels
      void createAndUpdateProbAtlasSurfaceChannelCheckBoxes();
      
      /// create and update the check boxes for prob atlas  surface
      void createAndUpdateProbAtlasSurfaceAreaNameCheckBoxes();
      
      /// create the probabilistic atlas volume main sub page
      void createProbAtlasVolumeMainPage();
      
      /// create the probabilistic atlas volume area sub page
      void createProbAtlasVolumeAreaPage();
      
      /// create the probabilistic atlas volume channel sub page
      void createProbAtlasVolumeChannelPage();
      
      /// create and update the check boxes for prob atlas volume channels
      void createAndUpdateProbAtlasVolumeChannelCheckBoxes();
      
      /// create and update the check boxes for prob atlas volume 
      void createAndUpdateProbAtlasVolumeAreaNameCheckBoxes();
      
      /// create the topography page
      void createTopographyPage();
      
      /// create the rgb paint page
      void createRgbPaintPage();
      
      /// create the geodesic page
      void createGeodesicPage();
      
      /// create and update the geodesic page
      void createAndUpdateGeodesicPage();
      
      /// create the surface vector selection page
      void createSurfaceVectorSelectionPage();

      /// create the surface vector settings page
      void createSurfaceVectorSettingsPage();

      /// create the lat/lon page
      void createLatLonPage();
      
      /// create and update the lat/lon page
      void createAndUpdateLatLonPage();
      
      /// create the scene page
      void createScenePage();
      
      /// get the selected scenes
      std::vector<int> getSelectedScenes() const;
      
      /// set the selected scene
      void setSelectedSceneItem(const int item);
      
      /// create the region page
      void createRegionPage();
      
      /// create the metric selections page
      void createMetricSelectionPage();
      
      /// create the metric settings page
      void createMetricSettingsPage();
      
      /// create and update metric selection page
      void createAndUpdateMetricSelectionPage();
      
      /// create the shape settings page
      void createShapeSettingsPage();
      
      /// create the shape selection page
      void createShapeSelectionPage();
      
      /// read shape color mapping min/max
      void readShapeColorMapping();

      /// create the border main sub page
      void createBorderMainPage();
      
      /// create the border name sub page
      void createBorderNamePage();
      
      /// create the border color sub page
      void createBorderColorPage();
      
      /// create and update border color toggles section
      void createAndUpdateBorderColorCheckBoxes();
      
      /// create and update border name toggles section
      void createAndUpdateBorderNameCheckBoxes();
      
      /// create the cell main sub page
      void createCellMainPage();
      
      /// create the cell class page
      void createCellClassPage();
      
      /// create the cell color page
      void createCellColorPage();
      
      /// create and update cell class toggles section
      void createAndUpdateCellClassCheckBoxes();
      
      /// create and update cell color toggles section
      void createAndUpdateCellColorCheckBoxes();
      
      /// create the deformation field page
      void createDeformationFieldPage();
      
      /// create the foci main sub page
      void createFociMainPage();
      
      /// create the foci class page
      void createFociClassPage();
      
      /// create the foci color page
      void createFociColorPage();
      
      /// create the foci name page
      void createFociNamePage();
      
      /// create the foci keywords page
      void createFociKeywordPage();
      
      /// create the foci tables page
      void createFociTablePage();
      
      /// create and update foci class toggles section
      void createAndUpdateFociClassCheckBoxes();
      
      /// create and update foci color toggles section
      void createAndUpdateFociColorCheckBoxes();
      
      /// create and update foci keywords toggles section
      void createAndUpdateFociKeywordCheckBoxes();
      
      /// create and update foci names toggles section
      void createAndUpdateFociNamesCheckBoxes();
      
      /// create and update foci table toggles section
      void createAndUpdateFociTableCheckBoxes();
      
      /// create the overlay underlay volume setttings page
      void createOverlayUnderlayVolumeSettingsPage();
      
      /// create the overlay underlay volume selection page
      void createOverlayUnderlayVolumeSelectionPage();
      
      /// create the overlay underlay volume surface outline page
      void createOverlayUnderlayVolumeSurfaceOutlinePage();
      
      /// create the images page
      void createImagesPage();
      
      /// create the models main page
      void createModelsMainPage();
      
      /// create and update the models main page
      void createAndUpdateModelsMainPage();
      
      /// create the model settings page
      void createModelsSettingsPage();
      
      /// create the paint column page
      void createPaintColumnPage();
      
      /// create and update the paint columns
      void createAndUpdatePaintColumnPage();
      
      /// create the paint name page
      void createPaintNamePage();
      
      /// create and update the paint names
      void createAndUpdatePaintNamePage();
      
      /// Create a surface model combo box
      void createSurfaceModelIndexComboBox();

      /// create the areal estimation page.
      void createArealEstimationPage();

      /// Create and update the areal estimation page.
      void createAndUpdateArealEstimationPage();

      /// Update the surface model combo box
      void updateSurfaceModelComboBoxes();

      /// print the sizes of each page when debugging is on
      void printPageSizesHelper(const QString& pageName,
                          QWidget* thePage);
      
      /// enumerated types for pages
      enum PAGE_NAME {
         PAGE_NAME_AREAL_ESTIMATION,
         PAGE_NAME_BORDER_MAIN,
         PAGE_NAME_BORDER_COLOR,
         PAGE_NAME_BORDER_NAME,
         PAGE_NAME_CELL_MAIN,
         PAGE_NAME_CELL_CLASS,
         PAGE_NAME_CELL_COLOR,
         PAGE_NAME_COCOMAC_DISPLAY,
         PAGE_NAME_COCOMAC_INFORMATION,
         PAGE_NAME_CONTOUR_MAIN,
         PAGE_NAME_CONTOUR_CLASS,
         PAGE_NAME_CONTOUR_COLOR,
         PAGE_NAME_DEFORMATION_FIELD,
         PAGE_NAME_FOCI_MAIN,
         PAGE_NAME_FOCI_CLASS,
         PAGE_NAME_FOCI_COLOR,
         PAGE_NAME_FOCI_KEYWORD,
         PAGE_NAME_FOCI_NAME,
         PAGE_NAME_FOCI_TABLE,
         PAGE_NAME_GEODESIC,
         PAGE_NAME_IMAGES,
         PAGE_NAME_LATLON,
         PAGE_NAME_METRIC_SELECTION,
         PAGE_NAME_METRIC_SETTINGS,
         PAGE_NAME_MODELS_MAIN,
         PAGE_NAME_MODELS_SETTINGS,
         PAGE_NAME_PAINT_COLUMN,
         PAGE_NAME_PAINT_NAMES,
         PAGE_NAME_PROB_ATLAS_SURFACE_MAIN,
         PAGE_NAME_PROB_ATLAS_SURFACE_AREA,
         PAGE_NAME_PROB_ATLAS_SURFACE_CHANNEL,
         PAGE_NAME_PROB_ATLAS_VOLUME_MAIN,
         PAGE_NAME_PROB_ATLAS_VOLUME_AREA,
         PAGE_NAME_PROB_ATLAS_VOLUME_CHANNEL,
         PAGE_NAME_REGION,
         PAGE_NAME_RGB_PAINT,
         PAGE_NAME_SCENE,
         PAGE_NAME_SHAPE_SELECTION,
         PAGE_NAME_SHAPE_SETTINGS,
         PAGE_NAME_SURFACE_AND_VOLUME,
         PAGE_NAME_SURFACE_MISC,
         PAGE_NAME_SURFACE_VECTOR_SELECTION,
         PAGE_NAME_SURFACE_VECTOR_SETTINGS,
         PAGE_NAME_SURFACE_OVERLAY_UNDERLAY,
         PAGE_NAME_TOPOGRAPHY,
         PAGE_NAME_VOLUME_SELECTION,
         PAGE_NAME_VOLUME_SETTINGS,
         PAGE_NAME_VOLUME_SURFACE_OUTLINE,
         PAGE_NAME_INVALID
      };
      
      /// get the name of a page
      QString getPageName(const PAGE_NAME pageName) const;
      
      /// show a display control page
      void showDisplayControlPage(const PAGE_NAME pageName,
                                  const bool updatePagesVisited);
      
      /// default size for this dialog
      QSize dialogDefaultSize;
      
      /// page selection combo box
      QComboBox* pageComboBox;
      
      /// page back tool button
      QToolButton* pageBackToolButton;
      
      /// page forward tool button
      QToolButton* pageForwardToolButton;
      
      /// tracks pages as they are visited
      std::vector<PAGE_NAME> pagesVisited;
      
      /// current index in pages visited
      int pagesVisitedIndex;
      
      /// items for page combo box
      std::vector<PAGE_NAME> pageComboBoxItems;
      
      /// page widget stack
      QStackedWidget* pageWidgetStack;
      
      /// scroll area for widget stack
      QScrollArea* widgetStackScrollArea;
      
      /// pop up a text area dialog about a data type
      void displayDataInfoDialog(const QString& title, const QString& info);
      
      /// text area dialog for data info
      QtTextEditDialog* dataInfoDialog;
      
      /// flag used when creating the dialog to prevent some problems
      bool creatingDialog;
      
      /// overlay/underlay page
      QWidget* pageOverlayUnderlaySurface;
      
      /// misc page
      QWidget* pageSurfaceMisc;
      
      /// prob atlas surface coloring to corresponding structures
      QCheckBox* probAtlasSurfaceApplySelectionToLeftAndRightStructuresFlagCheckBox;
      
      /// prob atlas surface main page
      QWidget* pageProbAtlasSurfaceMain;
      
      /// prob atlas surface channel page
      QWidget* pageProbAtlasSurfaceChannel;
      
      /// prob atlas surface channel sub page layout
      QVBoxLayout* probAtlasSurfaceSubPageChannelLayout;
      
      /// prob atlas surface area page
      QWidget* pageProbAtlasSurfaceArea;
      
      /// prob atlas surface area sub page layout
      QVBoxLayout* probAtlasSurfaceSubPageAreaLayout;
      
      /// prob atlas surface normal mode button
      QRadioButton* probAtlasSurfaceNormalButton;
      
      /// prob atlas surface threshold mode button
      QRadioButton* probAtlasSurfaceThresholdButton;
      
      /// prob atlas surface ??? as Unassigned button
      QCheckBox* probAtlasSurfaceUnassignedButton;
      
      /// number of prob atlas surface channel checkboxes being used
      int numValidProbAtlasSurfaceChannels;
      
      /// layout for probAtlasSurfaceChannelQVBox
      QGridLayout* probAtlasSurfaceChannelGridLayout;
      
      /// prob atlas surface channel button group
      QButtonGroup* probAtlasSurfaceChannelButtonGroup;
      
      /// prob atlas surface channel checkboxes
      std::vector<QCheckBox*> probAtlasSurfaceChannelCheckBoxes;
      
      /// number of prob atlas surface areas checkboxes being used
      int numValidProbAtlasSurfaceAreas;
      
      /// layout for probAtlasSurfaceAreasQVBox
      QGridLayout* probAtlasSurfaceAreasGridLayout;
      
      /// prob atlas surface areas button group
      QButtonGroup* probAtlasSurfaceAreasButtonGroup;
      
      /// prob atlas surface areas checkboxes
      std::vector<QCheckBox*> probAtlasSurfaceAreasCheckBoxes;
      
      /// prob atlas surface threshold display type ratio float spin box
      QDoubleSpinBox* probAtlasSurfaceThresholdRatioDoubleSpinBox;

      /// prob atlas volume main page
      QWidget* pageProbAtlasVolumeMain;
      
      /// prob atlas volume channel page
      QWidget* pageProbAtlasVolumeChannel;
      
      /// prob atlas volume channel sub page layout
      QVBoxLayout* probAtlasVolumeSubPageChannelLayout;
      
      /// prob atlas volume area sub page
      QWidget* pageProbAtlasVolumeArea;
      
      /// prob atlas volume area sub page
      QVBoxLayout* probAtlasVolumeSubPageAreaLayout;
      
      /// prob atlas volume normal mode button
      QRadioButton* probAtlasVolumeNormalButton;
      
      /// prob atlas volume threshold mode button
      QRadioButton* probAtlasVolumeThresholdButton;
      
      /// prob atlas volume ??? as Unassigned button
      QCheckBox* probAtlasVolumeUnassignedButton;
      
      /// number of prob atlas volume channel checkboxes being used
      int numValidProbAtlasVolumeChannels;
      
      /// layout for probAtlasVolumeChannelQVBox
      QGridLayout* probAtlasVolumeChannelGridLayout;
      
      /// prob atlas volume channel button group
      QButtonGroup* probAtlasVolumeChannelButtonGroup;
      
      /// prob atlas volume channel checkboxes
      std::vector<QCheckBox*> probAtlasVolumeChannelCheckBoxes;
      
      /// number of prob atlas volume areas checkboxes being used
      int numValidProbAtlasVolumeAreas;
      
      /// layout for probAtlasVolumeAreasQVBox
      QGridLayout* probAtlasVolumeAreasGridLayout;
      
      /// prob atlas volume areas button group
      QButtonGroup* probAtlasVolumeAreasButtonGroup;
      
      /// prob atlas volume areas checkboxes
      std::vector<QCheckBox*> probAtlasVolumeAreasCheckBoxes;
      
      /// prob atlas volume threshold display type ratio float spin box
      QDoubleSpinBox* probAtlasVolumeThresholdRatioDoubleSpinBox;

      /// topography page
      QWidget* pageTopography;
      
      /// rgb paint page
      QWidget* pageRgbPaintMain;
      
      /// rgb paint page file selection box
      QComboBox* rgbSelectionComboBox;
      
      /// rgb red selection checkbox
      QCheckBox* rgbRedCheckBox;
      
      /// rgb red name label
      QLabel* rgbRedNameLabel;
      
      /// rgb red threshold line edit
      QLineEdit* rgbRedThreshLineEdit;
      
      /// rgb red neg max label
      QLabel* rgbRedNegMaxLabel;
      
      /// rgb red pos max label
      QLabel* rgbRedPosMaxLabel;
      
      /// rgb green selection checkbox
      QCheckBox* rgbGreenCheckBox;
      
      /// rgb green name label
      QLabel* rgbGreenNameLabel;
      
      /// rgb green threshold line edit
      QLineEdit* rgbGreenThreshLineEdit;
      
      /// rgb green neg max label
      QLabel* rgbGreenNegMaxLabel;
      
      /// rgb green pos max label
      QLabel* rgbGreenPosMaxLabel;
      
      /// rgb blue selection checkbox
      QCheckBox* rgbBlueCheckBox;
      
      /// rgb blue name label
      QLabel* rgbBlueNameLabel;
      
      /// rgb blue threshold line edit
      QLineEdit* rgbBlueThreshLineEdit;
      
      /// rgb blue neg max label
      QLabel* rgbBlueNegMaxLabel;
      
      /// rgb blue pos max label
      QLabel* rgbBluePosMaxLabel;
      
      /// rgb positive only radio button
      QRadioButton* rgbPositiveOnlyRadioButton;
      
      /// rgb negative only radio button
      QRadioButton* rgbNegativeOnlyRadioButton;
      
      /// rgb apply coloring to corresponding structures
      QCheckBox* rgbApplySelectionToLeftAndRightStructuresFlagCheckBox;
      
      /// topography display type eccentricity radio button;
      QRadioButton* topographyTypeEccentricityRadioButton;
      
      /// topography display type polar angle radio button;
      QRadioButton* topographyPolarAngleRadioButton;
      
      /// topography file selection combo box
      QComboBox* topographyFileComboBox;
      
      /// surface shape minimum label
      QLabel* shapeViewMinimumLabel;
      
      /// surface shape maximum lable
      QLabel* shapeViewMaximumLabel;
      
      /// surface shape minimum color mapping
      QDoubleSpinBox* shapeMinimumMappingDoubleSpinBox;
      
      /// surface shape maximum color mapping
      QDoubleSpinBox* shapeMaximumMappingDoubleSpinBox;
      
      /// surfaced shape gray color map radio button
      QRadioButton* shapeColorMapGrayRadioButton;
      
      /// surfaced shape orange-yellow color map radio button
      QRadioButton* shapeColorMapOrangeYellowRadioButton;
      
      /// surfaced shape palette color map radio button
      QRadioButton* shapeColorMapPaletteRadioButton;
      
      /// shape apply coloring to corresponding structures
      QCheckBox* shapeApplySelectionToLeftAndRightStructuresFlagCheckBox;
      
      /// combo box for palette selection
      QComboBox* shapeColorMapPaletteComboBox;
      
      /// shape interpolate palette colors
      QCheckBox* shapeColorMapInterpolatePaletteCheckBox;
       
      /// button group for color map selections
      QButtonGroup* palColorMapButtonGroup;
      
      /// surface shape display color bar
      QCheckBox* shapeDisplayColorBarCheckBox;
      
      /// shape node id uncertainty combo box
      GuiNodeAttributeColumnSelectionComboBox* shapeNodeIdDeviationComboBox;
      
      /// shape Node ID Deviation Group Box
      QGroupBox* shapeNodeIdDeviationGroupBox;
      
      /// number of valid surface shape columns
      int numValidSurfaceShape;
      
      /// shape page for selections
      QWidget* pageSurfaceShapeSelections;

      /// shape page for settings
      QWidget* pageSurfaceShapeSettings;
      
      /// widget group for shape settings page
      WuQWidgetGroup* pageSurfaceShapeSettingsWidgetGroup;
      
      /// layout for shape sub page selections
      QVBoxLayout* surfaceShapeSubSelectionsLayout;
            
      /// layout for surface shape selection
      QGridLayout* surfaceShapeSelectionGridLayout;
      
      /// surface shape view radio button group
      QButtonGroup* surfaceShapeViewButtonGroup;
      
      /// surface shape comment push button group
      QButtonGroup* surfaceShapeCommentButtonGroup;

      /// surface shape metadata push button group
      QButtonGroup* surfaceShapeMetaDataButtonGroup;
      
      /// surface shape column number labels
      std::vector<QLabel*> surfaceShapeColumnNumberLabels;
      
      //// surface shape view radio buttons
      std::vector<QRadioButton*> surfaceShapeViewRadioButtons;
      
      /// surface shape comment push buttons
      std::vector<QPushButton*> surfaceShapeColumnCommentPushButtons;
      
      /// surface shape selection metadata push buttons
      std::vector<QPushButton*> surfaceShapeColumnMetaDataPushButtons;
      
      /// surface shape name line edits
      std::vector<QLineEdit*> surfaceShapeColumnNameLineEdits;
      
      /// metric selection page
      QWidget* pageMetricSelection;
      
      /// layout for metric sub page selections
      QVBoxLayout* metricSubPageSelectionsLayout;
      
      /// metric settings page
      QWidget* pageMetricSettings;
      
      /// widget group for metric settings page
      WuQWidgetGroup* pageMetricSettingsWidgetGroup;
      
      /// layout for metric selections
      QGridLayout* metricSelectionGridLayout;
      
      /// metric apply coloring to corresponding structures
      QCheckBox* metricApplySelectionToLeftAndRightStructuresFlagCheckBox;
      
      /// metric selection column labels
      std::vector<QLabel*> metricColumnNumberLabels;
      
      /// metric selection view radio buttons
      std::vector<QRadioButton*> metricViewRadioButtons;
      
      /// metric selection threshold radio buttons
      std::vector<QRadioButton*> metricThresholdRadioButtons;
      
      /// metric selection comment push buttons
      std::vector<QPushButton*> metricColumnCommentPushButtons;
      
      /// metric selection metadata push buttons
      std::vector<QPushButton*> metricColumnMetaDataPushButtons;
      
      /// metric selection line edits
      std::vector<QLineEdit*> metricColumnNameLineEdits;
      
      /// number of valid metrics
      int numValidMetrics;
      
      /// metric view radio button group
      QButtonGroup* metricViewButtonGroup;
      
      /// metric threshold radio button group
      QButtonGroup* metricThresholdButtonGroup;
      
      /// metric comment push button group
      QButtonGroup* metricCommentButtonGroup;
      
      /// metric metadata push button group
      QButtonGroup* metricMetaDataButtonGroup;
      
      /// metric display mode positiveradio button
      QRadioButton* metricDisplayModePositiveRadioButton;
      
      /// metric display mode negative radio button
      QRadioButton* metricDisplayModeNegativeRadioButton;
      
      /// metric display mode both radio button
      QRadioButton* metricDisplayModeBothRadioButton;
      
      /// metric positive threshold value
      QDoubleSpinBox* metricThresholdColumnPositiveDoubleSpinBox;
      
      /// metric negative threshold value
      QDoubleSpinBox* metricThresholdColumnNegativeDoubleSpinBox;
      
      /// metric average positive threshold value
      QDoubleSpinBox* metricThresholdAveragePositiveDoubleSpinBox;
      
      /// metric average negative threshold value
      QDoubleSpinBox* metricThresholdAverageNegativeDoubleSpinBox;
      
      /// metric user positive threshold value
      QDoubleSpinBox* metricThresholdUserPositiveDoubleSpinBox;
      
      /// metric user negative threshold value
      QDoubleSpinBox* metricThresholdUserNegativeDoubleSpinBox;
      
      /// metric threshold type combo box
      QComboBox* metricThresholdTypeComboBox;
      
      /// show thresholded regions check box
      QCheckBox* metricShowThresholdedRegionsCheckBox;
      
      /// metric animate spin box
      QSpinBox* metricAnimateSpinBox;
      
      /// metric popup graph on node ID
      QComboBox* metricGraphPopupComboBox;
      
      /// metric popup graph manual scaling
      QCheckBox* metricGraphManualScaleCheckBox;
      
      /// metric popup graph manual scaling min value float spin box
      QDoubleSpinBox* metricGraphManualScaleMinDoubleSpinBox;
      
      /// metric popup graph manual scaling max value float spin box
      QDoubleSpinBox* metricGraphManualScaleMaxDoubleSpinBox;
      
      /// metric thresholding column selection label
      QLabel* metricThresholdLabel;
      
      /// metric view column selection label
      QLabel* metricViewLabel;
      
      /// metric color mapping positive max
      QDoubleSpinBox* metricColorPositiveMaxDoubleSpinBox;
      
      /// metric color mapping positive min
      QDoubleSpinBox* metricColorPositiveMinDoubleSpinBox;
      
      /// metric color mapping negative max
      QDoubleSpinBox* metricColorNegativeMaxDoubleSpinBox;
      
      /// metric color mapping negative min
      QDoubleSpinBox* metricColorNegativeMinDoubleSpinBox;
      
      /// metric selected threshold column minimum label
      QLabel* metricThreshMinimumLabel;
      
      /// metric selected threshold column maximum label
      QLabel* metricThreshMaximumLabel;
      
      /// metric selected viewing column minimum label
      QLabel* metricViewMinimumLabel;
      
      /// metric selected viewing column maximum label
      QLabel* metricViewMaximumLabel;
      
      /// metric color mapping metric file auto scale radio button
      QRadioButton* metricFileAutoScaleRadioButton;
      
      /// metric file color mapping function volume auto scale radio button
      QRadioButton* metricFuncVolumeAutoScaleRadioButton;
      
      /// metric color mapping user scale radio button
      QRadioButton* metricUserScaleRadioButton;
      
      /// metric interpolate checkbox
      QCheckBox* metricColorInterpolateCheckBox;
      
      /// metric palette combo box
      QComboBox* metricPaletteComboBox;
      
      /// metric palette display color bar
      QCheckBox* metricDisplayColorBarCheckBox;
      
      /// border main page
      QWidget* pageBorderMain;
      
      /// border main page widget group (all widgets on foci main page)
      WuQWidgetGroup* pageBorderMainWidgetGroup;
      
      /// border color page
      QWidget* pageBorderColor;
      
      /// border color page widget group (all widgets on foci main page)
      WuQWidgetGroup* pageBorderColorWidgetGroup;
      
      /// border color sub page layout
      QVBoxLayout* borderSubPageColorLayout;
      
      /// border name page
      QWidget* pageBorderName;
      
      /// border name page widget group (all widgets on foci main page)
      WuQWidgetGroup* pageBorderNameWidgetGroup;
      
      /// layout for border name sub page
      QVBoxLayout* borderSubPageNameLayout;
      
      /// border name button group
      QButtonGroup* borderNameButtonGroup;
      
      /// layout for borderNameQVBox
      QGridLayout* borderNameGridLayout;
      
      /// border name checkboxes
      std::vector<QCheckBox*> borderNameCheckBoxes;
      
      /// border names associated with checkboxes
      std::vector<QString> borderNames;
      
      /// number of border names/checkboxes being used
      int numValidBorderNames;
      
      /// border color button group
      QButtonGroup* borderColorButtonGroup;
      
      /// layout for borderColorQVBox
      QGridLayout* borderColorGridLayout;
      
      /// border color checkboxes
      std::vector<QCheckBox*> borderColorCheckBoxes;
      
      /// number of border color check boxes being used
      int numValidBorderColors;
      
      /// cell main page
      QWidget* pageCellsMain;
      
      /// cell main page widget group
      WuQWidgetGroup* pageCellsMainWidgetGroup;
      
      /// cell color page
      QWidget* pageCellsColor;
      
      /// cell color page widget group
      WuQWidgetGroup* pageCellsColorWidgetGroup;
      
      /// cell color sub page layout
      QVBoxLayout* cellSubPageColorLayout;
      
      /// cell class page
      QWidget* pageCellsClass;
      
      /// cell class page widget group
      WuQWidgetGroup* pageCellsClassWidgetGroup;
      
      /// cell class sub page layout
      QVBoxLayout* cellSubPageClassLayout;
      
      /// cell class button group
      QButtonGroup* cellClassButtonGroup;
      
      /// layout for cellClassQVBox
      QGridLayout* cellColorGridLayout;
      
      /// cell class checkboxes
      std::vector<QCheckBox*> cellClassCheckBoxes;
      
      /// cell color button group
      QButtonGroup* cellColorButtonGroup;
      
      /// cell color checkboxes
      std::vector<QCheckBox*> cellColorCheckBoxes;
      
      /// layout for cellClassQVBox
      QGridLayout* cellClassGridLayout;
      
      /// number of valid cell color checkboxes
      int numValidCellColors;
      
      /// number of valid cell classes
      int numValidCellClasses;
      
      /// foci main page
      QWidget* pageFociMain;
      
      /// foci main page widget group (all widgets on foci main page)
      WuQWidgetGroup* pageFociMainWidgetGroup;
      
      /// foci color page
      QWidget* pageFociColor;
      
      /// foci color page widget group (all widgets on foci color page)
      WuQWidgetGroup* pageFociColorWidgetGroup;
      
      /// foci color sub page layout
      QVBoxLayout* fociSubPageColorLayout;
      
      /// foci class page
      QWidget* pageFociClass;
      
      /// foci class page widget group (all widgets on foci class page)
      WuQWidgetGroup* pageFociClassWidgetGroup;
      
      /// foci class sub page
      QVBoxLayout* fociSubPageClassLayout;
      
      /// foci class button group
      QButtonGroup* fociClassButtonGroup;
      
      /// foci keywords page
      QWidget* pageFociKeyword;
      
      /// foci keywords sub page layout
      QVBoxLayout* fociSubPageKeywordsLayout;
      
      /// foci main keyword widget group (all widgets on foci keywords page)
      WuQWidgetGroup* pageFociKeywordsWidgetGroup;
      
      /// foci keywords grid layout
      QGridLayout* fociKeywordGridLayout;
      
      /// button group for foci keyword checkboxes
      QButtonGroup* fociKeywordButtonGroup;
      
      /// foci keyword checkboxes
      std::vector<QCheckBox*> fociKeywordCheckBoxes;
      
      /// foci names page
      QWidget* pageFociName;
      
      /// foci name page widget group (all widgets on foci name page)
      WuQWidgetGroup* pageFociNameWidgetGroup;
      
      /// foci names sub page layout
      QVBoxLayout* fociSubPageNamesLayout;
      
      /// foci names grid layout
      QGridLayout* fociNamesGridLayout;
      
      /// button gropu for foci names checkboxes
      QButtonGroup* fociNamesButtonGroup;
      
      /// foci names checkboxes
      std::vector<QCheckBox*> fociNamesCheckBoxes;
      
      /// foci tables page
      QWidget* pageFociTable;
      
      /// foci tables page widget group (all widgets on foci table page)
      WuQWidgetGroup* pageFociTableWidgetGroup;
      
      /// foci tables sub page layout
      QVBoxLayout* fociSubPageTablesLayout;
      
      /// foci tables grid layout
      QGridLayout* fociTablesGridLayout;
      
      /// button group for foci tables checkboxes
      QButtonGroup* fociTablesButtonGroup;
      
      /// foci table checkboxes
      std::vector<QCheckBox*> fociTablesCheckBoxes;
      
      /// layout for fociColorQVBox
      QGridLayout* fociColorGridLayout;
      
      /// foci class checkboxes
      std::vector<QCheckBox*> fociClassCheckBoxes;
      
      /// foci color button group
      QButtonGroup* fociColorButtonGroup;
      
      /// foci color checkboxes
      std::vector<QCheckBox*> fociColorCheckBoxes;
      
      /// layout for fociClassQVBox
      QGridLayout* fociClassGridLayout;
      
      /// number of valid foci color checkboxes
      int numValidFociColors;
      
      /// number of valid foci classes
      int numValidFociClasses;
      
      /// display cells without class assignments check box
      QCheckBox* fociWithoutClassAssignmentsCheckBox;
      
      /// display cells without matching color check box
      QCheckBox* fociWithoutMatchingColorCheckBox;
      
      /// display cells without a link to a study with keywords check box
      QCheckBox* fociWithoutLinkToStudyWithKeywordsCheckBox;
      
      /// display cells without a link to a table subheader check box
      QCheckBox* fociWithoutLinkToStudyWithTableSubHeaderCheckBox;
      
      /// "none" selection items
      QRadioButton* primaryOverlayNoneButton;
      QRadioButton* secondaryOverlayNoneButton;
      QRadioButton* underlayNoneButton;
      
      /// update the areal estimation combo box
      void updateArealEstOverlayUnderlaySelections();
      
      /// areal estimation selection items
      QComboBox* arealEstSelectionComboBox;
      QRadioButton* primaryOverlayArealEstButton;
      QRadioButton* secondaryOverlayArealEstButton;
      QRadioButton* underlayArealEstButton;
      QLabel* arealEstSelectionLabel;
      QPushButton* arealEstInfoPushButton;
      
      /// cocmac selection items
      QRadioButton* primaryOverlayCocomacButton;
      QRadioButton* secondaryOverlayCocomacButton;
      QRadioButton* underlayCocomacButton;
      QLabel* cocomacSelectionLabel;
            
      /// metric selection items
      QComboBox* metricSelectionComboBox;
      QRadioButton* primaryOverlayMetricButton;
      QRadioButton* secondaryOverlayMetricButton;
      QRadioButton* underlayMetricButton;
      QLabel* metricSelectionLabel;
      QPushButton* metricInfoPushButton;

      /// paint selection items
      QComboBox* paintSelectionComboBox;
      QRadioButton* primaryOverlayPaintButton;
      QRadioButton* secondaryOverlayPaintButton;
      QRadioButton* underlayPaintButton;
      QLabel* paintSelectionLabel;
      QPushButton* paintInfoPushButton;
      
      /// prob atlas selection items
      QRadioButton* primaryOverlayProbAtlasSurfaceButton;
      QRadioButton* secondaryOverlayProbAtlasSurfaceButton;
      QRadioButton* underlayProbAtlasSurfaceButton;
      QLabel* probAtlasSurfaceSelectionLabel;
      QPushButton* probAtlasSurfaceInfoPushButton;
      
      /// rgb paint selection items
      QComboBox* rgbPaintSelectionComboBox;
      QRadioButton* primaryOverlayRgbPaintButton;
      QRadioButton* secondaryOverlayRgbPaintButton;
      QRadioButton* underlayRgbPaintButton;
      QLabel* rgbPaintSelectionLabel;
      QPushButton* rgbPaintInfoPushButton;
      
      /// surface shape selection items
      QComboBox* shapeSelectionComboBox;
      QRadioButton* primaryOverlayShapeButton;
      QRadioButton* secondaryOverlayShapeButton;
      QRadioButton* underlayShapeButton;
      QLabel* shapeSelectionLabel;
      QPushButton* shapeInfoPushButton;
      
      /// topography selection items
      QComboBox* topographySelectionComboBox;
      QRadioButton* primaryOverlayTopographyButton;
      QRadioButton* secondaryOverlayTopographyButton;
      QRadioButton* underlayTopographyButton;
      QLabel* topographySelectionLabel;
      
      /// crossover selection items
      QRadioButton* primaryOverlayCrossoversButton;
      
      /// edges selection items
      QRadioButton* primaryOverlayEdgesButton;
      
      /// sections items
      QRadioButton* primaryOverlaySectionsButton;
      QLabel*       primaryOverlaySectionsLabel;
      QLineEdit*    primaryOverlaySectionsLineEdit;
      
      /// geography blending (underlay only)
      QRadioButton* underlayGeographyBlendingButton;
      QLabel* geographyBlendingSelectionLabel;
      QDoubleSpinBox* geographyBlendingDoubleSpinBox;
      
      /// lighting for primary overlay
      QCheckBox* primaryOverlayLightingButton;
      QCheckBox* secondaryOverlayLightingButton;
      QCheckBox* underlayLightingButton;
      
      /// opacity spin box
      QDoubleSpinBox* opacityDoubleSpinBox;
      
      /// show borders check box
      QCheckBox* showBordersCheckBox;
      
      /// show raised borders check box
      QCheckBox* showRaisedBordersCheckBox;
      
      /// show border uncertainty vectors check box
      QCheckBox* showUncertaintyBordersCheckBox;
      
      /// show border's first link in red check box
      QCheckBox* showFirstLinkRedBordersCheckBox;
      
      /// border draw type combo box
      QComboBox* bordersDrawTypeComboBox;
      
      /// border size spin box
      QDoubleSpinBox* borderSizeSpinBox;
      
      /// border symbol combo box
      QComboBox* borderSymbolComboBox;
      
      /// border opacity
      QDoubleSpinBox* borderOpacityDoubleSpinBox;
      
      /// border stretched lines stretch factor 
      QDoubleSpinBox* unstretchedBordersDoubleSpinBox;
      
      /// override border colors with area colors check box
      QCheckBox* overrideBorderWithAreaColorsCheckBox;
      
      /// show foci check box;
      QCheckBox* showFociCheckBox;
      
      /// show volume foci check box
      QCheckBox* showVolumeFociCheckBox;
      
      /// show raised foci check box
      QCheckBox* showRaisedFociCheckBox;
      
      /// show pasted onto 3D foci check box
      QCheckBox* showPastedOnto3DFociCheckBox;
      
      /// foci symbol override combo box
      QComboBox* fociSymbolOverrideComboBox;
      
      /// foci opacity double spin box
      QDoubleSpinBox* fociOpacityDoubleSpinBox;
      
      /// foci size spin box
      QDoubleSpinBox* fociSizeSpinBox;
      
      /// foci distance spin box
      QDoubleSpinBox* fociDistSpinBox;
      
      /// foci coloring mode combo box
      QComboBox* fociColorModeComboBox;
      
      // show right hemisphere foci
      //QCheckBox* showRightHemisphereFociCheckBox;
      
      // show left hemisphere foci
      //QCheckBox* showLeftHemisphereFociCheckBox;
      
      /// show cells on correct hem only
      QCheckBox* showCorrectHemisphereCellsCheckBox;
      
      /// show cells check box;
      QCheckBox* showCellsCheckBox;
      
      /// show volume cells check box;
      QCheckBox* showVolumeCellsCheckBox;
      
      /// show raised cell check box
      QCheckBox* showRaisedCellCheckBox;
      
      /// cell symbol override combo box
      QComboBox* cellSymbolOverrideComboBox;
      
      /// cell opacity
      QDoubleSpinBox* cellOpacityDoubleSpinBox;
      
      /// cell display modes combo box
      QComboBox* cellDisplayModeComboBox;
      
      /// cell size spin box
      QDoubleSpinBox* cellSizeSpinBox;
      
      /// cell distance spin box
      QDoubleSpinBox* cellDistSpinBox;
      
      // show right hemisphere cell
      //QCheckBox* showRightHemisphereCellCheckBox;
      
      // show left hemisphere cell
      //QCheckBox* showLeftHemisphereCellCheckBox;
      
      /// show foci on correct hem only
      QCheckBox* showCorrectHemisphereFociCheckBox;
      
      /// active fiducial combo box
      GuiBrainModelSelectionComboBox* miscActiveFiducialComboBox;
      
      /// draw mode combo box
      QComboBox* miscDrawModeComboBox;
      
      /// brightness line edit
      QLineEdit* miscBrightnessLineEdit;
      
      /// contrast line edit
      QLineEdit* miscContrastLineEdit;
      
      /// node size spin box
      QDoubleSpinBox* miscNodeSizeSpinBox;
      
      /// link size spin box
      QDoubleSpinBox* miscLinkSizeSpinBox;
      
      /// identify node color combo box
      QComboBox* miscIdentifyNodeColorComboBox;
      
      /// show normals check box
      QCheckBox* miscShowNormalsCheckBox;
      
      /// show total forces check box
      QCheckBox* miscTotalForcesCheckBox;
      
      /// show angular forces check box
      QCheckBox* miscAngularForcesCheckBox;
      
      /// show linear forces check box
      QCheckBox* miscLinearForcesCheckBox;
      
      /// force length float spin box
      QDoubleSpinBox* miscForceVectorLengthDoubleSpinBox;
      
      /// surface axes group box
      QGroupBox* miscAxesGroupBox;
      
      /// surface axes
      QCheckBox* miscAxesShowLettersCheckBox;
      
      /// surface axes
      QCheckBox* miscAxesShowTickMarksCheckBox;
      
      /// surface axes
      QDoubleSpinBox* miscAxesLengthDoubleSpinBox;
      
      /// surface axes offset
      QDoubleSpinBox* miscAxesOffsetDoubleSpinBox[3];
      
      /// partial view combo box
      QComboBox* miscPartialViewComboBox;
      
      /// projection combo box
      QComboBox* miscProjectionComboBox;
      
      /// deformation field page
      QWidget* pageDeformationField;
      
      /// deformation field mode combo box
      QComboBox* deformationFieldModeComboBox;
      
      /// deformation field show vectors on identified nodes check box
      QCheckBox* deformationFieldShowIdNodesCheckBox;
      
      /// deformation field sparse distance spin box
      QSpinBox* deformationFieldSparseDistanceSpinBox;
      
      /// deformation field select column combo box
      GuiNodeAttributeColumnSelectionComboBox* deformationFieldComboBox;
      
      /// deformation field show unstretched on flat surface
      QCheckBox* deformationFieldShowUnstretchedCheckBox;
      
      /// deformation field unstretched factor
      QDoubleSpinBox* deformationFieldUnstretchedDoubleSpinBox;
      
      /// Cocomac display page
      QWidget* pageCocomacDisplay;
      
      /// Cocomac file info sub page
      QWidget* pageCocomacInformation;
      
      /// Cocoamc afferent connection  radio button
      QRadioButton* cocomacAfferentRadioButton;

      /// Cocoamc efferent connection  radio button
      QRadioButton* cocomacEfferentRadioButton;

      /// Cocoamc afferent and efferent connection  radio button
      QRadioButton* cocomacAfferentAndEfferentRadioButton;

      /// Cocoamc afferent or efferent connection  radio button
      QRadioButton* cocomacAfferentOrEfferentRadioButton;

      /// Cocomac paint column combo box
      QComboBox* cocomacPaintColumnComboBox;
      
      /// Cocomac version label
      QLabel* cocomacVersionLabel;
      
      /// Cocomac export date label
      QLabel* cocomacExportDate;
      
      /// Cocomac data type label
      QLabel* cocomacDataType;
      
      /// Cocomac  text area
      QTextEdit* cocomacCommentsTextEdit;
      
      /// Cocomac  text area
      QTextEdit* cocomacProjectionsTextEdit;
  
      /// Contour main page
      QWidget* pageContourMain;
      
      /// contour cell class sub page
      QWidget* pageContourClass;
      
      /// contour cell class sub page layout
      QVBoxLayout* contourSubPageClassLayout;
      
      /// contour color class sub page
      QWidget* pageContourColor;
      
      /// contour color class sub page layout
      QVBoxLayout* contourSubPageColorLayout;
      
      /// contour draw mode combo box
      QComboBox* contourDrawModeComboBox;
      
      /// contour origin cross check box
      QCheckBox* contourOriginCrossCheckBox;
      
      /// Contour show end points check box
      QCheckBox* contourShowEndPointsCheckBox;
      
      /// Contour point size spin box
      QDoubleSpinBox* contourPointSizeSpinBox;
      
      /// Contour line size spin box
      QDoubleSpinBox* contourLineThicknessSpinBox;
      
      /// contour cell color button group
      QButtonGroup* contourCellColorButtonGroup;
      
      /// layout for contourCellColorQVBox
      QGridLayout* contourCellColorGridLayout;
      
      /// contour cell class button group
      QButtonGroup* contourCellClassButtonGroup;
      
      /// layout for contourCellClassQVBox
      QGridLayout* contourCellClassGridLayout;
      
      /// number of valid contour cell classes
      int numValidContourCellClasses;
      
      /// number of valid contour cell colors
      int numValidContourCellColors;

      /// show contour cells check box
      QCheckBox* contourShowCellsCheckBox;
      
      /// contour cell size spin box
      QSpinBox* contourCellSizeSpinBox;
      
      /// contour cell class check boxes
      std::vector<QCheckBox*> contourCellClassCheckBoxes;
      
      /// contour cell color check boxes
      std::vector<QCheckBox*> contourCellColorCheckBoxes;
      
      /// the volume selection page 
      QWidget* pageVolumeSelection;
      
      /// the volume settings page 
      QWidget* pageVolumeSettings;
      
      /// widget group for volume settings page
      WuQWidgetGroup* pageVolumeSettingsWidgetGroup;
      
      /// the volume surface outline page 
      QWidget* pageVolumeSurfaceOutline;
      
      /// widget group for volume surface outline page
      WuQWidgetGroup* pageVolumeSurfaceOutlineWidgetGroup;
      
      /// volume primary overlay none button
      QRadioButton* volumePrimaryOverlayNoneButton;
      
      /// volume secondary overlay none button
      QRadioButton* volumeSecondaryOverlayNoneButton;
      
      /// volume underlay none button
      QRadioButton* volumeUnderlayNoneButton;
      
      /// volume primary overlay anatomy radio button
      QRadioButton* volumePrimaryOverlayAnatomyRadioButton;
      
      /// volume secondary overlay anatomy radio button
      QRadioButton* volumeSecondaryOverlayAnatomyRadioButton;
      
      /// volume underlay radio anatomy button
      QRadioButton* volumeUnderlayAnatomyRadioButton;
      
      /// volume anatomy combo box
      QComboBox* volumeAnatomyComboBox;
      
      /// volume anatomy label
      QLabel* volumeAnatomyLabel;
      
      /// volume functional primary overlay radio button
      QRadioButton* volumePrimaryOverlayFunctionalRadioButton;
      
      /// volume functional overlay secondary overlay radio button
      QRadioButton* volumeSecondaryOverlayFunctionalRadioButton;
      
      /// volume functional underlay radio button
      QRadioButton* volumeUnderlayFunctionalRadioButton;
      
      /// volume functional view combo box
      QComboBox* volumeFunctionalViewComboBox;
      
      /// volume functional threshold combo box
      QComboBox* volumeFunctionalThresholdComboBox;
      
      /// volume functional view label
      QLabel* volumeFunctionalViewLabel;
      
      /// volume functional threshold label
      QLabel* volumeFunctionalThresholdLabel;
      
      /// volume paint primary overlay radio button
      QRadioButton* volumePrimaryOverlayPaintRadioButton;
      
      /// volume paint secondary overlay radio button
      QRadioButton* volumeSecondaryOverlayPaintRadioButton;
      
      /// volume paint underlay radio button
      QRadioButton* volumeUnderlayPaintRadioButton;
      
      /// volume paint combo box
      QComboBox* volumePaintComboBox;
      
      /// volume paint label
      QLabel* volumePaintLabel;
      
      /// volume prob atlas primary overlay radio button
      QRadioButton* volumePrimaryOverlayProbAtlasRadioButton;
      
      /// volume prob atlas secondary overlay radio button
      QRadioButton* volumeSecondaryOverlayProbAtlasRadioButton;
      
      /// volume prob atlas underlay radio button
      QRadioButton* volumeUnderlayProbAtlasRadioButton;
            
      /// volume prob atlas label
      QLabel* volumeProbAtlasLabel;

      /// volume rgb primary overlay radio button
      QRadioButton* volumePrimaryOverlayRgbRadioButton;
      
      /// volume rgb secondary overlay radio button
      QRadioButton* volumeSecondaryOverlayRgbRadioButton;
      
      /// volume rgb underlay radio button
      QRadioButton* volumeUnderlayRgbRadioButton;
      
      /// volume rgb combo box
      QComboBox* volumeRgbComboBox;
      
      /// volume rgb label
      QLabel* volumeRgbLabel;
      
      /// volume segementation primary overlay radio button
      QRadioButton* volumePrimaryOverlaySegmentationRadioButton;
      
      /// volume segementation secondary overlay radio button
      QRadioButton* volumeSecondaryOverlaySegmentationRadioButton;
      
      /// volume segementation underlay radio button
      QRadioButton* volumeUnderlaySegmentationRadioButton;
      
      /// volume segementation combo box
      QComboBox* volumeSegmentationComboBox;
      
      /// volume segmentation transparency
      QDoubleSpinBox* volumeSegmentationTranslucencyDoubleSpinBox;
      
      /// volume segmentation label
      QLabel* volumeSegmentationLabel;
      
      /// volume vector primary overlay radio button
      QRadioButton* volumePrimaryOverlayVectorRadioButton;
      
      /// volume vector secondary overlay radio button
      QRadioButton* volumeSecondaryOverlayVectorRadioButton;
      
      /// volume vector underlay radio button
      QRadioButton* volumeUnderlayVectorRadioButton;
      
      /// volume vector combo box
      QComboBox* volumeVectorComboBox;
      
      /// volume vector label
      QLabel* volumeVectorLabel;
      
      /// volume functional display color bar
      QCheckBox* volumeFunctionalDisplayColorBarCheckBox;
      
      /// volume overlay opacity float spin box
      QDoubleSpinBox* volumeOverlayOpacityDoubleSpinBox;
      
      /// volume anatomy info push button
      QPushButton* volumeAnatomyInfoPushButton;
      
      /// volume functional view info push button
      QPushButton* volumeFunctionalViewInfoPushButton;
      
      /// volume functional thresh info push button
      QPushButton* volumeFunctionalThreshInfoPushButton;
      
      /// volume paint info push button
      QPushButton* volumePaintInfoPushButton;
      
      /// volume prob atlas info push button
      QPushButton* volumeProbAtlasInfoPushButton;
      
      /// volume rgb info push button
      QPushButton* volumeRgbInfoPushButton;
      
      /// volume segmentation info push button
      QPushButton* volumeSegmentationInfoPushButton;
      
      /// volume vector info push button
      QPushButton* volumeVectorInfoPushButton;
      
      /// volume anatomy metadata push button
      QPushButton* volumeAnatomyMetaDataPushButton;
      
      /// volume functional view metadata push button
      QPushButton* volumeFunctionalViewMetaDataPushButton;
      
      /// volume functional thresh metadata push button
      QPushButton* volumeFunctionalThreshMetaDataPushButton;
      
      /// volume paint metadata push button
      QPushButton* volumePaintMetaDataPushButton;
      
      /// volume prob atlas metadata push button
      QPushButton* volumeProbAtlasMetaDataPushButton;
      
      /// volume rgb metadata push button
      QPushButton* volumeRgbMetaDataPushButton;
      
      /// volume segmentation metadata push button
      QPushButton* volumeSegmentationMetaDataPushButton;
      
      /// volume vector metadata push button
      QPushButton* volumeVectorMetaDataPushButton;
      
      /// segmentation volume drawing type combo box
      QComboBox* volumeSegmentationDrawTypeComboBox;
      
      /// anatomy volume drawing type combo box
      QComboBox* volumeAnatomyDrawTypeComboBox;
      
      /// anatomy volume brightness control
      QSlider* volumeAnatomyBrightnessSlider;

      /// anatomy volume brightness label
      QLabel* volumeAnatomyBrightnessLabel;
      
      /// anatomy volume  contrast label
      QLabel* volumeAnatomyContrastLabel;
      
      /// anatomy volume contrast control
      QSlider* volumeAnatomyContrastSlider;

      /// vector volume group box
      QGroupBox* vectorVolumeGroupBox;
      
      /// vector volume sparsity spin box
      QSpinBox* vectorVolumeSparsitySpinBox;
      
      /// volume montage group box
      QGroupBox* volumeMontageGroupBox;
      
      /// volume montage rows spin box
      QSpinBox* volumeMontageRowsSpinBox;
      
      /// volume montage columns spin box
      QSpinBox* volumeMontageColumnsSpinBox;
      
      /// volume montage slice increment spin box
      QSpinBox* volumeMontageSliceIncrementSpinBox;
      
      /// brain model combo box for volume overlay surface
      GuiBrainModelSelectionComboBox* volumeOverlaySurfaceSelectionComboBox[DisplaySettingsVolume::MAXIMUM_OVERLAY_SURFACES];
      
      /// checkbox for volume overlay surface outline
      QCheckBox* volumeShowOverlaySurfaceOutlineCheckBox[DisplaySettingsVolume::MAXIMUM_OVERLAY_SURFACES];
      
      /// show volume overlay surface outline color combo box
      QComboBox* volumeOverlaySurfaceOutlineColorComboBox[DisplaySettingsVolume::MAXIMUM_OVERLAY_SURFACES];
      
      /// show volume overlay surface outline thickness float spin box
      QDoubleSpinBox* volumeOverlaySurfaceOutlineThicknessDoubleSpinBox[DisplaySettingsVolume::MAXIMUM_OVERLAY_SURFACES];
      
      /// show the coordinates of the volume crosshairs
      QCheckBox* volumeShowCrosshairCoordinatesCheckBox;
      
      /// oblique slices view matrix control
      GuiTransformationMatrixSelectionControl* obliqueVolumeSliceMatrixControl;
      
      /// oblique slices sampling size float spin box
      QDoubleSpinBox* obliqueSlicesSamplingSizeDoubleSpinBox;
      
      /// show the volume crosshairs
      QCheckBox* volumeShowCrosshairsCheckBox;
      
      /// surface and volume page
      QWidget* pageSurfaceAndVolume;
      
      /// surface and volume page widget group
      WuQWidgetGroup* pageSurfaceAndVolumeWidgetGroup;
      
      /// surface and volume draw black anatomy voxels
      QCheckBox* surfaceAndVolumeAnatomyBlackCheckBox;
      
      /// surface and volume display horizontal slice check box
      QCheckBox* surfaceAndVolumeHorizontalSliceCheckBox;
      
      /// surface and volume display parasagittal slice check box
      QCheckBox* surfaceAndVolumeParasagittalSliceCheckBox;
      
      /// surface and volume display coronal slice check box
      QCheckBox* surfaceAndVolumeCoronalSliceCheckBox;
      
      /// surface and volume horizontal slice spin box
      QSpinBox* surfaceAndVolumeHorizontalSliceSpinBox;
      
      /// surface and volume parasagittal slice spin box
      QSpinBox* surfaceAndVolumeParasagittalSliceSpinBox;
      
      /// surface and volume coronal slice spin box
      QSpinBox* surfaceAndVolumeCoronalSliceSpinBox;
            
      /// surface and volume show surface check box
      QCheckBox* surfaceAndVolumeShowSurfaceCheckBox;
      
      /// surface and volume show functional volume on slices
      QCheckBox* surfaceAndVolumeShowFunctionalCloudCheckBox;
      
      /// surface and volume functional volume group box
      QGroupBox* surfaceAndVolumeFunctionalGroupBox;
      
      /// surface and volume show primary overlay on slices
      QCheckBox* surfaceAndVolumeShowPrimaryCheckBox;
      
      /// surface and volume show secondary overlay on slices
      QCheckBox* surfaceAndVolumeShowSecondaryCheckBox;
      
      /// surface and volume functional volume opacity float spin box
      QDoubleSpinBox* surfaceAndVolumeFunctionalCloudOpacitySpinBox;
      
      /// surface and volume functional volume opacity check box
      QCheckBox* surfaceAndVolumeFunctionalCloudOpacityCheckBox;
      
      /// surface and volume functional distance threshold
      QDoubleSpinBox* surfaceAndVolumeFunctionalDistanceSpinBox;
      
      /// surface and volume segmentation volume group box
      QGroupBox* surfaceAndVolumeSegmentationGroupBox;
      
      /// surface and volume show segmentation volume on slices
      QCheckBox* surfaceAndVolumeShowSegmentationCloudCheckBox;
      
      /// box for vector items
      QGroupBox* surfaceAndVolumeVectorVBox;
      
      /// surface and volume show vector volume cloud
      QCheckBox* surfaceAndVolumeShowVectorCloudCheckBox;
      
      /// the images page
      QWidget* pageImages;
      
      /// show image in main window checkbox
      QCheckBox* showImageInMainWindowCheckBox;
      
      /// images button group
      QButtonGroup* imagesButtonGroup;
      
      /// layout for iamges
      QVBoxLayout* imagesLayout;
      
      /// images selection buttons
      std::vector<QRadioButton*> imagesRadioButtons;
      
      /// models main page
      QWidget* pageModelsMain;
      
      /// models settings page
      QWidget* pageModelsSettings;
      
      /// models main sub page layout
      QVBoxLayout* modelsSubPageMainLayout;
      
      /// models opacity float spin box
      QDoubleSpinBox* modelsOpacityDoubleSpinBox;
      
      /// models line size float spin box
      QDoubleSpinBox* modelsLineSizeDoubleSpinBox;
      
      /// models vertex size float spin box
      QDoubleSpinBox* modelsVertexSizeDoubleSpinBox;
      
      /// minimum line size
      float minLineSize;
      
      /// maximum line size
      float maxLineSize;
      
      /// minimum point size
      float minPointSize;
      
      /// maximum point size
      float maxPointSize;
      
      /// current number of models in brain set
      int numValidModels;
      
      /// checkbuttons for controlling display of models
      std::vector<QCheckBox*> modelCheckBoxes;
      
      /// transform selection for display of models
      std::vector<GuiTransformationMatrixSelectionControl*> modelTransformControls;
      
      /// layout for modelSelectionGridWidget
      QGridLayout* modelSelectionGridWidgetLayout;
      
      /// model vertices lighting check box
      QCheckBox* modelVerticesLightingCheckBox;
      
      /// model lines lighting check box
      QCheckBox* modelLinesLightingCheckBox;
      
      /// model polygons lighting check box
      QCheckBox* modelPolygonsLightingCheckBox;
      
      /// paint name page
      QWidget* pagePaintName;
      
      /// paint name checkbox layout
      QGridLayout* paintNameCheckBoxGridLayout;
      
      /// paint name checkboxes
      std::vector<QCheckBox*> paintNameCheckBoxes;
      
      /// paint name checkbox paint file indices
      std::vector<int> paintNameCheckBoxPaintFileNameIndices;
      
      /// paint name checkbox widget groupt
      WuQWidgetGroup* paintNameCheckBoxWidgetGroup;
      
      /// top level paint page
      QWidget* pagePaintColumn;
      
      /// widget group for paint column page
      WuQWidgetGroup* pagePaintColumnWidgetGroup;
      
      /// paint page layout
      QVBoxLayout* paintPageLayout;
      
      /// number of valid paint
      int numValidPaints;
      
      /// labels for paint column numbers
      std::vector<QLabel*> paintColumnNameLabels;
      
      /// radio buttons for paint selection
      std::vector<QRadioButton*> paintColumnRadioButtons;
      
      /// line edits for paint names
      std::vector<QLineEdit*> paintColumnNameLineEdits;
      
      /// layout for paint radio button and text boxes
      QGridLayout* paintColumnSelectionGridLayout;
      
      /// button group for paint radio buttons
      QButtonGroup* paintColumnButtonGroup;
      
      /// button group for paint comment buttons
      QButtonGroup* paintColumnCommentButtonGroup;
      
      /// button group for paint metadata buttons
      QButtonGroup* paintColumnMetaDataButtonGroup;
      
      /// paint apply coloring to corresponding structures
      QCheckBox* paintApplySelectionToLeftAndRightStructuresFlagCheckBox;
      
      /// paint column comment push buttons
      std::vector<QPushButton*> paintColumnCommentPushButtons;
      
      /// paint column metadat push buttons
      std::vector<QPushButton*> paintColumnMetaDataPushButtons;
      
      ///  radio button on overlay underlay surface page
      QCheckBox* layersBorderCheckBox;
      
      ///  radio button on overlay underlay surface page
      QCheckBox* layersCellsCheckBox;
      
      ///  radio button on overlay underlay surface page
      QCheckBox* layersFociCheckBox;
      
      /// current surface being controlled
      QComboBox* surfaceModelIndexComboBox;
      
      /// surface model combo box to brain model surface indices
      std::vector<int> surfaceModelIndexComboBoxValues;
      
      /// current brain model surface index that is being controlled
      /// If negative then controlling all surfaces
      int surfaceModelIndex;
      
      /// horizontal box containing surface model index selection
      QGroupBox* surfaceModelGroupBox;
      
      /// primary overlay radio button group
      QButtonGroup* primaryOverlayButtonGroup;
      
      /// secondary overlay radio button group
      QButtonGroup* secondaryOverlayButtonGroup;
      
      /// underlay radio button group
      QButtonGroup* underlayButtonGroup;
      
      /// volume animate direction combo box
      QComboBox* volumeAnimateDirectionComboBox;
      
      /// animate volume flag
      bool continueVolumeAnimation;
      
      /// scene main page
      QWidget* pageSceneMain;
      
      /// skip updating of scene page
      bool skipScenePageUpdate;
      
      /// scene list box
      QListWidget* sceneListBox;
      
      /// scene preserve foci, foci colors, and study metadata checkbox
      QCheckBox* scenePreserveFociCheckBox;
      
      /// scene window position combo box
      QComboBox* sceneWindowPositionComboBox;
      
      /// region main page
      QWidget* pageRegionMain;
      
      /// region all time courses check box
      QRadioButton* regionAllTimeCoursesCheckBox;
      
      /// region single time course check box
      QRadioButton* regionSingleTimeCourseCheckBox;
      
      /// region time course combo box
      QComboBox* regionTimeCourseComboBox;

      /// region case combo box
      QComboBox* regionCaseComboBox;

      /// region paint volume
      QComboBox* regionPaintVolumeComboBox;

      /// region popup graph check box
      QCheckBox* regionPopupGraphCheckBox;

      /// region popup graph auto scale radio button
      QRadioButton* regionGraphAutoScaleRadioButton;
      
      /// region popup graph user scale radio button
      QRadioButton* regionGraphUserScaleRadioButton;
      
      /// region popup graph user scale minimum line edit
      QLineEdit* regionGraphUserScaleMinLineEdit;
      
      /// region popup graph user scale maximum line edit
      QLineEdit* regionGraphUserScaleMaxLineEdit;
      
      /// paint medial wall override check box
      QCheckBox* paintMedWallCheckBox;
      
      /// paint medial wall column combo box
      GuiNodeAttributeColumnSelectionComboBox* paintMedWallColumnComboBox;
      
      /// lat/lon main page
      QWidget* pageLatLonMain;
      
      /// layout for lat/lon main page
      QVBoxLayout* latLonMainPageLayout;
      
      /// lat/lon comment button group
      QButtonGroup* latLonCommentButtonGroup;
      
      /// lat/lon grid layout
      QGridLayout* latLonSelectionGridLayout;
      
      /// number of valid lat/lon columns
      int numValidLatLon;
      
      /// lat lon comment push buttons
      std::vector<QPushButton*> latLonColumnCommentPushButtons;
      
      /// lat lon name line edits
      std::vector<QLineEdit*> latLonNameLineEdits;
      
      /// surface vector selection page
      QWidget* pageSurfaceVectorSelection;
      
      /// surface vector settings page
      QWidget* pageSurfaceVectorSettings;
      
      /// surface vector radio buttons
      std::vector<QRadioButton*> surfaceVectorRadioButtons;
      
      /// surface vector line edits
      std::vector<QLineEdit*> surfaceVectorLineEdits;
      
      /// surface vector comment view/edit pushbuttons
      std::vector<QPushButton*> surfaceVectorCommentPushButtons;
      
      /// surface vector comment pushbuttons group
      QButtonGroup* surfaceVectorCommentButtonGroup;
      
      /// surface vector columns grid
      QGridLayout* surfaceVectorColumnsGridLayout;
      
      /// surface vector column button group
      QButtonGroup* surfaceVectorButtonGroup;
      
      /// surface vector display mode combo box
      QComboBox* surfaceVectorDisplayModeComboBox;
      
      /// surface vector sparse distance spin box
      QSpinBox* surfaceVectorSparseDistanceSpinBox;
      
      /// surface vector length multiplier float spin box
      QDoubleSpinBox* surfaceVectorLengthMultiplierDoubleSpinBox;
      
      /// geodesic main page
      QWidget* pageGeodesicMain;
      
      /// geodesic main page layout
      QVBoxLayout* geodesicMainPageLayout;
      
      /// enable geodesic path check box
      QCheckBox* geodesicDistanceEnableCheckBox;
      
      /// geodesic comment button group
      QButtonGroup* geodesicCommentButtonGroup;
      
      /// geodesic grid layout
      QGridLayout* geodesicSelectionGridLayout;
      
      /// number of valid geodesic columns
      int numValidGeodesic;
      
      /// geodesic selection radio button group
      QButtonGroup* geodesicSelectionButtonGroup;
      
      /// geodesic comment push buttons
      std::vector<QPushButton*> geodesicColumnCommentPushButtons;
      
      /// geodesic selection radio buttons
      std::vector<QRadioButton*> geodesicSelectionRadioButtons;
      
      /// geodesic name line edits
      std::vector<QLineEdit*> geodesicNameLineEdits;
      
      /// geodesic path line width spin box
      QSpinBox* geodesicPathLineWidthSpinBox;
      
      /// geodesic show root node symbol check box
      QCheckBox* geodesicShowRootNodeCheckBox;
      
      /// areal estimation main page
      QWidget* pageArealEstimation;
      
      /// layout for areal estimation main page
      QVBoxLayout* arealEstimationMainPageLayout;
      
      /// areal estimation comment button group
      QButtonGroup* arealEstimationCommentButtonGroup;
      
      /// areal estimation metadata button group
      QButtonGroup* arealEstimationMetaDataButtonGroup;
      
      /// areal estimation selection button group
      QButtonGroup* arealEstimationSelectionButtonGroup;
      
      /// areal estimation grid layout
      QGridLayout* arealEstimationSelectionGridLayout;
      
      /// number of valid areal estimation columns
      int numValidArealEstimation;
      
      /// areal estimation comment push buttons
      std::vector<QPushButton*> arealEstimationColumnCommentPushButtons;
      
      /// areal estimation metadata push buttons
      std::vector<QPushButton*> arealEstimationColumnMetaDataPushButtons;
      
      /// areal estimation selection radio button
      std::vector<QRadioButton*> arealEstimationSelectionRadioButtons;
      
      /// areal estimation name line edits
      std::vector<QLineEdit*> arealEstimationNameLineEdits;
      
      /// surface data valid
      bool validSurfaceData;
      
      /// volume data valid
      bool validVolumeData;
      
      /// areal estimation data valid
      bool validArealEstimationData;
      
      /// border data valid
      bool validBorderData;
      
      /// cell data valid
      bool validCellData;
      
      /// Cocomac data valid
      bool validCocomacData;
      
      /// contour data valid
      bool validContourData;
      
      /// deformation field data valid
      bool validDeformationFieldData;
      
      /// foci data valid
      bool validFociData;
      
      /// geodesic data valid
      bool validGeodesicData;
      
      /// image data valid
      bool validImageData;
      
      /// latlon data valid
      bool validLatLonData;
      
      /// metric data valid
      bool validMetricData;
      
      /// model data valid
      bool validModelData;
      
      /// paint data valid
      bool validPaintData;
      
      /// prob atlas surface data valid
      bool validProbAtlasSurfaceData;
      
      /// prob atlas volume data valid
      bool validProbAtlasVolumeData;
      
      /// region data valid
      bool validRegionData;
      
      /// rgb paint data valid
      bool validRgbPaintData;
      
      /// scene data valid
      bool validSceneData;
      
      /// shape data valid
      bool validShapeData;
      
      /// surface and volume data valid
      bool validSurfaceAndVolumeData;
      
      /// surface vector data valid
      bool validSurfaceVectorData;
      
      /// topography data valid
      bool validTopographyData;
};

#endif // __GUI_DISPLAY_CONTROL_DIALOG_H__


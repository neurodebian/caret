
#ifndef __GUI_BORDER_OPERATIONS_DIALOG_H__
#define __GUI_BORDER_OPERATIONS_DIALOG_H__

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

#include "BrainModelBorderSet.h"
#include "WuQMultiPageDialog.h"

class BordersPageCreateFlatAnalysisGridBorders;
class BordersPageCreateFlatGridBorders;
class BordersPageCreateInterpolatedBorders;
class BordersPageCreateLandmarks;
class BordersPageDrawNew;
class BordersPageEditAttributes;
class BordersPageUpdateExisting;
class GuiBrainModelSelectionComboBox;
class GuiBrainModelSurfaceSelectionComboBox;
class GuiColorFileEditorWidget;
class GuiNodeAttributeColumnSelectionComboBox;
class GuiStereotaxicSpaceComboBox;
class GuiVolumeFileSelectionComboBox;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class VolumeFile;

/// dialog for border operations
class GuiBorderOperationsDialog : public WuQMultiPageDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiBorderOperationsDialog(QWidget* parent);
      
      // destructor
      ~GuiBorderOperationsDialog();

      // get border drawing parameters
      void getBorderDrawingParameters(QString& borderNameOut,
                                      float& samplingOut,
                                      int& borderColorIndexOut,
                                      int& surfacePaintAssignmentColumnNumberOut,
                                      int& surfacePaintNameAssignmentIndexOut,
                                      VolumeFile* &paintVolumeForVoxelAssignmentOut,
                                      int& paintVolumeSliceThicknessOut,
                                      bool& closedBorderFlagOut,
                                      bool& twoDimFlagOut,
                                      bool& autoProjectYesFlagOut) const;
                                      
      // get border update parameters
      void getBorderUpdateParameters(BrainModelBorderSet::UPDATE_BORDER_MODE& borderUpdateModeOut,
                                      float& samplingOut,
                                      bool& twoDimFlagOut,
                                      bool& autoProjectYesFlagOut) const;
            
      // set selected interpolated borders
      void setSelectedInterpolatedBorders(const int borderNumber,
                                          const int borderIndex);
                              
      // show the draw borders page
      void showPageDrawBorders();
      
      // show the update borders page
      void showPageUpdateBorders();
      
      // show the create landmark borders page
      void showPageCreateLandmarkBorders(const bool checkCreateFlattenLandmarks,
                                         const bool checkCreateRegistrationLandmarks,
                                         const BrainModelSurface* fiducialSurface = NULL,
                                         const BrainModelSurface* inflatedSurface = NULL,
                                         const BrainModelSurface* veryInflatedSurface = NULL,
                                         const BrainModelSurface* sphericalSurface = NULL);
      
   public slots:
      /// called when close push button clicked
      bool close();
      
   protected slots:
      // called to convert volume to fiducial borders
      void slotConvertVolumeToFiducialBorders();
      
      // called to create lat/lon spherical borders
      void slotLatLonSphericalBorders();
      
      // called to move border points with mouse
      void slotMoveBorderPointWithMouse();
      
      // called to rename borders with mouse
      void slotRenameBordersWithMouse();
      
      // called to reverse borders with mouse
      void slotReverseBordersWithMouse();
      
      // called to orient borders clockwise
      void slotOrientBordersClockwise();
      
      // called to reverse displayed borders
      void slotReverseDisplayedBorders();
      
      // called to delete all borders
      void slotDeleteAllBorders();
      
      // called to delete borders not displayed
      void slotDeleteBordersNotDisplayed();
      
      // called to delete borders with mouse
      void slotDeleteBordersWithMouse();
      
      // called to delete border points with mouse
      void slotDeleteBorderPointsWithMouse();
      
      // called to delete border points outside surface
      void slotDeleteBorderPointsOutsideSurface();
      
      // called to resample displayed borders
      void slotResampleDisplayedBorders();
      
   protected:
      /// page for drawing borders
      BordersPageDrawNew* pageBordersDrawNew;
      
      /// page for updating borders
      BordersPageUpdateExisting* pageBordersDrawUpdate;
      
      /// page for creating landmark borders
      BordersPageCreateLandmarks* pageCreateLandmarkBorders;
      
      /// page for interpolated borders
      BordersPageCreateInterpolatedBorders* pageInterpolatedBorders;
};

//=============================================================================

/// page for draw new borders
class BordersPageDrawNew : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageDrawNew();
                         
      // destructor
      virtual ~BordersPageDrawNew();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
   
      // get border drawing parameters
      void getBorderDrawingParameters(QString& borderNameOut,
                                      float& samplingOut,
                                      int& borderColorIndexOut,
                                      int& surfacePaintAssignmentColumnNumberOut,
                                      int& surfacePaintNameAssignmentIndexOut,
                                      VolumeFile* &paintVolumeForVoxelAssignmentOut,
                                      int& paintVolumeSliceThicknessOut,
                                      bool& closedBorderFlagOut,
                                      bool& twoDimFlagOut,
                                      bool& autoProjectYesFlagOut) const;
                                      
   protected slots:
      // called when name push button clicked
      void slotNamePushButton();
      
   private slots:
      // called when type changed
      void slotTypeChanged();
      
   protected:
      // create the closed border assignment widget
      QWidget* createClosedBorderNodeVoxelAssignmentWidget();
      
      /// name line edit
      QLineEdit* nameLineEdit;
      
      /// sampling double spin box
      QDoubleSpinBox* samplingDoubleSpinBox;
      
      /// type closed radio button
      QRadioButton* typeClosedRadioButton;
      
      /// type open radio button
      QRadioButton* typeOpenRadioButton;
      
      /// dimension 2D radio button
      QRadioButton* dimension2DRadioButton;
      
      /// dimension 2D Auto Project radio button
      QRadioButton* dimension2DAutoProjectRadioButton;
      
      /// dimension 3D radio button
      QRadioButton* dimension3DRadioButton;
      
      /// index of border color
      int borderColorIndex;
      
      /// assign nodes within closed border check box
      QGroupBox* assignNodesInsideBorderGroupBox;

      /// node voxel assignment widget
      QWidget* nodeVoxelAssignmentWidget;
      
      /// paint column selection
      GuiNodeAttributeColumnSelectionComboBox* paintColumnComboBox;
      
      /// name of column line edit
      QLineEdit* paintColumnNameLineEdit;
      
      /// paint column number for assignment
      int paintAssignmentColumnNumber;
      
      /// index of paint that is assigned
      int paintNameAssignmentIndex;
      
      /// paint volume selection control
      GuiVolumeFileSelectionComboBox* paintVolumeSelectionControl;
      
      /// paint volume slice thickness control
      QComboBox* paintVolumeSliceThicknessComboBox;
      
      /// paint volume assignment group box
      QGroupBox* assignVoxelsInsideBorderGroupBox;
      
      /// paint volume for voxel assignment
      VolumeFile* paintVolumeForVoxelAssignment;
};

//=============================================================================

/// page for updating borders
class BordersPageUpdateExisting : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageUpdateExisting();
                         
      // destructor
      virtual ~BordersPageUpdateExisting();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
      
      // get border update parameters
      void getBorderUpdateParameters(BrainModelBorderSet::UPDATE_BORDER_MODE& borderUpdateModeOut,
                                      float& samplingOut,
                                      bool& twoDimFlagOut,
                                      bool& autoProjectYesFlagOut) const;
                                      
   protected:
      /// mode extend radio button
      QRadioButton* extendBorderModeRadioButton;
      
      /// mode replace segment radio button
      QRadioButton* replaceSegmentInBorderModeRadioButton;
      
      /// mode erase radio button
      QRadioButton* eraseBorderModeRadioButton;
      
      /// sampling double spin box
      QDoubleSpinBox* samplingDoubleSpinBox;
      
      /// dimension 2D radio button
      QRadioButton* dimension2DRadioButton;
      
      /// dimension 2D Auto Project radio button
      QRadioButton* dimension2DAutoProjectRadioButton;
      
      /// dimension 3D radio button
      QRadioButton* dimension3DRadioButton;
};

//=============================================================================

/// page for updating borders
class BordersPageProjection : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageProjection();
                         
      // destructor
      virtual ~BordersPageProjection();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
      
   protected:
      /// project to nearest node radio button
      QRadioButton* nearestNodeRadioButton;
      
      /// project to nearest tile radio button
      QRadioButton* nearestTileRadioButton;
};

//=============================================================================

/// page for editing border colors
class BordersPageEditColors : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageEditColors();
                         
      // destructor
      virtual ~BordersPageEditColors();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
      
   protected slots:
      /// called if colors changed
      void slotColorsChanged();
      
   protected:
      /// the color editor
      GuiColorFileEditorWidget* colorEditorWidget;
};

//=============================================================================

/// page for create borders from paint regions
class BordersPageCreateFromPaintRegions : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageCreateFromPaintRegions();
                         
      // destructor
      virtual ~BordersPageCreateFromPaintRegions();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
      
   protected slots:
      /// called when paint column selection made
      void slotPaintColumnComboBox(int);
      
   protected:
      /// paint column selection
      GuiNodeAttributeColumnSelectionComboBox* paintColumnComboBox;
      
      /// name for borders
      QLineEdit* borderNameLineEdit;
};

//=============================================================================

/// page for create flat grid borders
class BordersPageCreateFlatGridBorders : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageCreateFlatGridBorders();
                         
      // destructor
      virtual ~BordersPageCreateFlatGridBorders();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
            
   protected:
      /// grid spacing double spin box
      QDoubleSpinBox* gridSpacingDoubleSpinBox;
      
      /// points per grid square double spin box
      QSpinBox* pointsPerGridSquareSpinBox;
      
      /// limit the points to the surface
      QCheckBox* restrictPointsToSurfaceCheckBox;
};

//=============================================================================

/// page for create flat analysis grid borders
class BordersPageCreateFlatAnalysisGridBorders : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageCreateFlatAnalysisGridBorders();
                         
      // destructor
      virtual ~BordersPageCreateFlatAnalysisGridBorders();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
            
   protected slots:
      // called when update grid extent push button pressed
      void slotUpdateGridExtentPushButton();
      
   protected:
      /// x min double spin box
      QDoubleSpinBox* xMinDoubleSpinBox;
      
      /// x max double spin box
      QDoubleSpinBox* xMaxDoubleSpinBox;
      
      /// y min double spin box
      QDoubleSpinBox* yMinDoubleSpinBox;
      
      /// y max double spin box
      QDoubleSpinBox* yMaxDoubleSpinBox;
      
      /// spacing double spin box
      QDoubleSpinBox* spacingDoubleSpinBox;
      
      /// limit the points to the surface
      QCheckBox* restrictPointsToSurfaceCheckBox;
};

//=============================================================================

/// page for create interpolated borders
class BordersPageCreateInterpolatedBorders : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageCreateInterpolatedBorders();
                         
      // destructor
      virtual ~BordersPageCreateInterpolatedBorders();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
            
      // set selected borders
      void setSelectedBorders(const int borderNumber,
                              const int borderIndex);
                              
   protected slots:
      /// called to enable mouse mode
      void slotEnableMousePushButton();
      
   protected:
      /// index of first border
      int border1Index;
      
      /// index of second border
      int border2Index;
      
      /// border 1 name label
      QLabel* border1NameLabel;
      
      /// border 2 name label
      QLabel* border2NameLabel;
      
      /// name prefix line edit
      QLineEdit* namePrefixLineEdit;
      
      /// sampling double spin box
      QDoubleSpinBox* samplingDoubleSpinBox;
      
      /// number of interpolated borders spin box
      QSpinBox* numberOfInterplatedBordersSpinBox;
      
};

//=============================================================================

/// page for create edit attributes
class BordersPageEditAttributes : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageEditAttributes();
                         
      // destructor
      virtual ~BordersPageEditAttributes();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
            
   protected:
};

//=============================================================================

/// page for create landmark borders
class BordersPageCreateLandmarks : public WuQMultiPageDialogPage {
   Q_OBJECT
   
   public:
      // constructor
      BordersPageCreateLandmarks();
                         
      // destructor
      virtual ~BordersPageCreateLandmarks();
      
      // apply the pages settings
      virtual void applyPage();
      
      // create the page
      virtual QWidget* createPage();
                              
      // update the page
      virtual void updatePage();

      // see if the page is valid
      virtual bool getPageValid() const;
            
      // set initialize settings
      void initializePage(const bool checkCreateFlattenLandmarks,
                          const bool checkCreateRegistrationLandmarks,
                          const BrainModelSurface* fiducialSurface = NULL,
                          const BrainModelSurface* inflatedSurface = NULL,
                          const BrainModelSurface* veryInflatedSurface = NULL,
                          const BrainModelSurface* sphericalSurface = NULL);
                                  
   protected:
      /// stereotaxic space combo box
      GuiStereotaxicSpaceComboBox* stereotaxicSpaceComboBox;
      
      /// anatomical volume selection combo box
      GuiVolumeFileSelectionComboBox* anatomicalVolumeComboBox;
      
      /// fiducial surface selection combo box
      GuiBrainModelSurfaceSelectionComboBox* fiducialSurfaceComboBox;
      
      /// inflated surface selection combo box
      GuiBrainModelSurfaceSelectionComboBox* inflatedSurfaceComboBox;
      
      /// very inflated surface selection combo box
      GuiBrainModelSurfaceSelectionComboBox* veryInflatedSurfaceComboBox;
      
      /// ellipsoid surface selection combo box
      GuiBrainModelSurfaceSelectionComboBox* ellipsoidSurfaceComboBox;
      
      /// surface shape depth column combo box
      GuiNodeAttributeColumnSelectionComboBox* shapeDepthColumnComboBox;
      
      /// paint geography column combo box
      GuiNodeAttributeColumnSelectionComboBox* paintGeographyColumnComboBox;
      
      /// create flatten landmarks check box
      QCheckBox* createFlattenLandmarksCheckBox;
      
      /// create registration landmarks check box
      QCheckBox* createRegistrationLandmarksCheckBox;
};

#endif // __GUI_BORDER_OPERATIONS_DIALOG_H__


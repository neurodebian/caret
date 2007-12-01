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


#ifndef __GUI_MAIN_WINDOW_LAYERS_ACTIONS_H__
#define __GUI_MAIN_WINDOW_LAYERS_ACTIONS_H__

#include <QObject>

class GuiMainWindow;
class QAction;

/// This class constructs the layers actions
class GuiMainWindowLayersActions : public QObject {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMainWindowLayersActions(GuiMainWindow* parent);
      
      /// Destructor
      ~GuiMainWindowLayersActions();
      
      /// action for drawing borders
      QAction* getBordersDrawAction() { return bordersDrawAction; }
      
      /// action for converting volume borders to fiducial cells
      QAction* getBordersVolumeToFiducialCellsAction() { return bordersVolumeToFiducialCellsAction; }
       
      /// action for converting volume borders to fiducial borders
      QAction* getBordersVolumeToBordersFiducialAction() { return bordersVolumeToBordersFiducialAction; }
      
      /// action for creating average borders
      QAction* getBordersAverageAction() { return bordersAverageAction; }
      
      /// action for comparing borders
      QAction* getBordersCompareAction() { return bordersCompareAction; }
      
      /// action for creating borders from paint column
      QAction* getBorderCreatedFromPaintAction() { return bordersFromPaintAction; }
      
      /// action for creating grid borders
      QAction* getBordersCreateGridAction() { return bordersCreateGridAction; }
      
      /// action for creating analysis grid borders
      QAction* getBordersCreateAnalysisGridAction() { return bordersCreateAnalysisGridAction; }
      
      /// action for creating spherical borders
      QAction* getBordersCreateSphericalAction() { return bordersCreateSphericalAction; }
      
      /// action for converting borders to a VTK model
      QAction* getBordersConvertToVtkModelAction() { return bordersConvertToVtkModelAction; }
      
      /// action for editing border colors
      QAction* getBordersEditColorsAction() { return bordersEditColorsAction; }
      
      /// action for resampling displayed borders
      QAction* getBordersResampleDisplayedAction() { return bordersResampleDisplayedAction; }
      
      /// action for renaming borders selected with mouse
      QAction* getBordersRenameWithMouseAction() { return bordersRenameWithMouseAction; }
      
      /// action for reversing borders selected with mouse
      QAction* getBordersReverseWithMouseAction() { return bordersReverseWithMouseAction; }
      
      /// action for reversing displayed borders
      QAction* getBordersReverseDisplayedAction() { return bordersReverseDisplayedAction; }
      
      /// action for orient displayed borders clockwise
      QAction* getBordersOrientDisplayedClockwiseAction() { return bordersOrientDisplayedClockwiseAction; }
      
      /// action for projecting borders
      QAction* getBordersProjectAction() { return bordersProjectAction; }
      
      /// action for delete border point with mouse
      QAction* getBordersDeletePointWithMouseAction() { return bordersDeletePointWithMouseAction; }
      
      /// action for delete border with mouse
      QAction* getBordersDeleteWithMouseAction() { return bordersDeleteWithMouseAction; }
      
      /// action for deleting borders by name
      QAction* getDeleteBordersByNameAction() { return bordersDeleteByNameAction; }
      
      /// action for delete all borders
      QAction* getBordersDeleteAllAction() { return bordersDeleteAllAction; }
   
      /// action for move border point with mouse
      QAction* getBordersMovePointWithMouseAction() { return bordersMovePointWithMouseAction; }
      
      /// action for delete border points outside surface
      QAction* getBordersDeletePointsOutsideSurfaceAction() { return bordersDeletePointsOutsideSurfaceAction; }
      
      /// action for edit border attributes
      QAction* getBordersEditAttributesAction() { return bordersEditAttributesAction; }
      
      /// action for clear border highlights
      QAction* getBordersClearHighlightingAction() { return bordersClearHighlightingAction; }
      
      /// action for borders interpolate
      QAction* getBordersCreateInterpolatedAction() { return bordersCreateInterpolatedAction; }
      
      /// action for foci map stereotaxic focus
      QAction* getFociMapStereotaxicFocusAction() { return fociMapStereotaxicFocusAction; }
      
      /// action for converting foci to VTK model
      QAction* getFociConvertToVtkModelAction() { return fociConvertToVtkModelAction; }
      
      /// action for foci edit colors
      QAction* getFociEditColorsAction() { return fociEditColorsAction; }
      
      /// action for foci project
      QAction* getFociProjectAction() { return fociProjectAction; }
      
      /// action for foci PALS project
      QAction* getFociPalsProjectAction() { return fociPalsProjectAction; }
      
      /// action for foci delete all
      QAction* getFociDeleteAllAction() { return fociDeleteAllAction; }
      
      /// action for foci delete non-displayed
      QAction* getFociDeleteNonDisplayedAction() { return fociDeleteNonDisplayedAction; }
      
      /// action for foci non-matching color removal
      QAction* getFociDeleteNonMatchingColorsAction() { return fociDeleteNonMatchingColors; }

      /// action for foci delete using mouse
      QAction* getFociDeleteUsingMouseAction() { return fociDeleteUsingMouseAction; }
      
      /// action for foci assign class to displayed foci
      QAction* getFociAssignClassToDisplayedFoci() { return fociAssignClassToDisplayedFoci; }

      /// action for uncertainty limits to RGB paint dialog
      QAction* getFociUncertaintyLimitsAction() { return fociUncertaintyLimitsAction; }
      
      /// action for foci report
      QAction* getFociReportAction() { return fociReportAction; }
      
      /// action for foci attribute assignment
      QAction* getFociAttributeAssignmentAction() { return fociAttributeAssignmentAction; }
      
      /// action for foci clear highlighting
      QAction* getFociClearHighlightingAction() { return fociClearHighlightingAction; }
      
      /// foci convert foci file study info to study meta data file
      QAction* getFociStudyInfoToStudyMetaDataFileAction() { return fociStudyInfoToStudyMetaDataFileAction; }

      /// action for updating foci PubMed ID if focus name same as study name
      QAction* getFociUpdatePubMedIDIfFocusNameMatchesStudyNameAction() { return fociUpdatePubMedIDIfFocusNameMatchesStudyNameAction; }
      
      /// action for converting cells to vtk model
      QAction* getCellsConvertToVtkModelAction() { return cellsConvertToVtkModelAction; }
      
      /// action for add cells
      QAction* getCellsAddAction() { return cellsAddAction; }
      
      /// action for cells
      QAction* getCellsEditColorsAction() { return cellsEditColorsAction; }
      
      /// action for cells
      QAction* getCellsProjectAction() { return cellsProjectAction; }
      
      /// action for cells
      QAction* getCellsDeleteAllAction() { return cellsDeleteAllAction; }
      
      /// action for cells
      QAction* getCellsEditAttributesAction() { return cellsEditAttributesAction; }
      
      /// action for cells
      QAction* getCellsDeleteUsingMouseAction() { return cellsDeleteUsingMouseAction; }
      
      /// action for converting cell density to metric
      QAction* getCellsDensityToMetricAction() { return cellsDensityToMetricAction; }
      
      /// action for cell report
      QAction* getCellReportAction() { return cellReportAction; }
      
      /// action for contour new set
      QAction* getContourNewSetAction() { return contourNewSetAction; }
      
      /// action for applying current view
      QAction* getContourApplyCurrentViewAction() { return contourApplyCurrentViewAction; }
      
      /// action for contour set scale
      QAction* getContourSetScaleAction() { return contourSetScaleAction; }
      
      /// action for contour select sections
      QAction* getContourSectionsAction() { return contourSectionsAction; }
      
      /// action for contour set spacing
      QAction* getContourSpacingAction() { return contourSpacingAction; }
      
      /// action for contour draw
      QAction* getContourDrawAction() { return contourDrawAction; }
      
      /// action for contour align
      QAction* getContourAlignAction() { return contourAlignAction; }
      
      /// action for contour merge
      QAction* getContourMergeAction() { return contourMergeAction; }
      
      /// action for contour move point
      QAction* getContourMovePointAction() { return contourMovePointAction; }
      
      /// action for contour delete all
      QAction* getContourDeleteAllAction() { return contourDeleteAllAction; }
      
      /// action for contour  delete point
      QAction* getContourDeletePointAction() { return contourDeletePointAction; }
      
      /// action for contour delete contour
      QAction* getContourDeleteAction() { return contourDeleteAction; }
      
      /// action for contour reverse contour
      QAction* getContourReverseAction() { return contourReverseAction; }
      
      /// action for contour cleanup
      QAction* getContourCleanUpAction() { return contourCleanUpAction; }
      
      /// action for contour reconstruct into surface
      QAction* getContourReconstructAction() { return contourReconstructAction; }
      
      /// action for contour resample 
      QAction* getContourResampleAction() { return contourResampleAction; }
      
      /// action for to add contour cells
      QAction* getContourCellsAddAction() { return contourCellsAddAction; }
      
      /// action for to delete contour cells with mouse
      QAction* getContourCellsDeleteWithMouseAction() { return contourCellsDeleteWithMouseAction; }
      
      /// action for to delete all contour cells
      QAction* getContourCellsDeleteAllAction() { return contourCellsDeleteAllAction; }
      
      /// action for to edit contour cell colors
      QAction* getContourCellsEditColorsAction() { return contourCellsEditColorsAction; }
      
      /// action for when contour move cell selected
      QAction* getContourCellsMoveWithMouseAction() { return contourCellsMoveWithMouseAction; }
      
      /// action for when contour information action selected
      QAction* getContourInformationAction() { return contourInformationAction; }
      
   public slots:
      /// update the actions (typically called when menu is about to show)
      void updateActions();

      /// slot for converting volume borders to fiducial cells
      void slotBordersVolumeToFiducialCells();
       
      /// slot for converting volume borders to fiducial borders
      void slotBordersVolumeToBordersFiducial();
      
      /// slot for creating average borders
      void slotBordersAverage();
      
      /// slot for comparing borders
      void slotBordersCompare();
      
      /// slot for creating grid borders
      void slotBordersCreateGrid();
      
      /// slot for creating analysis grid borders
      void slotBordersCreateAnalysisGrid();
      
      /// slot for creating spherical borders
      void slotBordersCreateSpherical();
      
      /// slot for converting borders to a VTK model
      void slotBordersConvertToVtkModel();
      
      /// slot for editing border colors
      void slotBordersEditColors();
      
      /// slot for resampling displayed borders
      void slotBordersResampleDisplayed();
      
      /// slot for renaming borders selected with mouse
      void slotBordersRenameWithMouse();
      
      /// slot for reversing borders selected with mouse
      void slotBordersReverseWithMouse();
      
      /// slot for reversing displayed borders
      void slotBordersReverseDisplayed();
      
      /// slot for orient displayed borders clockwise
      void slotBordersOrientDisplayedClockwise();
      
      /// slot for projecting borders
      void slotBordersProject();
      
      /// slot for delete border point with mouse
      void slotBordersDeletePointWithMouse();
      
      /// slot for delete border with mouse
      void slotBordersDeleteWithMouse();
      
      /// slot for deleting borders by name
      void slotDeleteBordersByName();
      
      /// slot for delete all borders
      void slotBordersDeleteAll();
   
      /// slot for borders from paint
      void slotBordersFromPaintAction();
      
      /// slot for move border point with mouse
      void slotBordersMovePointWithMouse();
      
      /// slot for delete border points outside surface
      void slotBordersDeletePointsOutsideSurface();
      
      /// slot for edit border attributes
      void slotBordersEditAttributes();
      
      /// slot for clear border highlighting
      void slotBordersClearHighlighting();
      
      /// slot for creating interpolated borders
      void slotBordersCreateInterpolated();
      
      /// slot for foci map stereotaxic focus
      void slotFociMapStereotaxicFocus();
      
      /// slot for converting foci to VTK model
      void slotFociConvertToVtkModel();
      
      /// slot for foci edit colors
      void slotFociEditColors();
      
      /// slot for foci project
      void slotFociProject();
      
      /// slot for foci PALS project
      void slotFociPalsProject();
      
      /// slot for foci delete all
      void slotFociDeleteAll();
      
      /// slot for foci assign class name
      void slotFociAssignClassToDisplayedFoci();
      
      /// slot for foci delete non-displayed
      void slotFociDeleteNonDisplayed();
      
      /// slot for foci non-matching color deletion
      void slotFociDeleteNonMatchingColors();
      
      /// slot for foci delete using mouse
      void slotFociDeleteUsingMouse();
      
      /// slot for uncertainty limits to RGB paint dialog
      void slotFociUncertaintyLimits();
      
      /// slot for foci report
      void slotFociReport();
      
      /// slot for foci clear highlighting action
      void slotFociClearHighlighting();
      
      /// slot for converting foci study info to study metadata
      void slotFociStudyInfoToStudyMetaDataFile();
      
      /// slot for updating foci PubMed ID if focus name same as study name
      void slotFociUpdatePubMedIDIfFocusNameMatchesStudyName();
      
      /// slot for converting cells to vtk model
      void slotCellsConvertToVtkModel();
      
      /// slot for add cells
      void slotCellsAdd();
      
      /// slot for cells
      void slotCellsEditColors();
      
      /// slot for cells
      void slotCellsProject();
      
      /// slot for cells
      void slotCellsDeleteAll();
      
      /// slot for cells
      void slotCellsEditAttributes();
      
      /// slot for cells
      void slotCellsDeleteUsingMouse();
      
      /// slot for converting cell density to metric
      void slotCellsDensityToMetric();
      
      /// slot for cell report
      void slotCellReport();
      
      /// slot for contour new set
      void slotContourNewSet();
      
      /// slot for contour clean up
      void slotContourCleanUp();
      
      /// slot for applying current view
      void slotContourApplyCurrentView();
      
      /// slot for contour set scale
      void slotContourSetScale();
      
      /// slot for contour select sections
      void slotContourSections();
      
      /// slot for contour set spacing
      void slotContourSpacing();
      
      /// slot for contour draw
      void slotContourDraw();
      
      /// slot for contour align
      void slotContourAlign();
      
      /// slot for contour merge
      void slotContourMerge();
      
      /// slot for contour move point
      void slotContourMovePoint();
      
      /// slot for contour delete all
      void slotContourDeleteAll();
      
      /// slot for contour  delete point
      void slotContourDeletePoint();
      
      /// slot for contour delete contour
      void slotContourDelete();
      
      /// slot for contour point reversal
      void slotContourReverse();
      
      /// slot for contour resampling
      void slotContourResample();
      
      /// slot for contour reconstruct into surface
      void slotContourReconstruct();
      
      /// slot called to add contour cells
      void slotContourCellsAdd();
      
      /// slot called to delete contour cells with mouse
      void slotContourCellsDeleteWithMouse();
      
      /// slot called to delete all contour cells
      void slotContourCellsDeleteAll();
      
      /// slot called to edit contour cell colors
      void slotContourCellsEditColors();
      
      /// slot called when contour move cell selected
      void slotContourCellsMoveWithMouse();
      
      /// slot called when contour information selected
      void slotContourInformation();
      
   protected slots:      
      /// slot called when contour cell colors are changed
      void slotContourCellsColorsChanged();
      
      /// slot used by border color editor
      void slotBorderColorsChanged();
        
      /// slot called by foci color editor when colors are changed
      void slotFociColorsChanged();
      
      /// slot for cells
      void slotCellsColorsChanged();
      
   protected:
      /// action for drawing borders
      QAction* bordersDrawAction;
      
      /// slot for converting volume borders to fiducial cells
      QAction* bordersVolumeToFiducialCellsAction;
       
      /// slot for converting volume borders to fiducial borders
      QAction* bordersVolumeToBordersFiducialAction;
      
      /// slot for creating average borders
      QAction* bordersAverageAction;
      
      /// slot for comparing borders
      QAction* bordersCompareAction;
      
      /// slot for creating grid borders
      QAction* bordersCreateGridAction;
      
      /// slot for creating analysis grid borders
      QAction* bordersCreateAnalysisGridAction;
      
      /// slot for creating spherical borders
      QAction* bordersCreateSphericalAction;
      
      /// slot for converting borders to a VTK model
      QAction* bordersConvertToVtkModelAction;
      
      /// slot for borders from paint 
      QAction* bordersFromPaintAction;
      
      /// slot for editing border colors
      QAction* bordersEditColorsAction;
      
      /// slot for resampling displayed borders
      QAction* bordersResampleDisplayedAction;
      
      /// slot for renaming borders selected with mouse
      QAction* bordersRenameWithMouseAction;
      
      /// slot for reversing borders selected with mouse
      QAction* bordersReverseWithMouseAction;
      
      /// slot for reversing displayed borders
      QAction* bordersReverseDisplayedAction;
      
      /// slot for orient displayed borders clockwise
      QAction* bordersOrientDisplayedClockwiseAction;
      
      /// slot for projecting borders
      QAction* bordersProjectAction;
      
      /// slot for delete border point with mouse
      QAction* bordersDeletePointWithMouseAction;
      
      /// slot for delete border with mouse
      QAction* bordersDeleteWithMouseAction;
      
      /// slot for deleting borders by name
      QAction* bordersDeleteByNameAction;
      
      /// slot for delete all borders
      QAction* bordersDeleteAllAction;
   
      /// slot for move border point with mouse
      QAction* bordersMovePointWithMouseAction;
      
      /// slot for delete border points outside surface
      QAction* bordersDeletePointsOutsideSurfaceAction;
      
      /// slot for edit border attributes
      QAction* bordersEditAttributesAction;
      
      /// slot for clear border highlighting
      QAction* bordersClearHighlightingAction;
      
      /// slot for borders interpolate
      QAction* bordersCreateInterpolatedAction;
      
      /// slot for foci map stereotaxic focus
      QAction* fociMapStereotaxicFocusAction;
      
      /// slot for converting foci to VTK model
      QAction* fociConvertToVtkModelAction;
      
      /// slot for foci edit colors
      QAction* fociEditColorsAction;
      
      /// slot for foci project
      QAction* fociProjectAction;
      
      /// slot for foci PALS projection
      QAction* fociPalsProjectAction;
      
      /// slot for foci delete all
      QAction* fociDeleteAllAction;
      
      /// for foci delete non displayed
      QAction* fociDeleteNonDisplayedAction;
      
      /// for foci assign class to displayed foci
      QAction* fociAssignClassToDisplayedFoci;
      
      /// for foci delete colors non-matching foci
      QAction* fociDeleteNonMatchingColors;
      
      /// slot for foci delete using mouse
      QAction* fociDeleteUsingMouseAction;
      
      /// slot for uncertainty limits to RGB paint dialog
      QAction* fociUncertaintyLimitsAction;
      
      /// slot for foci report
      QAction* fociReportAction;
      
      /// foci attribute assignment action
      QAction* fociAttributeAssignmentAction;
      
      /// foci clear highlighting action
      QAction* fociClearHighlightingAction;
      
      /// foci convert foci file study info to study meta data file
      QAction* fociStudyInfoToStudyMetaDataFileAction;
      
      /// action for updating foci PubMed ID if focus name same as study name
      QAction* fociUpdatePubMedIDIfFocusNameMatchesStudyNameAction;
      
      /// slot for converting cells to vtk model
      QAction* cellsConvertToVtkModelAction;
      
      /// slot for add cells
      QAction* cellsAddAction;
      
      /// slot for cells
      QAction* cellsEditColorsAction;
      
      /// slot for cells
      QAction* cellsProjectAction;
      
      /// slot for cells
      QAction* cellsDeleteAllAction;
      
      /// slot for cells
      QAction* cellsEditAttributesAction;
      
      /// slot for cells
      QAction* cellsDeleteUsingMouseAction;
      
      /// slot for converting cell density to metric
      QAction* cellsDensityToMetricAction;
      
      /// slot for cell report
      QAction* cellReportAction;
      
      /// slot for contour new set
      QAction* contourNewSetAction;
      
      /// slot for applying current view
      QAction* contourApplyCurrentViewAction;
      
      /// slot for contour set scale
      QAction* contourSetScaleAction;
      
      /// slot for contour select sections
      QAction* contourSectionsAction;
      
      /// slot for contour set spacing
      QAction* contourSpacingAction;
      
      /// slot for contour cleanup
      QAction* contourCleanUpAction;
      
      /// slot for contour draw
      QAction* contourDrawAction;
      
      /// slot for contour align
      QAction* contourAlignAction;
      
      /// slot for contour merge
      QAction* contourMergeAction;
      
      /// slot for contour move point
      QAction* contourMovePointAction;
      
      /// slot for contour delete all
      QAction* contourDeleteAllAction;
      
      /// slot for contour  delete point
      QAction* contourDeletePointAction;
      
      /// slot for contour delete contour
      QAction* contourDeleteAction;
      
      /// slot for contour reverse contour
      QAction* contourReverseAction;
      
      /// slot for contour reconstruct into surface
      QAction* contourReconstructAction;
      
      /// contour resample action
      QAction* contourResampleAction;
      
      /// slot called to add contour cells
      QAction* contourCellsAddAction;
      
      /// slot called to delete contour cells with mouse
      QAction* contourCellsDeleteWithMouseAction;
      
      /// slot called to delete all contour cells
      QAction* contourCellsDeleteAllAction;
      
      /// slot called to edit contour cell colors
      QAction* contourCellsEditColorsAction;
      
      /// slot called when contour move cell selected
      QAction* contourCellsMoveWithMouseAction;
      
      /// contour information action
      QAction* contourInformationAction;
};

#endif  // __GUI_MAIN_WINDOW_LAYERS_ACTIONS_H__


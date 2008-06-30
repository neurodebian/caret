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

#include <QAction>
#include <QApplication>
#include <QDoubleSpinBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>

#include "BorderColorFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelContours.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainModelSurfacePaintToBorderConverter.h"
#include "BrainSet.h"
#include "BrainModelSurfaceCellDensityToMetric.h"
#include "BrainModelVolumeFociDensity.h"
#include "BrainModelVolumeFociUnprojector.h"
#include "CellColorFile.h"
#include "CellFile.h"
#include "CellFileProjector.h"
#include "CellProjectionFile.h"
#include "ContourCellColorFile.h"
#include "ContourCellFile.h"
#include "DisplaySettingsBorders.h"
#include "DisplaySettingsFoci.h"
#include "DisplaySettingsPaint.h"
#include "FociColorFile.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GuiAverageBorderDialog.h"
#include "GuiBorderAttributesDialog.h"
#include "GuiBorderComparisonDialog.h"
#include "GuiBorderProjectionDialog.h"
#include "GuiBrainModelSelectionComboBox.h"
#include "GuiCellAndFociAttributeAssignmentDialog.h"
#include "GuiCellAndFociReportDialog.h"
#include "GuiCellAttributesDialog.h"
#include "GuiChooseNodeAttributeColumnDialog.h"
#include "GuiColorFileEditorDialog.h"
#include "GuiColorSelectionDialog.h"
#include "GuiContourAlignmentDialog.h"
#include "GuiContourDrawDialog.h"
#include "GuiContourReconstructionDialog.h"
#include "GuiContourSetScaleDialog.h"
#include "GuiFilesModified.h"
#include "GuiFociPalsProjectionDialog.h"
#include "GuiFociUncertaintyLimitsDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiDeleteBordersByNameDialog.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowLayersActions.h"
#include "GuiMultipleInputDialog.h"
#include "GuiCellsOrFociProjectionDialog.h"
#include "GuiVolumeFileSelectionComboBox.h"
#include "PaintFile.h"
#include "QtTableDialog.h"
#include "StudyMetaDataFile.h"
#include "WuQDataEntryDialog.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiMainWindowLayersActions::GuiMainWindowLayersActions(GuiMainWindow* parent)
   : QObject(parent)
{
   setObjectName("GuiMainWindowLayersActions");
   
   bordersDrawAction = new QAction(parent);
   bordersDrawAction->setText("Draw Borders...");
   bordersDrawAction->setObjectName("bordersDrawAction");
   QObject::connect(bordersDrawAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayDrawBorderDialog()));
                    
   bordersVolumeToFiducialCellsAction = new QAction(parent);
   bordersVolumeToFiducialCellsAction->setText("Convert Volume Borders to Fiducial Cells");
   bordersVolumeToFiducialCellsAction->setObjectName("bordersVolumeToFiducialCellsAction");
   QObject::connect(bordersVolumeToFiducialCellsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersVolumeToFiducialCells()));
                    
   bordersVolumeToBordersFiducialAction = new QAction(parent);
   bordersVolumeToBordersFiducialAction->setText("Convert Volume Borders to Fiducial Borders");
   bordersVolumeToBordersFiducialAction->setObjectName("bordersVolumeToBordersFiducialAction");
   QObject::connect(bordersVolumeToBordersFiducialAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersVolumeToBordersFiducial()));
                    
   bordersAverageAction = new QAction(parent);
   bordersAverageAction->setText("Create Average Borders...");
   bordersAverageAction->setObjectName("bordersAverageAction");
   QObject::connect(bordersAverageAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersAverage()));
                    
   bordersCompareAction = new QAction(parent);
   bordersCompareAction->setText("Compute Average Separation Between Border Point Pairs...");
   bordersCompareAction->setObjectName("bordersCompareAction");
   QObject::connect(bordersCompareAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersCompare()));
                    
   bordersFromPaintAction = new QAction(parent);
   bordersFromPaintAction->setText("Create Borders From Paint Regions...");
   bordersFromPaintAction->setObjectName("bordersFromPaintAction");
   QObject::connect(bordersFromPaintAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersFromPaintAction()));
                    
   bordersCreateGridAction = new QAction(parent);
   bordersCreateGridAction->setText("Create Cartesian Flat Grid Borders...");
   bordersCreateGridAction->setObjectName("bordersCreateGridAction");
   QObject::connect(bordersCreateGridAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersCreateGrid()));
                    
   bordersCreateAnalysisGridAction = new QAction(parent);
   bordersCreateAnalysisGridAction->setText("Create Cartesian Flat Analysis Grid Borders...");
   bordersCreateAnalysisGridAction->setObjectName("bordersCreateAnalysisGridAction");
   QObject::connect(bordersCreateAnalysisGridAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersCreateAnalysisGrid()));
                    
   bordersCreateSphericalAction = new QAction(parent);
   bordersCreateSphericalAction->setText("Create Lat/Lon Spherical Borders");
   bordersCreateSphericalAction->setObjectName("bordersCreateSphericalAction");
   QObject::connect(bordersCreateSphericalAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersCreateSpherical()));
                    
   bordersConvertToVtkModelAction = new QAction(parent);
   bordersConvertToVtkModelAction->setText("Convert Displayed Borders to VTK Model");
   bordersConvertToVtkModelAction->setObjectName("bordersConvertToVtkModelAction");
   QObject::connect(bordersConvertToVtkModelAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersConvertToVtkModel()));
                    
   bordersEditColorsAction = new QAction(parent);
   bordersEditColorsAction->setText("Edit Border Colors...");
   bordersEditColorsAction->setObjectName("bordersEditColorsAction");
   QObject::connect(bordersEditColorsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersEditColors()));
                    
   bordersResampleDisplayedAction = new QAction(parent);
   bordersResampleDisplayedAction->setText("Resample Displayed Borders...");
   bordersResampleDisplayedAction->setObjectName("bordersResampleDisplayedAction");
   QObject::connect(bordersResampleDisplayedAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersResampleDisplayed()));
                    
   bordersRenameWithMouseAction = new QAction(parent);
   bordersRenameWithMouseAction->setText("Rename Borders With Mouse");
   bordersRenameWithMouseAction->setObjectName("bordersRenameWithMouseAction");
   QObject::connect(bordersRenameWithMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersRenameWithMouse()));
                    
   bordersReverseWithMouseAction = new QAction(parent);
   bordersReverseWithMouseAction->setText("Reverse Borders With Mouse");
   bordersReverseWithMouseAction->setObjectName("bordersReverseWithMouseAction");
   QObject::connect(bordersReverseWithMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersReverseWithMouse()));
                    
   bordersReverseDisplayedAction = new QAction(parent);
   bordersReverseDisplayedAction->setText("Reverse Displayed Borders");
   bordersReverseDisplayedAction->setObjectName("bordersReverseDisplayedAction");
   QObject::connect(bordersReverseDisplayedAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersReverseDisplayed()));
                    
   bordersOrientDisplayedClockwiseAction = new QAction(parent);
   bordersOrientDisplayedClockwiseAction->setText("Orient Displayed Borders Clockwise");
   bordersOrientDisplayedClockwiseAction->setObjectName("bordersOrientDisplayedClockwiseAction");
   QObject::connect(bordersOrientDisplayedClockwiseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersOrientDisplayedClockwise()));
                    
   bordersProjectAction = new QAction(parent);
   bordersProjectAction->setText("Project Borders...");
   bordersProjectAction->setObjectName("bordersProjectAction");
   QObject::connect(bordersProjectAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersProject()));
                    
   bordersDeletePointWithMouseAction = new QAction(parent);
   bordersDeletePointWithMouseAction->setText("Delete Border Point With Mouse");
   bordersDeletePointWithMouseAction->setObjectName("bordersDeletePointWithMouseAction");
   QObject::connect(bordersDeletePointWithMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersDeletePointWithMouse()));
                    
   bordersDeleteWithMouseAction = new QAction(parent);
   bordersDeleteWithMouseAction->setText("Delete Border With Mouse");
   bordersDeleteWithMouseAction->setObjectName("bordersDeleteWithMouseAction");
   QObject::connect(bordersDeleteWithMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersDeleteWithMouse()));
                    
   bordersDeleteByNameAction = new QAction(parent);
   bordersDeleteByNameAction->setText("Delete Borders By Name...");
   bordersDeleteByNameAction->setObjectName("bordersDeleteByNameAction");
   QObject::connect(bordersDeleteByNameAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotDeleteBordersByName()));
                    
   bordersCopyByNameAction = new QAction(parent);
   bordersCopyByNameAction->setText("Duplicate Border By Name...");
   bordersCopyByNameAction->setObjectName("bordersCopyByNameAction");
   QObject::connect(bordersCopyByNameAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCopyBordersByName()));
                    
   bordersDeleteAllAction = new QAction(parent);
   bordersDeleteAllAction->setText("Delete All Borders...");
   bordersDeleteAllAction->setObjectName("bordersDeleteAllAction");
   QObject::connect(bordersDeleteAllAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersDeleteAll()));
                    
   bordersMovePointWithMouseAction = new QAction(parent);
   bordersMovePointWithMouseAction->setText("Move Border Point With Mouse");
   bordersMovePointWithMouseAction->setObjectName("bordersMovePointWithMouseAction");
   QObject::connect(bordersMovePointWithMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersMovePointWithMouse()));
                    
   bordersDeletePointsOutsideSurfaceAction = new QAction(parent);
   bordersDeletePointsOutsideSurfaceAction->setText("Delete Border Points Outside Surface");
   bordersDeletePointsOutsideSurfaceAction->setObjectName("bordersDeletePointsOutsideSurfaceAction");
   QObject::connect(bordersDeletePointsOutsideSurfaceAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersDeletePointsOutsideSurface()));
                    
   bordersEditAttributesAction = new QAction(parent);
   bordersEditAttributesAction->setText("Edit Border Attributes...");
   bordersEditAttributesAction->setObjectName("bordersEditAttributesAction");
   QObject::connect(bordersEditAttributesAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersEditAttributes()));
                    
   bordersClearHighlightingAction = new QAction(parent);
   bordersClearHighlightingAction->setText("Clear Border Highlighting");
   bordersClearHighlightingAction->setObjectName("bordersClearHighlightingAction");
   QObject::connect(bordersClearHighlightingAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersClearHighlighting()));
                    
   bordersCreateInterpolatedAction = new QAction(parent);
   bordersCreateInterpolatedAction->setText("Create Interpolated Borders...");
   bordersCreateInterpolatedAction->setObjectName("bordersCreateInterpolatedAction");
   QObject::connect(bordersCreateInterpolatedAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotBordersCreateInterpolated()));
     
   borderDrawUpdateAction = new QAction(parent);
   borderDrawUpdateAction->setText("Draw Border Update...");
   borderDrawUpdateAction->setObjectName("borderDrawUpdateAction");
   QObject::connect(borderDrawUpdateAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayBorderDrawUpdateDialog()));
                    
   fociMapStereotaxicFocusAction = new QAction(parent);
   fociMapStereotaxicFocusAction->setText("Map Stereotaxic Focus...");
   fociMapStereotaxicFocusAction->setObjectName("fociMapStereotaxicFocusAction");
   QObject::connect(fociMapStereotaxicFocusAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociMapStereotaxicFocus()));
                    
   fociConvertToVtkModelAction = new QAction(parent);
   fociConvertToVtkModelAction->setText("Convert Displayed Foci to VTK Model");
   fociConvertToVtkModelAction->setObjectName("fociConvertToVtkModelAction");
   QObject::connect(fociConvertToVtkModelAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociConvertToVtkModel()));
                    
   fociEditColorsAction = new QAction(parent);
   fociEditColorsAction->setText("Edit Foci Colors...");
   fociEditColorsAction->setObjectName("fociEditColorsAction");
   QObject::connect(fociEditColorsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociEditColors()));
                    
   fociProjectAction = new QAction(parent);
   fociProjectAction->setText("Project Fiducial Foci...");
   fociProjectAction->setObjectName("fociProjectAction");
   QObject::connect(fociProjectAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociProject()));
                    
   fociProjectToVolumeAction = new QAction(parent);
   fociProjectToVolumeAction->setText("Unproject Foci to Volume...");
   fociProjectToVolumeAction->setObjectName("fociProjectToVolumeAction");
   QObject::connect(fociProjectToVolumeAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociProjectToVolume()));
                    
   fociPalsProjectAction = new QAction(parent);
   fociPalsProjectAction->setText("Project Foci To PALS Atlas...");
   fociPalsProjectAction->setObjectName("fociPalsProjectAction");
   QObject::connect(fociPalsProjectAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociPalsProject()));
                    
   fociDeleteAllAction = new QAction(parent);
   fociDeleteAllAction->setText("Delete All Foci...");
   fociDeleteAllAction->setObjectName("fociDeleteAllAction");
   QObject::connect(fociDeleteAllAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociDeleteAll()));
    
   fociAssignClassToDisplayedFoci = new QAction(parent);
   fociAssignClassToDisplayedFoci->setText("Assign Class to Displayed Foci...");
   fociAssignClassToDisplayedFoci->setObjectName("fociAssignClassToDisplayedFoci");
   QObject::connect(fociAssignClassToDisplayedFoci, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociAssignClassToDisplayedFoci()));
                    
   fociDeleteNonDisplayedAction = new QAction(parent);
   fociDeleteNonDisplayedAction->setText("Delete Foci Not Displayed due to Display Control Selections...");
   fociDeleteNonDisplayedAction->setObjectName("fociDeleteNonDisplayedAction");
   QObject::connect(fociDeleteNonDisplayedAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociDeleteNonDisplayed()));
                    
   fociDeleteNonMatchingColors = new QAction(parent);
   fociDeleteNonMatchingColors->setText("Delete Foci Colors Not Matching Foci...");
   fociDeleteNonMatchingColors->setObjectName("fociDeleteNonMatchingColors");
   QObject::connect(fociDeleteNonMatchingColors, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociDeleteNonMatchingColors()));
                    
   fociDeleteUsingMouseAction = new QAction(parent);
   fociDeleteUsingMouseAction->setText("Delete Focus Using Mouse");
   fociDeleteUsingMouseAction->setObjectName("fociDeleteUsingMouseAction");
   QObject::connect(fociDeleteUsingMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociDeleteUsingMouse()));
                    
   fociUncertaintyLimitsAction = new QAction(parent);
   fociUncertaintyLimitsAction->setText("Convert Uncertainty Limits to RGB Paint...");
   fociUncertaintyLimitsAction->setObjectName("fociUncertaintyLimitsAction");
   QObject::connect(fociUncertaintyLimitsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociUncertaintyLimits()));
                    
   fociReportAction = new QAction(parent);
   fociReportAction->setText("Foci Report...");
   fociReportAction->setObjectName("fociReportAction");
   QObject::connect(fociReportAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociReport()));
       
   fociAttributeAssignmentAction = new QAction(parent);
   fociAttributeAssignmentAction->setText("Attribute Assignment...");
   fociAttributeAssignmentAction->setObjectName("fociAttributeAssigmentAction");
   QObject::connect(fociAttributeAssignmentAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayFociAttributeAssignmentDialog()));
       
   fociStudyInfoToStudyMetaDataFileAction = new QAction(parent);
   fociStudyInfoToStudyMetaDataFileAction->setText("Move Foci Study Info to Study Metadata File");
   fociStudyInfoToStudyMetaDataFileAction->setObjectName("fociStudyInfoToStudyMetaDataFileAction");
   QObject::connect(fociStudyInfoToStudyMetaDataFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociStudyInfoToStudyMetaDataFile()));
                    
   fociUpdatePubMedIDIfFocusNameMatchesStudyNameAction = new QAction(parent);
   fociUpdatePubMedIDIfFocusNameMatchesStudyNameAction->setText("Update Focus' PubMed ID if Focus Name Matches Study Name");
   fociUpdatePubMedIDIfFocusNameMatchesStudyNameAction->setObjectName("fociUpdatePubMedIDIfFocusNameMatchesStudyNameAction");
   QObject::connect(fociUpdatePubMedIDIfFocusNameMatchesStudyNameAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociUpdatePubMedIDIfFocusNameMatchesStudyName()));
                    
   fociClearHighlightingAction = new QAction(parent);
   fociClearHighlightingAction->setText("Clear Foci Highlighting");
   fociClearHighlightingAction->setObjectName("fociClearHighlightingAction");
   QObject::connect(fociClearHighlightingAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociClearHighlighting()));
                    
   cellsConvertToVtkModelAction = new QAction(parent);
   cellsConvertToVtkModelAction->setText("Convert Displayed Cells to VTK Model");
   cellsConvertToVtkModelAction->setObjectName("cellsConvertToVtkModelAction");
   QObject::connect(cellsConvertToVtkModelAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellsConvertToVtkModel()));
                    
   cellsAddAction = new QAction(parent);
   cellsAddAction->setText("Add Cells With Mouse...");
   cellsAddAction->setObjectName("cellsAddAction");
   QObject::connect(cellsAddAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellsAdd()));
                    
   cellsEditColorsAction = new QAction(parent);
   cellsEditColorsAction->setText("Edit Cell Colors...");
   cellsEditColorsAction->setObjectName("cellsEditColorsAction");
   QObject::connect(cellsEditColorsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellsEditColors()));
                    
   fociDensityToMetricAction = new QAction(parent);
   fociDensityToMetricAction->setText("Convert Foci Density to Metric...");
   fociDensityToMetricAction->setObjectName("fociDensityToMetricAction");
   QObject::connect(fociDensityToMetricAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociDensityToMetric()));
                    
   fociDensityToVolumeAction = new QAction(parent);
   fociDensityToVolumeAction->setText("Convert Foci Density to Functional Volume...");
   fociDensityToVolumeAction->setObjectName("fociDensityToVolumeAction");
   QObject::connect(fociDensityToVolumeAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFociDensityToFunctionalVolume()));
                    
   cellsProjectAction = new QAction(parent);
   cellsProjectAction->setText("Project Fiducial Cells...");
   cellsProjectAction->setObjectName("cellsProjectAction");
   QObject::connect(cellsProjectAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellsProject()));
                    
   cellsDeleteAllAction = new QAction(parent);
   cellsDeleteAllAction->setText("Delete All Cells...");
   cellsDeleteAllAction->setObjectName("cellsDeleteAllAction");
   QObject::connect(cellsDeleteAllAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellsDeleteAll()));
                    
   cellsEditAttributesAction = new QAction(parent);
   cellsEditAttributesAction->setText("Edit Cell Attributes");
   cellsEditAttributesAction->setObjectName("cellsEditAttributesAction");
   QObject::connect(cellsEditAttributesAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellsEditAttributes()));
                    
   cellsDeleteUsingMouseAction = new QAction(parent);
   cellsDeleteUsingMouseAction->setText("Delete Cells Using Mouse");
   cellsDeleteUsingMouseAction->setObjectName("cellsDeleteUsingMouseAction");
   QObject::connect(cellsDeleteUsingMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellsDeleteUsingMouse()));
                    
   cellsDensityToMetricAction = new QAction(parent);
   cellsDensityToMetricAction->setText("Convert Cell Density to Metric...");
   cellsDensityToMetricAction->setObjectName("cellsDensityToMetricAction");
   QObject::connect(cellsDensityToMetricAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellsDensityToMetric()));
                    
   cellReportAction = new QAction(parent);
   cellReportAction->setText("Cell Report...");
   cellReportAction->setObjectName("cellReportAction");
   QObject::connect(cellReportAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCellReport()));
                    
   contourNewSetAction = new QAction(parent);
   contourNewSetAction->setText("Create New Contour Set");
   contourNewSetAction->setObjectName("contourNewSetAction");
   QObject::connect(contourNewSetAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourNewSet()));
                    
   contourApplyCurrentViewAction = new QAction(parent);
   contourApplyCurrentViewAction->setText("Apply Current View");
   contourApplyCurrentViewAction->setObjectName("contourApplyCurrentViewAction");
   QObject::connect(contourApplyCurrentViewAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourApplyCurrentView()));
                    
   contourSetScaleAction = new QAction(parent);
   contourSetScaleAction->setText("Set Contour Scale...");
   contourSetScaleAction->setObjectName("contourSetScaleAction");
   QObject::connect(contourSetScaleAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourSetScale()));
                    
   contourSectionsAction = new QAction(parent);
   contourSectionsAction->setText("Select Contour Sections...");
   contourSectionsAction->setObjectName("contourSectionsAction");
   QObject::connect(contourSectionsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourSections()));
                    
   contourSpacingAction = new QAction(parent);
   contourSpacingAction->setText("Set Contour Section Spacing...");
   contourSpacingAction->setObjectName("contourSpacingAction");
   QObject::connect(contourSpacingAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourSpacing()));
                    
   contourDrawAction = new QAction(parent);
   contourDrawAction->setText("Draw Contours...");
   contourDrawAction->setObjectName("contourDrawAction");
   QObject::connect(contourDrawAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourDraw()));
                    
   contourAlignAction = new QAction(parent);
   contourAlignAction->setText("Align Contours...");
   contourAlignAction->setObjectName("contourAlignAction");
   QObject::connect(contourAlignAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourAlign()));
                    
   contourMergeAction = new QAction(parent);
   contourMergeAction->setText("Merge Contours");
   contourMergeAction->setObjectName("contourMergeAction");
   QObject::connect(contourMergeAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourMerge()));
                    
   contourMovePointAction = new QAction(parent);
   contourMovePointAction->setText("Move Contour Point With Mouse");
   contourMovePointAction->setObjectName("contourMovePointAction");
   QObject::connect(contourMovePointAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourMovePoint()));
                    
   contourDeleteAllAction = new QAction(parent);
   contourDeleteAllAction->setText("Delete All Contours...");
   contourDeleteAllAction->setObjectName("contourDeleteAllAction");
   QObject::connect(contourDeleteAllAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourDeleteAll()));
                    
   contourReverseAction = new QAction(parent);
   contourReverseAction->setText("Reverse Contour With Mouse");
   contourReverseAction->setObjectName("contourReverseAction");
   QObject::connect(contourReverseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourReverse()));
                    
   contourDeletePointAction = new QAction(parent);
   contourDeletePointAction->setText("Delete Contour Point With Mouse");
   contourDeletePointAction->setObjectName("contourDeletePointAction");
   QObject::connect(contourDeletePointAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourDeletePoint()));
                    
   contourDeleteAction = new QAction(parent);
   contourDeleteAction->setText("Delete Contour With Mouse");
   contourDeleteAction->setObjectName("contourDeleteAction");
   QObject::connect(contourDeleteAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourDelete()));
                    
   contourReconstructAction = new QAction(parent);
   contourReconstructAction->setText("Reconstruct Into Surface...");
   contourReconstructAction->setObjectName("contourReconstructAction");
   QObject::connect(contourReconstructAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourReconstruct()));
      
   contourResampleAction = new QAction(parent);
   contourResampleAction->setText("Resample All Contours...");
   contourResampleAction->setObjectName("contourResampleAction");
   QObject::connect(contourResampleAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourResample()));
                    
   contourCleanUpAction = new QAction(parent);
   contourCleanUpAction->setText("Cleanup Contours");
   contourCleanUpAction->setObjectName("contourCleanUpAction");
   QObject::connect(contourCleanUpAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourCleanUp()));
                    
   contourCellsAddAction = new QAction(parent);
   contourCellsAddAction->setText("Add Contour Cells...");
   contourCellsAddAction->setObjectName("contourCellsAddAction");
   QObject::connect(contourCellsAddAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourCellsAdd()));
                    
   contourCellsDeleteWithMouseAction = new QAction(parent);
   contourCellsDeleteWithMouseAction->setText("Delete Contour Cells With Mouse");
   contourCellsDeleteWithMouseAction->setObjectName("contourCellsDeleteWithMouseAction");
   QObject::connect(contourCellsDeleteWithMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourCellsDeleteWithMouse()));
                    
   contourCellsDeleteAllAction = new QAction(parent);
   contourCellsDeleteAllAction->setText("Delete All Contour Cells...");
   contourCellsDeleteAllAction->setObjectName("contourCellsDeleteAllAction");
   QObject::connect(contourCellsDeleteAllAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourCellsDeleteAll()));
                    
   contourCellsEditColorsAction = new QAction(parent);
   contourCellsEditColorsAction->setText("Edit Contour Cell Colors...");
   contourCellsEditColorsAction->setObjectName("contourCellsEditColorsAction");
   QObject::connect(contourCellsEditColorsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourCellsEditColors()));
                    
   contourCellsMoveWithMouseAction = new QAction(parent);
   contourCellsMoveWithMouseAction->setText("Move Contour Cells With Mouse");
   contourCellsMoveWithMouseAction->setObjectName("contourCellsMoveWithMouseAction");
   QObject::connect(contourCellsMoveWithMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourCellsMoveWithMouse()));

   contourInformationAction = new QAction(parent);
   contourInformationAction->setText("Contour Information...");
   contourInformationAction->setObjectName("contourInformationAction");
   QObject::connect(contourInformationAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotContourInformation()));
}

/**
 * Destructor.
 */
GuiMainWindowLayersActions::~GuiMainWindowLayersActions()
{
}

/**
 * slot for cell report.
 */
void 
GuiMainWindowLayersActions::slotCellReport()
{
   GuiCellAndFociReportDialog cfrd(theMainWindow, false);
   cfrd.exec();
   cfrd.close();
   QtTableDialog* tableDialog = cfrd.getResultsTableDialog();
   if (tableDialog != NULL) {
      tableDialog->show();
      tableDialog->activateWindow();
   }
}
      
/**
 * slot for converting cells to vtk model.
 */
void 
GuiMainWindowLayersActions::slotCellsConvertToVtkModel()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      theMainWindow->getBrainSet()->convertDisplayedCellsToVtkModel(bms);
      GuiFilesModified fm;
      fm.setVtkModelModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
   QApplication::restoreOverrideCursor();
}

/**
 * Slot to display add cells dialog.
 */
void
GuiMainWindowLayersActions::slotCellsAdd()
{
   theMainWindow->getAddCellsDialog(true);
}

/**
 * Slot for converting cell density to metric.
 */
void
GuiMainWindowLayersActions::slotCellsDensityToMetric()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelSurface* flatSurface = theMainWindow->getBrainModelSurface();
   if (flatSurface->getIsFlatSurface() == false) {
      QMessageBox::critical(theMainWindow, 
                            "ERROR", 
                            "The surface in the main window must be flat.");
   }
   
   static float gridSpacing = 5.0;
   bool valid = false;
   gridSpacing = QInputDialog::getDouble(theMainWindow, "Cell Density",
                                         "Grid Spacing",
                                         gridSpacing,
                                         0.001,
                                         10000000.0,
                                         3,
                                         &valid);
   if (valid) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      BrainModelSurfaceCellDensityToMetric cdm(bs,
                                               flatSurface,
                                               bs->getCellProjectionFile(),
                                               bs->getMetricFile(),
                                               gridSpacing,
                                               true);
      try {
         cdm.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
      
      //
      // Notify that metrics have changed and update display
      //
      GuiFilesModified fm;
      fm.setMetricModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
      
      theMainWindow->speakText("Cell density has been created.", false);
   }
}

/**
 * Slot for Edit Cell Colors Dialog
 */
void
GuiMainWindowLayersActions::slotCellsEditColors()
{
   GuiColorFileEditorDialog* gfed = new GuiColorFileEditorDialog(theMainWindow, 
                                                                 theMainWindow->getBrainSet()->getCellColorFile(),
                                                                 true,
                                                                 true, 
                                                                 false,
                                                                 true,
                                                                 false);
   QObject::connect(gfed, SIGNAL(redrawRequested()),
                    this, SLOT(slotCellsColorsChanged()));
   gfed->show();
}

/**
 * This slot is called by the color editor when user pressed apply or dialog is closed.
 */
void
GuiMainWindowLayersActions::slotCellsColorsChanged()
{
   GuiFilesModified fm;
   fm.setCellColorModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Slot for Project Cells
 */
void
GuiMainWindowLayersActions::slotCellsProject()
{
   GuiCellsOrFociProjectionDialog pd(theMainWindow, GuiCellsOrFociProjectionDialog::FILE_TYPE_CELL);
   pd.exec();
   
}

/**
 * Slot for Delete Cells By Name
 */
void
GuiMainWindowLayersActions::slotCellsEditAttributes()
{
   GuiCellAttributesDialog* cad = new GuiCellAttributesDialog(theMainWindow);
   cad->show();
}

/**
 * Slot for Delete All Cells
 */
void
GuiMainWindowLayersActions::slotCellsDeleteAll()
{
   if (QMessageBox::warning(theMainWindow, 
                            "Delete All Cells", 
                            "Are you sure you want to delete all cells?",
                            (QMessageBox::Yes | QMessageBox::No),
                            QMessageBox::Yes)
                               == QMessageBox::Yes) {
      theMainWindow->getBrainSet()->deleteAllCells(true, true);
      GuiFilesModified fm;
      fm.setCellModified();
      fm.setCellProjectionModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();   
   }
}

/**
 * Slot for Delete Cells Using Mouse
 */
void
GuiMainWindowLayersActions::slotCellsDeleteUsingMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CELL_DELETE);
}

/**
 * slot for updating foci PubMed ID if focus name same as study name.
 */
void 
GuiMainWindowLayersActions::slotFociUpdatePubMedIDIfFocusNameMatchesStudyName()
{
   if (QMessageBox::question(theMainWindow,
                               "Confirm",
                               "Update foci's PubMed IDs if the focus\nname matches the name of a study?",
                               (QMessageBox::Yes | QMessageBox::No),
                               QMessageBox::Yes)
                                  == QMessageBox::Yes) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      BrainSet* bs = theMainWindow->getBrainSet();
      FociProjectionFile* fpf = bs->getFociProjectionFile();
      StudyMetaDataFile* smdf = bs->getStudyMetaDataFile();
      fpf->updatePubMedIDIfCellNameMatchesStudyName(smdf);

      GuiFilesModified fm;
      fm.setFociModified();
      theMainWindow->fileModificationUpdate(fm);
      QApplication::restoreOverrideCursor();
   }
}
      
/**
 * slot for converting foci study info to study metadata.
 */
void 
GuiMainWindowLayersActions::slotFociStudyInfoToStudyMetaDataFile()
{
   theMainWindow->getBrainSet()->moveFociStudyInfoToStudyMetaDataFile();
   
   GuiFilesModified fm;
   fm.setFociModified();
   fm.setStudyMetaDataModified();
   theMainWindow->fileModificationUpdate(fm);
   
   const QString msg("The StudyInfo from the Foci Projection File has been moved\n"
                     "to the Study Metadata File.  As a result, both the \n"
                     "StudyMetaData and Foci Projection Files need to be saved.");
   QMessageBox::information(theMainWindow, "INFO", msg);
}
      
/**
 * slot for foci clear highlighting action.
 */
void 
GuiMainWindowLayersActions::slotFociClearHighlighting()
{
   FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   fpf->clearAllHighlightFlags();
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * slot for converting foci density to functional volume.
 */
void 
GuiMainWindowLayersActions::slotFociDensityToFunctionalVolume()
{
   WuQDataEntryDialog ded(theMainWindow);
   ded.setWindowTitle("Foci Density to Volume");
   GuiVolumeFileSelectionComboBox* volumeSelectionComboBox =
      new GuiVolumeFileSelectionComboBox(VolumeFile::VOLUME_TYPE_FUNCTIONAL);
   ded.addWidget("Functional Volume", volumeSelectionComboBox);
   QDoubleSpinBox* roiSizeDoubleSpinBox =
      ded.addDoubleSpinBox("ROI Size for Density", 3.0);
   roiSizeDoubleSpinBox->setSingleStep(2.0);
   if (ded.exec() == WuQDataEntryDialog::Accepted) {

      try {
         BrainSet* brainSet = theMainWindow->getBrainSet();
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         BrainModelVolumeFociDensity bmvfd(brainSet,
                                           brainSet->getFociProjectionFile(),
                                           roiSizeDoubleSpinBox->value(),
                                           volumeSelectionComboBox->getSelectedVolumeFile());
         bmvfd.execute();
         QApplication::restoreOverrideCursor();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
      
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
}      
      
/**
 * Slot for converting foci density to metric.
 */
void
GuiMainWindowLayersActions::slotFociDensityToMetric()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   BrainModelSurface* flatSurface = theMainWindow->getBrainModelSurface();
   if (flatSurface->getIsFlatSurface() == false) {
      QMessageBox::critical(theMainWindow, 
                            "ERROR", 
                            "The surface in the main window must be flat.");
   }
   
   static float gridSpacing = 5.0;
   bool valid = false;
   gridSpacing = QInputDialog::getDouble(theMainWindow, "Foci Density",
                                         "Grid Spacing",
                                         gridSpacing,
                                         0.001,
                                         10000000.0,
                                         3,
                                         &valid);
   if (valid) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      BrainModelSurfaceCellDensityToMetric cdm(bs,
                                               flatSurface,
                                               bs->getFociProjectionFile(),
                                               bs->getMetricFile(),
                                               gridSpacing,
                                               true);
      try {
         cdm.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
      
      //
      // Notify that metrics have changed and update display
      //
      GuiFilesModified fm;
      fm.setMetricModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
   }
}

/**
 * slot for foci report.
 */
void 
GuiMainWindowLayersActions::slotFociReport()
{
   GuiCellAndFociReportDialog cfrd(theMainWindow, true);
   cfrd.exec();
   cfrd.close();
   QtTableDialog* tableDialog = cfrd.getResultsTableDialog();
   if (tableDialog != NULL) {
      tableDialog->show();
      tableDialog->activateWindow();
   }
}
      
/**
 * slot for converting foci to VTK model.
 */
void 
GuiMainWindowLayersActions::slotFociConvertToVtkModel()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      theMainWindow->getBrainSet()->convertDisplayedFociToVtkModel(bms);
      GuiFilesModified fm;
      fm.setVtkModelModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
   QApplication::restoreOverrideCursor();
}
      
/**
 * Slot for Map Stereotaxic Focus Dialog
 */
void
GuiMainWindowLayersActions::slotFociMapStereotaxicFocus()
{
   theMainWindow->getMapStereotaxicFocusDialog(true);
}

/**
 * Slot for uncertainty limits to rgb paint dialog.
 */
void
GuiMainWindowLayersActions::slotFociUncertaintyLimits()
{
   GuiFociUncertaintyLimitsDialog* fuld = new GuiFociUncertaintyLimitsDialog(theMainWindow);
   fuld->exec();
}

/**
 * Slot for Edit Foci Colors Dialog
 */
void
GuiMainWindowLayersActions::slotFociEditColors()
{
   GuiColorFileEditorDialog* gfed = new GuiColorFileEditorDialog(theMainWindow, 
                                                                 theMainWindow->getBrainSet()->getFociColorFile(),
                                                                 true, 
                                                                 true,
                                                                 false,
                                                                 true,
                                                                 false);
   QObject::connect(gfed, SIGNAL(redrawRequested()),
                    this, SLOT(slotFociColorsChanged()));
   gfed->show();
}

/**
 * This slot is called by the color editor when user pressed apply or dialog is closed.
 */
void
GuiMainWindowLayersActions::slotFociColorsChanged()
{
   GuiFilesModified fm;
   fm.setFociColorModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Slot for Project Foci
 */
void
GuiMainWindowLayersActions::slotFociProject()
{
   GuiCellsOrFociProjectionDialog pd(theMainWindow, GuiCellsOrFociProjectionDialog::FILE_TYPE_FOCI);
   pd.exec();
}

/**
 * Slot for Project Foci
 */
void
GuiMainWindowLayersActions::slotFociPalsProject()
{
   GuiFociPalsProjectionDialog pd(theMainWindow);
   pd.exec();
}

/**
 * slot for foci project to volume.
 */
void 
GuiMainWindowLayersActions::slotFociProjectToVolume()
{
   //
   // Right, left, and cerebellum surface selection combo boxes
   //
   GuiBrainModelSelectionComboBox* leftSurfaceComboBox =
      new GuiBrainModelSelectionComboBox(
         GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_ALL |
            GuiBrainModelSelectionComboBox::OPTION_SHOW_ADD_NEW,
         "NONE");
   leftSurfaceComboBox->setSelectedBrainModelToFirstSurfaceOfType(
      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
      Structure::STRUCTURE_TYPE_CORTEX_LEFT);
   GuiBrainModelSelectionComboBox* rightSurfaceComboBox =
      new GuiBrainModelSelectionComboBox(
         GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_ALL |
            GuiBrainModelSelectionComboBox::OPTION_SHOW_ADD_NEW,
         "NONE");
   rightSurfaceComboBox->setSelectedBrainModelToFirstSurfaceOfType(
      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
      Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
   GuiBrainModelSelectionComboBox* cerebellumSurfaceComboBox =
      new GuiBrainModelSelectionComboBox(
         GuiBrainModelSelectionComboBox::OPTION_SHOW_SURFACES_ALL |
            GuiBrainModelSelectionComboBox::OPTION_SHOW_ADD_NEW,
         "NONE");
   cerebellumSurfaceComboBox->setSelectedBrainModelToFirstSurfaceOfType(
      BrainModelSurface::SURFACE_TYPE_FIDUCIAL,
      Structure::STRUCTURE_TYPE_CEREBELLUM);
   
   //
   // Create dialog for projecting the foci to the volume
   //
   WuQDataEntryDialog ftv(theMainWindow);
   ftv.setTextAtTop("The selected surfaces will be used to unproject "
                    "the foci and determine the coordinates for "
                    "display of foci in the volume.  For this to "
                    "function correctly, the foci must have been "
                    "projected to the surfaces.",
                    true);
   ftv.setWindowTitle("Unproject Foci to Volume");
   ftv.addWidget("Left ", leftSurfaceComboBox);
   ftv.addWidget("Right ", rightSurfaceComboBox);
   ftv.addWidget("Cerebellum ", cerebellumSurfaceComboBox);
   if (ftv.exec() == WuQDataEntryDialog::Accepted) {
      //
      // Get the selected surfaces
      //
      BrainModelSurface* leftBMS  = 
         leftSurfaceComboBox->getSelectedBrainModelSurface();
      BrainModelSurface* rightBMS = 
         rightSurfaceComboBox->getSelectedBrainModelSurface();
      BrainModelSurface* cerebellumBMS = 
         cerebellumSurfaceComboBox->getSelectedBrainModelSurface();
         
      //
      // Project foci to volume
      //
      BrainModelVolumeFociUnprojector bmvfu(theMainWindow->getBrainSet(),
                                            leftBMS,
                                            rightBMS,
                                            cerebellumBMS,
                                            theMainWindow->getBrainSet()->getFociProjectionFile());
      try {
         bmvfu.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
      
      //
      // Update GUI
      //
      GuiFilesModified fm;
      fm.setFociModified();
      fm.setFociProjectionModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();   
   }
}
      
/**
 * Slot for Delete All Foci
 */
void
GuiMainWindowLayersActions::slotFociDeleteAll()
{
   if (QMessageBox::warning(theMainWindow, 
                              "Delete All Foci", 
                              "Are you sure you want to delete all foci?",
                              (QMessageBox::Yes | QMessageBox::No),
                              QMessageBox::No) 
                                 == QMessageBox::Yes) {
      theMainWindow->getBrainSet()->deleteAllFociProjections();
      GuiFilesModified fm;
      fm.setFociModified();
      fm.setFociProjectionModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();   
   }
}

/**
 * slot for foci assign class name.
 */
void 
GuiMainWindowLayersActions::slotFociAssignClassToDisplayedFoci()
{
   bool ok = false;
   const QString name = QInputDialog::getText(theMainWindow,
                                              "Assign Class Name to Displayed Foci",
                                              "Class Name",
                                              QLineEdit::Normal,
                                              "",
                                              &ok);
   if (ok) {
      FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
      fpf->assignClassToDisplayedFoci(name);
      
      //
      // Find the matching color
      //
      bool fociColorMatch = false;
      FociColorFile* fociColorFile = theMainWindow->getBrainSet()->getFociColorFile();
      const int fociColorIndex = fociColorFile->getColorIndexByName(name, fociColorMatch);
     
      //
      // Foci color may need to be created
      //
      bool createFociColor = false;
      if ((fociColorIndex >= 0) && (fociColorMatch == true)) {
         createFociColor = false;
      }
      else if ((fociColorIndex >= 0) && (fociColorMatch == false)) {
         QString msg("Use foci color \"");
         msg.append(fociColorFile->getColorNameByIndex(fociColorIndex));
         msg.append("\" for focus ");
         msg.append(name);
         msg.append(" ?");
         const QString noButtonText("No, define color " + name);
         QMessageBox msgBox(theMainWindow);
         msgBox.setWindowTitle("Use Partially Matching Color");
         msgBox.setText(msg);
         msgBox.addButton("Yes", QMessageBox::YesRole);
         QPushButton* noPushButton = msgBox.addButton(noButtonText, QMessageBox::NoRole);
         msgBox.exec();
         if (msgBox.clickedButton() == noPushButton) {
            createFociColor = true;      
         }
      }   
      else {
         createFociColor = true;
      }
      if (createFociColor) {
         QString title("Create Focus Color: ");
         title.append(name);
         GuiColorSelectionDialog* csd = new GuiColorSelectionDialog(theMainWindow,
                                                                    title,
                                                                    false,
                                                                    false,
                                                                    false,
                                                                    false);
         csd->exec();

         //
         // Add new foci color
         //
         float pointSize = 2.0, lineSize = 1.0;
         unsigned char r, g, b, a;
         ColorFile::ColorStorage::SYMBOL symbol;
         csd->getColorInformation(r, g, b, a, pointSize, lineSize, symbol);
         fociColorFile->addColor(name, r, g, b, a, pointSize, lineSize, symbol);
      }
      
      GuiFilesModified fm;
      fm.setFociModified();
      fm.setFociColorModified();
      fm.setFociProjectionModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();  
   }
}

/**
 * Slot for Delete Non-Dispalyed Foci
 */
void
GuiMainWindowLayersActions::slotFociDeleteNonDisplayed()
{
   Structure structure;
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      structure = bms->getStructure();
   }
   
   QMessageBox msgBox(theMainWindow);
   QString msg("Delete foci not displayed due to Display Control Settings.");
   QPushButton* yesPushButton = msgBox.addButton("Yes", QMessageBox::YesRole);
   QPushButton* yesStructurePushButton = NULL;
   if ((structure.getType() == Structure::STRUCTURE_TYPE_CORTEX_LEFT) ||
       (structure.getType() == Structure::STRUCTURE_TYPE_CORTEX_RIGHT)) {
      yesStructurePushButton = msgBox.addButton("Yes, Not on Main Window Surface",
                                                QMessageBox::YesRole);
      msg += ("\n"
              "You may also remove those not associated with main window surface.");
   }
   QPushButton* noPushButton  = msgBox.addButton("No", QMessageBox::YesRole);
   msgBox.setText(msg);
   msgBox.setWindowTitle("Confirm Foci Deletion");
   msgBox.exec();
   
   if (msgBox.clickedButton() != noPushButton) {
      if (msgBox.clickedButton() == yesPushButton) {
         structure = Structure::STRUCTURE_TYPE_INVALID;
      }
      FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
      fpf->deleteAllNonDisplayedCellProjections(structure);
      GuiFilesModified fm;
      fm.setFociModified();
      fm.setFociProjectionModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();   
   }
}

/**
 * slot for foci non-matching color deletion.
 */
void 
GuiMainWindowLayersActions::slotFociDeleteNonMatchingColors()
{
   if (QMessageBox::warning(theMainWindow, 
                            "Delete Non-Matching Foci Colors", 
                            "Are you sure you want to delete\n"
                              "all foci colors that do not match\n"
                              "the names of any foci?",
                            (QMessageBox::Yes | QMessageBox::No),
                            QMessageBox::Yes) 
                               == QMessageBox::Yes) {
      const FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
      FociColorFile* fcf = theMainWindow->getBrainSet()->getFociColorFile();
      fcf->removeNonMatchingColors(fpf);
      GuiFilesModified fm;
      fm.setFociColorModified();
      fm.setFociProjectionModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();   
   }
}
      
/**
 * Slot for Delete Foci Using Mouse
 */
void
GuiMainWindowLayersActions::slotFociDeleteUsingMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_FOCI_DELETE);
}

/**
 * slot for borders from paint.
 */
void 
GuiMainWindowLayersActions::slotBordersFromPaintAction()
{
   BrainSet* bs = theMainWindow->getBrainSet();
   GuiChooseNodeAttributeColumnDialog cnacd(theMainWindow,
                                            GUI_NODE_FILE_TYPE_PAINT,
                                            "",
                                            false,
                                            false);
   DisplaySettingsPaint* dsp = bs->getDisplaySettingsPaint();
   const int columnNumber = dsp->getFirstSelectedColumnForBrainModel(0);
   cnacd.setSelectedColumnNumber(columnNumber);
   if (cnacd.exec() == GuiChooseNodeAttributeColumnDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      BrainModelSurfacePaintToBorderConverter ptb(bs,
                                                  theMainWindow->getBrainModelSurface(),
                                                  bs->getPaintFile(),
                                                  cnacd.getSelectedColumnNumber());
      try {
         ptb.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
      
      DisplaySettingsBorders* dsb = bs->getDisplaySettingsBorders();
      dsb->setDisplayBorders(true);
      GuiFilesModified fm;
      fm.setBorderModified();
      fm.setBorderColorModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
}
      
/**
 * slot for converting borders to a VTK model.
 */
void 
GuiMainWindowLayersActions::slotBordersConvertToVtkModel()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      theMainWindow->getBrainSet()->convertDisplayedBordersToVtkModel(bms);
      GuiFilesModified fm;
      fm.setVtkModelModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
   QApplication::restoreOverrideCursor();
}
      
/**
 * slot for comparing borders.
 */
void 
GuiMainWindowLayersActions::slotBordersCompare()
{
   static GuiBorderComparisonDialog* bcd = NULL;
   if (bcd == NULL) {
      bcd = new GuiBorderComparisonDialog(theMainWindow);
   }
   bcd->show();
   bcd->activateWindow();
}
      
/**
 * slot for converting volume borders to fiducial borders.
 */
void 
GuiMainWindowLayersActions::slotBordersVolumeToBordersFiducial()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->copyVolumeBordersToFiducialBorders();
   theMainWindow->getBrainSet()->assignBorderColors();
            
   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   
   GuiBrainModelOpenGL::updateAllGL();
   
   QApplication::restoreOverrideCursor();
}
      
/**
 * slot for converting volume borders to fiducial cells.
 */
void 
GuiMainWindowLayersActions::slotBordersVolumeToFiducialCells()
{
   theMainWindow->getBrainSet()->convertVolumeBordersToFiducialCells();
   GuiFilesModified fm;
   fm.setCellModified();
   fm.setCellColorModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * slot for creating average borders.
 */
void 
GuiMainWindowLayersActions::slotBordersAverage()
{
   static GuiAverageBorderDialog* abd = NULL;
   if (abd == NULL) {
      abd = new GuiAverageBorderDialog(theMainWindow);
   }
   abd->show();
   abd->activateWindow();
}

/**
 * slot for deleting borders by name.
 */
void
GuiMainWindowLayersActions::slotDeleteBordersByName()
{
   GuiDeleteBordersByNameDialog dbn(theMainWindow);
   dbn.exec();
}

/**
 * slot for copy borders by name.
 */
void 
GuiMainWindowLayersActions::slotCopyBordersByName()
{
   //
   // Get names of all borders
   //
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   std::vector<QString> allNames;
   bmbs->getAllBorderNames(allNames, false);
   if (allNames.empty()) {
      QMessageBox::critical(theMainWindow, "ERROR", "There are no borders loaded.");
      return;
   }
   QStringList namesSL;
   for (unsigned int i = 0; i < allNames.size(); i++) {
      namesSL += allNames[i];
   }
   
   //
   // Create dialog for copying a border
   //
   WuQDataEntryDialog ded(theMainWindow);
   QComboBox* nameComboBox = ded.addComboBox("Name to Copy",
                                             namesSL);
   QLineEdit* newNameLineEdit = ded.addLineEditWidget("New Name");
   QObject::connect(nameComboBox, SIGNAL(activated(const QString&)),
                    newNameLineEdit, SLOT(setText(const QString&)));
   if (ded.exec() == WuQDataEntryDialog::Accepted) {
      const QString oldName = nameComboBox->currentText();
      const QString newName = newNameLineEdit->text().trimmed();
      if (newName.isEmpty()) {
         QMessageBox::critical(theMainWindow, "ERROR", "New Name is empty.");
         return;
      }
      if (oldName == newName) {
         QMessageBox::critical(theMainWindow, "ERROR",
                               "The old and new names are the same.");
         return;
      }
      std::vector<int> borderIndices;
      bmbs->getAllBordersWithName(oldName, borderIndices);
      const int numOldNames = static_cast<int>(borderIndices.size());
      if (numOldNames == 0) {
         QMessageBox::critical(theMainWindow, "ERROR", "No border name selected for copying.");
         return;
      }
      
      if (numOldNames > 1) {
         if (QMessageBox::question(theMainWindow, "Confirm",
               "There is more than one border named \""
               + oldName
               + ".  Do you want to copy all of them?"
               "  If no, you will need to delete the duplcates that already exist "
               "and then select this menu item.",
               (QMessageBox::Yes | QMessageBox::No),
               QMessageBox::Yes) == QMessageBox::No) {
            return;
         }
      }
      
      for (int i = 0; i < numOldNames; i++) {
         bmbs->copyBorder(borderIndices[i], newName);
      }
      
      theMainWindow->getBrainSet()->assignBorderColors();
               
      GuiFilesModified fm;
      fm.setBorderModified();
      theMainWindow->fileModificationUpdate(fm);
      
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
   }
}      

/**
 * slot for projecting borders
 */
void
GuiMainWindowLayersActions::slotBordersProject()
{
   GuiBorderProjectionDialog bpd(theMainWindow);
   bpd.exec();
}

/**
 * slot for create cartesian grid borders
 */
void
GuiMainWindowLayersActions::slotBordersCreateGrid()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      //
      // See if a flat surface is in the main window
      //
      BrainModelSurface::SURFACE_TYPES st = bms->getSurfaceType();
      if ((st != BrainModelSurface::SURFACE_TYPE_FLAT) &&
          (st != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
         QMessageBox::critical(theMainWindow, "Surface Type", 
                          "The surface in the main window must be flat for this operation !!!");
         return;
      }
 
      BorderFile borderFile;
      std::vector<QString> labels;
      labels.push_back("Grid Spacing (mm)");
      labels.push_back("Points per Grid Square");
      GuiMultipleInputDialog mid(theMainWindow, "Create Cartesian Grid Borders", labels);
      mid.setLineEdit(0, 50.0f);
      mid.setLineEdit(1, 5);
      if (mid.exec() == QDialog::Accepted) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         
         float  gridSpace;
         int pointsPerSquare;
         mid.getLineEdit(0, gridSpace);
         mid.getLineEdit(1, pointsPerSquare);
         bms->createFlatGridBorders(borderFile, gridSpace, pointsPerSquare);

         theMainWindow->getBrainSet()->deleteAllBorders();
         
         BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
         bmbs->copyBordersFromBorderFile(bms, &borderFile);
         theMainWindow->getBrainSet()->assignBorderColors();
                  
         GuiFilesModified fm;
         fm.setBorderModified();
         theMainWindow->fileModificationUpdate(fm);
         
         GuiBrainModelOpenGL::updateAllGL();
         
         QApplication::restoreOverrideCursor();
      }
   }
}

/**
 * slot for create cartesian grid borders
 */
void
GuiMainWindowLayersActions::slotBordersCreateAnalysisGrid()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      //
      // See if a flat surface is in the main window
      //
      BrainModelSurface::SURFACE_TYPES st = bms->getSurfaceType();
      if ((st != BrainModelSurface::SURFACE_TYPE_FLAT) &&
          (st != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
         if (QMessageBox::critical(theMainWindow, "Surface Type", 
                          "The surface in the main window must be flat for this operation.\n"
                          "Do you want to continue?",
                          QMessageBox::Yes | QMessageBox::No)
               == QMessageBox::No) {
            return;
         }
      }
 
      //
      // Get the bounds of the surface
      //
      float bounds[6];
      bms->getBounds(bounds);
      
      //
      // Create a dialog for the parameters
      //
      WuQDataEntryDialog ded(theMainWindow);
      ded.setWindowTitle("Create Analysis Grid Borders");
      QSpinBox* xMinSpinBox = ded.addSpinBox("X-Min",
                                                   bounds[0]);
      QSpinBox* xMaxSpinBox = ded.addSpinBox("X-Max",
                                                   bounds[1]);
      QSpinBox* yMinSpinBox = ded.addSpinBox("Y-Min",
                                                   bounds[2]);
      QSpinBox* yMaxSpinBox = ded.addSpinBox("Y-Max",
                                                   bounds[3]);
      QSpinBox* spacingSpinBox = ded.addSpinBox("Spacing",
                                                      10.0,
                                                      0.01,
                                                      1000000.0,
                                                      1.0);
      BorderFile borderFile;
      if (ded.exec() == QDialog::Accepted) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         
         bounds[0] = xMinSpinBox->value();
         bounds[1] = xMaxSpinBox->value();
         bounds[2] = yMinSpinBox->value();
         bounds[3] = yMaxSpinBox->value();
         const float spacing = spacingSpinBox->value();
         
         bms->createFlatGridBordersForAnalysis(borderFile, bounds, spacing);

         theMainWindow->getBrainSet()->deleteAllBorders();
         BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
         bmbs->copyBordersFromBorderFile(bms, &borderFile);
         theMainWindow->getBrainSet()->assignBorderColors();
                  
         DisplaySettingsBorders* dsb = 
            theMainWindow->getBrainSet()->getDisplaySettingsBorders();
         dsb->setDisplayBorders(true);

         GuiFilesModified fm;
         fm.setBorderModified();
         theMainWindow->fileModificationUpdate(fm);
         
         GuiBrainModelOpenGL::updateAllGL();
         
         QApplication::restoreOverrideCursor();
      }
   }
}

/**
 * slot for create spherical lat/lon borders
 */
void
GuiMainWindowLayersActions::slotBordersCreateSpherical()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      //
      // See if a spherical surface is in the main window
      //
      BrainModelSurface::SURFACE_TYPES st = bms->getSurfaceType();
      if (st != BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
         QMessageBox::critical(theMainWindow, "Surface Type", 
                         "The surface in the main window must be a sphere for this operation !!!");
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      BorderFile borderFile;
      bms->createSphericalLatLonBorders(borderFile, true);
      
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      bmbs->copyBordersFromBorderFile(bms, &borderFile);
      theMainWindow->getBrainSet()->assignBorderColors();
      
      GuiFilesModified fm;
      fm.setBorderModified();
      theMainWindow->fileModificationUpdate(fm);
      
      GuiBrainModelOpenGL::updateAllGL();
      
      QApplication::restoreOverrideCursor();
   }
}

/**
 * slot for edit border colors
 */
void
GuiMainWindowLayersActions::slotBordersEditColors()
{
   GuiColorFileEditorDialog* gfed = new GuiColorFileEditorDialog(theMainWindow, 
                                                                 theMainWindow->getBrainSet()->getBorderColorFile(),
                                                                 false,
                                                                 true, 
                                                                 true,
                                                                 false,
                                                                 false);
   QObject::connect(gfed, SIGNAL(redrawRequested()),
                    this, SLOT(slotBorderColorsChanged()));
   gfed->show();
}

/**
 * This slot is called by the color editor when user pressed apply or dialog is closed.
 */
void
GuiMainWindowLayersActions::slotBorderColorsChanged()
{
   GuiFilesModified fm;
   fm.setBorderColorModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * slot for resample displayed borders
 */
void
GuiMainWindowLayersActions::slotBordersResampleDisplayed()
{
   bool ok = false;
   const float sampling = QInputDialog::getDouble(theMainWindow, "Border Resampling", "Border Resampling", 2.0,
                                                  0.1, 1000.0, 1, &ok);
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      bmbs->resampleDisplayedBorders(bms, sampling);
      bmbs->assignColors();
   }
   else {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         bmbs->resampleDisplayedBorders(bmv, sampling);
         bmbs->assignColors();
      }
   }
   
   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   
   GuiBrainModelOpenGL::updateAllGL();

   QApplication::restoreOverrideCursor();
}

/**
 * slot for rename borders selected with mouse
 */
void
GuiMainWindowLayersActions::slotBordersRenameWithMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_RENAME);
}

/**
 * slot for reverse borders selected with mouse
 */
void
GuiMainWindowLayersActions::slotBordersReverseWithMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_REVERSE);
}

/**
 * slot for move borders selected with mouse
 */
void
GuiMainWindowLayersActions::slotBordersMovePointWithMouse()
{
   GuiBrainModelOpenGL* mainOpenGL = theMainWindow->getBrainModelOpenGL();
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      bool verify = false;
      switch (bms->getSurfaceType()) {
         case BrainModelSurface::SURFACE_TYPE_RAW:
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_FIDUCIAL: 
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_INFLATED:
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED: 
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_FLAT:
            verify = false;
            break;
         case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
            verify = false;
            break;
         case BrainModelSurface::SURFACE_TYPE_HULL:
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
            verify = true;
            break;
         case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
            verify = true;
            break;
      }
      if (verify) {
         const QString msg("Border points move in the XY plane which means that\n"
                           "border points may only be moved on flat surfaces.");
         if (QMessageBox::warning(theMainWindow, 
                                  "Danger", 
                                  msg, 
                                  (QMessageBox::Ok | QMessageBox::Cancel),
                                  QMessageBox::Cancel)
                                     == QMessageBox::Cancel) {
            return;
         }
      }
   }
   mainOpenGL->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_MOVE_POINT);   
}

/**
 * slot for reverse displayed borders
 */
void
GuiMainWindowLayersActions::slotBordersReverseDisplayed()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      bmbs->reverseDisplayedBorders(bms);
   }
   else {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         bmbs->reverseDisplayedBorders(bmv);
      }
   }
   
   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   
   GuiBrainModelOpenGL::updateAllGL();

   QApplication::restoreOverrideCursor();
}

/**
 * slot for orient displayed borders clockwise
 */
void
GuiMainWindowLayersActions::slotBordersOrientDisplayedClockwise()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      //
      // See if a flat surface is in the main window
      //
      const BrainModelSurface::SURFACE_TYPES st = bms->getSurfaceType();
      if ((st != BrainModelSurface::SURFACE_TYPE_FLAT) &&
          (st != BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR)) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "Surface Type", 
                         "The surface in the main window must be flat for this operation !!!");
         return;
      }
      
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      bmbs->orientDisplayedBordersClockwise(bms);
   }
   else {
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
         bmbs->orientDisplayedBordersClockwise(bmv);
      }
   }

   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   
   GuiBrainModelOpenGL::updateAllGL();

   QApplication::restoreOverrideCursor();
}

/**
 * slot for delete border point with mouse
 */
void
GuiMainWindowLayersActions::slotBordersDeletePointWithMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE_POINT);
}

/**
 * slot for delete border with mouse
 */
void
GuiMainWindowLayersActions::slotBordersDeleteWithMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_DELETE);
}

/**
 * slot for delete all borders
 */
void
GuiMainWindowLayersActions::slotBordersDeleteAll()
{
   if (QMessageBox::warning(theMainWindow, 
                            "Delete All Borders", 
                            "Are you sure you want to delete all borders?",
                            (QMessageBox::Yes | QMessageBox::No),
                            QMessageBox::Yes)
                               == QMessageBox::Yes) {
      theMainWindow->getBrainSet()->deleteAllBorders();
      GuiFilesModified fm;
      fm.setBorderModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();   
   }
}

/**
 * slot for deleting border points outside surface
 */
void
GuiMainWindowLayersActions::slotBordersDeletePointsOutsideSurface()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      const int brainModelIndex = theMainWindow->getBrainSet()->getBrainModelIndex(bms);
      if (brainModelIndex < 0) {
         std::cout << "PROGRAM ERROR: Invalid brain model index at " << __LINE__
                   << " in file " << __FILE__ << std::endl;
         return;
      }
      //
      // Create a point projector with barycentric mode on.
      //
      BrainModelSurfacePointProjector* pointProjector = 
                             new BrainModelSurfacePointProjector(bms,
                                     BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                                     false);
       
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
      const int numBorders = bmbs->getNumberOfBorders();
      for (int j = 0; j < numBorders; j++) {
         BrainModelBorder* b = bmbs->getBorder(j);
         if (b->getValidForBrainModel(brainModelIndex)) {
            const int numLinks = b->getNumberOfBorderLinks();
            for (int k = numLinks - 1; k >= 0; k--) {
               const BrainModelBorderLink* link = b->getBorderLink(k);
               int nearestNode;
               int tileNodes[3];
               float barycentric[3];
               
               //
               // If the border link does not project to a tile then it must be outside
               // the surface.
               //
               const int tileNumber = pointProjector->projectBarycentric(
                                          link->getLinkPosition(brainModelIndex),
                                          nearestNode, tileNodes, barycentric);
               if (tileNumber < 0) {
                  b->deleteBorderLink(k);
               }
            }
         }
      }
   }
   
   GuiFilesModified fm;
   fm.setBorderModified();
   theMainWindow->fileModificationUpdate(fm);
   
   GuiBrainModelOpenGL::updateAllGL();
   
   QApplication::restoreOverrideCursor();
}

/**
 * slot for edit border attributes dialog.
 */
void
GuiMainWindowLayersActions::slotBordersEditAttributes()
{
   GuiBorderAttributesDialog bad(theMainWindow);
   bad.exec();
}

/**
 * slot for clear border highlighting.
 */
void 
GuiMainWindowLayersActions::slotBordersClearHighlighting()
{
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   bmbs->clearBorderHighlighting();
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * slot for creating interpolated borders.
 */
void 
GuiMainWindowLayersActions::slotBordersCreateInterpolated()
{
   theMainWindow->getBordersCreateInterpolatedDialog(true);
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_BORDER_INTERPOLATE);   
}
      
/**
 * slot called to add contour cells.
 */
void 
GuiMainWindowLayersActions::slotContourCellsAdd()
{
   theMainWindow->getAddContourCellsDialog(true);
}

/**
 * slot called to delete contour cells with mouse.
 */
void 
GuiMainWindowLayersActions::slotContourCellsDeleteWithMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_DELETE);
}

/**
 * slot called to delete contour cells with mouse.
 */
void 
GuiMainWindowLayersActions::slotContourCellsMoveWithMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_CELL_MOVE);
}

/**
 * slot called to delete all contour cells.
 */
void 
GuiMainWindowLayersActions::slotContourCellsDeleteAll()
{
   if (QMessageBox::warning(theMainWindow, 
                            "Delete All Contour Cells", 
                            "Are you sure you want to delete all contour cells?",
                            (QMessageBox::Yes | QMessageBox::No),
                            QMessageBox::Yes)
                               == QMessageBox::Yes) {
      theMainWindow->getBrainSet()->clearContourCellFile();
      GuiFilesModified fm;
      fm.setContourModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();   
   }
}
      
/**
 * Slot for Edit Cell Colors Dialog
 */
void
GuiMainWindowLayersActions::slotContourCellsEditColors()
{
   GuiColorFileEditorDialog* gfed = new GuiColorFileEditorDialog(theMainWindow, 
                                                                 theMainWindow->getBrainSet()->getContourCellColorFile(),
                                                                 false,
                                                                 true, 
                                                                 false,
                                                                 false,
                                                                 false);
   QObject::connect(gfed, SIGNAL(redrawRequested()),
                    this, SLOT(slotContourCellsColorsChanged()));
   gfed->show();
}

/**
 * This slot is called by the color editor when user pressed apply or dialog is closed.
 */
void
GuiMainWindowLayersActions::slotContourCellsColorsChanged()
{
   GuiFilesModified fm;
   fm.setContourModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * slot for contour apply current view
 */
void
GuiMainWindowLayersActions::slotContourApplyCurrentView()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc != NULL) {
      bmc->applyTransformationsToAllContours();
      GuiFilesModified fm;
      fm.setContourModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * slot for contour new set.
 */
void 
GuiMainWindowLayersActions::slotContourNewSet()
{
   //
   // Create a new contour model and display it in the main window
   //
   BrainModelContours* bmc = new BrainModelContours(theMainWindow->getBrainSet());
   theMainWindow->getBrainSet()->addBrainModel(bmc);
   
   //
   // Notify about new contours
   //
   GuiFilesModified fm;
   fm.setContourModified();
   theMainWindow->fileModificationUpdate(fm);
   theMainWindow->displayBrainModelInMainWindow(theMainWindow->getBrainSet()->getBrainModelContours());

   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * slot for contour set scale.
 */
void 
GuiMainWindowLayersActions::slotContourSetScale()
{
   theMainWindow->getContourSetScaleDialog(true);
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_SET_SCALE);
}

/**
 * slot for contour select sections.
 */
void 
GuiMainWindowLayersActions::slotContourSections()
{
   //
   // Popup the contour section control dialog
   //
   theMainWindow->getContourSectionControlDialog(true);
}

/**
 * slot for contour set spacing.
 */
void 
GuiMainWindowLayersActions::slotContourSpacing()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   ContourFile* cf = bmc->getContourFile();
   
   bool ok = false;
   const float spacing = QInputDialog::getDouble(theMainWindow,
                                                 "Contour Section Spacing",
                                                 "Spacing (mm)",
                                                 0.1,
                                                 -214748367,
                                                 2147483647,
                                                 2,
                                                 &ok);
   if (ok) {
      cf->setSectionSpacing(spacing);
   }

   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * slot for contour draw.
 */
void 
GuiMainWindowLayersActions::slotContourDraw()
{
   theMainWindow->getContourDrawDialog(true);
}

/**
 * slot for contour align.
 */
void 
GuiMainWindowLayersActions::slotContourAlign()
{
   theMainWindow->getContourAlignmentDialog(true);
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                             GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_ALIGN);
}

/**
 * slot for contour merge.
 */
void 
GuiMainWindowLayersActions::slotContourMerge()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_MERGE);
}

/**
 * slot for contour move point.
 */
void 
GuiMainWindowLayersActions::slotContourMovePoint()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_MOVE);
}

/**
 * slot for contour delete all.
 */
void 
GuiMainWindowLayersActions::slotContourDeleteAll()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   if (QMessageBox::warning(theMainWindow, "Delete All Contours", 
                            "Are you sure you want to delete all contours?",
                            (QMessageBox::Yes | QMessageBox::No),
                            QMessageBox::Yes) 
                               == QMessageBox::Yes) {
      theMainWindow->getBrainSet()->clearContourFile();
      GuiFilesModified fm;
      fm.setContourModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL();   
   }
}

/**
 * slot for contour  delete point.
 */
void 
GuiMainWindowLayersActions::slotContourDeletePoint()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_POINT_DELETE);
}

/**
 * slot for contour delete contour.
 */
void 
GuiMainWindowLayersActions::slotContourCleanUp()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc == NULL) {
      return;
   }
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   ContourFile* cf = bmc->getContourFile();
   const bool problemsCleanedFlag = cf->cleanupContours();
   QString message("No problems were found during cleaning.");
   if (problemsCleanedFlag) {
      message = "Problems were found and cleaned.";
   }
   GuiFilesModified fm;
   fm.setContourModified();
   theMainWindow->fileModificationUpdate(fm);
   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
   QMessageBox::information(theMainWindow, "INFO", message);
}

/**
 * slot for contour point reversal.
 */
void 
GuiMainWindowLayersActions::slotContourReverse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_REVERSE);
}
      
/**
 * slot for contour delete contour.
 */
void 
GuiMainWindowLayersActions::slotContourDelete()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CONTOUR_DELETE);
}

/**
 * slot for contour resampling.
 */
void 
GuiMainWindowLayersActions::slotContourResample()
{
   bool ok = false;
   const float value = QInputDialog::getDouble(theMainWindow,
                                               "Resample Contours",
                                               "New Point Spacing",
                                               1.0,
                                               0.00001,
                                               2147483647.0,
                                               2,
                                               &ok);
   if (ok) {
      BrainModelContours* bmc = theMainWindow->getBrainModelContours();
      if (bmc != NULL) {
         ContourFile* cf = bmc->getContourFile();
         cf->resampleAllContours(value);
      }
   }
}
      
/**
 * slot for contour reconstruct into surface.
 */
void 
GuiMainWindowLayersActions::slotContourReconstruct()
{
   const QString msg = 
      "You should run \"Clean Up Contours\" prior to reconstruction which will remove\n"
      "invalid contours that may cause problems during reconstruction.  Clean Up Contours\n"
      "will remove consecutive, duplicate points from contours and will remove contours\n"
      "containing less than three points.\n\n"
      "In addition, you should show the contours as lines to verify that all contours are\n"
      "properly closed (no unusual lines cutting across a contour).  To show contours as\n"
      "lines, press the D/C pushbutton in the Main Window's Toolbar.  Set Page Selection to\n"
      "Contours and Contour Cells and set the Draw Mode to Points and Lines.  In addition,\n"
      "select Show First Point in Each Contour in Red to verify that each contour is a \n"
      "single contour and not composed of multiple contours segments.";
   QMessageBox msgBox(theMainWindow);
   msgBox.setWindowTitle("Clean Up Contours?");
   msgBox.setText(msg);
   msgBox.addButton("Proceed With Reconstruction", QMessageBox::AcceptRole);
   QPushButton* cancelPushButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
   msgBox.exec();
   if (msgBox.clickedButton() == cancelPushButton) {   
      return;
   }
   
   GuiContourReconstructionDialog crd(theMainWindow, true);
   crd.exec();
   theMainWindow->displayNewestSurfaceInMainWindow();
}

/**
 * update the actions (typically called when menu is about to show)
 */
void 
GuiMainWindowLayersActions::updateActions()
{
   //
   // See if a contour model is in the main window and if
   // there are any contours
   //
   bool haveContourModel = false;
   bool haveContours     = false;
   bool haveContourCells = false;
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc != NULL) {
      haveContourModel = true;
      ContourFile* cf = bmc->getContourFile();
      if (cf->getNumberOfContours() > 0) {
         haveContours = true;
         ContourCellFile* cf = theMainWindow->getBrainSet()->getContourCellFile();
         if (cf != NULL) {
            haveContourCells = (cf->getNumberOfCells() > 0);
         }
      }
   }
   
   //
   // Items that require contours or contour model
   //
   contourNewSetAction->setEnabled(haveContourModel == false);
   contourSetScaleAction->setEnabled(haveContourModel);
   contourSectionsAction->setEnabled(haveContours);
   contourSpacingAction->setEnabled(haveContourModel);
   contourDrawAction->setEnabled(haveContourModel);
   contourApplyCurrentViewAction->setEnabled(haveContours);
   contourAlignAction->setEnabled(haveContours);
   contourMergeAction->setEnabled(haveContours);
   contourMovePointAction->setEnabled(haveContours);
   contourDeleteAllAction->setEnabled(haveContours);
   contourDeleteAction->setEnabled(haveContours);
   contourDeletePointAction->setEnabled(haveContours);
   contourReconstructAction->setEnabled(haveContours);
   contourResampleAction->setEnabled(haveContours);
   contourReverseAction->setEnabled(haveContours);
   contourCleanUpAction->setEnabled(haveContours);
   contourCellsAddAction->setEnabled(haveContours);
   
   //
   // Items that require contour cells
   //
   contourCellsDeleteAllAction->setEnabled(haveContourCells);
   contourCellsDeleteWithMouseAction->setEnabled(haveContourCells);
   contourCellsMoveWithMouseAction->setEnabled(haveContourCells);

   const bool palsCompatibleFlag = (theMainWindow->getBrainSet()->getNumberOfNodes() == 73730);
   fociPalsProjectAction->setEnabled(palsCompatibleFlag);
   
   const FociProjectionFile* fpf = theMainWindow->getBrainSet()->getFociProjectionFile();
   fociStudyInfoToStudyMetaDataFileAction->setEnabled(fpf->getNumberOfStudyInfo() > 0);
}

/**
 * slot called when contour information selected.
 */
void 
GuiMainWindowLayersActions::slotContourInformation()
{
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc != NULL) {
      ContourFile* cf = bmc->getContourFile();
      const int numContours = cf->getNumberOfContours();
      
      QString msg;
      msg += ("Number of Contours: " 
              + QString::number(numContours)
              + "\n");
      
      if (numContours > 0) {
         msg += ("Section Number Range: "
                 + QString::number(cf->getMinimumSection())
                 + " to "
                 + QString::number(cf->getMaximumSection())
                 + "\n");
         
         msg += ("Section Spacing: "
                 + QString::number(cf->getSectionSpacing(), 'f', 3)
                 + "\n");
                 
         float xmin, xmax, ymin, ymax;
         cf->getExtent(xmin, xmax, ymin, ymax);
         msg += ("X-Range: "
                 + QString::number(xmin, 'f', 3)
                 + " to "
                 + QString::number(xmax, 'f', 3)
                 + "\n");
         msg += ("Y-Range: "
                 + QString::number(ymin, 'f', 3)
                 + " to "
                 + QString::number(ymax, 'f', 3)
                 + "\n");
         msg += ("Z-Range: "
                 + QString::number(cf->getSectionSpacing() * cf->getMinimumSection(), 'f', 3)
                 + " to "
                 + QString::number(cf->getSectionSpacing() * cf->getMaximumSection(), 'f', 3)
                 + "\n");
                 
         QMessageBox::information(theMainWindow, 
                                  "Contour Information",
                                  msg);
      }
   }
}
      


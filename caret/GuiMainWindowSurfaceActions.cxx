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

#include <QGlobalStatic>

#include <sstream>
#include <limits>

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QRadioButton>

#include "BorderFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelIdentification.h"
#include "BrainModelStandardSurfaceReplacement.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformationMeasurement.h"
#include "BrainModelSurfaceFlatHexagonalSubsample.h"
#include "BrainModelSurfaceMultiresolutionMorphing.h"
#include "BrainModelSurfaceNodeColoring.h"
#include "BrainModelSurfaceCutter.h"
#include "BrainModelSurfaceDeformationMeasurement.h"
#include "BrainModelSurfaceSphericalTessellator.h"
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainModelSurfaceTopologyCorrector.h"
#include "BrainSet.h"
#include "CellProjectionFile.h"
#include "CutsFile.h"
#include "DebugControl.h"
#include "DisplaySettingsCuts.h"
#include "DisplaySettingsSurface.h"
#include "FociProjectionFile.h"
#include "GuiAlignSurfaceToStandardOrientationDialog.h"
#include "GuiApplyDeformationMapDialog.h"
#include "GuiAverageCoordinateDialog.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiDistortionDialog.h"
#include "GuiFilesModified.h"
#include "GuiFlattenFullHemisphereDialog.h"
#include "GuiFlattenHemisphereDialog.h"
#include "GuiGenerateSulcalDepthDialog.h"
#include "GuiGenerateSurfaceCurvatureDialog.h"
#include "GuiIdentifyDialog.h"
#include "GuiInflateAndSmoothFingersDialog.h"
#include "GuiInflateSurfaceDialog.h"
#include "GuiInterpolateSurfacesDialog.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowSurfaceActions.h"
#include "GuiMainWindowSurfaceMenu.h"
#include "GuiMorphingDialog.h" 
#include "GuiMultiresolutionMorphingDialog.h"
#include "GuiNodeAttributeFileClearResetDialog.h"
#include "GuiPaintColumnNamesListBoxSelectionDialog.h"
#include "GuiResectionDialog.h"
#include "GuiSetTopologyDialog.h"
#include "GuiSetViewDialog.h"
#include "GuiShapeOrVectorsFromCoordinateSubtractionDialog.h"
#include "GuiSurfaceInformationDialog.h"
#include "GuiSurfaceDeformationDialog.h"
#include "GuiSurfaceSpmTransformDialog.h"
#include "GuiSurfaceToVolumeDialog.h"
#include "GuiSurfaceTopologyReportDialog.h"
#include "GuiUserViewSaveDialog.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ParamsFile.h"
#include "QtMultipleInputDialog.h"
#include "QtRadioButtonSelectionDialog.h"
#include "SectionFile.h"
#include "StringUtilities.h"
#include "TopologyHelper.h"
#include "TransformationMatrixFile.h"
#include "WuQDataEntryDialog.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiMainWindowSurfaceActions::GuiMainWindowSurfaceActions(GuiMainWindow* parent) :
   QObject(parent)
{
   setObjectName("GuiMainWindowSurfaceActions");
   
   flatMultiResMorphObject = new BrainModelSurfaceMultiresolutionMorphing(NULL, NULL, NULL,
                                      BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT);
   sphericalMultiResMorphObject = new BrainModelSurfaceMultiresolutionMorphing(NULL, NULL, NULL,
                                      BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL);
                                      
   flatMorphDialog   = NULL;
   sphereMorphDialog = NULL;
   
   scaleDialog     = NULL;
   translateDialog = NULL;
   
   alignSurfacesToStandardOrientationAction = new QAction(parent);
   alignSurfacesToStandardOrientationAction->setObjectName("alignSurfacesToStandardOrientationAction");
   alignSurfacesToStandardOrientationAction->setText("Align Surface(s) to Standard Orientation...");
   QObject::connect(alignSurfacesToStandardOrientationAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotAlignSurfacesToStandardOrientation()));
   
   averageCoordinateFileAction = new QAction(parent);
   averageCoordinateFileAction->setObjectName("averageCoordinateFileAction");
   averageCoordinateFileAction->setText("Create Average Coordinate File...");
   QObject::connect(averageCoordinateFileAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotAverageCoordinateFile()));
   
   automaticRotationAction = new QAction(parent);
   automaticRotationAction->setObjectName("automaticRotationAction");
   automaticRotationAction->setText("Automatic Rotation...");
   QObject::connect(automaticRotationAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayAutomaticRotationDialog()));
   
   interpolateSurfacesAction = new QAction(parent);
   interpolateSurfacesAction->setObjectName("interpolateSurfacesAction");
   interpolateSurfacesAction->setText("Interpolate Surfaces...");
   QObject::connect(interpolateSurfacesAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotInterpolateSurfaces()));
   
   copyMainWindowSurfaceAction = new QAction(parent);
   copyMainWindowSurfaceAction->setObjectName("copyMainWindowSurfaceAction");
   copyMainWindowSurfaceAction->setText("Copy Main Window Surface to New Surface");
   QObject::connect(copyMainWindowSurfaceAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCopyMainWindowSurface()));
   
   simplifySurfaceAction = new QAction(parent);
   simplifySurfaceAction->setObjectName("simplifySurfaceAction");
   simplifySurfaceAction->setText("Simplify Surface (Fewer Polygons)...");
   QObject::connect(simplifySurfaceAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotSimplifySurface()));
   
   cutsShowAction = new QAction(parent);
   cutsShowAction->setObjectName("cutsShowAction");
   cutsShowAction->setText("Show Cuts");
   QObject::connect(cutsShowAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCutsShow()));
   
   cutsApplyAction = new QAction(parent);
   cutsApplyAction->setObjectName("cutsApplyAction");
   cutsApplyAction->setText("Apply All Cuts");
   QObject::connect(cutsApplyAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCutsApply()));
   
   cutsDeleteWithMouseAction = new QAction(parent);
   cutsDeleteWithMouseAction->setObjectName("cutsDeleteWithMouseAction");
   cutsDeleteWithMouseAction->setText("Delete Cut With Mouse");
   QObject::connect(cutsDeleteWithMouseAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCutsDeleteWithMouse()));
   
   cutsDeleteAllAction = new QAction(parent);
   cutsDeleteAllAction->setObjectName("cutsDeleteAllAction");
   cutsDeleteAllAction->setText("Delete All Cuts");
   QObject::connect(cutsDeleteAllAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCutsDeleteAll()));
   
   cutsDrawAction = new QAction(parent);
   cutsDrawAction->setObjectName("cutsDrawAction");
   cutsDrawAction->setText("Draw Cuts");
   QObject::connect(cutsDrawAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotCutsDraw()));
   
   deformationRunFlatDialogAction = new QAction(parent);
   deformationRunFlatDialogAction->setObjectName("deformationRunFlatDialogAction");
   deformationRunFlatDialogAction->setText("Run Flat Surface Deformation...");
   QObject::connect(deformationRunFlatDialogAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotDeformationRunFlatDialog()));
   
   deformationRunSphericalDialogAction = new QAction(parent);
   deformationRunSphericalDialogAction->setObjectName("deformationRunSphericalDialogAction");
   deformationRunSphericalDialogAction->setText("Run Spherical Surface Deformation...");
   QObject::connect(deformationRunSphericalDialogAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotDeformationRunSphericalDialog()));
   
   deformationApplyDialogAction = new QAction(parent);
   deformationApplyDialogAction->setObjectName("deformationApplyDialogAction");
   deformationApplyDialogAction->setText("Apply Deformation Map...");
   QObject::connect(deformationApplyDialogAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotDeformationApplyDialog()));
   
   standardMeshDialogAction = new QAction(parent);
   standardMeshDialogAction->setObjectName("standardMeshDialogAction");
   standardMeshDialogAction->setText("Convert to Standard Mesh (In Developement!)...");
   QObject::connect(standardMeshDialogAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayStandardMeshDialog()));
   
   flattenHemisphereAction = new QAction(parent);
   flattenHemisphereAction->setObjectName("flattenHemisphereAction");
   flattenHemisphereAction->setText("Flatten Full or Partial Hemisphere...");
   QObject::connect(flattenHemisphereAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotFlattenHemisphere()));
   
   flattenFullHemisphereAction = new QAction(parent);
   flattenFullHemisphereAction->setObjectName("flattenFullHemisphereAction");
   flattenFullHemisphereAction->setText("Flatten Full Hemisphere (In Development)...");
   QObject::connect(flattenFullHemisphereAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displayFlattenFullHemisphereDialog()));
   
   measurementsCrossoverCheckAction = new QAction(parent);
   measurementsCrossoverCheckAction->setObjectName("measurementsCrossoverCheckAction");
   measurementsCrossoverCheckAction->setText("Crossover Check");
   QObject::connect(measurementsCrossoverCheckAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotMeasurementsCrossoverCheck()));
   
   measurementsCurvatureAction = new QAction(parent);
   measurementsCurvatureAction->setObjectName("measurementsCurvatureAction");
   measurementsCurvatureAction->setText("Generate Curvature...");
   QObject::connect(measurementsCurvatureAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotMeasurementsCurvature()));
   
   measurementsDistortionAction = new QAction(parent);
   measurementsDistortionAction->setObjectName("measurementsDistortionAction");
   measurementsDistortionAction->setText("Generate Distortion...");
   QObject::connect(measurementsDistortionAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotMeasurementsDistortion()));
   
   measurementsSulcalDepthAction = new QAction(parent);
   measurementsSulcalDepthAction->setObjectName("measurementsSulcalDepthAction");
   measurementsSulcalDepthAction->setText("Generate Sulcal Depth...");
   QObject::connect(measurementsSulcalDepthAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotMeasurementsSulcalDepth()));
   
   measurementsShapeFromCoordinateSubtraction = new QAction(parent);
   measurementsShapeFromCoordinateSubtraction->setObjectName("measurementsShapeFromCoordinateSubtraction");
   measurementsShapeFromCoordinateSubtraction->setText("Generate Distance: Surface A to Surface B...");
   QObject::connect(measurementsShapeFromCoordinateSubtraction, SIGNAL(triggered(bool)),
                    this, SLOT(slotMeasurementsShapeFromCoordinateSubtraction()));

   normalsGenerateAction = new QAction(parent);
   normalsGenerateAction->setObjectName("normalsGenerateAction");
   normalsGenerateAction->setText("Generate Normals");
   QObject::connect(normalsGenerateAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNormalsGenerate()));
   
   normalsFlipAction = new QAction(parent);
   normalsFlipAction->setObjectName("normalsFlipAction");
   normalsFlipAction->setText("Flip Normals");
   QObject::connect(normalsFlipAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotNormalsFlip()));
   
   regionOfInterestAction = new QAction(parent);
   regionOfInterestAction->setObjectName("regionOfInterestAction");
   regionOfInterestAction->setText("Region Of Interest Operations...");
   QObject::connect(regionOfInterestAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotRegionOfInterest()));
   
   regionOfInterestActionOLD = new QAction(parent);
   regionOfInterestActionOLD->setObjectName("regionOfInterestAction");
   regionOfInterestActionOLD->setText("Region Of Interest Operations (OLD)...");
   QObject::connect(regionOfInterestActionOLD, SIGNAL(triggered(bool)),
                    this, SLOT(slotRegionOfInterestOLD()));
   
   surfaceInformationAction = new QAction(parent);
   surfaceInformationAction->setObjectName("surfaceInformationAction");
   surfaceInformationAction->setText("Information...");
   QObject::connect(surfaceInformationAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotSurfaceInformation()));
   
   transformSpmAction = new QAction(parent);
   transformSpmAction->setObjectName("transformSpmAction");
   transformSpmAction->setText("Apply SPM Inverse Transform...");
   QObject::connect(transformSpmAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTransformSpm()));
   
   transformCenterOfMassAction = new QAction(parent);
   transformCenterOfMassAction->setObjectName("transformCenterOfMassAction");
   transformCenterOfMassAction->setText("Translate to Center of Mass");
   QObject::connect(transformCenterOfMassAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTransformCenterOfMass()));
   
   transformSubtractACAction = new QAction(parent);
   transformSubtractACAction->setObjectName("transformSubtractACAction");
   transformSubtractACAction->setText("Translate - Subtract Params File AC");
   QObject::connect(transformSubtractACAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTransformSubtractAC()));
   
   transformDisconnectedNodesToOriginAction = new QAction(parent);
   transformDisconnectedNodesToOriginAction->setObjectName("transformDisconnectedNodesToOriginAction");
   transformDisconnectedNodesToOriginAction->setText("Move Disconnected Nodes to Origin");
   QObject::connect(transformDisconnectedNodesToOriginAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTransformDisconnectedNodesToOrigin()));
   
   transformApplyCurrentViewAction = new QAction(parent);
   transformApplyCurrentViewAction->setObjectName("transformApplyCurrentViewAction");
   transformApplyCurrentViewAction->setText("Apply Current View...");
   QObject::connect(transformApplyCurrentViewAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTransformApplyCurrentView()));
   
   transformTranslateAction = new QAction(parent);
   transformTranslateAction->setObjectName("transformTranslateAction");
   transformTranslateAction->setText("Translate...");
   QObject::connect(transformTranslateAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTransformTranslate()));
   
   transformScaleAction = new QAction(parent);
   transformScaleAction->setObjectName("transformScaleAction");
   transformScaleAction->setText("Scale...");
   QObject::connect(transformScaleAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTransformScale()));
   
   morphFlatAction = new QAction(parent);
   morphFlatAction->setObjectName("morphFlatAction");
   morphFlatAction->setText("Morph Flat Surface...");
   QObject::connect(morphFlatAction, SIGNAL(triggered(bool)),
                    parent, SLOT(showFlatMorphingDialog()));
   
   morphSphereAction = new QAction(parent);
   morphSphereAction->setObjectName("morphSphereAction");
   morphSphereAction->setText("Morph Spherical Surface...");
   QObject::connect(morphSphereAction, SIGNAL(triggered(bool)),
                    parent, SLOT(showSphereMorphingDialog()));
   
   multiresolutionMorphFlatAction = new QAction(parent);
   multiresolutionMorphFlatAction->setObjectName("multiresolutionMorphFlatAction");
   multiresolutionMorphFlatAction->setText("Multiresolution Morphing Flat Surface...");
   QObject::connect(multiresolutionMorphFlatAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotMultiresolutionMorphFlat()));
   
   multiresolutionMorphSphereAction = new QAction(parent);
   multiresolutionMorphSphereAction->setObjectName("multiresolutionMorphSphereAction");
   multiresolutionMorphSphereAction->setText("Multiresolution Morphing Spherical Surface...");
   QObject::connect(multiresolutionMorphSphereAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotMultiresolutionMorphSphere()));
   
   topologyDeleteCornersAction = new QAction(parent);
   topologyDeleteCornersAction->setObjectName("topologyDeleteCornersAction");
   topologyDeleteCornersAction->setText("Remove Corner and Straggler Tiles...");
   QObject::connect(topologyDeleteCornersAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyDeleteCorners()));
   
   topologySetAction = new QAction(parent);
   topologySetAction->setObjectName("topologySetAction");
   topologySetAction->setText("Set Topology Assigned to Surfaces...");
   QObject::connect(topologySetAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displaySetTopologyDialog()));
   
   topologyCheckForIslandsAction = new QAction(parent);
   topologyCheckForIslandsAction->setObjectName("topologyCheckForIslandsAction");
   topologyCheckForIslandsAction->setText("Check For Islands");
   QObject::connect(topologyCheckForIslandsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyCheckForIslands()));
   
   topologyDisconnectIslandsAction = new QAction(parent);
   topologyDisconnectIslandsAction->setObjectName("topologyDisconnectIslandsAction");
   topologyDisconnectIslandsAction->setText("Remove Islands");
   QObject::connect(topologyDisconnectIslandsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyDisconnectIslands()));
   
   topologyClassifyEdgesAction = new QAction(parent);
   topologyClassifyEdgesAction->setObjectName("topologyClassifyEdgesAction");
   topologyClassifyEdgesAction->setText("Classify Nodes");
   QObject::connect(topologyClassifyEdgesAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyClassifyEdges()));
   
   topologyOrientTilesConsistentlyAction = new QAction(parent);
   topologyOrientTilesConsistentlyAction->setObjectName("topologyOrientTilesConsistentlyAction");
   topologyOrientTilesConsistentlyAction->setText("Orient Tiles Consistently");
   QObject::connect(topologyOrientTilesConsistentlyAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyOrientTilesConsistently()));
   
   topologyReplaceWithStandardSurfaceAction = new QAction(parent);
   topologyReplaceWithStandardSurfaceAction->setObjectName("topologyReplaceWithStandardSurfaceAction");
   topologyReplaceWithStandardSurfaceAction->setText("Replace Surfaces With Standard Surface");
   QObject::connect(topologyReplaceWithStandardSurfaceAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyReplaceWithStandardSurface()));
   
   topologyCorrectFiducialAction = new QAction(parent);
   topologyCorrectFiducialAction->setObjectName("topologyCorrectFiducialAction");
   topologyCorrectFiducialAction->setText("Correct Fiducial Surface Topology (Remove Handles)");
   QObject::connect(topologyCorrectFiducialAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyCorrectFiducial()));
   
   topologyRetessellateSphereAction = new QAction(parent);
   topologyRetessellateSphereAction->setObjectName("topologyRetessellateSphereAction");
   topologyRetessellateSphereAction->setText("Retessellate Sphere");
   QObject::connect(topologyRetessellateSphereAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyRetessellateSphere()));
   
   topologyReportAction = new QAction(parent);
   topologyReportAction->setObjectName("topologyReportAction");
   topologyReportAction->setText("Topology Error Report...");
   QObject::connect(topologyReportAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotTopologyReport()));
   
   identifyHighlightTileAction = new QAction(parent);
   identifyHighlightTileAction->setObjectName("identifyHighlightTileAction");
   identifyHighlightTileAction->setText("Identify Tile By Number...");
   QObject::connect(identifyHighlightTileAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotIdentifyHighlightTile()));
   
   identifyHighlightNodeAction = new QAction(parent);
   identifyHighlightNodeAction->setObjectName("identifyHighlightNodeAction");
   identifyHighlightNodeAction->setText("Identify Node By Number...");
   QObject::connect(identifyHighlightNodeAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotIdentifyHighlightNode()));
   
   identifyClearSymbolsAction = new QAction(parent);
   identifyClearSymbolsAction->setObjectName("identifyClearSymbolsAction");
   identifyClearSymbolsAction->setText("Clear Node Identify Symbols");
   QObject::connect(identifyClearSymbolsAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotIdentifyClearSymbols()));
   
   sectionControlAction = new QAction(parent);
   sectionControlAction->setObjectName("sectionControlAction");
   sectionControlAction->setText("Section Control...");
   QObject::connect(sectionControlAction, SIGNAL(triggered(bool)),
                    parent, SLOT(displaySectionControlDialog()));
   
   
   sectionClearAllOrPartAction = new QAction(parent);
   sectionClearAllOrPartAction->setObjectName("sectionClearAllOrPartAction");
   sectionClearAllOrPartAction->setText("Clear All or Part of Section File...");
   QObject::connect(sectionClearAllOrPartAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotSectionClearAllOrPart()));
   
   sectionResectionAction = new QAction(parent);
   sectionResectionAction->setObjectName("sectionResectionAction");
   sectionResectionAction->setText("Resection...");
   QObject::connect(sectionResectionAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotSectionResection()));
   
   geometrySphereBorderDistanceAction = new QAction(parent);
   geometrySphereBorderDistanceAction->setObjectName("geometrySphereBorderDistanceAction");
   geometrySphereBorderDistanceAction->setText("Nearest Spherical Border Distance");
   QObject::connect(geometrySphereBorderDistanceAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometrySphereBorderDistance()));
   
   geometryReliefAction = new QAction(parent);
   geometryReliefAction->setObjectName("geometryReliefAction");
   geometryReliefAction->setText("Add Relief to Sphere/Flat");
   QObject::connect(geometryReliefAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryRelief()));
   
   geometryCompressMedialWallAction = new QAction(parent);
   geometryCompressMedialWallAction->setObjectName("geometryCompressMedialWallAction");
   geometryCompressMedialWallAction->setText("Compress Sphere Medial Wall...");
   QObject::connect(geometryCompressMedialWallAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryCompressMedialWall()));
   
   geometryCompressFrontFaceAction = new QAction(parent);
   geometryCompressFrontFaceAction->setObjectName("geometryCompressFrontFaceAction");
   geometryCompressFrontFaceAction->setText("Compress Front Face...");
   QObject::connect(geometryCompressFrontFaceAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryCompressFrontFace()));
   
   geometryToSphereAction = new QAction(parent);
   geometryToSphereAction->setObjectName("geometryToSphereAction");
   geometryToSphereAction->setText("Convert to Sphere");
   QObject::connect(geometryToSphereAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryToSphere()));
   
   geometryToEllipsoidAction = new QAction(parent);
   geometryToEllipsoidAction->setObjectName("geometryToEllipsoidAction");
   geometryToEllipsoidAction->setText("Convert to Ellipsoid");
   QObject::connect(geometryToEllipsoidAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryToEllipsoid()));
   
   geometryEllipseToSphereAction = new QAction(parent);
   geometryEllipseToSphereAction->setObjectName("geometryEllipseToSphereAction");
   geometryEllipseToSphereAction->setText("Convert Ellipsoid to Sphere");
   QObject::connect(geometryEllipseToSphereAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryEllipseToSphere()));
   
   geometryInflateAction = new QAction(parent);
   geometryInflateAction->setObjectName("geometryInflateAction");
   geometryInflateAction->setText("Inflate Surface...");
   QObject::connect(geometryInflateAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryInflate()));
   
   geometryExpandAction = new QAction(parent);
   geometryExpandAction->setObjectName("geometryExpandAction");
   geometryExpandAction->setText("Expand or Shrink Surface...");
   QObject::connect(geometryExpandAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryExpand()));
   
   geometryInflateAndSmoothFingersAction = new QAction(parent);
   geometryInflateAndSmoothFingersAction->setObjectName("geometryInflateAndSmoothFingersAction");
   geometryInflateAndSmoothFingersAction->setText("Inflate Surface And Smooth Fingers...");
   QObject::connect(geometryInflateAndSmoothFingersAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryInflateAndSmoothFingers()));
   
   geometrySphereToFlatAction = new QAction(parent);
   geometrySphereToFlatAction->setObjectName("geometrySphereToFlatAction");
   geometrySphereToFlatAction->setText("Project Sphere to Flat");
   QObject::connect(geometrySphereToFlatAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometrySphereToFlat()));
   
   geometrySphereToFlatThroughHoleAction = new QAction(parent);
   geometrySphereToFlatThroughHoleAction->setObjectName("geometrySphereToFlatThroughHoleAction");
   geometrySphereToFlatThroughHoleAction->setText("Project Sphere to Flat Through Hole...");
   QObject::connect(geometrySphereToFlatThroughHoleAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometrySphereToFlatThroughHole()));
   
   geometryScaleSphereToFiducialAreaAction = new QAction(parent);
   geometryScaleSphereToFiducialAreaAction->setObjectName("geometryScaleSphereToFiducialAreaAction");
   geometryScaleSphereToFiducialAreaAction->setText("Scale Sphere to Fiducial Surface Area");
   QObject::connect(geometryScaleSphereToFiducialAreaAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryScaleSphereToFiducialArea()));
   
   geometrySmoothingAction = new QAction(parent);
   geometrySmoothingAction->setObjectName("geometrySmoothingAction");
   geometrySmoothingAction->setText("Smoothing...");
   QObject::connect(geometrySmoothingAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometrySmoothing()));
   
   geometryInflatedAndEllipsoidFromFiducialAction = new QAction(parent);
   geometryInflatedAndEllipsoidFromFiducialAction->setObjectName("geometryInflatedAndEllipsoidFromFiducialAction");
   geometryInflatedAndEllipsoidFromFiducialAction->setText("Generate Inflated and Ellipsoid From Fiducial...");
   QObject::connect(geometryInflatedAndEllipsoidFromFiducialAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotGeometryInflatedAndEllipsoidFromFiducial()));
   
   projectToPlaneAction = new QAction(parent);
   projectToPlaneAction->setObjectName("projectToPlaneAction");
   projectToPlaneAction->setText("Project to Plane...");
   QObject::connect(projectToPlaneAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotProjectToPlane()));
   
   editAddNodeAction = new QAction(parent);
   editAddNodeAction->setObjectName("editAddNodeAction");
   editAddNodeAction->setText("Add Node");
   QObject::connect(editAddNodeAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotEditAddNode()));
   
   editAddTileAction = new QAction(parent);
   editAddTileAction->setObjectName("editAddTileAction");
   editAddTileAction->setText("Add Tile");
   QObject::connect(editAddTileAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotEditAddTile()));
   
   editDeleteTileByLinkAction = new QAction(parent);
   editDeleteTileByLinkAction->setObjectName("editDeleteTileByLinkAction");
   editDeleteTileByLinkAction->setText("Delete Link");
   QObject::connect(editDeleteTileByLinkAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotEditDeleteTileByLink()));
   
   editDisconnectNodeAction = new QAction(parent);
   editDisconnectNodeAction->setObjectName("editDisconnectNodeAction");
   editDisconnectNodeAction->setText("Disconnect Node");
   QObject::connect(editDisconnectNodeAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotEditDisconnectNode()));
   
   editMoveNodeAction = new QAction(parent);
   editMoveNodeAction->setObjectName("editMoveNodeAction");
   editMoveNodeAction->setText("Move Node");
   QObject::connect(editMoveNodeAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotEditMoveNode()));
                    
   viewAdjustAction = new QAction(parent);
   viewAdjustAction->setObjectName("viewAdjustAction");
   viewAdjustAction->setText("Adjust View...");
   QObject::connect(viewAdjustAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotViewAdjust()));
                    
   viewPlaceNodeAtCenterOfScreenAction = new QAction(parent);
   viewPlaceNodeAtCenterOfScreenAction->setObjectName("viewPlaceNodeAtCenterOfScreenAction");
   viewPlaceNodeAtCenterOfScreenAction->setText("Place Node at Center of Screen...");
   QObject::connect(viewPlaceNodeAtCenterOfScreenAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotViewPlaceNodeAtCenterOfScreen()));
                    
   viewScaleSurfaceToFitWindowAction = new QAction(parent);
   viewScaleSurfaceToFitWindowAction->setObjectName("viewScaleSurfaceToFitWindowAction");
   viewScaleSurfaceToFitWindowAction->setText("Scale Surface to Fit Window");
   QObject::connect(viewScaleSurfaceToFitWindowAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotViewScaleSurfaceToFitWindow()));
                    
   viewUserSaveViewAction = new QAction(parent);
   viewUserSaveViewAction->setObjectName("viewUserSaveViewAction");
   viewUserSaveViewAction->setText("Save User View...");
   QObject::connect(viewUserSaveViewAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotUserViewSaveView()));
                    
   surfaceToSegmentationVolumeAction = new QAction(parent);
   surfaceToSegmentationVolumeAction->setObjectName("surfaceToSegmentationVolumeAction");
   surfaceToSegmentationVolumeAction->setText("Convert Main Window Surface To Segmentation Volume...");
   QObject::connect(surfaceToSegmentationVolumeAction, SIGNAL(triggered(bool)),
                    this, SLOT(slotSurfaceToSegmentationVolume()));                    
}

/**
 * Destructor.
 */
GuiMainWindowSurfaceActions::~GuiMainWindowSurfaceActions()
{
   delete flatMultiResMorphObject;
   delete sphericalMultiResMorphObject;
}

/**
 * called to convert surface to a segmentation volume.
 */
void 
GuiMainWindowSurfaceActions::slotSurfaceToSegmentationVolume()
{
   GuiSurfaceToVolumeDialog svd(theMainWindow, GuiSurfaceToVolumeDialog::DIALOG_MODE_SEGMENT_VOLUME,
                                "Convert Surface to Segmentation Volume");
   if (svd.exec() == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      float offset[3];
      float voxelSize[3];
      float origin[3];
      int dim[3];
      svd.getSurfaceOffset(offset);
      svd.getVolumeDimensions(dim);
      svd.getVolumeVoxelSizes(voxelSize);
      svd.getVolumeOrigin(origin);
      BrainModelSurfaceToVolumeConverter stv(theMainWindow->getBrainSet(),
                                             svd.getSelectedSurface(),
                                             svd.getStandardVolumeSpace(),
                                             offset,
                                             dim,
                                             voxelSize,
                                             origin,
                                             svd.getSurfaceInnerBoundary(),
                                             svd.getSurfaceOuterBoundary(),
                                             svd.getSurfaceThicknessStep(),
                                             BrainModelSurfaceToVolumeConverter::CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES);
      stv.setNodeToVoxelMappingEnabled(svd.getNodeToVoxelMappingEnabled(),
                                       svd.getNodeToVoxelMappingFileName());

      try {
         stv.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "Error", e.whatQString());
         return;
      }

      theMainWindow->speakText("The metric data has been converted to a volume.", false);

      GuiBrainModelOpenGL* openGL = theMainWindow->getBrainModelOpenGL();
      openGL->displayBrainModelVolume();
      GuiFilesModified fm;
      fm.setVolumeModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Slot called to simplify the surface.
 */
void 
GuiMainWindowSurfaceActions::slotSimplifySurface()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms == NULL) {
      QMessageBox::critical(theMainWindow, "ERROR", "There is no surface in the main window.");
      return;
   }
   
   if (theMainWindow->getBrainSet()->getNumberOfBrainModels() > 1) {
      if (QMessageBox::question(theMainWindow, "Continue",
              "Contours/surfaces/volumes other than the surface in the main\n"
              "window are loaded.  They will be discarded if you continue.", 
              (QMessageBox::Ok | QMessageBox::Cancel),
              QMessageBox::Ok)
                 == QMessageBox::Ok) {
         return;
      }
   }
 
   QString msg;
   msg.append("Current number of polygons: ");
   msg.append(QString::number(bms->getTopologyFile()->getNumberOfTiles()));
   msg.append(".\n\nEnter Maximum Number of Polygons.\n");
   bool ok = false;
   const int maxPolygons = QInputDialog::getInteger(theMainWindow,
                                                    "Simplify Surface",
                                                    msg,
                                                    150000,
                                                    1,
                                                    1000000000,
                                                    1000,
                                                    &ok);
   if (ok) {
      try {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         theMainWindow->getBrainSet()->simplifySurface(bms, maxPolygons);
         QApplication::beep();
         QApplication::restoreOverrideCursor();
      }
      catch (FileException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
   }
}      

/**
 * Called to create an average coordinate file
 */
void
GuiMainWindowSurfaceActions::slotAverageCoordinateFile()
{
   static GuiAverageCoordinateDialog* acd = NULL;
   if (acd == NULL) {
      acd = new GuiAverageCoordinateDialog(theMainWindow);
   }
   acd->show();
   acd->activateWindow();
}

/**
 * called to copy the surface in the main window.
 */
void 
GuiMainWindowSurfaceActions::slotCopyMainWindowSurface()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      QString name("copy_of_");
      name.append(bms->getFileName());
      
      BrainModelSurface* bms2 = new BrainModelSurface(*bms);
      CoordinateFile* cf = bms2->getCoordinateFile();
      cf->setFileName(name);
      theMainWindow->getBrainSet()->addBrainModel(bms2);
      GuiFilesModified fm;
      fm.setCoordinateModified();
      theMainWindow->fileModificationUpdate(fm);
      theMainWindow->displayNewestSurfaceInMainWindow();
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
}
      
/**
 * Called to interpolate surfaces.
 */
void
GuiMainWindowSurfaceActions::slotInterpolateSurfaces()
{
   theMainWindow->getInterpolateSurfaceDialog(true);
}

/**
 * Called when align surfaces to standard orientation is selected.
 */
void
GuiMainWindowSurfaceActions::slotAlignSurfacesToStandardOrientation()
{
   GuiAlignSurfaceToStandardOrientationDialog* assod = 
      theMainWindow->getAlignSurfaceToStandardOrientationDialog();
   
   if (assod == NULL) { 
      assod = new GuiAlignSurfaceToStandardOrientationDialog(theMainWindow);
   }
   assod->show();
   assod->activateWindow();
}

/**
 * Display generate surface curvature dialog.
 */
void
GuiMainWindowSurfaceActions::slotMeasurementsCurvature()
{
   GuiGenerateSurfaceCurvatureDialog gscd(theMainWindow);
   gscd.exec();
}

/**
 * Display generate sulcal depth dialog.
 */
void
GuiMainWindowSurfaceActions::slotMeasurementsSulcalDepth()
{
   GuiGenerateSulcalDepthDialog gsdd(theMainWindow);
   gsdd.exec();
}

/**
 * Generate shape from coordinate difference.
 */
void 
GuiMainWindowSurfaceActions::slotMeasurementsShapeFromCoordinateSubtraction()
{
   GuiShapeOrVectorsFromCoordinateSubtractionDialog csd(theMainWindow,
                       GuiShapeOrVectorsFromCoordinateSubtractionDialog::MODE_SHAPE);
   csd.exec();
}

/**
 * Display flatten full hemisphere dialog.
 */
void
GuiMainWindowSurfaceActions::slotFlattenHemisphere()
{
   GuiFlattenHemisphereDialog gfhd(theMainWindow);
   gfhd.exec();
}

/**
 * Called to perform a crossover check
 */
void
GuiMainWindowSurfaceActions::slotMeasurementsCrossoverCheck()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      int defaultItem = 2;
      switch (bms->getSurfaceType()) {
         case BrainModelSurface::SURFACE_TYPE_RAW:
            defaultItem = 2;
            break;
         case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
            defaultItem = 2;
            break;
         case BrainModelSurface::SURFACE_TYPE_INFLATED:
            defaultItem = 2;
            break;
         case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
            defaultItem = 2;
            break;
         case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
            defaultItem = 1;
            break;
         case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
            defaultItem = 1;
            break;
         case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
            defaultItem = 2;
            break;
         case BrainModelSurface::SURFACE_TYPE_FLAT:
            defaultItem = 0;
            break;
         case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
            defaultItem = 0;
            break;
         case BrainModelSurface::SURFACE_TYPE_HULL:
            defaultItem = 2;
            break;
         case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
            defaultItem = 2;
            break;
         case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
            defaultItem = 2;
            break;
      }
      
      std::vector<QString> labels;
      labels.push_back("Flat Surface");
      labels.push_back("Spherical Surface");
      labels.push_back("Other Type of Surface");
      if (DebugControl::getTestFlag1()) {
         labels.push_back("SureFit Ellipsoid Crossover Check");
      }
      QApplication::beep();
      QtRadioButtonSelectionDialog rd(theMainWindow,
                                      "Crossover Check",
                                      "",
                                      labels,
                                      defaultItem);
      if (rd.exec() == QDialog::Accepted) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
         BrainModelSurface::SURFACE_TYPES surfaceType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
         switch (rd.getSelectedItemIndex()) {
            case 0:
               surfaceType = BrainModelSurface::SURFACE_TYPE_FLAT;
               break;
            case 1:
               surfaceType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
               break;
            case 2:
               surfaceType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
               break;
            case 3:
               // SureFit crossover check
               break;
         }
         
         int numTileCrossovers, numNodeCrossovers;
         if (rd.getSelectedItemIndex() == 3) {
            numNodeCrossovers = bms->crossoverCheckSureFitEllipsoid();
         }
         else {
            bms->crossoverCheck(numTileCrossovers, numNodeCrossovers, surfaceType);
         }
/*         
         //
         // Set the overlay to crossovers (if there are any)
         //
         bool haveCrossovers = false;
         const int numNodes = theMainWindow->getBrainSet()->getNumberOfNodes();
         for (int i = 0; i < numNodes; i++) {
            if (theMainWindow->getBrainSet()->getNodeAttributes(i)->getCrossover()) {
               haveCrossovers = true;
               break;
            }
         }
         if (haveCrossovers) {
*/
         BrainModelSurfaceNodeColoring* bsnc = theMainWindow->getBrainSet()->getNodeColoring();
         if ((numNodeCrossovers > 0) || (numTileCrossovers > 0)) {
            theMainWindow->getBrainSet()->getPrimarySurfaceOverlay()->setOverlay(-1,
                                    BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS);
            theMainWindow->updateDisplayControlDialog();
         }
         bsnc->assignColors();
   
         GuiBrainModelOpenGL::updateAllGL(NULL);
         std::ostringstream str;
         str << "There are " << numNodeCrossovers << " node crossovers.\n" 
             << "There are " << numTileCrossovers << " tile crossovers." 
             << std::ends;
         QApplication::restoreOverrideCursor();
         QMessageBox::information(theMainWindow, "Crossover Check Results", str.str().c_str());
      }
   }
}

/**
 * Called to generate distortion
 */
void
GuiMainWindowSurfaceActions::slotMeasurementsDistortion()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      GuiDistortionDialog gdd(theMainWindow, bms);
      gdd.exec();
   }
}

/**
 * Called to set toplogy file for surfaces
 */
void
GuiMainWindowSurfaceActions::slotSurfaceInformation()
{
   GuiSurfaceInformationDialog* std = new GuiSurfaceInformationDialog(theMainWindow);
   std->exec(); 
}

/**
 * Called when compute normals is selected.
 */
void
GuiMainWindowSurfaceActions::slotNormalsGenerate()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      bms->computeNormals();
      GuiBrainModelOpenGL::updateAllGL(NULL);
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * Called when flip normals is selected.
 */
void
GuiMainWindowSurfaceActions::slotNormalsFlip()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         //
         // flip the order of the tile's nodes
         //
         tf->flipTileOrientation();
         
         //
         // compute normals for all surfaces using this topology file
         //
         const int num = theMainWindow->getBrainSet()->getNumberOfBrainModels();
         for (int i = 0; i < num; i++) {
            BrainModel* bm = theMainWindow->getBrainSet()->getBrainModel(i);
            if (bm->getModelType() == BrainModel::BRAIN_MODEL_SURFACE) {
               BrainModelSurface* bms = dynamic_cast<BrainModelSurface*>(bm);
               if (bms->getTopologyFile() == tf) {
                  bms->computeNormals();
               }
            }
         }
         
         GuiBrainModelOpenGL::updateAllGL(NULL);
      }
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * Called to display the region of interest dialog
 */
void
GuiMainWindowSurfaceActions::slotRegionOfInterest()
{
   theMainWindow->getSurfaceRegionOfInterestDialog(true);
}

/**
 * Called to display the region of interest dialog
 */
void
GuiMainWindowSurfaceActions::slotRegionOfInterestOLD()
{
   theMainWindow->getSurfaceRegionOfInterestDialogOLD(true);
}

/**
 * Called when show cuts is selected
 */
void
GuiMainWindowSurfaceActions::slotCutsShow()
{
   DisplaySettingsCuts* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCuts();
   dsc->setDisplayCuts(! dsc->getDisplayCuts());
   GuiBrainModelOpenGL::updateAllGL(NULL);
}

/**
 * Called when apply cuts is selected
 */
void
GuiMainWindowSurfaceActions::slotCutsApply()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      CutsFile* cutsFile = theMainWindow->getBrainSet()->getCutsFile();
      if (cutsFile != NULL) {
         if (cutsFile->getNumberOfBorders() <= 0) {
            QMessageBox::warning(theMainWindow, "No Cuts", "There are no cuts.");
            return;
         }
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         BrainModelSurfaceCutter bsc(bms,
                                    theMainWindow->getBrainSet()->getCutsFile(),
                                    BrainModelSurfaceCutter::CUTTING_MODE_NORMAL);
         try {
            bsc.execute();
         }
         catch (BrainModelAlgorithmException& e) {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(theMainWindow, "Apply Cuts Error", e.whatQString());
         }

         TopologyFile* tf = bms->getTopologyFile();
         if (tf != NULL) {
            const int numIslands = tf->disconnectIslands();
            if (numIslands > 0) {
               //
               // Move any disconnected nodes in surfaces that use the topology file
               //
               for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
                  BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
                  if (bms != NULL) {
                     if (bms->getTopologyFile() == tf) {
                        bms->moveDisconnectedNodesToOrigin();
                     }
                  }
               }
            }
            
            GuiFilesModified fm;
            fm.setTopologyModified();
            fm.setCoordinateModified();
            theMainWindow->fileModificationUpdate(fm);

         }

         GuiFilesModified fm;
         fm.setTopologyModified();
         fm.setCoordinateModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL(NULL);
         QApplication::restoreOverrideCursor();
         
         theMainWindow->speakText("Cuts have been applied.", false);
      }
   }
}

/**
 * Called when delete cuts with mouse is selected
 */
void
GuiMainWindowSurfaceActions::slotCutsDeleteWithMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CUT_DELETE);
}

/**
 * Called when delete all cuts is selected
 */
void
GuiMainWindowSurfaceActions::slotCutsDeleteAll()
{
   if (QMessageBox::warning(theMainWindow, "Delete All Cuts", 
                            "Are you sure you want to delete all cuts?",
                            (QMessageBox::Yes | QMessageBox::No),
                            QMessageBox::Yes)
                               == QMessageBox::Yes) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      theMainWindow->getBrainSet()->clearCutsFile();
      GuiFilesModified fm;
      fm.setCutModified();
      theMainWindow->fileModificationUpdate(fm);
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called when draw cuts is selected
 */
void
GuiMainWindowSurfaceActions::slotCutsDraw()
{
   DisplaySettingsCuts* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCuts();
   if (dsc->getDisplayCuts() == false) {
      dsc->setDisplayCuts(true);
      GuiBrainModelOpenGL::updateAllGL(NULL);
   }
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_CUT_DRAW);
}

/**
 * Called to multiresolution morph a flat surface.
 */
void
GuiMainWindowSurfaceActions::slotMultiresolutionMorphFlat()
{
   if (theMainWindow->getBrainModelSurface() != NULL) {
      GuiMultiresolutionMorphingDialog mmd(theMainWindow, 
                                           getFlatMultiresolutionMorphingObject(), 
                                           false);
      mmd.exec();
   }
}

/**
 * Called to multiresolution morph a sphere surface.
 */
void
GuiMainWindowSurfaceActions::slotMultiresolutionMorphSphere()
{
   if (theMainWindow->getBrainModelSurface() != NULL) {
      GuiMultiresolutionMorphingDialog mmd(theMainWindow, 
                                           getSphericalMultiresolutionMorphingObject(), 
                                           false);
      mmd.exec();
   }
}

/**
 * called when run deformation flat dialog is selected.
 */
void 
GuiMainWindowSurfaceActions::slotDeformationRunFlatDialog()
{
#ifdef Q_OS_WIN32
   QMessageBox::information(theMainWindow, "Not Available",
                            "Flat registration is not availble for the Windows version of Caret.");
   return;
#endif
   GuiSurfaceDeformationDialog sdd(theMainWindow, DeformationMapFile::DEFORMATION_TYPE_FLAT);
   sdd.exec();
}

/**
 * called when run deformation spherical dialog is selected.
 */
void 
GuiMainWindowSurfaceActions::slotDeformationRunSphericalDialog()
{
   GuiSurfaceDeformationDialog sdd(theMainWindow, DeformationMapFile::DEFORMATION_TYPE_SPHERE);
   sdd.exec();
}

/**
 * called when apply deformation dialog is selected
 */
void 
GuiMainWindowSurfaceActions::slotDeformationApplyDialog()
{
   GuiApplyDeformationMapDialog* admd = new GuiApplyDeformationMapDialog(theMainWindow);
   admd->setAttribute(Qt::WA_DeleteOnClose);
   admd->show();
}

/**
 * Called to subtract the params file AC from the surface.
 */
void
GuiMainWindowSurfaceActions::slotTransformSubtractAC()
{
   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      float acx, acy, acz;
      if (pf->getParameter(ParamsFile::keyACx, acx) &&
          pf->getParameter(ParamsFile::keyACy, acy) &&
          pf->getParameter(ParamsFile::keyACz, acz)) {
         TransformationMatrix tm;
         tm.translate(-acx, -acy, -acz);
         applyMatrixToMainWindowSurface(tm);
      }
   }
}

/**
 * Called to translate the surface by a user entered amount.
 */
void
GuiMainWindowSurfaceActions::slotTransformTranslate()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      if (translateDialog == NULL) {
         std::vector<QString> labels;
         std::vector<QString> values;
         labels.push_back("Translate X");   values.push_back("0.0");
         labels.push_back("Translate Y");   values.push_back("0.0");
         labels.push_back("Translate Z");   values.push_back("0.0");
         translateDialog = new QtMultipleInputDialog(
                                 theMainWindow,
                                 "Translate Surface",
                                 "",
                                 labels,
                                 values,
                                 true,
                                 false);
         QObject::connect(translateDialog, SIGNAL(signalApplyPressed()),
                          this, SLOT(slotTransformTranslateApply()));
      }
      translateDialog->show();
      translateDialog->activateWindow();
   }
}

/**
 * Called when apply button hit on translate dialog.
 */
void
GuiMainWindowSurfaceActions::slotTransformTranslateApply()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      std::vector<float> t;
      translateDialog->getValues(t);
      TransformationMatrix tm;
      tm.translate(t[0], t[1], t[2]);
      applyMatrixToMainWindowSurface(tm);
   }
}

/**
 * Called to scale the surface by a user entered amount.
 */
void
GuiMainWindowSurfaceActions::slotTransformScale()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      if (scaleDialog == NULL) {
         std::vector<QString> labels;
         std::vector<QString> values;
         labels.push_back("Scale ");   values.push_back("1.0");
         scaleDialog = new QtMultipleInputDialog(theMainWindow,
                                 "Scale Surface",
                                 "",
                                 labels,
                                 values,
                                 true,
                                 false);
         QObject::connect(scaleDialog, SIGNAL(signalApplyPressed()),
                          this, SLOT(slotTransformScaleApply()));
      }
   }
   scaleDialog->show();
   scaleDialog->activateWindow();
}

/**
 * Called when apply button hit on scale dialog.
 */
void
GuiMainWindowSurfaceActions::slotTransformScaleApply()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      std::vector<float> floatValues;
      scaleDialog->getValues(floatValues);
      const float scale = floatValues[0];
      
      TransformationMatrix tm;
      tm.scale(scale, scale, scale);
      applyMatrixToMainWindowSurface(tm);
   }
}

/**
 * apply a matrix to the main window surface.
 */
void 
GuiMainWindowSurfaceActions::applyMatrixToMainWindowSurface(TransformationMatrix& tm)
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

/* cells/foci should be projections so do not transform
      //
      // Transform cells/foci before surface since cells/foci are projected to surface
      //
      const bool fiducialFlag = ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_RAW) ||
                                 (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL));
      CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
      if (cf != NULL) {
         cf->applyTransformationMatrix(bms->getCoordinateFile(),
                                          bms->getTopologyFile(),
                                          fiducialFlag,
                                          std::numeric_limits<int>::min(),
                                          std::numeric_limits<int>::max(),
                                          tm,
                                          false);
      }
      FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
      if (ff != NULL) {
         ff->applyTransformationMatrix(bms->getCoordinateFile(),
                                          bms->getTopologyFile(),
                                          fiducialFlag,
                                          std::numeric_limits<int>::min(),
                                          std::numeric_limits<int>::max(),
                                          tm,
                                          false);
      }
*/
      //
      // Transform surface
      //
      bms->applyTransformationMatrix(tm);
      
      //
      // Transform borders
      //
      BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();      
      bmbs->applyTransformationMatrix(bms, tm);
      
      theMainWindow->getBrainSet()->applyAllProjectedFiles();
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called to translate surface to its center of mass.
 */
void
GuiMainWindowSurfaceActions::slotTransformCenterOfMass()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      float com[3];
      bms->getCenterOfMass(com);
      TransformationMatrix tm;
      tm.translate(-com[0], -com[1], -com[2]);
      applyMatrixToMainWindowSurface(tm);
   }
}

/**
 * called when spm transform selected.
 */
void 
GuiMainWindowSurfaceActions::slotTransformSpm()
{
   GuiSurfaceSpmTransformDialog spm(theMainWindow);
   spm.exec();
}

/**
 * called when transform apply current view is selected.
 */
void
GuiMainWindowSurfaceActions::slotTransformApplyCurrentView()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      WuQDataEntryDialog at(theMainWindow);
      at.setWindowTitle("Apply Current View");
      QCheckBox* applyTranslationCheckBox = at.addCheckBox("Apply Translation",
                                                           true);
      QCheckBox* applyRotationCheckBox = at.addCheckBox("Apply Rotation",
                                                           true);
      QCheckBox* applyScalingCheckBox = at.addCheckBox("Apply Scaling",
                                                           true);
      if (at.exec() == QDialog::Accepted) {
         bms->applyCurrentView(BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW,
                               applyTranslationCheckBox->isChecked(),
                               applyRotationCheckBox->isChecked(),
                               applyScalingCheckBox->isChecked());
         theMainWindow->getBrainSet()->applyAllProjectedFiles();
         GuiBrainModelOpenGL::updateAllGL();
      }
   }
}

/**
 * called when transform move disconnected nodes to the origin.
 */
void
GuiMainWindowSurfaceActions::slotTransformDisconnectedNodesToOrigin()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      bms->moveDisconnectedNodesToOrigin();
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * called when topology report selected.
 */
void 
GuiMainWindowSurfaceActions::slotTopologyReport()      
{
   static GuiSurfaceTopologyReportDialog* trd = NULL;
   if (trd == NULL) {
      trd = new GuiSurfaceTopologyReportDialog(theMainWindow);
   }
   trd->show();
   trd->activateWindow();
}

/**
 * replace topology with standard topology 
 */
void
GuiMainWindowSurfaceActions::slotTopologyReplaceWithStandardSurface()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      BrainModelStandardSurfaceReplacement bmssr(theMainWindow->getBrainSet(), bms);
      try {
         bmssr.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
         return;
      }
      
      GuiFilesModified fm;
      fm.setStatusForAll(true);
      theMainWindow->fileModificationUpdate(fm);
      theMainWindow->displayNewestSurfaceInMainWindow();
      GuiBrainModelOpenGL::updateAllGL();
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * called when correct fiducial selected.
 */
void 
GuiMainWindowSurfaceActions::slotTopologyCorrectFiducial()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms == NULL) {
      return;
   }
   
   if (bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
      if (QMessageBox::question(theMainWindow, "ERROR", 
                                  "Surface in the main window does not appear to be a fiducial.",
                                  (QMessageBox::Ok | QMessageBox::Cancel),
                                  QMessageBox::Cancel)
                                     == QMessageBox::Cancel) {
         return;
      }
   }
   
   if (bms->isTopologicallyCorrect()) {
      QMessageBox::information(theMainWindow,
                                 "INFO",
                                 "Surface is already topologically correct.");
      return;
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelSurfaceTopologyCorrector bmstc(theMainWindow->getBrainSet(), 
                                               bms);
   try {
      bmstc.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
   }
   
   theMainWindow->postSpecFileReadInitializations();
   GuiFilesModified fm;
   fm.setCoordinateModified();
   fm.setTopologyModified();
   theMainWindow->fileModificationUpdate(fm);
   theMainWindow->displayNewestSurfaceInMainWindow();
   GuiBrainModelOpenGL::updateAllGL();
   
   QApplication::beep();
   QApplication::restoreOverrideCursor();
}
      
/**
 * called when retessellate sphere selected.
 */
void 
GuiMainWindowSurfaceActions::slotTopologyRetessellateSphere()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   
   if (bms != NULL) {
      if (bms->getSurfaceType() != BrainModelSurface::SURFACE_TYPE_SPHERICAL) {
         if (QMessageBox::question(theMainWindow, "ERROR", 
                                     "Surface in the main window does not appear to be a sphere.",
                                     (QMessageBox::Ok | QMessageBox::Cancel),
                                     QMessageBox::Cancel)
                                        == QMessageBox::Cancel) {
            return;
         }
      }
      
      const int numNodes = bms->getNumberOfNodes();
      const TopologyFile* tf = bms->getTopologyFile();
      const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
      std::vector<bool> useNodeInTessellationFlag(numNodes);
      for (int i = 0; i < numNodes; i++) {
         useNodeInTessellationFlag[i] = th->getNodeHasNeighbors(i);
      }
      BrainModelSurfaceSphericalTessellator bmsst(theMainWindow->getBrainSet(), 
                                                  bms,
                                                  useNodeInTessellationFlag);

      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      try {
         bmsst.execute();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
      
      theMainWindow->postSpecFileReadInitializations();
      GuiFilesModified fm;
      fm.setCoordinateModified();
      fm.setTopologyModified();
      theMainWindow->fileModificationUpdate(fm);
      theMainWindow->displayNewestSurfaceInMainWindow();
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::beep();
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called when orient tiles consistently is selected.
 */
void
GuiMainWindowSurfaceActions::slotTopologyOrientTilesConsistently()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      bms->orientTilesConsistently();
      bms->computeNormals();
      GuiBrainModelOpenGL::updateAllGL(NULL);
   }
   
   QApplication::restoreOverrideCursor();
}

/**
 * Called to classify nodes into edges and interior using topology
 */
void
GuiMainWindowSurfaceActions::slotTopologyClassifyEdges()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      theMainWindow->getBrainSet()->classifyNodes(bms->getTopologyFile());
      GuiFilesModified fm;
      theMainWindow->fileModificationUpdate(fm);
      theMainWindow->getBrainSet()->getNodeColoring()->assignColors();
      GuiBrainModelOpenGL::updateAllGL(NULL);
   }
   QApplication::restoreOverrideCursor();
}

/**
 * Called to check for islands using topology.
 */
void
GuiMainWindowSurfaceActions::slotTopologyCheckForIslands()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         std::vector<int> v1, v2, v3;
         int numIslands = tf->findIslands(v1, v2, v3);
         if (numIslands > 0) {
            numIslands--;   
         }
         QString msg(StringUtilities::fromNumber(numIslands));
         msg.append(" islands were found.");
         QApplication::restoreOverrideCursor();
         QMessageBox::information(theMainWindow, "Islands", msg);
      }
   }
   QApplication::restoreOverrideCursor();
}

/**
 * Called to delete corner tiles
 */
void
GuiMainWindowSurfaceActions::slotTopologyDeleteCorners()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         std::vector<QString> labels;
         labels.push_back("Delete Stragglers Only");
         labels.push_back("Delete Corners and Stragglers");
         QtRadioButtonSelectionDialog rbsd(theMainWindow,
                                           "Delete Corners and Stragglers",
                                           "",
                                           labels,
                                           0);
         if (rbsd.exec() == QDialog::Accepted) {
            int cornerSelection = 0;
            if (rbsd.getSelectedItemIndex() == 0) {
               cornerSelection = 2;
            }
            else {
               cornerSelection = 1;
            }
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            const int numCorners = tf->removeCornerTiles(cornerSelection);
            if (numCorners > 0) {
               //
               // Move any disconnected nodes in surfaces that use the topology file
               //
               for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
                  BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
                  if (bms != NULL) {
                     if (bms->getTopologyFile() == tf) {
                        bms->moveDisconnectedNodesToOrigin();
                     }
                  }
               }
            }
            QString msg(StringUtilities::fromNumber(numCorners));
            msg.append(" corner tiles were deleted.");
            GuiBrainModelOpenGL::updateAllGL(NULL);
            QApplication::restoreOverrideCursor();
            QMessageBox::information(theMainWindow, "Corners Info", msg);
         }
      }
   }
}

/**
 * Called to disconnect islands using topology.
 */
void
GuiMainWindowSurfaceActions::slotTopologyDisconnectIslands()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      TopologyFile* tf = bms->getTopologyFile();
      if (tf != NULL) {
         const int numIslands = tf->disconnectIslands();
         if (numIslands > 0) {
            //
            // Move any disconnected nodes in surfaces that use the topology file
            //
            for (int i = 0; i < theMainWindow->getBrainSet()->getNumberOfBrainModels(); i++) {
               BrainModelSurface* bms = theMainWindow->getBrainSet()->getBrainModelSurface(i);
               if (bms != NULL) {
                  if (bms->getTopologyFile() == tf) {
                     bms->moveDisconnectedNodesToOrigin();
                  }
               }
            }
            GuiBrainModelOpenGL::updateAllGL(NULL);
         }
         
         QString msg(StringUtilities::fromNumber(numIslands));
         msg.append(" islands were removed.");
         QApplication::restoreOverrideCursor();
         QMessageBox::information(theMainWindow, "Islands", msg);
         GuiFilesModified fm;
         fm.setTopologyModified();
         fm.setCoordinateModified();
         theMainWindow->fileModificationUpdate(fm);

      }
   }
   QApplication::restoreOverrideCursor();
}

/**
 * Called when Nearest Spherical Border Distance is selected.
 */
void 
GuiMainWindowSurfaceActions::slotGeometrySphereBorderDistance()
{
/*
   bool ok = false;
   int sphereNumber = QInputDialog::getInteger("Deformation Measurement",
                                              "Sphere Number",
                                              3,
                                              1,
                                              std::numeric_limits<int>::max(),
                                              1,
                                              &ok,
                                              theMainWindow,
                                              "defNodesSkip");
   if (ok) {
*/
      BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
      BorderFile borderFile;
      if (bms != NULL) {
         BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
         bmbs->copyBordersToBorderFile(bms, borderFile);
      }
      
      BrainModelSurfaceDeformationMeasurement bmsdm(theMainWindow->getBrainSet(),
                                                    bms,
                                                    &borderFile,
                                                    theMainWindow->getBrainSet()->getMetricFile(),
                                                    0);

      try {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         bmsdm.execute();
         GuiFilesModified fm;
         fm.setMetricModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
         QApplication::restoreOverrideCursor();
         QApplication::beep();
      }
      catch (BrainModelAlgorithmException& e) {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", e.whatQString());
      }
/*
   }
*/
}

/**
 * called by scale sphere to surface area of fiducial.
 */
void 
GuiMainWindowSurfaceActions::slotGeometryScaleSphereToFiducialArea()
{   
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms == NULL) {
      QMessageBox::critical(theMainWindow, "ERROR", "There is no surface in the main window.");
   }   
   
   const BrainModelSurface* fiducial = theMainWindow->getBrainSet()->getActiveFiducialSurface();
   if (fiducial == NULL) {
      QMessageBox::critical(theMainWindow, "ERROR", "There is no active fiducial surface.");
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

   const float fiducialArea = fiducial->getSurfaceArea();
   bms->convertToSphereWithSurfaceArea(fiducialArea);

   GuiBrainModelOpenGL::updateAllGL();
   QApplication::restoreOverrideCursor();
}

/**
 * Add relief to a flat or spherical surface.
 */
void
GuiMainWindowSurfaceActions::slotGeometryRelief()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      bms->applyShapeToSurface(*theMainWindow->getBrainSet()->getSurfaceShapeFile(),
                               0,
                               0.5);
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * Expand or shrink a surface along its node's normals.
 */
void
GuiMainWindowSurfaceActions::slotGeometryExpand()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      bool valid = false;
      const float factor = QInputDialog::getDouble(theMainWindow, "Expand or Shrink Surface",
                                                   "Amount to Shrink (negative) or Expand(positive)",
                                                   1.0,
                                                   -100000.0,
                                                    100000.0,
                                                   4,
                                                   &valid);
      if (valid) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         bms->expandSurface(factor);
         theMainWindow->getBrainSet()->applyAllProjectedFiles();
         GuiBrainModelOpenGL::updateAllGL();
         QApplication::restoreOverrideCursor();
      }
   }
}

/**
 * called when generate inflated and ellipsoid from fiducial is selected.
 */
void 
GuiMainWindowSurfaceActions::slotGeometryInflatedAndEllipsoidFromFiducial()
{
   const BrainModelSurface* fiducial = theMainWindow->getBrainModelSurface();
   if (fiducial == NULL) {
      QMessageBox::critical(theMainWindow, "ERROR", "There is not a FIDUCIAL surface in the Main Window.");
      return;
   }
   generateInflatedAndOtherSurfaces(fiducial);
}

/**
 * generate inflated and other surfaces from fiducial.
 */
void 
GuiMainWindowSurfaceActions::generateInflatedAndOtherSurfaces(
                                    const BrainModelSurface* fiducialSurface,
                                    const bool enableEllipsoidFlag,
                                    const bool enableFingerSmoothingFlag,
                                    const bool checkInflatedFlag,
                                    const bool checkVeryInflatedFlag,
                                    const bool checkSphericalFlag,
                                    const bool checkCompMedWallFlag)
{      
   if (fiducialSurface->getIsFiducialSurface() == false) {
      QMessageBox::critical(theMainWindow, "ERROR", "Surface is not a fiducial surface.");
      return;
   }

   WuQDataEntryDialog ded(theMainWindow);
   ded.setWindowTitle("Choose Surfaces To Create");
   QCheckBox* createInflatedSurfaceCheckBox = ded.addCheckBox("Create Inflated Surface",
                                                              checkInflatedFlag);
   QCheckBox* createVeryInflatedSurfaceCheckBox = ded.addCheckBox("Create Very Inflated Surface",
                                                                  checkVeryInflatedFlag);
   QCheckBox* createEllipsoidSurfaceCheckBox = NULL;
   if (enableEllipsoidFlag) {
      createEllipsoidSurfaceCheckBox =
         ded.addCheckBox("Create Ellipsoid Surface", true);
   }
   QCheckBox* createSphericalSurfaceCheckBox = ded.addCheckBox("Create Spherical Surface",
                                                               checkSphericalFlag);
   QCheckBox* createCompMedWallSurfaceCheckBox = ded.addCheckBox("Create Compressed Medial Wall Surface",
                                                               checkCompMedWallFlag);
   QCheckBox* enableFingerSmoothingCheckBox = NULL;
   if (enableFingerSmoothingFlag) {
      enableFingerSmoothingCheckBox =
         ded.addCheckBox("Enable Finger Smoothing (Use if Topological Defects)",
                                                              false);
   }
   
   QCheckBox* scaleSurfaceCheckBox = ded.addCheckBox("Scale Surface to Match Area of Fiducial Surface",
                                                     true);
   QDoubleSpinBox* iterationScaleDoubleSpinBox = ded.addDoubleSpinBox("Iterations Scale",
                                                                      1.0);
   iterationScaleDoubleSpinBox->setToolTip(
            "Use the \"Iterations Scale\" to scale the iterations\n"
            "during the inflation processes.  In most cases, it is\n"
            "not necessary to use this option such as when the \n"
            "surface has been generated using Caret.  However, \n"
            "surfaces produced by FreeSurfer often contain a large\n"
            "number of nodes, 150,000 or more.  In this case, try\n"
            "an \"Iterations Scale\" of 2.5.");
   QCheckBox* generateMetricMeasurementsCheckBox = NULL;
   if (enableFingerSmoothingFlag) {
      generateMetricMeasurementsCheckBox = 
         ded.addCheckBox("Generate Metric Measurements", false);
   }
   
   if (ded.exec() == QDialog::Accepted) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      QApplication::processEvents();
      MetricFile* mf = NULL;
      if (generateMetricMeasurementsCheckBox != NULL) {
         if (generateMetricMeasurementsCheckBox->isChecked()) {
            mf = theMainWindow->getBrainSet()->getMetricFile();
         }
      }
      
      bool doEllipsoidFlag = false;
      if (createEllipsoidSurfaceCheckBox != NULL) {
         doEllipsoidFlag = createEllipsoidSurfaceCheckBox->isChecked();
      }
      bool doFingerSmoothingFlag = false;
      if (enableFingerSmoothingCheckBox != NULL) {
         doFingerSmoothingFlag = enableFingerSmoothingCheckBox->isChecked();
      }
      fiducialSurface->createInflatedAndEllipsoidFromFiducial(createInflatedSurfaceCheckBox->isChecked(),
                                                       createVeryInflatedSurfaceCheckBox->isChecked(),
                                                       doEllipsoidFlag,
                                                       createSphericalSurfaceCheckBox->isChecked(),
                                                       createCompMedWallSurfaceCheckBox->isChecked(),
                                                       doFingerSmoothingFlag,
                                                       scaleSurfaceCheckBox->isChecked(),
                                                       iterationScaleDoubleSpinBox->value(),
                                                       mf);
      GuiFilesModified fm;
      fm.setCoordinateModified();
      theMainWindow->fileModificationUpdate(fm);
      if (enableEllipsoidFlag) {
         theMainWindow->displayNewestSurfaceInMainWindow();
      }
      
      GuiBrainModelOpenGL::updateAllGL(NULL);
      QApplication::restoreOverrideCursor();
      
      theMainWindow->speakText("Inflated and or ellipsoid surfaces have been created.", false);
   }
}    

/**
 * called by geometry compress front face menu item.
 */
void 
GuiMainWindowSurfaceActions::slotGeometryCompressFrontFace()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      bool valid = false;
      const float factor = QInputDialog::getDouble(theMainWindow, "Compress Medial Wall",
                                                   "Compression Factor",
                                                   0.5,
                                                   0.01,
                                                   1000.0,
                                                   2,
                                                   &valid);
      if (valid) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         bms->compressFrontFace(factor, 0);
         GuiBrainModelOpenGL::updateAllGL();
         QApplication::restoreOverrideCursor();
      }
   }
}
      
/**
 * Called when geometry compress medial wall sub menu is selected
 */
void
GuiMainWindowSurfaceActions::slotGeometryCompressMedialWall()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      bool valid = false;
      const float factor = QInputDialog::getDouble(theMainWindow, "Compress Medial Wall",
                                                   "Compression Factor",
                                                   0.5,
                                                   0.01,
                                                   1000.0,
                                                   2,
                                                   &valid);
      if (valid) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         bms->convertSphereToCompressedMedialWall(factor);
         GuiBrainModelOpenGL::updateAllGL();
         QApplication::restoreOverrideCursor();
      }
   }
}

/**
 * Called when geometry covert to sphere sub menu is selected
 */
void
GuiMainWindowSurfaceActions::slotGeometryToSphere()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      bms->convertToSphereWithSurfaceArea();
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called when geometry covert to ellispoid sub menu is selected
 */
void
GuiMainWindowSurfaceActions::slotGeometryToEllipsoid()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      bms->convertToSphereWithSurfaceArea();
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called when geometry convert ellipse to sphere sub menu is selected
 */
void
GuiMainWindowSurfaceActions::slotGeometryEllipseToSphere()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      bms->convertEllipsoidToSphereWithSurfaceArea();
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called when geometry inflate sub menu is selected
 */
void
GuiMainWindowSurfaceActions::slotGeometryInflate()
{
   static GuiInflateSurfaceDialog* isd = NULL;
   if (isd == NULL) {
      isd = new GuiInflateSurfaceDialog(theMainWindow);
   }
   isd->show();
   isd->activateWindow();
}

/**
 * Called when geometry inflate sub menu is selected
 */
void
GuiMainWindowSurfaceActions::slotGeometryInflateAndSmoothFingers()
{
   static GuiInflateAndSmoothFingersDialog* isd = NULL;
   if (isd == NULL) {
      isd = new GuiInflateAndSmoothFingersDialog(theMainWindow, false);
   }
   isd->show();
   isd->activateWindow();
}

/**
 * Called when geometry sphere to flat sub menu is selected
 */
void
GuiMainWindowSurfaceActions::slotGeometrySphereToFlat()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      bms->convertSphereToFlat();
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();
   }
}

/**
 * Called when geometry sphere to flat through hole sub menu is selected
 */
void
GuiMainWindowSurfaceActions::slotGeometrySphereToFlatThroughHole()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      //
      // Find the geography column in the paint file
      //
      PaintFile* pf = theMainWindow->getBrainSet()->getPaintFile();
      const int geographyColumn = pf->getGeographyColumnNumber();
      if (geographyColumn >= 0) {
         //
         // Let user choose the name of the hole
         //
         GuiPaintColumnNamesListBoxSelectionDialog pnld(theMainWindow, geographyColumn);
         if (pnld.exec()) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            const int paintIndex = pnld.getSelectedItemIndex();
            std::vector<QString> names;
            names.push_back(pf->getPaintNameFromIndex(paintIndex));
            
            //
            // orient the surface so that the hole is on the negative Z axis
            //
            QString msg;
            bms->orientPaintedNodesToNegativeZAxis(pf,
                                                   names, 
                                                   geographyColumn, 
                                                   msg);
            
            //
            // disconnect the nodes that identify the hole
            //
            theMainWindow->getBrainSet()->disconnectNodes(bms->getTopologyFile(), names, geographyColumn);
            
            //
            // Make the sphere flat
            //
            bms->convertSphereToFlat();
            
            //
            // Update the graphics windows
            //
            GuiBrainModelOpenGL::updateAllGL();
            QApplication::restoreOverrideCursor();
         }
      }
      else {
         QApplication::restoreOverrideCursor();
         QMessageBox::critical(theMainWindow, "ERROR", "There is no geography column in the paint file.");
         return;
      }
   }
}

/**
 * Called when geometry smoothing menu item is selected.
 */
void
GuiMainWindowSurfaceActions::slotGeometrySmoothing()
{
   theMainWindow->displaySmoothingDialog();
}

/**
 * slot for identify menu highlight tile item.
 */
void 
GuiMainWindowSurfaceActions::slotIdentifyHighlightTile()
{
   const BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      const TopologyFile* tf = bms->getTopologyFile();
      if (tf->getNumberOfTiles() > 0) {
         bool valid = false;
         const int tileNumber = QInputDialog::getInteger(theMainWindow, "Highlight Tile",
                                                         "Tile Number",
                                                         0,
                                                         0,
                                                         tf->getNumberOfTiles() - 1,
                                                         1,
                                                         &valid);
         if (valid) {
            int n1, n2, n3;
            tf->getTile(tileNumber, n1, n2, n3);
            
            BrainSet* bs = theMainWindow->getBrainSet();
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            if (id != NULL) {
               BrainModelIdentification* bmi = bs->getBrainModelIdentification();
               const QString id1 = bmi->getIdentificationTextForNode(bs,
                                                                     n1,
                                                                     true,
                                                                     true);
               id->appendHtml(id1);
               const QString id2 = bmi->getIdentificationTextForNode(bs,
                                                                     n2,
                                                                     true,
                                                                     true);
               id->appendHtml(id2);
               const QString id3 = bmi->getIdentificationTextForNode(bs,
                                                                     n3,
                                                                     true,
                                                                     true);
               id->appendHtml(id3);
               bs->setDisplayCrossForNode(n3, NULL);
              
               //
               // Highlight symbols
               //
               bs->getNodeAttributes(n1)->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
               bs->getNodeAttributes(n2)->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
               bs->getNodeAttributes(n3)->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
               
               GuiBrainModelOpenGL::updateAllGL();
            }
         }
      }
   }
}

/**
 * slot for identify menu highlight node item
 */
void 
GuiMainWindowSurfaceActions::slotIdentifyHighlightNode()
{
   if (theMainWindow->getBrainSet()->getNumberOfNodes() > 0) {
      bool valid = false;
      const QString nodeText = QInputDialog::getText(theMainWindow, "Highlight Node",
                                                      "Node number(s).  Separate with spaces.",
                                                      QLineEdit::Normal,
                                                      "",
                                                      &valid);
      if (valid) {
         BrainSet* bs = theMainWindow->getBrainSet();
         std::vector<int> nodes;
         StringUtilities::token(nodeText, " ", nodes);
         if (nodes.empty() == false) {
            GuiIdentifyDialog* id = theMainWindow->getIdentifyDialog(true);
            BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
            if (id != NULL) {
               for (unsigned int i = 0; i < nodes.size(); i++) {
                  if (i == 0) {
                     bs->setDisplayCrossForNode(nodes[i], NULL);
                  }
                  const QString id1 = bmi->getIdentificationTextForNode(bs,
                                                                        nodes[i],
                                                                        true,
                                                                        true);
                  id->appendHtml(id1);
                  
                  //
                  // Highlight symbol
                  //
                  bs->getNodeAttributes(nodes[i])->setHighlighting(BrainSetNodeAttribute::HIGHLIGHT_NODE_LOCAL);
               }
            }
            GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}

/**
 * slot for identify menu clear symbols item
 */
void 
GuiMainWindowSurfaceActions::slotIdentifyClearSymbols()
{
   theMainWindow->getBrainSet()->clearNodeHighlightSymbols();
   GuiBrainModelOpenGL::updateAllGL();
}

/**
 * Called to display resectioning dialog.
 */
void
GuiMainWindowSurfaceActions::slotSectionResection()
{
   GuiResectionDialog rd(theMainWindow);
   rd.exec();
}

/**
 * Clear all or part of the section file.
 */
void
GuiMainWindowSurfaceActions::slotSectionClearAllOrPart()
{
   GuiFilesModified fm;
   fm.setSectionModified();
   GuiNodeAttributeFileClearResetDialog d(theMainWindow, GUI_NODE_FILE_TYPE_SECTION);
   d.exec();
}

/**
 * Project surface to plane.
 */
void
GuiMainWindowSurfaceActions::slotProjectToPlane()
{
   WuQDataEntryDialog ded(theMainWindow);
   ded.setWindowTitle("Project to Plane");
   ded.setTextAtTop("If coordinates are projected more than once without "
                    "restoring, the original coordinates will be lost.",
                    true);
   QRadioButton* positiveXRadioButton = ded.addRadioButton("Positive X");
   positiveXRadioButton->setToolTip("All Positive X-coordinates become zero.");
   QRadioButton* negativeXRadioButton = ded.addRadioButton("Negative X");
   negativeXRadioButton->setToolTip("All Negative X-coordinates become zero.");
   QRadioButton* positiveYRadioButton = ded.addRadioButton("Positive Y");
   positiveYRadioButton->setToolTip("All Positive Y-coordinates become zero.");
   QRadioButton* negativeYRadioButton = ded.addRadioButton("Negative Y");
   negativeYRadioButton->setToolTip("All Negative Y-coordinates become zero.");
   QRadioButton* positiveZRadioButton = ded.addRadioButton("Positive Z");
   positiveZRadioButton->setToolTip("All Positive Z-coordinates become zero.");
   QRadioButton* negativeZRadioButton = ded.addRadioButton("Negative Z");
   negativeZRadioButton->setToolTip("All Negative Z-coordinates become zero.");
   QRadioButton* restoreRadioButton = ded.addRadioButton("Restore");
   restoreRadioButton->setToolTip("Restore projected coordinates.");
   
   positiveZRadioButton->setChecked(true);
   
   if (ded.exec() == WuQDataEntryDialog::Accepted) {
      BrainModelSurface::COORDINATE_PLANE coordinatePlane =
                                      BrainModelSurface::COORDINATE_PLANE_NONE;
      if (positiveXRadioButton->isChecked()) {
         coordinatePlane = BrainModelSurface::COORDINATE_PLANE_MOVE_POSITIVE_X_TO_ZERO;
      }
      else if (negativeXRadioButton->isChecked()) {
         coordinatePlane = BrainModelSurface::COORDINATE_PLANE_MOVE_NEGATIVE_X_TO_ZERO;
      }
      else if (positiveYRadioButton->isChecked()) {
         coordinatePlane = BrainModelSurface::COORDINATE_PLANE_MOVE_POSITIVE_Y_TO_ZERO;
      }
      else if (negativeYRadioButton->isChecked()) {
         coordinatePlane = BrainModelSurface::COORDINATE_PLANE_MOVE_NEGATIVE_Y_TO_ZERO;
      }
      else if (positiveZRadioButton->isChecked()) {
         coordinatePlane = BrainModelSurface::COORDINATE_PLANE_MOVE_POSITIVE_Z_TO_ZERO;
      }
      else if (negativeZRadioButton->isChecked()) {
         coordinatePlane = BrainModelSurface::COORDINATE_PLANE_MOVE_NEGATIVE_Z_TO_ZERO;
      }
      else if (restoreRadioButton->isChecked()) {
         coordinatePlane = BrainModelSurface::COORDINATE_PLANE_RESTORE;
      }
      else {
         QMessageBox::critical(theMainWindow, "ERROR", "No projection option was selected.");
         return;
      }
      BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
      if (bms != NULL) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         bms->projectCoordinatesToPlane(coordinatePlane);
         GuiBrainModelOpenGL::updateAllGL();
         QApplication::restoreOverrideCursor();   
      }
   }
}

/**
 * called to add node.
 */
void 
GuiMainWindowSurfaceActions::slotEditAddNode()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_EDIT_ADD_NODE);
}

/**
 * called to add tile.
 */
void 
GuiMainWindowSurfaceActions::slotEditAddTile()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_EDIT_ADD_TILE);
}

/**
 * called to delete tile by link.
 */
void 
GuiMainWindowSurfaceActions::slotEditDeleteTileByLink()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_EDIT_DELETE_TILE_BY_LINK);
}

/**
 * called to disconnect node.
 */
void 
GuiMainWindowSurfaceActions::slotEditDisconnectNode()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_EDIT_DISCONNECT_NODE);
}

/**
 * called to move node.
 */
void 
GuiMainWindowSurfaceActions::slotEditMoveNode()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(GuiBrainModelOpenGL::MOUSE_MODE_EDIT_MOVE_NODE);
}

/**
 * slot called to orient surface so node placed at screen center.
 */
void 
GuiMainWindowSurfaceActions::slotViewPlaceNodeAtCenterOfScreen()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      bool ok = false;
      const int node = QInputDialog::getInteger(theMainWindow, "Orient to Node",
                                                "Node Number",
                                                0,
                                                0,
                                                2147483647,
                                                1,
                                                &ok);
      if (ok) {
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         bms->orientNodeToPositiveScreenZ(node, 0);
         GuiBrainModelOpenGL::updateAllGL();
         QApplication::restoreOverrideCursor();   
      }
   }
}
      
/**
 * Called to scale the surface to fit the window.
 */
void
GuiMainWindowSurfaceActions::slotViewScaleSurfaceToFitWindow()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelOpenGL()->getDisplayedBrainModelSurface();
   if (bms != NULL) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      double left, right, bottom, top, nearOrtho, farOrtho;
      theMainWindow->getBrainModelOpenGL()->getOrthographicBox(left, right, bottom, top, 
                                           nearOrtho, farOrtho);
      bms->setDefaultScaling(right, top);
      bms->setToStandardView(0, BrainModelSurface::VIEW_RESET);
      GuiBrainModelOpenGL::updateAllGL();
      QApplication::restoreOverrideCursor();   
   }
}

/**
 * Called when adjust view selected.
 */
void
GuiMainWindowSurfaceActions::slotViewAdjust()
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {   
      GuiSetViewDialog svd(theMainWindow, bms, 0);
      svd.exec();
   }
}

/**
 * Called to save a user view
 */
void
GuiMainWindowSurfaceActions::slotUserViewSaveView()
{
   GuiUserViewSaveDialog uvsd(theMainWindow);
   uvsd.exec();
   GuiMainWindowSurfaceMenu* surfaceMenu = theMainWindow->getMainWindowSurfaceMenu();
   surfaceMenu->loadUserViewSwitchViewSubMenu();
}

/**
 * Called to switch to a user view
 */
void
GuiMainWindowSurfaceActions::slotUserViewSwitchToView(int item)
{
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
      const PreferencesFile::UserView* uv = pf->getUserView(item);
      if (uv != NULL) {
         QString name;
         float trans[3], scale[3], matrix[16];
         bool transValid, scaleValid, matrixValid;
         uv->getViewInfo(name, matrix, trans, scale, 
                         matrixValid, transValid, scaleValid);
         if (matrixValid) {
            bms->setRotationMatrix(0, matrix);
         }
         if (transValid) {
            bms->setTranslation(0, trans);
         }
         if (scaleValid) {
            bms->setScaling(0, scale);
         }
         GuiBrainModelOpenGL::updateAllGL();
      } 
   }
}

/**
 * update the actions (typically called when menu is about to show)
 */
void 
GuiMainWindowSurfaceActions::updateActions()
{
   //
   // Cuts sub menu items
   //
   DisplaySettingsCuts* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCuts();
   cutsShowAction->setChecked(dsc->getDisplayCuts());

   ParamsFile* pf = theMainWindow->getBrainSet()->getParamsFile();
   
   //
   // Edit sub menu items
   //
   DisplaySettingsSurface* dss = theMainWindow->getBrainSet()->getDisplaySettingsSurface();
   
   bool nodesDisplayed = false;
   bool linksDisplayed = false;
   bool tilesDisplayed = false;
   switch (dss->getDrawMode()) {
      case DisplaySettingsSurface::DRAW_MODE_NODES:
         nodesDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_LINKS:
         linksDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_LINK_HIDDEN_LINE_REMOVAL:
         linksDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_LINKS_EDGES_ONLY:
         linksDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_NODES_AND_LINKS:
         nodesDisplayed = true;
         linksDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_TILES:
         tilesDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_TILES_WITH_LIGHT:
         tilesDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_TILES_WITH_LIGHT_NO_BACK:
         tilesDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_TILES_LINKS_NODES:
         tilesDisplayed = true;
         nodesDisplayed = true;
         linksDisplayed = true;
         break;
      case DisplaySettingsSurface::DRAW_MODE_NONE:
         break;
   }
   editAddNodeAction->setEnabled(nodesDisplayed);
   editAddTileAction->setEnabled(nodesDisplayed);
   editMoveNodeAction->setEnabled(nodesDisplayed);
   editDisconnectNodeAction->setEnabled(nodesDisplayed);
   
   //
   // Section sub menu items
   //
   SectionFile* sf = theMainWindow->getBrainSet()->getSectionFile();
   const bool haveSections = (sf->getNumberOfColumns() > 0);
   sectionClearAllOrPartAction->setEnabled(haveSections);
   sectionControlAction->setEnabled(haveSections);
   
   //
   // Transform sub menu items
   //
   float acx, acy, acz;
   transformSubtractACAction->setEnabled(
                                    (pf->getParameter(ParamsFile::keyACx, acx) &&
                                     pf->getParameter(ParamsFile::keyACy, acy) &&
                                     pf->getParameter(ParamsFile::keyACz, acz)));
}


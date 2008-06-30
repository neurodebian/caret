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


#ifndef __GUI_MAIN_WINDOW_SURFACE_ACTIONS_H__
#define __GUI_MAIN_WINDOW_SURFACE_ACTIONS_H__

#include <QObject>

class BrainModelSurfaceMultiresolutionMorphing;
class GuiMainWindow;
class GuiMorphingDialog;
class QAction;
class QtMultipleInputDialog;
class TransformationMatrix;

/// This class creates the Main Window's Surface Actions
class GuiMainWindowSurfaceActions : public QObject {

   Q_OBJECT
   
   public:
      /// constructor
      GuiMainWindowSurfaceActions(GuiMainWindow* parent);
      
      /// destructor
      ~GuiMainWindowSurfaceActions();
   
      /// get the flat surface multiresolution morphing object
      BrainModelSurfaceMultiresolutionMorphing* 
         getFlatMultiresolutionMorphingObject() { return flatMultiResMorphObject; }
      
      /// get the spherical surface multiresolution morphing object
      BrainModelSurfaceMultiresolutionMorphing* 
         getSphericalMultiresolutionMorphingObject() { return sphericalMultiResMorphObject; }
  
      /// called when align surface(s) to standard orientation is selected
      QAction* getAlignSurfacesToStandardOrientationAction() { return alignSurfacesToStandardOrientationAction; }
      
      /// called to show create average coordinate file dialog
      QAction* getAverageCoordinateFileAction() { return averageCoordinateFileAction; }
      
      /// action for automatic rotation
      QAction* getAutomaticRotationAction() { return automaticRotationAction; }
      
      /// action for convert surface to segmentation volume
      QAction* getSurfaceToSegmentationVolumeAction() { return surfaceToSegmentationVolumeAction; }
      
      /// action for simplify surface
      QAction* getSimplifySurfaceAction() { return simplifySurfaceAction; }
      
      /// called to interpolate surfaces
      QAction* getInterpolateSurfacesAction() { return interpolateSurfacesAction; }
      
      /// called to copy the surface in the main window
      QAction* getCopyMainWindowSurfaceAction() { return copyMainWindowSurfaceAction; }
      
      /// called when show cuts is selected
      QAction* getCutsShowAction() { return cutsShowAction; }
      
      /// called when apply cuts is selected
      QAction* getCutsApplyAction() { return cutsApplyAction; }
      
      /// called when delete cuts with mouse is selected
      QAction* getCutsDeleteWithMouseAction() { return cutsDeleteWithMouseAction; }
      
      /// called when delete all cuts is selected
      QAction* getCutsDeleteAllAction() { return cutsDeleteAllAction; }
      
      /// called when draw cuts is selected
      QAction* getCutsDrawAction() { return cutsDrawAction; }
      
      /// called when Nearest Spherical Border Distance is selected
      QAction* getGeometrySphereBorderDistanceAction() { return geometrySphereBorderDistanceAction; }
      
      /// called when run deformation dialog is selected
      QAction* getDeformationRunFlatDialogAction() { return deformationRunFlatDialogAction; }
      
      /// called when run deformation dialog is selected
      QAction* getDeformationRunSphericalDialogAction() { return deformationRunSphericalDialogAction; }
      
      /// called when apply deformation dialog is selected
      QAction* getDeformationApplyDialogAction() { return deformationApplyDialogAction; }
      
      /// called when flatten hemisphere is selected
      QAction* getFlattenHemisphereAction() { return flattenHemisphereAction; }
      
      /// called when crossover check is selected
      QAction* getMeasurementsCrossoverCheckAction() { return measurementsCrossoverCheckAction; }
      
      /// called when curvature generation is selected
      QAction* getMeasurementsCurvatureAction() { return measurementsCurvatureAction; }
      
      /// called when distortion generation is selected
      QAction* getMeasurementsDistortionAction() { return measurementsDistortionAction; }
      
      /// called when sulcal depth generation is selected
      QAction* getMeasurementsSulcalDepthAction() { return measurementsSulcalDepthAction; }
      
      /// called when shape from coordinate subtraction is selected
      QAction* getMeasurementsShapeFromCoordinateSubtraction() { return measurementsShapeFromCoordinateSubtraction; }
      
      /// called to compute normals
      QAction* getNormalsGenerateAction() { return normalsGenerateAction; }
      
      /// called when flip normals menu items is selected
      QAction* getNormalsFlipAction() { return normalsFlipAction; }
      
      /// slot for region of interest dialog
      QAction* getRegionOfInterestAction() { return regionOfInterestAction; }
      
      /// slot for region of interest dialog OLD
      QAction* getRegionOfInterestActionOLD() { return regionOfInterestActionOLD; }
      
      /// called when surface information menu item is selected
      QAction* getSurfaceInformationAction() { return surfaceInformationAction; }
      
      /// called when spm transform selected
      QAction* getTransformSpmAction() { return transformSpmAction; }
      
      /// called to translate surface to center of mass
      QAction* getTransformCenterOfMassAction() { return transformCenterOfMassAction; }
      
      /// called to subtract AC from main window surface
      QAction* getTransformSubtractACAction() { return transformSubtractACAction; }
      
      /// called to move disconnected nodes to the origin
      QAction* getTransformDisconnectedNodesToOriginAction() { return transformDisconnectedNodesToOriginAction; }
      
      /// called to apply current view to the surface
      QAction* getTransformApplyCurrentViewAction() { return transformApplyCurrentViewAction; }
      
      /// called to translate the surface by a user entered amount
      QAction* getTransformTranslateAction() { return transformTranslateAction; }
      
      /// called to translate the surface by a user entered amount
      QAction* getTransformScaleAction() { return transformScaleAction; }
      
      /// get sphere morph action
      QAction* getMorphSphereAction() { return morphSphereAction; }
      
      /// get flat morph action
      QAction* getMorphFlatAction() { return morphFlatAction; }
      
      /// slot for multiresolution morphing flat
      QAction* getMultiresolutionMorphFlatAction() { return multiresolutionMorphFlatAction; }
      
      /// slot for multiresolution morphing spherical
      QAction* getMultiresolutionMorphSphereAction() { return multiresolutionMorphSphereAction; }
      
      /// slot for deleting corner tiles
      QAction* getTopologyDeleteCornersAction() { return topologyDeleteCornersAction; }
      
      /// slot for topology set topology menu
      QAction* getTopologySetAction() { return topologySetAction; }
      
      /// slot for topology check for islands
      QAction* getTopologyCheckForIslandsAction() { return topologyCheckForIslandsAction; }
      
      /// slot for topology disconnect islands
      QAction* getTopologyDisconnectIslandsAction() { return topologyDisconnectIslandsAction; }
      
      /// called when classify edges is selected
      QAction* getTopologyClassifyEdgesAction() { return topologyClassifyEdgesAction; }
      
      /// called when orient tiles consistently
      QAction* getTopologyOrientTilesConsistentlyAction() { return topologyOrientTilesConsistentlyAction; }
      
      /// replace topology with standard topology 
      QAction* getTopologyReplaceWithStandardSurfaceAction() { return topologyReplaceWithStandardSurfaceAction; }

      /// called when retessellate sphere selected
      QAction* getTopologyRetessellateSphereAction() { return topologyRetessellateSphereAction; }
      
      /// action when correct fiducial selected
      QAction* getTopologyCorrectFiducialAction() { return topologyCorrectFiducialAction; }
      
      /// called when topology report selected
      QAction* getTopologyReportAction() { return topologyReportAction; }
      
      /// slot for identify menu item
      QAction* getIdentifyHighlightTileAction() { return identifyHighlightTileAction; }
      
      /// slot for identify menu item
      QAction* getIdentifyHighlightNodeAction() { return identifyHighlightNodeAction; }
      
      /// slot for identify menu item
      QAction* getIdentifyClearSymbolsAction() { return identifyClearSymbolsAction; }
      
      /// action for section control dialog
      QAction* getSectionControlAction() { return sectionControlAction; }
      
      /// called when clear all or part of section file selected
      QAction* getSectionClearAllOrPartAction() { return sectionClearAllOrPartAction; }
      
      /// called to display resectioning dialog
      QAction* getSectionResectionAction() { return sectionResectionAction; }
      
      /// called to add relief to a flat or spherical surface
      QAction* getGeometryReliefAction() { return geometryReliefAction; }

      /// called by geometry compress medial wall menu item
      QAction* getGeometryCompressMedialWallAction() { return geometryCompressMedialWallAction; }
      
      /// called by geometry compress front face menu item
      QAction* getGeometryCompressFrontFaceAction() { return geometryCompressFrontFaceAction; }
      
      /// called by geometry convert to sphere menu item
      QAction* getGeometryToSphereAction() { return geometryToSphereAction; }
      
      /// called by geometry convert to ellipsoid menu item
      QAction* getGeometryToEllipsoidAction() { return geometryToEllipsoidAction; }
      
      /// called by geometry convert ellipse to sphere menu item
      QAction* getGeometryEllipseToSphereAction() { return geometryEllipseToSphereAction; }
      
      /// called by geometry inflate menu item
      QAction* getGeometryInflateAction() { return geometryInflateAction; }
      
      /// called to expand surface along its normals
      QAction* getGeometryExpandAction() { return geometryExpandAction; }
      
      /// called by geometry inflate and smooth fingers menu item
      QAction* getGeometryInflateAndSmoothFingersAction() { return geometryInflateAndSmoothFingersAction; }
      
      /// called by geometry sphere to flat menu item
      QAction* getGeometrySphereToFlatAction() { return geometrySphereToFlatAction; }
      
      /// called by geometry sphere to flat through hole menu item
      QAction* getGeometrySphereToFlatThroughHoleAction() { return geometrySphereToFlatThroughHoleAction; }
      
      /// called by scale sphere to surface area of fiducial
      QAction* getGeometryScaleSphereToFiducialAreaAction() { return geometryScaleSphereToFiducialAreaAction; }
      
      /// called when geometry smoothing menu item is selected
      QAction* getGeometrySmoothingAction() { return geometrySmoothingAction; }
      
      /// called when generate inflated and ellipsoid from fiducial is selected
      QAction* getGeometryInflatedAndEllipsoidFromFiducialAction() { return geometryInflatedAndEllipsoidFromFiducialAction; }
      
      /// called to project to plane positive Z
      QAction* getProjectToPlanePositiveZAction() { return projectToPlanePositiveZAction; }
      
      /// called to project to plane negative Z
      QAction* getProjectToPlaneNegativeZAction() { return projectToPlaneNegativeZAction; }
      
      /// called to project to plane restore
      QAction* getProjectToPlaneRestoreAction() { return projectToPlaneRestoreAction; }
      
      /// called to add node
      QAction* getEditAddNodeAction() { return editAddNodeAction; }
      
      /// called to add tile
      QAction* getEditAddTileAction() { return editAddTileAction; }
      
      /// called to delete tile by link
      QAction* getEditDeleteTileByLinkAction() { return editDeleteTileByLinkAction; }
      
      /// called to disconnect node
      QAction* getEditDisconnectNodeAction() { return editDisconnectNodeAction; }
      
      /// called to move node
      QAction* getEditMoveNodeAction() { return editMoveNodeAction; }

      /// view adjust action
      QAction* getViewAdjustAction() { return viewAdjustAction; }
      
      /// view place node at center of screen action
      QAction* getViewPlaceNodeAtCenterOfScreenAction() { return viewPlaceNodeAtCenterOfScreenAction; }
      
      /// view scale surface to fit window action
      QAction* getViewScaleSurfaceToFitWindowAction() { return viewScaleSurfaceToFitWindowAction; }
      
      /// view save user action
      QAction* getViewUserSaveViewAction() { return viewUserSaveViewAction; }
      
   public slots:
      /// update the actions (typically called when menu is about to show)
      void updateActions();

      /// called when align surface(s) to standard orientation is selected
      void slotAlignSurfacesToStandardOrientation();
      
      /// called to show create average coordinate file dialog
      void slotAverageCoordinateFile();
      
      /// called to convert surface to a segmentation volume
      void slotSurfaceToSegmentationVolume();
      
      /// called to interpolate surfaces
      void slotInterpolateSurfaces();
      
      /// called to copy the surface in the main window
      void slotCopyMainWindowSurface();
      
      /// called when show cuts is selected
      void slotCutsShow();
      
      /// called when apply cuts is selected
      void slotCutsApply();
      
      /// called when delete cuts with mouse is selected
      void slotCutsDeleteWithMouse();
      
      /// called when delete all cuts is selected
      void slotCutsDeleteAll();
      
      /// called when draw cuts is selected
      void slotCutsDraw();
      
      /// called when run deformation dialog is selected
      void slotDeformationRunFlatDialog();
      
      /// called when run deformation dialog is selected
      void slotDeformationRunSphericalDialog();
      
      /// called when apply deformation dialog is selected
      void slotDeformationApplyDialog();
      
      /// called when flatten hemisphere is selected
      void slotFlattenHemisphere();
      
      /// called when crossover check is selected
      void slotMeasurementsCrossoverCheck();
      
      /// called when curvature generation is selected
      void slotMeasurementsCurvature();
      
      /// called when distortion generation is selected
      void slotMeasurementsDistortion();
      
      /// called when sulcal depth generation is selected
      void slotMeasurementsSulcalDepth();
      
      /// called when shape from coordinate subtraction is selected
      void slotMeasurementsShapeFromCoordinateSubtraction();
      
      /// called to compute normals
      void slotNormalsGenerate();
      
      /// called when flip normals menu items is selected
      void slotNormalsFlip();
      
      /// slot for region of interest dialog
      void slotRegionOfInterest();
      
      /// slot for region of interest dialog OLD
      void slotRegionOfInterestOLD();
      
      /// called when surface information menu item is selected
      void slotSurfaceInformation();
      
      /// called when spm transform selected
      void slotTransformSpm();
      
      /// called to translate surface to center of mass
      void slotTransformCenterOfMass();
      
      /// called to subtract AC from main window surface
      void slotTransformSubtractAC();
      
      /// called to move disconnected nodes to the origin
      void slotTransformDisconnectedNodesToOrigin();
      
      /// called to apply current view to the surface
      void slotTransformApplyCurrentView();
      
      /// called to translate the surface by a user entered amount
      void slotTransformTranslate();
      
      /// called to translate the surface by a user entered amount
      void slotTransformScale();
      
      /// slot for multiresolution morphing flat
      void slotMultiresolutionMorphFlat();
      
      /// slot for multiresolution morphing spherical
      void slotMultiresolutionMorphSphere();
      
      /// slot for deleting corner tiles
      void slotTopologyDeleteCorners();
      
      /// slot for topology check for islands
      void slotTopologyCheckForIslands();
      
      /// slot for topology disconnect islands
      void slotTopologyDisconnectIslands();
      
      /// called when classify edges is selected
      void slotTopologyClassifyEdges();
      
      /// called when orient tiles consistently
      void slotTopologyOrientTilesConsistently();
      
      /// replace topology with standard topology 
      void slotTopologyReplaceWithStandardSurface();

      /// called when retessellate sphere selected
      void slotTopologyRetessellateSphere();
      
      /// called when correct fiducial selected
      void slotTopologyCorrectFiducial();
      
      /// called when topology report selected
      void slotTopologyReport();
      
      /// slot for identify menu item
      void slotIdentifyHighlightTile();
      
      /// slot for identify menu item
      void slotIdentifyHighlightNode();
      
      /// slot for identify menu item
      void slotIdentifyClearSymbols();
      
      /// called when clear all or part of section file selected
      void slotSectionClearAllOrPart();
      
      /// called to display resectioning dialog
      void slotSectionResection();
      
      /// called when Nearest Spherical Border Distance is selected
      void slotGeometrySphereBorderDistance();
      
      /// called to add relief to a flat or spherical surface
      void slotGeometryRelief();

      /// called by geometry compress medial wall menu item
      void slotGeometryCompressMedialWall();
      
      /// called by geometry compress front face menu item
      void slotGeometryCompressFrontFace();
      
      /// called by geometry convert to sphere menu item
      void slotGeometryToSphere();
      
      /// called by geometry convert to ellispoid menu item
      void slotGeometryToEllipsoid();
      
      /// called by geometry convert ellipse to sphere menu item
      void slotGeometryEllipseToSphere();
      
      /// called by geometry inflate menu item
      void slotGeometryInflate();
      
      /// called to expand surface along its normals
      void slotGeometryExpand();
      
      /// called by geometry inflate and smooth fingers menu item
      void slotGeometryInflateAndSmoothFingers();
      
      /// called by geometry sphere to flat menu item
      void slotGeometrySphereToFlat();
      
      /// called by geometry sphere to flat through hole menu item
      void slotGeometrySphereToFlatThroughHole();
      
      /// called by scale sphere to surface area of fiducial
      void slotGeometryScaleSphereToFiducialArea();
      
      /// called when geometry smoothing menu item is selected
      void slotGeometrySmoothing();
      
      /// called when generate inflated and ellipsoid from fiducial is selected
      void slotGeometryInflatedAndEllipsoidFromFiducial();
      
      /// called to project to plane positive Z
      void slotProjectToPlanePositiveZ();
      
      /// called to project to plane negative Z
      void slotProjectToPlaneNegativeZ();
      
      /// called to project to plane restore
      void slotProjectToPlaneRestore();
      
      /// called to add node
      void slotEditAddNode();
      
      /// called to add tile
      void slotEditAddTile();
      
      /// called to delete tile by link
      void slotEditDeleteTileByLink();
      
      /// called to disconnect node
      void slotEditDisconnectNode();
      
      /// called to move node
      void slotEditMoveNode();
      
      /// slot called to save a user view
      void slotUserViewSaveView();
      
      /// slot to switch to a user view
      void slotUserViewSwitchToView(int item);
      
      /// slot called when adjust view selected
      void slotViewAdjust();
      
      /// slot called to orient surface so node placed at screen center
      void slotViewPlaceNodeAtCenterOfScreen();
      
      /// slot called to scale the surface to fit the window
      void slotViewScaleSurfaceToFitWindow();
      
      /// slot called to simplify the surface
      void slotSimplifySurface();
    
   protected slots:
      /// Called when apply button hit on scale dialog
      void slotTransformScaleApply();     

      /// Called when apply button hit on translate dialog
      void slotTransformTranslateApply();     


   protected:
      /// apply a matrix to the main window surface
      void applyMatrixToMainWindowSurface(TransformationMatrix& tm);
      
      /// translate dialog
      QtMultipleInputDialog* translateDialog;
      
      /// scale dialog
      QtMultipleInputDialog* scaleDialog;
      
      /// the flat morphing dialog
      GuiMorphingDialog* flatMorphDialog;
      
      /// the sphere morphing dialog
      GuiMorphingDialog* sphereMorphDialog;
      
      /// the flat surface multiresolution morphing object
      BrainModelSurfaceMultiresolutionMorphing* flatMultiResMorphObject;
      
      /// the spherical surface multiresolution morphing object
      BrainModelSurfaceMultiresolutionMorphing* sphericalMultiResMorphObject;
      
      /// action when align surface(s) to standard orientation is selected
      QAction* alignSurfacesToStandardOrientationAction;
      
      /// action to show create average coordinate file dialog
      QAction* averageCoordinateFileAction;
      
      /// action for automatic rotation
      QAction* automaticRotationAction;
      
      /// action for simplify surface
      QAction* simplifySurfaceAction;
      
      /// action to interpolate surfaces
      QAction* interpolateSurfacesAction;
      
      /// action to copy the surface in the main window
      QAction* copyMainWindowSurfaceAction;
      
      /// action when show cuts is selected
      QAction* cutsShowAction;
      
      /// action when apply cuts is selected
      QAction* cutsApplyAction;
      
      /// action when delete cuts with mouse is selected
      QAction* cutsDeleteWithMouseAction;
      
      /// action when delete all cuts is selected
      QAction* cutsDeleteAllAction;
      
      /// action when draw cuts is selected
      QAction* cutsDrawAction;
      
     /// action when run deformation dialog is selected
      QAction* deformationRunFlatDialogAction;
      
      /// action when run deformation dialog is selected
      QAction* deformationRunSphericalDialogAction;
      
      /// action when apply deformation dialog is selected
      QAction* deformationApplyDialogAction;
      
      /// action when flatten hemisphere is selected
      QAction* flattenHemisphereAction;
      
      /// action when crossover check is selected
      QAction* measurementsCrossoverCheckAction;
      
      /// action when curvature generation is selected
      QAction* measurementsCurvatureAction;
      
      /// action when distortion generation is selected
      QAction* measurementsDistortionAction;
      
      /// action when sulcal depth generation is selected
      QAction* measurementsSulcalDepthAction;
      
      /// action when shape from coordinate subtraction is selected
      QAction* measurementsShapeFromCoordinateSubtraction;
      
      /// action to compute normals
      QAction* normalsGenerateAction;
      
      /// action when flip normals menu items is selected
      QAction* normalsFlipAction;
      
      /// action for region of interest dialog
      QAction* regionOfInterestAction;
      
      /// action for region of interest dialog OLD
      QAction* regionOfInterestActionOLD;
      
      /// action when surface information menu item is selected
      QAction* surfaceInformationAction;
      
      /// action when spm transform selected
      QAction* transformSpmAction;
      
      /// action to translate surface to center of mass
      QAction* transformCenterOfMassAction;
      
      /// action to subtract AC from main window surface
      QAction* transformSubtractACAction;
      
      /// action to move disconnected nodes to the origin
      QAction* transformDisconnectedNodesToOriginAction;
      
      /// action to apply current view to the surface
      QAction* transformApplyCurrentViewAction;
      
      /// action to translate the surface by a user entered amount
      QAction* transformTranslateAction;
      
      /// action to translate the surface by a user entered amount
      QAction* transformScaleAction;
      
      ///  morph flat action
      QAction* morphFlatAction;
      
      ///  morph sphere action
      QAction* morphSphereAction;
      
      /// action for multiresolution morphing flat
      QAction* multiresolutionMorphFlatAction;
      
      /// action for multiresolution morphing spherical
      QAction* multiresolutionMorphSphereAction;
      
      /// action for deleting corner tiles
      QAction* topologyDeleteCornersAction;
      
      /// action for topology set topology menu
      QAction* topologySetAction;
      
      /// action for topology check for islands
      QAction* topologyCheckForIslandsAction;
      
      /// action for topology disconnect islands
      QAction* topologyDisconnectIslandsAction;
      
      /// action when classify edges is selected
      QAction* topologyClassifyEdgesAction;
      
      /// action when orient tiles consistently
      QAction* topologyOrientTilesConsistentlyAction;
      
      /// replace topology with standard topology 
      QAction* topologyReplaceWithStandardSurfaceAction;

      /// action when retessellate sphere selected
      QAction* topologyRetessellateSphereAction;
      
      /// action when correct fiducial selected
      QAction* topologyCorrectFiducialAction;
      
      /// action when topology report selected
      QAction* topologyReportAction;
      
      /// action for identify menu item
      QAction* identifyHighlightTileAction;
      
      /// action for identify menu item
      QAction* identifyHighlightNodeAction;
      
      /// action for identify menu item
      QAction* identifyClearSymbolsAction;
      
      /// action for section control dialog
      QAction* sectionControlAction;
      
      /// action when clear all or part of section file selected
      QAction* sectionClearAllOrPartAction;
      
      /// action to display resectioning dialog
      QAction* sectionResectionAction;
      
      /// action when Nearest Spherical Border Distance is selected
      QAction* geometrySphereBorderDistanceAction;
      
      /// action to add relief to a flat or spherical surface
      QAction* geometryReliefAction;

      /// action by geometry compress medial wall menu item
      QAction* geometryCompressMedialWallAction;
      
      /// action by geometry compress front face menu item
      QAction* geometryCompressFrontFaceAction;
      
      /// action by geometry convert to sphere menu item
      QAction* geometryToSphereAction;
      
      /// action by geometry convert to ellipsoid menu item
      QAction* geometryToEllipsoidAction;
            
      /// action by geometry convert ellipse to sphere menu item
      QAction* geometryEllipseToSphereAction;
      
      /// action by geometry inflate menu item
      QAction* geometryInflateAction;
      
      /// action to expand surface along its normals
      QAction* geometryExpandAction;
      
      /// action by geometry inflate and smooth fingers menu item
      QAction* geometryInflateAndSmoothFingersAction;
      
      /// action by geometry sphere to flat menu item
      QAction* geometrySphereToFlatAction;
      
      /// action by geometry sphere to flat through hole menu item
      QAction* geometrySphereToFlatThroughHoleAction;
      
      /// action by scale sphere to surface area of fiducial
      QAction* geometryScaleSphereToFiducialAreaAction;
      
      /// action when geometry smoothing menu item is selected
      QAction* geometrySmoothingAction;
      
      /// action when generate inflated and ellipsoid from fiducial is selected
      QAction* geometryInflatedAndEllipsoidFromFiducialAction;
      
      /// action to project to plane positive Z
      QAction* projectToPlanePositiveZAction;
      
      /// action to project to plane negative Z
      QAction* projectToPlaneNegativeZAction;
      
      /// action to project to plane restore
      QAction* projectToPlaneRestoreAction;
      
      /// action to add node
      QAction* editAddNodeAction;
      
      /// action to add tile
      QAction* editAddTileAction;
      
      /// action to delete tile by link
      QAction* editDeleteTileByLinkAction;
      
      /// action to disconnect node
      QAction* editDisconnectNodeAction;
      
      /// action to move node
      QAction* editMoveNodeAction;
      
      /// view adjust action
      QAction* viewAdjustAction;
      
      /// view place node at center of screen action
      QAction* viewPlaceNodeAtCenterOfScreenAction;
      
      /// view scale surface to fit window action
      QAction* viewScaleSurfaceToFitWindowAction;
      
      /// view save user action
      QAction* viewUserSaveViewAction;
      
      /// convert surface to segmentation volume
      QAction* surfaceToSegmentationVolumeAction;
};

#endif // __GUI_MAIN_WINDOW_SURFACE_ACTIONS_H__


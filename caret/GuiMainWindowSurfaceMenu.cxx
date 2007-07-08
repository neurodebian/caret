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

#include "BrainSet.h"
#include "DebugControl.h"
#include "GuiMainWindow.h"
#include "GuiMainWindowSurfaceActions.h"
#include "GuiMainWindowSurfaceMenu.h"
#include "global_variables.h"

/**
 * Constructor.
 */
GuiMainWindowSurfaceMenu::GuiMainWindowSurfaceMenu(GuiMainWindow* parent) :
   QMenu("Surface", parent)
{
   setObjectName("GuiMainWindowSurfaceMenu");
 
   GuiMainWindowSurfaceActions* surfaceActions = parent->getSurfaceActions();
     
   addAction(surfaceActions->getAlignSurfacesToStandardOrientationAction());
              
   addAction(surfaceActions->getAutomaticRotationAction());
   
   addAction(surfaceActions->getSurfaceToSegmentationVolumeAction());
   
   addAction(surfaceActions->getCopyMainWindowSurfaceAction());
   
   addAction(surfaceActions->getAverageCoordinateFileAction());
   
   createCutsSubMenu(surfaceActions);
   
   createDeformationSubMenu(surfaceActions);
   
   addAction(surfaceActions->getFlattenHemisphereAction());
   
   createEditSubMenu(surfaceActions);
   
   createGeometrySubMenu(surfaceActions);
   
   createIdentifySubMenu(surfaceActions);
   
   addAction(surfaceActions->getSurfaceInformationAction());
   
   addAction(surfaceActions->getInterpolateSurfacesAction());
   
   createMeasurementsSubMenu(surfaceActions);
   
   createMorphingSubMenu(surfaceActions);
   
   createNormalsSubMenu(surfaceActions);
   
   createProjectToPlaneSubMenu(surfaceActions);
   
   addAction(surfaceActions->getRegionOfInterestAction());
              
   createSectionsSubMenu(surfaceActions);
   
   addAction(surfaceActions->getSimplifySurfaceAction());

   createTopologySubMenu(surfaceActions);
   
   createTransformSubMenu(surfaceActions);
   
   createUserViewSubMenu(surfaceActions);

   QObject::connect(this, SIGNAL(aboutToShow()),
                    surfaceActions, SLOT(updateActions()));
                    
}

/**
 * Destructor.
 */
GuiMainWindowSurfaceMenu::~GuiMainWindowSurfaceMenu()
{
}

/**
 * Create the the normals sub menu
 */
void
GuiMainWindowSurfaceMenu::createNormalsSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   normalsSubMenu = addMenu("Normals");

   normalsSubMenu->addAction(surfaceActions->getNormalsFlipAction());

   normalsSubMenu->addAction(surfaceActions->getNormalsGenerateAction());   
}

/**
 * Create the measurements sub menu.
 */
void
GuiMainWindowSurfaceMenu::createMeasurementsSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   measurementsSubMenu = addMenu("Measurements");
   
   measurementsSubMenu->addAction(surfaceActions->getMeasurementsCrossoverCheckAction());
   
   measurementsSubMenu->addAction(surfaceActions->getMeasurementsCurvatureAction());
                                   
   measurementsSubMenu->addAction(surfaceActions->getMeasurementsShapeFromCoordinateSubtraction());
   
   measurementsSubMenu->addAction(surfaceActions->getMeasurementsDistortionAction());

   measurementsSubMenu->addAction(surfaceActions->getMeasurementsSulcalDepthAction());
}

/**
 * Create the measurements sub menu.
 */
void
GuiMainWindowSurfaceMenu::createCutsSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   cutsSubMenu = addMenu("Cuts");
   
   cutsSubMenu->addAction(surfaceActions->getCutsShowAction());
   
   cutsSubMenu->addAction(surfaceActions->getCutsApplyAction()); 
   
   cutsSubMenu->addAction(surfaceActions->getCutsDeleteWithMouseAction());
   
   cutsSubMenu->addAction(surfaceActions->getCutsDeleteAllAction()); 
   
   cutsSubMenu->addAction(surfaceActions->getCutsDrawAction());
   
}

/**
 * Create the morphing sub menu.
 */
void
GuiMainWindowSurfaceMenu::createMorphingSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   morphingSubMenu = addMenu("Morphing");
   
   morphingSubMenu->addAction(surfaceActions->getMorphFlatAction());
   
   morphingSubMenu->addAction(surfaceActions->getMorphSphereAction());
   
   morphingSubMenu->addSeparator();

   morphingSubMenu->addAction(surfaceActions->getMultiresolutionMorphFlatAction()); 
   
   morphingSubMenu->addAction(surfaceActions->getMultiresolutionMorphSphereAction()); 
}

/**
 * Create the morphing sub menu.
 */
void
GuiMainWindowSurfaceMenu::createDeformationSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   deformationSubMenu = addMenu("Deformation");
   
   deformationSubMenu->addAction(surfaceActions->getDeformationApplyDialogAction()); 
   
   deformationSubMenu->addSeparator();
   deformationSubMenu->addAction(surfaceActions->getDeformationRunFlatDialogAction()); 
   
   deformationSubMenu->addAction(surfaceActions->getDeformationRunSphericalDialogAction()); 
}

/**
 * Create the morphing sub menu.
 */
void
GuiMainWindowSurfaceMenu::createTransformSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   transformSubMenu = addMenu("Transform");
   
   transformSubMenu->addSeparator();
   
   transformSubMenu->addAction(surfaceActions->getTransformApplyCurrentViewAction());
                                
   transformSubMenu->addAction(surfaceActions->getTransformSpmAction());
   
   transformSubMenu->addSeparator();
   
   transformSubMenu->addAction(surfaceActions->getTransformDisconnectedNodesToOriginAction());
            
   transformSubMenu->addSeparator();
   
   transformSubMenu->addAction(surfaceActions->getTransformScaleAction());
                                
   transformSubMenu->addAction(surfaceActions->getTransformTranslateAction());
                                
   transformSubMenu->addAction(surfaceActions->getTransformSubtractACAction());
   
   transformSubMenu->addAction(surfaceActions->getTransformCenterOfMassAction()); 
}

/**
 * Create the topology sub menu.
 */
void
GuiMainWindowSurfaceMenu::createTopologySubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   topologySubMenu = addMenu("Topology");
   
   if (DebugControl::getTestFlag()) {
      createTopologyCorrectionSubMenu(surfaceActions, topologySubMenu);
      topologySubMenu->addSeparator();
   }
   
   topologySubMenu->addAction(surfaceActions->getTopologyCorrectFiducialAction());
   topologySubMenu->addSeparator();

   topologySubMenu->addAction(surfaceActions->getTopologyClassifyEdgesAction());
   
   topologySubMenu->addAction(surfaceActions->getTopologyOrientTilesConsistentlyAction());
                               
   topologySubMenu->addSeparator();
   
   topologySubMenu->addAction(surfaceActions->getTopologyCheckForIslandsAction());
                                
   topologySubMenu->addAction(surfaceActions->getTopologyDisconnectIslandsAction());
                                
   topologySubMenu->addAction(surfaceActions->getTopologyDeleteCornersAction());
                               
   topologySubMenu->addSeparator();

   topologySubMenu->addAction(surfaceActions->getTopologySetAction());
                                
   topologySubMenu->addSeparator();

   topologySubMenu->addAction(surfaceActions->getTopologyReportAction());
}

/**
 * Create the topology correction sub menu.
 */
void
GuiMainWindowSurfaceMenu::createTopologyCorrectionSubMenu(GuiMainWindowSurfaceActions* surfaceActions,
                                                          QMenu* parentMenu)
{
   topologyCorrectionSubMenu = parentMenu->addMenu("Correction");
   
/*
   topologyCorrectionSubMenu->addAction(surfaceActions->getTopologyReplaceWithStandardSurfaceAction());
*/
   topologyCorrectionSubMenu->addAction(surfaceActions->getTopologyRetessellateSphereAction());
}

/**
 * Create the geometry sub menu.
 */
void
GuiMainWindowSurfaceMenu::createGeometrySubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   geometrySubMenu = addMenu("Geometry");
   
   geometrySubMenu->addAction(surfaceActions->getGeometryCompressFrontFaceAction());
                               
   geometrySubMenu->addAction(surfaceActions->getGeometryCompressMedialWallAction());

   geometrySubMenu->addSeparator();
   
   geometrySubMenu->addAction(surfaceActions->getGeometryToSphereAction());
                                
   geometrySubMenu->addAction(surfaceActions->getGeometryEllipseToSphereAction());
                    
   geometrySubMenu->addAction(surfaceActions->getGeometryInflatedAndEllipsoidFromFiducialAction());
                               
   geometrySubMenu->addSeparator();
   
   geometrySubMenu->addAction(surfaceActions->getGeometryExpandAction());
                               
   geometrySubMenu->addAction(surfaceActions->getGeometryInflateAction());
                                
   geometrySubMenu->addAction(surfaceActions->getGeometryInflateAndSmoothFingersAction());
                                
   geometrySubMenu->addSeparator();
   geometrySubMenu->addAction(surfaceActions->getGeometrySphereBorderDistanceAction());

   geometrySubMenu->addSeparator();
   
   geometrySubMenu->addAction(surfaceActions->getGeometrySphereToFlatAction());
                                
   geometrySubMenu->addAction(surfaceActions->getGeometrySphereToFlatThroughHoleAction());
                                
   geometrySubMenu->addAction(surfaceActions->getGeometryScaleSphereToFiducialAreaAction());
                                
   geometrySubMenu->addSeparator();
   
   geometrySubMenu->addAction(surfaceActions->getGeometrySmoothingAction());

   if (DebugControl::getTestFlag()) {
      geometrySubMenu->addAction(surfaceActions->getGeometryReliefAction());
   }

}

/**
 * Create the identify sub menu.
 */
void
GuiMainWindowSurfaceMenu::createIdentifySubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   identifySubMenu = addMenu("Identify");
   
   identifySubMenu->addAction(surfaceActions->getIdentifyClearSymbolsAction());
                                
   identifySubMenu->addSeparator();
   
   identifySubMenu->addAction(surfaceActions->getIdentifyHighlightNodeAction());
   
   identifySubMenu->addAction(surfaceActions->getIdentifyHighlightTileAction());
}

/**
 * Create the sections sub menu.
 */
void
GuiMainWindowSurfaceMenu::createSectionsSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   sectionsSubMenu = addMenu("Sections");
   
   sectionsSubMenu->addAction(surfaceActions->getSectionClearAllOrPartAction());
    
   sectionsSubMenu->addAction(surfaceActions->getSectionResectionAction());
                               
   sectionsSubMenu->addAction(surfaceActions->getSectionControlAction()); 
}

/**
 * Create the user view sub menu.
 */
void
GuiMainWindowSurfaceMenu::createUserViewSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   userViewSubMenu = addMenu("View");
   QObject::connect(userViewSubMenu, SIGNAL(aboutToShow()),
                    this, SLOT(slotUserViewMenuAboutToShow()));
                    
   userViewSubMenu->addAction(surfaceActions->getViewAdjustAction());
   userViewSubMenu->addSeparator();
   
   if (DebugControl::getTestFlag()) {
      userViewSubMenu->addAction(surfaceActions->getViewPlaceNodeAtCenterOfScreenAction());
   }
                               
   userViewSubMenu->addAction(surfaceActions->getViewScaleSurfaceToFitWindowAction());
                                  
   userViewSubMenu->addSeparator();

   userViewSubMenu->addAction(surfaceActions->getViewUserSaveViewAction());
                           
   userViewSwitchViewSubMenu = userViewSubMenu->addMenu("Switch to User View");
   QObject::connect(userViewSwitchViewSubMenu, SIGNAL(triggered(QAction*)),
                    this, SLOT(slotUserViewSelected(QAction*)));
   loadUserViewSwitchViewSubMenu();
}

/**
 * called when a user view is selected.
 */
void 
GuiMainWindowSurfaceMenu::slotUserViewSelected(QAction* action)
{
   GuiMainWindowSurfaceActions* surfaceActions = theMainWindow->getSurfaceActions();
   const int item = action->data().toInt();
   surfaceActions->slotUserViewSwitchToView(item);
}

/**
 * Called when user view menu is about to show
 */
void
GuiMainWindowSurfaceMenu::slotUserViewMenuAboutToShow()
{
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   userViewSwitchViewSubMenu->setEnabled(pf->getNumberOfUserViews() > 0);
}

/**
 * Load the user view switch view menu.
 */
void
GuiMainWindowSurfaceMenu::loadUserViewSwitchViewSubMenu()
{
   userViewSwitchViewSubMenu->clear();
   if (theMainWindow == NULL) {
      return;
   }
   
   PreferencesFile* pf = theMainWindow->getBrainSet()->getPreferencesFile();
   const int num = pf->getNumberOfUserViews();
   for (int i = 0; i < num; i++) {
      const PreferencesFile::UserView* uv = pf->getUserView(i);
      const QString name = uv->getViewName();
      QAction* action = userViewSwitchViewSubMenu->addAction(name);
      action->setData(i);
   }
}

/**
 * create the project to plane sub menu.
 */
void 
GuiMainWindowSurfaceMenu::createProjectToPlaneSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   projectToPlaneSubMenu = addMenu("Project To Plane");
   
   projectToPlaneSubMenu->addAction(surfaceActions->getProjectToPlanePositiveZAction());
   
   projectToPlaneSubMenu->addAction(surfaceActions->getProjectToPlaneNegativeZAction());
   
   projectToPlaneSubMenu->addAction(surfaceActions->getProjectToPlaneRestoreAction());
}    

/**
 * Create the edit sub menu
 */
void 
GuiMainWindowSurfaceMenu::createEditSubMenu(GuiMainWindowSurfaceActions* surfaceActions)
{
   editSubMenu = addMenu("Edit");
   
   editSubMenu->addAction(surfaceActions->getEditAddNodeAction()); 

   editSubMenu->addAction(surfaceActions->getEditAddTileAction());

   editSubMenu->addAction(surfaceActions->getEditDeleteTileByLinkAction()); 

   editSubMenu->addAction(surfaceActions->getEditDisconnectNodeAction()); 

   editSubMenu->addAction(surfaceActions->getEditMoveNodeAction());
}


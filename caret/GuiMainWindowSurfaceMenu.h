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


#ifndef __GUI_MAIN_WINDOW_SURFACE_MENU_H__
#define __GUI_MAIN_WINDOW_SURFACE_MENU_H__

#include <vector>

#include <QMenu>

class GuiMainWindow;
class GuiMainWindowSurfaceActions;

/// This class creates the Main Window's Surface Menu.
class GuiMainWindowSurfaceMenu : public QMenu {

   Q_OBJECT
   
   public:
      /// constructor
      GuiMainWindowSurfaceMenu(GuiMainWindow* parent);
      
      /// destructor
      ~GuiMainWindowSurfaceMenu();
   
      /// load the user view switch view sub menu
      void loadUserViewSwitchViewSubMenu();
      
   private slots:   
      /// slot called when user view menu is about to show
      void slotUserViewMenuAboutToShow();
      
      /// called when a user view is selected
      void slotUserViewSelected(QAction*);
      
   private:
      /// create the edit sub menu
      void createEditSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the cuts sub menu
      void createCutsSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the geometry sub menu
      void createGeometrySubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the deformation sub menu
      void createDeformationSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the identify sub menu
      void createIdentifySubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the normals sub menu
      void createNormalsSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the measurements sub menu
      void createMeasurementsSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the morphing sub menu
      void createMorphingSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the project to plane sub menu
      void createProjectToPlaneSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the sections sub menu
      void createSectionsSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the transform sub menu
      void createTransformSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the topology sub menu
      void createTopologySubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// create the topology correction sub menu
      void createTopologyCorrectionSubMenu(GuiMainWindowSurfaceActions* surfaceActions,
                                           QMenu* parentMenu);
      
      /// create the user view sub menu
      void createUserViewSubMenu(GuiMainWindowSurfaceActions* surfaceActions);
      
      /// cuts sub menu
      QMenu* cutsSubMenu;
      
      /// edit sub menu
      QMenu* editSubMenu;
      
      /// deformation sub menu
      QMenu* deformationSubMenu;
      
      /// geometry sub menu
      QMenu* geometrySubMenu;
      
      /// identify sub menu
      QMenu* identifySubMenu;
      
      /// morphing sub menu
      QMenu* morphingSubMenu;
      
      /// normals sub menu
      QMenu* normalsSubMenu;
      
      /// measurements sub menu
      QMenu* measurementsSubMenu;
      
      /// project to plane sub menu
      QMenu* projectToPlaneSubMenu;
      
      /// transform sub menu
      QMenu* transformSubMenu;
      
      /// topology sub menu
      QMenu* topologySubMenu;
      
      /// topology correction sub menu
      QMenu* topologyCorrectionSubMenu;
      
      /// section sub menu
      QMenu* sectionsSubMenu;
      
      /// user view sub menu
      QMenu* userViewSubMenu;
      
      /// user view switch to view sub menu
      QMenu* userViewSwitchViewSubMenu;
};

#endif


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


#ifndef __GUI_MAIN_WINDOW_VOLUME_MENU_H__
#define __GUI_MAIN_WINDOW_VOLUME_MENU_H__

#include <QMenu>

class GuiBrainModelOpenGL;
class GuiMainWindow;
class GuiMainWindowVolumeActions;

/// This class creates the Main Window's Volume Menu.
class GuiMainWindowVolumeMenu : public QMenu {

   Q_OBJECT
   
   public:
      /// constructor
      GuiMainWindowVolumeMenu(GuiMainWindow* parent);
      
      /// destructor
      ~GuiMainWindowVolumeMenu();
   
      /// update the dialog
      void updateDialog();
      
   private:
      /// create the anatomy sub menu
      void createAnatomySubMenu();
      
      /// create the functional sub menu
      void createFunctionalSubMenu();
      
      /// create the prob atlas sub menu
      void createProbAtlasSubMenu();
      
      /// create the paint sub menu
      void createPaintSubMenu();
      
      /// create the segmentation sub menu
      void createSegmentationSubMenu();
      
      /// Create the the tranform volume sub menu
      void createTransformSubMenu();

      /// anatomy sub menu
      QMenu* anatomySubMenu;
      
      /// functional sub menu
      QMenu* functionalSubMenu;
      
      /// segmentation sub menu
      QMenu* segmentationSubMenu;
      
      /// transform sub menu
      QMenu* transformSubMenu;
      
      /// paint sub menu
      QMenu* paintSubMenu;
      
      /// prob atlas sub menu
      QMenu* probAtlasSubMenu;
      
      /// actions for volume menu
      GuiMainWindowVolumeActions* volumeActions;
};

#endif  // __GUI_MAIN_WINDOW_VOLUME_MENU_H__


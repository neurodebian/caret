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


#ifndef __GUI_TOOLBAR_ACTIONS_H__
#define __GUI_TOOLBAR_ACTIONS_H__

#include <QObject>

#include "BrainModel.h"

class GuiBrainModelOpenGL;
class QAction;

/// ToolBar actions
class GuiToolBarActions : public QObject {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiToolBarActions(GuiBrainModelOpenGL* brainModelOpenGLIn,
                        QWidget* parent = 0);
      
      /// Destructor
      ~GuiToolBarActions();
      
      /// action for view mode
      QAction* getViewModeAction() { return viewModeAction; }
      
      /// action for medial view
      QAction* getMedialViewAction() { return medialViewAction; }
      
      /// action for lateral view
      QAction* getLateralViewAction() { return lateralViewAction; }
      
      /// action for anterior view
      QAction* getAnteriorViewAction() { return anteriorViewAction; }
      
      /// action for posterior
      QAction* getPosteriorViewAction() { return posteriorViewAction; }
      
      /// action for dorsal view
      QAction* getDorsalViewAction() { return dorsalViewAction; }
      
      /// action for ventral view
      QAction* getVentralViewAction() { return ventralViewAction; }
      
      /// action for reset view
      QAction* getResetViewAction() { return resetViewAction; }
      
      /// action for x rotation 90
      QAction* getXRotation90Action() { return xRotation90Action; }
      
      /// action for y rotation 90
      QAction* getYRotation90Action() { return yRotation90Action; }
      
      /// action for z rotation 90
      QAction* getZRotation90Action() { return zRotation90Action; }
      
      /// action for display control dialog
      QAction* getDisplayControlDialogAction() { return displayControlDialogAction; }
      
      /// action for spec dialog
      QAction* getSpecDialogAction() { return specDialogAction; }
      
      /// action for yoke
      QAction* getYokeAction() { return yokeAction; }
      
      // action for volume underlay only
      QAction* getUnderlayVolumeOnlyAction() { return volumeUnderlayOnlyAction; }
      
   public slots:
      // called when View Mode tool button is selected
      void viewModeSlot();
      
      // called when medial view tool button is selected
      void medialViewSlot();

      // called when lateral view tool button is selected
      void lateralViewSlot();

      // called when anterior view tool button is selected
      void anteriorViewSlot();

      // called when posterior view tool button is selected
      void posteriorViewSlot();

      // called when dorsal view tool button is selected
      void dorsalViewSlot();

      // called when ventral view tool button is selected
      void ventralViewSlot();

      // called when reset view tool button is selected
      void resetViewSlot();

      // called when X rotation 90 tool button is selected
      void xRotation90Slot();
      
      // called when Y rotation 90 tool button is selected
      void yRotation90Slot();
      
      // called when Z rotation 90 tool button is selected
      void zRotation90Slot();
      
      // called when display control button pressed
      void displayControlDialogSlot();
      
      // called when spec button pressed
      void specDialogSlot();
      
      // called when yoke button toggled
      void yokeSlot(bool b);
      
      // Called when a standard view is selected.
      void setViewSelection(const BrainModel::STANDARD_VIEWS standardView);
      
      // called when underlay only button toggled
      void underlayOnlySlot(bool b);
      
   protected:
      /// Called when a rotation axis is selected.
      void setRotationSelection(int axis);

      /// the OpenGL widget for the actions
      GuiBrainModelOpenGL* brainModelOpenGL;
      
      /// action for view mode
      QAction* viewModeAction;
      
      /// action for medial view
      QAction* medialViewAction;
      
      /// action for lateral view
      QAction* lateralViewAction;
      
      /// action for anterior view
      QAction* anteriorViewAction;
      
      /// action for posterior
      QAction* posteriorViewAction;
      
      /// action for dorsal view
      QAction* dorsalViewAction;
      
      /// action for ventral view
      QAction* ventralViewAction;
      
      /// action for reset view
      QAction* resetViewAction;
      
      /// action for x rotation 90
      QAction* xRotation90Action;
      
      /// action for y rotation 90
      QAction* yRotation90Action;
      
      /// action for z rotation 90
      QAction* zRotation90Action;
      
      /// action for display control dialog
      QAction* displayControlDialogAction;
      
      /// action for spec dialog
      QAction* specDialogAction;
      
      /// action for yoking
      QAction* yokeAction;
      
      /// action for volume underlay only
      QAction* volumeUnderlayOnlyAction;
};

#endif // __GUI_TOOLBAR_ACTIONS_H__



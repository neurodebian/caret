
#ifndef __GUI_SPEC_AND_SCENE_FILE_CREATION_DIALOG_H__
#define __GUI_SPEC_AND_SCENE_FILE_CREATION_DIALOG_H__

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

#include <vector>

#include "WuQDialog.h"

class BrainSet;
class QLineEdit;

/// dialog for creating a spec and scene files from a group of scenes
class GuiSpecAndSceneFileCreationDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiSpecAndSceneFileCreationDialog(QWidget* parent,
                                        BrainSet* brainSet,
                                        const std::vector<int>& sceneIndices);
                                        
      // destructor
      ~GuiSpecAndSceneFileCreationDialog();
      
   protected:
      // called when OK/Cancel button pressed
      void done(int r);
      
      /// line edit for spec file name
      QLineEdit* specFileNameLineEdit;
      
      /// line edit for scene file name
      QLineEdit* sceneFileNameLineEdit;
      
      /// the brain set
      BrainSet* brainSet;
      
      /// the scene indices
      std::vector<int> sceneIndices;
};

#endif // __GUI_SPEC_AND_SCENE_FILE_CREATION_DIALOG_H__


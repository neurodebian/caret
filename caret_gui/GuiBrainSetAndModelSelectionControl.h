
#ifndef __GUI_BRAIN_SET_AND_MODEL_SELECTION_CONTROL_H__
#define __GUI_BRAIN_SET_AND_MODEL_SELECTION_CONTROL_H__

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

#include <QComboBox>

class BrainSet;
class BrainModel;

/// this class encapsulates a control for selecting a brain set and model
class GuiBrainSetAndModelSelectionControl : public QComboBox {
   Q_OBJECT
   
   public:
      // constructor
      GuiBrainSetAndModelSelectionControl(QWidget* parent = 0);
      
      // destructor
      ~GuiBrainSetAndModelSelectionControl();
      
      // get the selected brain set
      BrainSet* getSelectedBrainSet();
      
      // get the selected brain model
      BrainModel* getSelectedBrainModel();
      
      /// see if all fiducial is selected
      bool getAllFiducialSelected() const { return ((currentIndex() == allFiducialIndex) &&
                                                    (allFiducialIndex >= 0)); }
      // update the control
      void updateControl(BrainSet* selectedBrain = NULL,
                         BrainModel* selectedModel = NULL);
      
   protected:
      /// the brain sets for each selection
      std::vector<BrainSet*> theBrainSets;
      
      /// the brain models for each selection
      std::vector<BrainModel*> theBrainModels;
      
      /// the brain set for the first fiducial surface
      BrainSet* firstFiducialBrainSet;
      
      /// the brain model of the first fiducial surface
      BrainModel* firstFiducialModel;
      
      /// index of all fiducial surface(s) item
      int allFiducialIndex;
};

#endif // __GUI_BRAIN_SET_AND_MODEL_SELECTION_CONTROL_H__


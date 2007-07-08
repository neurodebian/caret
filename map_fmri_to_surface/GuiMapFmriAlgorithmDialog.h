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



#ifndef __GUI_MAP_FMRI_ALGORITHM_DIALOG_H__
#define __GUI_MAP_FMRI_ALGORITHM_DIALOG_H__

#include <qdialog.h>

class QComboBox;
class QLabel;
class QLineEdit;

/// Dialog for setting algorithm parameters
class GuiMapFmriAlgorithmDialog : public QDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMapFmriAlgorithmDialog(QWidget* parent);
      
      /// Destructor
      ~GuiMapFmriAlgorithmDialog();
      
   private slots:
      /// called when user closes dialog
      void done(int r);
      
      /// called when an algorithm selection is made
      void slotAlgorithmSelection(int item);
      
   private:
      /// enable the parameters based upon the algorithm
      void enableParameters();
      
      /// load the algorithm parameters
      void loadAlgorithmParameters();
      
      /// read the algorithm parameters
      void readAlgorithmParameters();
      
      /// algorithm selection combo box
      QComboBox* algorithmSelectionComboBox;
      
      /// neighbors label
      QLabel* neighborsLabel;
      
      /// neighbors line edit
      QLineEdit* neighborsLineEdit;
      
      /// sigma norm label
      QLabel* sigmaNormLabel;
      
      /// sigma norm line edit
      QLineEdit* sigmaNormLineEdit;
      
      /// sigma tang label
      QLabel* sigmaTangLabel;
      
      /// sigma tang line edit
      QLineEdit* sigmaTangLineEdit;
      
      /// norm below cutoff label
      QLabel* normBelowCutoffLabel;
      
      /// norm below cutoff line edit
      QLineEdit* normBelowCutoffLineEdit;
      
      /// norm above cutoff label
      QLabel* normAboveCutoffLabel;
      
      /// norm above cutoff line edit
      QLineEdit* normAboveCutoffLineEdit;
      
      /// tang cutoff label
      QLabel* tangCutoffLabel;
      
      /// tang cutoff line edit
      QLineEdit* tangCutoffLineEdit;
      
      /// max distance label
      QLabel* maxDistanceLabel;
      
      /// max distance line edit
      QLineEdit* maxDistanceLineEdit;
      
      /// splat factor label
      QLabel* splatFactorLabel;
      
      /// splat factor line edit
      QLineEdit* splatFactorLineEdit;
      
};

#endif  // __GUI_MAP_FMRI_ALGORITHM_DIALOG_H__


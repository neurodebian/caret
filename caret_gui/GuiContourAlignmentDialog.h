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

#ifndef __GUI_CONTOUR_ALIGNMENT_DIALOG_H__
#define __GUI_CONTOUR_ALIGNMENT_DIALOG_H__

#include "WuQDialog.h"

class QSpinBox;

/// Dialog for drawing contours
class GuiContourAlignmentDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiContourAlignmentDialog(QWidget* parent = 0, bool modalFlag = false, Qt::WindowFlags f = 0);
      
      /// Destructor
      ~GuiContourAlignmentDialog();
      
      /// get the alignment section
      int getAlignmentSectionNumber() const;
      
      /// update the dialog due to contour changes
      void updateDialog();
      
      // show the dialog
      void show();
      
   private slots:
      /// Called when apply alignment button is pressed
      void slotApplyAlignmentButton();
      
      /// Called when close button is pressed
      void slotCloseButton();
      
      /// Called reset view button is pressed
      void slotResetViewButton();
      
      /// Called when value of section number spin box is changed
      void slotSectionNumberSpinBox();
      
      /// Called when select region button is pressed
      void slotSelectRegionButton();
      
      /// Called when clear region button is pressed
      void slotClearRegionButton();
      
   private:
      /// section number spin box
      QSpinBox* sectionNumberSpinBox;
};

#endif // __GUI_CONTOUR_ALIGNMENT_DIALOG_H__


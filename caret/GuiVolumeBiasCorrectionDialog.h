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

#ifndef __GUI_VOLUME_BIAS_CORRECTION_DIALOG_H__
#define __GUI_VOLUME_BIAS_CORRECTION_DIALOG_H__

#include <QSpinBox>

#include "QtDialog.h"

/// dialog for segmenting an anatomical volume using a threshold
class GuiVolumeBiasCorrectionDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiVolumeBiasCorrectionDialog(QWidget* parent);
      
      /// destructor
      ~GuiVolumeBiasCorrectionDialog();
      
      /// called to update the dialog
      void updateDialog();
      
      /// called to show the dialog (override's QDialog's show() method)
      void show();
      
   public slots:
      /// called to close the dialog (override's QDialog's close() method)
      void close();
      
   protected slots:
      /// called when apply button is pressed
      void slotApplyPushButton();
      
      /// called when min gray value spin box value is changed
      void slotMinGrayValueSpinBox(int val);
      
      /// called when max white value spin box value is changed
      void slotMaxWhiteValueSpinBox(int val);
      
      /// called when histogram button pressed
      void slotHistogramPushButton();
      
   protected:
      /// min gray value spin box
      QSpinBox* minGrayValueSpinBox;
   
      /// max white value spin box
      QSpinBox* maxWhiteValueSpinBox;
   
      /// iterations spin box
      QSpinBox* iterationsSpinBox;
   
};

#endif // __GUI_VOLUME_BIAS_CORRECTION_DIALOG_H__

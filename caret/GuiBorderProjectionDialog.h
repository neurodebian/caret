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


#ifndef __VE_GUI_BORDER_PROJECTION_DIALOG_H__
#define __VE_GUI_BORDER_PROJECTION_DIALOG_H__

#include "WuQDialog.h"

class QRadioButton;

/// This class creates the border projection dialog.
class GuiBorderProjectionDialog : public WuQDialog {
   
   Q_OBJECT
   
   public:
      /// Constructor
      GuiBorderProjectionDialog(QWidget* parent);
      
      /// Destructor
      ~GuiBorderProjectionDialog();
      
   private:
      /// called by QDialog when accept/reject signal is emitted
      void done(int r);
      
      /// projection method nearest button
      QRadioButton* nearestNodeButton;

      /// projection method nearest button
      QRadioButton* nearestTileButton;

};

#endif // __VE_GUI_BORDER_PROJECTION_DIALOG_H__


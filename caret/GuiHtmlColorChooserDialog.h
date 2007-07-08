

#ifndef __GUI_HTML_COLOR_CHOOSER_DIALOG_H__
#define __GUI_HTML_COLOR_CHOOSER_DIALOG_H__

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
#include "QtDialogModal.h"

class QListWidget;

/// class for a dialog that selects HTML standard colors
class GuiHtmlColorChooserDialog : public QtDialogModal {
   public:
      // constructor
      GuiHtmlColorChooserDialog(QWidget* parent = 0);
      
      // destructor
      ~GuiHtmlColorChooserDialog();
      
      // get the selected color name and color components
      void getSelectedColor(QString& name,
                            unsigned char& red,
                            unsigned char& green,
                            unsigned char& blue) const;
                            
   protected:
      // list widget for selection
      QListWidget* colorListWidget;
      
      // names of colors
      std::vector<QString> colorNames;

};

#endif // __GUI_HTML_COLOR_CHOOSER_DIALOG_H__

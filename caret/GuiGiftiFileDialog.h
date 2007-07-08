
#ifndef __GUI_GIFTI_FILE_DIALOG_H__
#define __GUI_GIFTI_FILE_DIALOG_H__

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

#include <QFileDialog>

/// dialog for opening/saving GIFTI files
class GuiGiftiFileDialog : public QFileDialog {
   Q_OBJECT
   
   public:
      /// dialog mode
      enum DIALOG_MODE {
         /// open file mode
         DIALOG_MODE_FILE_OPEN,
         /// save file mode
         DIALOG_MODE_FILE_SAVE
      };
      
      // constructor
      GuiGiftiFileDialog(QWidget* parent,
                         const DIALOG_MODE modeIn);
                         
      // destructor
      ~GuiGiftiFileDialog();
      
      // show the dialog
      virtual void show();
      
   protected:
      // called when open/save/cancel pushbutton pressed
      virtual void done(int r);
      
      // save the file and return true if successful
      bool saveFile();
      
      // open the file and return true if successful
      bool openFile();
      
      /// mode of dialog
      DIALOG_MODE dialogMode;
};

#endif // __GUI_GIFTI_FILE_DIALOG_H__


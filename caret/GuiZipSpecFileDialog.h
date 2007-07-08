
#ifndef __GUI_ZIP_SPEC_FILE_DIALOG_H__
#define __GUI_ZIP_SPEC_FILE_DIALOG_H__

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

#include "QtDialogNonModal.h"

class QLineEdit;
class PreferencesFile;

/// class for the zip spec file dialog
class GuiZipSpecFileDialog : public QtDialogNonModal {
   Q_OBJECT
   
   public:
      // constructor
      GuiZipSpecFileDialog(QWidget* parent,
                           PreferencesFile* pref);
      
      // destructor
      ~GuiZipSpecFileDialog();
      
   protected slots:
      // called when apply button is pressed
      void slotApplyButton();
      
      // called when spec file button is pressed
      void slotSpecFileButton();
      
      // called when zip file button is pressed
      void slotZipFileButton();
      
   protected:
      /// spec file line edit
      QLineEdit* specFileLineEdit;
      
      /// zip file line edit
      QLineEdit* zipFileLineEdit;
      
      /// unzip dir line edit
      QLineEdit* unzipDirLineEdit;
      
      /// the preferences file
      PreferencesFile* preferencesFile;
};

#endif // __GUI_ZIP_SPEC_FILE_DIALOG_H__


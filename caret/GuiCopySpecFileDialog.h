
#ifndef __GUI_COPY_SPEC_FILE_DIALOG_H__
#define __GUI_COPY_SPEC_FILE_DIALOG_H__

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

#include "QtDialog.h"

class PreferencesFile;
class QRadioButton;
class QLineEdit;

/// Dialog for copying a spec file
class GuiCopySpecFileDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiCopySpecFileDialog(QWidget* parent, PreferencesFile* pref);
      
      /// destructor
      ~GuiCopySpecFileDialog();
      
   protected slots:
      /// called when apply button is pressed
      void slotApplyButton();
      
      /// called when copy spec file button is pressed
      void slotCopySpecFilePushButton();
      
      /// called when copy into directory button is pressed
      void slotCopyIntoDirectoryPushButton();
      
      /// called when a previous directory is selected
      void slotPreviousDirectory(int);
      
   protected:
      /// copy spec file line edit
      QLineEdit* copySpecFileLineEdit;
      
      /// copy into directory line edit
      QLineEdit* copyIntoDirectoryLineEdit;
      
      /// new spec file name line edit
      QLineEdit* newSpecFileNameLineEdit;
      
      /// copy data files radio button
      QRadioButton* copyDataFilesRadioButton;
      
      /// point to data files abs path radio button
      QRadioButton* pointToDataFilesAbsPathRadioButton;
      
      /// point to data files rel path radio button
      QRadioButton* pointToDataFilesRelPathRadioButton;
      
      /// user's preferences files
      PreferencesFile* preferencesFile;
      
      /// the previous directories
      std::vector<QString> previousDirectories;
};

#endif // __GUI_COPY_SPEC_FILE_DIALOG_H__


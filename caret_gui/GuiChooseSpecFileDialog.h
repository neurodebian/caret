
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

#ifndef __GUI_CHOOSE_SPEC_FILE_DIALOG_H__
#define __GUI_CHOOSE_SPEC_FILE_DIALOG_H__

#include <vector>
#include <QString>

#include "WuQFileDialog.h"

class GuiPreviousSpecFileComboBox;
class QPushButton;
class QGroupBox;
class PreferencesFile;
class QtTextEditDialog;

/// class for a file dialog specialized for choosing spec files
class GuiChooseSpecFileDialog : public WuQFileDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiChooseSpecFileDialog(QWidget* parent,
                              const std::vector<QString>& previousSpecFilesIn,
                              const bool modal,
                              const bool allowMultipleSelectionsFlag = false);
      
      /// Constructor
      GuiChooseSpecFileDialog(QWidget* parent,
                              const PreferencesFile* pf,
                              const bool modal,
                              const bool allowMultipleSelectionsFlag = false);
                              
      /// Destructor
      ~GuiChooseSpecFileDialog();
      
      /// get the selected spec file
      QString getSelectedSpecFile() const;
      
      /// get the selected spec files
      std::vector<QString> getSelectedSpecFiles() const;
      
   protected slots:
      /// called when a previous spec file is selected
      void slotPreviousSpecFileComboBox(const QString& name);
      
      /// called when view pushbutton is pressed
      void slotViewPushButton();
      
      /// Called when a file is highlighted
      void slotFilesSelected(const QStringList& name);

   protected:
      /// create the dialog
      void createDialog(const std::vector<QString>& specFileNames,
                        const bool allowMultipleSpecFilesFlag);
      
      /// previous spec files combo box
      GuiPreviousSpecFileComboBox* previousSpecFilesComboBox;
      
      /// previous spec files group box
      QGroupBox* previousSpecFilesGroupBox;
      
      /// view push button
      QPushButton* viewPushButton;
      
      /// highlighted file name for viewing
      QString highlightedFileName;
      
      /// view contents of spec file
      QtTextEditDialog* viewContentsDialog;
};

#endif // __GUI_CHOOSE_SPEC_FILE_DIALOG_H__


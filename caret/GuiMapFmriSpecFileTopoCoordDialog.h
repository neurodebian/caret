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

#ifndef __GUI_SPEC_FILE_TOPO_COORD_DIALOG_H__
#define __GUI_SPEC_FILE_TOPO_COORD_DIALOG_H__

#include <QString>
#include <vector>

#include "QtDialog.h"

#include "SpecFile.h"

class QListWidget;
class QComboBox;
class QLineEdit;
class QGroupBox;
class PreferencesFile;

/// Class for selecting topology and coordinate files from a spec file
class GuiMapFmriSpecFileTopoCoordDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMapFmriSpecFileTopoCoordDialog(QWidget* parent,
                                        PreferencesFile* pf);
                                 
      /// Destructor
      ~GuiMapFmriSpecFileTopoCoordDialog();
      
      /// get the name of the topo file
      QString getTopoFileName() const;
      
      /// get number of coordinate files
      int getNumberOfCoordFileNames() const;
      
      /// get the names of a coordinate files
      void getCoordFileNames(std::vector<QString>& names) const;
      
      /// Get the name of the selected spec file.
      QString getSpecFileName() const;

   protected slots:
      /// called when select spec file pushbutton pressed
      void slotSelectSpecPushButton();
      
      /// called when select all coord files pushbutton is pressed
      void slotSelectAllCoordPushButton();
      
   protected:
      /// overrides parent's version
      void done(int r);
      
      /// spec file name line edit
      QLineEdit* specFileLineEdit;
      
      /// the spec file
      SpecFile specFile;
      
      /// topo file combo box
      QComboBox* topoFileComboBox;
      
      /// coord file list box
      QListWidget* coordFileListBox;
      
      /// topo file group
      QGroupBox* topoGroupBox;
      
      /// coord file group
      QGroupBox* coordGroupBox;
      
      /// the preferences file
      PreferencesFile* preferencesFile;
};

#endif // __GUI_SPEC_FILE_TOPO_COORD_DIALOG_H__


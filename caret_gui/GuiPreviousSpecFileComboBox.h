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

#ifndef __GUI_PREVIOUS_SPEC_FILE_COMBO_BOX_H__
#define __GUI_PREVIOUS_SPEC_FILE_COMBO_BOX_H__

#include <QComboBox>

#include "PreferencesFile.h"

/// combo box for selecting previously loaded spec files
class GuiPreviousSpecFileComboBox : public QComboBox {
   Q_OBJECT
   
   public:
      /// Constructor.
      GuiPreviousSpecFileComboBox(QWidget*parent, 
                                  const std::vector<QString>& specFilesIn);
                                             
      /// Constructor
      GuiPreviousSpecFileComboBox(QWidget*parent, const PreferencesFile* pf);
                                  
      /// Destructor
      ~GuiPreviousSpecFileComboBox();
   
   signals:
      /// emitted when a previous spec file is selected
      void specFileSelected(const QString &);

   protected slots:
      /// called when a combo box item is selected
      void slotItemSelected(int item);
      
   protected:
      /// initialize combo box
      void initializeComboBox(const std::vector<QString>& specFileNames);
      
      /// names of spec files
      std::vector<QString> previousSpecFiles;
};

#endif // __GUI_PREVIOUS_SPEC_FILE_COMBO_BOX_H__


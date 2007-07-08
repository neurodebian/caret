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

#ifndef __QT_CHECK_BOX_SELELCTION_DIALOG_H__
#define __QT_CHECK_BOX_SELELCTION_DIALOG_H__

#include "QtDialog.h"

#include <QString>
#include <vector>

class QCheckBox;

/// class that creates a dialog for choosing non-mutually exlusive items
class QtCheckBoxSelectionDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// constructor
      QtCheckBoxSelectionDialog(QWidget* parent,
                                const QString& title,
                                const QString& textMessage,
                                const std::vector<QString>& itemLabels,
                                const std::vector<bool>& itemChecked);
      
      /// destructor
      ~QtCheckBoxSelectionDialog();
      
      /// get number of checkboxes
      int getNumberOfCheckBoxes() const { return checkBoxes.size(); }
      
      /// get checkbox selected
      bool getCheckBoxStatus(const int index) const;
      
   private:
      /// the checkboxes
      std::vector<QCheckBox*> checkBoxes;
};

#endif // __QT_CHECK_BOX_SELELCTION_DIALOG_H__


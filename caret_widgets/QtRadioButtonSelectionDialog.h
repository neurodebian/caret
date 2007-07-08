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

#ifndef __QT_RADIO_BUTTON_SELELCTION_DIALOG_H__
#define __QT_RADIO_BUTTON_SELELCTION_DIALOG_H__

#include "QtDialog.h"

#include <QString>
#include <vector>

class QButtonGroup;

/// class that creates a dialog for choosing mutually exlusive items
class QtRadioButtonSelectionDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// constructor
      QtRadioButtonSelectionDialog(QWidget* parent,
                                   const QString& title,
                                   const QString& message,
                                   const std::vector<QString>& itemLabels,
                                   const int defaultItem = -1);
      
      /// destructor
      ~QtRadioButtonSelectionDialog();
      
      /// get the selected item index
      int getSelectedItemIndex() const;
      
   private:
      /// button group for the radio buttons
      QButtonGroup* radioButtonGroup;
};

#endif // __QT_RADIO_BUTTON_SELELCTION_DIALOG_H__


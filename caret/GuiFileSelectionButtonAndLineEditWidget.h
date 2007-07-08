
#ifndef __GUI_FILE_SELECTION_BUTTON_AND_LINE_EDIT_WIDGET_H__
#define __GUI_FILE_SELECTION_BUTTON_AND_LINE_EDIT_WIDGET_H__

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

#include <QWidget>

class GuiFileSelectionButton;
class QLineEdit;

/// class creates horizontally layout of file selection button and a line edit
class GuiFileSelectionButtonAndLineEditWidget : public QWidget {
   Q_OBJECT
   
   public:
      // constructor
      GuiFileSelectionButtonAndLineEditWidget(const QString& buttonText,
                                              const QString& fileFilter,
                                              const bool fileMustExistIn,
                                              QWidget* parent = 0);
                                              
      // destructor
      ~GuiFileSelectionButtonAndLineEditWidget();
      
      // get the file name
      QString getFileName() const;
      
      // set the file name
      void setFileName(const QString& name);
      
      // set the button label's text
      void setButtonLabelText(const QString& text);
      
      /// get the line edit
      QLineEdit* getLineEdit() { return fileNameLineEdit; }
      
      /// get the file selection button
      GuiFileSelectionButton* getFileSelectionButton() { return fileSelectionButton; }
      
   protected slots:
      /// called when return pressed in the line edit
      void slotLineEditReturnPressed();
      
   signals:
      // a filename was selected
      void fileSelected(const QString&);
       
   protected:
      /// the file selection button
      GuiFileSelectionButton* fileSelectionButton;
      
      /// the line edit
      QLineEdit* fileNameLineEdit;
};

#endif // __GUI_FILE_SELECTION_BUTTON_AND_LINE_EDIT_WIDGET_H__


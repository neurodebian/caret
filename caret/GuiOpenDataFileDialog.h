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


#ifndef __VE_GUI_OPEN_DATA_FILE_DIALOG_H__
#define __VE_GUI_OPEN_DATA_FILE_DIALOG_H__

#include <vector>
#include <QString>

#include <q3filedialog.h>

#include "GuiDataFileDialog.h"

class QCheckBox;
class QComboBox;

///  This dialog is used for opening caret data files
class GuiOpenDataFileDialog : public Q3FileDialog, GuiDataFileDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiOpenDataFileDialog(QWidget* parent,
                            const QString& useThisFileFilter = "");

      /// Destructor
      ~GuiOpenDataFileDialog();
      
      /// show the dialog
      virtual void show();
      
      /// update the dialog
      void updateDialog();
      
      /// status of append selection
      bool getAppendSelection() const;
      
      /// status of update spec file selection
      bool getUpdateSpecFileSelection() const;
      
      /// static method to open a data file and make necessary updates as a result.
      /// returns true if an error occurred.
      static bool openDataFile(QWidget* parentWidget, const QString specFileTag,
                               const QString& name, const bool append, const bool updateSpecFile,
                               QString& errorMessage,
                               bool& relatedFileWarningFlag);
   private slots:
      /// overrides QFileDialog's done() method.  Intercept to get the file selected
      /// and see if it already exists and leave dialog open in some cases.
      void done(int r);
      
      /// Called when the view comment button is pressed
      void commentButtonSlot();
      
      /// Called when a file is highlighed in the dialog
      void fileHighlightedSlot(const QString& fileName);
      
      /// Called when a file filter is selected
      void filterSelectedSlot(const QString& filterNameIn);
      
      /// called when a previous directory is selected
      void slotPreviousDirectory(int item);
      
   private:
      /// create the caret unique section
      QWidget* createCaretUniqueSection();
      
      /// previous directories checkbox
      QComboBox* previousDirectoryComboBox;
      
      /// the append check button
      QCheckBox* appendCheckButton;
      
      /// the update spec file check button
      QCheckBox* updateSpecFileCheckButton;
      
      /// name of file that is currently highlighted
      QString highlightedFileName;
      
      /// view/edit comment button
      QPushButton* commentButton;
      
      /// previous directories
      std::vector<QString> previousDirectories;
      
      /// spm volume being opened has right on left
      QCheckBox* spmRightOnLeftVolumeCheckBox;

      /// saved position and size
      QRect savedPositionAndSize;
      
      /// saved position and size valid
      bool savedPositionAndSizeValid;
};
      
#ifdef _GUI_OPEN_DATA_FILE_DIALOG_MAIN_

#endif // _GUI_OPEN_DATA_FILE_DIALOG_MAIN_

#endif // __VE_GUI_OPEN_DATA_FILE_DIALOG_H__

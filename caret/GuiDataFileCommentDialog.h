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


#ifndef __GUI_DATA_FILE_COMMENT_DIALOG_H__
#define __GUI_DATA_FILE_COMMENT_DIALOG_H__

#include <set>

#include "AbstractFile.h"
#include "QtDialog.h"

class AbstractFileHeaderUpdater;
class BrainModelBorderFileInfo;
class GuiHyperLinkTextBrowser;
class GiftiNodeDataFile;
class NodeAttributeFile;
class QGridLayout;
class QLineEdit;
class QTabWidget;
class QTextEdit;
class VolumeFile;

/// Dialog that displays a scrolling text widget
class GuiDataFileCommentDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor for an Abstract File subclass in memory
      GuiDataFileCommentDialog(QWidget* parent, 
                               AbstractFile* af);
      
      /// The constructor for an AbstractFile subclass in a file.
      GuiDataFileCommentDialog(QWidget* parent, 
                               const QString& fileNameIn);
                               
      /// The constructor for an VolumeFile in a file.
      GuiDataFileCommentDialog(QWidget* parent, 
                               const QString& fileNameIn,
                               const bool volumeFileFlag);
                               
      /// Constructor for a Node Attribute File Column subclass in memory
      GuiDataFileCommentDialog(QWidget* parent, 
                               NodeAttributeFile* naf,
                               int nodeAttributeFileColumnIn);
      
      /// Constructor for a Node Attribute File Column subclass in memory
      GuiDataFileCommentDialog(QWidget* parent, 
                               GiftiNodeDataFile* naf,
                               int nodeAttributeFileColumnIn);
      
      /// Constructor for a BrainModelBorderFileInfo class in memory
      GuiDataFileCommentDialog(QWidget* parent,
                               BrainModelBorderFileInfo* bfi);
                               
      /// Destructor
      ~GuiDataFileCommentDialog();
      
   private slots:
      /// called to close the dialog
      void slotCloseDialog();
      
      /// called to edit comment
      void slotTabWidgetPageChanged(int indx);
      
      /// called if a key is pressed in the text browser
      void slotTextBrowserKeyPress();
      
   private:
      /// dialog modes 
      enum DIALOG_MODE {
         DIALOG_MODE_ABSTRACT_FILE_IN_MEMORY,
         DIALOG_MODE_ABSTRACT_FILE_NAME,
         DIALOG_MODE_VOLUME_FILE_NAME,
         DIALOG_MODE_VOLUME_FILE_IN_MEMORY,
         DIALOG_MODE_NODE_ATTRIBUTE_FILE_COLUMN_IN_MEMORY,
         DIALOG_MODE_BORDER_FILE_INFO_IN_MEMORY
      };
      
      /// create the dialog in the specified mode
      void createDialog(const DIALOG_MODE modeIn, const QString& fileName,
                        const std::vector<QString>& namesList,
                        const bool viewCommentOnly);

      /// initialize the dialog
      void initialize();
      
      /// load the header tag grid
      void loadHeaderTagGrid(const AbstractFile::AbstractFileHeaderContainer& hc);
      
      /// the dialog's mode
      DIALOG_MODE dialogMode;
      
      /// the text display
      GuiHyperLinkTextBrowser* textBrowser;
      
      /// the text editor
      QTextEdit* textEditor;
      
      /// data file being edited
      AbstractFile* dataFile;
      
      /// the tab widget
      QTabWidget* tabWidget;
      
      /// the file being edited
      //AbstractFileHeaderUpdater* fileHeaderUpdater;
      AbstractFile* fileHeaderUpdater;
      
      /// the gifti node data file being edited
      GiftiNodeDataFile* giftiNodeDataFile;
      
      /// the node attribute file being edited
      NodeAttributeFile* nodeAttributeFile;
      
      /// the node attribute file column
      int nodeAttributeFileColumn;
      
      /// header tags grid
      QGridLayout* headerTagGridLayout;
      
      /// node attribute column name line edit
      QLineEdit* nodeAttributeColumnNameLineEdit;
      
      /// brain model border file info in memory
      BrainModelBorderFileInfo* borderFileInfo;
      
      /// volume file for editing volume file on disk
      VolumeFile* volumeFileOnDisk;
      
      /// volume file for editing volume file in memory
      VolumeFile* volumeFileInMemory;
      
      /// volume file label for a volume file in memory
      QLineEdit* volumeFileLabelLineEdit;
};

#endif  // __GUI_DATA_FILE_COMMENT_DIALOG_H__


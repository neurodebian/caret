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

#ifndef __GUI_VIEW_CURRENT_FILES_DIALOG_H__
#define __GUI_VIEW_CURRENT_FILES_DIALOG_H__

#include <vector>

#include "QtDialog.h"

#include "BrainModelSurface.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "SpecFile.h"

class AbstractFile;
class BrainModelBorderFileInfo;
class BrainModelBorderSet;
class QButtonGroup;
class QGridLayout;
class VolumeFile;

/// Dialog to view currently loaded files
class GuiViewCurrentFilesDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiViewCurrentFilesDialog(QWidget* parent);
      
      /// Destructor
      ~GuiViewCurrentFilesDialog();
      
   private slots:
      /// called when an info button is pressed
      void slotInfoButtonGroup(int item);
      
      /// called when a view button is pressed
      void slotViewButtonGroup(int item);
      
      /// called when an border info button is pressed
      void slotBorderInfoButtonGroup(int item);
      
      /// called when a clear button is pressed
      void slotClearButtonGroup(int item);
      
      /// called to delete border projections
      void slotDeleteBorderProjections();
      
   private:
      /// add a file to the list of files
      void addFileToGrid(AbstractFile* af,
                         const GuiFilesModified& fileMask,
                         const char* typeName = NULL);
      
      /// add a file to the list of files
      void addFileToGrid(AbstractFile* af,
                         const GuiFilesModified& fileMask,
                         const SpecFile::Entry& dataFileInfo,
                         const char* typeName = NULL);
                         
      /// add a border file to the list of files
      void addBorderFileToGrid(BrainModelBorderSet* bmbs,
                               const BrainModelSurface::SURFACE_TYPES surfaceType,
                               const char* typeName);
      
      /// Add a border projection file to the grid.
      void addBorderProjectionFileToGrid(BrainModelBorderSet* bmbs);
      
      /// widget that contains files
      QWidget* fileGridWidget;
      
      /// layout for files
      QGridLayout* fileGridLayout;
      
      /// view button group
      QButtonGroup* viewButtonGroup;
      
      /// info button group
      QButtonGroup* infoButtonGroup;
      
      /// clear button group
      QButtonGroup* clearButtonGroup;
      
      /// border info button group
      QButtonGroup* borderInfoButtonGroup;
      
      /// pointers to files
      std::vector<AbstractFile*> dataFiles;
      
      /// file changed mask
      std::vector<GuiFilesModified> dataFileTypeMask;
      
      /// pointers to border file info
      std::vector<BrainModelBorderFileInfo*> borderInfoFileInfo;
      
      /// border projection info button
      QPushButton* borderProjInfoPB;
      
      /// border projection clear button
      QPushButton* borderProjClearPB;
      
      /// column for file type
      int columnFileTypeIndex;
      
      /// column for modified
      int columnModifiedIndex;
      
      /// column for view file
      int columnViewIndex;
      
      /// column for comment info
      int columnCommentIndex;
      
      /// column for clear file
      int columnClearIndex;
      
      /// column for file name
      int columnFileNameIndex;
      
      /// width of view/clear/info buttons
      int buttonWidth;
};

#endif // __GUI_VIEW_CURRENT_FILES_DIALOG_H__


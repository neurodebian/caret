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



#ifndef __GUI_MAP_FMRI_METRIC_NAMING_DIALOG_H__
#define __GUI_MAP_FMRI_METRIC_NAMING_DIALOG_H__

#include <vector>
#include <qdialog.h>

class QComboBox;
class QLineEdit;

/// Dialog for naming metric file related data
class GuiMapFmriMetricNamingDialog : public QDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMapFmriMetricNamingDialog(QWidget* parent);
      
      /// Destructor
      ~GuiMapFmriMetricNamingDialog();
      
   private slots:
      /// called when user closes dialog
      void done(int r);
      
      /// called when spec file select button is pressed
      void slotSpecFile();
      
      /// called when a metric column view/edit button is pressed
      void slotMetricColumnComment(int item);
      
      /// called when metric file select button is pressed
      void slotMetricFile();
      
      /// called by surface selection combo box
      void slotSurfaceSelection(int item);
      
   private:
      /// read current selections from the dialog
      void readCurrentSelections();
      
      /// surface selection combo box
      QComboBox* surfaceComboBox;
      
      /// spec file name line edit
      QLineEdit* specFileNameLineEdit;

      /// metric file name line edit
      QLineEdit* metricFileNameLineEdit;

      /// metric title line edit
      QLineEdit* metricTitleLineEdit;
      
      /// line edits for metric column names
      std::vector<QLineEdit*> metricColumnNameLineEdits;
      
      /// currently selected surface file
      int currentSurfaceFileIndex;
};

#endif //  __GUI_MAP_FMRI_METRIC_NAMING_DIALOG_H__


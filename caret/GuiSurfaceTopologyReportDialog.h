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

#ifndef __GUI_SURFACE_TOPOLOGY_REPORT_DIALOG_H__
#define __GUI_SURFACE_TOPOLOGY_REPORT_DIALOG_H__

#include <QString>

#include "QtDialog.h"

class QCheckBox;
class QLabel;
class QTextBrowser;

/// Dialog for displaying a surface topology error report
class GuiSurfaceTopologyReportDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// constructor
      GuiSurfaceTopologyReportDialog(QWidget* parent);
      
      /// destructor
      ~GuiSurfaceTopologyReportDialog();
      
      /// show the dialog (overrides QDialog's show())
      virtual void show();
      
   protected slots:
      /// called when apply button pressed
      void slotApplyButton();
      
      /// called when close button pressed
      void slotCloseButton();
      
   protected:
      /// clear the dialog's data
      void clearDialog();
      
      /// set the label with green if the value is correct, else red.
      void setLabel(QLabel* label, const int value, const int correctValue,
                    const QString& message = "");

      /// show non-manifold nodes check box
      QCheckBox* showNonManifoldNodesCheckBox;
      
      /// show straggler tiles check box
      QCheckBox* showStragglerTilesCheckBox;
      
      /// degenerate edges check box
      QCheckBox* degenEdgesCheckBox;
      
      /// surface type label
      QLabel* surfaceTypeLabel;
      
      /// non-manifold nodes label
      QLabel* nonManifoldNodesLabel;
      
      /// straggler tiles label
      QLabel* stragglerTilesLabel;
      
      /// degenerate edges label
      QLabel* degenEdgesLabel;
      
      /// number of objects label
      QLabel* numberOfObjectsLabel;
      
      /// euler count label
      QLabel* eulerCountLabel;
      
      /// numbe of holes label
      QLabel* numberOfHolesLabel;
      
      /// nodes label
      QLabel* numberOfNodesLabel;
      
      /// connected nodes label
      QLabel* numberOfConnectedNodesLabel;
      
      /// edges label
      QLabel* numberOfEdgesLabel;
      
      /// tiles label
      QLabel* numberOfTilesLabel;
      
      /// default label value
      QString defaultLabelValue;
      
      /// text browser for information
      QTextBrowser* infoTextBrowser;
};

#endif // __GUI_SURFACE_TOPOLOGY_REPORT_DIALOG_H__


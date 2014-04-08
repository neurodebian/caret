
#ifndef __GUI_CELL_AND_FOCI_REPORT_DIALOG_H__
#define __GUI_CELL_AND_FOCI_REPORT_DIALOG_H__

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

#include <vector>

#include "WuQDialog.h"

class GuiBrainModelSelectionComboBox;
class CellFile;
class QCheckBox;
class QToolButton;
class QtTableDialog;
class WuQWidgetGroup;

/// dialog for cell and foci reports
class GuiCellAndFociReportDialog : public WuQDialog {
   Q_OBJECT
   
   public:
      // constructor
      GuiCellAndFociReportDialog(QWidget* parent, 
                                 const bool fociFlag);
                                 
      // destructor
      ~GuiCellAndFociReportDialog();
      
      /// get the table dialog containing the results
      QtTableDialog* getResultsTableDialog() { return resultsTableDialog; }
      
   protected slots:
      // called when attributes all on button clicked
      void slotAttributesAllOnToolButton();
      
      // called when attributes all off button clicked
      void slotAttributesAllOffToolButton();
      
      // called when attributes core on button clicked
      void slotAttributesCoreOnToolButton();
      
      // called when paint attributes all on button clicked
      void slotPaintAttributesAllOnToolButton();
      
      // called when paint attributes all off button clicked
      void slotPaintAttributesAllOffToolButton();
      
      // called when sums attributes all on button clicked
      void slotSumsAttributesAllOnToolButton();
      
      // called when sums attributes all off button clicked
      void slotSumsAttributesAllOffToolButton();
      
   protected:
      // called when ok/cancel button pressed
      virtual void done(int r);
      
      // creat cell selection section
      QWidget* createCellSelectionSection();
      
      // create the surface section
      QWidget* createSurfaceSection();
      
      // create the cell/foci section
      QWidget* createCellFociSection();
                                 
      // create the paint section
      QWidget* createPaintSection();
      
      // create the sums section
      QWidget* createSumsSection();
      
      // determine if a check box is shown and checked
      bool checked(const QCheckBox* cb) const;
      
      /// table dialog containing results
      QtTableDialog* resultsTableDialog;
      
      /// include only displayed cell/foci checkbox
      QCheckBox* includeDisplayedCellsOnlyCheckBox;
      
      /// number check box
      QCheckBox* numberCheckBox;
      
      /// name check box
      QCheckBox* nameCheckBox;
      
      /// position check box
      QCheckBox* positionCheckBox;
      
      /// geography check box
      QCheckBox* geographyCheckBox;
      
      /// area check box
      QCheckBox* areaCheckBox;
      
      /// region of interest check box
      QCheckBox* regionOfInterestCheckBox;
      
      /// size check box
      QCheckBox* sizeCheckBox;
      
      /// statistic check box
      QCheckBox* statisticCheckBox;
      
      /// comment check box
      QCheckBox* commentCheckBox;
      
      /// hemisphere check box
      QCheckBox* structureCheckBox;
      
      /// class check box
      QCheckBox* classCheckBox;
      
      /// study name check box
      QCheckBox* studyNameCheckBox;
      
      /// study PMID check box
      QCheckBox* studyPMIDCheckBox;
      
      /// study format check box
      QCheckBox* studyDataFormatCheckBox;
      
      /// study data type check box
      QCheckBox* studyDataTypeCheckBox;
      
      /// study stereotaxic space
      QCheckBox* studyStereotaxicSpaceCheckBox;
      
      /// study table number check box
      QCheckBox* studyTableNumberCheckBox;
      
      /// study table sub header check box
      QCheckBox* studyTableSubHeaderCheckBox;
      
      /// study figure number check box
      QCheckBox* studyFigureNumberCheckBox;
      
      /// study figure panel check box
      QCheckBox* studyFigurePanelCheckBox;
      
      /// study page reference number check box
      QCheckBox* studyPageReferenceNumberCheckBox;

      /// study page reference subheader check box
      QCheckBox* studyPageReferenceSubHeaderCheckBox;
            
      /// study page number check box
      //QCheckBox* studyPageNumberCheckBox;
      
      /// paint data type check boxes
      std::vector<QCheckBox*> paintNameCheckBoxes;
      
      /// sums check boxes
      std::vector<QCheckBox*> sumsCheckBoxes;
      
      /// sums ID Number check box
      QCheckBox* sumsIDNumberCheckBox;
      
      /// sums repeat number check box
      QCheckBox* sumsRepeatNumberCheckBox;
      
      /// sums parent cell base ID check box
      QCheckBox* sumsParentCellBaseIDCheckBox;
      
      /// sums version number check box
      QCheckBox* sumsVersionNumberCheckBox;
      
      /// sums MSL ID check box
      QCheckBox* sumsMSLIDCheckBox;
      
      /// sums attribute id check box
      QCheckBox* sumsAttributeIDCheckBox;
      
      /// file type string (cell or foci)
      QString typeString;
      
      /// foci flag
      bool fociFlag;
      
      /// combo box for left hem surface selection
      GuiBrainModelSelectionComboBox* leftHemSelectionComboBox;
      
      /// combo box for right hem surface selection
      GuiBrainModelSelectionComboBox* rightHemSelectionComboBox;
      
      /// combo box for cerebellum surface selection
      GuiBrainModelSelectionComboBox* cerebellumSelectionComboBox;
      
      /// all attributes on tool button
      QToolButton *allAttributesOnToolButton;
      
      /// all attributes off tool button
      QToolButton *allAttributesOffToolButton;
      
      /// core attributes on tool button
      QToolButton *coreAttributesOnToolButton;
      
      /// widget group for all attributes
      WuQWidgetGroup* allAttributesWidgetGroup;
      
      /// widget group for all attributes
      WuQWidgetGroup* coreAttributesWidgetGroup;      
      
};

#endif // __GUI_CELL_AND_FOCI_REPORT_DIALOG_H__


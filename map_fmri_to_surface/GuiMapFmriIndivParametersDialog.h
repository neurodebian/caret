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



#ifndef __GUI_MAP_FMRI_INDIV_PARAMETERS_DIALOG_H__
#define __GUI_MAP_FMRI_INDIV_PARAMETERS_DIALOG_H__

#include <qdialog.h>

class QCheckBox;
class QComboBox;
class QHBox;
class QLabel;
class QLineEdit;

/// Dialog for setting individual parameters
class GuiMapFmriIndivParametersDialog : public QDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiMapFmriIndivParametersDialog(QWidget* parent);
      
      /// Destructor
      ~GuiMapFmriIndivParametersDialog();
      
   private slots:
      /// called when user closes dialog
      void done(int r);
      
      /// called when params button pressed
      void slotParamsButton();
      
      /// called when cropping checkbox pressed
      void slotCroppingCheckBox();
      
      /// called when padding checkbox pressed
      void slotPaddingCheckBox();
      
   private:
      /// enable/disable dialog items
      void enableDialogItems();
      
      /// load parameters into the dialog
      void loadParameters();

      /// read parameters into the dialog
      void readParameters();

      /// enable cropping check box
      QCheckBox* enableCroppingCheckBox;
      
      /// cropping origin horizontal box
      QHBox* croppingOriginHBox;
      
      /// cropping origin X line edit
      QLineEdit* croppingOriginXLineEdit;
      
      /// cropping origin Y line edit
      QLineEdit* croppingOriginYLineEdit;
      
      /// cropping origin Z line edit
      QLineEdit* croppingOriginZLineEdit;
      
      /// widget containing padding line edits
      QWidget* paddingWidget;
      
      /// enable padding check box
      QCheckBox* enablePaddingCheckBox;
      
      /// padding negative X line edit
      QLineEdit* negativeXPaddingLineEdit;
      
      /// padding negative Y line edit
      QLineEdit* negativeYPaddingLineEdit;
      
      /// padding negative Z line edit
      QLineEdit* negativeZPaddingLineEdit;
      
      /// padding positive X line edit
      QLineEdit* positiveXPaddingLineEdit;
      
      /// padding positive Y line edit
      QLineEdit* positiveYPaddingLineEdit;
      
      /// padding positive Z line edit
      QLineEdit* positiveZPaddingLineEdit;
      
      /// x orientation combo box
      QComboBox* xOrientationComboBox;
      
      /// y orientation combo box
      QComboBox* yOrientationComboBox;
      
      /// z orientation combo box
      QComboBox* zOrientationComboBox;
      
};

#endif //  __GUI_MAP_FMRI_INDIV_PARAMETERS_DIALOG_H__


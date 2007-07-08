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


#ifndef __VE_GUI_TRANSFORMATION_MATRIX_DIALOG_H__
#define __VE_GUI_TRANSFORMATION_MATRIX_DIALOG_H__

#include <stack>
#include <vector>

#include "QtDialog.h"
#include <q3mainwindow.h>

class GuiTransformationMatrixSelectionControl; 
class QCheckBox;
class QComboBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QDoubleSpinBox;
class TransformationMatrix;
class vtkTransform;

/// Dialog for editing transformation matrices.
class GuiTransformationMatrixDialog : public QtDialog {
   Q_OBJECT
   
   public:
      /// Constructor
      GuiTransformationMatrixDialog(QWidget* parent);
      
      /// Destructor
      ~GuiTransformationMatrixDialog();
      
      /// called when the mouse is pressed to make matrix available for undo
      void axesEventInMainWindow();
      
      /// update the dialog
      void updateDialog();
      
      /// update the display if the matrix is linked to main window or axis
      void updateMatrixDisplay(const TransformationMatrix* tm);
      
      /// set the translation values
      void setTranslation(const float tx, const float ty, const float tz);
      
   private slots:
      /// called to close the dialog
      void slotCloseDialog();
      
      /// called when a matrix is selected
      void slotMatrixSelectionComboBox(int item);

      /// called when a matrix component is changed
      void slotMatrixComponentChanged();
      
      /// called to set the matrix to the identity matrix
      void slotOperationIdentity();
      
      /// called to set the matrix to its inverse
      void slotOperationInverse();
      
      /// called to transpose the matrix
      void slotOperationTranspose();
      
      /// called to multiply the matrix by another matrix
      void slotOperationMultiply();
      
      /// called to translate in screen axes
      void slotTranslateScreenAxes();
      
      /// called to translate in object axes
      void slotTranslateObjectAxes();
      
      /// called to rotate in screen axes
      void slotRotateScreenAxes();
      
      /// called to rotate in object axes
      void slotRotateObjectAxes();
      
      /// called to scale object
      void slotScale();
      
      /// called to set translation with mouse
      void slotSetTranslationWithMouse();
      
      /// called when new matrix button pressed
      void slotMatrixNew();
      
      /// called when delete matrix button pressed
      void slotMatrixDelete();
      
      /// called when matrix attributes button pressed
      void slotMatrixAttributes();
      
      /// called when copy matrix button pressed
      void slotMatrixCopy();
      
      /// called when paste matrix button pressed
      void slotMatrixPaste();
      
      /// called when apply matrix to main window button pressed
      void slotMatrixApplyMainWindow();
      
      /// called when apply matrix to transform data file button pressed
      void slotMatrixApplyTransformDataFile();
      
      /// called when load matrix button pressed
      void slotMatrixLoad();
      
      /// called when the show axes check box is toggled
      void slotShowAxesCheckBox(bool val);
      
      /// called to pop a matrix off the undo stack
      void slotUndoPushButton();
      
      /// called when a matrix is selected for a data file
      void slotTransformMatrixSelection();
      
   private:
      /// class for storing a matrix for "undoing" an operation
      class UndoMatrix {
         public:
            /// Constructor
            UndoMatrix(const TransformationMatrix* tm);
                     
            /// Destructor
            UndoMatrix();
            
            /// get the matrix
            void getMatrix(TransformationMatrix* tm) const;
         
         protected:
            /// the matrix
            double mm[16];
      };
      
      /// matrix view type
      enum MATRIX_VIEW_TYPE {
         MATRIX_VIEW_MATRIX,
         MATRIX_VIEW_TRANSFORMATIONS
      };
      
      /// create the matrix section
      QWidget* createMatrixSection();
      
      /// create the operations section
      QWidget* createOperationsSection();
      
      /// create the operation section.
      QWidget* createMatrixButtonsSection();

      /// add current matrix values to the undo stack
      void addMatrixToUndoStack();
      
      /// enable/disable the undo button
      void enableDisableUndoPushButton();
      
      /// clear the undo stack
      void clearUndoStack();
      
      /// transfer the matrix into the matrix element part of dialog
      void transferMatrixIntoMatrixElementLineEdits();
      
      /// transfer the matrix into the matrix transformation spin boxes
      void transferMatrixIntoMatrixTransformSpinBoxes();
      
      /// transfer the matrix into the dialog
      void transferMatrixIntoDialog();
      
      /// transfer dialog values into matrix
      void transferDialogValuesIntoMatrix();
      
      /// get the matrix view type
      MATRIX_VIEW_TYPE getMatrixViewType() const;
      
      /// get main window transform matrix
      vtkTransform* getMainWindowRotationTransform();
      
      /// create the matrix editor page
      QWidget* createMatrixEditorPage();
      
      /// create the transform data file page
      QWidget* createTransformDataFilePage();
      
      /// update the main window viewing matrix
      void updateMainWindowViewingMatrix();
      
      /// tab widget for matrices and data files
      QTabWidget* matricesDataFilesTabWidget;
      
      /// matrix editor page
      QWidget* matrixEditorPage;
      
      /// transform data file page
      QWidget* transformDataFilePage;
      
      /// transform data file grid layout
      QGridLayout* transformDataFileGridLayout;
      
      /// transform data file labels
      std::vector<QLabel*> transformFileLabels;
      
      /// transform data file matrix control
      std::vector<GuiTransformationMatrixSelectionControl*> transformFileMatrixControls;
      
      /// tab widget for matrix or trans/rotate
      QTabWidget* matrixViewTabWidget;
      
      /// widget for matrix elements
      QWidget* matrixElementWidget;
      
      /// line edits for matrix elements
      QLineEdit* matrixElementLineEdits[4][4];
      
      /// comment label
      QLabel* commentLabel;
      
      /// matrix selection combo box
      QComboBox* matrixSelectionComboBox;
      
      /// matrix transformations widget
      QWidget* matrixTransformationsWidget;
      
      /// matrix view translation X
      QDoubleSpinBox* matrixTranslateXDoubleSpinBox;
      
      /// matrix view translation Y
      QDoubleSpinBox* matrixTranslateYDoubleSpinBox;
      
      /// matrix view translation Z
      QDoubleSpinBox* matrixTranslateZDoubleSpinBox;
      
      /// matrix view rotate X
      QDoubleSpinBox* matrixRotateXDoubleSpinBox;
      
      /// matrix view rotate Y
      QDoubleSpinBox* matrixRotateYDoubleSpinBox;
      
      /// matrix view rotate Z
      QDoubleSpinBox* matrixRotateZDoubleSpinBox;
      
      /// matrix view scale X
      QDoubleSpinBox* matrixScaleXDoubleSpinBox;
      
      /// matrix view scale Y
      QDoubleSpinBox* matrixScaleYDoubleSpinBox;
      
      /// matrix view scale Z
      QDoubleSpinBox* matrixScaleZDoubleSpinBox;
      
      /// the show axes check box
      QCheckBox* showAxesCheckBox;
      
      /// yoke to main window
      QCheckBox* yokeToMainWindowCheckBox;
      
      /// current matrix in dialog
      TransformationMatrix* currentMatrix;
      
      /// the undo matrix stack
      std::stack<UndoMatrix> undoMatrixStack;
      
      /// the undo push button
      QPushButton* undoPushButton;
};

#endif // __VE_GUI_TRANSFORMATION_MATRIX_DIALOG_H__

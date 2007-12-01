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

#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <limits>
#include <sstream>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QToolTip>

#include "AbstractFile.h"
#include "BorderFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelContours.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceAndVolume.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "CellFile.h"
#include "CellProjectionFile.h"
#include "ContourFile.h"
#include "ContourCellFile.h"
#include "DisplaySettingsCells.h"
#include "DisplaySettingsContours.h"
#include "DisplaySettingsFoci.h"
#include "DisplaySettingsVolume.h"
#include "FileUtilities.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GuiBrainModelOpenGL.h"
#include "GuiFilesModified.h"
#include "GuiMainWindow.h"
#include "GuiTransformationMatrixDialog.h"
#include "GuiTransformationMatrixSelectionControl.h"
#include "MathUtilities.h"
#include "QtCheckBoxSelectionDialog.h"
#include <QDoubleSpinBox>
#include "QtMultipleInputDialog.h"
#include "QtRadioButtonSelectionDialog.h"
#include "TransformationMatrixFile.h"
#include "VolumeFile.h"
#include "VtkModelFile.h"
#include "global_variables.h"
#include "vtkTransform.h"

/**
 * Constructor.
 */
GuiTransformationMatrixDialog::GuiTransformationMatrixDialog(QWidget* parent)
   : QtDialog(parent, false)
{
   currentMatrix = NULL;
   setWindowTitle("Transformation Matrix Editor");
   
   //
   // Vertical box layout of all items
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(3);
   dialogLayout->setSpacing(3);
   
   //
   // Create a tab widget for matrices and data files
   //
   matricesDataFilesTabWidget = new QTabWidget;
   dialogLayout->addWidget(matricesDataFilesTabWidget);
   
   //
   // Create the matrix editing page
   //
   matrixEditorPage = createMatrixEditorPage();
   matricesDataFilesTabWidget->addTab(matrixEditorPage, "Matrix Editor");

   //
   // Create the transform data file page
   //
   transformDataFilePage = createTransformDataFilePage();
   matricesDataFilesTabWidget->addTab(transformDataFilePage, "Transform Data Files");
   
   //
   // Close button
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(3);
   dialogLayout->addLayout(buttonsLayout);
   QPushButton* closeButton = new QPushButton("Close");
   buttonsLayout->addWidget(closeButton);
   closeButton->setAutoDefault(false);
   closeButton->setFixedSize(closeButton->sizeHint());
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(slotCloseDialog()));
    
   //
   // Default to transform view
   //
   matrixViewTabWidget->setCurrentWidget(matrixTransformationsWidget);
   
   if (matrixSelectionComboBox->count() > 0) {
      slotMatrixSelectionComboBox(0);
   }
   
   updateDialog();
}

/**
 * Destructor.
 */
GuiTransformationMatrixDialog::~GuiTransformationMatrixDialog()
{
}

/**
 * create the transform data file page.
 */
QWidget* 
GuiTransformationMatrixDialog::createTransformDataFilePage()      
{
   //
   // Grid for transform data files and associated matrices
   //
   QWidget* transformDataFileGrid = new QWidget;
   transformDataFileGridLayout = new QGridLayout(transformDataFileGrid);
   transformDataFileGridLayout->setSpacing(5);
   
   transformDataFileGridLayout->addWidget(new QLabel("Data File Name  "), 0, 0);
   transformDataFileGridLayout->addWidget(new QLabel("Display Transform Matrix"), 0, 1);
   
   //
   // Scroll box for models
   //
   QWidget* transformDataWidget = new QWidget;
   QVBoxLayout* transformDataLayout = new QVBoxLayout(transformDataWidget);
   QScrollArea* transformDataFileScrollView = new QScrollArea;
   transformDataFileScrollView->setWidgetResizable(true);
   transformDataFileScrollView->setWidget(transformDataFileGrid);
   transformDataLayout->addWidget(transformDataFileScrollView);
   transformDataLayout->addStretch();
   
   return transformDataWidget;
}

/**
 * create the matrix editor page.
 */
QWidget* 
GuiTransformationMatrixDialog::createMatrixEditorPage()      
{
   QWidget* editorHBox = new QWidget;
   QHBoxLayout* editorLayout = new QHBoxLayout(editorHBox);
   
   //
   // Vertical box for matrix and operations
   //
   QVBoxLayout* matrixLayout = new QVBoxLayout;
   editorLayout->addLayout(matrixLayout);
   
   //
   // Create the matrix section
   //
   matrixLayout->addWidget(createMatrixSection());
   
   //
   // Create the operations section
   //
   matrixLayout->addWidget(createOperationsSection());
   
   //
   // Create the matrix buttons column
   //
   editorLayout->addWidget(createMatrixButtonsSection());
   
   return editorHBox;
}
   
/**
 * create the matrix section.
 */
QWidget* 
GuiTransformationMatrixDialog::createMatrixSection()
{
   QGroupBox* matrixGroupBox = new QGroupBox("Matrix Data");
   QVBoxLayout* matrixGroupLayout = new QVBoxLayout(matrixGroupBox);
   
   //
   // Matrix selection combo box
   //
   matrixSelectionComboBox = new QComboBox;
   matrixGroupLayout->addWidget(matrixSelectionComboBox);
   QObject::connect(matrixSelectionComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotMatrixSelectionComboBox(int)));
                    
   //
   // Comment label
   //
   QHBoxLayout* commentHBoxLayout = new QHBoxLayout;
   matrixGroupLayout->addLayout(commentHBoxLayout);
   QLabel* commLabel = new QLabel("Comment: ");
   commentHBoxLayout->addWidget(commLabel);
   commentHBoxLayout->setStretchFactor(commLabel, 0);
   commentLabel = new QLabel(" ");
   commentLabel->setAlignment(Qt::AlignLeft);
   commentHBoxLayout->addWidget(commentLabel);
   commentHBoxLayout->setStretchFactor(commentLabel, 100);
   
   //
   // Show axes check box
   //
   showAxesCheckBox = new QCheckBox("Show Axes");
   matrixGroupLayout->addWidget(showAxesCheckBox);
   QObject::connect(showAxesCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(slotShowAxesCheckBox(bool)));
   
   //
   // Yoke to main window check box
   //
   yokeToMainWindowCheckBox = new QCheckBox("Yoke to Main Window");
   matrixGroupLayout->addWidget(yokeToMainWindowCheckBox);                    
   
   //
   // tab widget for how to view matrix
   //
   matrixViewTabWidget = new QTabWidget(matrixGroupBox);
   matrixGroupLayout->addWidget(matrixViewTabWidget);
   
   //
   // matrix element widget
   //
   matrixElementWidget = new QWidget;
   QGridLayout* matrixElementGrid = new QGridLayout(matrixElementWidget);
   matrixElementGrid->setSpacing(5);
   matrixViewTabWidget->addTab(matrixElementWidget, "Matrix View");
   
   //
   // matrix element line edits
   //
   const int minWidth = 125;
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrixElementLineEdits[i][j] = new QLineEdit;
         matrixElementLineEdits[i][j]->setFixedWidth(minWidth);
         QObject::connect(matrixElementLineEdits[i][j], SIGNAL(returnPressed()),
                          this, SLOT(slotMatrixComponentChanged()));
         matrixElementGrid->addWidget(matrixElementLineEdits[i][j], i, j);
      }
   }
   
   //
   // Squish matrix element grid widget
   //
   matrixElementWidget->setFixedSize(matrixElementWidget->sizeHint());
   
   //
   // transformation widget
   //
   matrixTransformationsWidget = new QWidget;
   QGridLayout* matrixTransformationsGrid = new QGridLayout(matrixTransformationsWidget);
   matrixTransformationsGrid->setSpacing(5);
   matrixViewTabWidget->addTab(matrixTransformationsWidget, "Transform View");
   
   //
   // Column labels
   //
   matrixTransformationsGrid->addWidget(new QLabel("X"), 0, 1, Qt::AlignHCenter);
   matrixTransformationsGrid->addWidget(new QLabel("Y"), 0, 2, Qt::AlignHCenter);
   matrixTransformationsGrid->addWidget(new QLabel("Z"), 0, 3, Qt::AlignHCenter);
   
   //
   // Translate controls
   //
   matrixTransformationsGrid->addWidget(new QLabel("Translate "), 1, 0);
   matrixTranslateXDoubleSpinBox = new QDoubleSpinBox;
   matrixTranslateXDoubleSpinBox->setMinimum(-25000.0);
   matrixTranslateXDoubleSpinBox->setMaximum( 25000.0);
   matrixTranslateXDoubleSpinBox->setSingleStep(1.0);
   matrixTranslateXDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixTranslateXDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixTranslateYDoubleSpinBox = new QDoubleSpinBox;
   matrixTranslateYDoubleSpinBox->setMinimum(-25000.0);
   matrixTranslateYDoubleSpinBox->setMaximum( 25000.0);
   matrixTranslateYDoubleSpinBox->setSingleStep(1.0);
   matrixTranslateYDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixTranslateYDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixTranslateZDoubleSpinBox = new QDoubleSpinBox;
   matrixTranslateZDoubleSpinBox->setMinimum(-25000.0);
   matrixTranslateZDoubleSpinBox->setMaximum( 25000.0);
   matrixTranslateZDoubleSpinBox->setSingleStep(1.0);
   matrixTranslateZDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixTranslateZDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixTransformationsGrid->addWidget(matrixTranslateXDoubleSpinBox, 1, 1);
   matrixTransformationsGrid->addWidget(matrixTranslateYDoubleSpinBox, 1, 2);
   matrixTransformationsGrid->addWidget(matrixTranslateZDoubleSpinBox, 1, 3);
   
   //
   // Rotate controls
   //
   matrixTransformationsGrid->addWidget(new QLabel("Rotate "), 2, 0);
   matrixRotateXDoubleSpinBox = new QDoubleSpinBox;
   matrixRotateXDoubleSpinBox->setMinimum(-360.0);
   matrixRotateXDoubleSpinBox->setMaximum(360.0);
   matrixRotateXDoubleSpinBox->setSingleStep(1.0);
   matrixRotateXDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixRotateXDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixRotateYDoubleSpinBox = new QDoubleSpinBox;
   matrixRotateYDoubleSpinBox->setMinimum(-360.0);
   matrixRotateYDoubleSpinBox->setMaximum(360.0);
   matrixRotateYDoubleSpinBox->setSingleStep(1.0);
   matrixRotateYDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixRotateYDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixRotateZDoubleSpinBox = new QDoubleSpinBox;
   matrixRotateZDoubleSpinBox->setMinimum(-360.0);
   matrixRotateZDoubleSpinBox->setMaximum(360.0);
   matrixRotateZDoubleSpinBox->setSingleStep(1.0);
   matrixRotateZDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixRotateZDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixTransformationsGrid->addWidget(matrixRotateXDoubleSpinBox, 2, 1);
   matrixTransformationsGrid->addWidget(matrixRotateYDoubleSpinBox, 2, 2);
   matrixTransformationsGrid->addWidget(matrixRotateZDoubleSpinBox, 2, 3);
   
   //
   // Scale controls
   //
   matrixTransformationsGrid->addWidget(new QLabel("Scale "), 3, 0);
   matrixScaleXDoubleSpinBox = new QDoubleSpinBox;
   matrixScaleXDoubleSpinBox->setMinimum(-25000.0);
   matrixScaleXDoubleSpinBox->setMaximum(25000.0);
   matrixScaleXDoubleSpinBox->setSingleStep(1.0);
   matrixScaleXDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixScaleXDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixScaleYDoubleSpinBox = new QDoubleSpinBox;
   matrixScaleYDoubleSpinBox->setMinimum(-25000.0);
   matrixScaleYDoubleSpinBox->setMaximum(25000.0);
   matrixScaleYDoubleSpinBox->setSingleStep(1.0);
   matrixScaleYDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixScaleYDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixScaleZDoubleSpinBox = new QDoubleSpinBox;
   matrixScaleZDoubleSpinBox->setMinimum(-25000.0);
   matrixScaleZDoubleSpinBox->setMaximum(25000.0);
   matrixScaleZDoubleSpinBox->setSingleStep(1.0);
   matrixScaleZDoubleSpinBox->setDecimals(4);
   QObject::connect(matrixScaleZDoubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SLOT(slotMatrixComponentChanged()));
   matrixTransformationsGrid->addWidget(matrixScaleXDoubleSpinBox, 3, 1);
   matrixTransformationsGrid->addWidget(matrixScaleYDoubleSpinBox, 3, 2);
   matrixTransformationsGrid->addWidget(matrixScaleZDoubleSpinBox, 3, 3);
   
   //
   // Squish matrix transformations grid widget
   //
   matrixTransformationsWidget->setFixedSize(matrixTransformationsWidget->sizeHint());
   
   return matrixGroupBox;
}

/**
 * called when the show axes check box is toggled.
 */
void 
GuiTransformationMatrixDialog::slotShowAxesCheckBox(bool val)
{
   if (currentMatrix != NULL) {
      currentMatrix->setShowAxes(val);
      GuiBrainModelOpenGL::updateAllGL();
   }
}
      
/**
 * called when a matrix component is changed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixComponentChanged()
{
   //
   // Add previous matrix to undo stack
   //
   addMatrixToUndoStack();

   transferDialogValuesIntoMatrix();
   
   updateMainWindowViewingMatrix();
   
   if (currentMatrix->getShowAxes()) {
      GuiBrainModelOpenGL::updateAllGL();
   }
}

/**
 * called when a matrix is selected.
 */
void 
GuiTransformationMatrixDialog::slotMatrixSelectionComboBox(int item)
{
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet()->getTransformationMatrixFile();
   if ((item >= 0) && (item < tmf->getNumberOfMatrices())) {
      currentMatrix = tmf->getTransformationMatrix(item);
      transferMatrixIntoDialog();
   }
}

/**
 * create the operation section.
 */
QWidget* 
GuiTransformationMatrixDialog::createOperationsSection()
{
   //
   // identity push button
   //
   QPushButton* identityPushButton = new QPushButton("Identity");
   identityPushButton->setAutoDefault(false);
   identityPushButton->setToolTip(
                 "Set the matrix to the identity matrix.");
   QObject::connect(identityPushButton, SIGNAL(clicked()),
                    this, SLOT(slotOperationIdentity()));
                    
   //
   // inverse push button
   //
   QPushButton* inversePushButton = new QPushButton("Inverse");
   inversePushButton->setAutoDefault(false);
   inversePushButton->setToolTip(
                 "Set the matrix to its inverse.");
   QObject::connect(inversePushButton, SIGNAL(clicked()),
                    this, SLOT(slotOperationInverse()));
                    
   //
   // transpose push button
   //
   QPushButton* transposePushButton = new QPushButton("Transpose");
   transposePushButton->setAutoDefault(false);
   transposePushButton->setToolTip(
                 "Transpose the matrix.");
   QObject::connect(transposePushButton, SIGNAL(clicked()),
                    this, SLOT(slotOperationTranspose()));
                    
   //
   // multiply push button
   //
   QPushButton* multiplyPushButton = new QPushButton("Multiply...");
   multiplyPushButton->setAutoDefault(false);
   multiplyPushButton->setToolTip(
                 "Multiply the matrix by another matrix.");
   QObject::connect(multiplyPushButton, SIGNAL(clicked()),
                    this, SLOT(slotOperationMultiply()));
    
   //
   // Translate screen axes
   //
   QPushButton* translateScreenAxesPushButton = new QPushButton("Translate About Screen Axes...");
   translateScreenAxesPushButton->setAutoDefault(false);
   translateScreenAxesPushButton->setToolTip(
                 "Translate with respect to screen axes.");
   QObject::connect(translateScreenAxesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotTranslateScreenAxes()));
                    
   //
   // Translate object axes
   //
   QPushButton* translatePushButton = new QPushButton("Translate About Object Axes...");
   translatePushButton->setAutoDefault(false);
   translatePushButton->setToolTip(
                 "Translate with respect to object axes.");
   QObject::connect(translatePushButton, SIGNAL(clicked()),
                    this, SLOT(slotTranslateObjectAxes()));
                    
   //
   // Rotate screen axes
   //
   QPushButton* rotateScreenAxesPushButton = new QPushButton("Rotate About Screen Axes...");
   rotateScreenAxesPushButton->setAutoDefault(false);
   rotateScreenAxesPushButton->setToolTip(
                 "Rotate with respect to screen axes.");
   QObject::connect(rotateScreenAxesPushButton, SIGNAL(clicked()),
                    this, SLOT(slotRotateScreenAxes()));
                    
   //
   // Rotate object axes
   //
   QPushButton* rotatePushButton = new QPushButton("Rotate About Object Axes...");
   rotatePushButton->setAutoDefault(false);
   rotatePushButton->setToolTip(
                 "Rotate with respect to object axes.");
   QObject::connect(rotatePushButton, SIGNAL(clicked()),
                    this, SLOT(slotRotateObjectAxes()));
                    
   //
   // Scale
   //
   QPushButton* scalePushButton = new QPushButton("Scale...");
   scalePushButton->setAutoDefault(false);
   scalePushButton->setToolTip(
                 "Apply scaling to the matrix.");
   QObject::connect(scalePushButton, SIGNAL(clicked()),
                    this, SLOT(slotScale()));
   
   //
   // Set translate with mouse
   //
   QPushButton* mouseTransButton = new QPushButton("Set Translate With Mouse");
   mouseTransButton->setAutoDefault(false);
   mouseTransButton->setToolTip(
                 "Set the mouse mode so that when\n"
                 "the mouse is clicked in the main\n"
                 "window, the matrix's translation\n"
                 "is set to the model coordinates at\n"
                 "the location of the mouse click.");
   QObject::connect(mouseTransButton, SIGNAL(clicked()),
                    this, SLOT(slotSetTranslationWithMouse()));
                    
   //
   // Undo
   //
   undoPushButton = new QPushButton("Undo");
   undoPushButton->setAutoDefault(false);
   undoPushButton->setToolTip(
                 "Restore the matrix to its\n"
                 "values previous to the last\n"
                 "operation performed on the matrix.");
   QObject::connect(undoPushButton, SIGNAL(clicked()),
                    this, SLOT(slotUndoPushButton()));
    
   //
   // Grid layout for all of the buttons
   //
   QGroupBox* operationsGroupBox = new QGroupBox("Matrix Operations");
   QGridLayout* operationsGrid = new QGridLayout(operationsGroupBox);
   operationsGrid->setMargin(4);
   operationsGrid->setSpacing(4);
   operationsGrid->addWidget(identityPushButton, 0, 0);
   operationsGrid->addWidget(inversePushButton, 0, 1);
   operationsGrid->addWidget(transposePushButton, 0, 2);
   operationsGrid->addWidget(multiplyPushButton, 0, 3);
   operationsGrid->addWidget(translateScreenAxesPushButton, 1, 0, 1, 2);
   operationsGrid->addWidget(translatePushButton, 1, 2, 1, 2);
   operationsGrid->addWidget(rotateScreenAxesPushButton, 2, 0, 1, 2);
   operationsGrid->addWidget(rotatePushButton, 2, 2, 1, 2);
   operationsGrid->addWidget(scalePushButton, 3, 0, 1, 1);
   operationsGrid->addWidget(mouseTransButton, 3, 1, 1, 2);
   operationsGrid->addWidget(undoPushButton, 3, 3, 1, 1);
   
   return operationsGroupBox;
}

/**
 * called to set translation with mouse.
 */
void 
GuiTransformationMatrixDialog::slotSetTranslationWithMouse()
{
   theMainWindow->getBrainModelOpenGL()->setMouseMode(
                  GuiBrainModelOpenGL::MOUSE_MODE_TRANSFORMATION_MATRIX_SET_TRANSLATE);
}

/**
 * set the translation values.
 */
void 
GuiTransformationMatrixDialog::setTranslation(const float tx, const float ty, const float tz)
{
   if (currentMatrix != NULL) {
      addMatrixToUndoStack();
      currentMatrix->setTranslation(tx, ty, tz);
      transferMatrixIntoDialog();
      updateMainWindowViewingMatrix();
      if (currentMatrix->getShowAxes()) {
        GuiBrainModelOpenGL::updateAllGL();
      }      
   }
}
      
/**
 * called to scale object.
 */
void 
GuiTransformationMatrixDialog::slotScale()
{
   if (currentMatrix != NULL) {
      std::vector<QString> labels;
      std::vector<QString> values;

      labels.push_back("Scale X");   values.push_back("1.0");
      labels.push_back("Scale Y");   values.push_back("1.0");
      labels.push_back("Scale Z");   values.push_back("1.0");
      QtMultipleInputDialog mid(this, 
                                "Scale",
                                "",
                                labels,
                                values,
                                true,
                                true);
      if (mid.exec() == QDialog::Accepted) {
         //
         // Add previous matrix to undo stack
         //
         addMatrixToUndoStack();

         double v[3];
         mid.getValues(v);
         transferDialogValuesIntoMatrix();
         currentMatrix->scale(v);
         transferMatrixIntoDialog();
         updateMainWindowViewingMatrix();
         if (currentMatrix->getShowAxes()) {
           GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}
      
/**
 * called to translate in screen axes.
 */
void 
GuiTransformationMatrixDialog::slotTranslateScreenAxes()
{
   if (currentMatrix != NULL) {
      std::vector<QString> labels;
      std::vector<QString> values;

      labels.push_back("Translate X");   values.push_back("0.0");
      labels.push_back("Translate Y");   values.push_back("0.0");
      labels.push_back("Translate Z");   values.push_back("0.0");
      QtMultipleInputDialog mid(this, 
                                "Translate Screen Axes",
                                "",
                                labels,
                                values,
                                true,
                                true);
      if (mid.exec() == QDialog::Accepted) {
         //
         // Add previous matrix to undo stack
         //
         addMatrixToUndoStack();
         
         double v[3];
         mid.getValues(v);
         transferDialogValuesIntoMatrix();
         if (yokeToMainWindowCheckBox->isChecked()) {
            currentMatrix->translate(v);
         }
         else {
            currentMatrix->translate(v, getMainWindowRotationTransform());
         }
         transferMatrixIntoDialog();
         updateMainWindowViewingMatrix();
         if (currentMatrix->getShowAxes()) {
           GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}

/**
 * called to translate in object axes.
 */
void 
GuiTransformationMatrixDialog::slotTranslateObjectAxes()
{
   if (currentMatrix != NULL) {
      std::vector<QString> labels;
      std::vector<QString> values;

      labels.push_back("Translate X");   values.push_back("0.0");
      labels.push_back("Translate Y");   values.push_back("0.0");
      labels.push_back("Translate Z");   values.push_back("0.0");
      QtMultipleInputDialog mid(this, 
                                "Translate Object Axes",
                                "",
                                labels,
                                values,
                                true,
                                true);
      if (mid.exec() == QDialog::Accepted) {
         //
         // Add previous matrix to undo stack
         //
         addMatrixToUndoStack();
         
         double v[3];
         mid.getValues(v);
         transferDialogValuesIntoMatrix();
         if (yokeToMainWindowCheckBox->isChecked()) {
            TransformationMatrix tm;
            tm.setMatrix(getMainWindowRotationTransform());
            vtkTransform* m = vtkTransform::New();
            tm.getMatrix(m);
            m->Inverse();
            currentMatrix->translate(v, m);
            m->Delete();
         }
         else {
            currentMatrix->translate(v, NULL);
         }
         transferMatrixIntoDialog();
         updateMainWindowViewingMatrix();
         if (currentMatrix->getShowAxes()) {
           GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}

/**
 * called to rotate in screen axes.
 */
void 
GuiTransformationMatrixDialog::slotRotateScreenAxes()
{
   if (currentMatrix != NULL) {
      std::vector<QString> labels;
      std::vector<QString> values;

      labels.push_back("Rotate X (Degrees)");   values.push_back("0.0");
      labels.push_back("Rotate Y (Degrees)");   values.push_back("0.0");
      labels.push_back("Rotate Z (Degrees)");   values.push_back("0.0");
      QtMultipleInputDialog mid(this, 
                                "Rotate Screen Axes",
                                "",
                                labels,
                                values,
                                true,
                                true);
      if (mid.exec() == QDialog::Accepted) {
         //
         // Add previous matrix to undo stack
         //
         addMatrixToUndoStack();

         double v[3];
         mid.getValues(v);
         transferDialogValuesIntoMatrix();
         currentMatrix->rotate(v, getMainWindowRotationTransform()); //NULL);
         transferMatrixIntoDialog();
         updateMainWindowViewingMatrix();
         if (currentMatrix->getShowAxes()) {
           GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}

/**
 * called to rotate in object axes.
 */
void 
GuiTransformationMatrixDialog::slotRotateObjectAxes()
{
   if (currentMatrix != NULL) {
      std::vector<QString> labels;
      std::vector<QString> values;

      labels.push_back("Rotate X (Degrees)");   values.push_back("0.0");
      labels.push_back("Rotate Y (Degrees)");   values.push_back("0.0");
      labels.push_back("Rotate Z (Degrees)");   values.push_back("0.0");
      QtMultipleInputDialog mid(this, 
                                "Rotate Object Axes",
                                "",
                                labels,
                                values,
                                true,
                                true);
      if (mid.exec() == QDialog::Accepted) {
         //
         // Add previous matrix to undo stack
         //
         addMatrixToUndoStack();

         double v[3];
         mid.getValues(v);
         transferDialogValuesIntoMatrix();
         //currentMatrix->rotate(v, getMainWindowRotationTransform());
         currentMatrix->rotateZ(v[2]);
         currentMatrix->rotateX(v[0]);
         currentMatrix->rotateY(v[1]);
         transferMatrixIntoDialog();
         updateMainWindowViewingMatrix();
         if (currentMatrix->getShowAxes()) {
           GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
}

/**
 * called to set the matrix to the identity matrix.
 */
void 
GuiTransformationMatrixDialog::slotOperationIdentity()
{
   if (currentMatrix != NULL) {
      //
      // Add previous matrix to undo stack
      //
      addMatrixToUndoStack();

      transferDialogValuesIntoMatrix();
      currentMatrix->identity();
      transferMatrixIntoDialog();
      updateMainWindowViewingMatrix();
      if (currentMatrix->getShowAxes()) {
        GuiBrainModelOpenGL::updateAllGL();
      }
   }
}

/**
 * called to set the matrix to its inverse.
 */
void 
GuiTransformationMatrixDialog::slotOperationInverse()
{
   if (currentMatrix != NULL) {
      //
      // Add previous matrix to undo stack
      //
      addMatrixToUndoStack();

      transferDialogValuesIntoMatrix();
      currentMatrix->inverse();
      transferMatrixIntoDialog();
      updateMainWindowViewingMatrix();
      if (currentMatrix->getShowAxes()) {
         GuiBrainModelOpenGL::updateAllGL();
      }
   }
}

/**
 * called to transpose the matrix.
 */
void 
GuiTransformationMatrixDialog::slotOperationTranspose()
{
   if (currentMatrix != NULL) {
      //
      // Add previous matrix to undo stack
      //
      addMatrixToUndoStack();

      transferDialogValuesIntoMatrix();
      currentMatrix->transpose();
      transferMatrixIntoDialog();
      updateMainWindowViewingMatrix();
      if (currentMatrix->getShowAxes()) {
         GuiBrainModelOpenGL::updateAllGL();
      }
   }
}

/**
 * called to multiply the matrix by another matrix.
 */
void 
GuiTransformationMatrixDialog::slotOperationMultiply()
{
   if (currentMatrix != NULL) {
      //
      // Add previous matrix to undo stack
      //
      addMatrixToUndoStack();

      transferDialogValuesIntoMatrix();
      TransformationMatrixFile* matrixFile = theMainWindow->getBrainSet()->getTransformationMatrixFile();

      std::vector<QString> matrixNames;
      for (int i = 0; i < matrixFile->getNumberOfMatrices(); i++) {
         TransformationMatrix* tm = matrixFile->getTransformationMatrix(i);
         matrixNames.push_back(tm->getMatrixName());
      }
      QtRadioButtonSelectionDialog rbsd(this,
                                        "Multiply by Matrix",
                                        "",
                                        matrixNames,
                                        -1);
      if (rbsd.exec() == QDialog::Accepted) {
         const int matrixNumber = rbsd.getSelectedItemIndex();
         if (matrixNumber >= 0) {
            TransformationMatrix tm = *(matrixFile->getTransformationMatrix(matrixNumber));
            currentMatrix->multiply(tm);
            transferMatrixIntoDialog();
            updateMainWindowViewingMatrix();
            if (currentMatrix->getShowAxes()) {
               GuiBrainModelOpenGL::updateAllGL();
            }
         }
      }
   }
}

/**
 * update the display if the matrix is being manipulated.
 */
void 
GuiTransformationMatrixDialog::updateMatrixDisplay(const TransformationMatrix* tm)
{
   //
   // If current matrix yoked to main window viewing matrix
   //
   if (yokeToMainWindowCheckBox->isChecked() &&
       (tm == NULL)) {
      DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
      currentMatrix->identity();
      float trans[3];
      BrainModel* bm = theMainWindow->getBrainModel();
      bm->getTranslation(0, trans);
      currentMatrix->translate(trans);
      TransformationMatrix rot;
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         switch (bmv->getSelectedAxis(0)) {
            case VolumeFile::VOLUME_AXIS_X:
               rot.rotateX(-bmv->getDisplayRotation(0));
               break;
            case VolumeFile::VOLUME_AXIS_Y:
               rot.rotateY(bmv->getDisplayRotation(0));
               break;
            case VolumeFile::VOLUME_AXIS_Z:
               rot.rotateZ(-bmv->getDisplayRotation(0));
               break;
            case VolumeFile::VOLUME_AXIS_ALL:
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
               rot.setMatrix(bmv->getObliqueRotationMatrix());
               rot.transpose();
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
               {
                  TransformationMatrix* tmv = dsv->getObliqueSlicesTransformationMatrix();
                  if (tmv != NULL) {
                     rot = *tmv;
                  }
                  else {
                     rot.setMatrix(bmv->getObliqueRotationMatrix());
                     rot.transpose();
                  }
                  //int sliceOffsets[3];
                  //bmv->getSelectedObliqueSliceOffsets(0, sliceOffsets);
                  //rot.translate(sliceOffsets[0], sliceOffsets[1], sliceOffsets[2]);
               }
               break;
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
         float scale[3];
         bm->getScaling(0, scale);
         for (int j = 0; j < 3; j++) {
            if (scale[j] == 0.0) {
               scale[j] = 1.0;
            }
            scale[j] = 1.0 / scale[j];
         }
         //rot.scale(scale);
      }
      else {
         rot.setMatrix(bm->getRotationTransformMatrix(0));
         rot.transpose();
      }
      currentMatrix->multiply(rot);
      float scale[3];
      bm->getScaling(0, scale);
      currentMatrix->scale(scale);
      updateDialog();
      
      return;
   }
   
   //
   // If updating axes
   //
   if (showAxesCheckBox->isChecked()) {
      if (currentMatrix == tm) {
         if (currentMatrix->getShowAxes()) {
            transferMatrixIntoDialog();
         }
      }
   }
}      
      
/**
 * update the dialog.
 */
void
GuiTransformationMatrixDialog::updateDialog()
{
   TransformationMatrix* previousMatrix = currentMatrix;
   
   matrixSelectionComboBox->clear();
   
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet()->getTransformationMatrixFile();
   int numMatrices = tmf->getNumberOfMatrices();
   if (numMatrices <= 0) {
      TransformationMatrix tm;
      tmf->addTransformationMatrix(tm);
      numMatrices = tmf->getNumberOfMatrices();
   }
   
   if (numMatrices > 0) {
      int defaultItem = 0;
      for (int i = 0; i < numMatrices; i++) {
         const TransformationMatrix* tm = tmf->getTransformationMatrix(i);
         matrixSelectionComboBox->addItem(tm->getMatrixName());
         if (currentMatrix == tm) {
            defaultItem = i;
         }
      }
      
      matrixSelectionComboBox->setCurrentIndex(defaultItem);
      if ((defaultItem >= 0) && (defaultItem < tmf->getNumberOfMatrices())) {
         currentMatrix = tmf->getTransformationMatrix(defaultItem);
      }
   }
   else {
      currentMatrix = NULL;
   }
   
   transferMatrixIntoDialog();
   
   if (currentMatrix != previousMatrix) {
      clearUndoStack();
   }
   
   //
   // Update the transform data files section
   //
   const int numTransformDataFiles = theMainWindow->getBrainSet()->getNumberOfTransformationDataFiles();
   const int numExistingTransformDataFiles = static_cast<int>(transformFileLabels.size());
   
   //
   // Add items as needed
   //
   for (int i = numExistingTransformDataFiles; i < numTransformDataFiles; i++) {
      QLabel* label = new QLabel("");
      transformFileLabels.push_back(label);
      GuiTransformationMatrixSelectionControl* msc =
         new GuiTransformationMatrixSelectionControl(0,
                                                     theMainWindow->getBrainSet()->getTransformationMatrixFile(),
                                                     true);
      msc->setNoneLabel("Do Not Display");
      msc->setToolTip(
                    "Use this control to choose the matrix\n"
                    "that is used to display this transform\n"
                    "data file.");
                    
      QObject::connect(msc, SIGNAL(activated(int)),
                       this, SLOT(slotTransformMatrixSelection()));
      transformFileMatrixControls.push_back(msc);
      transformDataFileGridLayout->addWidget(label, i + 1, 0);
      transformDataFileGridLayout->addWidget(msc, i + 1, 1);
   }
   
   //
   // Update items 
   //
   for (int i = 0; i < numTransformDataFiles; i++) {
      AbstractFile* tdf = theMainWindow->getBrainSet()->getTransformationDataFile(i);
      transformFileLabels[i]->setText(FileUtilities::basename(tdf->getFileName()));
      transformFileLabels[i]->show();
      transformFileMatrixControls[i]->blockSignals(true);
      transformFileMatrixControls[i]->setSelectedMatrixIndex(
         tmf->getMatrixIndex(tdf->getAssociatedTransformationMatrix()));
      transformFileMatrixControls[i]->blockSignals(false);
      transformFileMatrixControls[i]->show();
      transformFileMatrixControls[i]->updateControl();
   }
   
   //
   // Hide unneeded items
   //
   for (int i = numTransformDataFiles; i < numExistingTransformDataFiles; i++) {
      transformFileLabels[i]->hide();
      transformFileMatrixControls[i]->hide();
   }
}

/**
 * called when a matrix is selected for a data file.
 */
void 
GuiTransformationMatrixDialog::slotTransformMatrixSelection()
{
   const int numTransformDataFiles = theMainWindow->getBrainSet()->getNumberOfTransformationDataFiles();
   const int numExistingTransformDataFiles = static_cast<int>(transformFileLabels.size());
   if (numTransformDataFiles != numExistingTransformDataFiles) {
      std::cout << "PROGRAM ERROR: Number of transform data files does not match those in dialog." << std::endl;
      return;
   }
   
   for (int i = 0; i < numTransformDataFiles; i++) {
      AbstractFile* af = theMainWindow->getBrainSet()->getTransformationDataFile(i);
      af->setAssociatedTransformationMatrix(transformFileMatrixControls[i]->getSelectedMatrix());         
   }
   GuiBrainModelOpenGL::updateAllGL();
}
      
/**
 * get the matrix view type.
 */
GuiTransformationMatrixDialog::MATRIX_VIEW_TYPE 
GuiTransformationMatrixDialog::getMatrixViewType() const
{
   return static_cast<MATRIX_VIEW_TYPE>(matrixViewTabWidget->currentIndex());
}
      
/**
 * transfer dialog values into matrix.
 */
void 
GuiTransformationMatrixDialog::transferDialogValuesIntoMatrix()
{
   if (currentMatrix != NULL) {
      switch (getMatrixViewType()) {
         case MATRIX_VIEW_MATRIX:
            for (int i = 0; i < 4; i++) {
               for (int j = 0; j < 4; j++) {
                  currentMatrix->setMatrixElement(i, j, 
                     matrixElementLineEdits[i][j]->text().toDouble());
               }
            }
            transferMatrixIntoMatrixTransformSpinBoxes();
            break;
         case MATRIX_VIEW_TRANSFORMATIONS:
            currentMatrix->identity();
            currentMatrix->translate(matrixTranslateXDoubleSpinBox->value(),
                                     matrixTranslateYDoubleSpinBox->value(),
                                     matrixTranslateZDoubleSpinBox->value());
            const double trans[3] = {
               matrixTranslateXDoubleSpinBox->value(),
               matrixTranslateYDoubleSpinBox->value(),
               matrixTranslateZDoubleSpinBox->value() 
            };
            const double rot[3] = {
               matrixRotateXDoubleSpinBox->value(),
               matrixRotateYDoubleSpinBox->value(),
               matrixRotateZDoubleSpinBox->value()
            };
            const double scalem[3] = {
               matrixScaleXDoubleSpinBox->value(),
               matrixScaleYDoubleSpinBox->value(),
               matrixScaleZDoubleSpinBox->value()
            };
            currentMatrix->setMatrix(trans, rot, scalem);
            transferMatrixIntoMatrixElementLineEdits();
            break;
      }
   }
}

/**
 * transfer the matrix into the matrix element part of dialog.
 */
void 
GuiTransformationMatrixDialog::transferMatrixIntoMatrixElementLineEdits()
{
   if (currentMatrix != NULL) {
      for (int i = 0; i < 4; i++) {
         for (int j = 0; j < 4; j++) {
            matrixElementLineEdits[i][j]->blockSignals(true);
            matrixElementLineEdits[i][j]->setText(
               QString::number(currentMatrix->getMatrixElement(i, j), 'f', 6));
            matrixElementLineEdits[i][j]->blockSignals(false);
         }
      }
   }
}

/**
 * transfer the matrix into the matrix transformation spin boxes.
 */
void 
GuiTransformationMatrixDialog::transferMatrixIntoMatrixTransformSpinBoxes()
{
   if (currentMatrix != NULL) {
      double tx, ty, tz;
      currentMatrix->getTranslation(tx, ty, tz);
      
      matrixTranslateXDoubleSpinBox->blockSignals(true);
      matrixTranslateXDoubleSpinBox->setValue(tx);
      matrixTranslateXDoubleSpinBox->blockSignals(false);
      
      matrixTranslateYDoubleSpinBox->blockSignals(true);
      matrixTranslateYDoubleSpinBox->setValue(ty);
      matrixTranslateYDoubleSpinBox->blockSignals(false);
      
      matrixTranslateZDoubleSpinBox->blockSignals(true);
      matrixTranslateZDoubleSpinBox->setValue(tz);
      matrixTranslateZDoubleSpinBox->blockSignals(false);
      
      double rx, ry, rz;
      currentMatrix->getRotationAngles(rx, ry, rz);
      
      matrixRotateXDoubleSpinBox->blockSignals(true);
      matrixRotateXDoubleSpinBox->setValue(rx);
      matrixRotateXDoubleSpinBox->blockSignals(false);
      
      matrixRotateYDoubleSpinBox->blockSignals(true);
      matrixRotateYDoubleSpinBox->setValue(ry);
      matrixRotateYDoubleSpinBox->blockSignals(false);
      
      matrixRotateZDoubleSpinBox->blockSignals(true);
      matrixRotateZDoubleSpinBox->setValue(rz);
      matrixRotateZDoubleSpinBox->blockSignals(false);
      
      double sx, sy, sz;
      currentMatrix->getScaling(sx, sy, sz);
      
      matrixScaleXDoubleSpinBox->blockSignals(true);
      matrixScaleXDoubleSpinBox->setValue(sx);
      matrixScaleXDoubleSpinBox->blockSignals(false);
      
      matrixScaleYDoubleSpinBox->blockSignals(true);
      matrixScaleYDoubleSpinBox->setValue(sy);
      matrixScaleYDoubleSpinBox->blockSignals(false);
      
      matrixScaleZDoubleSpinBox->blockSignals(true);
      matrixScaleZDoubleSpinBox->setValue(sz);
      matrixScaleZDoubleSpinBox->blockSignals(false);
   }
}      
      
/**
 * transfer the matrix into the dialog.
 */
void 
GuiTransformationMatrixDialog::transferMatrixIntoDialog()
{
   if (currentMatrix != NULL) {
      showAxesCheckBox->setChecked(currentMatrix->getShowAxes());
      commentLabel->setText(currentMatrix->getMatrixComment());
      transferMatrixIntoMatrixElementLineEdits();
      transferMatrixIntoMatrixTransformSpinBoxes();
      showAxesCheckBox->blockSignals(true);
      showAxesCheckBox->setChecked(currentMatrix->getShowAxes());
      showAxesCheckBox->blockSignals(false);
   }
}      

/**
 * called to close the dialog.
 */
void
GuiTransformationMatrixDialog::slotCloseDialog()
{
      QDialog::close();
}

/**
 * create the operation section.
 */
QWidget* 
GuiTransformationMatrixDialog::createMatrixButtonsSection()
{
   //
   // apply button
   //
   QPushButton* applyMatrixMainWindowButton = new QPushButton("Apply Matrix to\n"
                                                              "Main Window...");
   applyMatrixMainWindowButton->setAutoDefault(false);
   QObject::connect(applyMatrixMainWindowButton, SIGNAL(clicked()),
                    this, SLOT(slotMatrixApplyMainWindow()));
                    
   //
   // apply button
   //
   QPushButton* applyMatrixFileButton = new QPushButton("Apply Matrix to\n"
                                                        "Transform Data File...");
   applyMatrixFileButton->setAutoDefault(false);
   QObject::connect(applyMatrixFileButton, SIGNAL(clicked()),
                    this, SLOT(slotMatrixApplyTransformDataFile()));
                    
   //
   // load button
   //
   QPushButton* loadMatrixButton = new QPushButton("Load Matrix...");
   loadMatrixButton->setAutoDefault(false);
   loadMatrixButton->hide();
   QObject::connect(loadMatrixButton, SIGNAL(clicked()),
                    this, SLOT(slotMatrixLoad()));
                    
   //
   // new matrix button
   //
   QPushButton* newMatrixButton = new QPushButton("New Matrix...");
   newMatrixButton->setAutoDefault(false);
   QObject::connect(newMatrixButton, SIGNAL(clicked()),
                    this, SLOT(slotMatrixNew()));
                    
   //
   // delete matrix button
   //
   QPushButton* deleteMatrixButton = new QPushButton("Delete Matrix...");
   deleteMatrixButton->setAutoDefault(false);
   QObject::connect(deleteMatrixButton, SIGNAL(clicked()),
                    this, SLOT(slotMatrixDelete()));
                    
   //
   // copy matrix button
   //
   //QPushButton* copyMatrixButton = new QPushButton("Copy Matrix",
   //                                      matrixButtonsGroupBox,
   //                                      "copyMatrixButton");
   //copyMatrixButton->setAutoDefault(false);
   //QObject::connect(copyMatrixButton, SIGNAL(clicked()),
   //                 this, SLOT(slotMatrixCopy()));
                    
   //
   // paste matrix button
   //
   //QPushButton* pasteMatrixButton = new QPushButton("Paste Matrix",
   //                                      matrixButtonsGroupBox,
   //                                      "pasteMatrixButton");
   //pasteMatrixButton->setAutoDefault(false);
   //QObject::connect(pasteMatrixButton, SIGNAL(clicked()),
   //                 this, SLOT(slotMatrixPaste()));
                    
   //
   // attributes button
   //
   QPushButton* matrixAttributesButton = new QPushButton("Attributes...");
   matrixAttributesButton->setAutoDefault(false);
   QObject::connect(matrixAttributesButton, SIGNAL(clicked()),
                    this, SLOT(slotMatrixAttributes()));
                    
   //
   // Group box and layout
   //
   QGroupBox* matrixButtonsGroupBox = new QGroupBox("File Operations");
   QVBoxLayout* matrixButtonsLayout = new QVBoxLayout(matrixButtonsGroupBox);
   matrixButtonsLayout->addWidget(applyMatrixMainWindowButton);
   matrixButtonsLayout->addWidget(applyMatrixFileButton);
   matrixButtonsLayout->addWidget(loadMatrixButton);
   matrixButtonsLayout->addWidget(newMatrixButton);
   matrixButtonsLayout->addWidget(deleteMatrixButton);
   matrixButtonsLayout->addWidget(matrixAttributesButton);
   matrixButtonsLayout->addStretch();
   
   return matrixButtonsGroupBox;
}

/**
 * called when new matrix button pressed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixNew()
{
   TransformationMatrixFile* tmf = theMainWindow->getBrainSet()->getTransformationMatrixFile();
   std::ostringstream str;
   str << "Matrix "
       << TransformationMatrix::getMatrixNumberCounter();
       
   std::vector<QString> labels, labelValues;
   labels.push_back("Matrix Name");     labelValues.push_back(str.str().c_str());
   labels.push_back("Matrix Comment");  labelValues.push_back("");

   QtMultipleInputDialog mid(this,
                             "New Matrix",
                             "",
                             labels, 
                             labelValues,
                             true,
                             true);
   if (mid.exec() == QDialog::Accepted) {
      std::vector<QString> values;
      mid.getValues(values);
      TransformationMatrix tm;
      tm.setMatrixName(values[0]);
      tm.setMatrixComment(values[1]);
      tmf->addTransformationMatrix(tm);
      TransformationMatrix* oldMatrix = currentMatrix;
      double oldMatrixComponents[4][4];
      if (oldMatrix != NULL) {
         oldMatrix->getMatrix(oldMatrixComponents);
      }
      currentMatrix = tmf->getTransformationMatrix(tmf->getNumberOfMatrices() - 1);
      currentMatrix->identity();
      
      //
      // Choose matrix default values
      //
      std::vector<QString> itemLabels;
      itemLabels.push_back("Identity Matrix");
      int currentMatrixIndex = -1;
      if (oldMatrix != NULL) {
         currentMatrixIndex = static_cast<int>(itemLabels.size());
         itemLabels.push_back("Use Current Matrix in Matrix Editor");
      }
      
      int surfaceIndex = -1;
      float surfacePos[3];
      BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
      if (bms != NULL) {
         if (GuiBrainModelOpenGL::getMainWindowCenterModelCoordinate(surfacePos)) {
            surfaceIndex = static_cast<int>(itemLabels.size());
            itemLabels.push_back("Point at Center of Surface View");
         }
      }
      
      int viewMatrixTranslateIndex = -1;
      BrainModel* bm = theMainWindow->getBrainModel();
      if (bm != NULL) {
         viewMatrixTranslateIndex = static_cast<int>(itemLabels.size());
         itemLabels.push_back("Main Window Viewing Transformation Matrix (Translate Only)");
      }
      int viewMatrixIndex = -1;
      if (bm != NULL) {
         viewMatrixIndex = static_cast<int>(itemLabels.size());
         itemLabels.push_back("Main Window Viewing Transformation Matrix (Translate and Rotate)");
      }
      
      int volumeIndex = -1;
      float volumePos[3];
      BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
      if (bmv != NULL) {
         DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
         switch (bmv->getSelectedAxis(0)) {
            case VolumeFile::VOLUME_AXIS_X:
            case VolumeFile::VOLUME_AXIS_Y:
            case VolumeFile::VOLUME_AXIS_Z:
            case VolumeFile::VOLUME_AXIS_ALL:
               {
                  int slices[3];
                  bmv->getSelectedOrthogonalSlices(0, slices);
                  VolumeFile* vf = bmv->getMasterVolumeFile();
                  if (vf != NULL) {
                     float origin[3], spacing[3];
                     vf->getOrigin(origin);
                     vf->getSpacing(spacing);
                     volumePos[0] = origin[0] + spacing[0] * slices[0];
                     volumePos[1] = origin[1] + spacing[1] * slices[1];
                     volumePos[2] = origin[2] + spacing[2] * slices[2];
                     volumeIndex = static_cast<int>(itemLabels.size());
                     itemLabels.push_back("Volume Crosshairs");
                  }
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE:
               {
                  int slices[3];
                  bmv->getSelectedObliqueSlices(slices);
                  VolumeFile* vf = bmv->getMasterVolumeFile();
                  if (vf != NULL) {
                     float origin[3], spacing[3];
                     vf->getOrigin(origin);
                     vf->getSpacing(spacing);
                     volumePos[0] = origin[0] + spacing[0] * slices[0];
                     volumePos[1] = origin[1] + spacing[1] * slices[1];
                     volumePos[2] = origin[2] + spacing[2] * slices[2];
                     volumeIndex = static_cast<int>(itemLabels.size());
                     itemLabels.push_back("Volume Crosshairs");
                  }
               }
               break;
            case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
            case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
               {
                  int slices[3];
                  const TransformationMatrix* obtm = dsv->getObliqueSlicesTransformationMatrix();
                  if (obtm != NULL) {
                     float t[3];
                     obtm->getTranslation(t[0], t[1], t[2]);
                     slices[0] = static_cast<int>(t[0]);
                     slices[1] = static_cast<int>(t[1]);
                     slices[2] = static_cast<int>(t[2]);
                  }
                  else {
                     //int slices[3];
                     bmv->getSelectedObliqueSlices(slices);
                  }
                  int sliceOffsets[3];
                  bmv->getSelectedObliqueSliceOffsets(0, sliceOffsets);
                  VolumeFile* vf = bmv->getMasterVolumeFile();
                  if (vf != NULL) {
                     float origin[3], spacing[3];
                     vf->getOrigin(origin);
                     vf->getSpacing(spacing);
                     volumePos[0] = origin[0] + spacing[0] * slices[0];
                     volumePos[1] = origin[1] + spacing[1] * slices[1];
                     volumePos[2] = origin[2] + spacing[2] * slices[2];
                     volumePos[0] += spacing[0] * sliceOffsets[0];
                     volumePos[1] += spacing[1] * sliceOffsets[1];
                     volumePos[2] += spacing[2] * sliceOffsets[2];
                     volumeIndex = static_cast<int>(itemLabels.size());
                     itemLabels.push_back("Volume Crosshairs");
                  }
               }
               break;
            case VolumeFile::VOLUME_AXIS_UNKNOWN:
               break;
         }
      }
      QtRadioButtonSelectionDialog rbd(&mid,
                                       "Initial Matrix Values",
                                       "",
                                       itemLabels,
                                       0);
      if (rbd.exec() == QDialog::Accepted) {
         if (rbd.getSelectedItemIndex() > 0) {
            if (rbd.getSelectedItemIndex() == currentMatrixIndex) {
               if (oldMatrix != NULL) {
                  currentMatrix->setMatrix(oldMatrixComponents);
               }
            }
            else if (rbd.getSelectedItemIndex() == surfaceIndex) {
               currentMatrix->identity();
               currentMatrix->translate(surfacePos);
            }
            else if (rbd.getSelectedItemIndex() == volumeIndex) {
               currentMatrix->identity();
               currentMatrix->translate(volumePos);
            }
            else if ((rbd.getSelectedItemIndex() == viewMatrixIndex) ||
                     (rbd.getSelectedItemIndex() == viewMatrixTranslateIndex)) {
               VolumeFile* vf = bmv->getMasterVolumeFile();
               int obliqueSlices[3];
               bmv->getSelectedObliqueSlices(obliqueSlices);
               float obliqueSlicesPos[3];
               vf->getVoxelCoordinate(obliqueSlices, true, obliqueSlicesPos);
               
               int obliqueSliceOffsets[3];
               bmv->getSelectedObliqueSliceOffsets(0, obliqueSliceOffsets);
               currentMatrix->identity();
               float trans[3];
               bm->getTranslation(0, trans);
               currentMatrix->translate(trans);
               TransformationMatrix rot;
               if (dynamic_cast<BrainModelVolume*>(bm) != NULL) {
                  switch (bmv->getSelectedAxis(0)) {
                     case VolumeFile::VOLUME_AXIS_X:
                        rot.rotateX(-bmv->getDisplayRotation(0));
                        break;
                     case VolumeFile::VOLUME_AXIS_Y:
                        rot.rotateY(bmv->getDisplayRotation(0));
                        break;
                     case VolumeFile::VOLUME_AXIS_Z:
                        rot.rotateZ(-bmv->getDisplayRotation(0));
                        break;
                     case VolumeFile::VOLUME_AXIS_ALL:
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE:
                        currentMatrix->translate(obliqueSlicesPos[0], 
                                                 obliqueSlicesPos[1], 
                                                 obliqueSlicesPos[2]);
                        rot.setMatrix(bmv->getObliqueRotationMatrix());
                        rot.transpose();
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
                        currentMatrix->translate(obliqueSlicesPos[0], 
                                                 obliqueSlicesPos[1], 
                                                 obliqueSlicesPos[2]);
                        rot.setMatrix(bmv->getObliqueRotationMatrix());
                        rot.rotateX(90.0);
                        rot.transpose();
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
                        currentMatrix->translate(obliqueSlicesPos[0], 
                                                 obliqueSlicesPos[1], 
                                                 obliqueSlicesPos[2]);
                        rot.setMatrix(bmv->getObliqueRotationMatrix());
                        rot.rotateY(90.0);
                        rot.transpose();
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
                        currentMatrix->translate(obliqueSlicesPos[0], 
                                                 obliqueSlicesPos[1], 
                                                 obliqueSlicesPos[2]);
                        rot.setMatrix(bmv->getObliqueRotationMatrix());
                        rot.transpose();
                        break;
                     case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                        break;
                     case VolumeFile::VOLUME_AXIS_UNKNOWN:
                        break;
                  }
                  float scale[3];
                  bm->getScaling(0, scale);
                  for (int j = 0; j < 3; j++) {
                     if (scale[j] == 0.0) {
                        scale[j] = 1.0;
                     }
                     scale[j] = 1.0 / scale[j];
                  }
                  
                  if (rbd.getSelectedItemIndex() == viewMatrixTranslateIndex) {
                     //
                     // Translate only so eliminate rotation
                     //
                     rot.identity();
                  }
                  rot.scale(scale);
               }
               else {
                  rot.setMatrix(bm->getRotationTransformMatrix(0));
                  rot.transpose();
               }
               currentMatrix->multiply(rot);
            }
         }
      }
      
      updateDialog();
      GuiFilesModified fm;
      fm.setTransformationMatrixModified();
      theMainWindow->fileModificationUpdate(fm);
      updateMainWindowViewingMatrix();
   }
}

/**
 * called when delete matrix button pressed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixDelete()
{
   if (currentMatrix != NULL) {
      if (QMessageBox::warning(this, "Confirm",
          "Are you sure you want to delete the current matrix ?",
          (QMessageBox::Yes | QMessageBox::No),
          QMessageBox::Yes)
             == QMessageBox::Yes) {
         TransformationMatrixFile* tmf = theMainWindow->getBrainSet()->getTransformationMatrixFile();
         const int numMatrices = tmf->getNumberOfMatrices();
         for (int i = 0; i < numMatrices; i++) {
            if (tmf->getTransformationMatrix(i) == currentMatrix) {
               tmf->deleteMatrix(i);
               bool axesOn = false;
               if (currentMatrix != NULL) {
                  axesOn = currentMatrix->getShowAxes();
               }
               currentMatrix = NULL;
               updateDialog();
               GuiFilesModified fm;
               fm.setTransformationMatrixModified();
               theMainWindow->fileModificationUpdate(fm);
               GuiBrainModelOpenGL::updateAllGL();
               break;
            }
         }
      }
   }
}

/**
 * called when matrix attributes button pressed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixAttributes()
{
   if (currentMatrix != NULL) {
      std::vector<QString> labels, labelValues;
      labels.push_back("Matrix Name");
      labels.push_back("Matrix Comment");
      labelValues.push_back(currentMatrix->getMatrixName());
      labelValues.push_back(currentMatrix->getMatrixComment());

      QtMultipleInputDialog mid(this,
                                "Matrix Attributes",
                                "",
                                labels, 
                                labelValues,
                                true,
                                true);
      if (mid.exec() == QDialog::Accepted) {
         std::vector<QString> values;
         mid.getValues(values);
         currentMatrix->setMatrixName(values[0]);
         currentMatrix->setMatrixComment(values[1]);
         updateDialog();
      }
   }
}

/**
 * called when copy matrix button pressed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixCopy()
{
}

/**
 * called when paste matrix button pressed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixPaste()
{
}

/**
 * called when apply matrix to transform data file button pressed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixApplyTransformDataFile()
{
   transferDialogValuesIntoMatrix();

   const int num = theMainWindow->getBrainSet()->getNumberOfTransformationDataFiles();
   
   std::vector<QString> labels;
   for (int i = 0; i < num; i++) {
      const AbstractFile* af = theMainWindow->getBrainSet()->getTransformationDataFile(i);
      labels.push_back(FileUtilities::basename(af->getFileName()));
   }
   std::vector<bool> initChecks(labels.size(), false);
   QtCheckBoxSelectionDialog cbsd(this,
                                  "Apply Transformation",
                                  "",
                                  labels,
                                  initChecks);
   if (cbsd.exec() == QDialog::Accepted) {
    
      bool ok = false;
      QString commentString = QInputDialog::getText(this, "Enter Comment Information",
                                                    "Enter Comment Describing Transform",
                                                    QLineEdit::Normal,
                                                    "",
                                                    &ok);
      QString operationComment;
      if (ok) {
         if (commentString.isEmpty() == false) {
            operationComment.append("\n");
            operationComment.append(commentString);
         }
      }
      else {
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      bool addedCells = false;
      bool addedContours = false;
      bool addedContourCells = false;
      
      bool addedFoci = false;
      bool addedVtkModelFile = false;
      for (int i = 0; i < num; i++) {
         if (cbsd.getCheckBoxStatus(i)) {
            AbstractFile* af = theMainWindow->getBrainSet()->getTransformationDataFile(i);
            
            CellFile* cf = dynamic_cast<CellFile*>(af);
            if ((cf != NULL) && 
                (dynamic_cast<FociFile*>(af) == NULL) && 
                (dynamic_cast<ContourCellFile*>(af) == NULL)) {
               const int numCells = cf->getNumberOfCells();
               if (numCells > 0) {
                  addedCells = true;
         
                  CellFile newCellFile;
                  for (int j = 0; j < numCells; j++) {
                     CellData* cd = cf->getCell(j);
                     float xyz[3];
                     cd->getXYZ(xyz);
                     currentMatrix->multiplyPoint(xyz);
                     CellData newCell(cd->getName(), xyz[0], xyz[1], xyz[2]);
                     newCellFile.addCell(newCell);
                  }
                  
                  newCellFile.setFileComment(operationComment);
                  
                  theMainWindow->getBrainSet()->getCellProjectionFile()->append(newCellFile);
               }
            }

            FociFile* ff = dynamic_cast<FociFile*>(af);
            if (ff != NULL) {
               const int numCells = ff->getNumberOfCells();
               if (numCells > 0) {
                  addedFoci = true;
         
                  FociFile newFociFile;
                  for (int j = 0; j < numCells; j++) {
                     CellData* cd = ff->getCell(j);
                     float xyz[3];
                     cd->getXYZ(xyz);
                     currentMatrix->multiplyPoint(xyz);
                     CellData newCell(cd->getName(), xyz[0], xyz[1], xyz[2]);
                     newFociFile.addCell(newCell);
                  }
                  
                  newFociFile.setFileComment(operationComment);
                  
                  theMainWindow->getBrainSet()->getFociProjectionFile()->append(newFociFile);
               }
            }
            
            ContourFile* crf = dynamic_cast<ContourFile*>(af);
            if (crf != NULL) {
               const int numContours = crf->getNumberOfContours();
               if (numContours > 0) {
                  addedContours = true;
                  BrainModelContours* bmc = theMainWindow->getBrainSet()->getBrainModelContours(-1);
                  if (bmc == NULL) {
                     bmc = new BrainModelContours(theMainWindow->getBrainSet());
                     theMainWindow->getBrainSet()->addBrainModel(bmc);
                  }
                  ContourFile* contourFile = bmc->getContourFile();
                  contourFile->setSectionSpacing(crf->getSectionSpacing());
                  
                  for (int j = 0; j < numContours; j++) {
                     CaretContour* cc = crf->getContour(j);
                     CaretContour caretContour;
                     caretContour.setSectionNumber(cc->getSectionNumber());
                     
                     for (int k = 0; k < cc->getNumberOfPoints(); k++) {
                        float xyz[3];
                        cc->getPointXYZ(k, xyz[0], xyz[1], xyz[2]);  
                        currentMatrix->multiplyPoint(xyz);                      
                        caretContour.addPoint(xyz[0], xyz[1], xyz[2]);
                     }
                     
                     if (caretContour.getNumberOfPoints() > 0) {
                        contourFile->addContour(caretContour);
                     }
                  }
               }
            }
            
            ContourCellFile* ccf = dynamic_cast<ContourCellFile*>(af);
            if (ccf != NULL) {
               const int numCells = ccf->getNumberOfCells();
               if (numCells > 0) {
                  addedContourCells = true;
                  ContourCellFile* contourCells = theMainWindow->getBrainSet()->getContourCellFile();
         
                  for (int j = 0; j < numCells; j++) {
                     CellData* cd = ccf->getCell(j);
                     float xyz[3];
                     cd->getXYZ(xyz);
                     currentMatrix->multiplyPoint(xyz);
                     CellData newCell(cd->getName(), xyz[0], xyz[1], xyz[2]);
                     newCell.setSectionNumber(static_cast<int>(xyz[2] + 0.5));
                     contourCells->addCell(newCell);
                  }
                  
                  contourCells->appendToFileComment(operationComment);
               }
            }
            
            VtkModelFile* vmf = dynamic_cast<VtkModelFile*>(af);
            if (vmf != NULL) {
               VtkModelFile* copyVMF = new VtkModelFile(*vmf);
               if (copyVMF != NULL) {
                  copyVMF->applyTransformationMatrix(*currentMatrix);
                  copyVMF->appendToFileComment(operationComment);
                  copyVMF->setDisplayFlag(true);
                  theMainWindow->getBrainSet()->addVtkModelFile(copyVMF);     
                  addedVtkModelFile = true;             
               }
            }
         }
      }
      
      if (addedCells) {
         GuiFilesModified fm;
         fm.setCellModified();
         theMainWindow->fileModificationUpdate(fm);
         theMainWindow->getBrainSet()->assignCellColors();
         DisplaySettingsCells* dsc = theMainWindow->getBrainSet()->getDisplaySettingsCells();
         dsc->setDisplayCells(true);
         GuiBrainModelOpenGL::updateAllGL();
      }
      
      if (addedContours) {
         GuiFilesModified fm;
         fm.setContourModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
      }
      
      if (addedContourCells) {
         GuiFilesModified fm;
         fm.setContourCellModified();
         theMainWindow->fileModificationUpdate(fm);
         theMainWindow->getBrainSet()->assignContourCellColors();
         DisplaySettingsContours* dsc = theMainWindow->getBrainSet()->getDisplaySettingsContours();
         dsc->setDisplayContourCells(true);
         GuiBrainModelOpenGL::updateAllGL();
      }
      
      if (addedFoci) {
         GuiFilesModified fm;
         fm.setFociModified();
         theMainWindow->fileModificationUpdate(fm);
         theMainWindow->getBrainSet()->assignFociColors();
         DisplaySettingsFoci* dsf = theMainWindow->getBrainSet()->getDisplaySettingsFoci();
         dsf->setDisplayCells(true);
         GuiBrainModelOpenGL::updateAllGL();
      }
      
      if (addedVtkModelFile) {
         GuiFilesModified fm;
         fm.setVtkModelModified();
         theMainWindow->fileModificationUpdate(fm);
         GuiBrainModelOpenGL::updateAllGL();
      }
      
      QApplication::restoreOverrideCursor();
   }
}      

/**
 * called when apply matrix button pressed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixApplyMainWindow()
{
   transferDialogValuesIntoMatrix();
   
   int contourIndex = -1;
   int contourCellIndex = -1;
   int surfaceIndex = -1;
   int volumeIndex  = -1;
   int surfaceVolumeIndex = -1;
   
   std::vector<QString> labels;
   
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   BrainModelVolume*  bmv = theMainWindow->getBrainModelVolume();
   BrainModelSurfaceAndVolume* bmsv = theMainWindow->getBrainModelSurfaceAndVolume();
   if (bmsv != NULL) {
      labels.push_back("Main Window Surface And Volume");
      surfaceVolumeIndex = labels.size() - 1;
   }
   else if (bms != NULL) {
      labels.push_back("Main Window Surface");
      surfaceIndex = labels.size() - 1;
   }
   else if (bmv != NULL) {
      labels.push_back("Main Window Volume");
      volumeIndex = labels.size() - 1;
   }
   else if (bmc != NULL) {
      labels.push_back("Main Window Contours");
      contourIndex = labels.size() - 1;
      labels.push_back("Main Window Contour Cells");
      contourCellIndex = labels.size() - 1;
   }

   int surfaceBordersIndex = -1;                              
   BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
   if (bms != NULL) {
      const int brainModelIndex = theMainWindow->getBrainSet()->getBrainModelIndex(bms);
      if (brainModelIndex < 0) {
         std::cout << "PROGRAM ERROR: invalid brain model index at " << __LINE__
                   << " in " << __FILE__ << std::endl;
         return;
      }
      const int num = bmbs->getNumberOfBorders();
      for (int i = 0; i < num; i++) {
         const BrainModelBorder* b = bmbs->getBorder(i);
         if (b->getValidForBrainModel(brainModelIndex) &&
             b->getDisplayFlag()) {
            labels.push_back("Main Window Surface Borders");
            surfaceBordersIndex = labels.size() - 1;
            break;
         }
      }
   }  
   
   int volumeBordersIndex = -1;
   if (bmv != NULL) {
      BorderFile* bf = theMainWindow->getBrainSet()->getVolumeBorderFile();
      if (bf->getNumberOfBorders() > 0) {
         labels.push_back("Main Window Volume Borders");
         volumeBordersIndex = labels.size() - 1;
      }
   }
   
   bool fiducialFlag = false;
   if (bms != NULL) {
      fiducialFlag = ((bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_RAW) ||
                      (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL));
   }
   int cellsIndex = -1;
   if (fiducialFlag) {
      CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
      if (cf->getNumberOfCellProjections() > 0) {
         labels.push_back("Main Window Cells");
         cellsIndex = labels.size() - 1;
      }
   }

   int fociIndex = -1;
   if (fiducialFlag) {
      FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
      if (ff->getNumberOfCellProjections() > 0) {
         labels.push_back("Main Window Foci");
         fociIndex = labels.size() - 1;
      }
   }
   
   std::vector<bool> initChecks(labels.size(), false);
   QtCheckBoxSelectionDialog cbsd(this,
                                  "Apply Transformation",
                                  "",
                                  labels,
                                  initChecks);
   if (cbsd.exec() == QDialog::Accepted) {
    
      bool ok = false;
      QString commentString = QInputDialog::getText(this, "Enter Comment Information",
                                                    "Enter Comment Describing Transform",
                                                    QLineEdit::Normal,
                                                    "",
                                                    &ok);
      QString operationComment;
      if (ok) {
         if (commentString.isEmpty() == false) {
            operationComment.append("\n");
            operationComment.append(commentString);
         }
      }
      else {
         return;
      }
      
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      
      if (surfaceIndex >= 0) {
         if (cbsd.getCheckBoxStatus(surfaceIndex)) {
            bms->applyTransformationMatrix(*currentMatrix);
            bms->setToStandardView(0, BrainModelSurface::VIEW_RESET);
            bms->appendToCoordinateFileComment(operationComment);
         }
      }
      
      if (volumeIndex >= 0) {
         if (cbsd.getCheckBoxStatus(volumeIndex)) {
         /*
            TransformationMatrix tm(*currentMatrix);
            float trans[3];
            tm.getTranslation(trans[0], trans[1], trans[2]);
            tm.translate(0.0, 0.0, 0.0);
            tm.transpose();
            tm.translate(trans[0], trans[1], trans[2]);
          */   
            VolumeFile* primaryOverlay = bmv->getOverlayPrimaryVolumeFile();
            TransformationMatrix tm = *currentMatrix;
            for (int j = 0; j < 3; j++) {
               float f = tm.getMatrixElement(j, 3);
               f = -f;
               tm.setMatrixElement(j, 3, f);
            }
            tm.transpose();
            if (primaryOverlay != NULL) {
               primaryOverlay->applyTransformationMatrix(tm);
               primaryOverlay->appendToFileComment(operationComment);
            }
            VolumeFile* secondaryOverlay = bmv->getOverlaySecondaryVolumeFile();
            if (secondaryOverlay != NULL) {
               secondaryOverlay->applyTransformationMatrix(tm);
               secondaryOverlay->appendToFileComment(operationComment);
            }
            VolumeFile* underlay = bmv->getUnderlayVolumeFile();
            if (underlay != NULL) {
               underlay->applyTransformationMatrix(tm);
               underlay->appendToFileComment(operationComment);
            }
         }
      }
      
      if (contourIndex >= 0) {
         if (cbsd.getCheckBoxStatus(contourIndex)) {
            ContourFile* cf = bmc->getContourFile();
            cf->applyTransformationMatrix(std::numeric_limits<int>::min(),
                                          std::numeric_limits<int>::max(),
                                          *currentMatrix,
                                          false);
         }
      }
      if (contourCellIndex >= 0) {
         if (cbsd.getCheckBoxStatus(contourCellIndex)) {
            ContourCellFile* ccf = theMainWindow->getBrainSet()->getContourCellFile();
            ccf->applyTransformationMatrix(*currentMatrix);
         }
      }
      
      if (surfaceVolumeIndex >= 0) {
         if (cbsd.getCheckBoxStatus(surfaceVolumeIndex)) {
            bmsv->applyTransformationMatrix(*currentMatrix);
            bmsv->setToStandardView(0, BrainModelSurface::VIEW_RESET);
            bmsv->appendToCoordinateFileComment(operationComment);
         }
      }
      
      if (surfaceBordersIndex >= 0) {
         if (cbsd.getCheckBoxStatus(surfaceBordersIndex)) {
            BrainModelBorderSet* bmbs = theMainWindow->getBrainSet()->getBorderSet();
            bmbs->applyTransformationMatrix(bms, *currentMatrix);
            BrainModelBorderFileInfo* bmbfi = bmbs->getBorderFileInfo(bms->getSurfaceType());
            if (bmbfi != NULL) {
               if (operationComment.isEmpty() == false) {
                  QString fileComment(bmbfi->getFileComment());
                  fileComment.append(operationComment);
                  bmbfi->setFileComment(fileComment);
               }
            }
         }
      }
      
      if (volumeBordersIndex >= 0) {
         if (cbsd.getCheckBoxStatus(volumeBordersIndex)) {
            BorderFile* bf = theMainWindow->getBrainSet()->getVolumeBorderFile();
            bf->applyTransformationMatrix(*currentMatrix);
            bf->appendToFileComment(operationComment);
         }
      }
      
      if (cellsIndex >= 0) {
         if (cbsd.getCheckBoxStatus(cellsIndex)) {
            CellProjectionFile* cf = theMainWindow->getBrainSet()->getCellProjectionFile();
            cf->applyTransformationMatrix(bms->getCoordinateFile(),
                                          bms->getTopologyFile(),
                                          fiducialFlag,
                                          std::numeric_limits<int>::min(),
                                          std::numeric_limits<int>::max(),
                                          *currentMatrix,
                                          false);

            cf->appendToFileComment(operationComment);
         }
      }
      
      if (fociIndex >= 0) {
         if (cbsd.getCheckBoxStatus(fociIndex)) {
            FociProjectionFile* ff = theMainWindow->getBrainSet()->getFociProjectionFile();
            ff->applyTransformationMatrix(bms->getCoordinateFile(),
                                          bms->getTopologyFile(),
                                          fiducialFlag,
                                          std::numeric_limits<int>::min(),
                                          std::numeric_limits<int>::max(),
                                          *currentMatrix,
                                          false);
            ff->appendToFileComment(operationComment);
         }
      }
      
      GuiBrainModelOpenGL::updateAllGL();
         
      QApplication::restoreOverrideCursor();
   }
}

/**
 * called when load matrix button pressed.
 */
void 
GuiTransformationMatrixDialog::slotMatrixLoad()
{
}

/**
 * get main window transform matrix.
 */
vtkTransform* 
GuiTransformationMatrixDialog::getMainWindowRotationTransform()
{
   BrainModelSurfaceAndVolume* bmsv = theMainWindow->getBrainModelSurfaceAndVolume();
   if (bmsv != NULL) {
      return bmsv->getRotationTransformMatrix(0);
   }
   
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   if (bms != NULL) {
      return bms->getRotationTransformMatrix(0);
   }
   
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   if (bmv != NULL) {
      return bmv->getRotationTransformMatrix(0);
   }
   
   BrainModelContours* bmc = theMainWindow->getBrainModelContours();
   if (bmc != NULL) {
      return bmc->getRotationTransformMatrix(0);
   }
   
   return NULL;
}

/**
 * add current matrix values to the undo stack.
 */
void 
GuiTransformationMatrixDialog::addMatrixToUndoStack()
{   
   if (currentMatrix != NULL) {
      undoMatrixStack.push(UndoMatrix(currentMatrix));   
   }
   enableDisableUndoPushButton();
}

/** 
 * called to pop a matrix off the undo stack.
 */
void 
GuiTransformationMatrixDialog::slotUndoPushButton()
{
   if (undoMatrixStack.empty() == false) {
      UndoMatrix ums = undoMatrixStack.top();
      undoMatrixStack.pop();
      if (currentMatrix != NULL) {
         ums.getMatrix(currentMatrix);
         transferMatrixIntoDialog();
         if (currentMatrix->getShowAxes()) {
            GuiBrainModelOpenGL::updateAllGL();
         }
      }
   }
   enableDisableUndoPushButton();
}      

/**
 * enable/disable the undo button.
 */
void 
GuiTransformationMatrixDialog::enableDisableUndoPushButton()
{
   undoPushButton->setEnabled(undoMatrixStack.empty() == false);
}
      
/**
 * clear the undo stack
 */
void 
GuiTransformationMatrixDialog::clearUndoStack()
{
   while (undoMatrixStack.empty() == false) {
      undoMatrixStack.pop();
   }
   enableDisableUndoPushButton();
}

/**
 * called when the axes is moved int the main window with mouse or keyboard.
 */
void 
GuiTransformationMatrixDialog::axesEventInMainWindow()
{
   addMatrixToUndoStack();
}      

/**
 * update the main window viewing matrix.
 */
void 
GuiTransformationMatrixDialog::updateMainWindowViewingMatrix()
{
   bool redrawFlag = false;
   BrainModelVolume* bmv = theMainWindow->getBrainModelVolume();
   BrainModelSurface* bms = theMainWindow->getBrainModelSurface();
   //
   // Is matrix yoked to main window viewing matrix
   //
   if (yokeToMainWindowCheckBox->isChecked()) {
      redrawFlag = true;
      
      //
      // Get transformations from matrix
      //      
      BrainModel* bm = theMainWindow->getBrainModel();
      if (bm != NULL) {
         float t[3];
         currentMatrix->getTranslation(t[0], t[1], t[2]);
         bm->setTranslation(BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW, t);
         
         float rx, ry, rz;
         currentMatrix->getRotationAngles(rx, ry, rz);
         TransformationMatrix rot;
         const double zeros[3] = { 0.0, 0.0, 0.0 };
         const double ones[3] = { 1.0, 1.0, 1.0 };
         const double r[3] = { rx, ry, rz };
         rot.setMatrix(zeros, r, ones);
         float mat[16];
         rot.getMatrix(mat);
         
         if (bms != NULL) {
            bms->setRotationMatrix(BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW, mat);
         }
         
         if (bmv != NULL) {
            switch (bmv->getSelectedAxis(0)) {
               case VolumeFile::VOLUME_AXIS_X:
               case VolumeFile::VOLUME_AXIS_Y:
               case VolumeFile::VOLUME_AXIS_Z:
               case VolumeFile::VOLUME_AXIS_ALL:
                  bmv->setRotationMatrix(BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW, mat);
                  break;
               case VolumeFile::VOLUME_AXIS_OBLIQUE:
                  bmv->setObliqueRotationMatrix(mat);
                  break;
               case VolumeFile::VOLUME_AXIS_OBLIQUE_X:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_Y:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_Z:
               case VolumeFile::VOLUME_AXIS_OBLIQUE_ALL:
                  {
                     DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
                     TransformationMatrix* obtm = dsv->getObliqueSlicesTransformationMatrix();
                     if (obtm != NULL) {
                        obtm->setMatrix(mat);
                     }
                     else {
                        bmv->setObliqueRotationMatrix(mat);
                     }
                  }
                  break;
               case VolumeFile::VOLUME_AXIS_UNKNOWN:
                  break;
            }
         }
         
         float sx, sy, sz;
         currentMatrix->getScaling(sx, sy, sz);
         bm->setScaling(BrainModel:: BRAIN_MODEL_VIEW_MAIN_WINDOW, sx, sy, sz);
      }
   }
   
   
   //
   // Are there oblique brain model volume slices displayed
   //
   if (bmv != NULL) {
      DisplaySettingsVolume* dsv = theMainWindow->getBrainSet()->getDisplaySettingsVolume();
      if (dsv->getObliqueSlicesTransformationMatrix() == currentMatrix) {
         if (GuiBrainModelOpenGL::viewingObliqueSlice()) {
            redrawFlag = true;
         }
      }
   }

   if (redrawFlag) {
      GuiBrainModelOpenGL::updateAllGL();
   }
   
}
      
//=====================================================================================

/**
 * Constructor.
 */
GuiTransformationMatrixDialog::UndoMatrix::UndoMatrix(const TransformationMatrix* tm)
{
   double m[16];
   tm->getMatrix(m);
   for (int i = 0; i < 16; i++) {
      mm[i] = m[i];
   }
}
      
/**
 * Destructor.
 */
GuiTransformationMatrixDialog::UndoMatrix::UndoMatrix()
{
}

/**
 * get the matrix.
 */
void 
GuiTransformationMatrixDialog::UndoMatrix::getMatrix(TransformationMatrix* tm) const
{
   tm->setMatrix(mm);
}


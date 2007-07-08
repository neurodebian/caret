
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

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QGlobalStatic>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QTextStream>
#include <QStackedWidget>

#include "BrainModelRunCaretUtilityProgram.h"
#include "Categories.h"
#include "DebugControl.h"
#include "GuiDataFileDialog.h"
#include "GuiMessageBox.h"
#include "GuiScriptArg.h"
#include "GuiScriptDialog.h" 
#include "GuiScriptOperation.h"
#include "QtDialog.h"
#include "QtTextEditDialog.h"
#include "QtUtilities.h"
#include "SpecFile.h"
#include "Species.h"
#include "StereotaxicSpace.h"
#include "StringUtilities.h"
#include "Structure.h"
#include "SystemUtilities.h"

/**
 * constructor
 */
GuiScriptDialog::GuiScriptDialog(QWidget* parent,
                                 const QString& caretHomeDirectory)
   : QtDialog(parent, false)
{
   setWindowTitle("Caret Script Builder");

   //
   // Set path/name of Caret program
   //
   caretVolumeProgramName = "";
   if (caretHomeDirectory.isEmpty() == false) {
      caretVolumeProgramName = caretHomeDirectory;
      caretVolumeProgramName.append(QDir::separator());
      caretVolumeProgramName.append("bin");
      caretVolumeProgramName.append(QDir::separator());
   }
   caretVolumeProgramName.append("caret_command");
   
   //
   // Vertical box for all items in dialog
   //
   QVBoxLayout* dialogLayout = new QVBoxLayout(this);
   dialogLayout->setMargin(5);
   dialogLayout->setSpacing(2);
   
   //
   // Group box for mode section
   //
   QGroupBox* modeGroupBox = new QGroupBox("Mode");
   dialogLayout->addWidget(modeGroupBox);
   QGridLayout* modeGroupBoxLayout = new QGridLayout(modeGroupBox);
   
   //
   // Mode combo box
   //
   modeGroupBoxLayout->addWidget(new QLabel("Mode "), 0, 0);
   modeComboBox = new QComboBox;
   modeGroupBoxLayout->addWidget(modeComboBox, 0, 1);
   modeComboBox->insertItem(MODE_CREATE_SCRIPT, "Create Script");
   modeComboBox->insertItem(MODE_CREATE_SCRIPT_AND_EXECUTE_COMMAND, "Create Script and Execute Command");
   modeComboBox->insertItem(MODE_EXECUTE_COMMAND, "Execute Command");
   QObject::connect(modeComboBox, SIGNAL(activated(int)),
                    this, SLOT(slotModeComboBox(int)));
   
   //
   // Script name
   //
   modeGroupBoxLayout->addWidget(new QLabel("Script Name "), 1, 0);
   scriptFileNameLineEdit = new QLineEdit;
   modeGroupBoxLayout->addWidget(scriptFileNameLineEdit, 1, 1);
   scriptFileNameLineEdit->setText("volume_script.sh");

   //
   // Log file
   //   
   logFileCheckBox = new QCheckBox("Log File ");
   modeGroupBoxLayout->addWidget(logFileCheckBox, 2, 0);
   logFileNameLineEdit = new QLineEdit;
   modeGroupBoxLayout->addWidget(logFileNameLineEdit, 2, 1);
   logFileNameLineEdit->setText("script.log");
   QObject::connect(logFileCheckBox, SIGNAL(toggled(bool)),
                    logFileNameLineEdit, SLOT(setEnabled(bool)));
   logFileCheckBox->setChecked(true);
   
   //
   // Group box for operations section
   //
   QGroupBox* operationsGroupBox = new QGroupBox("Operations");
   QVBoxLayout* operationsGroupLayout = new QVBoxLayout(operationsGroupBox);
   
   //
   // List box for operations
   //
   operationsListBox = new QListWidget;
   operationsGroupLayout->addWidget(operationsListBox);
   QObject::connect(operationsListBox, SIGNAL(currentRowChanged(int)),
                    this, SLOT(slotOperationsListBox(int)));
   operationsListBox->setMinimumHeight(50);
      
   //
   // Limit size of operaitons group box
   //
   //operationsGroupBox->setFixedHeight(operationsGroupBox->sizeHint().height());
   
   //
   // Group box for operations description section
   //
   QGroupBox* operationsDescriptionGroupBox = new QGroupBox("Operation Description");
   QVBoxLayout* operationsDescriptionLayout = new QVBoxLayout(operationsDescriptionGroupBox);
   operationsDescriptionLabel = new QLabel("", operationsDescriptionGroupBox);
   operationsDescriptionLayout->addWidget(operationsDescriptionLabel);

   //
   // Group box for parameters
   //
   QGroupBox* parametersGroupBox = new QGroupBox("Operation Parameters");
   QVBoxLayout* parametersGroupLayout = new QVBoxLayout(parametersGroupBox);
   
   //
   // Widget stack for operation parameters in scrolled area
   //
   operationsWidgetStack = new QStackedWidget;
   QScrollArea* operationsScrollArea = new QScrollArea;
   operationsScrollArea->setWidget(operationsWidgetStack);
   operationsScrollArea->setWidgetResizable(true);
   parametersGroupLayout->addWidget(operationsScrollArea);

   //
   // Create deformation map items
   //
   createDeformationMapPathUpdate();
   
   //
   // create the spec file operation pages
   //
   createSpecFileAddPage();
   createSpecFileCreatePage();
   
   //
   // create the surface operations pages
   //
   
   //
   // create the volume operations pages
   //
   createBlurPage();
   createClassifyIntensitiesPage();
   createConvertVectorToVolumePage();
   createCopyVolumePage();
   createCreateVolumePage();
   createDilatePage();
   createDilateErodePage();
   createErodePage();
   createFillBiggestObjectPage();
   createFillHolesPage();
   createFillSlicePage();
   createFindLimitsPage();
   createFloodFillPage();
   createGradientPage();
   createInformationPage();
   createIntersectVolumeWithSurfacePage();
   createMakePlanePage();
   createMakeShellPage();
   createMakeSpherePage();
   createMathPage();
   createMathUnaryPage();
   createMaskVolumePage();
   createNearToPlanePage();
   createPadVolumePage();
   createRemoveIslandsPage();
   createResamplePage();
   createReplaceVolumeVoxelsWithVectorMagnitude();
   createRescaleVoxelsPage();
   createScaleVoxels0255Page();
   createSculptPage();
   createSetOrientationPage();
   createSetOriginPage();
   createSetSpacingPage();
   createShellCommandPage();
   createShiftAxisPage();
   createSmearAxisPage();
   createThresholdPage();
   createThresholdDualPage();
   createThresholdInversePage();
   createVectorCombinePage();
   createVectorMagReplaceWithVolumePage();
   
   //
   // Load list box and operations widget stack
   //
   for (unsigned int i = 0; i < operations.size(); i++) {
      GuiScriptOperation* op = operations[i];
      operationsListBox->addItem(op->getDescriptiveName());
      operationsWidgetStack->addWidget(op->getWidget());
   }
   
   //
   // Splitter for operations, description, and parameters
   //
   QSplitter* splitter = new QSplitter(Qt::Vertical);
   splitter->addWidget(operationsGroupBox);
   splitter->addWidget(operationsDescriptionGroupBox);
   splitter->addWidget(parametersGroupBox);
   dialogLayout->addWidget(splitter);
   
   //
   // Horizontal layout for buttons
   //
   QHBoxLayout* buttonsLayout = new QHBoxLayout;
   buttonsLayout->setSpacing(2);
   dialogLayout->addLayout(buttonsLayout);
  
   //
   // Apply button
   //
   QPushButton* applyButton = new QPushButton("Apply");
   applyButton->setAutoDefault(false);
   buttonsLayout->addWidget(applyButton);
   QObject::connect(applyButton, SIGNAL(clicked()),
                    this, SLOT(slotApplyButton()));
  
   //
   // Close button
   //
   QPushButton* closeButton = new QPushButton("Close");
   closeButton->setAutoDefault(false);
   buttonsLayout->addWidget(closeButton);
   QObject::connect(closeButton, SIGNAL(clicked()),
                    this, SLOT(close()));

   QtUtilities::makeButtonsSameSize(applyButton, closeButton);
   
   modeComboBox->setCurrentIndex(MODE_CREATE_SCRIPT_AND_EXECUTE_COMMAND);
   slotModeComboBox(modeComboBox->currentIndex());
   
   if (operations.empty() == false) {
      slotOperationsListBox(0);
   }
   
   firstCommandFlag = true;
}

/**
 *  destructor
 */
GuiScriptDialog::~GuiScriptDialog()
{
}

/**
 * called when mode selected.
 */
void 
GuiScriptDialog::slotModeComboBox(int item)
{
   scriptFileNameLineEdit->setEnabled(item != MODE_EXECUTE_COMMAND);
}

/**
 * called when an operation is selected,
 */
void 
GuiScriptDialog::slotOperationsListBox(int item)
{
   operationsDescriptionLabel->setText("invalid command");
   if ((item >= 0) && (item < static_cast<int>(operations.size()))) {
      operationsWidgetStack->setCurrentWidget(operations[item]->getWidget());
      operationsDescriptionLabel->setText(operations[item]->getExplanationText());
   }
}
      
/**
 * called when apply button is pressed.
 */
void 
GuiScriptDialog::slotApplyButton()
{
   const QString envVarName("CARET_DEBUG");
   if (std::getenv(envVarName.toAscii().constData()) != NULL) {
      std::ostringstream str;
      str << "WARNING: The environment variable "
          << envVarName.toAscii().constData()
          << " is on.\n"
          << "This may cause the caret_command program to hang if\n"
          << "there is too much printed output.";
      if (GuiMessageBox::question(this, "WARNING", str.str().c_str(), "Continue", "Cancel") != 0) {
         return;
      }
   }
   
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   
   QWidget* w = operationsWidgetStack->currentWidget();
   if (w != NULL) {
      GuiScriptOperation* op = NULL;
      for (unsigned int i = 0; i < operations.size(); i++) {
         if (operations[i]->getWidget() == w) {
            op = operations[i];
            break;
         }
      }
      
      if (op != NULL) {
         QString commandName, errorMessage;
         QStringList commandArgs;
         if (op->getCommand(commandName, commandArgs, errorMessage) == false) {
            GuiMessageBox::critical(this, "ERROR", errorMessage, "OK");
            return;
         }
         
         QString command(commandName);
         if (commandName.isEmpty() == false) {
            command.append(" ");
         }
         command.append(commandArgs.join(" "));
         
         bool addToScript = false;
         bool runCommand = false;
         bool commandSuccessful = false;
         switch(static_cast<MODE>(modeComboBox->currentIndex())) {
            case MODE_CREATE_SCRIPT:
               addToScript = true;
               commandSuccessful = true;
               break;
            case MODE_CREATE_SCRIPT_AND_EXECUTE_COMMAND:
               addToScript = true;
               runCommand = true;
               break;
            case MODE_EXECUTE_COMMAND:
               runCommand = true;
               break;
         }
         
         //
         // Check script file
         //
         const QString scriptFileName(scriptFileNameLineEdit->text());
         if (addToScript) {
            if (scriptFileName.isEmpty() == false) {
               if (firstCommandFlag) {
                  if (QFile::exists(scriptFileName)) {
                     const QString msg = "The script file exists.  Do you want to\n"
                                         "append to the script file or replace\n"
                                         "the script file.";
                     const int result = GuiMessageBox::question(this, "Script File", msg, 
                                                                "Append", "Replace", "Cancel");
                     if (result == 1) {
                        QFile::remove(scriptFileName);
                     }
                     else if (result == 2) {
                        QApplication::restoreOverrideCursor();
                        return;
                     }
                  }
               }
            }
            else {
               GuiMessageBox::critical(this, "ERROR", "Script file name is empty.", "OK");
               return;
            }
         }
         
         //
         // Check log file
         //
         bool haveLogFile = true;
         const QString logFileName(logFileNameLineEdit->text());
         if (logFileCheckBox->isChecked() &&
             (logFileName.isEmpty() == false)) {
            if (firstCommandFlag) {
               if (QFile::exists(logFileName)) {
                  const QString msg = "The log file exists.  Do you want to\n"
                                      "append to the log file or replace\n"
                                      "the log file.";
                  const int result = GuiMessageBox::question(this, "Log File", msg, 
                                                             "Append", "Replace", "Cancel");
                  if (result == 1) {
                     QFile::remove(logFileName);
                  }
                  else if (result == 2) {
                     QApplication::restoreOverrideCursor();
                     return;
                  }
               }
            }
         }
         else {
            haveLogFile = false;
         }
         
         //
         // Should command be executed
         //
         if (runCommand) {
            //
            // Create a new QProcess and add its arguments
            //
            process = new QProcess(this);
            if (DebugControl::getDebugOn()) {
               std::cout << "COMMAND: " << commandName.toAscii().constData() << std::endl;
               std::cout << "   ARGS: ";
               for (int i = 0; i < commandArgs.size(); i++) {
                  std::cout << commandArgs.at(i).toAscii().constData() << " " << std::endl;
               }
            }
            
            //
            // Start execution of the command
            //
            process->start(commandName, commandArgs);
            if (!process->waitForStarted()) {
               QString msg("Error starting command: ");
               msg.append(commandName);
               GuiMessageBox::critical(this, "ERROR", msg, "OK");
               return;
            }
            
            //
            // Wait until the program is complete
            //
            if (!process->waitForFinished(100000000)) {
               QString msg("Error waiting for command to finish: ");
               msg.append(commandName);
               GuiMessageBox::critical(this, "ERROR", msg, "OK");
               return;
            }
            
            //
            // Display the results of the command
            //
            QtTextEditDialog* te = new QtTextEditDialog(this, true);
            QString txt;
            if (DebugControl::getDebugOn()) {
               txt.append("Command \"");
               txt.append(commandName);
               if (commandArgs.isEmpty() == false) {
                  txt.append(" ");
                  txt.append(commandArgs.join(" "));
               }
               txt.append("\"\n\n");
            }
            if (process->exitStatus() == QProcess::NormalExit) {
               commandSuccessful = true;
               txt.append("COMMAND SUCCESSFUL\n\n");
            }
            else {
               txt.append("COMMAND FAILED\n\n");
            }
            txt.append(process->readAll());
            
            te->setText(txt);
            te->show();
            
            //
            // Delete the QProcess
            //
            delete process;
            process = NULL;

         }
         
         //
         // Add on to the script file
         //
         if (addToScript && commandSuccessful) {
            const bool scriptFileExistsFlag = QFile::exists(scriptFileName);
            QFile scriptFile(scriptFileName);
            if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
               QTextStream stream(&scriptFile);
               
               if (scriptFileExistsFlag == false) {
#ifndef Q_OS_WIN32
                  stream << "#!/bin/sh\n";
                  stream << "\n";
#endif
               }
               const QString comment(op->getOperationComment());
               if (comment.isEmpty() == false) {
                  QString commentSymbol("# ");
#ifdef Q_OS_WIN32
                  commentSymbol = "REM ";
#endif
                  stream << commentSymbol << "\n";
                  stream << commentSymbol << comment << "\n";
                  stream << commentSymbol << "\n";
               }
               stream << command << "\n";
               stream << "\n";
               scriptFile.close();
            }
            else {
               std::ostringstream str;
               str << "Unable to open "
                   << scriptFileName.toAscii().constData()
                   << " for appending.";
               GuiMessageBox::critical(this, "ERROR", str.str().c_str(), "OK");
               return; 
            }
         }
         
         //
         // Add on to the log file
         //
         if (haveLogFile && commandSuccessful) {
            QFile logFile(logFileName);
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
               QTextStream stream(&logFile);               
               stream << command << "\n";
               stream << "\n";
               logFile.close();
            }
            else {
               std::ostringstream str;
               str << "Unable to open "
                   << logFileName.toAscii().constData()
                   << " for appending.";
               GuiMessageBox::critical(this, "ERROR", str.str().c_str(), "OK");
               return; 
            }
         }
         
         firstCommandFlag = false;
      }
   }
   
   QApplication::beep();
   QApplication::restoreOverrideCursor();
}

/**
 * add an operation.
 */
void 
GuiScriptDialog::addOperation(GuiScriptOperation* op)
{
   QWidget* w = op->getWidget();
   w->setFixedHeight(w->sizeHint().height());
   operations.push_back(op);
}      

/**
 * create the blur page.
 */
void 
GuiScriptDialog::createBlurPage()
{
   const QString description = 
       "Blur the volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Blur",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-blur");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the classify intensities page.
 */
void 
GuiScriptDialog::createClassifyIntensitiesPage()
{
   const QString description = 
       "Classify Intensities.";
   GuiScriptOperation* op = new GuiScriptOperation(0,
                                                   "Classify Intensities",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-classify-intensity");
   op->addFloatParameter("Mean");
   op->addFloatParameter("Low");
   op->addFloatParameter("High");
   op->addFloatParameter("Signum");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * convert a vector file to volume page.
 */
void 
GuiScriptDialog::createConvertVectorToVolumePage()
{
   const QString description = 
       "Convert a Vector File to a Volume File.";
   GuiScriptOperation* op = new GuiScriptOperation(0,
                                                   "Convert Vector File to Volume File",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-convert-vector-to-volume");
   op->addVectorFileParameter("Vector File");
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}      

/**
 * copy a volume page.
 */
void 
GuiScriptDialog::createCopyVolumePage()
{
   const QString description = 
       "Copy a volume file.  Copies both header and data file.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Copy Volume File",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-copy");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}      

/**
 * create a volume page.
 */
void 
GuiScriptDialog::createCreateVolumePage()
{
   const QString description = 
       "Create a volume in LPI orientation, with the origin\n"
       "at the ceneter, and all voxels set to zero.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Create a Volume",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-create");
   op->addIntParameter("X Dimension");
   op->addIntParameter("Y Dimension");
   op->addIntParameter("Z Dimension");
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}
      
/**
 * create the copy vec mag to volume volume page.
 */
void 
GuiScriptDialog::createReplaceVolumeVoxelsWithVectorMagnitude()
{
   const QString description = 
       "Replace the volume's voxels with the \n"
       "vector file's magnitude.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Replace Voxels With Vector Magnitude",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-copy-vector-mag");
   op->addVectorFileParameter("Vector File");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the dilate page.
 */
void 
GuiScriptDialog::createDilatePage()
{
   const QString description = 
       "Dilate the volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Dilate",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-dilate");
   op->addIntParameter("Iterations");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the dilate and erode page.
 */
void 
GuiScriptDialog::createDilateErodePage()
{
   const QString description = 
       "Dilate and erode the volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Dilate and Erode",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-dilate-erode");
   op->addIntParameter("Dilate Iterations");
   op->addIntParameter("Erode Iterations");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the erode page.
 */
void 
GuiScriptDialog::createErodePage()
{
   const QString description = 
       "Erode the volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Erode",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-erode");
   op->addIntParameter("Iterations");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the fill biggest object page.
 */
void 
GuiScriptDialog::createFillBiggestObjectPage()
{
   const QString description = 
       "Fill the Biggest Object.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Fill Biggest Object",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-fill-biggest-object");
   op->addExtentParameter("Extent");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the fill holes page.
 */
void 
GuiScriptDialog::createFillHolesPage()
{
   const QString description = 
       "Fill holes in the volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Fill Holes",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-fill-holes");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the fill slice page.
 */
void 
GuiScriptDialog::createFillSlicePage()
{
   const QString description = 
       "Fill a slice in the volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Fill Slice",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-fill-slice");
                                                   
   op->addAxisParameter("Axis");
   op->addSeedParameter("Seed");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/** 
 * create the find limits page.
 */
void 
GuiScriptDialog::createFindLimitsPage()
{
   const QString description = 
       "Find the limits (non-zero voxel extent) of a volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Find Limits",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-find-limits");
                                                   
   op->addInputVolumeFileParameter();
   op->addLimitsFileParameter("Limits File");
   
   addOperation(op);
}
      
/**
 * create the flood fill page.
 */
void 
GuiScriptDialog::createFloodFillPage()
{
   const QString description = 
       "Flood fill a volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Flood Fill",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-flood-fill");
                                                   
   op->addSeedParameter("Seed");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the gradient page.
 */
void 
GuiScriptDialog::createGradientPage()
{
   const QString description = 
       "Compute a gradient and store result in vector file.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Gradient",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-gradient");
                                                   
   op->addIntParameter("Lambda")->setValue(1);
   op->addBoolParameter("Gradient Flag")->setValue(true);
   op->addBoolParameter("Mask Flag")->setValue(false);
   op->addInputVolumeFileParameter();
   op->addInputVolumeFileParameter("Mask Volume");
   op->addVectorFileParameter("Output Vector");
   
   addOperation(op);
}

/**
 * create the volume information page.
 */
void 
GuiScriptDialog::createInformationPage()
{
   const QString description = 
       "Get information about a volume file.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Information (Volume)",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-info");
                                                   
   op->addInputVolumeFileParameter();
   
   addOperation(op);
}
      
/**
 * create the intersect volume with surface page.
 */
void 
GuiScriptDialog::createIntersectVolumeWithSurfacePage()
{
   const QString description = 
       "Intersect a volume with a surface and assign the paint\n"
       "name to any nodes that intersect a non-zero voxel. A\n"
       "paint file named \"<Paint Name>.paint\" is created.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Intersect Volume With Surface",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-intersect-volume-with-surface");
                                                   
   op->addStringParameter("Paint Name");
   op->addInputVolumeFileParameter("Volume File");
   op->addSpecFileParameter("Spec File");
   op->addFloatParameter("X Offset");
   op->addFloatParameter("Y Offset");
   op->addFloatParameter("Z Offset");
   
   addOperation(op);
}
      
/**
 * create the make plane page.
 */
void 
GuiScriptDialog::createMakePlanePage()
{
   const QString description = 
       "Make plane volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Make Plane",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-make-plane");
                                                   
   op->addFloatParameter("X-Slope");
   op->addFloatParameter("X-Offset");
   op->addFloatParameter("Y-Slope");
   op->addFloatParameter("Y-Offset");
   op->addFloatParameter("Z-Slope");
   op->addFloatParameter("Z-Offset");
   op->addFloatParameter("Offset");
   op->addFloatParameter("Thickness");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}
      
/**
 * create the make shell page.
 */
void 
GuiScriptDialog::createMakeShellPage()
{
   const QString description = 
       "Make shell volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Make Shell",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-make-shell");
                                                   
   op->addIntParameter("Dilation");
   op->addIntParameter("Erosion");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the make sphere page.
 */
void 
GuiScriptDialog::createMakeSpherePage()
{
   const QString description = 
       "Make a sphere volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Make Sphere",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-make-sphere");
                                                   
   op->addIntParameter("Center X");
   op->addIntParameter("Center Y");
   op->addIntParameter("Center Z");
   op->addFloatParameter("Radius");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the math page.
 */
void 
GuiScriptDialog::createMathPage()
{
   const QString description = 
       "Perform mathematical operations on volumes.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Mathematical Operations",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-math");
                                                   
   std::vector<QString> labels, values;
   labels.push_back("Add Volumes 1 and 2");    values.push_back("ADD");
   labels.push_back("And - Logical");    values.push_back("AND");
   labels.push_back("Diff Ratio (requires Volume 3)");    values.push_back("DIFF_RATIO");
   labels.push_back("Divide Volume 1 by Volume 2");    values.push_back("DIVIDE");
   labels.push_back("Maximum of Volumes 1 and 2");    values.push_back("MAX");
   labels.push_back("Multiply Volumes 1 and 2");    values.push_back("MULTIPLY");
   labels.push_back("Nand - Logical");    values.push_back("NAND");
   labels.push_back("Nor - Logical");    values.push_back("NOR");
   labels.push_back("Or - Logical");    values.push_back("OR");
   labels.push_back("Paint (Combine Volumes 1 and 2)j");    values.push_back("PAINT_COMBINE");
   labels.push_back("Square Root of (Volume 1 multiplied by Volume 2)");    values.push_back("SQRT");
   labels.push_back("Subtract Volume 2 from Volume 1");    values.push_back("SUBTRACT");
   labels.push_back("Subtract Volume 2 from Volume 1 (All results >= 0)");    values.push_back("SUBTRACT_POSITIVE");
   op->addComboBoxParameter("Operation", labels, values);
   op->addInputVolumeFileParameter("Input Volume 1");
   op->addInputVolumeFileParameter("Input Volume 2");
   op->addInputVolumeFileParameter("Input Volume 3", true);
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the math unary page.
 */
void 
GuiScriptDialog::createMathUnaryPage()
{
   const QString description = 
       "Perform unary mathematical operation on a volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Mathematical Operation (Unary)",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-math-unary");
                                                   
   std::vector<QString> labels, values;
   labels.push_back("Absolute Value of Voxels");    values.push_back("ABS_VALUE");
   labels.push_back("Add Scalar to Voxels");    values.push_back("ADD_SCALAR");
   labels.push_back("Maximum Value for Voxels");    values.push_back("CEILING");
   labels.push_back("Fix Not A Number Voxels");    values.push_back("FIX_NOT_A_NUMBER");
   labels.push_back("Minimum Value for Voxels");    values.push_back("FLOOR");
   labels.push_back("Multiply Voxel By Scalar");    values.push_back("MULTIPLY_SCALAR");
   op->addComboBoxParameter("Operation", labels, values);
   op->addFloatParameter("Scalar");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}      

/**
 * create the mask volume page.
 */
void 
GuiScriptDialog::createMaskVolumePage()
{
   const QString description = 
       "Create a mask volume (all voxels outside\n"
       "extent are set to zero).";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Mask Volume",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-mask-volume");
                                                   
   op->addExtentParameter("Extent");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the near to plane page.
 */
void 
GuiScriptDialog::createNearToPlanePage()
{
   const QString description = 
       "Perform near to plane operation.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Near to Plane",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-near-to-plane");
                                                   
   op->addFloatParameter("Sigma N");
   op->addFloatParameter("Sigma W");
   op->addFloatParameter("Offset");
   op->addBoolParameter("Down Flag");
   op->addIntParameter("Grad Sign");
   op->addBoolParameter("Masking Flag");
   op->addVectorFileParameter("Input Vector");
   op->addInputVolumeFileParameter("Mask Volume");
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the pad volume page.
 */
void 
GuiScriptDialog::createPadVolumePage() 
{
   const QString description = 
       "Pad a volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Pad Volume",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-pad-volume");
                                                   
   op->addExtentParameter("Padding");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();

   addOperation(op);
}

/**
 * create the remove islands page.
 */
void 
GuiScriptDialog::createRemoveIslandsPage()
{
   const QString description = 
       "Remove islands from segmentation volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Remove Islands",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-remove-islands");
                                                   
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the resample page.
 */
void 
GuiScriptDialog::createResamplePage()
{
   const QString description = 
       "Resample a volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Resample Volume",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-resample");
                                                   
   op->addFloatParameter("New Voxel Size X");
   op->addFloatParameter("New Voxel Size Y");
   op->addFloatParameter("New Voxel Size Z");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the rescale voxels page.
 */
void 
GuiScriptDialog::createRescaleVoxelsPage()
{
   const QString description = 
       "Rescale a volume's voxels.\n"
       "\"Input Min\" and below are mapped to \"Output Min\".\n"
       "\"Input Max\" and below are mapped to \"Output Max\".";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Rescale Voxels",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-rescale");
                                                   
   op->addFloatParameter("Input Min");
   op->addFloatParameter("Input Max");
   op->addFloatParameter("Output Min");
   op->addFloatParameter("Output Max");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the scale voxels 0 to 255 page.
 */
void 
GuiScriptDialog::createScaleVoxels0255Page()
{
   const QString description = 
       "Rescale a volume's voxels so that they are\n"
       "in the range 0 to 255.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Scale Voxels 0 to 255",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-scale-255");
                                                   
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the sculpt page.
 */
void 
GuiScriptDialog::createSculptPage()
{
   const QString description = 
       "Sculpt a volume.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Sculpt",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-sculpt");
                                                   
   op->addExtentParameter("Extent");
   op->addSeedParameter("Seed");
   std::vector<QString> labels, values;
   labels.push_back("And");   values.push_back("AND");
   labels.push_back("Seed And");   values.push_back("SEED-AND");
   labels.push_back("And Not");   values.push_back("AND-NOT");
   labels.push_back("Seed And Not");   values.push_back("SEED-AND-NOT");
   op->addComboBoxParameter("Mode", labels, values);
   op->addIntParameter("Number Of Steps");
   op->addInputVolumeFileParameter("Other Volume");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the set orientation page.
 */
void 
GuiScriptDialog::createSetOrientationPage()
{
   const QString description = 
       "Set the orientation.\n"
       "The orientation must be three letters where:\n"
       "   \"L\" Left to Right\n"
       "   \"R\" Right to Left\n"
       "   \"A\" Anterior to Posterior\n"
       "   \"P\" Posterior to Anterior\n"
       "   \"I\" Inferior to Superior\n"
       "   \"S\" Superior to Inferior\n";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Set Orientation",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-set-orientation");
   op->addStringParameter("Orientation");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the set origin page.
 */
void 
GuiScriptDialog::createSetOriginPage()
{
   const QString description = 
       "Set the origin (stereotaxic coordinates of first voxel).";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Set Origin",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-set-origin");
   op->addFloatParameter("Origin X");
   op->addFloatParameter("Origin Y");
   op->addFloatParameter("Origin Z");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the set spacing page.
 */
void 
GuiScriptDialog::createSetSpacingPage()
{
   const QString description = 
       "Set the origin (stereotaxic coordinates of first voxel).";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Set Voxel Size",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-set-spacing");
   op->addFloatParameter("Voxel Size X");
   op->addFloatParameter("Voxel Size Y");
   op->addFloatParameter("Voxel Size Z");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the shell command page.
 */
void 
GuiScriptDialog::createShellCommandPage()
{
   const QString description = 
       "Create a shell (UNIX or DOS) command.";
#ifdef Q_OS_WIN32
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Shell (DOS) Command",
                                                   description,
                                                   "cmd.exe",
                                                   "/C");
#else  // Q_OS_WIN32
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Shell (UNIX/DOS) Command",
                                                   description,
                                                   "/bin/sh",
                                                   "-c");
#endif // Q_OS_WIN32
   op->addStringParameter("Shell Command ");
   
   addOperation(op);   
}
      
/**
 * create the shift axis page.
 */
void 
GuiScriptDialog::createShiftAxisPage()
{
   const QString description = 
       "Shift Axis.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Shift Axis",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-shift-axis");
   op->addAxisParameter("Axis");
   op->addIntParameter("Offset");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the smear axis page.
 */
void 
GuiScriptDialog::createSmearAxisPage()
{
   const QString description = 
       "Smear Axis.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Smear Axis",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-smear-axis");
   op->addAxisParameter("Axis");
   op->addIntParameter("Mag");
   op->addIntParameter("Sign");
   op->addIntParameter("Core");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);   
}

/**
 * create the threshold page.
 */
void 
GuiScriptDialog::createThresholdPage()
{
   const QString description = 
       "All voxels below threshold are set to 0.\n"
       "All voxels above threshold are set to 255.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Threshold",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-threshold");
   op->addFloatParameter("Threshold");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the threshold dual page.
 */
void 
GuiScriptDialog::createThresholdDualPage()
{
   const QString description = 
       "All voxels between Low and High are set to 255.\n"
       "All other voxels are set to 0.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Threshold - Dual",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-threshold-dual");
   op->addFloatParameter("Low");
   op->addFloatParameter("High");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the threshold inverse page.
 */
void 
GuiScriptDialog::createThresholdInversePage()
{
   const QString description = 
       "All voxels below threshold are set to 255.\n"
       "All voxels above threshold are set to 0.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Threshold Inverse",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-threshold-inverse");
   op->addFloatParameter("Threshold-Inverse");
   op->addInputVolumeFileParameter();
   op->addOutputVolumeFileParameter();
   
   addOperation(op);
}

/**
 * create the combine vector files page.
 */
void 
GuiScriptDialog::createVectorCombinePage()
{
   const QString description = 
       "Combine vector files.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Vector Files - Combine",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-vector-combine");
   std::vector<QString> labels, values;
   labels.push_back("DOT_SQRT_RECT_MINUS");   values.push_back("DOT_SQRT_RECT_MINUS");
   labels.push_back("2_VEC_NORMAL");   values.push_back("2_VEC_NORMAL");
   labels.push_back("2_VEC");   values.push_back("2_VEC");
   op->addComboBoxParameter("Operation", labels, values);
   op->addBoolParameter("Mask Flag");
   op->addInputVolumeFileParameter("Mask Volume");
   op->addVectorFileParameter("Input Vector 1");
   op->addVectorFileParameter("Input Vector 2");
   op->addVectorFileParameter("Output Vector");
   
   addOperation(op);
}

/**
 * create the replace vector magnitude with volume page.
 */
void 
GuiScriptDialog::createVectorMagReplaceWithVolumePage()
{
   const QString description = 
       "Replace vector file magnitude with volume voxels.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Vector Files - Replace Magnitude with Volume Voxels",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-volume-vector-replace-mag-volume");
   std::vector<QString> labels, values;
   labels.push_back("Replace Magnitude With Voxels");   values.push_back("REPLACE");
   labels.push_back("Multiply Magnitude By Voxels");   values.push_back("MULTIPLY");
   op->addComboBoxParameter("Operation", labels, values);
   op->addInputVolumeFileParameter("Volume");
   op->addVectorFileParameter("Input Vector");
   op->addVectorFileParameter("Output Vector");
   
   addOperation(op);
}

/**
 * create the spec file create page.
 */
void 
GuiScriptDialog::createSpecFileCreatePage()
{
   const QString description = 
       "Create a spec file.  The name of the spec file does not need\n."
       "to be specified if species, subject, and structure are specified.\n"
       "If the name of the spec file is not specified, it will be created\n"
       "from the other parameters.  For example, if species is \"Human\",\n"
       "subject is \"Bill\", and structure is \"right\", the spec file\n"
       "named \"Human.Bill.R.spec\" will be created.\n"
       "\n"
       "Note: None of the parameters may contain spaces.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Spec File - Create",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-spec-file-create");
                                                   
   std::vector<QString> speciesLabels;
   Species::getAllSpecies(speciesLabels);
   int defaultSpecies = 0;
   for (unsigned int i = 0; i < speciesLabels.size(); i++) {
      if (speciesLabels[i] == "Human") {
         defaultSpecies = i;
         break;
      }
   }
   op->addComboBoxParameter("Species", speciesLabels, speciesLabels, defaultSpecies);

   op->addStringParameter("Subject", "subject1");
   
   std::vector<Structure::STRUCTURE_TYPE> structureTypes;
   std::vector<QString> structureNames;
   Structure::getAllTypesAndNames(structureTypes, structureNames, true);
   op->addComboBoxParameter("Structure", structureNames, structureNames);

   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);
   std::vector<QString> allSpaceNames;
   for (int i = 0; i < static_cast<int>(allSpaces.size()); i++) {
      allSpaceNames.push_back(allSpaces[i].getName());
   }
   op->addComboBoxParameter("Stereotaxic Space", allSpaceNames, allSpaceNames);

   std::vector<QString> allCategories;
   Categories::getAllCategories(allCategories);
   int defaultCategory = 0;
   for (unsigned int i = 0; i < allCategories.size(); i++) {
      if (allCategories[i] == "INDIVIDUAL") {
         defaultCategory = i;
         break;
      }
   }
   op->addComboBoxParameter("Category", allCategories, allCategories, defaultCategory);
   
   op->addSpecFileParameter("Spec File", true);
   
   addOperation(op);
}

/**
 * create the spec file add page.
 */
void 
GuiScriptDialog::createSpecFileAddPage()
{
   const QString description = 
       "Add a file to the spec file.\n."
       "Note: If adding a volume file that stores the volume data separate\n"
       "from the header, use the second file name for the data file name.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Spec File - Add",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-spec-file-add");
                                                   
   op->addSpecFileParameter("Spec File", true);
   
   std::vector<SpecFile::Entry*> allEntries;
   SpecFile sf;
   sf.getAllEntries(allEntries);
   std::vector<QString> allLabels, allTags;
   for (unsigned int i = 0; i < allEntries.size(); i++) {
     const QString tag(allEntries[i]->getSpecFileTag());
     const QString description(allEntries[i]->getDescriptiveName());
     QString label = description.leftJustified(35, ' ');
     label.append("(");
     label.append(tag);
     label.append(")");
     
     allLabels.push_back(label);
     allTags.push_back(tag);
   }
   
   op->addComboBoxParameter("Spec File Tag", allLabels, allTags);
            
   op->addGenericFileParameter("File Name", "(*)", true, false);

   op->addGenericFileParameter("File Name 2", "(*)", false, true);

   addOperation(op);
}

/**
 * create the deformation map path update page.
 */
void 
GuiScriptDialog::createDeformationMapPathUpdate()
{
   const QString description = 
       "Update the paths in a deformation map file.";
   GuiScriptOperation* op = new GuiScriptOperation(operationsWidgetStack,
                                                   "Deformation Map - Update Paths",
                                                   description,
                                                   caretVolumeProgramName,
                                                   "-def-map-path");
                                                   
   op->addGenericFileParameter("Deformation Map File",
                               GuiDataFileDialog::deformationMapFileFilter, 
                               true);
   
   op->addDirectoryParameter("Source (Indiv) Path");
   op->addDirectoryParameter("Target (Atlas) Path");
   
   addOperation(op);
}
      

      


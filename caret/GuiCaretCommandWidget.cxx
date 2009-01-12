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

#include <cstdlib>
#include <iostream>

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>

#include "CommandBase.h"
#include "CommandScriptComment.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "GuiCaretCommandWidget.h"
#include "ProgramParameters.h"
#include "QtUtilities.h"
#include "StringUtilities.h"
#include "Structure.h"
#include "WuQFileDialog.h"
#include "WuQWidgetGroup.h"

//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandWidget::GuiCaretCommandWidget(CommandBase* commandIn,
                                             const bool showCommentOptionIn)
   : QGroupBox("")
{
   command = commandIn;
   commandSwitch = command->getOperationSwitch();
   commandShortDescription = command->getShortDescription();
   
   //
   // No comment line if command is script comment
   //
   bool showCommentOption = showCommentOptionIn;
   if (dynamic_cast<CommandScriptComment*>(command) != NULL) {
      showCommentOption = false;
   }
   
   //
   // Set "this" group box title
   //
   setTitle(commandIn->getShortDescription() + " Parameters");
   
   //
   // Comment
   //   
   commentLineEdit = new QLineEdit;
   QObject::connect(commentLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SIGNAL(signalDataModified()));
   QLabel* commentLabel = new QLabel("Comment");
   QHBoxLayout* commentLayout = new QHBoxLayout;
   commentLayout->addWidget(commentLabel);
   commentLayout->addWidget(commentLineEdit);
   commentLayout->setStretchFactor(commentLabel, 0);
   commentLayout->setStretchFactor(commentLineEdit, 1000);
   
   //
   // Layout parameters
   //
   ScriptBuilderParameters parameters;
   command->getScriptBuilderParameters(parameters);   
   QGridLayout* gridLayout = new QGridLayout;
   gridLayout->setColumnStretch(GuiCaretCommandParameter::GRID_COLUMN_CHECK_BOX, 0);
   gridLayout->setColumnStretch(GuiCaretCommandParameter::GRID_COLUMN_DESCRIPTION, 0);
   gridLayout->setColumnStretch(GuiCaretCommandParameter::GRID_COLUMN_VALUE, 1000);
   for (int i = 0; i < parameters.getNumberOfParameters(); i++) {
      GuiCaretCommandParameter* gccp = 
         GuiCaretCommandParameter::createParameter(parameters.getParameter(i),
                                                   gridLayout);
      QObject::connect(gccp, SIGNAL(signalDataModified()),
                       this, SIGNAL(signalDataModified()));
                       
      commandParameters.push_back(gccp);
   }
   
   //
   // Hide comment?
   //
   if (showCommentOption == false) {
      commentLabel->hide();
      commentLineEdit->hide();
   }
   
   //
   // Layout widgets
   //
   QVBoxLayout* widgetLayout = new QVBoxLayout(this);
   widgetLayout->addLayout(commentLayout);
   widgetLayout->addLayout(gridLayout);
   widgetLayout->addStretch();
}

/**
 * destructor.
 */
GuiCaretCommandWidget::~GuiCaretCommandWidget()
{
   if (command != NULL) {
      delete command;
      command = NULL;
   }
}

/**
 * get the command switch and parameters.
 */
void
GuiCaretCommandWidget::getCommandLineForGUI(QString& commandSwitchOut,
                                QStringList& commandParametersOut,
                                QString& commandShortDescriptionOut,
                                bool& lastParameterIsVariableListFlag) const
{
   lastParameterIsVariableListFlag = false;
   
   commandSwitchOut = commandSwitch;
   commandShortDescriptionOut = commandShortDescription;

   commandParametersOut.clear();

   int numParams = static_cast<int>(commandParameters.size());
   for (int i = 0; i < numParams; i++) {
      bool paramValidFlag = false;
      const QStringList param = commandParameters[i]->getParameterForGUI(paramValidFlag);
      
      if (paramValidFlag) {
         commandParametersOut += param;
         
         if (i == (numParams - 1)) {
            if (dynamic_cast<GuiCaretCommandParameterVariableList*>(commandParameters[i]) != NULL) {
               lastParameterIsVariableListFlag = true;
            }
         }
      }
   }  
}

/**
 * get the command switch and parameters.
 */
void
GuiCaretCommandWidget::getCommandLineForCommandExecution(QString& commandSwitchOut,
                                             QStringList& commandParametersOut,
                                             QString& commandShortDescriptionOut,
                                             QString& errorMessageOut) const
{
   errorMessageOut = "";
   
   commandSwitchOut = commandSwitch;
   commandShortDescriptionOut = commandShortDescription;

   commandParametersOut.clear();

   int numParams = static_cast<int>(commandParameters.size());
   for (int i = 0; i < numParams; i++) {
      bool paramValidFlag = false;
      const QStringList param = commandParameters[i]->getParameterForGUI(paramValidFlag);
      
      if (paramValidFlag) {
         if (param.isEmpty()) {
            if (commandParameters[i]->getParameterAllowedToBeEmpty() == false) {
               if (errorMessageOut.isEmpty() == false) {
                  errorMessageOut += "\n";
               }
               errorMessageOut += ("Parameter \""
                                   + commandParameters[i]->getParameterDescription()
                                   + "\" is missing its value.");
            }
            
            //
            // Empty parameters goes in double quotes
            //
            //param = "\"\"";
         }
         
         //
         // Is last parameter variable list parameter?
         //
         if (i == (numParams - 1)) {
            if (dynamic_cast<GuiCaretCommandParameterVariableList*>(commandParameters[i]) != NULL) {
               //
               // Split up variable list
               //
               //QStringList sl;
               //StringUtilities::tokenStringsWithQuotes(param, sl);
               if (param.count() == 1) {
                  const QString p = param.at(0).trimmed();
                  if (p.isEmpty() == false) {
                     commandParametersOut += param;
                  }
               }
               else {
                  commandParametersOut += param;
               }
            }
            else {
               commandParametersOut += param;
            }
         }
         else {
            commandParametersOut += param;
         }
      }
   }  
}

/** 
 * get the comment.
 */
QString 
GuiCaretCommandWidget::getComment() const 
{ 
   return commentLineEdit->text(); 
}
      
/**
 * set the comment.
 */
void 
GuiCaretCommandWidget::setComment(const QString& s)
{
   commentLineEdit->setText(s);
}

/**
 * set the parameters.
 */
void 
GuiCaretCommandWidget::setParameters(const QStringList& parametersIn)
{
   QStringList parameters = parametersIn;
   
   const int numParamGUI = static_cast<int>(commandParameters.size());

   //
   // Look for and set any optional parameters first
   //
   for (int i = 0; i < numParamGUI; i++) {
      GuiCaretCommandParameter* cmdParam = commandParameters[i];
      const QString optSwitch = cmdParam->getOptionalSwitch();
      if (optSwitch.isEmpty() == false) {
         for (int j = 0; j < parameters.count(); j++) {
            if (parameters.at(j) == optSwitch) {
               cmdParam->setChecked(true);
               const int nextJ = j + 1;
               if (nextJ < parameters.count()) {
                  cmdParam->setParameterValueFromText(parametersIn.at(nextJ));
                  parameters.removeAt(nextJ);
               }
               parameters.removeAt(j);
               break;
            }
         }
      }
   }

   //
   // Set required parameters last
   //
   const int num = std::min(numParamGUI,
                            static_cast<int>(parameters.count()));
   for (int i = 0; i < num; i++) {
      GuiCaretCommandParameter* cmdParam = commandParameters[i];
      if (cmdParam->getOptionalSwitch().isEmpty()) {
         //
         // If parameter is variable list, use all 
         // remaining parameters
         //
         GuiCaretCommandParameterVariableList* varListParam =
           dynamic_cast<GuiCaretCommandParameterVariableList*>(cmdParam);
         if (varListParam != NULL) {
            QString str = "";
            while (i < static_cast<int>(parameters.count())) {
               if (str.isEmpty() == false) {
                  str += " ";
               }
               str += parametersIn.at(i);
               
               i++;
            } 
            cmdParam->setParameterValueFromText(str);
         }
         else {
            cmdParam->setParameterValueFromText(parametersIn.at(i));
         }
         
      }
   }
}

//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameter::GuiCaretCommandParameter(const ScriptBuilderParameters::Parameter* parameter)
{
   optionalSwitch = parameter->getOptionalSwitch();
   optionCheckBox = NULL;
}

/**
 * destructor.
 */
GuiCaretCommandParameter::~GuiCaretCommandParameter()
{
}

/**
 * add widgets to the grid.
 */
void 
GuiCaretCommandParameter::addWidgetsToGridLayout(QGridLayout* gridLayout,
                                                 QWidget* descriptionWidget,
                                                 QWidget* valueWidget)
{
   const int rowNumber = gridLayout->rowCount();
   if (optionalSwitch.isEmpty() == false) {
      optionCheckBox = new QCheckBox("");
      gridLayout->addWidget(optionCheckBox, rowNumber, GRID_COLUMN_CHECK_BOX);
   }
   gridLayout->addWidget(descriptionWidget, rowNumber, GRID_COLUMN_DESCRIPTION);
   gridLayout->addWidget(valueWidget, rowNumber, GRID_COLUMN_VALUE);
   
   if (optionCheckBox != NULL) {
      WuQWidgetGroup* wg = new WuQWidgetGroup(optionCheckBox);
      wg->addWidget(descriptionWidget);
      wg->addWidget(valueWidget);
      QObject::connect(optionCheckBox, SIGNAL(toggled(bool)),
                       wg, SLOT(setEnabled(bool)));
      wg->setEnabled(optionCheckBox->isChecked());
   }
}
                            
/**
 * set the checkbox.
 */
void 
GuiCaretCommandParameter::setChecked(const bool b)
{
   if (optionCheckBox != NULL) {
      optionCheckBox->setChecked(b);
   }
}
      
/**
 * get parameter value as text for the GUI.
 */
QStringList
GuiCaretCommandParameter::getParameterForGUI(bool& parameterValidOut) const
{
   QStringList cmdParams;
   parameterValidOut = true;
   if (optionCheckBox != NULL) {
      if (optionCheckBox->isChecked()) {
         cmdParams += optionalSwitch;
      }
      else {
         parameterValidOut = false;
      }
   }
   
   if (parameterValidOut) {
      const QStringList sl = getParameterValueAsText();
      for (int i = 0; i < sl.count(); i++) {
         cmdParams += sl.at(i).trimmed();
      }
   }
   
   return cmdParams;
}

/**
 * set the parameter description.
 */
void 
GuiCaretCommandParameter::setParameterDescription(const QString& parameterDescriptionIn)
{
   parameterDescription = parameterDescriptionIn;
}

/**
 * create a parameter.
 */
GuiCaretCommandParameter* 
GuiCaretCommandParameter::createParameter(const ScriptBuilderParameters::Parameter* parameter,
                                          QGridLayout* gridLayout)
{
   GuiCaretCommandParameter* commandParameter = NULL;
   
   switch (parameter->getType()) {
      case ScriptBuilderParameters::Parameter::TYPE_BOOLEAN:
         commandParameter = new GuiCaretCommandParameterBoolean(parameter,
                                                              gridLayout);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_DIRECTORY:
         commandParameter = new GuiCaretCommandParameterDirectory(parameter,
                                                                  gridLayout);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_FILE:
         commandParameter = new GuiCaretCommandParameterFile(parameter,
                                                              gridLayout,
                                                              false);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_FILE_MULTIPLE:
         commandParameter = new GuiCaretCommandParameterFile(parameter,
                                                              gridLayout,
                                                              true);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_FLOAT:
         commandParameter = new GuiCaretCommandParameterFloat(parameter,
                                                              gridLayout);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_INT:
         commandParameter = new GuiCaretCommandParameterInt(parameter,
                                                              gridLayout);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_LIST_OF_ITEMS:
         commandParameter = new GuiCaretCommandParameterDataItemList(parameter,
                                                                 gridLayout);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_STRING:
         commandParameter = new GuiCaretCommandParameterString(parameter,
                                                              gridLayout);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_VARIABLE_LIST_OF_PARAMETERS:
         commandParameter = new GuiCaretCommandParameterVariableList(parameter,
                                                              gridLayout);
         break;
      case ScriptBuilderParameters::Parameter::TYPE_STRUCTURE:
         commandParameter = new GuiCaretCommandParameterStructure(parameter,
                                                                  gridLayout);
         break;
      default:
         {
            QLabel* label1 = new QLabel(parameter->getDescription());
            QLabel* label2 = new QLabel("UNKNOWN PARAMETER TYPE");
            commandParameter->addWidgetsToGridLayout(gridLayout, label1, label2);
            //const int rowNumber = gridLayout->rowCount();
            //gridLayout->addWidget(label1, rowNumber, GRID_COLUMN_DESCRIPTION);
            //gridLayout->addWidget(label2, rowNumber, GRID_COLUMN_VALUE);
         }
         std::cout << "PROGRAM ERROR: Unsupported parameter type for." << std::endl;
         break;
   }
   
   if (commandParameter != NULL) {
      commandParameter->setParameterDescription(parameter->getDescription());
   }
   
   return commandParameter;
}

//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameterFloat::GuiCaretCommandParameterFloat(const ScriptBuilderParameters::Parameter* parameter,
                                                             QGridLayout* gridLayout)
   : GuiCaretCommandParameter(parameter)
{
   QLabel* label = new QLabel(parameter->getDescription());
   
   float defaultValue, minimumValue, maximumValue;
   parameter->getFloatParameters(defaultValue,
                                 minimumValue,
                                 maximumValue);
   doubleSpinBox = new QDoubleSpinBox;
   doubleSpinBox->setMinimum(minimumValue);
   doubleSpinBox->setMaximum(maximumValue);
   doubleSpinBox->setDecimals(3);
   doubleSpinBox->setValue(defaultValue);
   QObject::connect(doubleSpinBox, SIGNAL(valueChanged(double)),
                    this, SIGNAL(signalDataModified()));
   addWidgetsToGridLayout(gridLayout, label, doubleSpinBox);                 
   //const int rowNumber = gridLayout->rowCount();
   //gridLayout->addWidget(label, rowNumber, GRID_COLUMN_DESCRIPTION);
   //gridLayout->addWidget(doubleSpinBox, rowNumber, GRID_COLUMN_VALUE);
}

/**
 * destructor.
 */
GuiCaretCommandParameterFloat::~GuiCaretCommandParameterFloat()
{
}

/**
 * get parameter value as text.
 */
QStringList
GuiCaretCommandParameterFloat::getParameterValueAsText() const
{
   const QString s = QString::number(doubleSpinBox->value(), 'f', 3);
   return QStringList(s);
}
   
/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterFloat::setParameterValueFromText(const QString& s)
{
   doubleSpinBox->setValue(s.toFloat());
}
      
//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameterInt::GuiCaretCommandParameterInt(const ScriptBuilderParameters::Parameter* parameter,
                                                             QGridLayout* gridLayout)
   : GuiCaretCommandParameter(parameter)
{
   QLabel* label = new QLabel(parameter->getDescription());
   
   int defaultValue, minimumValue, maximumValue;
   parameter->getIntParameters(defaultValue,
                                 minimumValue,
                                 maximumValue);
   spinBox = new QSpinBox;
   spinBox->setMinimum(minimumValue);
   spinBox->setMaximum(maximumValue);
   spinBox->setValue(defaultValue);
   QObject::connect(spinBox, SIGNAL(valueChanged(int)),
                    this, SIGNAL(signalDataModified()));
   
   addWidgetsToGridLayout(gridLayout, label, spinBox);
   //const int rowNumber = gridLayout->rowCount();
   //gridLayout->addWidget(label, rowNumber, GRID_COLUMN_DESCRIPTION);
   //gridLayout->addWidget(spinBox, rowNumber, GRID_COLUMN_VALUE);
}

/**
 * destructor.
 */
GuiCaretCommandParameterInt::~GuiCaretCommandParameterInt()
{
}

/**
 * get parameter value as text.
 */
QStringList 
GuiCaretCommandParameterInt::getParameterValueAsText() const
{
   const QString s = QString::number(spinBox->value());
   return QStringList(s);
}
   
/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterInt::setParameterValueFromText(const QString& s)
{
   spinBox->setValue(s.toInt());
}
      
//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameterBoolean::GuiCaretCommandParameterBoolean(const ScriptBuilderParameters::Parameter* parameter,
                                                             QGridLayout* gridLayout)
   : GuiCaretCommandParameter(parameter)
{
   QLabel* label = new QLabel(parameter->getDescription());
   
   bool defaultValue;
   parameter->getBooleanParameters(defaultValue);
   comboBox = new QComboBox;
   comboBox->addItem("false");
   comboBox->addItem("true");
   QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)),
                    this, SIGNAL(signalDataModified()));
   
   if (defaultValue) {
      comboBox->setCurrentIndex(1);
   }
   else {
      comboBox->setCurrentIndex(0);
   }
   
   addWidgetsToGridLayout(gridLayout, label, comboBox);
   //const int rowNumber = gridLayout->rowCount();
   //gridLayout->addWidget(label, rowNumber, GRID_COLUMN_DESCRIPTION);
   //gridLayout->addWidget(comboBox, rowNumber, GRID_COLUMN_VALUE);
}

/**
 * destructor.
 */
GuiCaretCommandParameterBoolean::~GuiCaretCommandParameterBoolean()
{
}

/**
 * get parameter value as text.
 */
QStringList 
GuiCaretCommandParameterBoolean::getParameterValueAsText() const
{
   QString s("false");
   if (comboBox->currentIndex() == 1) {
      s = "true";
   }
   return QStringList(s);
}
   
/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterBoolean::setParameterValueFromText(const QString& s)
{
   if (s.toLower().trimmed() == "true") {
      comboBox->setCurrentIndex(1);
   }
   else {
      comboBox->setCurrentIndex(0);
   }
}
      
//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameterFile::GuiCaretCommandParameterFile(const ScriptBuilderParameters::Parameter* parameter,
                                                           QGridLayout* gridLayout,
                                                           const bool multipleSelectionFlagIn)
   : GuiCaretCommandParameter(parameter)
{
   multipleSelectionFlag = multipleSelectionFlagIn;

   QPushButton* pushButton = new QPushButton(parameter->getDescription() + "...");
   pushButton->setAutoDefault(false);
   QObject::connect(pushButton, SIGNAL(clicked()),
                    this, SLOT(slotPushButtonPressed()));
                    
   fileNameLineEdit = new QLineEdit;
   QObject::connect(fileNameLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SIGNAL(signalDataModified()));
   
   addWidgetsToGridLayout(gridLayout, pushButton, fileNameLineEdit);
   //const int rowNumber = gridLayout->rowCount();
   //gridLayout->addWidget(pushButton, rowNumber, GRID_COLUMN_DESCRIPTION);
   //gridLayout->addWidget(fileNameLineEdit, rowNumber, GRID_COLUMN_VALUE);
   
   QString defaultFileName;
   parameter->getFileParameters(fileFilters, defaultFileName);
   fileNameLineEdit->setText(defaultFileName);
}

/**
 * destructor.
 */
GuiCaretCommandParameterFile::~GuiCaretCommandParameterFile()
{
}

/**
 * called when push button pressed.
 */
void 
GuiCaretCommandParameterFile::slotPushButtonPressed()
{
   WuQFileDialog fd(fileNameLineEdit);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(QDir::currentPath());
   if (multipleSelectionFlag) {
      fd.setFileMode(WuQFileDialog::ExistingFiles);
   }
   else {
      fd.setFileMode(WuQFileDialog::ExistingFile);
   }
   fd.setFilters(fileFilters);
   if (fd.exec() == WuQFileDialog::Accepted) {
      
      QString fileList;
      QStringList selectedFiles = fd.selectedFiles();
      for (int i = 0; i < selectedFiles.count(); i++) {
         QString fileName = selectedFiles.at(i);
         QFileInfo fi(fileName);
         if (fi.absolutePath() == QDir::currentPath()) {
            fileName = fi.fileName();
         }
         
         if (i > 0) {
            fileList += " ";
         }
         fileList += fileName;
         if (multipleSelectionFlag == false) {
            break;
         }
      }

      fileNameLineEdit->setText(fileList);
   }
}

/**
 * get parameter value as text.
 */
QStringList 
GuiCaretCommandParameterFile::getParameterValueAsText() const
{
   return QStringList(fileNameLineEdit->text().split(' ', QString::SkipEmptyParts));
}

/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterFile::setParameterValueFromText(const QString& s)
{
   fileNameLineEdit->setText(s);
}
      
//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameterDirectory::GuiCaretCommandParameterDirectory(const ScriptBuilderParameters::Parameter* parameter,
                                                           QGridLayout* gridLayout)
   : GuiCaretCommandParameter(parameter)
{
   QPushButton* pushButton = new QPushButton(parameter->getDescription() + "...");
   pushButton->setAutoDefault(false);
   QObject::connect(pushButton, SIGNAL(clicked()),
                    this, SLOT(slotPushButtonPressed()));
                    
   directoryNameLineEdit = new QLineEdit;
   QObject::connect(directoryNameLineEdit, SIGNAL(textChanged(const QString&)),
                    this, SIGNAL(signalDataModified()));
   
   addWidgetsToGridLayout(gridLayout, pushButton, directoryNameLineEdit);
   //const int rowNumber = gridLayout->rowCount();
   //gridLayout->addWidget(pushButton, rowNumber, GRID_COLUMN_DESCRIPTION);
   //gridLayout->addWidget(directoryNameLineEdit, rowNumber, GRID_COLUMN_VALUE);
}

/**
 * destructor.
 */
GuiCaretCommandParameterDirectory::~GuiCaretCommandParameterDirectory()
{
}

/**
 * called when push button pressed.
 */
void 
GuiCaretCommandParameterDirectory::slotPushButtonPressed()
{
   WuQFileDialog fd(directoryNameLineEdit);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(QDir::currentPath());
   fd.setFileMode(WuQFileDialog::DirectoryOnly);
   if (fd.exec() == WuQFileDialog::Accepted) {
      if (fd.selectedFiles().count() > 0) {
         const QString dirName = fd.selectedFiles().at(0);
         directoryNameLineEdit->setText(dirName);
      }
   }
}

/**
 * get parameter value as text.
 */
QStringList 
GuiCaretCommandParameterDirectory::getParameterValueAsText() const
{
   return QStringList(directoryNameLineEdit->text());
}

/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterDirectory::setParameterValueFromText(const QString& s)
{
   directoryNameLineEdit->setText(s);
}
      
//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameterDataItemList::GuiCaretCommandParameterDataItemList(const ScriptBuilderParameters::Parameter* parameter,
                               QGridLayout* gridLayout)
   : GuiCaretCommandParameter(parameter)
{
   QLabel* label = new QLabel(parameter->getDescription());
   
   comboBox = new QComboBox;

   std::vector<QString> dataNames;
   parameter->getListOfItemParameters(dataValues, dataNames);
   for (unsigned int i = 0; i < dataNames.size(); i++) {
      comboBox->addItem(dataNames[i]);
   }
   QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)),
                    this, SIGNAL(signalDataModified()));
   
   addWidgetsToGridLayout(gridLayout, label, comboBox);
   //const int rowNumber = gridLayout->rowCount();
   //gridLayout->addWidget(label, rowNumber, GRID_COLUMN_DESCRIPTION);
   //gridLayout->addWidget(comboBox, rowNumber, GRID_COLUMN_VALUE);
}

/**
 * destructor.
 */
GuiCaretCommandParameterDataItemList::~GuiCaretCommandParameterDataItemList()
{
}

/**
 * get parameter value as text.
 */
QStringList 
GuiCaretCommandParameterDataItemList::getParameterValueAsText() const
{
   const int indx = comboBox->currentIndex();
   const QString s = dataValues[indx];
   return QStringList(s);
}

/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterDataItemList::setParameterValueFromText(const QString& s)
{
   for (int i = 0; i < comboBox->count(); i++) {
      if (s == dataValues[i]) {
         comboBox->setCurrentIndex(i);
         break;
      }
   }
}
      
//=============================================================================
//=============================================================================
//=============================================================================
/**
 * constructor.
 */
GuiCaretCommandParameterStructure::GuiCaretCommandParameterStructure(const ScriptBuilderParameters::Parameter* parameter,
                                                             QGridLayout* gridLayout)
   : GuiCaretCommandParameter(parameter)
{
   std::vector<Structure::STRUCTURE_TYPE> types;
   std::vector<QString> names;
   Structure::getAllTypesAndNames(types, names, false);
   comboBoxValues = names;
   
   QLabel* label = new QLabel(parameter->getDescription());
   
   comboBox = new QComboBox;
   QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)),
                    this, SIGNAL(signalDataModified()));

   for (unsigned int i = 0; i < comboBoxValues.size(); i++) {
      comboBox->addItem(comboBoxValues[i]);
   }
   
   addWidgetsToGridLayout(gridLayout, label, comboBox);
   //const int rowNumber = gridLayout->rowCount();
   //gridLayout->addWidget(label, rowNumber, GRID_COLUMN_DESCRIPTION);
   //gridLayout->addWidget(comboBox, rowNumber, GRID_COLUMN_VALUE);
}

/**
 * destructor.
 */ 
GuiCaretCommandParameterStructure::~GuiCaretCommandParameterStructure()
{
}

/**
 * get parameter value as text.
 */
QStringList 
GuiCaretCommandParameterStructure::getParameterValueAsText() const
{
   const int indx = comboBox->currentIndex();
   const QString s = comboBoxValues[indx];
   return QStringList(s);
}

/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterStructure::setParameterValueFromText(const QString& s)
{
   for (int i = 0; i < comboBox->count(); i++) {
      if (s == comboBoxValues[i]) {
         comboBox->setCurrentIndex(i);
         break;
      }
   }
}
      
//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameterString::GuiCaretCommandParameterString(const ScriptBuilderParameters::Parameter* parameter,
                                                             QGridLayout* gridLayout)
   : GuiCaretCommandParameter(parameter)
{
   QLabel* label = new QLabel(parameter->getDescription());
   
   lineEdit = new QLineEdit;

   QString defaultValue;
   parameter->getStringParameters(defaultValue);
   lineEdit->setText(defaultValue);
   QObject::connect(lineEdit, SIGNAL(textChanged(const QString&)),
                    this, SIGNAL(signalDataModified()));
   
   addWidgetsToGridLayout(gridLayout, label, lineEdit);
   //const int rowNumber = gridLayout->rowCount();
   //gridLayout->addWidget(label, rowNumber, GRID_COLUMN_DESCRIPTION);
   //gridLayout->addWidget(lineEdit, rowNumber, GRID_COLUMN_VALUE);
}

/**
 * destructor.
 */
GuiCaretCommandParameterString::~GuiCaretCommandParameterString()
{
}

/**
 * get parameter value as text.
 */
QStringList 
GuiCaretCommandParameterString::getParameterValueAsText() const
{
   const QString s = lineEdit->text();
   return QStringList(s);
}
   
/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterString::setParameterValueFromText(const QString& s)
{
   lineEdit->setText(s);
}
      
//=============================================================================
//=============================================================================
//=============================================================================

/**
 * constructor.
 */
GuiCaretCommandParameterVariableList::GuiCaretCommandParameterVariableList(const ScriptBuilderParameters::Parameter* parameter,
                                                             QGridLayout* gridLayout)
   : GuiCaretCommandParameter(parameter)
{
   QLabel* label = new QLabel(parameter->getDescription());
   
   QPushButton* addFilePushButton = new QPushButton("Add File...");
   addFilePushButton->setAutoDefault(false);
   addFilePushButton->setFixedSize(addFilePushButton->sizeHint());
   QObject::connect(addFilePushButton, SIGNAL(clicked()),
                    this, SLOT(slotAddFileButton()));
   textEdit = new QTextEdit;
   QString defaultValue;
   parameter->getVariableListParameters(defaultValue);
   textEdit->setText(defaultValue);
   QObject::connect(textEdit, SIGNAL(textChanged()),
                    this, SIGNAL(signalDataModified()));

   const int rowNumber = gridLayout->rowCount();
   gridLayout->addWidget(label, rowNumber, GRID_COLUMN_DESCRIPTION);
   gridLayout->addWidget(addFilePushButton, rowNumber + 1, GRID_COLUMN_DESCRIPTION);
   gridLayout->addWidget(textEdit, rowNumber, GRID_COLUMN_VALUE, 2, 1);
   gridLayout->setRowStretch(rowNumber, 0);
   gridLayout->setRowStretch(rowNumber + 1, 0);
   gridLayout->setRowStretch(rowNumber + 2, 100);
}

/**
 * destructor.
 */
GuiCaretCommandParameterVariableList::~GuiCaretCommandParameterVariableList()
{
}

/**
 * called when add file button is selected.
 */
void 
GuiCaretCommandParameterVariableList::slotAddFileButton()
{
   QStringList allFileFilters;
   FileFilters::getAllFileFilters(allFileFilters);
   static QString lastFileFilter(FileFilters::getAnyFileFilter());
   
   WuQFileDialog fd(textEdit);
   fd.setModal(true);
   fd.setAcceptMode(WuQFileDialog::AcceptOpen);
   fd.setDirectory(QDir::currentPath());
   fd.setFileMode(WuQFileDialog::ExistingFiles);
   fd.setFilters(allFileFilters);
   fd.selectFilter(lastFileFilter);
   if (fd.exec() == WuQFileDialog::Accepted) {
      
      QString fileList;
      QStringList selectedFiles = fd.selectedFiles();
      for (int i = 0; i < selectedFiles.count(); i++) {
         QString fileName = selectedFiles.at(i);
         QFileInfo fi(fileName);
         if (fi.absolutePath() == QDir::currentPath()) {
            fileName = fi.fileName();
         }
         fileList += " ";
         fileList += fileName;
      }

      textEdit->setText(textEdit->toPlainText() + fileList);
   }
}
      
/**
 * get parameter value as text.
 */
QStringList 
GuiCaretCommandParameterVariableList::getParameterValueAsText() const
{
   const QString s = textEdit->toPlainText();
   
   QStringList sl;
   QString str;
   bool inDoubleQuoteFlag = false;
   
   //
   // Loop through the string keeping anything between double quotes intact
   //
   const int slen = s.length();
   for (int i = 0; i < slen; i++) {
      const QChar c = s[i];
      
      //
      // Beginning or end of double quote
      //
      if (c == '"') {
         str += c;
         
         if (inDoubleQuoteFlag) {
            //
            // String in double quotes is complete
            //
            sl += str;
            str = "";
            inDoubleQuoteFlag = false;
         }
         else {
            //
            // Starting string in double quotes
            //
            inDoubleQuoteFlag = true;
         }
      }
      else if ((c == ' ') ||
               (c == '\n') ||
               (c == '\r')) {
         if (inDoubleQuoteFlag) {
            //
            // Keep blanks in double quotes
            //
            str += c;
         }
         else {
            if (str.isEmpty() == false) {
               //
               // Conclude string
               //
               sl += str;
               str = "";
            }
         }
      }
      else {
         //
         // Add character to string
         //
         str += c;
      }
   }

   //
   // Use any remaining string at end of text
   //
   if (str.isEmpty() == false) {
      sl += str;
   }
   //QStringList sl = s.split(' ', QString::SkipEmptyParts);

   return sl;
}
   
/**
 * set parameter value from text.
 */
void 
GuiCaretCommandParameterVariableList::setParameterValueFromText(const QString& s)
{
   textEdit->setText(s);
}
      
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

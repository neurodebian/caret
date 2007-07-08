
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

#include <sstream>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include "GuiScriptArg.h"
#include "GuiScriptOperation.h"

/**
 * constructor.
 */
GuiScriptOperation::GuiScriptOperation(QWidget* /*parentWidget*/,
                   const QString& commandDescriptiveNameIn,
                   const QString& explanationTextIn,
                   const QString& programNameIn,
                   const QString& programArgsIn)
{
   commandDescriptiveName = commandDescriptiveNameIn;
   explanationText = explanationTextIn;
   programName = programNameIn;
   programArgs = programArgsIn;
   
   gridWidget = new QWidget;
   gridLayout = new QGridLayout(gridWidget);
   gridLayout->setSpacing(5);
   
   gridLayout->addWidget(new QLabel("Comment "), 0, 0);
   commentLineEdit = new QLineEdit;
   gridLayout->addWidget(commentLineEdit, 0, 1);
}
                  
/**
 * destructor.
 */
GuiScriptOperation::~GuiScriptOperation()
{
   for (unsigned int i = 0; i < operationParameters.size(); i++) {
      delete operationParameters[i];
   }
   operationParameters.clear();
}

/**
 * get operation comment.
 */
QString 
GuiScriptOperation::getOperationComment() const 
{ 
   return commentLineEdit->text().trimmed(); 
}
      
/**
 * get the command for execution as a string.
 */
bool 
GuiScriptOperation::getCommand(QString& programNameOut,
                               QStringList& programArgsOut,
                               QString& errorMessageOut) const
{
   errorMessageOut = "";
   
   programArgsOut.clear();
   programNameOut = programName;

   if (programArgs.isEmpty() == false) {
      programArgsOut << programArgs;  
   }
   
   for (unsigned int i = 0; i < operationParameters.size(); i++) {
      const GuiScriptArg* op = operationParameters[i];
      QString msg;
      if (op->getValueValid(msg)) {
         const QString s(op->getValue());
         programArgsOut << s;
         //str << s.toAscii().constData() << " ";
      }
      else {
         if (errorMessageOut.isEmpty() == false) {
            errorMessageOut.append("\n");
         }
         errorMessageOut.append(msg);
      }
   }
   
   return (errorMessageOut.isEmpty());
}

/**
 * add an axis parameter.
 */
GuiScriptAxisArg*
GuiScriptOperation::addAxisParameter(const QString& label)
{
   GuiScriptAxisArg* op = new GuiScriptAxisArg(gridLayout,
                                                     label);
   operationParameters.push_back(op);
   return op;
}

/**
 * add a boolean parameter.
 */
GuiScriptBoolArg* 
GuiScriptOperation::addBoolParameter(const QString& label)
{
   GuiScriptBoolArg* op = new GuiScriptBoolArg(gridLayout,
                                               label);
   operationParameters.push_back(op);
   return op;
}

/**
 * add an int parameter.
 */
GuiScriptIntArg* 
GuiScriptOperation::addIntParameter(const QString& label)
{
   GuiScriptIntArg* op = new GuiScriptIntArg(gridLayout,
                                             label);
   operationParameters.push_back(op);
   return op;
}

/**
 * add a float parameter.
 */
GuiScriptFloatArg*
GuiScriptOperation::addFloatParameter(const QString& label)
{
   GuiScriptFloatArg* op = new GuiScriptFloatArg(gridLayout,
                                                 label);
   operationParameters.push_back(op);
   return op;
}

/**
 * add a string parameter.
 */
GuiScriptStringArg* 
GuiScriptOperation::addStringParameter(const QString& label,
                                       const QString& defaultValue)
{
   GuiScriptStringArg* op = new GuiScriptStringArg(gridLayout,
                                                   label,
                                                   defaultValue);
   operationParameters.push_back(op);
   return op;
}
      
// add a generic file name parameter
GuiScriptFileNameArg* 
GuiScriptOperation::addGenericFileParameter(const QString& label,
                                            const QString& fileFilter,
                                            const bool existingFilesOnlyFlag,
                                            const bool fileIsOptionalFlag)
{
   GuiScriptFileNameArg* op = new GuiScriptFileNameArg(
                                                 gridLayout,
                                                 label,
                                                 fileFilter,
                                                 existingFilesOnlyFlag,
                                                 fileIsOptionalFlag);
   operationParameters.push_back(op);
   return op;
}                                                    
                                                    
/**
 * add a spec file name parameter.
 */
GuiScriptFileNameArg* 
GuiScriptOperation::addSpecFileParameter(const QString& label, 
                                         const bool fileIsOptional)
{
   const bool existingFilesOnlyFlag = false;
   GuiScriptFileNameArg* op = new GuiScriptFileNameArg(
                                                 gridLayout,
                                                 label,
                                                 GuiScriptFileNameArg::FILE_TYPE_SPEC,
                                                 existingFilesOnlyFlag,
                                                 fileIsOptional);
   operationParameters.push_back(op);
   return op;
}

/**
 * add a directory name parameter.
 */
GuiScriptDirectoryArg* 
GuiScriptOperation::addDirectoryParameter(const QString& label)
{
   GuiScriptDirectoryArg* op = new GuiScriptDirectoryArg(gridLayout,
                                                         label);
   operationParameters.push_back(op);
   return op;
}
      
/**
 * add a limits file name parameter.
 */
GuiScriptFileNameArg* 
GuiScriptOperation::addLimitsFileParameter(const QString& label)
{
   const bool existingFilesOnlyFlag = false;
   GuiScriptFileNameArg* op = new GuiScriptFileNameArg(
                                                 gridLayout,
                                                 label,
                                                 GuiScriptFileNameArg::FILE_TYPE_LIMITS,
                                                 existingFilesOnlyFlag);
   operationParameters.push_back(op);
   return op;
}

/**
 * add an input volume file name parameter.
 */
GuiScriptFileNameArg* 
GuiScriptOperation::addInputVolumeFileParameter(const QString& label,
                                                const bool optionalFileNameFlag)
{
   const bool existingFilesOnlyFlag = false;
   GuiScriptFileNameArg* op = new GuiScriptFileNameArg(
                                                 gridLayout,
                                                 label,
                                                 GuiScriptFileNameArg::FILE_TYPE_VOLUME_INPUT,
                                                 existingFilesOnlyFlag,
                                                 optionalFileNameFlag);
   operationParameters.push_back(op);
   return op;
}

/**
 * add an output volume file name parameter.
 */
GuiScriptFileNameArg* 
GuiScriptOperation::addOutputVolumeFileParameter(const QString& label)
{
   const bool existingFilesOnlyFlag = false;
   GuiScriptFileNameArg* op = new GuiScriptFileNameArg(
                                                 gridLayout,
                                                 label,
                                                 GuiScriptFileNameArg::FILE_TYPE_VOLUME_OUTPUT,
                                                 existingFilesOnlyFlag);
   operationParameters.push_back(op);
   return op;
}

/**
 * add a vector file parameter.
 */
GuiScriptFileNameArg* 
GuiScriptOperation::addVectorFileParameter(const QString& label)
{
   const bool existingFilesOnlyFlag = false;
   GuiScriptFileNameArg* op = new GuiScriptFileNameArg(
                                                 gridLayout,
                                                 label,
                                                 GuiScriptFileNameArg::FILE_TYPE_VECTOR,
                                                 existingFilesOnlyFlag);
   operationParameters.push_back(op);
   return op;
}

/**
 * add a seed parameter.
 */
GuiScriptSeedArg* 
GuiScriptOperation::addSeedParameter(const QString& label)
{
   GuiScriptSeedArg* op = new GuiScriptSeedArg(
                                           gridLayout,
                                           label);
   operationParameters.push_back(op);
   return op;
}

/**
 * add an extent parameter.
 */
GuiScriptExtentArg* 
GuiScriptOperation::addExtentParameter(const QString& label)
{
   GuiScriptExtentArg* op = new GuiScriptExtentArg(
                                           gridLayout,
                                           label);
   operationParameters.push_back(op);
   return op;
}

/**
 * add a combo box parameter.
 */
GuiScriptComboBoxArg* 
GuiScriptOperation::addComboBoxParameter(const QString& label,
                                         const std::vector<QString>& comboBoxLabels,
                                         const std::vector<QString>& comboBoxValues,
                                         const int defaultItem)
{
   GuiScriptComboBoxArg* op = new GuiScriptComboBoxArg(gridLayout,
                                                          label,
                                                          comboBoxLabels,
                                                          comboBoxValues,
                                                          defaultItem);
   operationParameters.push_back(op);
   return op;
}


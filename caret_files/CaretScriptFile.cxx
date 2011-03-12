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

#include <iostream>
#include <set>

#include <QApplication>
#include <QCursor>
#include <QDomDocument>
#include <QDomElement>
#include <QInputDialog>
#include <QProcess>
#include <QProgressDialog>

#include "CaretScriptFile.h"
#include "CommandScriptComment.h"
#include "CommandScriptVariableRead.h"
#include "CommandScriptVariableSet.h"
#include "DebugControl.h"
#include "ProgramParameters.h"
#include "QtScriptInputDialog.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
CaretScriptFile::CaretScriptFile()
   : AbstractFile("Caret Script File",
                  SpecFile::getCaretScriptFileExtension(),
                  true,                 // has header
                  FILE_FORMAT_XML,      // default write type
                  FILE_IO_NONE,         // ASCII read/write
                  FILE_IO_NONE,         // Binary read/write
                  FILE_IO_READ_AND_WRITE,  // XML read/write
                  FILE_IO_NONE,         // XML Base64 read/write
                  FILE_IO_NONE,         // XML GZip Base64
                  FILE_IO_NONE,         // other read/write
                  FILE_IO_NONE)         // CSVF read/write
{
   clear();
}

/**
 * destructor.
 */
CaretScriptFile::~CaretScriptFile()
{
   clear();
}

/**
 * call AbstractFile::clearAbstractFile() from its clear method..
 */
void 
CaretScriptFile::clear()
{
   AbstractFile::clearAbstractFile();
   const int num = static_cast<int>(commandOperations.size());
   for (int i = 0; i < num; i++) {
      delete commandOperations[i];
      commandOperations[i] = NULL;
   }
   commandOperations.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
CaretScriptFile::empty() const
{
   return true;
}

/**
 * add an operation.
 */
void 
CaretScriptFile::addCommandOperation(CaretCommandOperation* commandOperation) 
{ 
   commandOperations.push_back(commandOperation);
   setModified();
}
      
/**
 * delete an operation.
 */
void 
CaretScriptFile::deleteOperation(CaretCommandOperation* commandOperation)
{
   const int num = getNumberOfCommandOperations();
   for (int i = 0; i < num; i++) {
      CaretCommandOperation* cco = getCommandOperation(i);
      if (cco == commandOperation) {
         deleteOperation(i);
         break;
      }
   }
}
      
/**
 * delete an operation.
 */
void 
CaretScriptFile::deleteOperation(const int indx)
{
   commandOperations.erase(commandOperations.begin() + indx);
}
      
/**
 * run the commands in the script file.
 * If parent is a widget, a progress dialog will be displayed.
 * If parent is zero, no progress dialog is displayed.
 */
void 
CaretScriptFile::runCommandsInFile(QWidget* parentWidget,
                                   const QString& caretCommandProgramName,
                                   QString& commandsOutputText) throw (FileException)
{
   commandsOutputText.clear();
   
   QString errorMessage;

   //
   // Names of variables and their values
   //
   std::multiset<Variable> variables;
   
   //
   // Get number of command operations
   //
   const int num = getNumberOfCommandOperations();
   if (num <= 0) {
      return;
   }
   
   //
   // Command used for reading and setting variables
   //
   CommandScriptComment commandScriptComment;
   CommandScriptVariableRead commandScriptVariableRead;
   CommandScriptVariableSet commandScriptVariableSet;
   
   //
   // Use progress dialog only if parent widget is valid
   //
   QProgressDialog* progressDialog = NULL;
   if (parentWidget != NULL) {
      progressDialog = new QProgressDialog(parentWidget);
      progressDialog->setMinimum(0);
      progressDialog->setMaximum(num);
      progressDialog->setMinimumDuration(0);
   }
   
   //
   // Loop through the commands
   //
   for (int i = 0; i < num; i++) {
      //
      // Get the operation
      //
      const CaretCommandOperation* op = getCommandOperation(i);
      const QString commandSwitch = op->getSwitch();
      QStringList commandParameters = op->getParametersForCommandExecution();

      //
      // If this is neither a read nor set variable command
      //
      if ((commandSwitch != commandScriptVariableRead.getOperationSwitch()) &&
          (commandSwitch != commandScriptVariableSet.getOperationSwitch()) &&
          (commandSwitch != commandScriptComment.getOperationSwitch())) {
         //
         // Replace any variables in the parameters with the corresponding value
         //
         for (int j = 0; j < commandParameters.count(); j++) {
            QString paramName = commandParameters.at(j);
            
            //
            // Try all variable substitutions
            //
            for (std::multiset<Variable>::iterator iter = variables.begin();
                 iter != variables.end(); 
                 iter++) {
               paramName = paramName.replace(iter->getVariableName(),
                                             iter->getVariableValue());
            }
            if (paramName.indexOf('$') >= 0) {
               throw FileException("Variable substitution failed for \""
                                      + paramName
                                      + "\"");
            }
            
            commandParameters[j] = paramName;
         }
      }
      
      //
      // Assemble the parameters
      //
      QStringList commandSwitchAndParameters;
      commandSwitchAndParameters << commandSwitch;
      commandSwitchAndParameters << commandParameters;
      const QString cmdText = caretCommandProgramName
                             + " "
                             + commandSwitchAndParameters.join(" ");
     
      const QString comment(op->getComment());
      
      if (DebugControl::getDebugOn()) {
         std::cout << "Comment: " 
                  << comment.toAscii().constData()
                  << std::endl;
                  
         std::cout << "Command: "
                  << cmdText.toAscii().constData()
                  << std::endl;
      }
      
      //
      // Remove double quotes from any parameters
      //
      for (int m = 0; m < commandSwitchAndParameters.count(); m++) {
         //
         // Remove anything enclosed in double quotes
         //
         QString param = commandSwitchAndParameters[m];
         if (param.startsWith("\"") &&
             param.endsWith("\"")) {
            const int len = param.length();
            if (len >= 2) {
               param = param.mid(1, len - 2);
               commandSwitchAndParameters[m] = param;
            }
         }
      }
      
      //
      // Update progress dialog
      //
      if (progressDialog != NULL) {
         allowEventsToProcess();
      
         progressDialog->setValue(i);
         progressDialog->setLabelText("Running "
                                      + op->getShortDescription());
         progressDialog->show();
         
         if (progressDialog->wasCanceled()) {
            throw FileException("Script execution canceled by user.");
         }
      }
      else {
         std::cout << "Running "
                   << op->getShortDescription().toAscii().constData()
                   << std::endl;
      }
      
      //
      // Allow other events to be processed (do not hog CPU)
      //
      allowEventsToProcess();
      
      //
      // Is this a read parameters command ?
      //
      if (commandSwitch == commandScriptVariableRead.getOperationSwitch()) {
         //
         // Create the parameters and get the variable name and value
         //
         ProgramParameters setVarParams(caretCommandProgramName,
                                        commandSwitchAndParameters);
         commandScriptVariableRead.setParameters(&setVarParams);
         const QString switchName = setVarParams.getNextParameterAsString("switch"); //skip switch
         try {
            //
            // Execute to get variable name and prompt message
            //
            commandScriptVariableRead.execute(errorMessage);
            const QString variableName = commandScriptVariableRead.getVariableName();
            const QString promptMessage = commandScriptVariableRead.getPromptMessage();
            if (DebugControl::getDebugOn()) {
               std::cout << "Variable: "
                         << variableName.toAscii().constData()
                         << std::endl
                         << "   Prompt: " 
                         << promptMessage.toAscii().constData()
                         << std::endl;
            }
            
            //
            // If GUI available?
            //
            QString variableValue;
            if (progressDialog != NULL) {
               //
               // Use GUI to get variable's value
               //
               QApplication::restoreOverrideCursor();
               QtScriptInputDialog inputDialog(progressDialog,
                                         promptMessage,
                                         true);
               if (inputDialog.exec() == QtScriptInputDialog::Accepted) {
                  variableValue = inputDialog.getInputText();
               }
               else {
                  commandsOutputText = "";
                  errorMessage = "Cancel pressed.  Script execution terminated.";
               }
               QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            }
            else {
               //
               // Read variables value from terminal
               //
               std::cout << promptMessage.toAscii().constData() << std::endl;
               std::string str;
               std::getline(std::cin, str);
               variableValue = StringUtilities::fromStdString(str);
            }
            
            //
            // Save variable names and values
            //
            variables.insert(Variable(variableName, variableValue));
         }
         catch (CommandException& e) {
           throw FileException(e.whatQString());
         }         
      }
      else if (commandSwitch == commandScriptVariableSet.getOperationSwitch()) { // set variable
         //
         // Create the parameters and get the variable name and value
         //
         ProgramParameters setVarParams(caretCommandProgramName,
                                        commandSwitchAndParameters);
         commandScriptVariableSet.setParameters(&setVarParams);
         const QString switchName = setVarParams.getNextParameterAsString("switch"); //skip switch
         try {
            //
            // Execute to get variable name and value
            //
            commandScriptVariableSet.execute(errorMessage);
            const QString variableName = commandScriptVariableSet.getVariableName();
            const QString variableValue = commandScriptVariableSet.getVariableValue();
            if (DebugControl::getDebugOn()) {
               std::cout << "Variable: "
                         << variableName.toAscii().constData()
                         << std::endl
                         << "   Value: " 
                         << variableValue.toAscii().constData()
                         << std::endl;
            }
            
            //
            // Save variable names and values
            //
            variables.insert(Variable(variableName, variableValue));
         }
         catch (CommandException& e) {
           throw FileException(e.whatQString());
         }         
      }
      else { // normal command so run it
         //
         // Create a new QProcess and add its arguments
         //
         QProcess process(parentWidget);
         
         //
         // Start execution of the command
         //
         process.start(caretCommandProgramName, commandSwitchAndParameters);
         if (!process.waitForStarted()) {
            QString msg("Error starting command: ");
            msg.append(cmdText);
            errorMessage.append(msg);
         }
         
         //
         // Wait until the program is complete
         //
         if (!process.waitForFinished(100000000)) {
            QString msg("Error waiting for command to finish: ");
            msg.append(caretCommandProgramName);
            msg.append(cmdText);
            msg.append("\nError Message" + process.errorString());
            errorMessage.append(msg);
         }
         
         //const QString processOutput(process.readAll());
         QString processOutput(process.readAllStandardOutput());
         processOutput.append(process.readAllStandardError());
         commandsOutputText.append(processOutput);
         
         if (process.exitStatus() == QProcess::NormalExit) {
            if (process.exitCode() == 0) {
               commandsOutputText.append("COMMAND SUCCESSFUL: ");
               commandsOutputText.append(cmdText + "\n\n");
            }
            else {
               //std::cout << "Text output: "
               //          << commandsOutputText.toAscii().constData()
               //          << std::endl;
               errorMessage.append("COMMAND FAILED1: ");
               errorMessage.append(cmdText);
               errorMessage.append("\nExit Code " + QString::number(process.exitCode()));
               errorMessage.append("\nError Message" + process.errorString());
               errorMessage.append("\nCommand output: "
                                   + processOutput);
            }
         }
         else {
            errorMessage.append("COMMAND FAILED2: ");
            errorMessage.append(cmdText + processOutput);
         }
      }
      
      //
      // Did something go wrong
      //
      if (errorMessage.isEmpty() == false) {
         break;
      }
   }

   //
   // Remove progress dialog
   //
   if (progressDialog != NULL) {
      progressDialog->setValue(progressDialog->maximum());
      delete progressDialog;
      progressDialog = NULL;
   }

   if (errorMessage.isEmpty() == false) {
      commandsOutputText.append("\n" + errorMessage);
      throw FileException(errorMessage);
   }      
}

/**
 * Read the contents of the file (header has already been read).
 */
void 
CaretScriptFile::readFileData(QFile& /*file*/,
                              QTextStream& /*stream*/,
                              QDataStream& /*binStream*/,
                              QDomElement& rootElement) throw (FileException)
{
   //       
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Reading in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) {
                  //
                  // Is this a "CellData" element
                  //
                  if (elem.tagName() == "Command") {
                     CaretCommandOperation* op = new CaretCommandOperation;
                     op->readXML(node);
                     addCommandOperation(op);
                  }
                  else if ((elem.tagName() == xmlHeaderOldTagName) ||
                           (elem.tagName() == xmlHeaderTagName)) {
                     // ignore, read by AbstractFile::readFile()
                  }
                  else {                     
                      std::cout << "WARNING: unrecognized Caret Script File element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
                  }
               }
               node = node.nextSibling();
            }  
         }     
         break;   
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;   
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");         
         break;      
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Reading XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:         
         throw FileException(filename, "Reading in Other format not supported.");
         break;      
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:         
         throw FileException(filename, "Reading in Comma Separated Value File format not supported.");
         break;     
   }
}

/**
 * Write the file's data (header has already been written).
 */
void 
CaretScriptFile::writeFileData(QTextStream& /*stream*/,
                               QDataStream& /*binStream*/,
                               QDomDocument& xmlDoc,
                               QDomElement& rootElement) throw (FileException)
{
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Writing in Ascii format not supported.");
         break; 
      case FILE_FORMAT_BINARY:            
         throw FileException(filename, "Writing in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            const int num = getNumberOfCommandOperations();
            for (int i = 0; i < num; i++) {
               CaretCommandOperation* op = getCommandOperation(i);
               op->writeXML(xmlDoc, rootElement);
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Writing in XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Writing in XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Writing in Comma Separated Value File format not supported.");
         break;
   }
}

//=======================================================================

/**
 * get parameters for command execution (variable list is split up).
 */
QStringList 
CaretScriptFile::CaretCommandOperation::getParametersForCommandExecution() const
{
   QStringList paramsOut;
   
   //
   // Loop through parameters
   //
   const int numParams = parameters.count();
   for (int i = 0; i < numParams; i++) {
      //
      // Get the parameter
      //
      const QString param(parameters.at(i));
      
      //
      // is this the last parameter and is last parameter
      // a variable list
      //
      if ((i == (numParams - 1)) &&
          (lastParameterIsVariableListFlag)) {
         QStringList splitParams;
         StringUtilities::tokenStringsWithQuotes(param, splitParams);
         paramsOut << splitParams;
      }
      else {
         paramsOut << param;
      }
   }
   
   return paramsOut;
}
            
/**
 * read from XML.
 */
void 
CaretScriptFile::CaretCommandOperation::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != "Command") {
      QString msg("Incorrect element type passed to CaretCommandOperation::readXML(): \"");
      msg.append(elem.tagName());
      msg.append("\"");
      throw FileException("", msg);
   }

   commandSwitch = "";
   parameters.clear();
   lastParameterIsVariableListFlag = false;
   comment = "";
   shortDescription = "";
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "commandSwitch") {
            commandSwitch = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "comment") {
            comment = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "parameters") {
            const QString s = AbstractFile::getXmlElementFirstChildAsString(elem);
            parameters.append(s);
         }
         else if (elem.tagName() == "shortDescription") {
            shortDescription = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "lastParameterIsVariableListFlag") {
            lastParameterIsVariableListFlag = false;
            if (AbstractFile::getXmlElementFirstChildAsString(elem) == "true") {
               lastParameterIsVariableListFlag = true;
            }
         }
         else {
            std::cout << "WARNING: unrecognized CaretCommandOperation element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      node = node.nextSibling();
   }
}
            
/**
 * write as XML.
 */
void 
CaretScriptFile::CaretCommandOperation::writeXML(QDomDocument& xmlDoc,
                                                 QDomElement&  parentElement)
{
   //
   // Create the element for the command
   //
   QDomElement commandElement = xmlDoc.createElement("Command");
   
   //
   // Create the elements for the switch and the parameters
   //
   AbstractFile::addXmlCdataElement(xmlDoc, commandElement, "commandSwitch", commandSwitch);
   AbstractFile::addXmlCdataElement(xmlDoc, commandElement, "comment", comment);
   AbstractFile::addXmlCdataElement(xmlDoc, commandElement, "shortDescription", shortDescription);
   for (int i = 0; i < parameters.count(); i++) {
      AbstractFile::addXmlCdataElement(xmlDoc, commandElement, "parameters", parameters.at(i));
   }
   AbstractFile::addXmlCdataElement(xmlDoc, commandElement, "lastParameterIsVariableListFlag", 
                                    (lastParameterIsVariableListFlag ? "true" : "false"));
   
   //
   // Add to parent element
   //
   parentElement.appendChild(commandElement);
}

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

#include "CaretScriptFile.h"
#include "CommandScriptComment.h"
#include "CommandScriptConvert.h"
#include "CommandScriptVariableRead.h"
#include "CommandScriptVariableSet.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandScriptConvert::CommandScriptConvert()
   : CommandBase("-script-convert",
                 "SCRIPT CONVERT")
{
}

/**
 * destructor.
 */
CommandScriptConvert::~CommandScriptConvert()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandScriptConvert::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   QStringList shellFilters;
   shellFilters << FileFilters::getAnyFileFilter();
   shellFilters << FileFilters::getBourneShellFileFilter();
   shellFilters << FileFilters::getCShellFileFilter();
   shellFilters << FileFilters::getWindowsBATFileFilter();
   shellFilters << FileFilters::getPythonFileFilter();
   
   paramsOut.clear();
   paramsOut.addFile("Input Caret Script File Name", FileFilters::getCaretScriptFileFilter());
   paramsOut.addFile("Output Shell File Name", shellFilters);
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandScriptConvert::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-caret-script-file-name>\n"
       + indent9 + "<output-file-name>\n"
       + indent9 + "   [-interpreter   interpreter-name]\n"
       + indent9 + "\n"
       + indent9 + "Create a scripting language file from the commands \n"
       + indent9 + "contained in a caret script file.\n"
       + indent9 + "\n"
       + indent9 + "The scripting language is determined by the filename \n"
       + indent9 + "extension of the output file name.  \n"
       + indent9 + "   file-name     scripting       default\n"
       + indent9 + "   extension     language        interpreter\n"
       + indent9 + "   ---------     ---------       -----------\n"
       + indent9 + "     .bat        DOS             Windows BAT File\n"
       + indent9 + "     .csh        C-Shell         /bin/csh \n"
       + indent9 + "     .py         Python          /usr/bin/python \n"
       + indent9 + "     .sh         bourne shell    /bin/sh \n"
       + indent9 + "\n"
       + indent9 + "WARNING: The C-Shell appears to limit variable names to a\n"
       + indent9 + "maximum length of 31 characters.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandScriptConvert::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString inputCaretScriptFileName =
      parameters->getNextParameterAsString("Input Caret Script File Name");
   const QString outputFileName =
      parameters->getNextParameterAsString("Output File Name");
      
   QString interpreterOverrideName;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Script Convert Parameter");
      if (paramName == "-interpreter") {
         interpreterOverrideName = parameters->getNextParameterAsString("Interpreter Name");
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }
   
   //
   // Check the output file type
   //
   QString interpreterName;
   OUTPUT_FILE_TYPE outputFileType;
   if (outputFileName.endsWith(".sh")) {
      interpreterName = "#!/bin/sh";
      outputFileType = OUTPUT_FILE_TYPE_BOURNE_SHELL;
   }
   else if (outputFileName.endsWith(".csh")) {
      interpreterName = "#!/bin/csh";
      outputFileType = OUTPUT_FILE_TYPE_C_SHELL;
   }
   else if (outputFileName.endsWith(".bat")) {
      outputFileType = OUTPUT_FILE_TYPE_DOS_SHELL;
   }
   else if (outputFileName.endsWith(".py")) {
      interpreterName = "#!/usr/bin/python";
      outputFileType = OUTPUT_FILE_TYPE_PYTHON;
   }
   else {
      throw CommandException("Invalid output file name extension: "
                             + outputFileName);
   }
   
   //
   // Possibly override name of interpreter
   //
   if (interpreterOverrideName.isEmpty() == false) {
      interpreterName = interpreterOverrideName;
   }
   
   //
   // Read the Caret script file
   //
   CaretScriptFile scriptFile;
   scriptFile.readFile(inputCaretScriptFileName);
   
   //
   // Create a text file 
   //
   TextFile textFile;
   
   //
   // Variable name of caret command program
   //
   QString caretCommandProgramVariableName("CARET_COMMAND_PROGRAM");
    
   //
   // Add the appropriate interpreter as the first line
   //
   switch (outputFileType) {
      case OUTPUT_FILE_TYPE_BOURNE_SHELL:
         textFile.appendLine(interpreterName);
         textFile.appendLine(" ");
         textFile.appendLine(caretCommandProgramVariableName
                             + "=\""
                             + parameters->getProgramNameWithoutPath()
                             + "\"");
         textFile.appendLine(" ");
         caretCommandProgramVariableName.insert(0, "${");
         caretCommandProgramVariableName += "}";
         break;
      case OUTPUT_FILE_TYPE_C_SHELL:
         textFile.appendLine(interpreterName);
         textFile.appendLine(" ");
         textFile.appendLine("set "
                             + caretCommandProgramVariableName
                             + "=\""
                             + parameters->getProgramNameWithoutPath()
                             + "\"");
         textFile.appendLine(" ");
         caretCommandProgramVariableName.insert(0, "${");
         caretCommandProgramVariableName += "}";
         break;
      case OUTPUT_FILE_TYPE_DOS_SHELL:
         textFile.appendLine(interpreterName);
         textFile.appendLine("set "
                             + caretCommandProgramVariableName
                             + "=\""
                             + parameters->getProgramNameWithoutPath()
                             + "\"");
         textFile.appendLine(" ");
         caretCommandProgramVariableName.insert(0, "%");
         caretCommandProgramVariableName += "%";
         break;
      case OUTPUT_FILE_TYPE_PYTHON:
         textFile.appendLine(interpreterName);
         textFile.appendLine("");
         textFile.appendLine("import os");
         textFile.appendLine("import sys");
         textFile.appendLine("");
         textFile.appendLine(caretCommandProgramVariableName
                             + " = \""
                             + parameters->getProgramNameWithoutPath()
                             + "\"");
         textFile.appendLine(" ");
         break;
   }
   
   //
   // Switches for some commands
   //
   CommandScriptComment scriptComment;
   const QString commentSwitch = scriptComment.getOperationSwitch();
   CommandScriptVariableSet scriptSetVariable;
   const QString variableSetSwitch = scriptSetVariable.getOperationSwitch();
   CommandScriptVariableRead scriptVariableRead;
   const QString variableReadSwitch = scriptVariableRead.getOperationSwitch();
   
   //
   // Loop through the operations
   //
   const int num = scriptFile.getNumberOfCommandOperations();
   for (int i = 0; i < num; i++) {
      //
      // Get the command
      //
      CaretScriptFile::CaretCommandOperation* op = scriptFile.getCommandOperation(i);
      //
      // Add the comment
      //
      switch (outputFileType) {
         case OUTPUT_FILE_TYPE_BOURNE_SHELL:
         case OUTPUT_FILE_TYPE_C_SHELL:
         case OUTPUT_FILE_TYPE_PYTHON:
            textFile.appendLine("#\n# "
                                + op->getComment()
                                + "\n#");
            break;
         case OUTPUT_FILE_TYPE_DOS_SHELL:
            textFile.appendLine("REM\nREM "
                                + op->getComment()
                                + "\nREM");
            break;
      }
      
      //
      // Special handle comment and variable read/set
      //
      if (op->getSwitch() == variableReadSwitch) {
         const QStringList params = op->getParametersForGUI();
         if (params.count() < 2) {
            throw CommandException("Parameters missing for variable read (command #"
                                   + QString::number(i + 1)
                                   + ": \""
                                   + params.join(" ")
                                   + "\"");
         }
         const QString varName = params.at(0);
         const QString promptString = params.at(1);
         if (varName.isEmpty()) {
            throw CommandException("Missing variable name for variable read (command #"
                                   + QString::number(i + 1)
                                   + ")");
         }
         if (varName.isEmpty()) {
            throw CommandException("Missing prompt string for variable read (command #"
                                   + QString::number(i + 1)
                                   + ")");
         }
         
         switch (outputFileType) {
            case OUTPUT_FILE_TYPE_BOURNE_SHELL:
               textFile.appendLine("echo \""  
                                   + promptString
                                   + "\"");
               textFile.appendLine("read "
                                   + varName.mid(1));
               break;
            case OUTPUT_FILE_TYPE_C_SHELL:
               textFile.appendLine("echo \""  
                                   + promptString
                                   + "\"");
               textFile.appendLine("set "
                                   + varName.mid(1)
                                   + "=$<");
               break;
            case OUTPUT_FILE_TYPE_DOS_SHELL:
              textFile.appendLine("set /p "
                                  + varName.mid(1) // remove '$' character
                                  + "=\""
                                  + promptString
                                  + " ");
               break;
            case OUTPUT_FILE_TYPE_PYTHON:
               textFile.appendLine("print \""
                                   + promptString
                                   + "\"");
               textFile.appendLine(varName.mid(1)
                                   + " = sys.stdin.readline()");
               break;
         }
      }
      else if (op->getSwitch() == variableSetSwitch) {
         const QStringList params = op->getParametersForGUI();
         if (params.count() < 2) {
            throw CommandException("Parameters missing for variable set (command #"
                                   + QString::number(i + 1)
                                   + ": \""
                                   + params.join(" ")
                                   + "\"");
         }
         const QString varName = params.at(0);
         const QString varValue = params.at(1);
         if (varName.isEmpty()) {
            throw CommandException("Missing variable name for variable set (command #"
                                   + QString::number(i + 1)
                                   + ")");
         }
         if (varValue.isEmpty()) {
            throw CommandException("Missing variable value for variable set (command #"
                                   + QString::number(i + 1)
                                   + ")");
         }
         
         switch (outputFileType) {
            case OUTPUT_FILE_TYPE_BOURNE_SHELL:
               textFile.appendLine(varName.mid(1) // remove '$' character
                                   + "=\""
                                   + varValue
                                   + "\"");
               break;
            case OUTPUT_FILE_TYPE_C_SHELL:
               textFile.appendLine("set "
                                   + varName.mid(1) // remove '$' character
                                   + "=\""
                                   + varValue
                                   + "\"");
               if (varName.mid(1).length() > 31) {
                  std::cout << "WARNING: variable named \""
                            << varName.toAscii().constData()
                            << "\" is more than 31 characters which may not be "
                            << "accepted by the C-Shell."
                            << std::endl;
               }
               break;
            case OUTPUT_FILE_TYPE_DOS_SHELL:
               textFile.appendLine("set "
                                   + varName.mid(1) // remove '$' character
                                   + "=\""
                                   + varValue
                                   + "\"");
               break;
            case OUTPUT_FILE_TYPE_PYTHON:
               textFile.appendLine(varName.mid(1) // remove '$' character
                                   + " = \""
                                   + varValue
                                   + "\"");
               break;
         }
      }
      else if (op->getSwitch() == commentSwitch) {
         const QStringList params = op->getParametersForGUI();
         if (params.count() > 0) {
            const QStringList commentLines = params.at(0).split('\n');
            switch (outputFileType) {
               case OUTPUT_FILE_TYPE_BOURNE_SHELL:
               case OUTPUT_FILE_TYPE_C_SHELL:
               case OUTPUT_FILE_TYPE_PYTHON:
                  textFile.appendLine("#");
                  for (int j = 0; j < commentLines.count(); j++) {
                     textFile.appendLine("#   " + commentLines.at(j));
                  }
                  textFile.appendLine("#");
                  break;
               case OUTPUT_FILE_TYPE_DOS_SHELL:
                  textFile.appendLine("REM");
                  for (int j = 0; j < commentLines.count(); j++) {
                     textFile.appendLine("REM   " + commentLines.at(j));
                  }
                  textFile.appendLine("REM");
                  break;
            }
         }
      }
      else {    
         //
         // Continue to next line character and newline
         //  
         QString linePrefix;
         QString lineSuffix;
         QString continueToNextLine(" \\\n");
         QString opSwitchPrefix;
         QString opSwitchSuffix;
         QString programSuffix;
         switch (outputFileType) {
            case OUTPUT_FILE_TYPE_BOURNE_SHELL:
            case OUTPUT_FILE_TYPE_C_SHELL:
               continueToNextLine= " \\\n";
               break;
            case OUTPUT_FILE_TYPE_DOS_SHELL:
               continueToNextLine = "";
               break;
            case OUTPUT_FILE_TYPE_PYTHON:
               linePrefix = "os.system(\" \".join((";
               lineSuffix = ")))";
               continueToNextLine= " \\\n";
               opSwitchPrefix = "\"";
               opSwitchSuffix = "\",";
               programSuffix = ",";
               break;
         }
         
         //
         // Create the line for execution
         //
         QString line(linePrefix);
         line += (caretCommandProgramVariableName + programSuffix + continueToNextLine);
         line += ("   " + opSwitchPrefix + op->getSwitch() + opSwitchSuffix
                  + continueToNextLine);
         const QStringList paramList = op->getParametersForCommandExecution();
         for (int i = 0; i < paramList.count(); i++) {
            QString param = paramList.at(i);
            const int paramLength = param.length();
            if (paramLength <= 0) {
               throw CommandException("parameter is blank for \""
                                      + op->getSwitch()
                                      + "\" (command #"
                                      + QString::number(i + 1)
                                      + ")");
            }
            
            //
            // Is this a variable
            //
            if (param[0] == '$') {
               switch (outputFileType) {
                  case OUTPUT_FILE_TYPE_BOURNE_SHELL:
                  case OUTPUT_FILE_TYPE_C_SHELL:
                     param.insert(1, "{");
                     param += "}";
                     break;
                  case OUTPUT_FILE_TYPE_DOS_SHELL:
                     param = ("%"
                              + param.mid(1)  // remove '$'
                              + "%");
                     break;
                  case OUTPUT_FILE_TYPE_PYTHON:
                     param = param.mid(1);    // remove '$'
                     break;
               }
            }
            else {
               switch (outputFileType) {
                  case OUTPUT_FILE_TYPE_BOURNE_SHELL:
                     break;
                  case OUTPUT_FILE_TYPE_C_SHELL:
                     break;
                  case OUTPUT_FILE_TYPE_DOS_SHELL:
                     break;
                  case OUTPUT_FILE_TYPE_PYTHON:
                     //
                     // If parameter is in quotes add another set
                     //
                     if (param.startsWith("\"")) { 
                        param.insert(param.length() - 1, "\\\"");
                        param.insert(1, "\\\"");
                     }
                     else {
                        param = "\"" + param + "\"";
                     }
                     break;
               }
            }
            
            switch (outputFileType) {
               case OUTPUT_FILE_TYPE_BOURNE_SHELL:
                  break;
               case OUTPUT_FILE_TYPE_C_SHELL:
                  break;
               case OUTPUT_FILE_TYPE_DOS_SHELL:
                  break;
               case OUTPUT_FILE_TYPE_PYTHON:
                  if (i < (paramList.count() - 1)) {
                     param += ",";
                  }
                  break;
            }

            if (i < (paramList.count() - 1)) {
               param += continueToNextLine;
            }
            line += ("   " + param);
         }
         
         //
         // add suffix to line
         //
         line += lineSuffix;
         
         //
         // Add to output file
         //
         textFile.appendLine(line);
      }
      
      //
      // Add blank line
      //
      textFile.appendLine(" ");
   }
   
   //
   // Write the output file
   //
   textFile.writeFile(outputFileName);   
}

      


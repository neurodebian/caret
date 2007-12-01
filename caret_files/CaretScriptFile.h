
#ifndef __CARET_SCRIPT_FILE_H__
#define __CARET_SCRIPT_FILE_H__

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

#include "AbstractFile.h"

class QDomNode;
class QWidget;

/// class for storing a caret command script
class CaretScriptFile : public AbstractFile {
   public:
      
      /// class for caret command operations
      class CaretCommandOperation {
         public:
            /// constructor
            CaretCommandOperation() {
               lastParameterIsVariableListFlag = false;
            }

            /// constructor
            CaretCommandOperation(const QString& commandSwitchIn,
                                  const QStringList& parametersIn,
                                  const bool lastParameterIsVariableListFlagIn,
                                  const QString& shortDescriptionIn,
                                  const QString& commentIn) {
               setSwitch(commandSwitchIn);
               setParameters(parametersIn);
               setLastParameterIsVariableListFlag(lastParameterIsVariableListFlagIn);
               setShortDescription(shortDescriptionIn);
               setComment(commentIn);
            }
            
            /// destructor
            ~CaretCommandOperation() { }
            
            /// get switch
            QString getSwitch() const { return commandSwitch; }
            
            /// get parameters for GUI display
            QStringList getParametersForGUI() const { return parameters; }
            
            /// get parameters for command execution (variable list is split up)
            QStringList getParametersForCommandExecution() const;
            
            /// get comment
            QString getComment() const { return comment; }
            
            /// get last parameter is variable list flag
            bool getLastParameterIsVariableListFlag() { return lastParameterIsVariableListFlag; }
            
            /// set last parameter is variable list flag
            void setLastParameterIsVariableListFlag(const bool flag) {
               lastParameterIsVariableListFlag = flag;
            }
            
            /// set the switch
            void setSwitch(const QString& switchIn) { commandSwitch = switchIn; }
            
            /// set parameters
            void setParameters(const QStringList& parametersIn) { parameters = parametersIn; }
            
            /// set comment
            void setComment(const QString& commentIn) { comment = commentIn; }
            
            /// get the short description
            QString getShortDescription() const { return shortDescription; }
            
            /// set the short description
            void setShortDescription(const QString& s) { shortDescription = s; }
            
            // read from XML
            void readXML(QDomNode& nodeIn) throw (FileException);
            
            // write as XML
            void writeXML(QDomDocument& xmlDoc,
                          QDomElement&  parentElement);
                          
         protected:
            // the command switch
            QString commandSwitch;
            
            // the parameters
            QStringList parameters;
            
            // the last parameter is variable list flag
            bool lastParameterIsVariableListFlag;
            
            // the comment
            QString comment;
            
            // the short description
            QString shortDescription;
      };
      
      /// class for sorting variable names by length
      class Variable {
         public:
            // constructor
            Variable(const QString& nameIn,
                     const QString& valueIn) {
               name = nameIn;
               value = valueIn;
               nameLength = name.length();
            }
            
            /// destructor
            ~Variable() { }
            
            /// get the variable name
            QString getVariableName() const { return name; }
            
            /// get the variable value
            QString getVariableValue() const { return value; }
            
            /// comparison operator so LONGEST FIRST
            bool operator<(const Variable& v) const { return nameLength > v.nameLength; }
         
         protected:
            /// name of variable
            QString name;
            
            /// value of variable
            QString value;
            
            /// length of variable name
            int nameLength;
      };
      
      // constructor 
      CaretScriptFile();
      
      // destructor 
      ~CaretScriptFile();
      
      // call AbstractFile::clearAbstractFile() from its clear method.
      void clear();

      // returns true if the file is isEmpty (contains no data)
      bool empty() const;

      // add an operation
      void addCommandOperation(CaretCommandOperation* commandOperation);
      
      // delete an operation
      void deleteOperation(CaretCommandOperation* commandOperation);
      
      // delete an operation
      void deleteOperation(const int indx);
      
      // get the number of operations
      int getNumberOfCommandOperations() const { return commandOperations.size(); }
      
      // get an operation
      CaretCommandOperation* getCommandOperation(const int indx) { return commandOperations[indx]; }
      
      // get an operation (const method)
      const CaretCommandOperation* getCommandOperation(const int indx) const { return commandOperations[indx]; }
      
      // run the commands in the script file
      void runCommandsInFile(QWidget* parentWidget,
                             const QString& caretCommandProgramName,
                             QString& commandsOutputText) throw (FileException);

   protected:
      // Read the contents of the file (header has already been read)
      void readFileData(QFile& file,
                        QTextStream& stream,
                        QDataStream& binStream,
                        QDomElement& rootElement) throw (FileException);

      // Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);
                         
      // the operations
      std::vector<CaretCommandOperation*> commandOperations;
};

#endif // __CARET_SCRIPT_FILE_H__


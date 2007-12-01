
#ifndef __COMMAND_BASE_H__
#define __COMMAND_BASE_H__

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

#include <QString>

#include "BrainModelAlgorithmException.h"
#include "CommandException.h"
#include "FileException.h"
#include "ProgramParametersException.h"
#include "ScriptBuilderParameters.h"
#include "StatisticException.h"

class GiftiNodeDataFile;
class NodeAttributeFile;
class ProgramParameters;
class VolumeFile;

/// base class for caret command line operations
class CommandBase {
   public:
      // constructor
      CommandBase(const QString& operationSwitchIn,
                  const QString& shortDescriptionIn);

      // destructor
      virtual ~CommandBase();

      // get all commands 
      static void getAllCommandsSortedBySwitch(std::vector<CommandBase*>& commandsOut);
                            
      // get all commands 
      static void getAllCommandsSortedByDescription(std::vector<CommandBase*>& commandsOut);
                            
      // set the parameters for the command
      void setParameters(ProgramParameters* parametersIn);
      
      /// get the operation switch
      QString getOperationSwitch() const { return operationSwitch; }
      
      /// get the short description of the command
      QString getShortDescription() const { return shortDescription; }
      
      // get full help information
      virtual QString getHelpInformation() const = 0;
      
      // execute the command (returns true if successful)
      bool execute(QString& errorMessageOut);
      
      /// get the script builder parameters
      virtual void getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const = 0;
      
      /// command requires GUI flag sent to QApplication to be true
      virtual bool getHasGUI() const { return false; }
      
      /// command has no parameters
      virtual bool commandHasNoParameters() const { return false; }
      
      /// get general help information
      static QString getGeneralHelpInformation();
      
      /// get the exit code
      int getExitCode() const { return exitCode; }
      
   protected:
      // get all commands 
      static void getAllCommandsUnsorted(std::vector<CommandBase*>& commandsOut);
                            
      // execute the command
      virtual void executeCommand() throw (BrainModelAlgorithmException,
                                           CommandException,
                                           FileException,
                                           ProgramParametersException,
                                           StatisticException) = 0;

      /// get the name and label from a volume file name
      void splitOutputVolumeNameIntoNameAndLabel(QString& nameInOut,
                                                 QString& labelOut) const;
                                     
      /// read a volume file
      void readVolumeFile(VolumeFile& vf,
                          const QString& name) throw (FileException);
                          
      /// write a volume file
      void writeVolumeFile(VolumeFile& vf,
                           const QString& name,
                           const QString& label) throw (FileException);

/*
      // get a node attribute file column number where input may be a column 
      // name or number.  Input numbers range 1..N and output column 
      // numbers range 0..(N-1)
      int getNodeAttributeFileColumnNumber(const GiftiNodeDataFile& gndf,
                           const QString& columnNameOrNumber) const throw (CommandException);
      
      // get a node attribute file column number where input may be a column 
      // name or number.  Input numbers range 1..N and output column 
      // numbers range 0..(N-1)
      int getNodeAttributeFileColumnNumber(const NodeAttributeFile& gndf,
                                           const QString& columnNameOrNumber) const throw (CommandException);
*/      
      /// checks for excessive parameters and throws exception if any are found
      void checkForExcessiveParameters() throw (CommandException);
      
      /// get pointer to empty parameters
      static ProgramParameters* getEmptyParameters();
      
      /// sets the exit code for the program (normally zero if no error).
      void setExitCode(const int exitCodeIn) { exitCode = exitCodeIn; }
      
      /// the switch that identifies the command
      QString operationSwitch;
      
      /// a short description of the command
      QString shortDescription;
      
      /// the commands parameters
      ProgramParameters* parameters;
      
      /// indentation of 3 spaces
      static const QString indent3;
      
      /// indentation of 6 spaces
      static const QString indent6; 
      
      /// indentation of 9 spaces
      static const QString indent9; 
      
      /// exit code for the program
      int exitCode;
};

#ifdef __COMMAND_BASE_MAIN__
const QString CommandBase::indent3 = "   ";
const QString CommandBase::indent6 = "      ";
const QString CommandBase::indent9 = "         ";
#endif // __COMMAND_BASE_MAIN__

#endif // __COMMAND_BASE_H__


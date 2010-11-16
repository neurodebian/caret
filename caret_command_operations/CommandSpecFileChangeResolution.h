
#ifndef __COMMAND_SPEC_FILE_CHANGE_RESOLUTION_H__
#define __COMMAND_SPEC_FILE_CHANGE_RESOLUTION_H__

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

#include "CommandBase.h"

class BrainModelSurface;
class BrainSet;

/// class for changing resolution of spec file and its data files
class CommandSpecFileChangeResolution : public CommandBase {
   public:
      // constructor 
      CommandSpecFileChangeResolution();
      
      // destructor
      ~CommandSpecFileChangeResolution();
      
      // get full help information
      QString getHelpInformation() const;
      
      // get the script builder parameters
      virtual void getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const;
      
   protected:
      // execute the command
      void executeCommand() throw (BrainModelAlgorithmException,
                                   CommandException,
                                   FileException,
                                   ProgramParametersException,
                                   StatisticException);

      QString createOutputSpecFileName(const QString& sourceSpecName,
                                       const int numberOfNodes);
                                       
      // create the output directory
      void createOutputDirectory(const QString& directoryName);

      // read the user's spec file
      BrainSet* readUsersSpecFile(const QString& userSpecFileName);

      // create a spherical surface from a fiducial surface
      BrainModelSurface* createSphereFromUsersFiducial(BrainSet* bs);

};

#endif // __COMMAND_SPEC_FILE_CHANGE_RESOLUTION_H__


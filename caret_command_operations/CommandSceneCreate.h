
#ifndef __COMMAND_SCENE_CREATE_H__
#define __COMMAND_SCENE_CREATE_H__

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

#include "BrainModel.h"
#include "CommandBase.h"
#include "VolumeFile.h"

class BrainModelSurface;
class BrainSet;

/// class for creating scenes
class CommandSceneCreate : public CommandBase {
   public:
      // constructor 
      CommandSceneCreate();
      
      // destructor
      ~CommandSceneCreate();
      
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

      // get surface standard view from its name
      BrainModel::STANDARD_VIEWS getSurfaceStandardView(const QString& viewName) const throw (CommandException);
      
      // get the number of a window from its name
      int getWindowNumber(const QString& windowName) const throw (CommandException);
      
      // set the default window scaling
      void setWindowScaling(BrainSet& bs,
                            const int sizeX,
                            const int sizeY);
                            
      // find a brain model surface
      BrainModelSurface* findBrainModelSurface(BrainSet& bs,
                                               const QString& coordFileName,
                                               const QString& topoFileName) 
                                                  const throw (CommandException);
                                               
      // find a brain model surface from types
      BrainModelSurface* findBrainModelSurfaceFromTypes(BrainSet& bs,
                                               const QString& coordFileType,
                                               const QString& topoFileType) 
                                                  const throw (CommandException);
                                                  
      // select a volume of the specified type
      void selectVolumeOfType(BrainSet& brainSet,
                              const VolumeFile::VOLUME_TYPE volumeType,
                              const QString& volumeFileName,
                              const int subVolumeNumber,
                              const bool functionalThresholdFlag = false) throw (CommandException);
                                               
};

#endif // __COMMAND_SCENE_CREATE_H__


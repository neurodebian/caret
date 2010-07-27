/* 
 * File:   CommandDeformationMapApplyGenericNames.h
 * Author: john
 *
 * Created on April 13, 2010, 3:54 PM
 */

#ifndef __COMMAND_DEFORMATION_MAP_APPLY_GENERIC_NAMES_H__
#define	__COMMAND_DEFORMATION_MAP_APPLY_GENERIC_NAMES_H__

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

class BrainSet;
class DeformationMapFile;

/// class for applying a deformation map to a data file
class CommandDeformationMapApplyGenericNames : public CommandBase {
   public:
      // constructor
      CommandDeformationMapApplyGenericNames();

      // destructor
      ~CommandDeformationMapApplyGenericNames();

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

      /// Read in BrainSets for applying a deformation map to a data file.
      void readBrainSetsForDeformation(BrainSet& sourceBrainSet,
                                       BrainSet& targetBrainSet,
                                       DeformationMapFile& deformationMapFile)
                                          throw (BrainModelAlgorithmException,
                                                 CommandException,
                                                 FileException);
};


#endif	/* __COMMAND_DEFORMATION_MAP_APPLY_GENERIC_NAMES_H__ */


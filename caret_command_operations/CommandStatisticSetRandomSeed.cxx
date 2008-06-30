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

#include "BrainSet.h"
#include "CommandStatisticSetRandomSeed.h"
#include "FileFilters.h"
#include "PreferencesFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandStatisticSetRandomSeed::CommandStatisticSetRandomSeed()
   : CommandBase("-statistic-set-random-seed",
                 "STATISTICS SET RANDOM SEED")
{
}

/**
 * destructor.
 */
CommandStatisticSetRandomSeed::~CommandStatisticSetRandomSeed()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandStatisticSetRandomSeed::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandStatisticSetRandomSeed::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "   <seed>\n"
       + indent9 + "\n"
       + indent9 + "Set the seed for the random number generator.\n"
       + indent9 + "\n"
       + indent9 + "Set \"seed\" to a positive integer.  Using the same seed \n"
       + indent9 + "results in the same sequence of random numbers being \n"
       + indent9 + "generated. \n"
       + indent9 + "\n"
       + indent9 + "This command sets the random seed that is stored in the  \n"
       + indent9 + "user's Preferences File.  The preferences file is read \n"
       + indent9 + "and the random seed is used each time a Caret program (the\n"
       + indent9 + "caret5 GUI or caret_command) is started.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandStatisticSetRandomSeed::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const unsigned int seedValue = parameters->getNextParameterAsInt("Seed");
   if (seedValue <= 1) {
      throw CommandException("Random seed must be greater than or equal to one.");
   }
   checkForExcessiveParameters();
   
   //
   // Set the seed
   //
   BrainSet bs;
   PreferencesFile* pf = bs.getPreferencesFile();
   pf->setRandomSeedOverride(true);
   pf->setRandomSeedOverrideValue(seedValue);
   try {
      pf->writeFile(pf->getFileName());
   }
   catch (FileException& e) {
      throw CommandException(e.whatQString());
   }
}

      


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

#include "CommandCaretFileNamingUnitTesting.h"
#include "FileUtilities.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandCaretFileNamingUnitTesting::CommandCaretFileNamingUnitTesting()
   : CommandBase("-caret-file-name-unit-test",
                 "CARET FILE NAME UNIT TESTING")
{
}

/**
 * destructor.
 */
CommandCaretFileNamingUnitTesting::~CommandCaretFileNamingUnitTesting()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandCaretFileNamingUnitTesting::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addString("Any Parameter", "test-on");
}

/**
 * get full help information.
 */
QString 
CommandCaretFileNamingUnitTesting::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + " test-on\n"
       + indent9 + "\n"
       + indent9 + "Perform unit testing on caret file naming code.  Provide\n"
       + indent9 + "any single parameter to run test.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandCaretFileNamingUnitTesting::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   bool fileNameTestsValid = true;
   
   {
      const QString& name = "Human.1582.LR.Full.CLOSED.65950.topo";
      QString directory, species,  casename,  anatomy, hemisphere;
      QString description, descriptionNoTypeName, theDate, numNodes, extension;
      const bool valid = FileUtilities::parseCaretDataFileName(name,
                           directory, species,  casename,  anatomy, hemisphere,
                           description, descriptionNoTypeName, theDate, numNodes, extension);
      if ((valid == false) ||
          (species != "Human") ||
          (casename != "1582") ||
          (hemisphere != "LR") ||
          (description != "Full.CLOSED") ||
          (numNodes != "65950") ||
          (extension != "topo")) {
         fileNameTestsValid = false;
         std::cout << "Parsing " << name.toAscii().constData() << " failed." << std::endl;
         std::cout << "directory   () \"" << directory.toAscii().constData() << "\"" << std::endl;
         std::cout << "species   (Human) \"" << species.toAscii().constData() << "\"" << std::endl;
         std::cout << "casename   (1582) \"" << casename.toAscii().constData() << "\"" << std::endl;
         std::cout << "anatomy   () \"" << anatomy.toAscii().constData() << "\"" << std::endl;
         std::cout << "hemisphere   (LR) \"" << hemisphere.toAscii().constData() << "\"" << std::endl;
         std::cout << "description   (Full.CLOSED) \"" << description.toAscii().constData() << "\"" << std::endl;
         std::cout << "descriptionNoTypeName   (Full) \"" << descriptionNoTypeName.toAscii().constData() << "\"" << std::endl;
         std::cout << "theDate   () \"" << theDate.toAscii().constData() << "\"" << std::endl;
         std::cout << "numNodes   (65950) \"" << numNodes.toAscii().constData() << "\"" << std::endl;
         std::cout << "extension   (topo) \"" << extension.toAscii().constData() << "\"" << std::endl;
      }
   }
   
   {
      const QString& name = "Human.colin.Cerebral.R.FIDUCIAL.TLRC.711-2B.71723.coord";
      QString directory, species,  casename,  anatomy, hemisphere;
      QString description, descriptionNoTypeName, theDate, numNodes, extension;
      const bool valid = FileUtilities::parseCaretDataFileName(name,
                           directory, species,  casename,  anatomy, hemisphere,
                           description, descriptionNoTypeName, theDate, numNodes, extension);
      if ((valid == false) ||
          (species != "Human") ||
          (casename != "colin") ||
          (hemisphere != "R") ||
          (description != "FIDUCIAL.TLRC.711-2B") ||
          (numNodes != "71723") ||
          (extension != "coord")) {
         fileNameTestsValid = false;
         std::cout << "Parsing " << name.toAscii().constData() << " failed." << std::endl;
         std::cout << "directory   () \"" << directory.toAscii().constData() << "\"" << std::endl;
         std::cout << "species   (Human) \"" << species.toAscii().constData() << "\"" << std::endl;
         std::cout << "casename   (colin) \"" << casename.toAscii().constData() << "\"" << std::endl;
         std::cout << "anatomy   (Cerebral) \"" << anatomy.toAscii().constData() << "\"" << std::endl;
         std::cout << "hemisphere   (R) \"" << hemisphere.toAscii().constData() << "\"" << std::endl;
         std::cout << "description   (FIDUCIAL.TLRC.711-2B) \"" << description.toAscii().constData() << "\"" << std::endl;
         std::cout << "descriptionNoTypeName   () \"" << descriptionNoTypeName.toAscii().constData() << "\"" << std::endl;
         std::cout << "theDate   () \"" << theDate.toAscii().constData() << "\"" << std::endl;
         std::cout << "numNodes   (71723) \"" << numNodes.toAscii().constData() << "\"" << std::endl;
         std::cout << "extension   (coord) \"" << extension.toAscii().constData() << "\"" << std::endl;
      }
   }
   
   if (fileNameTestsValid) {
      std::cout << "All caret file name tests passed." << std::endl;
   }
   else {
      throw CommandException("Caret file naming unit testing failed.");
   }
}

      


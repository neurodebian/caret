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

#include "Categories.h"
#include "CommandSpecFileCreate.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "Species.h"
#include "StereotaxicSpace.h"
#include "Structure.h"

/**
 * constructor.
 */
CommandSpecFileCreate::CommandSpecFileCreate()
   : CommandBase("-spec-file-create",
                 "SPEC FILE CREATE")
{
}

/**
 * destructor.
 */
CommandSpecFileCreate::~CommandSpecFileCreate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSpecFileCreate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);
   std::vector<QString> spaceNames;
   for (unsigned int i = 0; i < allSpaces.size(); i++) {
      spaceNames.push_back(allSpaces[i].getName());
   }
      
   std::vector<QString> allSpecies;
   Species::getAllSpecies(allSpecies);
   
   std::vector<Structure::STRUCTURE_TYPE> structureTypes;
   std::vector<QString> structureNames;
   Structure::getAllTypesAndNames(structureTypes, structureNames, false);
      
   paramsOut.clear();
   paramsOut.addListOfItems("Species", allSpecies, allSpecies);
   paramsOut.addString("Subject", "");
   paramsOut.addListOfItems("Structure", structureNames, structureNames);
   paramsOut.addListOfItems("Stereotaxic Space", spaceNames, spaceNames);
   paramsOut.addVariableListOfParameters("Create Spec Options");
}

/**
 * get full help information.
 */
QString 
CommandSpecFileCreate::getHelpInformation() const
{
   std::vector<StereotaxicSpace> allSpaces;
   StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);
      
   std::vector<QString> allSpecies;
   Species::getAllSpecies(allSpecies);
   
   std::vector<Structure::STRUCTURE_TYPE> structureTypes;
   std::vector<QString> structureNames;
   Structure::getAllTypesAndNames(structureTypes, structureNames, false);
   
   std::vector<QString> allCategories;
   Categories::getAllCategories(allCategories);

   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<species>\n"
       + indent9 + "<subject>\n"
       + indent9 + "<structure>\n"
       + indent9 + "<stereotaxic-space>\n"
       + indent9 + "[-category   category-name]\n"
       + indent9 + "[-spec-file-name  spec-file-name]\n"
       + indent9 + "\n"
       + indent9 + "Create a Spec File\n"
       + indent9 + "\n"
       + indent9 + "The \"spec-file-name\" is optional and should only be specified\n"
       + indent9 + "if you must name the spec file.  If the \"spec-file-name\" is not\n"
       + indent9 + "specified, the spec file name will be created using the Caret \n"
       + indent9 + "standard which composes the spec file name using the species,\n"
       + indent9 + "subject, and structure.\n"
       + indent9 + "\n"
       + indent9 + "The category is optional and the default value is "
       +                Categories::getIndividualCategoryValue() + "\n"
       + indent9 + "      \n"
       + indent9 + "Examples of \"species\" are: \n");
      for (int i = 0; i < static_cast<int>(allSpecies.size()); i++) {
         helpInfo += (indent9 + "   " + allSpecies[i] + "\n");
      }
      helpInfo += "\n";

      helpInfo += (indent9 + "Examples of \"structure\" are: \n");
      for (int i = 0; i < static_cast<int>(structureNames.size()); i++) {
         helpInfo += (indent9 + "   " + structureNames[i] + "\n");
      }
      helpInfo += "\n";

      helpInfo += (indent9 + "Examples of \"stereotaxic-space\" are: \n");
      for (int i = 0; i < static_cast<int>(allSpaces.size()); i++) {
         helpInfo += (indent9 + "   " + allSpaces[i].getName() + "\n");
      }
      helpInfo += "\n";

      helpInfo += (indent9 + "   Examples of \"category\" are: \n");
      for (int i = 0; i < static_cast<int>(allCategories.size()); i++) {
         helpInfo += (indent9 + "   " + allCategories[i] + "\n");
      }
      helpInfo += "\n";
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSpecFileCreate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString speciesName =
      parameters->getNextParameterAsString("Species Name");
   const QString subjectName =
      parameters->getNextParameterAsString("Subject Name");
   const Structure structure =
      parameters->getNextParameterAsStructure("Structure");
   const QString spaceName =
      parameters->getNextParameterAsString("Stereotaxic Space Name");

   //
   // Optional parameters
   //
   QString categoryName;
   QString specFileName;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Spec File Create Option");
      if (paramName == "-category") {
         categoryName = parameters->getNextParameterAsFloat("Category");
      }
      else if (paramName == "-spec-file-name") {
         specFileName = parameters->getNextParameterAsString("Spec File Name");
      }
      else {
         throw CommandException("Volume Create Corpus Callosum invalid parameter "
                                + paramName);
      }
   }

   SpecFile sf;
   if (specFileName.isEmpty()) {
      specFileName = speciesName + "."
                   + subjectName + "."
                   + structure.getTypeAsAbbreviatedString()
                   + SpecFile::getSpecFileExtension();
   }
   
   sf.setSpecies(speciesName);
   sf.setSubject(subjectName);
   sf.setSpace(spaceName); 
   sf.setStructure(structure.getTypeAsString());
   sf.setCategory(categoryName);   
   sf.writeFile(specFileName);
}

      


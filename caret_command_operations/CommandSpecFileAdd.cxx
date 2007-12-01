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

#include "CommandSpecFileAdd.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandSpecFileAdd::CommandSpecFileAdd()
   : CommandBase("-spec-file-add",
                 "SPEC FILE ADD")
{
}

/**
 * destructor.
 */
CommandSpecFileAdd::~CommandSpecFileAdd()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSpecFileAdd::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> names, tags;
   std::vector<SpecFile::Entry> allEntries;
   SpecFile sf;
   sf.getAllEntries(allEntries);
   for (unsigned int i = 0; i < allEntries.size(); i++) {
      const QString tag(allEntries[i].getSpecFileTag());
      const QString name(allEntries[i].getDescriptiveName()
                         + " ("
                         + tag
                         + ")");
      names.push_back(name);
      tags.push_back(tag);
   }
   
   paramsOut.clear();
   paramsOut.addFile("Spec File Name", 
                     FileFilters::getSpecFileFilter());
   paramsOut.addListOfItems("Tag", tags, names);
   paramsOut.addFile("Data File Name", 
                     FileFilters::getAnyFileFilter());
   paramsOut.addVariableListOfParameters("Add Spec File Options");
}
/**
 * get full help information.
 */
QString 
CommandSpecFileAdd::getHelpInformation() const
{
   //
   // Get all of the spec file entries
   //
   std::vector<SpecFile::Entry> allEntries;
   SpecFile sf;
   sf.getAllEntries(allEntries);

   //
   // Find the longest tag length
   //
   int longestTagLength = 0;
   for (unsigned int i = 0; i < allEntries.size(); i++) {
     longestTagLength = std::max(longestTagLength,
                                 allEntries[i].getSpecFileTag().length());
   }
   longestTagLength += 1;
   
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "<spec-file-tag>\n"
       + indent9 + "<data-file-name>\n"
       + indent9 + "[data-file-second-name]\n"
       + indent9 + "\n"
       + indent9 + "Add a file to a spec file\n"
       + indent9 + "\n"
       + indent9 + "If adding a volume header/data file pair, enter the header\n"
       + indent9 + "file name as \"data-file-name\" and the data file as \n"
       + indent9 + "\"data-file-second-name\".\n"
       + indent9 + "\n"
       + indent9 + "The spec file must exist.  A spec file is created using.\n"
       + indent9 + "the command \"-spec-file-create\".\n"
       + indent9 + "Valid spec file tags are:\n");
       
         const QString title1(QString("TAG").leftJustified(longestTagLength,' '));
         QString s = indent9 
                     + title1
                     + " DESCRIPTION\n";
         for (unsigned int i = 0; i < allEntries.size(); i++) {
           QString tag(allEntries[i].getSpecFileTag());
           const QString description(allEntries[i].getDescriptiveName());
           tag = tag.leftJustified(longestTagLength, ' ');
           s += (indent9
                 + tag + " "
                 + description
                 + "\n");
         }

       helpInfo += s;
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSpecFileAdd::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString specFileName = 
      parameters->getNextParameterAsString("Spec File Name");
   QString specFileTag =
      parameters->getNextParameterAsString("Spec File Tag");
   QString dataFileName =
      parameters->getNextParameterAsString("Data File Name");

   QString volumeDataFileName;
   if (parameters->getParametersAvailable()) {
      volumeDataFileName = 
         parameters->getNextParameterAsString("Data File Second Name (Volume Data)");
   }
   
   addTagAndFileToSpecFile(specFileName,
                           specFileTag,
                           dataFileName,
                           volumeDataFileName);
}

/**
 *  add tag and file to spec file
 */
void
CommandSpecFileAdd::addTagAndFileToSpecFile(const QString& specFileName,
                                            const QString& specFileTag,
                                            const QString& dataFileName,
                                            const QString& volumeDataFileName)
                                                              throw (FileException)
{
   SpecFile sf;
   if (specFileName.isEmpty() == false) {
      if (QFile::exists(specFileName)) {
         sf.readFile(specFileName);
      }
   }
   
   sf.addToSpecFile(specFileTag, dataFileName, volumeDataFileName, false);
   
   sf.writeFile(specFileName);
}


      


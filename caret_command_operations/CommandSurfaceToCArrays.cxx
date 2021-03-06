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

#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceToCArrays.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "TextFile.h"

/**
 * constructor.
 */
CommandSurfaceToCArrays::CommandSurfaceToCArrays()
   : CommandBase("-surface-to-c-arrays",
                 "SURFACE CONVERT TO C-LANGUAGE ARRAYS")
{
}

/**
 * destructor.
 */
CommandSurfaceToCArrays::~CommandSurfaceToCArrays()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceToCArrays::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Coordinate File", FileFilters::getCoordinateUnknownFileFilter());
   paramsOut.addFile("Input Topology File", FileFilters::getTopologyUnknownFileFilter());
   paramsOut.addString("C-language File");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceToCArrays::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-coordinate-file-name>\n"
       + indent9 + "<input-topology-file-name>\n"
       + indent9 + "<output-c-language-file-name>\n"
       + indent9 + "\n"
       + indent9 + "Write the coordinates, surface normals, and triangles\n"
       + indent9 + "that form a surface as C-language arrays.\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceToCArrays::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString coordFileName =
             parameters->getNextParameterAsString("Input Coordinate File Name");
   const QString topoFileName =
              parameters->getNextParameterAsString("Input Topology File Name");
   const QString cLanguageFileName =
              parameters->getNextParameterAsString("C-language File Name");

   BrainSet bs(topoFileName, coordFileName);
   BrainModelSurface* bms = bs.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Problem loading coordinate or topolgy file.");
   }

   const QString s = bms->convertToCLanguageArrays();

   TextFile tf;
   tf.setText(s);
   tf.writeFile(cLanguageFileName);
}

      


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

#include "BorderProjectionFile.h"
#include "BrainModelSurfaceMultiresolutionMorphing.h"
#include "BrainSet.h"
#include "CommandSurfaceFlatMultiResMorphing.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "StatisticsUtilities.h"

/**
 * constructor.
 */
CommandSurfaceFlatMultiResMorphing::CommandSurfaceFlatMultiResMorphing()
   : CommandBase("-surface-flat-multi-morph",
                 "SURFACE FLAT MULTIRESOLUTION MORPHING")
{
}

/**
 * destructor.
 */
CommandSurfaceFlatMultiResMorphing::~CommandSurfaceFlatMultiResMorphing()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceFlatMultiResMorphing::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Spec File Name", FileFilters::getSpecFileFilter());
   paramsOut.addFile("Fiducial Coordinate File Name", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Flat Coordinate File Name", FileFilters::getCoordinateFlatFileFilter());
   paramsOut.addFile("Cut Topology File Name", FileFilters::getTopologyCutFileFilter());
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceFlatMultiResMorphing::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "<fiducial-coordinate-file-name>\n"
       + indent9 + "<flat-coordinate-file-name>\n"
       + indent9 + "<cut-topology-file-name>\n"
       + indent9 + "[-ces-borderprojection-file  \n"
       + indent9 + "   <border-projection-file-name>  \n"
       + indent9 + "   <central-sulcus-border-name> ]\n"
       + indent9 + "\n"
       + indent9 + "Peform flat multi-resolution morphing (distortion correction)\n"
       + indent9 + "\n"
       + indent9 + "If a border projection file and the name of the central\n"
       + indent9 + "sulcus border are provided, the surface will be aligned\n"
       + indent9 + "to standard orientation.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceFlatMultiResMorphing::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString specFileName =
      parameters->getNextParameterAsString("Spec File Name");
   const QString fiducialCoordinateFileName =
      parameters->getNextParameterAsString("Fiducial Coordinate File Name");
   const QString flatCoordinateFileName =
      parameters->getNextParameterAsString("Flat Coordinate File Name");
   const QString cutTopologyFileName =
      parameters->getNextParameterAsString("Cut Topology File Name");
      
   QString borderProjectionFileName;
   QString centralSulcusBorderName;
   while (parameters->getParametersAvailable()) {
      const QString paramName(parameters->getNextParameterAsString("opt-param"));
      if (paramName == "-ces-borderprojection-file") {
         borderProjectionFileName = 
            parameters->getNextParameterAsString("Border Projection File Name");
         centralSulcusBorderName = 
            parameters->getNextParameterAsString("Central Sulcus Border Name");
      }
      else {
         throw CommandException("Unrecognized optional parameter: " + paramName);
      }
   }
   
   //
   // Read spec file
   //
   SpecFile specFile;
   specFile.readFile(specFileName);

   //
   // Set the selected files
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getCutTopoFileTag(), cutTopologyFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getFiducialCoordFileTag(), fiducialCoordinateFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getFlatCoordFileTag(), flatCoordinateFileName, 
                          "", SpecFile::SPEC_FALSE);
    
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, specFileName, errorMessage)) {
      throw CommandException("Reading spec file: " + errorMessage);
   }
   
   //
   // Find the fiducial surface
   //
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fiducialSurface == NULL) {
      throw CommandException("Unable to find fiducial surface.");
   }
   
   //
   // Find the flat surface
   //
   BrainModelSurface* flatSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT);
   if (flatSurface == NULL) {
      throw CommandException("Unable to find flat surface.");
   }
   
   //
   // Read the border projection file
   //
   BorderProjection centralSulcusBorderProjection;
   if (borderProjectionFileName.isEmpty() == false) {
      BorderProjectionFile borderProjectionFile;
      borderProjectionFile.readFile(borderProjectionFileName);
      const BorderProjection* cesBP =
         borderProjectionFile.getFirstBorderProjectionByName(centralSulcusBorderName);
      if (cesBP != NULL) {
         centralSulcusBorderProjection = *cesBP;
      }
      else {
         throw CommandException("Unable to find border projection named \""
                                + centralSulcusBorderName
                                + "\" in border projection file \""
                                + borderProjectionFileName
                                + "\".");
      }
   }
      
   //
   // Do flat multiresolution morphing
   //
   BrainModelSurfaceMultiresolutionMorphing bmsmm(&brainSet,
                                                  fiducialSurface,
                                                  flatSurface,
                                                  BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT,
                                                  &centralSulcusBorderProjection);
   bmsmm.execute();   
   
   std::cout << "cycle                "
             << "crossovers "
             << "    avg AD "
             << "    dev AD "
             << "    avg LD "
             << "    dev LD " 
             << std::endl;
   std::vector<MorphingMeasurements> meas;
   bmsmm.getMorphingMeasurements(meas);
   for (int i = 0; i < static_cast<int>(meas.size()); i++) {
      QString name;
      StatisticsUtilities::DescriptiveStatistics ad, ld;
      int nodeCrossovers, tileCrossovers;
      float elapsedTime;
      meas[i].get(name, ad, ld,
                  nodeCrossovers, tileCrossovers, elapsedTime);
                
      QString s;
      s.sprintf("%20s %10d %10.4f %10.4f %10.4f %10.4f", 
                name.toAscii().constData(), nodeCrossovers, 
                ad.average, ad.standardDeviation,
                ld.average, ld.standardDeviation);
      std::cout << s.toAscii().constData() << std::endl;
   }
}

      


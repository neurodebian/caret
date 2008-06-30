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
#include "CommandSurfaceInflate.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceInflate::CommandSurfaceInflate()
   : CommandBase("-surface-inflate",
                 "SURFACE INFLATE")
{
}

/**
 * destructor.
 */
CommandSurfaceInflate::~CommandSurfaceInflate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceInflate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   
   paramsOut.addFile("Input FiducialCoordinate File", FileFilters::getCoordinateFiducialFileFilter());
   paramsOut.addFile("Input Smoothing Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Input Smoothing Topology File", FileFilters::getTopologyGenericFileFilter());
   paramsOut.addFile("Output Smoothed Coordinate File", FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addInt("Smoothing Cycles", 2);
   paramsOut.addFloat("Smoothing Strength", 1.0);
   paramsOut.addInt("Smoothing Iterations", 100);
   paramsOut.addFloat("Inflation Factor", 1.4);
   paramsOut.addInt("Finger Smoothing Iterations", 30);
   paramsOut.addFloat("Finger Smoothing Strength", 1.0);
   paramsOut.addFloat("Finger Smoothing Compress/Stretch Threshold", 3.0);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceInflate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-fiducial-coordinate-file-name>\n"
       + indent9 + "<input-smoothing-coordinate-file-name>\n"
       + indent9 + "<input-smoothing-topology-file-name>\n"
       + indent9 + "<output-smoothed-coordinate-file-name>\n"
       + indent9 + "<number-of-smoothing-cycles>\n"
       + indent9 + "<smoothing-strength>\n"
       + indent9 + "<smoothing-iterations>\n"
       + indent9 + "<inflation-factor>\n"
       + indent9 + "<finger-smoothing-iterations>\n"
       + indent9 + "<finger-smoothing-strength>\n"
       + indent9 + "<finger-compress-stretch-threshold>\n"
       + indent9 + "\n"
       + indent9 + "Inflate a surface.\n"
       + indent9 + "\n"
       + indent9 + "Sequence to start with fiducial and create low smooth\n"
       + indent9 + "(low), inflated (INF), Very Inflated (VI), high smooth\n"
       + indent9 + "(high), and Ellipse Surfaces.\n"
       + indent9 + "               low    INF    VI   high    Ellipse\n"
       + indent9 + "cycles          1      2     4     6        6\n"
       + indent9 + "strength        0.2    1.0   1.0   1.0     1.0\n"
       + indent9 + "iterations     50     30    30    60      50\n"
       + indent9 + "inflate-factor  1.0    1.4   1.1   1.6     1.4\n"
       + indent9 + "fing-iter       0      30    0    60      60\n"
       + indent9 + "fing-strength   1.0    1.0   1.0   1.0    1.0\n"
       + indent9 + "fing-cst        3.0    3.0   3.0   3.0    4.0\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceInflate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputFiducialCoordinateFileName =
      parameters->getNextParameterAsString("Input Fiducial Coordinate File Name");
   const QString inputCoordinateFileName =
      parameters->getNextParameterAsString("Input Smoothing Coordinate File Name");
   const QString inputTopologyFileName =
      parameters->getNextParameterAsString("Input Smoothing Topology File Name");
   const QString outputCoordinateFileName =
      parameters->getNextParameterAsString("Output Smoothed Coordinate File Name");
   const int numberOfSmoothingCycles =
      parameters->getNextParameterAsInt("Number of Smoothing Cycles");
   const float smoothingStrength =
      parameters->getNextParameterAsFloat("Smoothing Strength");
   const int smoothingIterations =
      parameters->getNextParameterAsInt("Smoothing Iterations");
   const float inflationFactor =
      parameters->getNextParameterAsFloat("Inflation Factor");
   const int fingerSmoothingIterations =
      parameters->getNextParameterAsInt("Finger Smoothing Iterations");
   const float fingerSmoothingStrength =
      parameters->getNextParameterAsFloat("Finger Smoothing Strength");
   const float fingerSmoothingCompressStretchThreshold =
      parameters->getNextParameterAsFloat("Finger Smoothing Compress Stretch Threshold");

   // 
   // Create a brain set
   //
   BrainSet brainSet(inputTopologyFileName,
                     inputFiducialCoordinateFileName,
                     "",
                     true); 
   brainSet.readCoordinateFile(inputCoordinateFileName,
                               BrainModelSurface::SURFACE_TYPE_UNKNOWN,
                               false,
                               true,
                               false);
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurface(0);
   BrainModelSurface* smoothingSurface = brainSet.getBrainModelSurface(1);
   if ((fiducialSurface == NULL) ||
       (smoothingSurface == NULL)) {
      throw CommandException("Problem reading one or both surfaces.");
   }
   const TopologyFile* tf = smoothingSurface->getTopologyFile();
   if (tf == NULL) {
      throw CommandException("Problem reading topology file.");
   }
   
   //
   // Smooth the surface
   //
   smoothingSurface->inflateSurfaceAndSmoothFingers(fiducialSurface,
                                                    numberOfSmoothingCycles,
                                                    smoothingStrength,
                                                    smoothingIterations,
                                                    inflationFactor,
                                                    fingerSmoothingCompressStretchThreshold,
                                                    fingerSmoothingStrength,
                                                    fingerSmoothingIterations,
                                                    NULL);
                                                    
   //
   // Write the smoothed coordinate file
   //
   smoothingSurface->getCoordinateFile()->writeFile(outputCoordinateFileName);
}

      


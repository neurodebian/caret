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

#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BorderProjectionUnprojector.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandSurfaceBorderMerge.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandSurfaceBorderMerge::CommandSurfaceBorderMerge()
   : CommandBase("-surface-border-merge",
                 "SURFACE BORDER MERGE")
{
}

/**
 * destructor.
 */
CommandSurfaceBorderMerge::~CommandSurfaceBorderMerge()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceBorderMerge::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   
   paramsOut.addFile("Input Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addFile("Output Border Projection File",
                     FileFilters::getBorderProjectionFileFilter());
   paramsOut.addString("Output Border Projection Name");
   paramsOut.addString("Input Border Projection 1 Name");
   paramsOut.addString("Input Border Projection 2 Name");
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandSurfaceBorderMerge::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-border-projection-file-name>\n"
       + indent9 + "<output-border-projection-file-name>\n"
       + indent9 + "<output-border-projection-name>\n"
       + indent9 + "<input-border-projection-1-name>\n"
       + indent9 + "<input-border-projection-2-name>\n"
       + indent9 + "[-border-proj  additional-input-border-projection-name]\n"
       + indent9 + "[-delete-input-border-projections]\n"
       + indent9 + "[-close-border]\n"
       + indent9 + "[-smooth-junctions \n"
       + indent9 + "    coordinate-file-name \n"
       + indent9 + "    topology-file-name \n"
       + indent9 + "    junction-neighbors \n"
       + indent9 + "    number-of-iterations] \n"
       + indent9 + "\n"
       + indent9 + "Merge two or more border projections into a single\n"
       + indent9 + "border projection.  An option is provided that will\n"
       + indent9 + "delete all of the input border projections.  Use\n"
       + indent9 + "\"-border-proj\" if more than two border projections\n"
       + indent9 + "need to be merged.\n"
       + indent9 + "\n"
       + indent9 + "Use \"-close-border\" if the borders are to be merged into\n"
       + indent9 + "a closed border.\n"
       + indent9 + "\n"
       + indent9 + "Use \"-smooth-junctions\" to smooth the border links \n"
       + indent9 + "around the junctions (the border links that are \n"
       + indent9 + "connected to form the output border).  \n"
       + indent9 + "\"junction-neighbors\" is the number of links on both \n"
       + indent9 + "sides of the junction that get smoothed for \n"
       + indent9 + "\"number-of-iterations\". \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceBorderMerge::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get required parameters
   //
   const QString inputBorderProjectionFileName = 
      parameters->getNextParameterAsString("Input Border Projection File");
   const QString outputBorderProjectionFileName = 
      parameters->getNextParameterAsString("Output Border Projection File");
   const QString outputBorderProjectionName =
      parameters->getNextParameterAsString("Output Border Projection Name");
   std::vector<QString> inputBorderProjectionNames;
   inputBorderProjectionNames.push_back(
      parameters->getNextParameterAsString("Input Border Projection 1 Name"));
   inputBorderProjectionNames.push_back(
      parameters->getNextParameterAsString("Input Border Projection 2 Name"));
      
   //
   // Process optional parameters
   //
   bool smoothFlag = false;
   QString smoothCoordFileName;
   QString smoothTopoFileName;
   int smoothJunctionNeighborsCount = 0;
   int smoothJunctionIterations = 0;
   bool closeBorderFlag = false;
   bool deleteInputBorderProjectionsFlag = false;
   while (parameters->getParametersAvailable()) {
      const QString paramName =
         parameters->getNextParameterAsString("Merge Border Parameter");
      if (paramName == "-border-proj") {
         inputBorderProjectionNames.push_back(
            parameters->getNextParameterAsString("Additional Border Projection Names"));
      }
      else if (paramName == "-close-border") {
         closeBorderFlag = true;
      }
      else if (paramName == "-delete-input-border-projections") {
         deleteInputBorderProjectionsFlag = true;
      }
      else if (paramName == "-smooth-junctions") {
         smoothCoordFileName = 
            parameters->getNextParameterAsString("Coordinate File Name");
         smoothTopoFileName = 
            parameters->getNextParameterAsString("Topology File Name");
         smoothJunctionNeighborsCount = 
            parameters->getNextParameterAsInt("Junction Neighbors");
         smoothJunctionIterations = 
            parameters->getNextParameterAsInt("Smoothing Iterations");
         smoothFlag = true;
      }
      else {
         throw CommandException("Border Merge unrecognized option: \"" +
                                paramName + "\"");
      }
   }
   
   //
   // Read the border projection file
   //
   BorderProjectionFile borderProjectionFile;
   borderProjectionFile.readFile(inputBorderProjectionFileName);
   
   //
   // Create a new border projection
   //
   BorderProjection borderProjection(outputBorderProjectionName);
   
   //
   // Locations of junctions for smoothing
   //
   std::vector<int> junctionLinks;
   
   //
   // loop through input border projections and append
   //
   std::vector<int> borderProjectionsForDeletion;
   const int numInput = static_cast<int>(inputBorderProjectionNames.size());
   for (int i = 0; i < numInput; i++) {
      if (borderProjection.getNumberOfLinks() > 0) {
         junctionLinks.push_back(borderProjection.getNumberOfLinks() - 1);
      }
      const BorderProjection* bp = getBorderProjectionWithUniqueName(borderProjectionFile,
                                                                     inputBorderProjectionNames[i]);
      borderProjection.append(*bp);
      borderProjectionsForDeletion.push_back(borderProjectionFile.getBorderProjectionIndex(bp));
   }
   
   //
   // Close the border?
   //
   if (closeBorderFlag) {
      const int numLinks = borderProjection.getNumberOfLinks();
      if (numLinks > 1) {
         //
         // Add first link to the end of the border so it closes
         //
         junctionLinks.push_back(numLinks - 1);
         borderProjection.addBorderProjectionLink(*borderProjection.getBorderProjectionLink(0));
      }
   }
   
   //
   // Should input border projections be deleted
   //
   if (deleteInputBorderProjectionsFlag) {
      borderProjectionFile.removeBordersWithIndices(borderProjectionsForDeletion);
   }
   
   //
   // Should smoothing be performed ?
   //
   if (smoothFlag &&
       (borderProjection.getNumberOfLinks() > 2)) {
      // 
      // Create a brain set
      //
      BrainSet brainSet(smoothTopoFileName,
                        smoothCoordFileName,
                        "",
                        true); 
      BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
      if (bms == NULL) {
         throw CommandException("unable to find surface.");
      }
      const CoordinateFile* cf = bms->getCoordinateFile();
      const TopologyFile* tf = bms->getTopologyFile();
      if (tf == NULL) {
         throw CommandException("unable to find topology.");
      }
      const int numNodes = bms->getNumberOfNodes();
      if (numNodes == 0) {      
         throw CommandException("surface contains not nodes.");  
      }

      //
      // Place the border projection in a temporary file
      //
      BorderProjectionFile tempBorderProjectionFile;
      tempBorderProjectionFile.addBorderProjection(borderProjection);
      
      //
      // Unproject the border
      //
      BorderProjectionUnprojector unprojector;
      BorderFile tempBorderFile;
      unprojector.unprojectBorderProjections(*cf,
                                             tempBorderProjectionFile,
                                             tempBorderFile);
                                             
      //
      // Get border
      //
      if (tempBorderFile.getNumberOfBorders() <= 0) {
         throw CommandException("unprojection error.");
      }
      
      //
      // Get links for smoothing
      //
      const int numLinks = borderProjection.getNumberOfLinks();
      std::vector<bool> smoothFlags(numLinks, false);
      for (int m = 0; m < static_cast<int>(junctionLinks.size()); m++) {
         const int junctionIndex = junctionLinks[m];
         
         //
         // Smooth links before junction
         //
         int mStart = junctionIndex - smoothJunctionNeighborsCount;
         if (mStart < 0) {
            //
            // flag nodes at start of border
            //
            for (int n = 0; n <= junctionIndex; n++) {
               smoothFlags[n] = true;
            }
            
            if (closeBorderFlag) {
               //
               // wrap around beginning and flag nodes at end of border
               //
               const int remainingLinks = smoothJunctionNeighborsCount - junctionIndex;
               mStart = numLinks - remainingLinks;
               if (mStart < 0) {
                  mStart = 0;
               }
               for (int n = mStart; n < numLinks; n++) {
                  smoothFlags[n] = true;
               }
            }
         }
         else {
            for (int n = mStart; n <= junctionIndex; n++) {
               smoothFlags[n] = true;
            }
         }
         
         //
         // Smooth links after junction
         //
         int mEnd   = junctionIndex + smoothJunctionNeighborsCount;
         if (mEnd >= numLinks) {
            //
            // Flag nodes at end of border
            //
            for (int n = junctionIndex; n <= numLinks; n++) {
               smoothFlags[n] = true;
            }
            
            if (closeBorderFlag) {
               //
               // wrap around beginning and flag nodes at start of border
               //
               const int remainingLinks = mEnd - numLinks;
               mEnd = remainingLinks;
               if (mEnd > numLinks) {
                  mEnd = numLinks;
               }
               for (int n = 0; n < mEnd; n++) {
                  smoothFlags[n] = true;
               }
            }
         }
         else {
            for (int n = junctionIndex; n < mEnd; n++) {
               smoothFlags[n] = true;
            }
         }
      }
      
      //
      // Smooth the border
      //
      Border* b = tempBorderFile.getBorder(0);
      b->smoothBorderLinks(smoothJunctionIterations,
                           closeBorderFlag,
                           &smoothFlags);

      //
      // Reproject the border
      //
      tempBorderProjectionFile.clear();
      BorderFileProjector projector(bms, true);
      projector.projectBorderFile(&tempBorderFile,
                                  &tempBorderProjectionFile,
                                  NULL);
                                  
      //
      // Add border projection to output file
      //
      if (tempBorderProjectionFile.getNumberOfBorderProjections() <= 0) {
         throw CommandException("reprojection error.");
      }
      borderProjectionFile.addBorderProjection(
                     *tempBorderProjectionFile.getBorderProjection(0));
   }
   else {
      //
      // Add to border projection file
      //
      borderProjectionFile.addBorderProjection(borderProjection);
   }
   
   //
   // Write the border projection file
   //
   borderProjectionFile.writeFile(outputBorderProjectionFileName);
}

/**
 * get border projection by name and make sure only one.
 */
const BorderProjection*
CommandSurfaceBorderMerge::getBorderProjectionWithUniqueName(const BorderProjectionFile& bpf,
                                                                 const QString& name) const
                                                                throw (CommandException)
{
   const BorderProjection* bpFirst = bpf.getFirstBorderProjectionByName(name);
   const BorderProjection* bpLast  = bpf.getLastBorderProjectionByName(name);
   if (bpFirst == bpLast) {
      if (bpFirst != NULL) {
         return bpFirst;
      }
      else {
         throw CommandException("no border projection with "
                                + name
                                + " was found.");
      }
   }
   
   throw CommandException("there is more than one border projection named "
                          + name);
}



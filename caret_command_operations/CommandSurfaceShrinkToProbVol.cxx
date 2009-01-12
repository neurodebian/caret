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
#include <cmath>

#include "CommandSurfaceShrinkToProbVol.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelVolumeLigaseSegmentation.h"
#include "BrainSet.h"
#include "StringUtilities.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceToVolumeSegmentationConverter.h"
#include "BrainModelVolumeGradient.h"
#include "VectorFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandSurfaceShrinkToProbVol::CommandSurfaceShrinkToProbVol()
   : CommandBase("-surface-shrink-to-prob-vol",
                 "SURFACE SHRINK TO PROBABILISTIC VOLUME")
{
}

/**
 * destructor.
 */
CommandSurfaceShrinkToProbVol::~CommandSurfaceShrinkToProbVol()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceShrinkToProbVol::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandSurfaceShrinkToProbVol::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-topo-file>\n"
       + indent9 + "<input-coord-file>\n"
       + indent9 + "<input-probabilistic-boundary-volume>\n"
       + indent9 + "<output-segmentation-volume>\n"
       + indent9 + "<output-volume-label>\n"
       + indent9 + "<iterations>\n"
       + indent9 + "\n"
       + indent9 + "Generate a segmentation by moving the surface inwards to probability peaks\n"
       + indent9 + "\n"
       + indent9 + "      iterations  number of times to grow inwards, each time a maximum of\n"
       + indent9 + "                   one erosion of the segmentation.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceShrinkToProbVol::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString inputSurfaceTopoName =
      parameters->getNextParameterAsString("Input Surface Topo Name");
   const QString inputSurfaceCoordName =
      parameters->getNextParameterAsString("Input Surface Coord Name");
   const QString inputVolumeFileName =
      parameters->getNextParameterAsString("Input Probabilistic Volume File Name");
   const QString outputVolumeFileName =
      parameters->getNextParameterAsString("Output Segmentation Volume File Name");
   const QString outputVolumeLabel =
      parameters->getNextParameterAsString("Output Segmentation Volume Label");
   const int numberOfIterations = 
      parameters->getNextParameterAsInt("Number of Iterations");
   checkForExcessiveParameters();
   
   //
   // Create a brain set
   //
   BrainSet brainSet(inputSurfaceTopoName, inputSurfaceCoordName);
   
   //
   // Read the input files
   //
   BrainModelSurface* inputSurf = brainSet.getBrainModelSurface(0);
   inputSurf->computeNormals();
   inputSurf->orientNormalsOut();
   VolumeFile inputVol;
   inputVol.readFile(inputVolumeFileName);
   //
   // Create output volume file
   //
   VolumeFile segVolume(inputVol);
   segVolume.setVolumeType(VolumeFile::VOLUME_TYPE_SEGMENTATION);
   segVolume.setFileComment(outputVolumeLabel);
   int i, j, k, ti, tj, tk, max_i, max_j, max_k, total_nodes = inputSurf->getNumberOfNodes(), index;
   inputVol.getDimensions(max_i, max_j, max_k);
   //
   // Generate initial segmentation from surface
   //
   BrainModelSurfaceToVolumeSegmentationConverter* bmsvsc = new BrainModelSurfaceToVolumeSegmentationConverter(&brainSet, inputSurf, &segVolume, true, false);
   bmsvsc->execute();
   delete bmsvsc;
   //
   // Take the gradient of the probability volume to get directionality easily, and the benefits of the filtering/unbiased directionality
   //
   VectorFile prob_grad(max_i, max_j, max_k);
   BrainModelVolumeGradient* bmvg = new BrainModelVolumeGradient(&brainSet, 5, true, false, &inputVol, &segVolume, &prob_grad);
   std::cout << "starting gradient" << std::endl;
   bmvg->execute();
   std::cout << "gradient done" << std::endl;
   delete bmvg;
   //
   // Voxels not considered in the iteration and voxels that failed to erode, which will not erode with another test
   //
   VolumeFile voxelsToLeave, voxelsFailed(segVolume);
   voxelsFailed.setAllVoxels(0.0f);
   int coord_length = 3 * total_nodes;
   float ii, ij, ik, min, temp, tempa, tempb, spacing[3], tempCoord[3], *coord = new float[coord_length];
   const float* normals = inputSurf->getNormal(0);//silly hack to get flat array of normals
   float thisVal, tempVec[3], tempFloat[3], avgspace, weight;
   inputSurf->getCoordinateFile()->getAllCoordinates(coord);
   inputVol.getSpacing(spacing);
   avgspace = pow(spacing[0] * spacing[1] * spacing[2], 1.0f / 3.0f);//geometric average of spacing to estimate "1 voxel away" in stereotaxic space (mm)
   for (int iter = 0; iter < numberOfIterations; ++iter)
   {
      voxelsToLeave = segVolume;
      voxelsToLeave.doVolMorphOps(0, 1);//erode
      for (i = 0; i < max_i; ++i)
      {
         for (j = 0; j < max_j; ++j)
         {
            for (k = 0; k < max_k; ++k)
            {
               if (segVolume.getVoxel(i, j, k) > 1.0f && voxelsToLeave.getVoxel(i, j, k) < 1.0f && voxelsFailed.getVoxel(i, j, k) < 1.0f)
               {
                  //
                  // Voxel is in current segmentation, not in the eroded segmentation, and hasn't been tested before, so try it
                  //
                  inputVol.getVoxelCoordinate(i, j, k, tempCoord);
                  index = 0;
                  temp = coord[0] - tempCoord[0];
                  tempa = coord[1] - tempCoord[1];
                  tempb = coord[2] - tempCoord[2];
                  min = temp * temp + tempa * tempa + tempb * tempb;
                  //
                  // Find closest surface node
                  //
                  for (int node = 3; node < coord_length; node += 3)//NOTE: could be optimized by indexing to find closest node without searching all
                  {//however, that may weaken the stand that the closest node will always be found.  Index by surface or volume?
                     temp = coord[node] - tempCoord[0];
                     tempa = coord[node + 1] - tempCoord[1];
                     tempb = coord[node + 2] - tempCoord[2];
                     temp = temp * temp + tempa * tempa + tempb * tempb;
                     if (temp < min)
                     {
                        min = temp;
                        index = node;
                     }
                  }
                  prob_grad.getVector(i, j, k, tempVec);
                  //
                  // Dot product of normal at closest node and vector at voxel
                  //
                  thisVal = normals[index] * tempVec[0] + normals[index + 1] * tempVec[1] + normals[index + 2] * tempVec[2];
                  //evaluate identical fitness calculation at other node, but using same normal
                  /*ti = i - (int)(normals[index] * 1.9999f);//HACK: multiply by 2, cast to int equals round to integer for (-1, 1)
                  tj = j - (int)(normals[index + 1] * 1.9999f);
                  tk = k - (int)(normals[index + 2] * 1.9999f);
                  inputVol.getVoxelCoordinate(ti, tj, tk, tempCoord);*///replaced by interpolation
                  /*tempCoord[0] -= normals[index] * avgspace;//coordinates used only for searching for closest node
                  tempCoord[1] -= normals[index + 1] * avgspace;
                  tempCoord[2] -= normals[index + 2] * avgspace;*///not used, because using the same normal as the testing voxel
                  //
                  // Interpolate the gradient vector at voxel by interpolating each component separately, with no curvature
                  //
                  tempVec[0] = tempVec[1] = tempVec[2] = 0.0f;
                  ii = i - normals[index] * avgspace / spacing[0];//"index" to interpolate, this is NOT a coordinate, adjusts the "one voxel away" vector back
                  ij = j - normals[index + 1] * avgspace / spacing[1];//into index space
                  ik = k - normals[index + 2] * avgspace / spacing[2];
                  for (ti = (int)floor(ii); ti <= (int)ceil(ii); ++ti)
                  {
                     for (tj = (int)floor(ij); tj <= (int)ceil(ij); ++tj)
                     {
                        for (tk = (int)floor(ik); tk <= (int)ceil(ik); ++tk)
                        {
                           prob_grad.getVector(ti, tj, tk, tempFloat);
                           weight = fabs(tempCoord[0] - ti) * fabs(tempCoord[1] - tj) * fabs(tempCoord[2] - tk);//linear weighting in all directions
                           tempVec[0] += tempFloat[0] * weight;
                           tempVec[1] += tempFloat[1] * weight;
                           tempVec[2] += tempFloat[2] * weight;
                        }
                     }
                  }
                  /*index = 0;//search for closest node to new coordinate is BAD with a capital BAD for thin white matter
                  temp = coord[0] - tempCoord[0];
                  tempa = coord[1] - tempCoord[1];
                  tempb = coord[2] - tempCoord[2];
                  min = temp * temp + tempa * tempa + tempb * tempb;
                  for (int node = 3; node < coord_length; node += 3)
                  {
                     temp = coord[node] - tempCoord[0];
                     tempa = coord[node + 1] - tempCoord[1];
                     tempb = coord[node + 2] - tempCoord[2];
                     temp = temp * temp + tempa * tempa + tempb * tempb;
                     if (temp < min)
                     {
                        min = temp;
                        index = node;
                     }
                  }*/
                  if (thisVal < 0.0f)//if the gradient says go inwards
                  {
                     //if the dot product of probability gradient and outward surface normal is farther from the peak than the neighbor voxel
                     //in the opposite direction from the surface normal, erode this voxel
                     //essentially, if following the inward normal would end up farther down the inside slope of the probability peak, dont erode
                     //NOTE: negative values mean gradient is inward (opposed to outward normals)
                     if (thisVal < -(normals[index] * tempVec[0] + normals[index + 1] * tempVec[1] + normals[index + 2] * tempVec[2]))
                     {
                        segVolume.setVoxel(i, j, k, 0, 0.0f);
                     } else {
                        voxelsFailed.setVoxel(i, j, k, 0, 255.0f);
                     }
                  } else {
                     //if the gradient says go outwards, but the next voxel says inwards or less strongly outwards, erode
                     //bad for thin white matter
                     /*if (normals[index] * tempVec[0] + normals[index + 1] * tempVec[1] + normals[index + 2] * tempVec[2] < thisVal)
                     {
                        segVolume.setVoxel(i, j, k, 0, 0.0f);
                     } else {*/
                        voxelsFailed.setVoxel(i, j, k, 0, 255.0f);
                     //}
                  }
               }
            }
         }
      }
   }
   //
   // Write the file
   //
   segVolume.writeFile(outputVolumeFileName);
}

      


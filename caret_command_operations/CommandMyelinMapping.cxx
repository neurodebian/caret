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

#include "CommandMyelinMapping.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "MetricFile.h"
#include "CoordinateFile.h"
#include "TopologyFile.h"
#include "BrainModelSurface.h"
#include "VolumeFile.h"
#include "BrainModel.h"
#include "BrainSet.h"
#include "TopologyHelper.h"
#include "MatrixUtilities.h"
#include "GeodesicHelper.h"
#include "BrainModelSurfaceROIMetricSmoothing.h"
#include "BrainModelSurfaceROINodeSelection.h"

#include <cmath>
#include <vector>

#ifdef _OPENMP
#include "omp.h"
#endif

using namespace std;

/**
 * constructor.
 */
CommandMyelinMapping::CommandMyelinMapping()
   : CommandBase("-myelin-mapping",
                 "MYELIN MAPPING")
{
}

/**
 * destructor.
 */
CommandMyelinMapping::~CommandMyelinMapping()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMyelinMapping::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
}

/**
 * get full help information.
 */
QString 
CommandMyelinMapping::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<left-midthickness-coordinate-file-name>\n"
       + indent9 + "<right-midthickness-coordinate-file-name>\n"
       + indent9 + "<left-topology-file-name>\n"
       + indent9 + "<right-topology-file-name>\n"
       + indent9 + "<T1-weighted-volume>\n"
       + indent9 + "<T2-weighted-volume>\n"
       + indent9 + "<freesurfer-label-volume>\n"
       + indent9 + "<left-cortical-thickness-metric>\n"
       + indent9 + "<right-cortical-thickness-metric>\n"
       + indent9 + "<left-surface-curvature-metric>\n"
       + indent9 + "<right-surface-curvature-metric>\n"
       + indent9 + "<left-output-metric-file-name>\n"
       + indent9 + "<right-output-metric-file-name>\n"
       + indent9 + "<output-T1w/T2w-ratio-volume>\n"
       + indent9 + "<output-T1w/T2w-ratio-ribbon-masked-volume>\n"
       + indent9 + "[-neighbor-depth]\n"
       + indent9 + "[-number-of-standard-deviations]\n"
       + indent9 + "[-smoothing-FWHM]\n"
       + indent9 + "[-volume-outliers]\n"
       + indent9 + "\n"
       + indent9 + "Create a surface myelin map from T1w and T2w volumetric data.  This\n"
       + indent9 + "function will take the ratio of the T1w to T2w images, map them to the\n"
       + indent9 + "surface, and correct for small errors in the reconstructed surface\n"
       + indent9 + "that affect both the myelin map and cortical thickness.  T1w and T2w\n"
       + indent9 + "volumes must already be aligned to each other and to the surface.\n"
       + indent9 + "It is recommended that you reconstruct surfaces using an unresampled\n"
       + indent9 + "T1w image and that you do not do an atlas transform on the surfaces,\n"
       + indent9 + "volumes, and freesurfer-label-volume prior to making the myelin\n"
       + indent9 + "map.  It is recommended that you use spline or similar\n" 
       + indent9 + "interpolation (not trilinear) to resample the T2w to be in register\n"
       + indent9 + "with the T1w.  We do not recommend myelin mapping with anything worse\n"
       + indent9 + "than 1mm isotropic T1w and T2w volumes.  The function outputs a metric\n"
       + indent9 + "file for each hemisphere with these columns: a raw myelin map (with\n"
       + indent9 + "no outlier correction) a corrected myelin map, a smoothed myelin map,\n"
       + indent9 + "and a cortical thickness corrected for surface curvature.\n"
       + indent9 + "\n"
       + indent9 + "[-neighbor-depth] defaults to 10 if not specified\n"
       + indent9 + "[-number-of-standard-deviations] defaults to 2 if not specified\n"
       + indent9 + "[-smoothing-FWHM] defaults to 5mm if not specified\n"
       + indent9 + "[-volume-outliers] defaults to 1.0 standard deviations if not specified\n"
       + indent9 + "\n"
       + indent9 + "Higher values of neighbor-depth and lower values of \n"
       + indent9 + "number-of-standard-deviations and volume-outliers will result in more\n"
       + indent9 + "aggressive outlier removal at the cost of slightly increased processing\n"
       + indent9 + "time.  These parameters control the stringency of outlier detection.\n"
       + indent9 + "The smoothing parameter controls the Gaussian smoothing kernels\n"
       + indent9 + "employed in volume to surface mapping, outlier detection, and final\n"
       + indent9 + "myelin map smoothing.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMyelinMapping::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{   
   const QString leftCoordFile =
      parameters->getNextParameterAsString("Input Left Coordinate File");
   const QString rightCoordFile =
      parameters->getNextParameterAsString("Input RightCoordinate File");
   const QString leftTopoFile =
      parameters->getNextParameterAsString("Input Left Topology File");
   const QString rightTopoFile =
      parameters->getNextParameterAsString("Input Right Topology File");
   const QString t1wVolumeFile =
      parameters->getNextParameterAsString("Input T1 Weighted Volume");
   const QString t2wVolumeFile =
      parameters->getNextParameterAsString("Input T2 Weighted Volume");
   const QString volMaskFile =
      parameters->getNextParameterAsString("Cortical Ribbon Volume Mask");
   const QString leftThicknessFile =
      parameters->getNextParameterAsString("Left Cortical Thickness Metric");
   const QString rightThicknessFile =
      parameters->getNextParameterAsString("Right Cortical Thickness Metric");
   const QString leftCurvatureFile =
      parameters->getNextParameterAsString("Left Surface Curvature Metric");
   const QString rightCurvatureFile =
      parameters->getNextParameterAsString("Right Surface Curvature Metric");
   const QString leftMMFile =
      parameters->getNextParameterAsString("Left Myelin Map Output Metric");
   const QString rightMMFile =
      parameters->getNextParameterAsString("Right Myelin Map Output Metric");
   const QString outputT1T2RatioVolFile =
      parameters->getNextParameterAsString("Output T1/T2 Ratio Volume File");
   const QString outputRibbonMaskedT1T2RatioVolFile =
      parameters->getNextParameterAsString("Output Ribbon Masked T1/T2 Ratio Volume");
   
   bool averageNormals = true;
   m_neighborDepth = 10;
   m_numStdDev = 2.0f;
   float smoothFWHM = 5.0f;
   float volOutliers = 1.0f;
   while(parameters->getParametersAvailable())
   {
      const QString paramValue = parameters->getNextParameterAsString("Myelin Mapping Parameter");
      if(paramValue == "-neighbor-depth")
      {
         m_neighborDepth = parameters->getNextParameterAsInt("Neighbor Depth");
      }
      else if(paramValue == "-volume-outliers")
      {
         volOutliers = parameters->getNextParameterAsFloat("Volume Outlier Threshhold");
      }
      else if(paramValue == "-number-of-standard-deviations")
      {
         m_numStdDev = parameters->getNextParameterAsFloat("Number of Standard Deviations");
      }
      else if(paramValue == "-smoothing-FWHM")
      {
         smoothFWHM = parameters->getNextParameterAsFloat("Smoothing FWHM");
      }
      else if(paramValue == "-average-normals")
      {
         averageNormals = parameters->getNextParameterAsBoolean("Average Normals");
      }
      else
      {
         throw CommandException("Unrecognized parameter: " + paramValue);
      }
   }
   
   ///1.  load the following
   ///load right and left mid thickness coord and topo files
   BrainSet leftBS(leftTopoFile,leftCoordFile);
   BrainSet rightBS(rightTopoFile,rightCoordFile);
   BrainModelSurface *leftSurf = leftBS.getBrainModelSurface(0);
   BrainModelSurface *rightSurf = rightBS.getBrainModelSurface(0);
   leftSurf->computeNormals(NULL, averageNormals);
   rightSurf->computeNormals(NULL, averageNormals);
   ///load t1 and t2 weighted volume files (*.nii)
   VolumeFile t1Volume;
   t1Volume.readFile(t1wVolumeFile);
   VolumeFile t2Volume;
   t2Volume.readFile(t2wVolumeFile);
   /// load cortical ribbon volume mask (*.nii)
   m_fsLabelVol = new VolumeFile();
   m_fsLabelVol->readFile(volMaskFile);
   
   ///load left and right cortical thickness files (*.metric)
   MetricFile leftThickness;
   leftThickness.readFile(leftThicknessFile);
   MetricFile rightThickness;
   rightThickness.readFile(rightThicknessFile);
   ///load left and right surface curvature files (*.metric)
   MetricFile leftCurvature;
   leftCurvature.readFile(leftCurvatureFile);
   MetricFile rightCurvature;
   rightCurvature.readFile(rightCurvatureFile);
   
   ///create output t1/t2 ratio volume files (*.nii)
   VolumeFile outputT1T2RatioVol(t1Volume);
   outputT1T2RatioVol.setVoxelDataType(VolumeFile::VOXEL_DATA_TYPE_FLOAT);
   
   ///create output t1/2 ratio ribbon masked volume files (*.nii)
   m_outputRibbonMaskedT1T2RatioVol = new VolumeFile(t1Volume);//make sure it matches the t1, including spacing
   m_outputRibbonMaskedT1T2RatioVol->setVoxelDataType(VolumeFile::VOXEL_DATA_TYPE_FLOAT);
  
   ///2.  Divide T1w volume by T2w volume, output array T1w/T2w datatype float, also write <output-T1w/T2w-ratio-volume> to disk
   //note: would be slightly faster without this step, by calculating only in ribbon
   ///3. Mask T1w/T2w 3D array with ribbon binary mask, also write <output-T1w/T2w-ratio-ribbon-volume> to disk
   int t1Dim[3], t2Dim[3];
   t1Volume.getDimensions(t1Dim);
   t2Volume.getDimensions(t2Dim);
   if((t1Dim[0] != t2Dim[0]) || (t1Dim[1] != t2Dim[1]) || (t1Dim[2] != t2Dim[2])) throw FileException("t1 Dimensions do not match t2 dimensions");
   int maskDim[3];
   m_fsLabelVol->getDimensions(maskDim);
   if((t1Dim[0] != maskDim[0]) || (t1Dim[1] != maskDim[1]) || (t1Dim[2] != maskDim[2])) throw FileException("t1 Dimensions do not match t2 dimensions");
   float maskGlobalMean = 0.0f;
   float tempf, tempf2, tempf3;
   long long maskCount = 0;
   cout << "start volume math" << endl;
   for(int i = 0;i < t1Dim[0];i++)
   {
      for (int j = 0;j < t1Dim[1];j++)
      {
         for (int k = 0;k < t1Dim[2];k++)
         {
            tempf2 = t2Volume.getVoxel(i, j, k);
            if (tempf2 > 1.0f)
            {//assume t1Volume and t2Volume have no NaN or inf values
               tempf = t1Volume.getVoxel(i, j, k) / tempf2;
            } else {
               tempf = 0.0f;
            }
            outputT1T2RatioVol.setVoxel(i, j, k, 0, tempf);
            tempf3 = m_fsLabelVol->getVoxel(i, j, k);
            if (tempf3 == 3.0f || tempf3 == 42.0f)
            {
               maskGlobalMean += tempf;
               ++maskCount;
               m_outputRibbonMaskedT1T2RatioVol->setVoxel(i, j, k, 0, tempf);
            } else {
               m_outputRibbonMaskedT1T2RatioVol->setVoxel(i, j, k, 0, 0.0f);
            }
         }
      }
   }
   maskGlobalMean /= maskCount;//use stable stddev formula, 2 step
   float maskStdDev = 0.0f;
   for(int i = 0; i < t1Dim[0]; i++)
   {
      for(int j = 0; j < t1Dim[1]; j++)
      {
         for(int k = 0; k < t1Dim[2]; k++)
         {
            tempf = m_fsLabelVol->getVoxel(i, j, k);
            if(tempf == 3.0f || tempf == 42.0f)
            {
               tempf = outputT1T2RatioVol.getVoxel(i, j, k) - maskGlobalMean;
               maskStdDev += tempf * tempf;
            }
         }
      }
   }
   cout << "end volume math" << endl;
   maskStdDev = sqrt(maskStdDev / (maskCount - 1));//sample standard deviation
   m_volUpperLim = maskGlobalMean + maskStdDev * volOutliers;//set bounds for trusted values
   m_volLowerLim = maskGlobalMean - maskStdDev * volOutliers;
   cout << "volume thresholds are " << m_volLowerLim << " to " << m_volUpperLim << endl;
   
   outputT1T2RatioVol.setDataFileName(outputT1T2RatioVolFile);
   outputT1T2RatioVol.setDescriptiveLabel(outputT1T2RatioVolFile);
   outputT1T2RatioVol.writeFile(outputT1T2RatioVolFile);
   m_outputRibbonMaskedT1T2RatioVol->setDataFileName(outputRibbonMaskedT1T2RatioVolFile);
   m_outputRibbonMaskedT1T2RatioVol->setDescriptiveLabel(outputRibbonMaskedT1T2RatioVolFile);
   m_outputRibbonMaskedT1T2RatioVol->writeFile(outputRibbonMaskedT1T2RatioVolFile);
   
   ///4. Convert FWHM to sigma: sigma = FWHM / (2 * (sqrt(2 * log(2)))); regress thickness in terms of curvature and subtract the scale times curvature from thickness
   m_sigma = smoothFWHM / (2.0 * (sqrt(2.0 * log(2.0))));
   
   MetricFile leftOutput, rightOutput, leftCorrThickness, rightCorrThickness;
   MetricFile leftDebug, rightDebug;
   
   
   ///5.  CalculateRawMyelinMap for left hemisphere
   ///8.  CalculateRawMyelinMap for right hemisphere
   cout << "start raw mapping" << endl;
   doRawMapping(leftSurf, &leftBS, 3.0f, &leftThickness, &leftCorrThickness, &leftCurvature, &leftOutput);
   doRawMapping(rightSurf, &rightBS, 42.0f, &rightThickness, &rightCorrThickness, &rightCurvature, &rightOutput);
   leftOutput.setColumnName(0, QString("left raw myelin map"));
   rightOutput.setColumnName(0, QString("right raw myelin map"));
   cout << "end raw mapping" << endl;
   //Can delete volumes from memory here
   
   ///6.  CalculateCorrectedMyelinMap for left hemisphere
   ///9.  CalculateCorrectedMyelinMap for right hemisphere
   cout << "start correction" << endl;
   doCorrection(leftSurf, &leftThickness, &leftCorrThickness, &leftOutput, &leftOutput, &leftDebug);
   doCorrection(rightSurf, &rightThickness, &rightCorrThickness, &rightOutput, &rightOutput, &rightDebug);
   leftOutput.setColumnName(1, QString("left corrected myelin map"));
   rightOutput.setColumnName(1, QString("right corrected myelin map"));
   leftDebug.writeFile(QString("debug.L.metric"));
   rightDebug.writeFile(QString("debug.R.metric"));
   cout << "end correction" << endl;
   
   ///7.  Smooth corrected myelin map with function METRIC SMOOTHING according to calculated sigma (FWHM / (2 * (sqrt(2*log(2))))
   ///10.  Smooth corrected myelin map with function METRIC SMOOTHING according to calculated sigma (FWHM / (2 * (sqrt(2*log(2))))
   cout << "start smoothing" << endl;
   {
      /*BrainModelSurfaceMetricSmoothing smoothobj(NULL,
            leftSurf,
            NULL,
            &leftOutput,
            BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN,
            1,
            2,
            QString("left smoothed corrected myelin map"),
            1.0f,
            1,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            m_sigma);//*/
      BrainModelSurfaceROIMetricSmoothing smoothobj(NULL,
                                                    leftSurf,
                                                    &leftOutput,
                                                    &leftCorrThickness,
                                                    1,
                                                    2,
                                                    QString("left smoothed corrected myelin map"),
                                                    1.0f,
                                                    1,
                                                    1.0f,
                                                    1.0f,
                                                    1.0f,
                                                    1.0f,
                                                    1.0f,
                                                    m_sigma);
                                                    
      smoothobj.execute();
   }
   {
      /*BrainModelSurfaceMetricSmoothing smoothobj2(NULL,
            rightSurf,
            NULL,
            &rightOutput,
            BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_GEODESIC_GAUSSIAN,
            1,
            2,
            QString("right smoothed corrected myelin map"),
            1.0f,
            1,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            1.0f,
            m_sigma);//*/
      BrainModelSurfaceROIMetricSmoothing smoothobj2(NULL,
                                                     rightSurf,
                                                     &rightOutput,
                                                     &rightCorrThickness,
                                                     1,
                                                     2,
                                                     QString("right smoothed corrected myelin map"),
                                                     1.0f,
                                                     1,
                                                     1.0f,
                                                     1.0f,
                                                     1.0f,
                                                     1.0f,
                                                     1.0f,
                                                     m_sigma);
                                                    
      smoothobj2.execute();
   }
   cout << "end smoothing" << endl;
   
   leftOutput.addColumns(1);
   rightOutput.addColumns(1);
   vector<float> copyData;
   leftCorrThickness.getColumnForAllNodes(0, copyData);
   leftOutput.setColumnForAllNodes(leftOutput.getNumberOfColumns() - 1, copyData);
   leftOutput.setColumnName(leftOutput.getNumberOfColumns() - 1, QString("left corrected thickness"));
   rightCorrThickness.getColumnForAllNodes(0, copyData);
   rightOutput.setColumnForAllNodes(rightOutput.getNumberOfColumns() - 1, copyData);
   rightOutput.setColumnName(rightOutput.getNumberOfColumns() - 1, QString("right corrected thickness"));
   
   ///11. Save raw surface myelin map (“Myelin Map Raw”), corrected myelin map (“Myelin Map”), smoothed corrected myelin map (“Myelin Map Smoothed [FWHM]mm”) and corrected cortical thickness map (“Curvature Corrected Thickness Map”) to appropriate <left-output-metric-file-name> or <right-output-metric-file-name>
   leftOutput.writeFile(leftMMFile);
   rightOutput.writeFile(rightMMFile);
   
   delete m_fsLabelVol;
   delete m_outputRibbonMaskedT1T2RatioVol;
}

void CommandMyelinMapping::doRawMapping(BrainModelSurface* surface,
               BrainSet* myset,
               float ribbonLabel,
               MetricFile* thickness,
               MetricFile* corrThickness,
               MetricFile* curvature,
               MetricFile* output)
{
   float myspacing[3], myorigin[3];
   int mydim[3];
   float invnegsigmasquaredx2 = -1.0f / (2.0f * m_sigma * m_sigma);//to make gaussian function require only one multiply and one exp()
   long numNodes = surface->getNumberOfNodes();
   output->setNumberOfNodesAndColumns(numNodes, 1);
   corrThickness->setNumberOfNodesAndColumns(numNodes, 1);
   MetricFile cleanThickness;
   cleanThickness.setNumberOfNodesAndColumns(numNodes, 1);
   output->setColumnName(0, QString("raw myelin map"));
   CoordinateFile* mycoords = surface->getCoordinateFile();
   TopologyFile* mytopo = surface->getTopologyFile();
   m_outputRibbonMaskedT1T2RatioVol->getSpacing(myspacing);//this should be identical to t1 spacing
   m_outputRibbonMaskedT1T2RatioVol->getOrigin(myorigin);
   m_outputRibbonMaskedT1T2RatioVol->getDimensions(mydim);
   BrainModelSurfaceROINodeSelection thicknessCleanup(myset);
   thicknessCleanup.selectNodesWithMetric(BrainModelSurfaceROINodeSelection::SELECTION_LOGIC_NORMAL,
                                          surface,
                                          thickness,
                                          0, 0.0f, 0.0f
   );//selects medial wall, plus artifacts outside, minus artifacts inside
   thicknessCleanup.discardIslands(surface);//discards outside artifacts
   thicknessCleanup.invertSelectedNodes(surface);//now everything except medial wall, plus artifacts inside
   thicknessCleanup.discardIslands(surface);//discard artifacts inside medial wall
#ifdef _OPENMP
#pragma omp parallel
#endif
   {
      float tempf;
      GeodesicHelper mygeohelp(mycoords, mytopo);//this should probably be consolidated across the two places it is done, but that will be tough
      vector<int> nodeslist;//better solution: a GeodesicHelperBase that contains the disntaces, and GeodesicHelper contains the mutex and local arrays only
      vector<float> distancelist;
      float totalweight, weight, weightedsum;
#ifdef _OPENMP
#pragma omp for
#endif
      for (long i = 0; i < numNodes; ++i)
      {
         tempf = thickness->getValue(i, 0);
         if (thicknessCleanup.getNodeSelected(i) == (tempf == 0.0f))
         {//selected and zero, or not selected and nonzero
            if (tempf)
            {
               cleanThickness.setValue(i, 0, 0.0f);
            } else {
               mygeohelp.getNodesToGeoDist(i, m_sigma * 4, nodeslist, distancelist);
               totalweight = 0.0f;
               weightedsum = 0.0f;
               for (long j = 0; j < (long)nodeslist.size(); ++j)
               {
                  tempf = thickness->getValue(nodeslist[j], 0);
                  if (thicknessCleanup.getNodeSelected(i) && tempf != 0.0f)
                  {
                     weight = exp(distancelist[j] * invnegsigmasquaredx2);
                     totalweight += weight;
                     weightedsum += weight * tempf;
                  }
               }
               if (totalweight > 0.0f)
               {
                  cleanThickness.setValue(i, 0, weightedsum / totalweight);
               } else {
                  cleanThickness.setValue(i, 0, 0.0f);
               }
            }
         } else {
            cleanThickness.setValue(i, 0, tempf);
         }
      }
   }
   vector<vector<float> > regressMat, tempmat1, tempmat2, indvars, indvarstrans, depvars;
   MatrixUtilities matUtil;
   matUtil.resize(2, numNodes, indvars);//regression for a line
   matUtil.resize(numNodes, 1, depvars);
#ifdef _OPENMP
#pragma omp parallel for
#endif
   for (long i = 0; i < numNodes; ++i)
   {
      float tempf = cleanThickness.getValue(i, 0);
      if (tempf != 0.0f)
      {
         indvars[1][i] = 1.0f;//constant term
         indvars[0][i] = curvature->getValue(i, 0);
         depvars[i][0] = tempf;
      }
   }
   matUtil.transpose(indvars, indvarstrans);
   matUtil.multiply(indvars, indvarstrans, tempmat1);//this does a bit more work than actually needed, but it is easier to read
   matUtil.multiply(indvars, depvars, tempmat2);
   matUtil.horizCat(tempmat1, tempmat2, regressMat);
   matUtil.rref(regressMat);//our answer for slope is in [0][2]
#ifdef _OPENMP
#pragma omp parallel for
#endif
   for (long i = 0; i < numNodes; ++i)
   {
      float tempf = cleanThickness.getValue(i, 0);
      if (tempf != 0.0f)
      {
         corrThickness->setValue(i, 0, tempf - curvature->getValue(i, 0) * regressMat[0][2]);
      } else {
         corrThickness->setValue(i, 0, 0.0f);
      }
   }
   //debug code
   long long withinMask = 0, failedThreshold = 0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+: withinMask, failedThreshold)
#endif
   for (long i = 0; i < numNodes; ++i)
   {
      float thisNodeThickness = corrThickness->getValue(i, 0);
      if (thisNodeThickness == 0.0f)
      {
         output->setValue(i, 0, 0.0f);
         continue;
      }
      float totalDistSquare, delta[3], tempf;
      float* thisNodeXYZ;
      float weightedsum = 0.0f, totalweight = 0.0f;
      thisNodeXYZ = mycoords->getCoordinate(i);
      totalDistSquare = thisNodeThickness * thisNodeThickness;//this controls euclidean distance cutoff
      int vibegin = (int)ceil((thisNodeXYZ[0] - thisNodeThickness - myorigin[0]) / myspacing[0]);
      int viend = (int)floor((thisNodeXYZ[0] + thisNodeThickness - myorigin[0]) / myspacing[0]);
      if (vibegin < 0) vibegin = 0;
      if (viend >= mydim[0]) viend = mydim[0] - 1;
      for (int vi = vibegin; vi <= viend; ++vi)
      {//this loop should automatically filter out zero and negative thicknesses
         delta[0] = vi * myspacing[0] + myorigin[0] - thisNodeXYZ[0];
         float vjdistsquareleft = totalDistSquare - delta[0] * delta[0];
         tempf = sqrt(vjdistsquareleft);
         int vjbegin = (int)ceil((thisNodeXYZ[1] - tempf - myorigin[1]) / myspacing[1]);
         int vjend = (int)floor((thisNodeXYZ[1] + tempf - myorigin[1]) / myspacing[1]);
         if (vjbegin < 0) vjbegin = 0;
         if (vjend >= mydim[1]) vjend = mydim[1] - 1;
         for (int vj = vjbegin; vj <= vjend; ++vj)
         {
            delta[1] = vj * myspacing[1] + myorigin[1] - thisNodeXYZ[1];
            float vkdistsquareleft = vjdistsquareleft - delta[1] * delta[1];
            tempf = sqrt(vkdistsquareleft);
            int vkbegin = (int)ceil((thisNodeXYZ[2] - tempf - myorigin[2]) / myspacing[2]);
            int vkend = (int)floor((thisNodeXYZ[2] + tempf - myorigin[2]) / myspacing[2]);
            if (vkbegin < 0) vkbegin = 0;
            if (vkend >= mydim[2]) vkend = mydim[2] - 1;
            for (int vk = vkbegin; vk <= vkend; ++vk)
            {
               delta[2] = vk * myspacing[2] + myorigin[2] - thisNodeXYZ[2];
               float distancesqr = totalDistSquare - vkdistsquareleft + delta[2] * delta[2];
               if (distancesqr < 0.0f)
               {//can happen (VERY infrequently) due to rounding errors, just skip
                  continue;
               }
               if (m_fsLabelVol->getVoxel(vi, vj, vk) == ribbonLabel)
               {
                  const float* mynormal = surface->getNormal(i);
                  tempf = mynormal[0] * delta[0] + mynormal[1] * delta[1] + mynormal[2] * delta[2];
                  if (fabs(tempf) < thisNodeThickness * 0.5f)
                  {//if you reach here, you are within euclidean distance, inside mask, and within half thickness along normal
                     ++withinMask;
                     tempf = m_outputRibbonMaskedT1T2RatioVol->getVoxel(vi, vj, vk);
                     if (tempf < m_volUpperLim && tempf > m_volLowerLim)
                     {
                        float weight = exp(distancesqr * invnegsigmasquaredx2);
                        weightedsum += weight * tempf;
                        totalweight += weight;
                     } else {
                        ++failedThreshold;
                     }
                  }
               }
            }
         }
      }
      if (totalweight > 0.0f)
      {
         output->setValue(i, 0, weightedsum / totalweight);
      } else {
         output->setValue(i, 0, 0.0f);
      }
   }//output is now the RAW map
   cout << 100.0f * (float)failedThreshold / (float)withinMask << "% of in-range, in-mask voxels failed the threshold" << endl;
}

void CommandMyelinMapping::doCorrection(BrainModelSurface* surface,
                                        MetricFile* thickness,
                                        MetricFile* corrThickness,
                                        MetricFile* rawMap,
                                        MetricFile* output,
                                        MetricFile* debug)
{
   int numNodes = surface->getNumberOfNodes();
   int colNum = 0;
   if (output->getNumberOfNodes() != numNodes)
   {
      output->setNumberOfNodesAndColumns(numNodes, 1);
   } else {
      output->addColumns(1);
      colNum = output->getNumberOfColumns() - 1;
   }
   output->setColumnName(colNum, QString("corrected myelin map"));
   float invnegsigmasquaredx2 = -1.0f / (2.0f * m_sigma * m_sigma);//to make gaussian function require only one multiply and one exp()
   //debug code
   debug->setNumberOfNodesAndColumns(numNodes, 2);
   debug->setColumnName(0, QString("reconstructed nodes"));
   debug->setColumnName(1, QString("nodes ignored at least once in reconstruction"));
   for (int i = 0; i < numNodes; ++i)
   {
      debug->setValue(i, 0, 0.0f);
      debug->setValue(i, 1, 0.0f);
   }
   long long zeroNodes = 0, nodesFaked = 0, nodesInCortex = 0, nodesConsidered = 0, nodesRejected = 0, zeroNeighbors = 0;
#ifdef _OPENMP
#pragma omp parallel reduction(+: zeroNodes, nodesFaked, nodesInCortex, nodesConsidered, nodesRejected, zeroNeighbors)
#endif
   {
      vector<int> neighbors;
      vector<float> geodists;
      float tempf, tempf2;
      GeodesicHelper mygeohelp(surface->getCoordinateFile(), surface->getTopologyFile());
      TopologyHelper mytopohelp(surface->getTopologyFile(), false, true, false);
#ifdef _OPENMP
#pragma omp for
#endif
      for (int i = 0; i < numNodes; ++i)
      {
         if (thickness->getValue(i, 0) > 0.0f)
         {
            ++nodesInCortex;
            mytopohelp.getNodeNeighborsToDepth(i, m_neighborDepth, neighbors);
            int numNeigh = neighbors.size();
            float thickmean = 0.0f, thickstddev = 0.0f, rawmean = 0.0f, rawstddev = 0.0f;
            for (int j = 0; j < numNeigh; ++j)
            {
               tempf = rawMap->getValue(neighbors[j], 0);
               if (tempf != 0.0f)
               {
                  rawmean += tempf;
                  thickmean += corrThickness->getValue(neighbors[j], 0);
               }
            }
            thickmean /= numNeigh;
            rawmean /= numNeigh;
            for (int j = 0; j < numNeigh; ++j)
            {
               tempf2 = rawMap->getValue(neighbors[j], 0);
               if (tempf2 != 0.0f)
               {
                  tempf = rawmean - tempf2;
                  rawstddev += tempf * tempf;
                  tempf = thickmean - corrThickness->getValue(neighbors[j], 0);
                  thickstddev += tempf * tempf;
               }
            }
            thickstddev = sqrt(thickstddev / (numNeigh - 1));
            rawstddev = sqrt(rawstddev / (numNeigh - 1));
            tempf = rawstddev * m_numStdDev;
            float rawMax = rawmean + tempf;
            float rawMin = rawmean - tempf;
            tempf = thickstddev * m_numStdDev;
            float thickMax = thickmean + tempf;
            float thickMin = thickmean - tempf;
            tempf = rawMap->getValue(i, 0);
            tempf2 = corrThickness->getValue(i, 0);
            if (tempf == 0.0f || ((tempf < rawMin || tempf > rawMax) && (tempf2 < thickMin || tempf2 > thickMax)))
            {
               debug->setValue(i, 0, 1.0f);
               if (tempf == 0.0f)
               {
                  ++zeroNodes;
               } else {
                  ++nodesFaked;
               }
               float weightedsum = 0.0f;
               float totalweight = 0.0f;
               mygeohelp.getGeoToTheseNodes(i, neighbors, geodists);
               for (int j = 0; j < numNeigh; ++j)
               {
                  ++nodesConsidered;
                  tempf = rawMap->getValue(neighbors[j], 0);
                  tempf2 = corrThickness->getValue(neighbors[j], 0);
                  if (tempf != 0.0f && (tempf > rawMin && tempf < rawMax) && (tempf2 > thickMin && tempf2 < thickMax))
                  {
                     float weight = exp(geodists[j] * geodists[j] * invnegsigmasquaredx2);
                     weightedsum += weight * tempf;
                     totalweight += weight;
                  } else {
                     debug->setValue(i, 1, 1.0f);
                     if (tempf == 0.0f)
                     {
                        ++zeroNeighbors;
                     } else {
                        ++nodesRejected;
                     }
                  }
               }
               if (totalweight != 0.0f)
               {
                  output->setValue(i, colNum, weightedsum / totalweight);
               } else {
                  output->setValue(i, colNum, 0.0f);
               }
            } else {
               output->setValue(i, colNum, rawMap->getValue(i, 0));
            }
         } else {
            output->setValue(i, colNum, 0.0f);
         }
      }
   }
   cout << "Out of " << nodesInCortex << " cortex nodes:" << endl;
   cout << 100.0f * (float)zeroNodes / (float)nodesInCortex << "% were zero and needed reconstruction" << endl;
   cout << 100.0f * (float)nodesFaked / (float)nodesInCortex << "% were discarded by threshold" << endl;
   cout << "When reconstructing discarded nodes:" << endl;
   cout << 100.0f * (float)zeroNeighbors / (float)nodesConsidered << "% were zero and not used" << endl;
   cout << 100.0f * (float)nodesRejected / (float)nodesConsidered << "% were outside threshold and ignored" << endl;
}

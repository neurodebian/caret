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

#include <cmath>
#include <cstdlib>
#include <iostream> 
 
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QGlobalStatic>
#include <QImageIOPlugin>
#include <QLabel>
#include <QPixmap>
#include <QPluginLoader>
#include <QScrollArea>
#include <QString>

#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BorderProjectionUnprojector.h"
#include "BrainModelOpenGL.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceDeformDataFile.h"
#include "BrainModelSurfaceDeformationSpherical.h"
#include "BrainModelSurfaceMetricAnovaOneWay.h"
#include "BrainModelSurfaceMetricInGroupDifference.h"
#include "BrainModelSurfaceMetricInterHemClusters.h"
#include "BrainModelSurfaceMetricCoordinateDifference.h"
#include "BrainModelSurfaceMetricTwoSampleTTest.h"
#include "BrainModelSurfaceMultiresolutionMorphing.h"
#include "BrainModelSurfaceMetricOneAndPairedTTest.h"
#include "BrainModelSurfaceMetricTwinComparison.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainModelSurfaceRegionOfInterest.h"
#include "BrainModelSurfaceSulcalDepthWithNormals.h"
#include "BrainModelSurfaceToVolumeConverter.h"
#include "BrainModelSurfaceToVolumeSegmentationConverter.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeGradient.h"
#include "BrainModelVolumeNearToPlane.h"
#include "BrainModelVolumeSegmentationStereotaxic.h"
#include "BrainModelVolumeSureFitSegmentation.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "CaretVersion.h"
#include "Categories.h"
#include "CellFileProjector.h"
#include "CellProjectionUnprojector.h"
#include "CommandLineUtilities.h"
#include "DebugControl.h"
#include "DeformationMapFile.h"
#include "DisplaySettingsVolume.h"
#include "FileUtilities.h"
#include "FociFile.h"
#include "FociProjectionFile.h"
#include "GiftiDataArray.h"
#include "ImageFile.h"
#include "OffScreenOpenGLWidget.h"
#include "PaintFile.h"
#include "SegmentationMaskListFile.h"
#include "SpecFile.h"
#include "Species.h"
#include "StatisticHistogram.h"
#include "StatisticUnitTesting.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TextFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VectorFile.h"
#include "VolumeFile.h"
#include "XhtmlTableExtractorFile.h"

/*----------------------------------------------------------------------------------------
 * Global Variables.
 */
static QString programName;
static const QString outputVolumeDelimeter(":::");

static const int HELP_BRIEF = -1;
static const int HELP_FULL  = -2;

static QApplication* myApplication = NULL;

static int displayQImage(const QImage& qimage);

/*----------------------------------------------------------------------------------------
 *
 */
static void
printSeparatorLine()
{
   std::cout << "------------------------------------------------------------------------------" << std::endl;
}

/*----------------------------------------------------------------------------------------
 * Get the input and output file names.
 */
static void
splitOutputNameIntoFileNameAndLabel(QString& outputName,
                                    QString& labelOut)
{
   labelOut = "";
   int colonPos = outputName.indexOf(outputVolumeDelimeter);
   if (colonPos != -1) {
      labelOut = outputName.mid(0, colonPos);
      outputName = outputName.mid(colonPos + outputVolumeDelimeter.length());
   }
}

/*----------------------------------------------------------------------------------------
 * Get the input and output volume file names.
 */
static void
getInputAndOutputVolumeFileNames(const int argc, 
                           char* argv[],
                           int& argIndex,
                           QString& inputName,
                           QString& outputName,
                           QString& outputLabel)
{

   CommandLineUtilities::getNextParameter("Input Volume File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputName);
   CommandLineUtilities::getNextParameter("Output Volume File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputName);
   splitOutputNameIntoFileNameAndLabel(outputName, outputLabel);
}

/*----------------------------------------------------------------------------------------
 * Get the output volume file names.
 */
static void
getOutputVolumeFileName(const int argc, 
                        char* argv[],
                        int& argIndex,
                        QString& outputName,
                        QString& outputLabel)
{
   CommandLineUtilities::getNextParameter("Output Volume File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputName);
   splitOutputNameIntoFileNameAndLabel(outputName, outputLabel);
}

/*----------------------------------------------------------------------------------------
 * Read a volume file.
 */
static void
readVolumeFile(VolumeFile& vf, const QString& name)
{
   try {
      vf.readFile(name);
   }
   catch (FileException& e) {
      std::cout << std::endl;
      std::cout << "ERROR reading: " << name.toAscii().constData() << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl << std::endl;
      exit(-1);
   }
}

/*----------------------------------------------------------------------------------------
 * Write a volume file.
 */
static void
writeVolumeFile(VolumeFile& vf, 
                const QString& name,
                const QString& volumeLabel)
{
   try {
      if (volumeLabel.isEmpty() == false) {
         vf.setDescriptiveLabel(volumeLabel);
      }
      
      //
      // Always write float since some operations have results ranging 0.0 to 1.0
      //
      vf.setVoxelDataType(VolumeFile::VOXEL_DATA_TYPE_FLOAT);
      
      vf.writeFile(name);
   }
   catch (FileException& e) {
      std::cout << std::endl;
      std::cout << "ERROR writing: " << name.toAscii().constData() << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl << std::endl;
      exit(-1);
   }
}

/*----------------------------------------------------------------------------------------
 * Read a coordinate file.
 */
static void
readCoordinateFile(CoordinateFile& cf, const QString& name)
{
   try {
      cf.readFile(name); 
   }
   catch (FileException& e) {
      std::cout << std::endl;
      std::cout << "ERROR reading: " << name.toAscii().constData() << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl << std::endl;
      exit(-1);
   }
}

/*----------------------------------------------------------------------------------------
 * Write a coordinate file.
 */
static void
writeCoordinateFile(CoordinateFile& cf, 
                const QString& name)
{
   try {
      cf.writeFile(name);
   }
   catch (FileException& e) {
      std::cout << std::endl;
      std::cout << "ERROR writing: " << name.toAscii().constData() << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl << std::endl;
      exit(-1);
   }
}

/*----------------------------------------------------------------------------------------
 * Write a surface shape file.
 */
static void
writeSurfaceShapeFile(SurfaceShapeFile& ssf, 
                      const QString& name)
{
   try {
      ssf.writeFile(name);
   }
   catch (FileException& e) {
      std::cout << std::endl;
      std::cout << "ERROR writing: " << name.toAscii().constData() << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl << std::endl;
      exit(-1);
   }
}

/*----------------------------------------------------------------------------------------
 * Read a vector file.
 */
static void
readVectorFile(VectorFile& vf, const QString& name)
{
   try {
      vf.readFile(name);
   }
   catch (FileException& e) {
      std::cout << std::endl;
      std::cout << "ERROR reading: " << name.toAscii().constData() << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl << std::endl;
      exit(-1);
   }
}

/*----------------------------------------------------------------------------------------
 * Write a vector file.
 */
static void
writeVectorFile(VectorFile& vf, const QString& name)
{
   try {
      vf.writeFile(name);
   }
   catch (FileException& e) {
      std::cout << std::endl;
      std::cout << "ERROR writing: " << name.toAscii().constData() << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl << std::endl;
      exit(-1);
   }
}

/*----------------------------------------------------------------------------------------
 * Get the extent from a list of parameters.
 */
static void
getExtentFromParameters(const int argc, char* argv[], int& argIndex, int extentOut[6])
{
   CommandLineUtilities::getNextParameter("Extent Min X",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          extentOut[0]);
   CommandLineUtilities::getNextParameter("Extent Max X",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          extentOut[1]);
   CommandLineUtilities::getNextParameter("Extent Min Y",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          extentOut[2]);
   CommandLineUtilities::getNextParameter("Extent Max Y",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          extentOut[3]);
   CommandLineUtilities::getNextParameter("Extent Min Z",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          extentOut[4]);
   CommandLineUtilities::getNextParameter("Extent Max Z",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          extentOut[5]);
}

/*----------------------------------------------------------------------------------------
 * Get the seed from a list of parameters.
 */
static void
getSeedFromParameters(const int argc, char* argv[], int& argIndex, int seedOut[3])
{
   CommandLineUtilities::getNextParameter("Seed X",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          seedOut[0]);
   CommandLineUtilities::getNextParameter("Seed Y",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          seedOut[1]);
   CommandLineUtilities::getNextParameter("Seed Z",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          seedOut[2]);
}

/*----------------------------------------------------------------------------------------
 * Get the seed from a list of parameters.
 */
static VolumeFile::VOLUME_AXIS
getAxisFromParameters(const int argc, char* argv[], int& argIndex)
{
   QString axisString;
   CommandLineUtilities::getNextParameter("Axis",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          axisString);
   VolumeFile::VOLUME_AXIS axis;
   if ((axisString == "X") || (axisString == "x")) {
      axis = VolumeFile::VOLUME_AXIS_X;
   }
   else if ((axisString == "Y") || (axisString == "y")) {
      axis = VolumeFile::VOLUME_AXIS_Y;
   }
   else if ((axisString == "Z") || (axisString == "z")) {
      axis = VolumeFile::VOLUME_AXIS_Z;
   }
   else {
      std::cout << "Invalid axis: \"" << axisString.toAscii().constData() << "\"." << std::endl;
      exit(-1);
   }
   return axis;
}

/*----------------------------------------------------------------------------------------
 * Print brief help.
 */
static void
printBriefHelp(const QString& name, const QString& command)
{
   std::cout << "   "
             << name.toAscii().constData()
             << "   "
             << command.toAscii().constData()
             << std::endl;
}

/*----------------------------------------------------------------------------------------
 * Threshold a volume.
 */
static int
operationVolumeThreshold(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME THRESHOLDING", "-volume-threshold");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME THRESHOLDING" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-threshold <f-value> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Voxels less than \"value\" are set to zero.  Voxels greater than " << std::endl
         << "      or equal to \"value\" are set to 255." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float thresholdValue;
   CommandLineUtilities::getNextParameter("Threshold value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          thresholdValue);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // Threshold the volume
   //
   volume.thresholdVolume(thresholdValue);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Threshold a volume.
 */
static int
operationVolumeThresholdDual(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME THRESHOLDING DUAL", "-volume-threshold-dual");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME THRESHOLDING DUAL" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-threshold-dual <f-value-low> <f-value-high>  \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Voxels between <value-low>  and <value-high> are set to 255." << std::endl
         << "      All other voxels are set to 0." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float thresholdLowValue;
   CommandLineUtilities::getNextParameter("Threshold low value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          thresholdLowValue);
   float thresholdHighValue;
   CommandLineUtilities::getNextParameter("Threshold high value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          thresholdHighValue);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // Threshold the volume
   //
   volume.dualThresholdVolume(thresholdLowValue, thresholdHighValue);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Inverse Threshold a volume.
 */
static int
operationVolumeThresholdInverse(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME THRESHOLDING INVERSE", "-volume-threshold-inverse");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME THRESHOLDING INVERSE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-threshold-inverse <f-value> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Voxels greater than \"value\" are set to zero.  Voxels less than " << std::endl
         << "      or equal to \"value\" are set to 255." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float thresholdValue;
   CommandLineUtilities::getNextParameter("Inverse Threshold value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          thresholdValue);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // Inverse Threshold the volume
   //
   volume.inverseThresholdVolume(thresholdValue);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Close a volume.
 */
static int
operationVolumeClosing(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME CLOSING", "-volume-closing");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME CLOSING" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-closing <i-iterations> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Apply closing to the volume for the specified number of iterations. " << std::endl
         << "      Closing is simply dilation followed by erosion." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int iterations;
   CommandLineUtilities::getNextParameter("Closing Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // dilate
   //
   for (int i = 0; i < iterations; i++) {
      volume.doVolMorphOps(1, 0);
      volume.doVolMorphOps(0, 1);
   }
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Dilate a volume.
 */
static int
operationVolumeDilate(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME DILATE", "-volume-dilate");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME DILATE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-dilate <i-iterations> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Dilate the volume for the specified number of iterations. " << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int iterations;
   CommandLineUtilities::getNextParameter("Dilate Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // dilate
   //
   volume.doVolMorphOps(iterations, 0);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * erode a volume.
 */
static int
operationVolumeErode(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME ERODE", "-volume-erode");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME ERODE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-erode <i-iterations> \\" << std::endl
         << "       <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Erode the volume for the specified number of iterations. " << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int iterations;
   CommandLineUtilities::getNextParameter("Erode Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // erode
   //
   volume.doVolMorphOps(0, iterations);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * dilate and erode a volume.
 */
static int
operationVolumeDilateErode(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME DILATE AND ERODE", "-volume-dilate-erode");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME DILATE AND ERODE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-dilate-erode <i-dialate-iterations> \\" << std::endl
         << "         <i-erode-iterations> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Dilate and erode the volume for the specified number of iterations. " << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int dilateIterations;
   CommandLineUtilities::getNextParameter("Dilate Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          dilateIterations);
   int erodeIterations;
   CommandLineUtilities::getNextParameter("Erode Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          erodeIterations);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // erode
   //
   volume.doVolMorphOps(dilateIterations, erodeIterations);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * dilate and erode a volume.
 */
static int
operationVolumeDilateErodeWithinMask(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME DILATE AND ERODE WITHIN MASK", "-volume-dilate-erode-mask");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME DILATE AND ERODE WITHIN MASK" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-dilate-erode-mask \\" << std::endl
         << "         <i-dialate-iterations> <i-erode-iterations> \\" << std::endl
         << "         <i-min-x> <i-max-x> <i-min-y> <i-max-y> <i-min-z> <i-max-z> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Dilate and erode the volume for the specified number of iterations." << std::endl
         << "      Only voxels within extent are modified." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int dilateIterations;
   CommandLineUtilities::getNextParameter("Erode Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          dilateIterations);
   int erodeIterations;
   CommandLineUtilities::getNextParameter("Erode Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          erodeIterations);
   int extent[6];
   getExtentFromParameters(argc, argv, argIndex, extent);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // erode
   //
   volume.doVolMorphOpsWithinMask(extent, dilateIterations, erodeIterations);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * erode a volume. 
 */
static int
operationVolumeEulerCount(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME EULER COUNT", "-volume-euler");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME EULER COUNT" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-euler \\" << std::endl
         << "          <i-min-x> <i-max-x> <i-min-y> <i-max-y> <i-min-z> <i-max-z> \\" << std::endl
         << "          <input-volume-name>" << std::endl
         << "         " << std::endl
         << "      Perform and print the Euler Counts for a segmentation volume. " << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int extent[6];
   getExtentFromParameters(argc,
                           argv,
                           argIndex,
                           extent);
   QString inputName;
   CommandLineUtilities::getNextParameter("Input Volume File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputName);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   int NumObjects, NumCavities, NumHandles, eulerCount;
   volume.getEulerCountsForSegmentationSubVolume(NumObjects,
                                                 NumCavities,
                                                 NumHandles,
                                                 eulerCount,
                                                 extent);

   //
   // Print the counts
   //
   std::cout << "Num Objects  " << NumObjects << std::endl;
   std::cout << "Num Cavities " << NumCavities << std::endl;
   std::cout << "Num Handles  " << NumHandles << std::endl;
   std::cout << "Euler Count  " << eulerCount << std::endl;
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Fill biggest object in a volume.
 */
static int
operationVolumeFillBiggestObject(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME FILL BIGGEST OBJECT", "-volume-fill-biggest-object");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME FILL BIGGEST OBJECT" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-fill-biggest-object  \\" << std::endl
         << "         <i-min-x> <i-max-x> <i-min-y> <i-max-y> <i-min-z> <i-max-z> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Fill the biggest object with extent and remove all islands." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int extent[6];
   getExtentFromParameters(argc, argv, argIndex, extent);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // find the biggest object and remove any islands.
   //
   volume.fillBiggestObjectWithinMask(extent, 255.0, 255.0);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Print the estimated peaks from a histogram.
 */
static void
printHistogramPeaks(const StatisticHistogram* histo)
{
   int grayPeakBucketNumber;
   int whitePeakBucketNumber;
   int grayMinimumBucketNumber;
   int whiteMaximumBucketNumber;
   int grayWhiteBoundaryBucketNumber;
   int csfPeakBucketNumber;
   histo->getGrayWhitePeakEstimates(grayPeakBucketNumber,
                                    whitePeakBucketNumber,
                                    grayMinimumBucketNumber,
                                    whiteMaximumBucketNumber,
                                    grayWhiteBoundaryBucketNumber,
                                    csfPeakBucketNumber);
   //
   // Print results
   //
   std::cout << std::endl;
   std::cout << "CSF Peak:            " << histo->getDataValueForBucket(csfPeakBucketNumber) << std::endl;
   std::cout << "Gray Minimum:        " << histo->getDataValueForBucket(grayMinimumBucketNumber) << std::endl;
   std::cout << "Gray Peak:           " << histo->getDataValueForBucket(grayPeakBucketNumber) << std::endl;
   std::cout << "Gray/White Boundary: " << histo->getDataValueForBucket(grayWhiteBoundaryBucketNumber) << std::endl;
   std::cout << "White Peak:          " << histo->getDataValueForBucket(whitePeakBucketNumber) << std::endl;
   std::cout << "White Maximum:       " << histo->getDataValueForBucket(whiteMaximumBucketNumber) << std::endl;
   std::cout << std::endl;

}

/*----------------------------------------------------------------------------------------
 * Estimate the peaks in an anatomy volume.
 */
static int
operationVolumeAnatomyPeaks(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME ANATOMY PEAK ESTIMATION", "-volume-anatomy-peaks");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME ANATOMY PEAK ESTIMATION" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-anatomy-peaks \\ " << std::endl
         << "         <anatomy-volume-name>" << std::endl
         << "         " << std::endl
         << "      Estimate the peaks in an anatomy volume file. " << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   QString anatomyVolumeFileName;
   CommandLineUtilities::getNextParameter("Anatomy Volume File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          anatomyVolumeFileName);

   //
   // Read the anatomy volume file
   //
   VolumeFile volume;
   readVolumeFile(volume, anatomyVolumeFileName);
   
   
   //
   // Get the histogram and the estimated peaks
   //
   const StatisticHistogram* histo = volume.getHistogram();
   printHistogramPeaks(histo);

   delete histo;
   histo = NULL;
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Perform bias correction on a volume.
 */
static int
operationVolumeBiasCorrection(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME BIAS CORRECTION", "-volume-bias-correction");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME BIAS CORRECTION" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-bias-correction \\ " << std::endl
         << "         <i-gray-minimum> <i-white-maximum> <i-iterations>  \\ " << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Perform bias correction on a volume. " << std::endl
         << "      " << std::endl
         << "      \"gray-minimum\" is a value that is less than but close to the minimum" << std::endl
         << "      value of all gray matter voxels." << std::endl
         << "      " << std::endl
         << "      \"white-maximum\" is a value that is greater than but close to the maximum" << std::endl
         << "      value of all white matter voxels." << std::endl
         << "      " << std::endl
         << "      \"iterations\" is the number of iterations used in the bias correction" << std::endl
         << "      algorithm.  A value of 5 is sufficient in most cases." << std::endl
         << "      " << std::endl
         << "      This algorithm was taken from AFNI's 3dUniformize program." << std::endl
         << std::endl;
      return 0;
   }
   
   int grayMinimum;
   CommandLineUtilities::getNextParameter("Gray Minimum",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          grayMinimum);

   int whiteMaximum;
   CommandLineUtilities::getNextParameter("White Maximum",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          whiteMaximum);
                                          
   int iterations;
   CommandLineUtilities::getNextParameter("Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);
   //
   // Get the parameters
   //
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // bias correction
   //
   volume.biasCorrectionWithAFNI(grayMinimum, whiteMaximum, iterations);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Blur a volume.
 */
static int
operationVolumeBlur(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME BLUR", "-volume-blur");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME BLUR" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-blur <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Blur the volume. " << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // blur
   //
   volume.blur();
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Mask a volume.
 */
static int
operationVolumeMaskVolume(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME MASK VOLUME", "-volume-mask-volume");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME MASK VOLUME" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-mask-volume \\ " << std::endl
         << "         <i-min-x> <i-max-x> <i-min-y> <i-max-y> <i-min-z> <i-max-z> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Clear all of the voxels outside the specified extent. " << std::endl
         << "      " << std::endl
         << "      " << programName.toAscii().constData() << " -volume-mask-volume <i-dilation-iterations>\\ " << std::endl
         << "         <input-volume-name> <mask-volume> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Clear all of the voxels in the input volume that do map into " << std::endl
         << "      a non-zero voxel in the mask volume.  The mapping is performed" << std::endl
         << "      using stereotaxic coordinates so the volumes do not need to have" << std::endl
         << "      have the same dimensions but must overlap in stereotaxic " << std::endl
         << "      coordinates.  Create a mask volume using this program with the" << std::endl
         << "      \"-volume-segment-mask-creation\" option." << std::endl
         << "      " << std::endl
         << "      If \"dilation-iterations\" is non-zero, the mask will be dilated" << std::endl
         << "      the specified number of times prior to the application of the mask." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Mask with volume file?
   //
   if (argc == 6) {
      int dilationIterations;
      CommandLineUtilities::getNextParameter("Dilation Iterations",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             dilationIterations);

      QString inputFileName;
      CommandLineUtilities::getNextParameter("Input Filename",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             inputFileName);

      QString maskFileName;
      CommandLineUtilities::getNextParameter("Mask Filename",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             maskFileName);

      //
      // Get output file name and label
      //
      QString outputFileName, outputFileLabel;
      getOutputVolumeFileName(argc,
                              argv,
                              argIndex,
                              outputFileName,
                              outputFileLabel);
      //
      // Read the input file
      //
      VolumeFile volume;
      readVolumeFile(volume, inputFileName);
      
      //
      // Read the mask file
      //
      VolumeFile maskVolume;
      readVolumeFile(maskVolume, maskFileName);
      if (dilationIterations > 0) {
         maskVolume.doVolMorphOps(dilationIterations, 0);
      }
      
      //
      // mask the volume
      //
      volume.maskWithVolume(&maskVolume);
      
      //
      // Write the volume
      //
      writeVolumeFile(volume, outputFileName, outputFileLabel);
   }
   else if (argc == 10) {
      //
      // Get the parameters
      //
      int extent[6];
      getExtentFromParameters(argc, argv, argIndex, extent);
      QString inputName, outputName, outputLabel;
      getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
      
      //
      // Read the input file
      //
      VolumeFile volume;
      readVolumeFile(volume, inputName);
      
      //
      // mask the volume
      //
      volume.maskVolume(extent);
      
      //
      // Write the volume
      //
      writeVolumeFile(volume, outputName, outputLabel);
   }
   else {
      std::cout << "ERROR: The number of parameters appears to be incorrect." << std::endl;
      std::exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Create a mask volume by combining a group of segmentation volumes.
 */
static int
operationVolumeSegmentMaskCreation(int argc, char* argv[], int argIndex)
{
   //
   // Print help information only ?
   //
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SEGMENTATION MASK CREATION", "-volume-segment-mask-creation");
         return 0;
      }
      printSeparatorLine();
      std::cout 
         << "   VOLUME SEGMENTATION MASK CREATION" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-segment-mask-creation <i-dilation-iterations> \\" << std::endl
         << "                <output-volume-name> <one-or-more-input-segmentation-volumes> " << std::endl
         << "" << std::endl
         << "         Create a segmentation mask volume by merging a group of " << std::endl
         << "         segmentation volumes.  The merging is done using stereotaxic" << std::endl
         << "         coordinates so the input volumes may be different dimensions." << std::endl
         << "         The output volume will have the same dimensions and " << std::endl
         << "         stereotaxic coordinates as the first input volume file." << std::endl
         << "         If you want the output volume to have specific dimensions" << std::endl
         << "         or coordinates, use this program with the option" << std::endl
         << "         \"-volume-create\" to create an empty input volume and" << std::endl
         << "         specify this empty volume as the first input volume file." << std::endl
         << "" << std::endl
         << "         Dilation is optional so use a value of zero if you do not" << std::endl
         << "         want the mask dilated after merging all of the input volume" << std::endl
         << "         files." << std::endl
         << "" << std::endl
         << "         Apply a mask using this program with the \"-volume-mask-volume\"" << std::endl
         << "         option." << std::endl
         << "" << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int numberOfDialations;
   CommandLineUtilities::getNextParameter("Number Of Dilations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfDialations);
   QString outputFileName, outputFileLabel;
   getOutputVolumeFileName(argc,
                           argv,
                           argIndex,
                           outputFileName,
                           outputFileLabel);
                           
   std::vector<QString> inputFileNames;
   for (int i = (argIndex + 1); i < argc; i++) {
      QString name;
      CommandLineUtilities::getNextParameter("Input File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             name);
      inputFileNames.push_back(name);
   }

   try {
      VolumeFile::createSegmentationMask(outputFileName,
                                         inputFileNames,
                                         numberOfDialations);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Perform segmentation of a volume in a specific stereotaxic space.
 */
static int
operationVolumeSegmentationStereotaxicSpace(int argc, char* argv[], int argIndex)
{
   bool errorCorrectVolumeFlag = false;
   bool errorCorrectSurfaceFlag = false;
   bool maxPolygonsFlag = false;
   bool flatteningFlag = false;
   int uniformityIterations = BrainModelVolumeSegmentationStereotaxic::getDefaultUniformityIterations();

   //
   // Read in segmentation masks list file
   //
   const QString maskVolumesDirectory(BrainSet::getCaretHomeDirectory()
                                      + "/data_files/segmentation_masks/");
   const QString maskVolumeListFileName(maskVolumesDirectory
                                        + "mask_list.txt.csv");
   SegmentationMaskListFile maskVolumeListFile;
   try {
      maskVolumeListFile.readFile(maskVolumeListFileName);
   }
   catch (FileException& e) {
      // don't throw so help can be printed
   }
   
   //
   // Print help information only ?
   //
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SEGMENTATION STEREOTAXIC SPACE", "-volume-segment-stereo-space");
         return 0;
      }
      printSeparatorLine();
      std::cout 
         << "   VOLUME SEGMENTATION STEREOTAXIC SPACE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-segment-stereo-space \\" << std::endl
         << "         [-ecv] \\" << std::endl
         << "         [-ecs] \\" << std::endl
         << "         [-flat] \\" << std::endl
         << "         [-mp] \\" << std::endl
         << "         [-noeye] \\" << std::endl
         //<< "         [-uniform iterations] \\" << std::endl
         << "         <spec-file-name> \\" << std::endl
         << "         <anatomical-volume-file-name> " << std::endl
         << "" << std::endl
         << "      THIS IS EXPERIMENTAL BUT IT WILL PRODUCE A SURFACE" << std::endl
         << "" << std::endl
         << "      The input spec file must contain the stereotaxic space and the " << std::endl
         << "      structure (LEFT or RIGHT) for this command to operate.  A spec" << std::endl
         << "      is created by running this program with the \"-spec-file-create\"" << std::endl
         << "      command." << std::endl
         << "" << std::endl
         << "      This input volume must be in one of the supported stereotaxic" << std::endl
         << "      spaces with the origin at the anterior commissure.  The volume" << std::endl
         << "      should not have non-uniformity (bias) problems.  The input" << std::endl
         << "      volume is cropped to a single hemisphere, the voxels are" << std::endl
         << "      mapped to the range 0 to 255, the gray and white matter" << std::endl
         << "      histogram peaks are estimated, the volume is segmented, and" << std::endl
         << "      fiducial and inflated surfaces are generated." << std::endl
         << "" << std::endl
         << "      Stereotaxic Spaces Supported" << std::endl;

      std::cout << maskVolumeListFile.getAvailableMasks("         ").toAscii().constData();

      std::cout 
         << "      " << std::endl
         << "      OPTIONS" << std::endl
         << "         -ecv   Perform error correction of the segmentation volume." << std::endl
         << "      " << std::endl
         << "         -ecs   Perform error correction of the surface." << std::endl
         << "      " << std::endl
         << "         -flat  Generate files for flattening (very inflated, ellipsoidal," << std::endl
         << "                paint, and surface shape." << std::endl
         << "      " << std::endl
         << "         -mp    Generate surface with maxmimum number of polygons." << std::endl
         << "      " << std::endl
         << "         -noeye  Do NOT disconnect eye and strip skull" << std::endl
         << "      " << std::endl
         //<< "         -uniform iterations    Perform the specified number of iterations" << std::endl
         //<< "                during non-uniformity correction.  The default number of" << std::endl
         //<< "                iterations is " << BrainModelVolumeSegmentationStereotaxic::getDefaultUniformityIterations() << "." << std::endl
         << "      " << std::endl
         << "" << std::endl;
      return 0;
   }
   
   bool disconnectEyeAndSkull = true;
   
   argIndex++;
   while (argIndex < argc) {
      const QString arg(argv[argIndex]);
      if (arg.isEmpty()) {
         argIndex--;
         break;
      }
      if (arg[0] != '-') {
         argIndex--;
         break;
      }
      
      if (arg == "-ecv") {
         errorCorrectVolumeFlag = true;
      }
      else if (arg == "-ecs") {
         errorCorrectSurfaceFlag = true;
      }
      else if (arg == "-flat") {
         flatteningFlag = true;
      }
      else if (arg == "-mp") {
         maxPolygonsFlag = true;
      }
      else if (arg == "-noeye") {
         disconnectEyeAndSkull = false;
      }
      else if (arg == "-uniform") {
         CommandLineUtilities::getNextParameter("Uniformity Iterations",
                                                argc,
                                                argv,
                                                true,
                                                argIndex,
                                                uniformityIterations);
      }
      else {
         std::cout << "ERROR: Invalid argument encountered: "
                   << arg.toAscii().constData() << std::endl;
         std::exit(-1);
      }
      
      argIndex++;
   }
   
   //
   // Get the parameters 
   //
   QString specFileName;
   CommandLineUtilities::getNextParameter("Spec File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);
   QString inputVolumeName;
   CommandLineUtilities::getNextParameter("Anatomy  Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputVolumeName);
                                          
   
   //
   // Setup spec file with anatomy volume file name and params file
   //
   SpecFile specFile;
   try {
      specFile.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR, Unable to read spec file: "
                << e.whatQString().toAscii().constData()
                << std::endl;
      std::exit(-1);
   }

   //
   // Clear surface files from spec file and disk
   //
   specFile.clearFiles(false, // volume files
                       true,  // surface files,
                       false, // other files
                       true); // remove the files
                       

   //
   // Select anatomy and params
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::volumeAnatomyFileTag,
                          inputVolumeName,
                          "",
                          false);
   if (specFile.paramsFile.getNumberOfFiles() > 0) {
      specFile.paramsFile.setSelected(specFile.paramsFile.getFileName(0),
                                      true,
                                      Structure::STRUCTURE_TYPE_CORTEX_BOTH);
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(true); 
   QString specReadMsg;
   brainSet.readSpecFile(specFile,
                         specFileName,
                         specReadMsg);
   if (specReadMsg.isEmpty() == false) {
      std::cout << "ERROR, Reading spec file: "
                << specReadMsg.toAscii().constData() << std::endl;
      std::exit(-1);
   }

   //
   // Perform the segmentation operations
   //
   BrainModelVolumeSegmentationStereotaxic segment(&brainSet,
                                                   brainSet.getVolumeAnatomyFile(0),
                                                   uniformityIterations,
                                                   disconnectEyeAndSkull,
                                                   errorCorrectVolumeFlag,
                                                   errorCorrectSurfaceFlag,
                                                   maxPolygonsFlag,
                                                   flatteningFlag);

   //
   // Execute the segmentation
   //
   try {
      segment.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << std::endl;
      std::cout << "ERROR: running segmentation operation." << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::cout << std::endl;
   }

   return 0;
}

/*----------------------------------------------------------------------------------------
 * Perform segmentation operations a volume.
 */
static int
operationVolumeSegmentation(int argc, char* argv[], int argIndex)
{
   //
   // Print help information only ?
   //
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SEGMENTATION", "-volume-segment");
         return 0;
      }
      printSeparatorLine();
      std::cout 
         << "   VOLUME SEGMENTATION" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-segment <operation_code> \\" << std::endl
         << "                <f-gray-peak> <f-white-peak> \\" << std::endl
         << "                <padding-code> <structure> <spec-file-name> " << std::endl
         << "                <anat-or-segment-volume> [write-volume-type]" << std::endl
         << "" << std::endl
         << "      Operation_Code characters" << std::endl
         << "         Specify each with either a \"Y\" or \"N\"." << std::endl
         << "         All characters must be specified." << std::endl
         << "         Character   Operation Description" << std::endl
         << "         ---------   ---------------------" << std::endl
         << "                 1   Disconnect Eye and Skull" << std::endl
         << "                 2   Disconnect Hindbrain" << std::endl
         << "                 3   Use High Threshold for Hindbrain disconnection" << std::endl
         << "                 4   Cut Corpus Callossum" << std::endl
         << "                 5   Generate Segmentation" << std::endl
         << "                 6   Fill Ventricles" << std::endl
         << "                 7   Automatic Error Correction" << std::endl
         << "                 8   Generate Raw and Fiducial Surfaces" << std::endl
         << "                 9   Reduce polygons in surfaces" << std::endl
         << "                10   Correct topological errors in surfaces" << std::endl
         << "                11   Generate Inflated Surface" << std::endl
         << "                12   Generate Very Inflated Surface" << std::endl
         << "                13   Generate Ellipsoid Surface (For Flattening)" << std::endl
         << "                14   Generate Hull Surface " << std::endl
         << "                15   Generate Curvature, Depth, and Paint Attributes" << std::endl
         << "" << std::endl
         << "      gray-peak  specifies the intensity of the gray matter peak in the " << std::endl
         << "                 anatomy volume." << std::endl
         << "" << std::endl
         << "      white-peak  specifies the intensity of the white matter peak in the" << std::endl
         << "                  anatomy volume." << std::endl
         << "" << std::endl
         << "      padding-code" << std::endl
         << "         Specify padding for any cut faces when segmenting a partial hemisphere." << std::endl
         << "         Specify each with either a \"Y\" for padding or \"N\" for no padding." << std::endl
         << "         All characters must be specified." << std::endl   
         << "         Character   Padding Description" << std::endl
         << "         ---------   -------------------" << std::endl
         << "                 1   Pad Negative X" << std::endl
         << "                 2   Pad Positive X" << std::endl
         << "                 3   Pad Posterior Y" << std::endl
         << "                 4   Pad Anterior Y" << std::endl
         << "                 5   Pad Inferior Z" << std::endl
         << "                 6   Pad Superior Z" << std::endl
         << "" << std::endl
         << "      structure  Specifies the brain structure." << std::endl
         << "                  Acceptable values are RIGHT or LEFT" << std::endl
         << "" << std::endl
         << "      spec-file-name  Name of specification file." << std::endl
         << "" << std::endl
         << "      anat-or-segment-volume  Name of input volume." << std::endl
         << "         The volume must be in a Left-Posterior-Inferior orientation and" << std::endl
         << "         its stereotaxic coordinates must be set so that the origin is " << std::endl
         << "         at the anterior commissure." << std::endl
         << "" << std::endl
         << "      write-volume-type   Type of volume files to write." << std::endl
         << "         Specifies the type of the volume files that will be written" << std::endl
         << "         during the segmentation process.  Valid values are" << std::endl
         << "            AFNI" << std::endl
         << "            NIFTI (default)" << std::endl
         << "            SPM" << std::endl
         << "            WUNIL" << std::endl
         << "" << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString operationCode;
   CommandLineUtilities::getNextParameter("Operation Code",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          operationCode);
   float grayPeak;
   CommandLineUtilities::getNextParameter("Gray Peak",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          grayPeak);
   float whitePeak;
   CommandLineUtilities::getNextParameter("White Peak",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          whitePeak);
   QString paddingCode;
   CommandLineUtilities::getNextParameter("Padding Code",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paddingCode);
   QString structureName;
   CommandLineUtilities::getNextParameter("Structure Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          structureName);
   QString specFileName;
   CommandLineUtilities::getNextParameter("Spec File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);
   QString inputVolumeName;
   CommandLineUtilities::getNextParameter("Anatomy or Segmentation Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputVolumeName);
                                          
   VolumeFile::FILE_READ_WRITE_TYPE writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
   if (argc >= 10) {
      QString volumeFileTypeName;
      CommandLineUtilities::getNextParameter("Write Volume File Type",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             volumeFileTypeName);
      if (volumeFileTypeName == "AFNI") {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
      }
      else if (volumeFileTypeName == "NIFTI") {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
      }
      else if (volumeFileTypeName == "SPM") {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX;
      }
      else if (volumeFileTypeName == "WUNIL") {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_WUNIL;
      }
      else  {
         std::cout << "ERROR: Invalid volume file write type: " 
                   << volumeFileTypeName.toAscii().constData() << std::endl;
         exit(-1);
      }
   }
   else {
      if (inputVolumeName.endsWith(SpecFile::getAfniVolumeFileExtension())) {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
      }
      else if (inputVolumeName.endsWith(SpecFile::getNiftiVolumeFileExtension()) ||
               inputVolumeName.endsWith(SpecFile::getNiftiGzipVolumeFileExtension())) {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
      }
      else if (inputVolumeName.endsWith(SpecFile::getWustlVolumeFileExtension())) {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_WUNIL;
      }
   }
                                          
   //
   // Check the operation code
   //
   const int operationCodeLength = 15;
   if (operationCode.length() != operationCodeLength) {
      std::cout << "ERROR: Operation code must have exactly "
                << operationCodeLength
                << " characters." << std::endl;
      exit(-1);
   }
   for (int i = 0; i < operationCodeLength; i++) {
      if ((operationCode[i] != 'Y') && (operationCode[i] != 'N')) {
         std::cout << "ERROR: Operation code character "
                   << (i + 1)
                   << " must be \"Y\" or \"N\"." << std::endl;
         exit(-1);
      }
   }
   
   const bool disconnectEyeFlag          = (operationCode[0] == 'Y');
   const bool disconnectHindbrainFlag    = (operationCode[1] == 'Y');
   const bool disconnectHindHiThreshFlag = (operationCode[2] == 'Y');
   const bool cutCorpusCallossumFlag     = (operationCode[3] == 'Y');
   const bool generateSegmentationFlag   = (operationCode[4] == 'Y');
   const bool fillVentriclesFlag         = (operationCode[5] == 'Y');
   const bool errorCorrectionFlag        = (operationCode[6] == 'Y');
   const bool rawFiducialSurfaceFlag     = (operationCode[7] == 'Y');
   const bool reduceSurfacePolygonsFlag  = (operationCode[8] == 'Y');
   const bool topologicalCorrectFlag     = (operationCode[9] == 'Y');
   const bool inflatedSurfaceFlag        = (operationCode[10] == 'Y');
   const bool veryInflatedSurfaceFlag    = (operationCode[11] == 'Y');
   const bool ellipsoidSurfaceFlag       = (operationCode[12] == 'Y');
   const bool hullSurfaceFlag            = (operationCode[13] == 'Y');
   const bool attributesFlag             = (operationCode[14] == 'Y');
   
   //
   // Get the structure
   //
   Structure::STRUCTURE_TYPE structure;
   structureName = StringUtilities::makeUpperCase(structureName);
   if (structureName == "RIGHT") {
      structure = Structure::STRUCTURE_TYPE_CORTEX_RIGHT;
   }
   else if (structureName == "LEFT") {
      structure = Structure::STRUCTURE_TYPE_CORTEX_LEFT;
   }
   else {
      std::cout << "ERROR: Structure must be either RIGHT or LEFT." << std::endl;
      std::cout << "       Value entered \"" << structureName.toAscii().constData() << "\"" << std::endl;
      exit(-1);
   }
   
   //
   // Determine padding
   //
   const int paddingCodeLength = 6;
   if (paddingCode.length() != paddingCodeLength) {
      std::cout << "ERROR: Padding code must have exactly "
                << paddingCodeLength
                << " characters." << std::endl;
      exit(-1);
   }
   int paddingAmount[6] = { 0, 0, 0, 0, 0, 0 };
   for (int i = 0; i < paddingCodeLength; i++) {
      if (paddingCode[i] == 'Y') {
         paddingAmount[i] = 30;
      }
      else if (paddingCode[i] == 'N') {
         paddingAmount[i] = 0;
      }
      else {
         std::cout << "ERROR: Padding code character "
                   << (i + 1)
                   << " must be \"Y\" or \"N\"." << std::endl;
         exit(-1);
      }
   }
   
   //
   // Create a brain set
   //
   BrainSet brainSet(specFileName, false, true);

   //
   // Read the volume file
   //
   VolumeFile inputVolume;
   readVolumeFile(inputVolume, inputVolumeName);
   
   //
   // Determine the AC.
   //
   const float zeros[3] = { 0.0, 0.0, 0.0 };
   int acIJK[3];
   inputVolume.convertCoordinatesToVoxelIJK(zeros, acIJK);
   
   //
   // Create the segmentation object
   //
   BrainModelVolumeSureFitSegmentation 
      segmentationObject(&brainSet,
                         &inputVolume,
                         &inputVolume,
                         writeVolumeType,
                         acIJK,
                         paddingAmount,
                         whitePeak,
                         grayPeak,
                         0.0,
                         structure,
                         disconnectEyeFlag,
                         disconnectHindbrainFlag,
                         disconnectHindHiThreshFlag,
                         cutCorpusCallossumFlag,
                         generateSegmentationFlag,
                         fillVentriclesFlag,
                         errorCorrectionFlag,
                         rawFiducialSurfaceFlag,
                         (reduceSurfacePolygonsFlag == false),
                         topologicalCorrectFlag,
                         inflatedSurfaceFlag,
                         veryInflatedSurfaceFlag,
                         ellipsoidSurfaceFlag,
                         hullSurfaceFlag,
                         attributesFlag,
                         true);
   
   //
   // Execute the segmentation
   //
   try {
      segmentationObject.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << std::endl;
      std::cout << "ERROR: running segmentation operation." << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::cout << std::endl;
   }
   
   const QString warningMessages = segmentationObject.getWarningMessages();
   if (warningMessages.isEmpty() == false) {
      std::cout << "Segmentation Warnings: " << warningMessages.toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Flood fill a volume.
 */
static int
operationVolumeFloodFill(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME FLOOD FILL", "-volume-flood-fill");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME FLOOD FILL" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-flood-fill \\ " << std::endl
         << "         <i-seed-x> <i-seed-y> <i-seed-z> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Flood fill the object starting at seed x/y/z.  All " << std::endl
         << "      voxels not connected to seed are set to zero." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int seed[3];
   getSeedFromParameters(argc, argv, argIndex, seed);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // flood fill
   //
   volume.floodFillWithVTK(seed, 255, 255, 0);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Sculpt a volume.
 */
static int
operationVolumeSculpt(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SCULPT", "-volume-sculpt");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME SCULPT" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-sculpt \\ " << std::endl
         << "         <i-min-x> <i-max-x> <i-min-y> <i-max-y> <i-min-z> <i-max-z> \\" << std::endl
         << "         <i-seed-x> <i-seed-y> <i-seed-z> \\" << std::endl
         << "         <mode> <i-num-steps> <other-volume> \\ " << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << std::endl
         << "      Sculpt a volume." << std::endl
         << "      \"mode\" is one of:" << std::endl
         << "          AND" << std::endl
         << "          SEED-AND" << std::endl
         << "          AND-NOT" << std::endl
         << "          SEED-AND-NOT" << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int extent[6];
   getExtentFromParameters(argc, argv, argIndex, extent);
   int seed[3];
   getSeedFromParameters(argc, argv, argIndex, seed);
   QString modeString, otherVolumeName;
   int numSteps;
   CommandLineUtilities::getNextParameter("Sculpt Mode",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          modeString);
   CommandLineUtilities::getNextParameter("Number of Steps",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numSteps);
   CommandLineUtilities::getNextParameter("Other Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          otherVolumeName);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   modeString = StringUtilities::makeUpperCase(modeString);
   VolumeFile::SCULPT_MODE mode;
   if (modeString == "AND") {
      mode = VolumeFile::SCULPT_MODE_AND;
   }
   else if (modeString == "SEED-AND") {
      mode = VolumeFile::SCULPT_MODE_SEED_AND;
   }
   else if (modeString == "AND-NOT") {
      mode = VolumeFile::SCULPT_MODE_AND_NOT;
   }
   else if (modeString == "SEED-AND-NOT") {
      mode = VolumeFile::SCULPT_MODE_SEED_AND_NOT;
   }
   else {
      std::cout << "ERROR: Invalid sculpt mode: " << modeString.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // Read the other volume
   //
   VolumeFile otherVolume;
   readVolumeFile(otherVolume, otherVolumeName);
   
   //
   // sculpt
   //
   volume.sculptVolume(mode, &otherVolume, numSteps, seed, extent);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Smear along an axis.
 */
static int
operationVolumeSmearAxis(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SMEAR AXIS", "-volume-smear-axis");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME SMEAR AXIS" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-smear-axis \\ " << std::endl
         << "         <axis> <i-mag> <i-sign> <i-core> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Smear along an axis." << std::endl
         << "      \"axis\" is one of \"X\", \"Y\", or \"Z\"." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   const VolumeFile::VOLUME_AXIS axis = getAxisFromParameters(argc, argv, argIndex);
   int mag, sign, core;
   CommandLineUtilities::getNextParameter("Mag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          mag);
   CommandLineUtilities::getNextParameter("Sign",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sign);
   CommandLineUtilities::getNextParameter("Core",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          core);
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // smear
   //
   volume.smearAxis(axis, mag, sign, core);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Shift along an axis.
 */
static int
operationVolumeShiftAxis(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SHIFT AXIS", "-volume-shift-axis");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME SHIFT AXIS" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-shift-axis \\ " << std::endl
         << "         <axis> <i-offset> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Shift along an axis." << std::endl
         << "      \"axis\" is one of \"X\", \"Y\", or \"Z\"." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   const VolumeFile::VOLUME_AXIS axis = getAxisFromParameters(argc, argv, argIndex);
   int offset;
   CommandLineUtilities::getNextParameter("Offset",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          offset);
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // shift
   //
   volume.shiftAxis(axis, offset);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Make a shell.
 */
static int
operationVolumeMakeShell(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME MAKE SHELL", "-volume-make-shell");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME MAKE SHELL" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-make-shell \\ " << std::endl
         << "         <i-num-dilation> <i-num-erosion> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Make shell." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int numDilation, numErosion;
   CommandLineUtilities::getNextParameter("Num Dilation",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numDilation);
   CommandLineUtilities::getNextParameter("Num Erosion",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numErosion);
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // make shell
   //
   volume.makeShellVolume(numDilation, numErosion);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Make a plane.
 */
static int
operationVolumeMakePlane(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME MAKE PLANE", "-volume-make-plane");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME MAKE PLANE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-make-plane \\ " << std::endl
         << "         <x-slope> <x-offset> <y-slope> <y-offset> <z-slope> <z-offset> \\" << std::endl
         << "         <offset> <thickness> <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Make plane." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int xOff, yOff, zOff, xSlope, ySlope, zSlope, offset, thickness;
   CommandLineUtilities::getNextParameter("X Slope",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          xSlope);
   CommandLineUtilities::getNextParameter("X Offset",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          xOff);
   CommandLineUtilities::getNextParameter("Y Slope",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          ySlope);
   CommandLineUtilities::getNextParameter("Y Offset",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          yOff);
   CommandLineUtilities::getNextParameter("Z Slope",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          zSlope);
   CommandLineUtilities::getNextParameter("Z Offset",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          zOff);
   CommandLineUtilities::getNextParameter("Offset",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          offset);
   CommandLineUtilities::getNextParameter("Thickness",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          thickness);
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // make shell
   //
   volume.makePlane(xSlope, xOff, ySlope, yOff, zSlope, zOff, offset, thickness);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Scale voxels to be in range (0, 255) excluding percentage of minimimum and maximum values
 */
static int
operationVolumeScalePercent0to255(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SCALE VOXELS 0 to 255 WITH PERCENTAGE OF MIN/MAX", "-volume-scale-percent-min-max-255");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME SCALE VOXELS 0 to 255 WITH PERCENTAGE OF MIN/MAX" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-scale-percent-min-max-255 \\ " << std::endl
         << "         <percent-minimum> <percent-maximum> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Scale voxels so that they are in the range 0 to 255 but " << std::endl
         << "      map the first \"percent-minimum\" voxels to 0 and map the" << std::endl
         << "      last \"percent-maximum\" voxels to 255." << std::endl
         << "      " << std::endl
         << "      The \"percent-minimum\" and \"percent-maximum\" values should" << std::endl
         << "      range from 0.0 to 100.0.  Setting the \"percent-minimum\" to " << std::endl
         << "      2.0 and the \"percent-maximum\" to 3.0 will result in the first" << std::endl
         << "      two percent of voxel values being mapped to zero and the last" << std::endl
         << "      three percent of voxel values being mapped to two hundred " << std::endl
         << "      fifty five." << std::endl
         << std::endl;
      return 0;
   }
   
   float percentMinimum;
   CommandLineUtilities::getNextParameter("Percent Minimum",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          percentMinimum);
                                          
   float percentMaximum;
   CommandLineUtilities::getNextParameter("Percent Maximum",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          percentMaximum);
                                          
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // rescale
   //
   volume.stretchVoxelValuesExcludePercentage(percentMinimum, percentMaximum);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Scale voxels to be in range (0, 255).
 */
static int
operationVolumeScale0to255(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SCALE VOXELS 0 to 255", "-volume-scale-255");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME SCALE VOXELS 0 to 255" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-scale-255 \\ " << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Scale voxels so that they are in the range 0 to 255." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // rescale
   //
   volume.stretchVoxelValues();
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Histogram.
 */
static int
operationVolumeHistogram(int argc, char* argv[], int argIndex)
{
   int numBuckets = 64;
   float yMaximum = 500000.0;
    
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME HISTOGRAM", "-volume-histogram");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME HISTOGRAM" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-histogram \\ " << std::endl
         << "         <volume-file-name> [b-list-peaks] [i-number-of-buckets] [i-Y-Maximum]" << std::endl
         << "         " << std::endl
         << "      Display a histogram of the volume in the terminal window." << std::endl
         << "      " << std::endl
         << "      If \"list-peaks\" is true, the numerical estimates of the " << std::endl
         << "      gray and white matter peaks will be listed." << std::endl
         << "      " << std::endl
         << "      if \"number-of-buckets\" is not specified, the histogram will" << std::endl
         << "      contain " << numBuckets << " buckets." << std::endl
         << "      " << std::endl
         << "      If \"Y-Maximum\" is not specified, the maximum Y-value will be" << std::endl
         << "      " << yMaximum << "." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString volumeFileName;
   CommandLineUtilities::getNextParameter("Volume File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          volumeFileName);

   bool listPeaksFlag = false;
   if (argc >= 4) {
      CommandLineUtilities::getNextParameter("List Peaks",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             listPeaksFlag);
   }
   
   if (argc >= 5) {
      CommandLineUtilities::getNextParameter("Number of Buckets",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             numBuckets);
   }
   
   bool yMaxWasSetByUser = false;
   if (argc >= 6) {
      yMaxWasSetByUser = true;
      CommandLineUtilities::getNextParameter("Y-Maximum",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             yMaximum);
   }
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, volumeFileName);
   
   //
   // Determine the histogram
   //
   StatisticHistogram* histo = volume.getHistogram(numBuckets);
   
   //
   // Adjust Y-Max
   //
   float maxBucketValue = 0.0;
   if (yMaxWasSetByUser == false) {
      std::set<float> sortedBucketCounts;
      for (int i = 0; i < numBuckets; i++) {
         float bucketDataValue, bucketCount;
         histo->getDataForBucket(i, bucketDataValue, bucketCount);
         sortedBucketCounts.insert(bucketCount);
      }
      int count = 0;
      for (std::set<float>::reverse_iterator iter = sortedBucketCounts.rbegin();
           iter != sortedBucketCounts.rend();
           iter++) {
         
         if (DebugControl::getDebugOn()) {
            std::cout << "Find Y-Max: " << *iter << std::endl;
         }
         
         if (count >= 2) {
            yMaximum = *iter;
            break;
         }
         
         if (count == 0) {
            maxBucketValue = *iter;
         }
         count++;
      }
   }
   
   //
   // Place histogram into a 2D array
   //
   std::vector<QString> bucketValueStrings;
   const int numRows = 40;
   const int arraySize = numRows * numBuckets;
   char* histoArray = new char[arraySize];
   for (int i = 0; i < arraySize; i++) {
      histoArray[i] = ' ' ;
   } 
   for (int i = 0; i < numBuckets; i++) {
      float bucketDataValue, bucketCount;
      histo->getDataForBucket(i, bucketDataValue, bucketCount);
      const float percentToSet = bucketCount / yMaximum;
      const int numYUsed = std::min(static_cast<int>(numRows * percentToSet), numRows);
      for (int j = 0; j < numYUsed; j++) {
         const int offset = (j * numBuckets) + i;
         histoArray[offset] = '*';
      }
      
      if (DebugControl::getDebugOn()) {
         std::cout << "Bucket, count, percentToSet, numYUsed: " << i << ", " << bucketCount
                   << ", " << percentToSet << ", " << numYUsed << std::endl;
      }
      
      bucketValueStrings.push_back(QString::number(bucketDataValue, 'f', 0));
   }
     
   //
   // Print the array
   //
   int maxChars = -1;
   std::cout << std::endl;
   for (int j = (numRows - 1); j >= 0; j--) {
      for (int i = 0; i < numBuckets; i++) {
         const int offset = (j * numBuckets) + i;
         std::cout << histoArray[offset];
      }
      
      //
      // Print the Y-Value on the right side
      //
      const float rowHeight = yMaximum / numRows;
      const float yValue = rowHeight * (j + 1);
      QString yString = QString::number(yValue, 'f', 1);
      if (maxChars < 0) {
         maxChars = yString.length();
      }
      else {
         yString = yString.rightJustified(maxChars, ' ');
      }
      std::cout << " " << yString.toAscii().constData();
      std::cout << std::endl;
   }
   
   //
   // print value at each column vertically
   //
   int maxDigits = 0;
   for (int i = 0; i < numBuckets; i++) {
      maxDigits = std::max(maxDigits, bucketValueStrings[i].length());
   }
   for (int i = 0; i < numBuckets; i++) {
      bucketValueStrings[i] = bucketValueStrings[i].rightJustified(maxDigits, '0');
   }
   for (int i = 0; i < maxDigits; i++) {
      for (int j = 0; j < numBuckets; j++) {
         const QString s(bucketValueStrings[j]);
         const char c = s[i].toAscii();
         std::cout << c;
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;
   
   //
   // Print maximum bucket value
   //
   std::cout << "Maximum Y-Value: " << QString::number(maxBucketValue, 'f', 1).toAscii().constData() << std::endl;
   std::cout << "Max Y-Value Displayed: " << QString::number(yMaximum, 'f', 1).toAscii().constData() << std::endl;
   std::cout << std::endl;
   
   //
   // Print the histogram peaks
   //
   if (listPeaksFlag) {
      printHistogramPeaks(histo);
   }
   
   delete histo;
   histo = NULL;
   delete histoArray;
   histoArray = NULL;
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Gradient.
 */
static int
operationVolumeGradient(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME GRADIENT", "-volume-gradient");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME GRADIENT" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-gradient \\ " << std::endl
         << "         <i-lambda> <b-grad-flag> <b-mask-flag>  \\" << std::endl
         << "         <input-volume-name> <mask-volume-name> <gradient-vector-file-name>" << std::endl
         << "         " << std::endl
         << "      Compute a gradient." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int lambda;
   CommandLineUtilities::getNextParameter("Lambda",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          lambda);
   bool gradFlag;
   CommandLineUtilities::getNextParameter("Grad-Flag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          gradFlag);
   bool maskFlag;
   CommandLineUtilities::getNextParameter("Mask-Flag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          maskFlag);
   
   QString inputName, maskVolumeName;
   CommandLineUtilities::getNextParameter("Input Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputName);
   CommandLineUtilities::getNextParameter("Mask Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          maskVolumeName);
   QString gradVecName;
   CommandLineUtilities::getNextParameter("Gradent Vector File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          gradVecName);
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // Read the mask volume
   //
   VolumeFile maskVolume;
   readVolumeFile(maskVolume, maskVolumeName);
   
   //
   // Gradient vector file
   //
   int xDim, yDim, zDim;
   volume.getDimensions(xDim, yDim, zDim);
   VectorFile gradFile(xDim, yDim, zDim);
   
   //
   // determine gradient
   //
   BrainSet bs;
   BrainModelVolumeGradient grad(&bs,
                                 lambda,
                                 gradFlag,
                                 maskFlag,
                                 &volume,
                                 &maskVolume,
                                 &gradFile);
   try {
      grad.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << "Error determing gradient. " << std::endl;
      std::cout << "   " << e.whatQString().toAscii().constData() << std::endl;
   }
   
   //
   // Write the gradient file
   //
   writeVectorFile(gradFile, gradVecName);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Near To Plane.
 */
static int
operationVolumeNearToPlane(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME NEAR TO PLANE", "-volume-near-to-plane");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME NEAR TO PLANE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-near-to-plane \\ " << std::endl
         << "         <f-sigma-N> <f-sigma-W> <f-offset> <b-down-flag> <i-grad-sign> \\" << std::endl
         << "         <b-masking-flag> <vector-file-name> \\" << std::endl
         << "         <mask-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Near to plane." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float sigmaN;
   CommandLineUtilities::getNextParameter("Sigma-N",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sigmaN);
   float sigmaW;
   CommandLineUtilities::getNextParameter("Sigma-W",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sigmaW);
   float offset;
   CommandLineUtilities::getNextParameter("Offset",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          offset);
   bool downFlag;
   CommandLineUtilities::getNextParameter("Down-Flag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          downFlag);
   int gradSign;
   CommandLineUtilities::getNextParameter("Grad-Sign",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          gradSign);
   bool maskFlag;
   CommandLineUtilities::getNextParameter("Mask-Flag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          maskFlag);
   QString vectorFileName;
   CommandLineUtilities::getNextParameter("Vector File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          vectorFileName);
   QString maskVolumeName;
   CommandLineUtilities::getNextParameter("Mask Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          maskVolumeName);
   QString outputVolumeName;
   CommandLineUtilities::getNextParameter("Output Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputVolumeName);
   QString outputLabel;
   splitOutputNameIntoFileNameAndLabel(outputVolumeName, outputLabel);
   
   //
   // Read the mask volume
   //
   VolumeFile maskVolume;
   readVolumeFile(maskVolume, maskVolumeName);
   
   //
   // Read the vector file
   //
   VectorFile vectorFile;
   readVectorFile(vectorFile, vectorFileName);
   
   VolumeFile outputVolume = maskVolume;
   
   //
   // perform near to plane
   //
   BrainSet bs;
   BrainModelVolumeNearToPlane np(&bs,
                                 &vectorFile,
                                 sigmaN,
                                 sigmaW,
                                 offset,
                                 downFlag,
                                 gradSign,
                                 maskFlag,
                                 &maskVolume,
                                 &outputVolume);
   try {
      np.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << "Error running near to plane. " << std::endl;
      std::cout << "   " << e.whatQString().toAscii().constData() << std::endl;
   }
   
   //
   // Write the volume file
   //
   writeVolumeFile(outputVolume, outputVolumeName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Classify Intensities.
 */
static int
operationVolumeClassifyIntensities(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME CLASSIFY INTENSITIES", "-volume-classify-intensity");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME CLASSIFY INTENSITIES" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-classify-intensity \\ " << std::endl
         << "         <f-mean> <f-low> <f-high> <f-signum> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Classify intensities." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float mean, low, high, signum;
   CommandLineUtilities::getNextParameter("Mean",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          mean);
   CommandLineUtilities::getNextParameter("Low",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          low);
   CommandLineUtilities::getNextParameter("High",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          high);
   CommandLineUtilities::getNextParameter("Signum",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          signum);
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // classify
   //
   volume.classifyIntensities(mean, low, high, signum);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Remove islands.
 */
static int
operationVolumeRemoveIslands(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME REMOVE ISLANDS", "-volume-remove-islands");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME REMOVE ISLANDS" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-remove-islands <input-volume-name> \\" << std::endl
         << "       <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Remove islands from a segmentation volume." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // remove the islands
   //
   volume.removeIslandsFromSegmentation();
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Pad a volume.
 */
static int
operationVolumePadVolume(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME PAD A VOLUME", "-volume-pad-volume");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME PAD A VOLUME" << std::endl
         << "      " << programName.toAscii().constData()<< " -volume-pad-volume <i-pad-neg-x> <i-pad-pos-x> <i-pad-neg-y> \\" << std::endl
         << "      <i-pad-pos-y> <i-pad-neg-z> <i-pad-pos-z> \\" << std::endl
         << "      <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Add padding around a volume." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int pad[6];
   CommandLineUtilities::getNextParameter("Neg-X Padding",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pad[0]);
   CommandLineUtilities::getNextParameter("Pos-X Padding",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pad[1]);
   CommandLineUtilities::getNextParameter("Neg-Y Padding",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pad[2]);
   CommandLineUtilities::getNextParameter("Pos-Y Padding",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pad[3]);
   CommandLineUtilities::getNextParameter("Neg-Z Padding",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pad[4]);
   CommandLineUtilities::getNextParameter("Pos-Z Padding",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pad[5]);
   //
   // Get the parameters
   //
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // pad the volume
   //
   volume.padSegmentation(pad);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Make a rectangle.
 */
static int
operationVolumeMakeRectangle(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME MAKE RECTANGLE", "-volume-make-rectangle");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME MAKE RECTANGLE" << std::endl
         << "      " << programName.toAscii().constData()<< " -volume-make-rectangle \\ " << std::endl
         << "         <i-min> <i-max> <j-min> <j-max> <k-min> <k-max> <f-voxel-value>\\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Set the voxels within the range [i-min, i-max), ." << std::endl
         << "      [j-min, j-max), [k-min, k-max) to the specified value." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int extent[6];
   getExtentFromParameters(argc, argv, argIndex, extent);
   
   float voxelValue;
   CommandLineUtilities::getNextParameter("Voxel Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          voxelValue);
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // set the voxels
   //
   volume.setAllVoxelsInRectangle(extent, voxelValue);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Make a sphere.
 */
static int
operationVolumeMakeSphere(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME MAKE SPHERE", "-volume-make-sphere");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME MAKE SPHERE" << std::endl
         << "      " << programName.toAscii().constData()<< " -volume-make-sphere \\ " << std::endl
         << "         <i-center-x> <i-center-y> <i-center-z> <f-radius> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Make a sphere within the volume." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int center[3];
   CommandLineUtilities::getNextParameter("Center X",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          center[0]);
   CommandLineUtilities::getNextParameter("Center Y",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          center[1]);
   CommandLineUtilities::getNextParameter("Center Z",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          center[2]);
   float radius;
   CommandLineUtilities::getNextParameter("Radius",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          radius);
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // classify
   //
   volume.makeSphere(center, radius);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Fill Holes (cavities) in a segmentation volume.
 */
static int
operationVolumeFillHoles(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME FILL HOLES", "-volume-fill-holes");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME FILL HOLES" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-fill-holes \\ " << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Fill holes (cavities) in a segmentation volume." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // fill the holes
   //
   volume.fillSegmentationCavities();
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Fill Slice in a segmentation volume.
 */
static int
operationVolumeFillSlice(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME FILL SLICE", "-volume-fill-slice");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME FILL SLICE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-fill-slice \\ " << std::endl
         << "         <axis> <i-seed-x> <i-seed-y> <i-seed-z>" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Flood fill a single slice." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   const VolumeFile::VOLUME_AXIS axis = getAxisFromParameters(argc, argv, argIndex);
   
   int seed[3];
   getSeedFromParameters(argc, argv, argIndex, seed);
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // fill the holes
   //
   volume.floodFillSliceWithVTK(axis, seed, 255, 255, 0);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Combine vector files.
 */
static int
operationVolumeVectorCombine(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME VECTOR FILES COMBINE", "-volume-vector-combine");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME VECTOR FILES COMBINE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-vector-combine \\ " << std::endl
         << "         <operation> <b-mask-flag> <mask-volume-name>   \\" << std::endl
         << "         <input-vector-1-name> <input-vector-2-name> <output-vector-name>" << std::endl
         << "         " << std::endl
         << "      Combine vector files." << std::endl
         << "         \"operation\" is one of:" << std::endl
         << "            DOT_SQRT_RECT_MINUS" << std::endl
         << "            2_VEC_NORMAL" << std::endl
         << "            2_VEC" << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString operationString;
   CommandLineUtilities::getNextParameter("Operation",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          operationString);
   bool maskFlag;
   CommandLineUtilities::getNextParameter("Mask Flag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          maskFlag);
   QString maskVolumeName;
   CommandLineUtilities::getNextParameter("Mask Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          maskVolumeName);
   QString vector1Name;
   CommandLineUtilities::getNextParameter("Vector File 1",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          vector1Name);
   QString vector2Name;
   CommandLineUtilities::getNextParameter("Vector File 2",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          vector2Name);
   QString outputVectorName;
   CommandLineUtilities::getNextParameter("Output Vector File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputVectorName);
   
   VectorFile::COMBINE_OPERATION operation;
   operationString = StringUtilities::makeUpperCase(operationString);
   if (operationString == "DOT_SQRT_RECT_MINUS") {
      operation = VectorFile::COMBINE_OPERATION_DOT_SQRT_RECT_MINUS;
   }
   else if (operationString == "2_VEC_NORMAL") {
      operation = VectorFile::COMBINE_OPERATION_2_VEC_NORMAL;
   }
   else if (operationString == "2_VEC") {
      operation = VectorFile::COMBINE_OPERATION_2_VEC;
   }
   else {
      std::cout << "Invalid operation \"" << operationString.toAscii().constData() << "\"" << std::endl;
      exit(-1);
   }
   
   //
   // Read the input files
   //
   VectorFile vector1;
   readVectorFile(vector1, vector1Name);
   VectorFile vector2;
   readVectorFile(vector2, vector2Name);
   VolumeFile maskVolume;
   readVolumeFile(maskVolume, maskVolumeName);
   
   //
   // Create the output vector file
   //
   VectorFile outputVector = vector1;
   
   //
   // combine vectors
   //
   try {
      VectorFile::combineVectorFiles(maskFlag,
                                     operation,
                                     &vector1,
                                     &vector2,
                                     &maskVolume,
                                     &outputVector);
   }
   catch (FileException& e) {
      std::cout << "ERROR combining vector files: " << std::endl;
      std::cout << "   " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the vector file
   //
   writeVectorFile(outputVector, outputVectorName);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Replace magnitude with volume's voxels.
 */
static int
operationVolumeReplaceVectorMagnitudeWithVolume(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME VECTOR MAGNITUDE - REPLACE WITH VOLUME", "-volume-vector-replace-mag-volume");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME VECTOR MAGNITUDE - REPLACE WITH VOLUME" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-vector-replace-mag-volume \\ " << std::endl
         << "         <operation> <volume-file>    \\" << std::endl
         << "         <input-vector-name> <output-vector-name>" << std::endl
         << "         " << std::endl
         << "      Replace vector file's magnitude." << std::endl
         << "         \"operation\" is one of:" << std::endl
         << "            REPLACE  - replace magnitude with volume's voxel" << std::endl
         << "            MULTIPLY - multiply magnitude with volume's voxel" << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString operationString;
   CommandLineUtilities::getNextParameter("Operation",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          operationString);
   QString volumeName;
   CommandLineUtilities::getNextParameter("Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          volumeName);
   QString inputVectorName;
   CommandLineUtilities::getNextParameter("Input Vector File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputVectorName);
   QString outputVectorName;
   CommandLineUtilities::getNextParameter("Output Vector File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputVectorName);
   
   VectorFile::COMBINE_VOLUME_OPERATION operation;
   operationString = StringUtilities::makeUpperCase(operationString);
   if (operationString == "REPLACE") {
      operation = VectorFile::COMBINE_VOLUME_REPLACE_MAGNITUDE_WITH_VOLUME;
   }
   else if (operationString == "MULTIPLY") {
      operation = VectorFile::COMBINE_VOLUME_MULTIPLY_MAGNITUDE_WITH_VOLUME;
   }
   else {
      std::cout << "Invalid operation \"" << operationString.toAscii().constData() << "\"" << std::endl;
      exit(-1);
   }
   
   //
   // Read the input files
   //
   VectorFile vector;
   readVectorFile(vector, inputVectorName);
   VolumeFile volume;
   readVolumeFile(volume, volumeName);
   
   //
   // replace magnitude with volume's voxels
   //
   try {
      vector.combineWithVolumeOperation(operation,
                                        &volume);
   }
   catch (FileException& e) {
      std::cout << "ERROR replacing vector magnitude with volume voxels: " << std::endl;
      std::cout << "   " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the vector file
   //
   writeVectorFile(vector, outputVectorName);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * replace volume's voxels with vector magnitude.
 */
static int
operationVolumeReplaceVoxelsWithVectorMagnitude(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME COPY VECTOR FILE MAGNITUDE INTO VOXELS", "-volume-copy-vector-mag");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME COPY VECTOR FILE MAGNITUDE INTO VOXELS" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-copy-vector-mag \\ " << std::endl
         << "         <vector-file> <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Replace the volume's voxels with the vector file's magnitude." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString vectorFileName;
   CommandLineUtilities::getNextParameter("Vector File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          vectorFileName);
   VectorFile vectorFile;
   readVectorFile(vectorFile, vectorFileName);
      
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // replace the voxels with vector magnitude
   //
   try {
      vectorFile.copyMagnitudeToVolume(&volume);
   }
   catch (FileException& e) {
      std::cout << "ERROR replacing volume voxels with vector magnitude: " << std::endl;
      std::cout << "   " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Perform mathematical operations using volumes.
 */
static int
operationVolumeMathematics(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME MATHEMATICAL OPERATIONS ON VOLUMES", "-volume-math");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME MATHEMATICAL OPERATIONS ON VOLUMES" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-math <operation> \\ " << std::endl
         << "         <input-volume-1-name> <input-volume-2-name> \\" << std::endl 
         << "         <input-volume-3-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      If <input-volume-3-name> is only required for the operation" << std::endl
         << "      \"DIFF_RATIO\".  For all other operations, specify its name as \"NULL\"." << std::endl
         << std::endl
         << "      Perform mathematical operations on volumes." << std::endl
         << "         \"operation\" is one of: " << std::endl
         << "            ADD" << std::endl
         << "            AND" << std::endl
         << "            DIFF_RATIO" << std::endl
         << "            DIVIDE" << std::endl
         << "            MAX" << std::endl
         << "            MULTIPLY" << std::endl
         << "            NAND" << std::endl
         << "            NOR" << std::endl
         << "            OR" << std::endl
         << "            PAINT_COMBINE" << std::endl
         << "            SQRT" << std::endl
         << "            SUBTRACT" << std::endl
         << "            SUBTRACT_POSITIVE  (subtract and if result < 0, result = 0)" << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString operationString;
   CommandLineUtilities::getNextParameter("Operation",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          operationString);
                                          
   VolumeFile::VOLUME_MATH_OPERATION operation;
   if (operationString == "ADD") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_ADD;
   }
   else if (operationString == "AND") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_AND;
   }
   else if (operationString == "DIFF_RATIO") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_DIFFRATIO;
   }
   else if (operationString == "DIVIDE") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_DIVIDE;
   }
   else if (operationString == "MAX") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_MAX;
   }
   else if (operationString == "MULTIPLY") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_MULTIPLY;
   }
   else if (operationString == "NAND") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_NAND;
   }
   else if (operationString == "NOR") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_NOR;
   }
   else if (operationString == "OR") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_OR;
   }
   else if (operationString == "PAINT_COMBINE") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_COMBINE_PAINT;
   }
   else if (operationString == "SQRT") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_SQRT;
   }
   else if (operationString == "SUBTRACT") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT;
   }
   else if (operationString == "SUBTRACT_POSITIVE") {
      operation = VolumeFile::VOLUME_MATH_OPERATION_SUBTRACT_POSITIVE;
   }
   else {
      std::cout << "ERROR: Invalid operation \"" << operationString.toAscii().constData() << "\"." << std::endl;
   }

   QString inputVolumeName1;
   CommandLineUtilities::getNextParameter("Input Volume 1",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputVolumeName1);
   QString inputVolumeName2;
   CommandLineUtilities::getNextParameter("Input Volume 2",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputVolumeName2);
   QString inputVolumeName3;
   CommandLineUtilities::getNextParameter("Input Volume 3",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputVolumeName3);
   QString outputVolumeName;
   CommandLineUtilities::getNextParameter("Output Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputVolumeName);
   QString outputLabel;
   splitOutputNameIntoFileNameAndLabel(outputVolumeName, outputLabel);
   
   VolumeFile inputVolume1;
   readVolumeFile(inputVolume1, inputVolumeName1);
   VolumeFile inputVolume2;
   readVolumeFile(inputVolume2, inputVolumeName2);
   VolumeFile inputVolume3;
   if ((inputVolumeName3 != "NULL") && (inputVolumeName3 != "\"\"")) {
      readVolumeFile(inputVolume3, inputVolumeName3);
   }
   VolumeFile outputVolume = inputVolume1;
   
   //
   // perform the mathematical operation
   //
   try {
      VolumeFile::performMathematicalOperation(operation,
                                               &inputVolume1,
                                               &inputVolume2,
                                               &inputVolume3,
                                               &outputVolume);
   }
   catch (FileException& e) {
      std::cout << "ERROR combining volumes: " << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the volume
   //
   writeVolumeFile(outputVolume, outputVolumeName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Perform unary mathematical operations on a volume.
 */
static int
operationVolumeMathematicsUnary(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME MATHEMATICAL OPERATIONS (UNARY) ON A VOLUME", "-volume-math-unary");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME MATHEMATICAL OPERATIONS (UNARY) ON A VOLUME" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-math-unary <operation> <f-scalar> \\ " << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Perform mathematical operations on a volume." << std::endl
         << "         \"operation\" is one of: " << std::endl
         << "            ADD_SCALAR" << std::endl
         << "            ABS_VALUE" << std::endl
         << "            CEILING" << std::endl
         << "            FIX_NOT_A_NUMBER" << std::endl
         << "            FLOOR" << std::endl
         << "            MULTIPLY_SCALAR" << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString operationString;
   CommandLineUtilities::getNextParameter("Operation",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          operationString);
                                          
   AbstractFile::UNARY_OPERATION operation;
   if (operationString == "ADD_SCALAR") {
      operation = AbstractFile::UNARY_OPERATION_ADD;
   }
   else if (operationString == "ABS_VALUE") {
      operation = AbstractFile::UNARY_OPERATION_ABS_VALUE;
   }
   else if (operationString == "CEILING") {
      operation = AbstractFile::UNARY_OPERATION_CEILING;
   }
   else if (operationString == "FIX_NOT_A_NUMBER") {
      operation = AbstractFile::UNARY_OPERATION_FIX_NOT_A_NUMBER;
   }
   else if (operationString == "FLOOR") {
      operation = AbstractFile::UNARY_OPERATION_FLOOR;
   }
   else if (operationString == "MULTIPLY_SCALAR") {
      operation = AbstractFile::UNARY_OPERATION_MULTIPLY;
   }
   else {
      std::cout << "ERROR: Invalid operation \"" << operationString.toAscii().constData() << "\"." << std::endl;
   }

   float scalar;
   CommandLineUtilities::getNextParameter("Scalar",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          scalar);
   
   QString inputVolumeName;
   CommandLineUtilities::getNextParameter("Input Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputVolumeName);
   QString outputVolumeName;
   CommandLineUtilities::getNextParameter("Output Volume",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputVolumeName);
   QString outputLabel;
   splitOutputNameIntoFileNameAndLabel(outputVolumeName, outputLabel);
   
   VolumeFile inputVolume;
   readVolumeFile(inputVolume, inputVolumeName);
   VolumeFile outputVolume = inputVolume;
   
   //
   // replace the voxels with vector magnitude
   //
   try {
      VolumeFile::performUnaryOperation(operation,
                                        &inputVolume,
                                        &outputVolume,
                                        scalar);
   }
   catch (FileException& e) {
      std::cout << "ERROR performing unary operation on volume: " << std::endl;
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the volume
   //
   writeVolumeFile(outputVolume, outputVolumeName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Set the volume's origin.
 */
static int
operationVolumeSetOrigin(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SET ORIGIN", "-volume-set-origin");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME SET ORIGIN" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-set-origin \\ " << std::endl
         << "         <f-origin-x> <f-origin-y> <f-origin-z> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Set the origin for the volume file." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float origin[3];
   CommandLineUtilities::getNextParameter("Origin X",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          origin[0]);
   CommandLineUtilities::getNextParameter("Origin Y",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          origin[1]);
   CommandLineUtilities::getNextParameter("Origin Z",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          origin[2]);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // set the spacing
   //
   volume.setOrigin(origin);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Find the voxel extent.
 */
static int
operationVolumeFindLimits(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME FIND LIMITS", "-volume-find-limits");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME FIND LIMITS" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-find-limits <input-volume-name> <limits-file-name>" << std::endl
         << "         " << std::endl
         << "      Find the limits (non-zero voxel extent) of the volume." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString volumeFileName, limitsFileName;
   CommandLineUtilities::getNextParameter("Input Volume Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          volumeFileName);
   CommandLineUtilities::getNextParameter("Limits File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          limitsFileName);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, volumeFileName);
   
   //
   // set the spacing
   //
   int extent[6];
   volume.findLimits(limitsFileName, extent);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Set the volume's spacing.
 */
static int
operationVolumeSetSpacing(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SET SPACING", "-volume-set-spacing");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME SET SPACING" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-set-spacing \\ " << std::endl
         << "         <f-spacing-x> <f-spacing-y> <f-spacing-z> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         "
         << "      Set the voxel spacing for the volume file." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float spacing[3];
   CommandLineUtilities::getNextParameter("Spacing X",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          spacing[0]);
   CommandLineUtilities::getNextParameter("Spacing Y",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          spacing[1]);
   CommandLineUtilities::getNextParameter("Origin Z",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          spacing[2]);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // set the spacing
   //
   volume.setSpacing(spacing);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Copy the volume.
 */
static int
operationVolumeCopy(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME COPY A VOLUME", "-volume-copy");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME COPY A VOLUME" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-copy <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << std::endl
         << "      Copy a volume.  If the input volume's orientation is valid," << std::endl
         << "      the output volume will be written in LPI orientation." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   try {
      //
      // Read the input file
      //
      std::vector<VolumeFile*> volumes;
      VolumeFile::readFile(inputName,
                           VolumeFile::VOLUME_READ_SELECTION_ALL,
                           volumes);
                           
      if (volumes.empty() == false) {
         //
         // Write the output file
         //
         volumes[0]->setDescriptiveLabel(outputLabel);
         VolumeFile::writeFile(outputName,
                               volumes[0]->getVoxelDataType(),
                               volumes);
                               
         //
         // Free memory
         //
         for (unsigned int i = 0; i < volumes.size(); i++) {
            delete volumes[i];
         }
      }
      else {
         std::cout << "ERROR: no volumes were read successfully." << std::endl;
         std::exit(-1);
      }
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::exit(-1);
   }
   
   return 0;
}
   
/*----------------------------------------------------------------------------------------
 * Convert a vector file to a vector volume file.
 */
static int
operationVolumeConvertVectorToVolume(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME CONVERT A VECTOR FILE TO A VOLUME FILE", "-volume-copy");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME CONVERT A VECTOR FILE TO A VOLUME FILE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-copy <input-vector-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << std::endl
         << "      Convert a vector file into a vector type volume file," << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VectorFile vectorFile;
   readVectorFile(vectorFile, inputName);
   
   //
   // Write the volume
   //
   VolumeFile volume(vectorFile);
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}
   
/*----------------------------------------------------------------------------------------
 * Create the volume.
 */
static int
operationVolumeCreate(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME CREATE", "-volume-create");
         return 0;
      }
      printSeparatorLine();
      std::vector<StereotaxicSpace> spaces;
      StereotaxicSpace::getAllStereotaxicSpaces(spaces);

      std::cout
         << "   VOLUME CREATE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-create \\" << std::endl
         << "         <i-x-dimension> <i-y-dimension> <i-z-dimension> \\" << std::endl
         << "         <output-volume-name>" << std::endl
         << std::endl
         << "      " << programName.toAscii().constData() << " -volume-create \\" << std::endl
         << "         <STEREOTAXIC-SPACE> \\" << std::endl
         << "         <output-volume-name>" << std::endl
         << std::endl
         << "      " << programName.toAscii().constData() << " -volume-create \\" << std::endl
         << "         <i-x-dimension> <i-y-dimension> <i-z-dimension> \\" << std::endl
         << "         <f-x-origin> <f-y-origin> <f-z-origin> \\" << std::endl
         << "         <f-x-spacing> <f-y-spacing> <f-z-spacing> \\" << std::endl
         << "         <ORIENTATION> \\ " << std::endl
         << "         <output-volume-name>" << std::endl
         << std::endl
         << "      Create a volume." << std::endl
         << std::endl
         << "      The volume created will have all voxels set to zero." << std::endl
         << std::endl
         << "      The first form in which only the dimensions are specified will have " << std::endl
         << "      its origin defaulted to zeros, the spacing to ones, and the orienation" << std::endl
         << "      to LPI." << std::endl
         << "" << std::endl
         << "      With the second form, the volume is created in the specified" << std::endl
         << "      stereotaxic space.  Valid spaces are:" << std::endl;
      
      for (unsigned int i = 0; i < spaces.size(); i++) {
         if ((spaces[i].getSpace() != StereotaxicSpace::SPACE_UNKNOWN) &&
             (spaces[i].getSpace() != StereotaxicSpace::SPACE_OTHER)) {
            const QString name = spaces[i].getName();
            std::cout << "            " << name.toAscii().constData() << std::endl;
         }
      }
      
      std::cout
         << "" << std::endl
         << "      The third form creates a volume using the specified parameters." << std::endl
         << "" << std::endl
         << "      \"orientation\" consists of three letters from \"LRPAIS\" where" << std::endl
         << "         L     represents left" << std::endl
         << "         R     represents right" << std::endl
         << "         P     represents posterior" << std::endl
         << "         A     represents anterior" << std::endl
         << "         I     represents inferior" << std::endl
         << "         S     represents superior" << std::endl
         << std::endl;
      return 0;
   }
   
   bool allParamsFlag = false;
   bool spaceParamFlag = false;
   if (argc == 13) {
      allParamsFlag = true;
   }
   else if (argc == 4) {
      spaceParamFlag = true;
   }
   else if (argc != 6) {
      std::cout << "ERROR: wrong number of parameters." << std::endl;
   }
   
   //
   // Get the parameters
   //
   int dimensions[3] = { 0, 0, 0 };
   if (spaceParamFlag == false) {
      CommandLineUtilities::getNextParameter("X Dimension",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             dimensions[0]);
      CommandLineUtilities::getNextParameter("Y Dimension",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             dimensions[1]);
      CommandLineUtilities::getNextParameter("Z Dimension",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             dimensions[2]);
   }
                                          
   float origin[3] = { 0.0, 0.0, 0.0 };
   float spacing[3] = { 1.0, 1.0, 1.0 };
   VolumeFile::ORIENTATION orient[3] = {
      VolumeFile::ORIENTATION_LEFT_TO_RIGHT,
      VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR,
      VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR
   };
   
   if (spaceParamFlag) {
      QString spaceName;
      CommandLineUtilities::getNextParameter("Stereotaxic Space",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             spaceName);
      StereotaxicSpace space = StereotaxicSpace::getStereotaxicSpace(spaceName);
      if ((space.getSpace() == StereotaxicSpace::SPACE_UNKNOWN) ||
          (space.getSpace() == StereotaxicSpace::SPACE_OTHER)) {
         std::cout << "ERROR: Stereotaxic space name not recognized." << std::endl;
         exit(-1);
      }
      
      space.getDimensions(dimensions);
      space.getVoxelSize(spacing);
      space.getOrigin(origin);
      orient[0] = VolumeFile::ORIENTATION_LEFT_TO_RIGHT;
      orient[1] = VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR;
      orient[2] = VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR;
   }
   
   if (allParamsFlag) {
      CommandLineUtilities::getNextParameter("X Origin",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             origin[0]);
      CommandLineUtilities::getNextParameter("Y Origin",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             origin[1]);
      CommandLineUtilities::getNextParameter("Z Origin",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             origin[2]);

      CommandLineUtilities::getNextParameter("X Spacing",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             spacing[0]);
      CommandLineUtilities::getNextParameter("Y Spacing",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             spacing[1]);
      CommandLineUtilities::getNextParameter("Z Spacing",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             spacing[2]);
                                             
      QString orientationString;
      CommandLineUtilities::getNextParameter("Orientation",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             orientationString);
      if (orientationString.length() != 3) {
         std::cout << "Orientation should contain 3 letters." << std::endl;
         exit(-1);
      }
      for (int i = 0; i < 3; i++) {
         switch (orientationString[i].toLatin1()) {
            case 'L':
               orient[i] = VolumeFile::ORIENTATION_LEFT_TO_RIGHT;
               break;
            case 'R':
               orient[i] = VolumeFile::ORIENTATION_RIGHT_TO_LEFT;
               break;
            case 'P':
               orient[i] = VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR;
               break;
            case 'A':
               orient[i] = VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR;
               break;
            case 'I':
               orient[i] = VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR;
               break;
            case 'S':
               orient[i] = VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR;
               break;
            default:
               std::cout << "Invalid orientation character \""
                         << orientationString[i].toLatin1() << "\"" << std::endl;
               exit(-1);
               break;
         }
      }
   }
   
   QString outputVolumeName;
   CommandLineUtilities::getNextParameter("Output Volume Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputVolumeName);
   QString outputLabel;
   splitOutputNameIntoFileNameAndLabel(outputVolumeName, outputLabel);
   
   //
   // Create the volume
   //
   VolumeFile volume;
   volume.initialize(VolumeFile::VOXEL_DATA_TYPE_FLOAT,
                     dimensions,
                     orient,
                     origin,
                     spacing);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputVolumeName, outputLabel);
   
   return 0;
}
   
/*----------------------------------------------------------------------------------------
 * Information  the volume.
 */
static int
operationVolumeInformation(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME INFORMATION", "-volume-info");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME INFORMATION" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-info <volume-file-name>" << std::endl
         << "         " << std::endl
         << "      Print information about the volume file." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString volumeFileName;
   CommandLineUtilities::getNextParameter("New Volume File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          volumeFileName);
   //
   // Read the input file
   //
   VolumeFile volume;
   volume.setVolumeSpace(VolumeFile::VOLUME_SPACE_COORD_LPI);
   readVolumeFile(volume, volumeFileName);
   
   std::cout << "Volume File: " << FileUtilities::basename(volumeFileName).toAscii().constData() << std::endl;
   
   int dim[3];
   volume.getDimensions(dim);
   std::cout << "   dimensions: " << dim[0] << ", " << dim[1] << ", " << dim[2] << ", "
             << volume.getNumberOfComponentsPerVoxel() << std::endl;
   
   float space[3];
   volume.getSpacing(space);
   std::cout << "   spacing: " << space[0] << ", " << space[1] << ", " << space[2] << std::endl;
   
   float org[3];
   volume.getOrigin(org);
   std::cout << "   origin: " << org[0] << ", " << org[1] << ", " << org[2] << std::endl;
   
   VolumeFile::ORIENTATION orient[3];
   volume.getOrientation(orient);
   std::cout << "   orientation: " 
             << VolumeFile::getOrientationLabel(orient[0]).toAscii().constData() << ", "
             << VolumeFile::getOrientationLabel(orient[1]).toAscii().constData() << ", "
             << VolumeFile::getOrientationLabel(orient[2]).toAscii().constData() << std::endl;
            
   std::cout << "   label: " << volume.getDescriptiveLabel().toAscii().constData() << std::endl;
   
   float minValue, maxValue;
   volume.getMinMaxVoxelValues(minValue, maxValue);
   std::cout << "   voxel range: " << minValue << ", " << maxValue << std::endl;
   
   const int numRegionNames = volume.getNumberOfRegionNames();
   if (numRegionNames > 0) {
      std::cout << "   Region Names: " << std::endl;
      for (int i = 0; i < numRegionNames; i++) {
         std::cout << "      " << i << " " << volume.getRegionNameFromIndex(i).toAscii().constData() << std::endl;
      }
   }

   return 0;
}

/*----------------------------------------------------------------------------------------
 * Intersect a volume with a surface and assign paints to surface.
 */
static int
operationVolumeIntersectVolumeWithSurface(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME INTERSECT VOLUME WITH SURFACE", "-volume-intersect-volume-with-surface");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME INTERSECT VOLUME WITH SURFACE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-intersect-volume-with-surface \\ " << std::endl
         << "         <paint-name>  X  Y  Z  <input-volume> <spec-file>" << std::endl
         << "         " << std::endl
         << "      Intersect a volume with a surface and for all nodes that" << std::endl
         << "      intersect a non-zero voxel, assign \"paint-name\" to that node." << std::endl
         << "      A paint file named \"<paint-name>.paint\" is created. " << std::endl
         << "      The surface is offset by the X, Y, Z values." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString paintName, volumeFileName, specFileName;
   CommandLineUtilities::getNextParameter("Paint Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintName);
   CommandLineUtilities::getNextParameter("Volume File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          volumeFileName);
   CommandLineUtilities::getNextParameter("Spec File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);
   float offsetX, offsetY, offsetZ;
   CommandLineUtilities::getNextParameter("Surface Offset X",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          offsetX);
   CommandLineUtilities::getNextParameter("Surface Offset Y",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          offsetY);
   CommandLineUtilities::getNextParameter("Surface Offset Z",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          offsetZ);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, volumeFileName);
   
   //
   // Read in the spec file
   //
   SpecFile sf;
   try {
      sf.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: Unable to read spec file " << specFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   QString topoFileName, coordFileName;
   if (sf.closedTopoFile.getNumberOfFiles() > 0) {
      topoFileName = sf.closedTopoFile.getFileName(0);
   }
   else {
      std::cout << "ERROR: spec file " << specFileName.toAscii().constData()
                << " does not contain a closed topo file." << std::endl;
      exit(-1);
   }
   if (sf.rawCoordFile.getNumberOfFiles() > 0) {
      coordFileName = sf.rawCoordFile.getFileName(0);
   }
   else {
      std::cout << "ERROR: spec file " << specFileName.toAscii().constData()
                << " does not contain a raw coord file." << std::endl;
      exit(-1);
   }
   
   
   //
   // Create a brain set
   //
   BrainSet brainSet(topoFileName, coordFileName);
   if (brainSet.getNumberOfBrainModels() <= 0) {
      std::cout << "ERROR: Failure creating the BrainSet from "
                << topoFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: Cannot find surface in BrainSet from "
                << coordFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Translate the surface 1/2 voxel size
   //
   float voxelSize[3];
   volume.getSpacing(voxelSize);
   TransformationMatrix tm;
   //tm.translate(voxelSize[0] * 0.5, voxelSize[1] * 0.5, voxelSize[2] * 0.5);
   tm.translate(offsetX, offsetY, offsetZ);
   bms->applyTransformationMatrix(tm);
   
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      std::cout << "ERROR: BrainSet created from "
                << topoFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << " has no nodes." << std::endl;
      exit(-1);
   }
   
   //
   // Create the paint file
   //
   PaintFile paintFile;
   paintFile.setNumberOfNodesAndColumns(numNodes, 1);
   
   //
   // Assign the paints
   //
   try {
      paintFile.assignPaintColumnWithVolumeFile(&volume,
                                                bms->getCoordinateFile(),
                                                0,
                                                paintName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the paint file
   //
   QString paintFileName(paintName);
   paintFileName.append(SpecFile::getPaintFileExtension());
   try {
      paintFile.writeFile(paintFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Resample the volume.
 */
static int
operationVolumeResample(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME RESAMPLE", "-volume-resample");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME RESAMPLE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-resample \\ " << std::endl
         << "         <f-new-spacing-x> <f-new-spacing-y> <f-new-spacing-z> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Resample the volume to the specified spacing." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float spacing[3];
   CommandLineUtilities::getNextParameter("New Spacing X",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          spacing[0]);
   CommandLineUtilities::getNextParameter("New Spacing Y",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          spacing[1]);
   CommandLineUtilities::getNextParameter("New Origin Z",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          spacing[2]);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // set the spacing
   //
   volume.resampleToSpacing(spacing);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Resize the volume.
 */
static int
operationVolumeResize(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME RESIZE", "-volume-resize");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME RESIZE" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-resize \\ " << std::endl
         << "         <i-x-min> <i-x-max> <i-y-min> <i-y-max> <i-z-min> <i-z-max> \\" << std::endl
         << "         <input-volume-name> <output-volume-name>" << std::endl
         << "         " << std::endl
         << "      Resize the volume.  If \"min\" values are less than zero," << std::endl
         << "      voxels will be added to that side of the volume." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int cropping[6];
   CommandLineUtilities::getNextParameter("X-Min",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cropping[0]);
   CommandLineUtilities::getNextParameter("X-Max",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cropping[1]);
   CommandLineUtilities::getNextParameter("Y-Min",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cropping[2]);
   CommandLineUtilities::getNextParameter("Y-Max",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cropping[3]);
   CommandLineUtilities::getNextParameter("Z-Min",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cropping[4]);
   CommandLineUtilities::getNextParameter("Z-Max",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cropping[5]);

   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // resize the volume
   //
   volume.resize(cropping);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Set the volume's orientation.
 */
static int
operationVolumeSetOrientation(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME SET ORIENTATION", "-volume-set-orientation");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME SET ORIENTATION" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-set-orientation \\ " << std::endl
         << "         <orientation> <input-volume-name> <output-volume-name> " << std::endl
         << "         " << std::endl
         << "      Set the orientation for the volume file." << std::endl
         << "      \"orientation\" consists of three letters from \"LRPAIS\" where" << std::endl
         << "         L     represents left" << std::endl
         << "         R     represents right" << std::endl
         << "         P     represents posterior" << std::endl
         << "         A     represents anterior" << std::endl
         << "         I     represents inferior" << std::endl
         << "         S     represents superior" << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString orientationString;
   CommandLineUtilities::getNextParameter("Orientation",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          orientationString);
   if (orientationString.length() != 3) {
      std::cout << "Orientation should contain 3 letters." << std::endl;
      exit(-1);
   }
   VolumeFile::ORIENTATION orient[3];
   for (int i = 0; i < 3; i++) {
      switch (orientationString[i].toLatin1()) {
         case 'L':
            orient[i] = VolumeFile::ORIENTATION_LEFT_TO_RIGHT;
            break;
         case 'R':
            orient[i] = VolumeFile::ORIENTATION_RIGHT_TO_LEFT;
            break;
         case 'P':
            orient[i] = VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR;
            break;
         case 'A':
            orient[i] = VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR;
            break;
         case 'I':
            orient[i] = VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR;
            break;
         case 'S':
            orient[i] = VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR;
            break;
         default:
            std::cout << "Invalid orientation character \""
                      << orientationString[i].toLatin1() << "\"" << std::endl;
            exit(-1);
            break;
      }
   }
   
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // set the orientation
   //
   volume.setOrientation(orient);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Rescale the volume's voxels.
 */
static int
operationVolumeRescaleVoxels(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("VOLUME RESCALE VOXELS", "-volume-rescale");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   VOLUME RESCALE VOXELS" << std::endl
         << "      " << programName.toAscii().constData() << " -volume-rescale \\ " << std::endl
         << "         <f-input-min> <f-input-max> <f-output-min> <f-output-max> \\ " << std::endl
         << "         <input-volume-name> <output-volume-name> " << std::endl
         << "         " << std::endl
         << "      Rescale a volume's voxels." << std::endl
         << "      \"input-min\" and below are mapped to \"output-min\"." << std::endl
         << "      \"input-max\" and above are mapped to \"output-max\"." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float inputMin;
   CommandLineUtilities::getNextParameter("Input Min",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputMin);
   float inputMax;
   CommandLineUtilities::getNextParameter("Input Max",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputMax);
   float outputMin;
   CommandLineUtilities::getNextParameter("Output Min",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMin);
   float outputMax;
   CommandLineUtilities::getNextParameter("Output Max",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMax);
   QString inputName, outputName, outputLabel;
   getInputAndOutputVolumeFileNames(argc, argv, argIndex, inputName, outputName, outputLabel);
   
   //
   // Read the input file
   //
   VolumeFile volume;
   readVolumeFile(volume, inputName);
   
   //
   // set the orientation
   //
   volume.rescaleVoxelValues(inputMin, inputMax, outputMin, outputMax);
   
   //
   // Write the volume
   //
   writeVolumeFile(volume, outputName, outputLabel);
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * add to spec file helper
 */
static int
addToSpecFileHelper(const QString& specFileName,
                    const QString& specFileTag,
                    const QString& dataFileName,
                    const QString& volumeDataFileName = "")
{
   SpecFile sf;
   try {
      sf.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading " << specFileName.toAscii().constData()
                << ": " << e.whatQString().toAscii().constData() << std::endl;
   }
   
   sf.addToSpecFile(specFileTag, dataFileName, volumeDataFileName, false);
   
   try {
      sf.writeFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: writing " << specFileName.toAscii().constData() 
                << ": " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Add to a spec file.
 */
static int
operationSpecFileAdd(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SPEC FILE ADD", "-spec-file-add");
         return 0;
      }
      printSeparatorLine();
      std::vector<SpecFile::Entry*> allEntries;
      SpecFile sf;
      sf.getAllEntries(allEntries);
      
      std::cout
         << "   SPEC FILE ADD" << std::endl
         << "      " << programName.toAscii().constData() << " -spec-file-add \\ " << std::endl
         << "         <spec-file-name> <spec-file-tag> <data-file-name> \\ " << std::endl
         << "         [data-file-second-name] " << std::endl
         << "         " << std::endl
         << "      Add a tag to a spec file." << std::endl
         << "      " << std::endl
         << "      The filename \"data-file-name\" is added to the spec file using" << std::endl
         << "      the \"spec-file-tag\".  If a volume file is added, specify the" << std::endl
         << "      header file as \"data-file-name\" and the data file as" << std::endl
         << "      \"data-file-second-name\"." << std::endl
         << std::endl
         << "      NOTE: The spec file must already exist." << std::endl
         << std::endl
         << "      Valid spec file tags are: " << std::endl;
         const QString title1(QString("TAG").leftJustified(35,' '));
         std::cout << "         " << title1.toAscii().constData() << " DESCRIPTION" << std::endl;
         for (unsigned int i = 0; i < allEntries.size(); i++) {
           QString tag(allEntries[i]->getSpecFileTag());
           const QString description(allEntries[i]->getDescriptiveName());
           tag = tag.leftJustified(35, ' ');
           std::cout << "         " << tag.toAscii().constData() << " "
                     << description.toAscii().constData() << std::endl;
         }
      std::cout
         << "      " << std::endl;

      return 0;
   }
   
   //
   // Get the parameters
   //
   QString specFileName;
   CommandLineUtilities::getNextParameter("Spec File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);
   QString specFileTag;
   CommandLineUtilities::getNextParameter("Spec File Tag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileTag);
   QString dataFileName;
   CommandLineUtilities::getNextParameter("Data File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          dataFileName);

   QString volumeDataFileName;
   if (argIndex < (argc - 1)) {
      CommandLineUtilities::getNextParameter("Data File Second Name (Volume Data)",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             volumeDataFileName);
   }
   
   addToSpecFileHelper(specFileName,
                       specFileTag,
                       dataFileName,
                       volumeDataFileName);
/*
   SpecFile sf;
   try {
      sf.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading " << specFileName.toAscii().constData()
                << ": " << e.whatQString().toAscii().constData() << std::endl;
   }
   
   sf.addToSpecFile(specFileTag, dataFileName, volumeDataFileName, false);
   
   try {
      sf.writeFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: writing " << specFileName.toAscii().constData() 
                << ": " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
*/
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Create a spec file.
 */
static int
operationSpecFileCreate(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SPEC FILE CREATE", "-spec-file-create");
         return 0;
      }
      printSeparatorLine();
      std::vector<QString> allSpecies;
      Species::getAllSpecies(allSpecies);
      
      std::vector<Structure::STRUCTURE_TYPE> structureTypes;
      std::vector<QString> structureNames;
      Structure::getAllTypesAndNames(structureTypes, structureNames, false);
      
      std::vector<StereotaxicSpace> allSpaces;
      StereotaxicSpace::getAllStereotaxicSpaces(allSpaces);
      
      std::vector<QString> allCategories;
      Categories::getAllCategories(allCategories);
      
      std::cout
         << "   SPEC FILE CREATE" << std::endl
         << "      " << programName.toAscii().constData() << " -spec-file-create \\ " << std::endl
         << "         <species> <subject> <structure> <stereotaxic-space> \\ " << std::endl
         << "         <category> [spec-file-name] " << std::endl
         << "         " << std::endl
         << "      Create a spec file." << std::endl
         << "      " << std::endl
         << "      The \"spec-file-name\" is optional and should only be specified " << std::endl
         << "      if you must name the spec file.  If the \"spec-file-name\" is not" << std::endl
         << "      specified, the spec file name will be created using the Caret " << std::endl
         << "      standard which composes the spec file name using the species," << std::endl
         << "      subject, and structure." << std::endl
         << "      " << std::endl
         << "      Note: None of the parameters may contain blank characters (spaces)." << std::endl
         << "      " << std::endl
         << "      Examples of \"species\" are: " << std::endl;

      for (int i = 0; i < static_cast<int>(allSpecies.size()); i++) {
         std::cout
            << "         " << allSpecies[i].toAscii().constData() << std::endl;
      }
      std::cout << std::endl;

      std::cout << "      Examples of \"structure\" are: " << std::endl;
      for (int i = 0; i < static_cast<int>(structureNames.size()); i++) {
         std::cout
            << "         " << structureNames[i].toAscii().constData() << std::endl;
      }
      std::cout << std::endl;

      std::cout << "      Examples of \"stereotaxic-space\" are: " << std::endl;
      for (int i = 0; i < static_cast<int>(allSpaces.size()); i++) {
         std::cout
            << "         " << allSpaces[i].getName().toAscii().constData() << std::endl;
      }
      std::cout << std::endl;

      std::cout << "      Examples of \"category\" are: " << std::endl;
      for (int i = 0; i < static_cast<int>(allCategories.size()); i++) {
         std::cout
            << "         " << allCategories[i].toAscii().constData() << std::endl;
      }
      std::cout << std::endl;

      return 0;
   }
   
   //
   // Get the parameters
   //
   QString speciesName;
   CommandLineUtilities::getNextParameter("Species",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          speciesName);
   QString subjectName;
   CommandLineUtilities::getNextParameter("Subject",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          subjectName);
   QString structureName;
   CommandLineUtilities::getNextParameter("Structure",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          structureName);

   QString spaceName;
   CommandLineUtilities::getNextParameter("Space",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          spaceName);

   QString categoryName;
   CommandLineUtilities::getNextParameter("category",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          categoryName);

   QString specFileName;
   if (argc >= 8) {
      CommandLineUtilities::getNextParameter("Spec File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             specFileName);
   }

   Structure st(structureName);
   if (st.isInvalid()) {
      std::cout << "WARNING: Structure is invalid." << std::endl;
   }
   
   SpecFile sf;
   
   if (specFileName.isEmpty()) {
      specFileName = speciesName + "."
                   + subjectName + "."
                   + st.getTypeAsAbbreviatedString() 
                   + SpecFile::getSpecFileExtension();
   }
   
   sf.setSpecies(speciesName);
   sf.setSubject(subjectName);
   sf.setSpace(spaceName);
   sf.setStructure(structureName);
   sf.setCategory(categoryName);
   
   try {
      sf.writeFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: writing " << specFileName.toAscii().constData() 
                << ": " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Create an average surface.
 */
static int
operationSurfaceAverage(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE CREATE AVERAGE", "-surface-average");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE CREATE AVERAGE" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-average \\ " << std::endl
         << "         <output-coordinate-file> \\ " << std::endl
         << "         <input-coordinate-file> [additional-input-coordinate-files] \\" << std::endl
         << "         [optional-surface-shape-file-name] " << std::endl
         << "         " << std::endl
         << "      Create an average surface." << std::endl
         << "      " << std::endl
         << "      Since the type of surface being averaged is unknown, there is " << std::endl
         << "      no option to update a spec file.  If you wish to update a spec" << std::endl
         << "      file, see the help information for \"-spec-file-add\"." << std::endl
         << "      " << std::endl
         << "      If a surface shape file name is specified, it will be created" << std::endl
         << "      and contain the coordinate uncertainty.  If a surface shape file" << std::endl
         << "      is specified, its name MUST END WITH \"" << SpecFile::getSurfaceShapeFileExtension().toAscii().constData() << "\"." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString outputCoordFileName;
   CommandLineUtilities::getNextParameter("Output Coordinate File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputCoordFileName);

   std::vector<QString> inputCoordFileNames;
   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coordinate File 1",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   inputCoordFileNames.push_back(coordFileName);
             
   //
   // Is the last file a surface shape file?
   //
   QString surfaceShapeFileName = argv[argc - 1];
   if (surfaceShapeFileName.endsWith(SpecFile::getSurfaceShapeFileExtension())) {
      argc--;
   }
   else {
      surfaceShapeFileName = "";
   }
   
   //
   // Get the coordinate file names
   //
   for (int i = argIndex; i < (argc - 1); i++) {
      QString promptString("Input Coordinate File ");
      promptString += QString::number(i - argIndex + 2);
      CommandLineUtilities::getNextParameter(promptString,
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             coordFileName);
      inputCoordFileNames.push_back(coordFileName);
   }
   
   //
   // Read the input coordinate files
   //   
   std::vector<CoordinateFile*> coordinateFiles;
   for (int i = 0; i < static_cast<int>(inputCoordFileNames.size()); i++) {
      CoordinateFile* coordFile = new CoordinateFile;
      readCoordinateFile(*coordFile, inputCoordFileNames[i]);
      coordinateFiles.push_back(coordFile);
   }
   
   //
   // Should a surface shape file containing uncertainty be created?
   //
   SurfaceShapeFile* ssf = NULL;
   if (surfaceShapeFileName.isEmpty() == false) {
      ssf = new SurfaceShapeFile;
   }
   
   //
   // Create the average coordinate file
   //
   CoordinateFile outputCoordinateFile;
   CoordinateFile::createAverageCoordinateFile(coordinateFiles,
                                               outputCoordinateFile,
                                               ssf);
   
   //
   // Write the coordinate file
   //
   writeCoordinateFile(outputCoordinateFile, outputCoordFileName);
   
   //
   // Write surface shape file if it was created
   //
   if (ssf != NULL) {
      writeSurfaceShapeFile(*ssf, surfaceShapeFileName);
   }
   
   //
   // Free memory
   //
   for (unsigned int i = 0; i < coordinateFiles.size(); i++) {
      delete coordinateFiles[i];
   }
   if (ssf != NULL) {
      delete ssf;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Create a inflated and/or ellipsoid surfaces.
 */
static int
operationSurfaceInflatedEllipsoid(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE GENERATE INFLATED and/or ELLIPSOID", "-surface-generate");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE GENERATE INFLATED and/or ELLIPSOID" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-generate \\ " << std::endl
         << "         <input-fiducial-coord-file> <input-closed-topology-file> \\ " << std::endl
         << "         <output-inflated-coord-file> <output-very-inflated-coord-file> \\ " << std::endl
         << "         <output-ellipsoid-coord-file> <output-spherical-coord-file> \\ " << std::endl
         << "         [output-spec-file]  " << std::endl
         << "         " << std::endl
         << "      Generate inflated, ellipsoid, and spherical coordinate files." << std::endl
         << "      " << std::endl
         << "      If any of the output coordinate files have an empty name (two " << std::endl
         << "      consecutive double qutoes \"\", the coordinate file will not be" << std::endl
         << "      output.  The spec file is optional, and if specified, the generated" << std::endl
         << "      coordinate files will be added to the spec file.  For best results, " << std::endl
         << "      the fiducial surface must not contain topological defects. " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Output Coordinate File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);

   QString topoFileName;
   CommandLineUtilities::getNextParameter("Closed Topology File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString inflatedCoordFileName;
   CommandLineUtilities::getNextParameter("Inflated Coordinate File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inflatedCoordFileName);

   QString veryInflatedCoordFileName;
   CommandLineUtilities::getNextParameter("Very Inflated Coordinate File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          veryInflatedCoordFileName);

   QString ellipsoidCoordFileName;
   CommandLineUtilities::getNextParameter("Ellipsoid Coordinate File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          ellipsoidCoordFileName);

   QString sphericalCoordFileName;
   CommandLineUtilities::getNextParameter("Spherical Coordinate File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sphericalCoordFileName);


   QString specFileName;
   if (argIndex < (argc -1)) {
      CommandLineUtilities::getNextParameter("Spec File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             specFileName);
   }
                                          
   //
   // Create and verify a brain set using the coord and topo files
   //
   BrainSet bs(topoFileName, fiducialCoordFileName);
   if (bs.getNumberOfBrainModels() <= 0) {
      std::cerr << "ERROR: Problem with fiducial coord or closed topo file (no brain models found)." << std::endl;
      exit(-1);
   }
   if (bs.getNumberOfTopologyFiles() <= 0) {
      std::cerr << "ERROR: Problem with closed topo file." << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = bs.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cerr << "ERROR: Problem with coordinate file (no brain model surface found)." << std::endl;
      exit(-1);
   }
   if (bms->getTopologyFile() == NULL) {
      std::cerr << "ERROR: Problem with closed topo file (brain model surface has no topology)." << std::endl;
      exit(-1);
   }
   
   //
   // Generate surfaces
   //
   bms->createInflatedAndEllipsoidFromFiducial((inflatedCoordFileName.isEmpty() == false),
                                               (veryInflatedCoordFileName.isEmpty() == false),
                                               (ellipsoidCoordFileName.isEmpty() == false),
                                               (sphericalCoordFileName.isEmpty() == false),
                                               false,
                                               true,
                                               NULL);
   
   //
   // If the spec file name is avaiable tell the brain set so that the 
   // spec file will be updated.
   //
   bool haveSpecFile =false;
   if (specFileName.isEmpty() == false) {
      bs.setSpecFileName(specFileName);
      haveSpecFile = true;
   }
   
   //
   // Write the inflated surface
   //
   if (inflatedCoordFileName.isEmpty() == false) {
      BrainModelSurface* bms = bs.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_INFLATED);
      if (bms != NULL) {
         try {
            bs.writeCoordinateFile(inflatedCoordFileName,
                                   bms->getSurfaceType(),
                                   bms->getCoordinateFile(),
                                   haveSpecFile);
         }
         catch (FileException& e) {
            std::cerr << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
         }
      }
      else {
         std::cerr << "ERROR: Unable to find inflated type surface for writing." << std::endl;
      }
   }
   
   //
   // Write the very inflated surface
   //
   if (veryInflatedCoordFileName.isEmpty() == false) {
      BrainModelSurface* bms = bs.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_VERY_INFLATED);
      if (bms != NULL) {
         try {
            bs.writeCoordinateFile(veryInflatedCoordFileName,
                                   bms->getSurfaceType(),
                                   bms->getCoordinateFile(),
                                   haveSpecFile);
         }
         catch (FileException& e) {
            std::cerr << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
         }
      }
      else {
         std::cerr << "ERROR: Unable to find very inflated type surface for writing." << std::endl;
      }
   }
   
   //
   // Write the ellipsoid surface
   //
   if (ellipsoidCoordFileName.isEmpty() == false) {
      BrainModelSurface* bms = bs.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL);
      if (bms != NULL) {
         try {
            bs.writeCoordinateFile(ellipsoidCoordFileName,
                                   bms->getSurfaceType(),
                                   bms->getCoordinateFile(),
                                   haveSpecFile);
         }
         catch (FileException& e) {
            std::cerr << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
         }
      }
      else {
         std::cerr << "ERROR: Unable to find ellipsoid type surface for writing." << std::endl;
      }
   }
   
   //
   // Write the spherical surface
   //
   if (sphericalCoordFileName.isEmpty() == false) {
      BrainModelSurface* bms = bs.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
      if (bms != NULL) {
         try {
            bs.writeCoordinateFile(sphericalCoordFileName,
                                   bms->getSurfaceType(),
                                   bms->getCoordinateFile(),
                                   haveSpecFile);
         }
         catch (FileException& e) {
            std::cerr << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
         }
      }
      else {
         std::cerr << "ERROR: Unable to find spherical type surface for writing." << std::endl;
      }
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Create a segmentation volume from a surface.
 */
static int
operationSurfaceToSegmentationVolume(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE CREATE SEGMENTATION", "-surface-to-segmentation-volume");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE CREATE SEGMENTATION" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-to-segmentation-volume \\ " << std::endl
         << "         <input-coordinate-file> <input-topology-file> \\ " << std::endl
         << "         <output-volume-file> " << std::endl
         << "         " << std::endl
         << "      Create a segmentation volume from a surface.  The output volume file" << std::endl
         << "      MUST be an existing file that is in the same stereotaxic space as the" << std::endl
         << "      surface.  Run this program with \"-volume-create\" to create a volume" << std::endl
         << "      file." << std::endl
         << "      " << std::endl
         << "      Also see \"-surface-to-segmentation-volume\"." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputCoordFileName;
   CommandLineUtilities::getNextParameter("Input Coordinate File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputCoordFileName);

   QString inputTopoFileName;
   CommandLineUtilities::getNextParameter("Input Topology File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputTopoFileName);
   
   QString outputVolumeFileName, outputVolumeFileLabel;
   getOutputVolumeFileName(argc,
                           argv,
                           argIndex,
                           outputVolumeFileName,
                           outputVolumeFileLabel);
   
   //
   // Read the volume file
   //
   VolumeFile outputVolumeFile;
   readVolumeFile(outputVolumeFile, outputVolumeFileName);

   //
   // Create a brain set from coord and topo
   //
   BrainSet brain(inputTopoFileName, inputCoordFileName);
   if (brain.getNumberOfBrainModels() < 1) {
      std::cout << "ERROR: reading topology or coordinate file (no brain model)." << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brain.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: reading topology or coordinate file (no surface)." << std::endl;
      exit(-1);
   }
   
   //
   // Create the segmentation volume
   //
   BrainModelSurfaceToVolumeSegmentationConverter bmssc(&brain,
                                                        bms,
                                                        &outputVolumeFile,
                                                        false,
                                                        false);
   try {
      bmssc.execute();
      writeVolumeFile(outputVolumeFile, outputVolumeFileName, outputVolumeFileLabel);      
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::exit(-1);
   }
   
/*
   //
   // Get volume attributes
   // 
   int dimensions[3];
   outputVolumeFile.getDimensions(dimensions);
   float spacing[3];
   outputVolumeFile.getSpacing(spacing);
   float origin[3];
   outputVolumeFile.getOrigin(origin);
   
   //
   // Convert the surface to a segmentation volume
   //
   const float surfaceOffset[3] = { 0.0, 0.0, 0.0 };
   BrainModelSurfaceToVolumeConverter bmsv(&brain,
                                           bms,
                                           StereotaxicSpace::SPACE_UNKNOWN,
                                           surfaceOffset,
                                           dimensions,
                                           spacing,
                                           origin,
                                           -1.5,
                                           0.0,
                                           0.5,
            BrainModelSurfaceToVolumeConverter::CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES);
   try {
      bmsv.execute();
      
      VolumeFile* vf = bmsv.getOutputVolume();
      if (vf != NULL) {
         writeVolumeFile(*vf, outputVolumeFileName, outputVolumeFileLabel);
      }
      else {
         std::cout << "ERROR: Failed to create volume." << std::endl;
         exit(-1);
      }
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::exit(-1);
   }
*/
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Display information about a surface.
 */
static int
operationSurfaceInformation(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE INFORMATION", "-surface-information");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE INFORMATION" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-information \\ " << std::endl
         << "         <input-topo-file> <input-coord-file>\\ " << std::endl
         << "         [output-text-file]" << std::endl
         << "         " << std::endl
         << "      Display information about the surface made using the specified" << std::endl
         << "      coordinate and topology files. " << std::endl
         << std::endl
         << "      If the \"output-test-file\" is not specified, then the information." << std::endl
         << "      will be printed in the terminal." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Topology File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);
   
   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coordinate File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   QString textFileName;
   if (argc >= 5) {
      CommandLineUtilities::getNextParameter("Output Text File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             textFileName);
   }
   
   //
   // Create a brain set from the topo/coord pair
   //
   BrainSet brainSet(topoFileName, coordFileName);
   const BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: surface was not created.  Are coord and topo names valid?" << std::endl;
      std::exit(-1);
   }
   const TopologyFile* tf = bms->getTopologyFile();
   if (tf == NULL) {
      std::cout << "ERROR: cannot find topology.  Is topo name valid?" << std::endl;
      std::exit(-1);
   }

   std::vector<QString> labels, values;
   bms->getSurfaceInformation(labels,
                              values);

   //
   // Determine minimum width for labels
   //   
   const int numItems = static_cast<int>(labels.size());
   int minWidth = 1;
   for (int i = 0; i < numItems; i++) {
      minWidth = std::max(minWidth, labels[i].length());
   }
   minWidth += 2;
   
   QString s;
   for (int i = 0; i < numItems; i++) {
      const QString space((minWidth - labels[i].size()), QChar(' '));
      s += (labels[i]
            + ":"
            + space
            + values[i]
            + "\n");
   }
   
   if (textFileName.isEmpty() == false) {
      TextFile textFile;
      textFile.setText(s);
      try {
         textFile.writeFile(textFileName);
      }
      catch (FileException& e) {
         std::cout << "ERROR writing text file: " << e.whatQString().toAscii().constData() << std::endl;
         std::exit(-1);
      }
   }
   else {
      std::cout << s.toAscii().constData()
                << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Create a sulcal depth, curvature, and geography paint file a surface.
 */
static int
operationSurfaceIdentifySulci(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE IDENTIFY SULCI", "-surface-identify-sulci");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE IDENTIFY SULCI" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-identify-sulci \\ " << std::endl
         << "         <input-spec-file> <input-structure>\\ " << std::endl
         << "         <input-segmentation-volume> <input-closed-topo-file> \\" << std::endl
         << "         <input-raw-coord-file> <input-fiducial-coord-file> [write-volume-type]" << std::endl
         << "         " << std::endl
         << "      Create a surface shape file containing depth and curvature measurements," << std::endl
         << "      a paint file identifying the sulci, and an area color file.  If there " << std::endl
         << "      is no raw coordinate file, specify fiducial coordinate file instead." << std::endl
         << std::endl
         << "      NOTE: This command MUST be run in the directory containing the files." << std::endl
         << std::endl
         << "      <input-structure> is one of:" << std::endl
         << "         " << Structure::convertTypeToString(Structure::STRUCTURE_TYPE_CORTEX_LEFT).toAscii().constData() << std::endl
         << "         " << Structure::convertTypeToString(Structure::STRUCTURE_TYPE_CORTEX_RIGHT).toAscii().constData() << std::endl
         << "" << std::endl
         << "      [write-volume-type]   Type of volume files to write." << std::endl
         << "         Specifies the type of the volume files that will be written" << std::endl
         << "         during the segmentation process.  Valid values are" << std::endl
         << "            AFNI (default)" << std::endl
         << "            NIFTI " << std::endl
         << "            SPM" << std::endl
         << "            WUNIL" << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString specFileName;
   CommandLineUtilities::getNextParameter("Input Spec File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);

   QString structureName;
   CommandLineUtilities::getNextParameter("Structure Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          structureName);
   Structure structure(structureName);
   
   QString segmentationVolumeFileName;
   CommandLineUtilities::getNextParameter("Segmentation Volume File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          segmentationVolumeFileName);
   
   QString closedTopoFileName;
   CommandLineUtilities::getNextParameter("Closed Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          closedTopoFileName);
   
   QString rawCoordFileName;
   CommandLineUtilities::getNextParameter("Raw Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          rawCoordFileName);
   
   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Fiducial Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);
   
   VolumeFile::FILE_READ_WRITE_TYPE writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
   if (argc >= 9) {
      QString volumeFileTypeName;
      CommandLineUtilities::getNextParameter("Write Volume File Type",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             volumeFileTypeName);
      if (volumeFileTypeName == "AFNI") {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_AFNI;
      }
      else if (volumeFileTypeName == "NIFTI") {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_NIFTI;
      }
      else if (volumeFileTypeName == "SPM") {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_SPM_OR_MEDX;
      }
      else if (volumeFileTypeName == "WUNIL") {
         writeVolumeType = VolumeFile::FILE_READ_WRITE_TYPE_WUNIL;
      }
      else  {
         std::cout << "ERROR: Invalid volume file write type: " 
                   << volumeFileTypeName.toAscii().constData() << std::endl;
         exit(-1);
      }
   }
                                          
   //
   // Load the segmentation volume to get the data file name
   //
   VolumeFile volTest;
   QString segmentationVolumeDataFileName;
   try {
      volTest.readFileMetaDataOnly(segmentationVolumeFileName);
      segmentationVolumeDataFileName = volTest.getDataFileName();
   }
   catch (FileException&) {
      // ignore
   }
   
   //
   // Read spec file
   //
   SpecFile specFile;
   try {
      specFile.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading spec file " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Set the selected files
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::volumeSegmentationFileTag, segmentationVolumeFileName, 
                          segmentationVolumeDataFileName, SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::closedTopoFileTag, closedTopoFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::rawCoordFileTag, rawCoordFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::fiducialCoordFileTag, fiducialCoordFileName, 
                          "", SpecFile::SPEC_FALSE);
    
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, specFileName, errorMessage)) {
      std::cout << "ERROR: Reading spec file: " << std::endl
                << errorMessage.toAscii().constData() << std::endl;
   }
   
   //
   // Identify the sulci
   //
   BrainModelVolumeSureFitSegmentation bmsf(&brainSet,
                                            structure.getType(),
                                            writeVolumeType,
                                            true);
   try {
      bmsf.executeIdentifySulci();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << "ERROR identifying sulci: " << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Spherical registration of a surface.
 */
static int
operationSurfaceRegistrationSpherical(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE SPHERICAL REGISTRATION", "-surface-register-sphere");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE SPHERICAL REGISTRATION" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-register-sphere \\ " << std::endl
         << "         <options> <deformation-map-file-containing-registration-parameters> \\" << std::endl
         << "         <indiv-spec-file> <atlas-spec-file>   " << std::endl
         << "      " << std::endl
         << "      The above method requires that the each of the spec file have one and only" << std::endl
         << "      one border projection file, closed topology file,fiducial coordinate file," << std::endl
         << "      and spherical coordinate file.  The spec files may have zero or one" << std::endl
         << "      cut topology file and flat coordinate file. " << std::endl
         << "      " << std::endl
         << "      " << std::endl
         << "      " << programName.toAscii().constData() << " -surface-register-sphere \\ " << std::endl
         << "         <options> <deformation-map-file-containing-registration-parameters> \\" << std::endl
         << "         <indiv-spec-file> <indiv-border-projection-file>  \\ " << std::endl
         << "         <indiv-closed-topo-file> <indiv-cut-topo-file> \\" << std::endl
         << "         <indiv-fiducial-coord-file> <indiv-spherical-coord-file> \\" << std::endl
         << "         <indiv-flat-coord-file> \\" << std::endl
         << "         <atlas-spec-file> <atlas-border-projection-file>  \\ " << std::endl
         << "         <atlas-closed-topo-file> <atlas-cut-topo-file> \\" << std::endl
         << "         <atlas-fiducial-coord-file> <atlas-spherical-coord-file> \\" << std::endl
         << "         <atlas-flat-coord-file> " << std::endl
         << "         " << std::endl
         << "         " << std::endl
         << "      Peform spherical registration." << std::endl
         << "      " << std::endl
         << "      Some files, such as the flat coordinate files or deformation map file" << std::endl
         << "      are optional and in this case the name should be specified with two" << std::endl
         << "      consecutive double quotes (eg: \"\")." << std::endl
         << "         " << std::endl
         << "      options - a single character either 'Y' or 'N'   " << std::endl
         << "         Y - deform the individual to the atlas and the atlas to the individual." << std::endl
         << "         N - deform the individual to the atlas only." << std::endl
         << "         " << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   QString options;
   QString indivSpecFileName;
   QString indivBorderFileName;
   QString indivClosedTopoFileName;
   QString indivCutTopologyFileName;
   QString indivFiducialCoordFileName;
   QString indivSphericalCoordFileName;
   QString indivFlatCoordFileName;
   QString atlasSpecFileName;
   QString atlasBorderFileName;
   QString atlasClosedTopoFileName;
   QString atlasCutTopologyFileName;
   QString atlasFiducialCoordFileName;
   QString atlasSphericalCoordFileName;
   QString atlasFlatCoordFileName;
   QString defMapFileName;
   
   //
   // Get the parameters
   //
   CommandLineUtilities::getNextParameter("Options",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          options);
   //
   // Get the parameters
   //
   CommandLineUtilities::getNextParameter("Deformation Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          atlasFlatCoordFileName);
         
   if (argc == 6) {
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Indiv Spec File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivSpecFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Atlas Spec File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasSpecFileName);
     
      QString errorMessage; 
      
      //
      // Read the spec file and set the files for deformation for the individual
      //
      try {
         SpecFile sf;
         sf.readFile(indivSpecFileName);
         
         if (sf.borderProjectionFile.getNumberOfFiles() == 1) {
            indivBorderFileName = sf.borderProjectionFile.getFileName(0);
         }
         else {
            errorMessage.append("Indiv spec file does not have exactly one border projection file.\n");
         }
         if (sf.closedTopoFile.getNumberOfFiles() == 1) {
            indivClosedTopoFileName = sf.closedTopoFile.getFileName(0);
         }
         else {
            errorMessage.append("Indiv spec file does not have exactly one closed topology file.\n");
         }
         if (sf.cutTopoFile.getNumberOfFiles() > 1) {
            errorMessage.append("Indiv spec file has more than one cut topology file.\n");
         }
         else if (sf.cutTopoFile.getNumberOfFiles() == 1) {
            indivCutTopologyFileName = sf.cutTopoFile.getFileName(0);
         }
         if (sf.fiducialCoordFile.getNumberOfFiles() == 1) {
            indivFiducialCoordFileName = sf.fiducialCoordFile.getFileName(0);
         }
         else {
            errorMessage.append("Indiv spec file does not have exactly one fiducial coordinate file.\n");
         }
         if (sf.sphericalCoordFile.getNumberOfFiles() == 1) {
            indivSphericalCoordFileName = sf.sphericalCoordFile.getFileName(0);
         }
         else {
            errorMessage.append("Indiv spec file does not have exactly one spherical coordinate file.\n");
         }
         if (sf.flatCoordFile.getNumberOfFiles() > 1) {
            errorMessage.append("Indiv spec file has more than one flat coordinate file.\n");
         }
         else if (sf.flatCoordFile.getNumberOfFiles() == 1) {
            indivFlatCoordFileName = sf.flatCoordFile.getFileName(0);
         }
      }
      catch (FileException& e) {
         std::cout << e.whatQString().toAscii().constData() << std::endl;
         exit(-1);
      }

      //
      // Read the spec file and set the files for deformation for the atlas
      //
      try {
         SpecFile sf;
         sf.readFile(atlasSpecFileName);
         
         if (sf.borderProjectionFile.getNumberOfFiles() == 1) {
            atlasBorderFileName = sf.borderProjectionFile.getFileName(0);
         }
         else {
            errorMessage.append("atlas spec file does not have exactly one border projection file.\n");
         }
         if (sf.closedTopoFile.getNumberOfFiles() == 1) {
            atlasClosedTopoFileName = sf.closedTopoFile.getFileName(0);
         }
         else {
            errorMessage.append("atlas spec file does not have exactly one closed topology file.\n");
         }
         if (sf.cutTopoFile.getNumberOfFiles() > 1) {
            errorMessage.append("atlas spec file has more than one cut topology file.\n");
         }
         else if (sf.cutTopoFile.getNumberOfFiles() == 1) {
            atlasCutTopologyFileName = sf.cutTopoFile.getFileName(0);
         }
         if (sf.fiducialCoordFile.getNumberOfFiles() == 1) {
            atlasFiducialCoordFileName = sf.fiducialCoordFile.getFileName(0);
         }
         else {
            errorMessage.append("atlas spec file does not have exactly one fiducial coordinate file.\n");
         }
         if (sf.sphericalCoordFile.getNumberOfFiles() == 1) {
            atlasSphericalCoordFileName = sf.sphericalCoordFile.getFileName(0);
         }
         else {
            errorMessage.append("atlas spec file does not have exactly one spherical coordinate file.\n");
         }
         if (sf.flatCoordFile.getNumberOfFiles() > 1) {
            errorMessage.append("atlas spec file has more than one flat coordinate file.\n");
         }
         else if (sf.flatCoordFile.getNumberOfFiles() == 1) {
            atlasFlatCoordFileName = sf.flatCoordFile.getFileName(0);
         }
      }
      catch (FileException& e) {
         std::cout << e.whatQString().toAscii().constData() << std::endl;
         exit(-1);  
      }
      
      if (errorMessage.isEmpty() == false) {
         std::cout << "ERROR: " << errorMessage.toAscii().constData() << std::endl;
         exit(-1);
      }
   }
   else {
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Indiv Spec File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivSpecFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Indiv Border Projection File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivBorderFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Indiv Closed Topology File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivClosedTopoFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Indiv Cut Topology File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivCutTopologyFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Indiv Fiducial Coordinate File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivFiducialCoordFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Indiv Spherical Coordinate File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivSphericalCoordFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Indiv Flat Coordinate File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivFlatCoordFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Atlas Spec File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasSpecFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Atlas Border Projection File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasBorderFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Atlas Closed Topology File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasClosedTopoFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Atlas Cut Topology File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasCutTopologyFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Atlas Fiducial Coordinate File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasFiducialCoordFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Atlas Spherical Coordinate File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasSphericalCoordFileName);
      
      //
      // Get the parameters
      //
      CommandLineUtilities::getNextParameter("Atlas Flat Coordinate File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasFlatCoordFileName);
   }
         
   //
   // Read in the deformation map (if specified)
   //
   DeformationMapFile defMapFile;
   if (defMapFileName.isEmpty() == false) {
      try {
         defMapFile.readFile(defMapFileName);
      }
      catch (FileException& e) {
         std::cout << e.whatQString().toAscii().constData() << std::endl;
      }
   }
   
   //
   // Process options
   //   
   defMapFile.setDeformBothWays(false);
   if (options.length() > 0) {
      if (options[0] == 'Y') {
         defMapFile.setDeformBothWays(true);
      }
   }
   
   //
   // Spherical deformation
   //
   defMapFile.setFlatOrSphereSelection(DeformationMapFile::DEFORMATION_TYPE_SPHERE);

   //
   // Make paths absolute
   //   
   if (QFileInfo(indivSpecFileName).isRelative()) {
      indivSpecFileName = QFileInfo(indivSpecFileName).absoluteFilePath();
   }
   if (QFileInfo(atlasSpecFileName).isRelative()) {
      atlasSpecFileName = QFileInfo(atlasSpecFileName).absoluteFilePath();
   }
      
   //
   // Set the files
   //
   //defMapFile.setSourceDirectory(FileUtilities::dirname(indivSpecFileName));
   //defMapFile.setSourceSpecFileName(FileUtilities::basename(indivSpecFileName));
   defMapFile.setSourceSpecFileName(indivSpecFileName);
   defMapFile.setSourceBorderFileName(indivBorderFileName, DeformationMapFile::BORDER_FILE_PROJECTION);
   defMapFile.setSourceClosedTopoFileName(indivClosedTopoFileName);
   defMapFile.setSourceCutTopoFileName(indivCutTopologyFileName);
   defMapFile.setSourceFiducialCoordFileName(indivFiducialCoordFileName);
   defMapFile.setSourceSphericalCoordFileName(indivSphericalCoordFileName);
   defMapFile.setSourceDeformedFlatCoordFileName(indivFlatCoordFileName);
   //defMapFile.setTargetDirectory(FileUtilities::dirname(atlasSpecFileName));
   //defMapFile.setTargetSpecFileName(FileUtilities::basename(atlasSpecFileName));
   defMapFile.setTargetSpecFileName(atlasSpecFileName);
   defMapFile.setTargetBorderFileName(atlasBorderFileName, DeformationMapFile::BORDER_FILE_PROJECTION);
   defMapFile.setTargetClosedTopoFileName(atlasClosedTopoFileName);
   defMapFile.setTargetCutTopoFileName(atlasCutTopologyFileName);
   defMapFile.setTargetFiducialCoordFileName(atlasFiducialCoordFileName);
   defMapFile.setTargetSphericalCoordFileName(atlasSphericalCoordFileName);
   defMapFile.setTargetFlatCoordFileName(atlasFlatCoordFileName);
   
   //
   // Run the deformation
   //
   BrainSet bs;
   BrainModelSurfaceDeformationSpherical alg(&bs,
                                             &defMapFile);
   try {
      alg.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Perform a crossover check on a surface.
 */
static int
operationSurfaceCrossoverCheck(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE CROSSOVER CHECK", "-surface-crossover-check");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE CROSSOVER CHECK" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-crossover-check \\ " << std::endl
         << "         <coordinate-file-name> <topology-file-name> " << std::endl
         << "         " << std::endl
         << "      Perform a crossover check on a surface (coordinate and topology files)." << std::endl
         << "         " << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString coordFileName;
   CommandLineUtilities::getNextParameter("Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);
   
   
   //
   // Create a brain set from coord and topo
   //
   BrainSet brain(topoFileName, coordFileName);
   if (brain.getNumberOfBrainModels() < 1) {
      std::cout << "ERROR: reading topology or coordinate file (no brain model)." << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brain.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: reading topology or coordinate file (no surface)." << std::endl;
      exit(-1);
   }
   
   //
   // Do a crossover check
   //   
   int tileCrossovers = 0;
   int nodeCrossovers = 0;
   bms->crossoverCheck(tileCrossovers,
                       nodeCrossovers);
                       
   std::cout << "Crossover Check for "
             << FileUtilities::basename(coordFileName).toAscii().constData() << " "
             << FileUtilities::basename(topoFileName).toAscii().constData() << std::endl;
   std::cout << "   Tile Crossovers: " << tileCrossovers << std::endl;
   std::cout << "   Node Crossovers: " << nodeCrossovers << std::endl;
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Smooth an identified area on a surface.
 */
static int
operationSurfaceRoiCoordReport(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE ROI COORD REPORT", "-surface-roi-coord-report");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE ROI COORD REPORT" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-roi-coord-report \\ " << std::endl
         << "         <output-file-name.txt> \\" << std::endl
         << "         <input-topo-file-name> \\" << std::endl
         << "         <input-paint-file-name> \\" << std::endl
         << "         <i-input-paint-file-column> \\" << std::endl
         << "         <input-paint-name> \\" << std::endl
         << "         <input-coord-file-1> \\" << std::endl
         << "         <input-coord-file-2> \\" << std::endl
         << "         ...\\" << std::endl
         << "         <input-coord-file-N" << std::endl
         << "         " << std::endl
         << "      Print a report listing the mean X, Y, an Z coordinates for " << std::endl
         << "      the nodes identified by the paint name.  " << std::endl
         << "      " << std::endl
         << "      Note: The paint file column indices start at one." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString outputTextFileName;
   CommandLineUtilities::getNextParameter("Output Text File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputTextFileName);

   QString topoFileName;
   CommandLineUtilities::getNextParameter("Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);
   
   QString paintFileName;
   CommandLineUtilities::getNextParameter("Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintFileName);
   
   int paintColumnNumber;
   CommandLineUtilities::getNextParameter("Paint Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintColumnNumber);
   
   QString paintName;
   CommandLineUtilities::getNextParameter("Paint Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintName);
   
   //
   // Coordinate files
   //
   std::vector<QString> inputCoordFileNames;
   for (int i = (argIndex + 1); i < argc; i++) {
      inputCoordFileNames.push_back(argv[i]);
   }
   const int numInputCoordFiles = static_cast<int>(inputCoordFileNames.size());
   if (numInputCoordFiles <= 0) {
      std::cout << "ERROR: No coordinate files are specified." << std::endl;
      std::exit(-1);
   }
   
   //
   // Read the spec file into a brain set
   //
   SpecFile specFile;
   for (int i = 0; i < numInputCoordFiles; i++) {
      specFile.addToSpecFile(SpecFile::fiducialCoordFileTag,
                             inputCoordFileNames[i],
                             "",
                             false);
   }
   specFile.addToSpecFile(SpecFile::closedTopoFileTag,
                          topoFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::paintFileTag,
                          paintFileName,
                          "",
                          false);
                          
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   brainSet.setIgnoreTopologyFileInCoordinateFileHeaderFlag(true);
   if (brainSet.readSpecFile(specFile, "", errorMessage)) {
      std::cout << "ERROR: Reading spec file: " << std::endl
                << errorMessage.toAscii().constData() << std::endl;
      std::exit(-1);
   }

   //
   // Find the paint file column with the proper name
   //
   const PaintFile* paintFile = brainSet.getPaintFile();
   paintColumnNumber--;  // user enters 1..N but index 0..N-1
   if ((paintColumnNumber < 0) ||
       (paintColumnNumber >= paintFile->getNumberOfColumns())) {
      std::cout << "ERROR: Invalid paint column number: "
                << paintColumnNumber << std::endl;
      std::exit(-1);
   }
   
   //
   // Find the index of the paint name
   //
   const int paintIndex = paintFile->getPaintIndexFromName(paintName);
   if (paintIndex < 0) {
      std::cout << "Unable to find a paint named " << paintName.toAscii().constData() << " in the paint file." << std::endl;
      std::exit(-1);
   }
   
   //
   // Find the coordinate files
   //
   std::vector<CoordinateFile*> coordFiles;
   for (int i = 0; i < brainSet.getNumberOfBrainModels(); i++) {
      BrainModelSurface* bms = brainSet.getBrainModelSurface(i);
      if (bms != NULL) {
         coordFiles.push_back(bms->getCoordinateFile());
      }
   }
   const int numCoordFiles = static_cast<int>(coordFiles.size());
   if (numCoordFiles <= 0) {
      std::cout << "ERROR: No surfaces were read." << std::endl;
      std::exit(-1);
   }

   //
   // Get topology file
   //
   if (brainSet.getNumberOfTopologyFiles() < 0) {
      std::cout << "ERROR: No topology file was read." << std::endl;
      std::exit(-1);
   }
   else if (brainSet.getNumberOfTopologyFiles() > 1) {
      std::cout << "ERROR: There is more than one topology file." << std::endl;
      std::exit(-1);
   }
   const TopologyFile* topoFile = brainSet.getTopologyFile(0);
   
   //
   // Verify surface has nodes
   //
   const int numNodes = brainSet.getNumberOfNodes();
   if (numNodes <= 0) {
      std::cout << "The surfaces contain no nodes." << std::endl;
      std::exit(-1);
   }
   
   //
   // Create a topology helper
   //
   const TopologyHelper* topologyHelper = topoFile->getTopologyHelper(false, true, false);
   
   //
   // Flag the nodes for the ROI
   //
   std::vector<bool> roiNodes(numNodes);
   for (int i = 0; i < numNodes; i++) {
      roiNodes[i] = false;
      if (topologyHelper->getNodeHasNeighbors(i)) {
         if (paintFile->getPaint(i, paintColumnNumber) == paintIndex) {
            roiNodes[i] = true;
         }
      }
   }

   //
   // Generate the report
   //   
   BrainModelSurfaceRegionOfInterest surfaceROI(&brainSet,
                                                brainSet.getBrainModelSurface(0),
                                                BrainModelSurfaceRegionOfInterest::OPERATION_SURFACE_XYZ_MEANS_REPORT,
                                                roiNodes);
   surfaceROI.setSurfaceMeansReportControlsAndOptions(coordFiles);
   try {
      surfaceROI.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::exit(-1);
   }
   
   //
   // File for text report
   //
   TextFile textReportFile;
   textReportFile.setText(surfaceROI.getReportText());

   //
   // Write the text file
   //
   try {
      textReportFile.writeFile(outputTextFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Smooth an identified area on a surface.
 */
static int
operationSurfaceSmoothArea(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE SMOOTH AREA", "-surface-smooth-area");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE SMOOTH AREA" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-smooth-area \\ " << std::endl
         << "         <f-strength> <i-iterations> <i-edge-iterations> \\ " << std::endl
         << "         <i-project-to-sphere-every-x-iterations> \\" << std::endl
         << "         <paint-file-name>  <paint-file-column-name> <paint-name> \\" << std::endl
         << "         <input-coord-file-name> <topology-file-name> <output-coord-file-name>" << std::endl
         << "         " << std::endl
         << "      Smooth a region of a surface (coord/topo pair) that is " << std::endl
         << "      identified by a column in a paint file.  " << std::endl
         << "      Parameters:" << std::endl
         << "         strength  - Percentage of smoothing applied to neighboring" << std::endl
         << "                     nodes.  Ranges from 0 to 1." << std::endl
         << "         iterations - Number of iterations for which to smooth." << std::endl
         << "         edge-iterations - smooth edges once every \"edge-iterations\"." << std::endl
         << "         project-to-sphere-every-x-iterations - Project the surface to" << std::endl
         << "                     a sphere every \"this many\" iterations.  Set to " << std::endl
         << "                     zero if the surface is not a sphere." << std::endl
         << "         paint-file-name - Name of the paint file that contains a " << std::endl
         << "                     column that identifies the nodes for smoothing." << std::endl
         << "         paint-file-column-name - Name of the paint file column." << std::endl
         << "         paint-name - Name in \"paint-file-column-name\" (eg. MEDIAL.WALL) " << std::endl
         << "                      that identifies the nodes that are to be smoothed." << std::endl
         << "         input-coord-file-name - Name of INPUT coordinate file." << std::endl
         << "         topology-file-name - Name of topology file." << std::endl
         << "         output-coord-file-name - Name of smoothed OUTPUT coordinate file." << std::endl
         << "         " << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   float strength;
   CommandLineUtilities::getNextParameter("Smoothing Strength",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          strength);

   int iterations;
   CommandLineUtilities::getNextParameter("Smoothing Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);
   
   int edgeIterations;
   CommandLineUtilities::getNextParameter("Edge Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          edgeIterations);
   
   int projectToSphereIterations;
   CommandLineUtilities::getNextParameter("Project to Sphere Every X Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          projectToSphereIterations);
   
   QString paintFileName;
   CommandLineUtilities::getNextParameter("Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintFileName);
   
   QString paintColumnName;
   CommandLineUtilities::getNextParameter("Paint Column Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintColumnName);
   
   QString paintName;
   CommandLineUtilities::getNextParameter("Paint Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintName);
   
   QString inputCoordFileName;
   CommandLineUtilities::getNextParameter("Input Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputCoordFileName);
   
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);
   
   QString outputCoordFileName;
   CommandLineUtilities::getNextParameter("Output Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputCoordFileName);
   
   
   //
   // Read the spec file into a brain set
   //
   SpecFile specFile;
   specFile.addToSpecFile(SpecFile::fiducialCoordFileTag,
                          inputCoordFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::closedTopoFileTag,
                          topoFileName,
                          "",
                          false);
   specFile.addToSpecFile(SpecFile::paintFileTag,
                          paintFileName,
                          "",
                          false);
                          
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   brainSet.setIgnoreTopologyFileInCoordinateFileHeaderFlag(true);
   if (brainSet.readSpecFile(specFile, "", errorMessage)) {
      std::cout << "ERROR: Reading spec file: " << std::endl
                << errorMessage.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Find the surface
   //
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: Cannot find surface in BrainSet from "
                << inputCoordFileName.toAscii().constData() << " and " << inputCoordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Find the paint file column with the proper name
   //
   PaintFile* paintFile = brainSet.getPaintFile();
   const int paintColumn = paintFile->getColumnWithName(paintColumnName);
   if (paintColumn < 0) {
      std::cout << "Unable to find a paint column named " << paintColumnName.toAscii().constData()
                << " in the file " << paintFileName.toAscii().constData() << "." << std::endl;
      exit(-1);
   }
   
   //
   // Find the index of the paint name
   //
   const int paintIndex = paintFile->getPaintIndexFromName(paintName);
   if (paintIndex < 0) {
      std::cout << "Unable to find a paint named " << paintName.toAscii().constData() << " in the paint file." << std::endl;
      exit(-1);
   }
   
   //
   // Verify surface has nodes
   //
   const int numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      std::cout << "The surface contains no nodes." << std::endl;
   }
   
   //
   // Flag the nodes that need to be smoothed
   //
   std::vector<bool> nodesToBeSmoothed(numNodes);
   bool nodeMatched = false;
   for (int i = 0; i < numNodes; i++) {
      nodesToBeSmoothed[i] = (paintFile->getPaint(i, paintColumn) == paintIndex);
      nodeMatched = true;
   }
   if (nodeMatched == false) {
      std::cout << "There are no nodes in the column " << paintColumnName.toAscii().constData()
                << " that are named " << paintName.toAscii().constData() << std::endl;
   }
   
   //
   // Smooth the surface
   //
   bms->arealSmoothing(strength,
                       iterations,
                       edgeIterations,
                       &nodesToBeSmoothed,
                       projectToSphereIterations);
   
   //
   // Write the coordinate file
   //
   CoordinateFile* outputCoordFile = bms->getCoordinateFile();
   try {
      outputCoordFile->writeFile(outputCoordFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Set a border file's link variability.
 */
static int
operationSurfaceBorderVariability(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE BORDER VARIABILITY", "-surface-border-variability");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE BORDER VARIABILITY" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-border-variability \\ " << std::endl
         << "         <input-border-file> <input-landmark-average-border-file> \\" << std::endl
         << "         <output-border-file> \\" << std::endl
         << "         " << std::endl
         << "      Resample the input borders so that they have the same number of " << std::endl
         << "      links as the border with the corresponding name in the input landmark " << std::endl
         << "      average border file.  In addition, the variability radius for the " << std::endl
         << "      borders will also be set." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputBorderFileName;
   CommandLineUtilities::getNextParameter("Input Border File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputBorderFileName);

   QString landmarkAverageBorderFileName;
   CommandLineUtilities::getNextParameter("Landmark Average Border File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          landmarkAverageBorderFileName);
   
   QString outputBorderFileName;
   CommandLineUtilities::getNextParameter("Output Border File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputBorderFileName);
   
   //
   // Read the input border file
   //   
   BorderFile inputBorderFile;
   try {
      inputBorderFile.readFile(inputBorderFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading input border file" << inputBorderFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Read the landmark average border file
   //   
   BorderFile landmarkAverageBorderFile;
   try {
      landmarkAverageBorderFile.readFile(landmarkAverageBorderFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading landmark average border file" << landmarkAverageBorderFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Resample and set variability
   //
   try {
      inputBorderFile.resampleToMatchLandmarkBorders(landmarkAverageBorderFile);
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Write the border file
   //
   try {
      inputBorderFile.writeFile(outputBorderFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems writing output border file" << outputBorderFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Nibble (remove) points from a border based upon distance to a surface extrema.
 */
static int
operationSurfaceBorderNibbler(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE BORDER NIBBLER", "-surface-border-nibbler");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE BORDER NIBBLER (remove border points within a distance to surface extrema)" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-border-nibbler \\ " << std::endl
         << "         <input-topo-file> <input-coord-file> <input-border-file> <border-name>" << std::endl
         << "         <output-border-file> <surface-extrema-name-or-node-indicator>" << std::endl
         << "          <surface-extrema-offset-or-node-number> <pos-neg>" << std::endl
         << "         " << std::endl
         << "      Remove points from the named border that are within that are on a . " << std::endl
         << "      specified side of a surface extrema or node number." << std::endl
         << "      " << std::endl
         << "      Surface-extrema-name-or-node-indicator is one of \"xmin\", \"xmax\", " << std::endl
         << "      \"ymin\", \"ymax\", \"zmin\", \"zmax\", \"node-x\", \"node-y\", or" << std::endl
         << "      or \"node-z\"." << std::endl
         << "      " << std::endl
         << "      Surface-extrema-offset-or-node-number is added to the X, Y, or Z of the" << std::endl
         << "       specified surface-extrema or is the number of a node." << std::endl
         << "      " << std::endl
         << "      <pos-neg> is either \"POS\" or \"NEG\" and indicates if border points" << std::endl
         << "      should be removed from the positive or negative side of the surface " << std::endl
         << "      extrema and offset or node number." << std::endl
         << "      " << std::endl
         << "      Suppose the surface extrema name/offset/pos-neg are \"ymin  20  NEG\" " << std::endl
         << "      and the surface's minimum Y-coordinate is -125.0.   Border points having" << std::endl
         << "      a Y-coordinate less than 105.0 (-125 + 20) are removed.  Changing the" << std::endl
         << "      plus-minus to POS would remove all border points with a Y-coordinate" << std::endl
         << "      greater than 105.0." << std::endl
         << "      " << std::endl
         << "      Suppose the node indicator/node number/pos-neg are \"node-x 25432 POS\"." << std::endl
         << "      Any border points with an X-coordinate greater than the X-coordinate of " << std::endl
         << "      node 25432 are removed." << std::endl
         << "      " << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   QString inputBorderFileName;
   CommandLineUtilities::getNextParameter("Input Border File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputBorderFileName);
   
   QString borderName;
   CommandLineUtilities::getNextParameter("Input Border Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          borderName);
   
   QString outputBorderFileName;
   CommandLineUtilities::getNextParameter("Output Border File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputBorderFileName);
   
   QString surfaceExtremaName;
   CommandLineUtilities::getNextParameter("Surface Extrema Name or Node Indicator",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          surfaceExtremaName);
   
   QString surfaceExtremaOffsetOrNodeNumber;
   CommandLineUtilities::getNextParameter("Surface Extrema Offset or Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          surfaceExtremaOffsetOrNodeNumber);
   const float surfaceExtremaOffset = surfaceExtremaOffsetOrNodeNumber.toFloat();
   const int nodeNumber = surfaceExtremaOffsetOrNodeNumber.toInt();
   
   QString posNeg;
   CommandLineUtilities::getNextParameter("pos-neg",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          posNeg);
   
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet(topoFileName, coordFileName);
   if (brainSet.getNumberOfBrainModels() <= 0) {
      std::cout << "ERROR: Failure creating the BrainSet from "
                << topoFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: Cannot find surface in BrainSet from "
                << coordFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Read the border file
   //   
   BorderFile borderFile;
   try {
      borderFile.readFile(inputBorderFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading border file" << inputBorderFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // set a scalar for plane normal direction
   //
   float normalScalar = 1.0;
   if (posNeg == "POS") {
      normalScalar = -1.0;
   }
   else if (posNeg == "NEG") {
      normalScalar = 1.0;
   }
   else {
      std::cout << "ERROR: Invalid pos-neg value: " << posNeg.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Get the bounds for the surface
   //
   float surfaceBounds[6];
   bms->getBounds(surfaceBounds);
   
   //
   // See if surface extrema mode
   //
   float nodeXYZ[3] = { 0.0, 0.0, 0.0 };
   if ((surfaceExtremaName == "xmin") ||
       (surfaceExtremaName == "xmax") ||
       (surfaceExtremaName == "ymin") ||
       (surfaceExtremaName == "ymax") ||
       (surfaceExtremaName == "zmin") ||
       (surfaceExtremaName == "zmax")) {
   }
   else if ((surfaceExtremaName == "node-x") ||
            (surfaceExtremaName == "node-y") ||
            (surfaceExtremaName == "node-z")) {
      const CoordinateFile* cf = bms->getCoordinateFile();
      if ((nodeNumber >= 0) &&
          (nodeNumber < cf->getNumberOfCoordinates())) {
         cf->getCoordinate(nodeNumber, nodeXYZ);
      }
      else {
         std::cout << "ERROR: Invalid node number " << nodeNumber << std::endl;
      }
   }
   else {
      std::cout << "ERROR: Invalid value for surface extrema or node indicator "
                << surfaceExtremaName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Set the normal vector and a point in the plane
   //
   float planeNormal[3], planePoint[3];
   if (surfaceExtremaName == "xmin") {
      planeNormal[0] = 0.0;
      planeNormal[1] = 0.0;
      planeNormal[2] = 0.0;
      planePoint[0] = surfaceBounds[0] + surfaceExtremaOffset;
      planePoint[1] = normalScalar;
      planePoint[2] = 0.0;
   }
   else if (surfaceExtremaName == "xmax") {
      planeNormal[0] = normalScalar;
      planeNormal[1] = 0.0;
      planeNormal[2] = 0.0;
      planePoint[0] = surfaceBounds[1] + surfaceExtremaOffset;
      planePoint[1] = 0.0;
      planePoint[2] = 0.0;
   }
   else if (surfaceExtremaName == "ymin") {
      planeNormal[0] = 0.0;
      planeNormal[1] = normalScalar;
      planeNormal[2] = 0.0;
      planePoint[0] = 0.0;
      planePoint[1] = surfaceBounds[2] + surfaceExtremaOffset;
      planePoint[2] = 0.0;
   }
   else if (surfaceExtremaName == "ymax") {
      planeNormal[0] = 0.0;
      planeNormal[1] = normalScalar;
      planeNormal[2] = 0.0;
      planePoint[0] = 0.0;
      planePoint[1] = surfaceBounds[3] + surfaceExtremaOffset;
      planePoint[2] = 0.0;
   }
   else if (surfaceExtremaName == "zmin") {
      planeNormal[0] = 0.0;
      planeNormal[1] = 0.0;
      planeNormal[2] = normalScalar;
      planePoint[0] = 0.0;
      planePoint[1] = 0.0;
      planePoint[2] = surfaceBounds[4] + surfaceExtremaOffset;
   }
   else if (surfaceExtremaName == "zmax") {
      planeNormal[0] = 0.0;
      planeNormal[1] = 0.0;
      planeNormal[2] = normalScalar;
      planePoint[0] = 0.0;
      planePoint[1] = 0.0;
      planePoint[2] = surfaceBounds[5] + surfaceExtremaOffset;
   }
   else if (surfaceExtremaName == "node-x") {
      planeNormal[0] = normalScalar;
      planeNormal[1] = 0.0;
      planeNormal[2] = 0.0;
      planePoint[0] = nodeXYZ[0];
      planePoint[1] = nodeXYZ[1];
      planePoint[2] = nodeXYZ[2];
   }
   else if (surfaceExtremaName == "node-y") {
      planeNormal[0] = 0.0;
      planeNormal[1] = normalScalar;
      planeNormal[2] = 0.0;
      planePoint[0] = nodeXYZ[0];
      planePoint[1] = nodeXYZ[1];
      planePoint[2] = nodeXYZ[2];
   }
   else if (surfaceExtremaName == "node-z") {
      planeNormal[0] = 0.0;
      planeNormal[1] = 0.0;
      planeNormal[2] = normalScalar;
      planePoint[0] = nodeXYZ[0];
      planePoint[1] = nodeXYZ[1];
      planePoint[2] = nodeXYZ[2];
   }
   else {
      std::cout << "ERROR: Invalid surface extrema name " << surfaceExtremaName.toAscii().constData() 
                << std::endl;
      exit(-1);
   }
   
   //
   // Loop through the borders
   //
   int bordersFound = 0;
   const int numBorders = borderFile.getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      //
      // Find the border
      //
      Border* border = borderFile.getBorder(i);
      if (border->getName() == borderName) {
         bordersFound++;
         border->removePointsOnNegativeSideOfPlane(planeNormal, planePoint);
      }
   }
   
   if (bordersFound > 0) {
      //
      // Write the border file
      //
      try {
         borderFile.writeFile(outputBorderFileName);
      }
      catch (FileException& e) {
         std::cout << "ERROR writing border file " << e.whatQString().toAscii().constData() << std::endl;
      }
   }
   else {
      std::cout << "ERROR: unable to find borders named \""
                << borderName.toAscii().constData()
                << "\" in the border file "
                << inputBorderFileName.toAscii().constData()
                << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Print the extrema for a surface, a named border, and the difference.
 */
static int
operationSurfaceBorderExtrema(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE BORDER EXTREMA", "-surface-border-extrema");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE BORDER EXTREMA" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-border-extrema \\ " << std::endl
         << "         <input-topo-file> <input-coord-file> <input-border-file> <border-name>" << std::endl
         << "         " << std::endl
         << "      List the extrema for the surface, the extrema for the border, and the " << std::endl
         << "      difference between the extrema. " << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   QString borderFileName;
   CommandLineUtilities::getNextParameter("Input Border File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          borderFileName);
   
   QString borderName;
   CommandLineUtilities::getNextParameter("Input Border Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          borderName);
   
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet(topoFileName, coordFileName);
   if (brainSet.getNumberOfBrainModels() <= 0) {
      std::cout << "ERROR: Failure creating the BrainSet from "
                << topoFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: Cannot find surface in BrainSet from "
                << coordFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Read the border file
   //   
   BorderFile borderFile;
   try {
      borderFile.readFile(borderFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading border file" << borderFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Get the bounds for the surface
   //
   float surfaceBounds[6];
   bms->getBounds(surfaceBounds);
   
   //
   // Loop through the borders
   //
   int bordersFound = 0;
   const int numBorders = borderFile.getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      //
      // Find the border
      //
      const Border* border = borderFile.getBorder(i);
      if (border->getName() == borderName) {
         if (bordersFound == 0) {
            //                     1         2         3         4         5         6
            //           0123456789012345678901234567890123456789012345678901234567890123456789
            std::cout << "Border: " << i << " " << border->getName().toAscii().constData() << std::endl;
            std::cout << "Extrema    Coordinate        Border    Difference" << std::endl;
         }
         bordersFound++;
         
         //
         // Get the bounds of the borders
         //
         float borderBounds[6];
         border->getBounds(borderBounds);
         
         //
         // Print the extremas
         //
         for (int i = 0; i < 6; i++) {
            QString legend;
            switch (i) {
               case 0:
                  legend = "x-min";
                  break;
               case 1:
                  legend = "x-max";
                  break;
               case 2:
                  legend = "y-min";
                  break;
               case 3:
                  legend = "y-max";
                  break;
               case 4:
                  legend = "z-min";
                  break;
               case 5:
                  legend = "z-max";
                  break;
            }
            
            const float diff = borderBounds[i] - surfaceBounds[i];
            std::cout << legend.rightJustified(7).toAscii().constData()
                      << QString::number(surfaceBounds[i], 'f', 2).rightJustified(14).toAscii().constData()
                      << QString::number(borderBounds[i], 'f', 2).rightJustified(14).toAscii().constData()
                      << QString::number(diff, 'f', 2).rightJustified(14).toAscii().constData()
                      << std::endl;
         }
      }
   }
   
   if (bordersFound == 0) {
      std::cout << "ERROR: unable to find borders named \""
                << borderName.toAscii().constData()
                << "\" in the border file "
                << borderFileName.toAscii().constData()
                << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Project a border file into a border projection file.
 */
static int
operationSurfaceBorderProjection(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE BORDER PROJECTION", "-surface-border-projection");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE BORDER PROJECTION" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-border-projection \\ " << std::endl
         << "         <input-topo-file> <input-coord-file> \\" << std::endl
         << "         <input-border-file>  <output-border-projection-file> \\" << std::endl
         << "         <output-spec-file>" << std::endl
         << "         " << std::endl
         << "      Project a border file to the surface (coord/topo pair)." << std::endl
         << "      The spec file is optional and, if supplied, the output border " << std::endl
         << "      projection file will be added to the spec file." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   QString borderFileName;
   CommandLineUtilities::getNextParameter("Input Border File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          borderFileName);
   
   QString borderProjectionFileName;
   CommandLineUtilities::getNextParameter("Output Border Projection File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          borderProjectionFileName);
   
   QString specFileName;
   if (argIndex < (argc - 1)) {
      CommandLineUtilities::getNextParameter("Output Spec File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             specFileName);
   }
   
   
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet(topoFileName, coordFileName);
   if (brainSet.getNumberOfBrainModels() <= 0) {
      std::cout << "ERROR: Failure creating the BrainSet from "
                << topoFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: Cannot find surface in BrainSet from "
                << coordFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Read the border file
   //   
   BorderFile borderFile;
   try {
      borderFile.readFile(borderFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading border file" << borderFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Create the border projection file
   //
   BorderProjectionFile borderProjectionFile;
   //
   // Create a border projector and project the borders
   //
   BorderFileProjector projector(bms, true);
   projector.projectBorderFile(&borderFile, &borderProjectionFile, NULL);
   
   //
   // Write the border projection file
   //
   try {
      borderProjectionFile.writeFile(borderProjectionFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems writing border projection file" << borderProjectionFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Update spec file
   //
   if (specFileName.isEmpty() == false) {
      addToSpecFileHelper(specFileName,
                          SpecFile::borderProjectionFileTag,
                          borderProjectionFileName);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Unproject a border projection file to a border file.
 */
static int
operationSurfaceBorderUnprojection(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE BORDER UNPROJECTION", "-surface-border-unprojection");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE BORDER UNPROJECTION" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-border-unprojection \\ " << std::endl
         << "         <input-coord-file> \\" << std::endl
         << "         <input-border-projectionfile>  <output-border-file> \\" << std::endl
         << "         <output-spec-file> <spec-file-tag>" << std::endl
         << "         " << std::endl
         << "      Unproject a border projection file to the coordinate file." << std::endl
         << "      The spec file is optional and, if supplied, the output border file" << std::endl
         << "      will be added to the spec file." << std::endl
         << std::endl
         << "      spec-file-tag should be one of the following border file tags:" << std::endl
         << "         " << SpecFile::rawBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::fiducialBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::inflatedBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::veryInflatedBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::sphericalBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::ellipsoidBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::compressedBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::flatBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::lobarFlatBorderFileTag.toAscii().constData() << std::endl
         << "         " << SpecFile::unknownBorderFileMatchTag.toAscii().constData() << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   QString borderProjectionFileName;
   CommandLineUtilities::getNextParameter("Input Border ProjectionFile",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          borderProjectionFileName);
   
   QString borderFileName;
   CommandLineUtilities::getNextParameter("Output Border File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          borderFileName);
   
   QString specFileName;
   QString specFileTag;
   if (argIndex < (argc - 2)) {
      CommandLineUtilities::getNextParameter("Output Spec File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             specFileName);

      CommandLineUtilities::getNextParameter("Spec File Tag",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             specFileTag);
   }
   
   //
   // Read the coordinate file
   //   
   CoordinateFile coordFile;
   try {
      coordFile.readFile(coordFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading border projectionfile" << coordFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   
   //
   // Read the projection border file
   //   
   BorderProjectionFile borderProjectionFile;
   try {
      borderProjectionFile.readFile(borderProjectionFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading border projectionfile" << borderProjectionFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Create the border file
   //
   BorderFile borderFile;
   //
   // Create a border unprojector and unproject the borders
   //
   BorderProjectionUnprojector unprojector;
   unprojector.unprojectBorderProjections(coordFile, borderProjectionFile, borderFile);
   
   //
   // Write the border file
   //
   try {
      borderFile.writeFile(borderFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems writing border file" << borderFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Update spec file
   //
   if (specFileName.isEmpty() == false) {
      addToSpecFileHelper(specFileName,
                          specFileTag,
                          borderFileName);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Project a cell file into a cell projection file.
 */
static int
operationSurfaceCellProjection(int argc, char* argv[], int argIndex, const bool cellFlag)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         if (cellFlag) {
            printBriefHelp("SURFACE CELL PROJECTION", "-surface-cell-projection");
         }
         else {
            printBriefHelp("SURFACE FOCI PROJECTION", "-surface-foci-projection");
         }
         return 0;
      }
      printSeparatorLine();
      
      if (cellFlag) {
         std::cout
            << "   SURFACE CELL PROJECTION" << std::endl
            << "      " << programName.toAscii().constData() << " -surface-cell-projection \\ " << std::endl
            << "         <input-topo-file> <input-coord-file> \\" << std::endl
            << "         <input-cell-file>  <output-cell-projection-file> \\" << std::endl
            << "         <b-onto-surface-flag>  <f-onto-surface-above-distance> \\" << std::endl
            << "         <output-spec-file>" << std::endl
            << "         " << std::endl
            << "      Project a cell file to the surface (coord/topo pair)." << std::endl
            << "      The spec file is optional and, if supplied, the output cell " << std::endl
            << "      projection file will be added to the spec file." << std::endl
            << std::endl
            << "      b-onto-surface-flag should be either \"true\" or \"false\".  If" << std::endl
            << "      it is true, cells will be projected so that they are " << std::endl
            << "      f-onto-surface-above-distance units above the surface.  If false" << std::endl
            << "      cells are projected so that they retain their surface offset." << std::endl
            << std::endl
            << std::endl;
      }
      else {
         std::cout
            << "   SURFACE FOCI PROJECTION" << std::endl
            << "      " << programName.toAscii().constData() << " -surface-foci-projection \\ " << std::endl
            << "         <input-topo-file> <input-coord-file> \\" << std::endl
            << "         <input-foci-file>  <output-foci-projection-file> \\" << std::endl
            << "         <b-onto-surface-flag>  <f-onto-surface-above-distance> \\" << std::endl
            << "         <output-spec-file>" << std::endl
            << "         " << std::endl
            << "      Project a foci file to the surface (coord/topo pair)." << std::endl
            << "      The spec file is optional and, if supplied, the output foci " << std::endl
            << "      projection file will be added to the spec file." << std::endl
            << std::endl
            << "      b-onto-surface-flag should be either \"true\" or \"false\".  If" << std::endl
            << "      it is true, foci will be projected so that they are " << std::endl
            << "      f-onto-surface-above-distance units above the surface.  If false" << std::endl
            << "      foci are projected so that they retain their surface offset." << std::endl
            << std::endl
            << std::endl;
      }
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   QString cellFileName;
   CommandLineUtilities::getNextParameter("Input Cell/Foci File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cellFileName);
   
   QString cellProjectionFileName;
   CommandLineUtilities::getNextParameter("Output Cell/Foci Projection File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cellProjectionFileName);
   
   bool ontoSurfaceFlag = false;
   CommandLineUtilities::getNextParameter("Onto Surface Flag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          ontoSurfaceFlag);
   
   float ontoSurfaceDistance = 0.0;
   CommandLineUtilities::getNextParameter("Onto Surface Above Distance",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          ontoSurfaceDistance);
   
   QString specFileName;
   if (argIndex < (argc - 1)) {
      CommandLineUtilities::getNextParameter("Output Spec File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             specFileName);
   }
   
   
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet(topoFileName, coordFileName);
   if (brainSet.getNumberOfBrainModels() <= 0) {
      std::cout << "ERROR: Failure creating the BrainSet from "
                << topoFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: Cannot find surface in BrainSet from "
                << coordFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Read the cell file
   //   
   CellFile* cellFile = NULL;
   if (cellFlag) {
      cellFile = new CellFile;
   }
   else {
      cellFile = new FociFile;
   }
   try {
      cellFile->readFile(cellFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading cell/foci file " << cellFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Create the cell projection file
   //
   CellProjectionFile* cellProjectionFile = NULL;
   if (cellFlag) {
      cellProjectionFile = new CellProjectionFile;
   }
   else {
      cellProjectionFile = new FociProjectionFile;
   }
   cellProjectionFile->append(*cellFile);
   
   //
   // Create a cell projector and project the cells
   //
   CellFileProjector projector(bms);
   projector.projectFile(cellProjectionFile,
                             0,
                             CellFileProjector::PROJECTION_TYPE_ALL,
                             ontoSurfaceDistance,
                             ontoSurfaceFlag,
                             NULL);
   //
   // Write the cell projection file
   //
   try {
      cellProjectionFile->writeFile(cellProjectionFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems writing cell/foci projection file" << cellProjectionFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Update spec file
   //
   if (specFileName.isEmpty() == false) {
      if (cellFlag) {
         addToSpecFileHelper(specFileName,
                             SpecFile::cellProjectionFileTag,
                             cellProjectionFileName);
      }
      else {
         addToSpecFileHelper(specFileName,
                             SpecFile::fociProjectionFileTag,
                             cellProjectionFileName);
      }
   }
   
   delete cellFile;
   delete cellProjectionFile;
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Unproject a cell projection file to a cell file.
 */
static int
operationSurfaceCellUnprojection(int argc, char* argv[], int argIndex, const bool cellFlag)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         if (cellFlag) {
            printBriefHelp("SURFACE CELL UNPROJECTION", "-surface-cell-unprojection");
         }
         else {
            printBriefHelp("SURFACE FOCI UNPROJECTION", "-surface-foci-unprojection");
         }
         return 0;
      }
      printSeparatorLine();
      if (cellFlag) {
         std::cout
            << "   SURFACE CELL UNPROJECTION" << std::endl
            << "      " << programName.toAscii().constData() << " -surface-cell-unprojection \\ " << std::endl
            << "         <input-topo-file> <input-coord-file> \\" << std::endl
            << "         <input-cell-projection-file>  <output-cell-file> \\" << std::endl
            << "         <output-spec-file>" << std::endl
            << "         " << std::endl
            << "      Unproject a cell projection file to the coordinate file." << std::endl
            << "      The spec file is optional and, if supplied, the output cell file" << std::endl
            << "      will be added to the spec file as a fiducial cell file." << std::endl
            << std::endl
            << std::endl;
      }
      else {
         std::cout
            << "   SURFACE FOCI UNPROJECTION" << std::endl
            << "      " << programName.toAscii().constData() << " -surface-foci-unprojection \\ " << std::endl
            << "         <input-topo-file> <input-coord-file> \\" << std::endl
            << "         <input-foci-projection-file>  <output-foci-file> \\" << std::endl
            << "         <output-spec-file> " << std::endl
            << "         " << std::endl
            << "      Unproject a foci projection file to the coordinate file." << std::endl
            << "      The spec file is optional and, if supplied, the output foic file" << std::endl
            << "      will be added to the spec file as a fiducial foci file." << std::endl
            << std::endl
            << std::endl;
      }
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   QString cellProjectionFileName;
   CommandLineUtilities::getNextParameter("Input Cell/Foci ProjectionFile",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cellProjectionFileName);
   
   QString cellFileName;
   CommandLineUtilities::getNextParameter("Output Cell/Foci File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          cellFileName);
   
   QString specFileName;
   QString specFileTag;
   if (argIndex < (argc - 1)) {
      CommandLineUtilities::getNextParameter("Output Spec File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             specFileName);

   }
   
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet(topoFileName, coordFileName);
   if (brainSet.getNumberOfBrainModels() <= 0) {
      std::cout << "ERROR: Failure creating the BrainSet from "
                << topoFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: Cannot find surface in BrainSet from "
                << coordFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }   
   
   //
   // Read the projection cell file
   //   
   CellProjectionFile cellProjectionFile;
   try {
      cellProjectionFile.readFile(cellProjectionFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems reading cell/foci projectionfile" << cellProjectionFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Create the cell file
   //
   CellFile* cellFile = NULL;
   if (cellFlag) {
      cellFile = new CellFile;
   }
   else {
      cellFile = new FociFile;
   }
   
   //
   // Create a cell unprojector and unproject the cells
   //
   CellProjectionUnprojector unprojector;
   unprojector.unprojectCellProjections(cellProjectionFile, bms, *cellFile, 0);
   
   //
   // Write the cell file
   //
   try {
      cellFile->writeFile(cellFileName);
      delete cellFile;
   }
   catch (FileException& e) {
      std::cout << "ERROR: problems writing cell/foci file" << cellFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Update spec file
   //
   if (specFileName.isEmpty() == false) {
      if (cellFlag) {
         addToSpecFileHelper(specFileName,
                             SpecFile::cellFileTag,
                             cellFileName);
      }
      else {
         addToSpecFileHelper(specFileName,
                             SpecFile::fociFileTag,
                             cellFileName);
      }
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Render a surface to an image.
 */
static int
operationShowSurface(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SHOW SURFACE", "-show-surface");
         return 0;
      }
      printSeparatorLine();
         std::cout
            << "   SHOW SURFACE" << std::endl
            << "      " << programName.toAscii().constData() << " -show-surface \\ " << std::endl
            << "         <topo-file> <coord-file> <view> \\" << std::endl
            << "         <i-image-width> <i-image-height> [image-file-name]" << std::endl
            << "         " << std::endl
            << "      Render an image of the surface into an image file or to the user's display." << std::endl
            << std::endl
            << "      view is one of:  " << std::endl
            << "         A    => anterior view" << std::endl
            << "         ALL  => all size views in one image" << std::endl
            << "         D    => dorsal view" << std::endl
            << "         L    => lateral view" << std::endl
            << "         M    => medial view" << std::endl
            << "         P    => posterior view" << std::endl
            << "         V    => ventral view" << std::endl
            << "         " << std::endl
            << std::endl
            << "      NOTE: If the image file name is not specified, the image of the surface" << std::endl
            << "      will be shown in a window on the user's display." << std::endl
            << std::endl;
            return 0;
   }
   
   const int MAIN_WINDOW_INDEX = 0;
   
   int imageWidth = 512;
   int imageHeight = 512;
   
   //
   // Get the parameters
   //
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString coordFileName;
   CommandLineUtilities::getNextParameter("Input Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          coordFileName);
   
   //
   // Get the view
   //
   QString viewName;
   CommandLineUtilities::getNextParameter("View",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          viewName);
   BrainModel::STANDARD_VIEWS view;
   if (viewName == "L") {
      view = BrainModel::VIEW_LATERAL;
   }
   else if (viewName == "M") {
      view = BrainModel::VIEW_MEDIAL;
   } 
   else if (viewName == "D") {
      view = BrainModel::VIEW_DORSAL;
   } 
   else if (viewName == "V") {
      view = BrainModel::VIEW_VENTRAL;
   } 
   else if (viewName == "A") {
      view = BrainModel::VIEW_ANTERIOR;
   } 
   else if (viewName == "P") {
      view = BrainModel::VIEW_POSTERIOR;
   } 
   else if (viewName == "ALL") {
      view = BrainModel::VIEW_NONE;
   }
   else {
      std::cout << "ERROR: Invalid view \"" << viewName.toAscii().constData() << "\"." << std::endl;
   }
   
   //
   // Get the image X size
   //
   CommandLineUtilities::getNextParameter("Image Width",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          imageWidth);
   if (imageWidth <= 0) {
      std::cout << "ERROR: Invalid image width = " << imageWidth << std::endl;
      exit(-1);
   }
   
   
   //
   // Get the image Y size
   //
   CommandLineUtilities::getNextParameter("Image Height",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          imageHeight);
   if (imageHeight <= 0) {
      std::cout << "ERROR: Invalid image height = " << imageHeight << std::endl;
      exit(-1);
   }

   bool saveImageToFile = false;
   QString imageFileName;
   if (argc > 7) {
      saveImageToFile = true;
      
      //
      // Image file name
      //
      CommandLineUtilities::getNextParameter("Image File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             imageFileName);      
   }
   
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet(topoFileName, coordFileName);
   if (brainSet.getNumberOfBrainModels() <= 0) {
      std::cout << "ERROR: Failure creating the BrainSet from "
                << topoFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: Cannot find surface in BrainSet from "
                << coordFileName.toAscii().constData() << " and " << coordFileName.toAscii().constData() << std::endl;
      exit(-1);
   }   
   
   //
   // setup the off screen renderer
   //
   QImage image;
   OffScreenOpenGLWidget opengl;
   
   //
   // "none" is all views in one image
   //
   if (view == BrainModel::VIEW_NONE) {
      const int xSize = imageWidth / 2;
      const int ySize = imageHeight / 3;
      opengl.setFixedSize(xSize, ySize);
      
      //
      // Render the surface into size images
      //
      QImage a, d, l, m, p, v;
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_ANTERIOR);
      opengl.drawToImage(&brainSet, bms, a);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_DORSAL);
      opengl.drawToImage(&brainSet, bms, d);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_MEDIAL);
      opengl.drawToImage(&brainSet, bms, m);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_LATERAL);
      opengl.drawToImage(&brainSet, bms, l);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_POSTERIOR);
      opengl.drawToImage(&brainSet, bms, p);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_VENTRAL);
      opengl.drawToImage(&brainSet, bms, v);
      
      //
      // Assemble the images into one image
      //
      image = QImage(imageWidth, imageHeight, a.format());
      for (int i = 0; i < xSize; i++) {
         for (int j = 0; j < ySize; j++) {
            image.setPixel(i, j, l.pixel(i, j));
            image.setPixel(i + xSize, j, m.pixel(i, j));
            image.setPixel(i, j + ySize, d.pixel(i, j));
            image.setPixel(i + xSize, j + ySize, v.pixel(i, j));
            image.setPixel(i, j + 2*ySize, a.pixel(i, j));
            image.setPixel(i + xSize, j + 2*ySize, p.pixel(i, j));
         }
      }
      
   }
   else {
      opengl.setFixedSize(imageWidth, imageHeight);
      bms->setToStandardView(MAIN_WINDOW_INDEX, view);
   
      //
      // Render the image
      //
      opengl.drawToImage(&brainSet,
                         bms,
                         image);
   }
                      
   //
   // save the image file
   //
   if (saveImageToFile) {
      if (image.save(imageFileName, "jpg") == false) {
         std::cout << "ERROR: Unable to write image." << std::endl;
      }
   }
   else {
      return displayQImage(image);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Render a surface to an image.
 */
static int
operationShowVolume(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SHOW VOLUME", "-show-volume");
         return 0;
      }
      printSeparatorLine();
         std::cout
            << "   SHOW VOLUME" << std::endl
            << "      " << programName.toAscii().constData() << " -show-volume \\ " << std::endl
            << "         <axis> <slice> <volume-file-name> \\" << std::endl
            << "         <i-image-width> <i-image-height> [image-file-name]" << std::endl
            << "         " << std::endl
            << "      Render an image of the surface into an image file or to the user's display." << std::endl
            << std::endl
            << "      axis is one of:  " << std::endl
            << "         X    => x-axis (parasagittal view)" << std::endl
            << "         Y    => y-axis (coronal view)" << std::endl
            << "         Z    => z-axis (horizontal/axial view)" << std::endl
            << "         " << std::endl
            << std::endl
            << "      NOTE: If the image file name is not specified, the image of the volume" << std::endl
            << "      will be shown in a window on the user's display." << std::endl
            << std::endl;
      return 0;
   }
   
   const int MAIN_WINDOW_INDEX = 0;
   
   int imageWidth = 512;
   int imageHeight = 512;
   
   //
   // Get the parameters
   //
   VolumeFile::VOLUME_AXIS axis = getAxisFromParameters(argc, argv, argIndex);
   
   int sliceNumber;
   CommandLineUtilities::getNextParameter("Slice Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sliceNumber);

   QString volumeFileName;
   CommandLineUtilities::getNextParameter("Volume File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          volumeFileName);
   
      
   //
   // Get the image X size
   //
   CommandLineUtilities::getNextParameter("Image Width",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          imageWidth);
   if (imageWidth <= 0) {
      std::cout << "ERROR: Invalid image width = " << imageWidth << std::endl;
      exit(-1);
   }
   
   
   //
   // Get the image Y size
   //
   CommandLineUtilities::getNextParameter("Image Height",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          imageHeight);
   if (imageHeight <= 0) {
      std::cout << "ERROR: Invalid image height = " << imageHeight << std::endl;
      exit(-1);
   }

   //
   // Image file name
   //
   bool saveImageToFile = false;
   QString imageFileName;
   if (argc > 7) {
      saveImageToFile = true;
      CommandLineUtilities::getNextParameter("Image File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             imageFileName);
   }
   
   //
   // Add the volume to a spec file
   //
   SpecFile sf;
   sf.addToSpecFile(SpecFile::volumeAnatomyFileTag,
                    volumeFileName,
                    "",
                    false);
                    
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet;
   QString errorMessage;
   if (brainSet.readSpecFile(sf,
                             "",
                             errorMessage)) {
      std::cout << "ERROR reading volume file: " << errorMessage.toAscii().constData() << std::endl;
      exit(-1);
   }
   BrainModelVolume* bmv = brainSet.getBrainModelVolume();
   if (bmv == NULL) {
      std::cout << "ERROR: Cannot find volume in BrainSet.  Problem with volume file? " << std::endl;
      exit(-1);
   }   
   
   //
   // Set the underlay to anatomy
   //
   brainSet.getVoxelColoring()->setUnderlay(BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY);
   
   //
   // Set the view
   //
   bmv->setSelectedAxis(MAIN_WINDOW_INDEX, axis);
   
   //
   // Set the slices
   //
   int slices[3];
   bmv->setToStandardView(MAIN_WINDOW_INDEX, BrainModelVolume::VIEW_RESET);
   bmv->getSelectedOrthogonalSlices(MAIN_WINDOW_INDEX, slices);
   switch (axis) {
      case VolumeFile::VOLUME_AXIS_X:
         slices[0] = sliceNumber;
         break;
      case VolumeFile::VOLUME_AXIS_Y:
         slices[1] = sliceNumber;
         break;
      case VolumeFile::VOLUME_AXIS_Z:
         slices[2] = sliceNumber;
         break;
      default:
         break;
   }
   bmv->setSelectedOrthogonalSlices(MAIN_WINDOW_INDEX, slices);
   
   //
   // Remove crosshairs
   //
   DisplaySettingsVolume* dsv = brainSet.getDisplaySettingsVolume();
   dsv->setDisplayCrosshairCoordinates(false);
   dsv->setDisplayCrosshairs(false);
   dsv->setDisplayOrientationLabels(false);
   
   //
   // setup the off screen renderer
   //
   QImage image;
   OffScreenOpenGLWidget opengl;
   
   opengl.setFixedSize(imageWidth, imageHeight);

   //
   // Render the image
   //
   opengl.drawToImage(&brainSet,
                      bmv,
                      image);
                      
   //
   // Write the image file
   //
   if (saveImageToFile) {
      if (image.save(imageFileName, "jpg") == false) {
         std::cout << "ERROR: Unable to write image." << std::endl;
      }
   }
   else {
      return displayQImage(image);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Render a surface to an image.
 */
static int
operationShowScene(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SHOW SCENE", "-show-scene");
         return 0;
      }
      printSeparatorLine();
         std::cout
            << "   SHOW SCENE" << std::endl
            << "      " << programName.toAscii().constData() << " -show-scene \\ " << std::endl
            << "         <spec-file> <scene-file> <i-scene-number> \\" << std::endl
            << "         [image-file-name]" << std::endl
            << "         " << std::endl
            << "      Render the images of a scene into an image file." << std::endl
            << "      Note: The scene numbers start at one.   " << std::endl
            << "         " << std::endl
            << std::endl
            << "      NOTE: If the image file name is not specified, the image of the scene" << std::endl
            << "      will be shown in a window on the user's display." << std::endl
            << std::endl;
      return 0;
   }
   
   int imageWidth = 512;
   int imageHeight = 512;
   
   //
   // Get the spec file name
   //
   QString specFileName;
   CommandLineUtilities::getNextParameter("Spec File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);

   //
   // Get the scene file name
   //
   QString sceneFileName;
   CommandLineUtilities::getNextParameter("Scene File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sceneFileName);
   
   //
   // Get the scene number
   //
   int sceneNumber;
   CommandLineUtilities::getNextParameter("Scene Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sceneNumber);
   
   //
   // Save image to a file ??
   //
   bool saveImageToFile = false;
   QString imageFileName;
   if (argc > 5) {
      //
      // Image file name
      //
      CommandLineUtilities::getNextParameter("Image File Name",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             imageFileName);
      saveImageToFile = true;
   }
      
   //
   // Read in the spec file
   //
   SpecFile specFile;
   try {
      specFile.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Deselect all files in spec file and add scene file
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::sceneFileTag,
                          sceneFileName,
                          "",
                          false);
                          
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet;
   if (brainSet.readSpecFile(specFile,
                             specFileName,
                             errorMessage)) {
      std::cout << "ERROR reading spec file " << specFileName.toAscii().constData() << std::endl
                << errorMessage.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Get the scene file
   //   
   SceneFile* sceneFile = brainSet.getSceneFile();
   const int numScenes = sceneFile->getNumberOfScenes();
   if (numScenes <= 0) {
      std::cout << "ERROR: Scene file contains no scenes." << std::endl;
      exit(-1);
   }
   if ((sceneNumber < 1) ||
       (sceneNumber > numScenes)) {
      std::cout << "ERROR: Invalid scene number: " << sceneNumber << std::endl;
      std::cout << "   Valid Scene Numbers range from 1 to " << numScenes << std::endl;
      exit(-1);
   }
   
   //
   // Users enter scene numbers 1 to N but C++ indexes 0 to N-1
   //
   sceneNumber--;
   
   //
   // Show number of scene
   //
   std::cout << "Showing scene num=" << (sceneNumber + 1)
             <<": " << sceneFile->getScene(sceneNumber)->getName().toAscii().constData() << std::endl;

   //
   // Setup the scene
   //
   QString sceneErrorMessage;
   SceneFile::Scene* scene = sceneFile->getScene(sceneNumber);
   brainSet.showScene(scene,
                      false,
                      sceneErrorMessage);
       
   //
   // The output image file
   //
   ImageFile outputImageFile;
   
   //
   // Loop through main and viewing windows
   //
   for (int windowNumber = 0; windowNumber < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; windowNumber++) {
      //
      // Get the brain model for the main window in the scene
      //
      QString modelError;
      int geometry[4];
      int glWidthWidthHeight[2];
      BrainModel* brainModel = brainSet.showSceneGetBrainModel(scene,
                                                               windowNumber,
                                                               geometry,
                                                               glWidthWidthHeight,
                                                               modelError);
      //
      // Was there a brain window in "window number" window
      //
      if (brainModel == NULL) {
         continue;
      }
      
      //
      // Use scene window size for image size
      // note that height includes the toolbar and window title bar so shrink height some
      //
      imageWidth  = geometry[2];
      imageHeight = geometry[3];
      if ((glWidthWidthHeight[0] > 0) && 
          (glWidthWidthHeight[1] > 0)) {
         imageWidth  = glWidthWidthHeight[0];
         imageHeight = glWidthWidthHeight[1];
      }
      else {
         if (imageHeight > 200) {
            imageHeight -= 100;
         }
      }
      
      //
      // setup the off screen renderer
      //
      QImage image;
      OffScreenOpenGLWidget opengl;
      opengl.setFixedSize(imageWidth, imageHeight);

      //
      // Render the image
      //
      opengl.drawToImage(&brainSet,
                         brainModel,
                         image);
                      
      if (sceneErrorMessage.isEmpty() == false) {
         std::cout << "Scene Message: " << sceneErrorMessage.toAscii().constData() << std::endl;
      }
      
      //
      // Add to the bottom of the output image file
      //
      try {
         outputImageFile.appendImageAtBottom(ImageFile(image));
      }
      catch (FileException& e) {
         std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
         exit(-1);
      }
   }
   
   //
   // Write the image file
   //
   if (saveImageToFile) {
      if (outputImageFile.getImage()->save(imageFileName, "jpg") == false) {
         std::cout << "ERROR: Unable to write image." << std::endl;
         exit(-1);
      }
   }
   else {
      return displayQImage(*outputImageFile.getImage());
   }

   return 0;
}

/*----------------------------------------------------------------------------------------
 * Read in BrainSets for applying a deformation map to a data file.
 */
static bool
readBrainSetsForDeformation(BrainSet& sourceBrainSet,
                            BrainSet& targetBrainSet,
                            DeformationMapFile& deformationMapFile,
                            QString& errorMessage)
{
   errorMessage = ""; 
   
   const QString savedDirectory(QDir::currentPath());
   
   //
   // create source brain set
   //
   bool sourceSpecMissing = true;
   SpecFile sourceSpecFile;
   try {
      QString specFileName;
      if (deformationMapFile.getFileVersion() >= 2) {
         specFileName.append(deformationMapFile.getSourceDirectory());
         specFileName.append("/");
      }
      specFileName.append(deformationMapFile.getSourceSpecFileName());
      sourceSpecFile.readFile(specFileName);
      sourceSpecMissing = false;
   }
   catch (FileException& e) {
      //
      // David has a bad habit of renaming spec files, so just hope the
      // data files are still the same name and in the same location.
      //
      QDir::setCurrent(deformationMapFile.getSourceDirectory());
      //errorMessage = e.whatQString().toAscii().constData());
      //return true;
   }
   
   //
   // Select the deformation files
   //
   sourceSpecFile.setDeformationSelectedFiles(
         deformationMapFile.getSourceClosedTopoFileName(),
         deformationMapFile.getSourceCutTopoFileName(),
         deformationMapFile.getSourceFiducialCoordFileName(),
         deformationMapFile.getSourceSphericalCoordFileName(),
         deformationMapFile.getSourceFlatCoordFileName(),
         "",
         sourceSpecMissing,
         sourceSpecFile.getStructure());
   
   //
   // Read in the source brain set
   //
   std::vector<QString> errorMessages;
   sourceBrainSet.readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                sourceSpecFile,
                                deformationMapFile.getSourceSpecFileName(),
                                errorMessages, NULL, NULL);
   if (errorMessages.empty() == false) {
      if (sourceSpecMissing) {
         errorMessages.push_back("Source spec file was not found.\n"
                                 "Tried to load data files.\n");
      }
      errorMessage = StringUtilities::combine(errorMessages, "\n");
      return true;
   }
   
   //
   // Read in the deformed coordinate file
   //
   if (deformationMapFile.getInverseDeformationFlag() == false) {
      QString coordFileName;
      BrainModelSurface::SURFACE_TYPES surfaceType =
                         BrainModelSurface::SURFACE_TYPE_UNKNOWN;
      switch(deformationMapFile.getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            coordFileName = 
               deformationMapFile.getSourceDeformedFlatCoordFileName();
            surfaceType = BrainModelSurface::SURFACE_TYPE_FLAT;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
            coordFileName = 
               deformationMapFile.getSourceDeformedSphericalCoordFileName();
            surfaceType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
            break;
      }
      if (coordFileName.isEmpty()) {
         errorMessage = "Deformed source coordinate file is missing.";
         return true;
      }
      try {
         sourceBrainSet.readCoordinateFile(coordFileName,
                                            surfaceType,
                                            false,
                                            true,
                                            false);
      }
      catch (FileException& e) {
         errorMessage = e.whatQString().toAscii().constData();
         return true;
      }
   }
   
   QDir::setCurrent(savedDirectory);
   
   //
   // Read in the target spec file
   //
   bool targetSpecMissing = true;
   SpecFile targetSpecFile;
   try {
      QString specFileName;
      if (deformationMapFile.getFileVersion() >= 2) {
         specFileName.append(deformationMapFile.getTargetDirectory());
         specFileName.append("/");
      }
      specFileName.append(deformationMapFile.getTargetSpecFileName());
      targetSpecFile.readFile(specFileName);
      targetSpecMissing = false;
   }
   catch (FileException& e) {
      //
      // David has a bad habit of renaming spec files, so just hope the
      // data files are still the same name and in the same location.
      //
      QDir::setCurrent(deformationMapFile.getSourceDirectory());
      //errorMessage = e.whatQString().toAscii().constData());
      //return true;
   }
   
   //
   // Select the deformation files
   //
   targetSpecFile.setDeformationSelectedFiles(
         deformationMapFile.getTargetClosedTopoFileName(),
         deformationMapFile.getTargetCutTopoFileName(),
         deformationMapFile.getTargetFiducialCoordFileName(),
         deformationMapFile.getTargetSphericalCoordFileName(),
         deformationMapFile.getTargetFlatCoordFileName(),
         "",
         targetSpecMissing,
         targetSpecFile.getStructure());
   
   //
   // Read in the target brain set
   //
   errorMessages.clear();
   targetBrainSet.readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                                targetSpecFile,
                                deformationMapFile.getTargetSpecFileName(),
                                errorMessages, NULL, NULL);
   if (errorMessages.empty() == false) {
      if (targetSpecMissing) {
         errorMessages.push_back("Target spec file was not found.\n"
                                 "Tried to load data files.\n");
      }
      errorMessage = StringUtilities::combine(errorMessages, "\n");
      return true;
   }
   
   //
   // Read in the deformed coordinate file
   //
   if (deformationMapFile.getInverseDeformationFlag()) {
      QString coordFileName;
      BrainModelSurface::SURFACE_TYPES surfaceType =
                         BrainModelSurface::SURFACE_TYPE_UNKNOWN;
      switch(deformationMapFile.getFlatOrSphereSelection()) {
         case DeformationMapFile::DEFORMATION_TYPE_FLAT:
            coordFileName = 
               deformationMapFile.getSourceDeformedFlatCoordFileName();
            surfaceType = BrainModelSurface::SURFACE_TYPE_FLAT;
            break;
         case DeformationMapFile::DEFORMATION_TYPE_SPHERE:
            coordFileName = 
               deformationMapFile.getSourceDeformedSphericalCoordFileName();
            surfaceType = BrainModelSurface::SURFACE_TYPE_SPHERICAL;
            break;
      }
      if (coordFileName.isEmpty()) {
         errorMessage = "Deformed source coordinate file is missing.";
         return true;
      }
      try {
         targetBrainSet.readCoordinateFile(coordFileName,
                                            surfaceType,
                                            false,
                                            true,
                                            false);
      }
      catch (FileException& e) {
         errorMessage = e.whatQString().toAscii().constData();
         return true;
      }
   }
   
   QDir::setCurrent(savedDirectory);

   return false;
}

/*----------------------------------------------------------------------------------------
 * Apply a deformation map to a data file.
 */
static int
operationSurfaceDeformationApply(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE APPLY DEFORMATION MAP", "-surface-deformation-apply");
         return 0;
      }
      printSeparatorLine();
      
      std::cout
         << "   SURFACE APPLY DEFORMATION MAP" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-deformation-apply \\ " << std::endl
         << "         <deformation-map-file> <file-type> \\" << std::endl
         << "         <input-file>  <output-file> \\" << std::endl
         << "         [source-topo-file]  [source-deformed-topo-file] \\" << std::endl
         << "         [atlas-topo-file]" << std::endl
         << "         " << std::endl
         << "      Deform a data file." << std::endl
         << std::endl
         << "      Note: \"source-topo-file\", \"source-deformed-topo-file\" and " << std::endl
         << "      \"atlas-topo-file\" are only required when deforming" << std::endl
         << "      coordinate files." << std::endl
         << std::endl
         << "      \"file-type\" is one of: " << std::endl
         << "         AREAL_ESTIMATION" << std::endl
         << "         BORDER_FLAT" << std::endl
         << "         BORDER_PROJECTION" << std::endl
         << "         BORDER_SPHERICAL" << std::endl
         << "         CELL" << std::endl
         << "         CELL_PROJECTION" << std::endl
         << "         COORDINATE" << std::endl
         << "         COORDINATE_FLAT" << std::endl
         << "         FOCI" << std::endl
         << "         FOCI_PROJECTION" << std::endl
         << "         LAT_LON" << std::endl
         << "         METRIC" << std::endl
         << "         PAINT" << std::endl
         << "         PROB_ATLAS" << std::endl
         << "         RGB_PAINT" << std::endl
         << "         SURFACE_SHAPE" << std::endl
         << "         TOPOGRAPHY" << std::endl
         << std::endl
         << std::endl;
         
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString deformationMapFileName;
   CommandLineUtilities::getNextParameter("Deformation Map File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          deformationMapFileName);

   QString fileType;
   CommandLineUtilities::getNextParameter("File Type",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fileType);
   
   QString dataFileName;
   CommandLineUtilities::getNextParameter("Input File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          dataFileName);
   
   QString deformedFileName;
   CommandLineUtilities::getNextParameter("Output File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          deformedFileName);
  
   QString indivTopoFileName;
   QString indivDeformTopoFileName;
   QString atlasTopoFileName;
   if (fileType == "COORDINATE-FLAT") {
      CommandLineUtilities::getNextParameter("Source Topo File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivTopoFileName);
     
      CommandLineUtilities::getNextParameter("Source Deformed Topo File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             indivDeformTopoFileName);
     
      CommandLineUtilities::getNextParameter("Atlas Topo File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             atlasTopoFileName);
     
   }   
   
   //
   // Get file type
   //
   BrainModelSurfaceDeformDataFile::DATA_FILE_TYPE dft;
   if (fileType == "AREAL_ESTIMATION") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION;
   }
   else if (fileType == "BORDER_FLAT") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT;
   }
   else if (fileType == "BORDER_SPHERICAL") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL;
   }
   else if (fileType == "BORDER_PROJECTION") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION;
   }
   else if (fileType == "CELL") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_CELL;
   }
   else if (fileType == "CELL_PROJECTION") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION;
   }
   else if (fileType == "COORDINATE") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE;
   }
   else if (fileType == "COORDINATE_FLAT") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT;
   }
   else if (fileType == "FOCI") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI;
   }
   else if (fileType == "FOCI_PROJECTION") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION;
   }
   else if (fileType == "LAT_LON") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON;
   }
   else if (fileType == "METRIC") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC;
   }
   else if (fileType == "PAINT") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT;
   }
   else if (fileType == "PROB_ATLAS") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS;
   }
   else if (fileType == "RGB_PAINT") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT;
   }
   else if (fileType == "SURFACE_SHAPE") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE;
   }
   else if (fileType == "TOPOGRAPHY") {
      dft = BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY;
   }
   else {
      QString msg("Invalid file type: \"");
      msg.append(fileType);
      msg.append("\"\n");
      std::cout << msg.toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Read the deformation map file
   //
   DeformationMapFile deformationMapFile;
   try {
      deformationMapFile.readFile(deformationMapFileName);
   }
   catch (FileException& e) {
      QString msg("ERROR: Unable to read: ");
      msg += deformationMapFileName;
      msg += "\n";
      msg += e.whatQString().toAscii().constData();
      std::cout << msg.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   if (deformationMapFile.getFileVersion() >= 2) {      
      QString msg;
      if (QFile::exists(deformationMapFile.getSourceDirectory()) == false) {
         msg.append("Individual directory is invalid.  Change the\n"
                    "individual directory to the directory containing\n"
                    "the individual spec file.");
      }
      if (QFile::exists(deformationMapFile.getTargetDirectory()) == false) {
         msg.append("Atlas directory is invalid.  Change the\n"
                    "atlas directory to the directory containing\n"
                    "the atlas spec file.");
      }
      if (msg.isEmpty() == false) {
         std::cout << msg.toAscii().constData() << std::endl;
         exit(-1);
      }
   }
   
   //
   // Read the brain sets if needed
   //
   BrainSet sourceBrainSet, targetBrainSet;
   switch(dft) {
      case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
         break;
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
      case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
         {
            QString errorMessage;
            if (readBrainSetsForDeformation(sourceBrainSet,
                                            targetBrainSet,
                                            deformationMapFile,
                                            errorMessage)) {
               std::cout << "Deform Error: " << errorMessage.toAscii().constData() << std::endl;
               exit(-1);
            }
         }
   }
 
   if (deformationMapFile.getFileVersion() >= 2) {
      //
      // Prepend with path
      //
      QString name(deformationMapFile.getTargetDirectory());
      name.append("/");
      name.append(deformationMapFile.getOutputSpecFileName());
      deformationMapFile.setOutputSpecFileName(name);
   }

   //
   // set metric deformation
   //
   //if (metricNearestNodeRadioButton->isChecked()) {
      deformationMapFile.setMetricDeformationType(DeformationMapFile::METRIC_DEFORM_NEAREST_NODE);
   //}
   //else if (metricAverageTileNodesRadioButton->isChecked()) {
   //   deformationMapFile.setMetricDeformationType(DeformationMapFile::METRIC_DEFORM_AVERAGE_TILE_NODES);
   //}

   //
   // Deform the data file
   //
   try {
      switch(dft) {
         case BrainModelSurfaceDeformDataFile::DATA_FILE_AREAL_ESTIMATION:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_ATLAS:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_LAT_LON:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_RGB_PAINT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_TOPOGRAPHY:
            BrainModelSurfaceDeformDataFile::deformNodeAttributeFile(&deformationMapFile,
                                                                     dft,
                                                                     dataFileName,
                                                                     deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_METRIC:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_PAINT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_SHAPE:
            BrainModelSurfaceDeformDataFile::deformGiftiNodeDataFile(&deformationMapFile,
                                                                     dft,
                                                                     dataFileName,
                                                                     deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE:
            {
               QString deformedFileName2(deformedFileName);
               BrainModelSurfaceDeformDataFile::deformCoordinateFile(&deformationMapFile,
                                                                     dataFileName,
                                                                     deformedFileName2,
                                                                     true);
            }
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_COORDINATE_FLAT:
            BrainModelSurfaceDeformDataFile::deformFlatCoordinateFile(
                                    &deformationMapFile,
                                    atlasTopoFileName,
                                    dataFileName,
                                    indivTopoFileName,
                                    deformedFileName,
                                    indivDeformTopoFileName,
                                    10.0); //flatCoordMaxEdgeLengthFloatSpinBox->floatValue());
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_FLAT:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_SPHERICAL:
         case BrainModelSurfaceDeformDataFile::DATA_FILE_BORDER_PROJECTION:
            BrainModelSurfaceDeformDataFile::deformBorderFile(
                             &sourceBrainSet,
                             &targetBrainSet,
                             &deformationMapFile,
                             dft,
                             dataFileName,
                             deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL:
            BrainModelSurfaceDeformDataFile::deformCellOrFociFile(
                                 &sourceBrainSet,
                                 &targetBrainSet,
                                 &deformationMapFile,
                                 dataFileName,
                                 false,
                                 deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_CELL_PROJECTION:
            BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFile(
                                 &sourceBrainSet,
                                 &targetBrainSet,
                                 &deformationMapFile,
                                 dataFileName,
                                 false,
                                 deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI:
            BrainModelSurfaceDeformDataFile::deformCellOrFociFile(
                                 &sourceBrainSet,
                                 &targetBrainSet,
                                 &deformationMapFile,
                                 dataFileName,
                                 true,
                                 deformedFileName);
            break;
         case BrainModelSurfaceDeformDataFile::DATA_FILE_FOCI_PROJECTION:
            BrainModelSurfaceDeformDataFile::deformCellOrFociProjectionFile(
                                 &sourceBrainSet,
                                 &targetBrainSet,
                                 &deformationMapFile,
                                 dataFileName,
                                 true,
                                 deformedFileName);
            break;
      }
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Flat multiresolution morphing.
 */
static int
operationSurfaceFlatMultiResMorphing(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE FLAT MULTIRESOLUTION MORPHING", "-surface-flat-multi-morph");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE FLAT MULTIRESOLUTION MORPHING" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-flat-multi-morph \\ " << std::endl
         << "         <input-spec-file> <input-cut-topo-file>\\" << std::endl
         << "         <input-fiducial-coord-file>  <input-flat-coord-file> \\" << std::endl
         << "         " << std::endl
         << "      Peform flat multi-resolution morphing (distortion correction)." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString specFileName;
   CommandLineUtilities::getNextParameter("Input Spec File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);
                                          
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Cut Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Input Fiducial Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);
   
   QString flatCoordFileName;
   CommandLineUtilities::getNextParameter("Input Flat Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          flatCoordFileName);
   
   //
   // Read spec file
   //
   SpecFile specFile;
   try {
      specFile.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading spec file " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Set the selected files
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::cutTopoFileTag, topoFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::fiducialCoordFileTag, fiducialCoordFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::flatCoordFileTag, flatCoordFileName, 
                          "", SpecFile::SPEC_FALSE);
    
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, specFileName, errorMessage)) {
      std::cout << "ERROR: Reading spec file: " << std::endl
                << errorMessage.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Find the fiducial surface
   //
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fiducialSurface == NULL) {
      std::cout << "ERROR: Unable to find fiducial surface." << std::endl;
      exit(-1);
   }
   
   //
   // Find the flat surface
   //
   BrainModelSurface* flatSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FLAT);
   if (flatSurface == NULL) {
      std::cout << "ERROR: Unable to find flat surface." << std::endl;
      exit(-1);
   }
   
   //
   // Do flat multiresolution morphing
   //
   BrainModelSurfaceMultiresolutionMorphing bmsmm(&brainSet,
                                                  fiducialSurface,
                                                  flatSurface,
                                                  BrainModelSurfaceMorphing::MORPHING_SURFACE_FLAT);
   try {
      bmsmm.execute();   
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << "ERROR: Flat multi-resolution morphing failed." << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
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
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Spherical multiresolution morphing.
 */
static int
operationSurfaceSphericalMultiResMorphing(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE SPHERICAL MULTIRESOLUTION MORPHING", "-surface-sphere-multi-morph");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE SPHERICAL MULTIRESOLUTION MORPHING" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-sphere-multi-morph \\ " << std::endl
         << "         <input-spec-file> <input-closed-topo-file>\\" << std::endl
         << "         <input-fiducial-coord-file>  <input-spherical-coord-file> \\" << std::endl
         << "         " << std::endl
         << "      Peform spherical multi-resolution morphing (distortion correction)." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString specFileName;
   CommandLineUtilities::getNextParameter("Input Spec File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);
                                          
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Cut Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Input Fiducial Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);
   
   QString sphericalCoordFileName;
   CommandLineUtilities::getNextParameter("Input Spherical Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sphericalCoordFileName);
   
   //
   // Read spec file
   //
   SpecFile specFile;
   try {
      specFile.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading spec file " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Set the selected files
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::cutTopoFileTag, topoFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::fiducialCoordFileTag, fiducialCoordFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::sphericalCoordFileTag, sphericalCoordFileName, 
                          "", SpecFile::SPEC_FALSE);
    
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, specFileName, errorMessage)) {
      std::cout << "ERROR: Reading spec file: " << std::endl
                << errorMessage.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Find the fiducial surface
   //
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fiducialSurface == NULL) {
      std::cout << "ERROR: Unable to find fiducial surface." << std::endl;
      exit(-1);
   }
   
   //
   // Find the spherical surface
   //
   BrainModelSurface* sphereSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_SPHERICAL);
   if (sphereSurface == NULL) {
      std::cout << "ERROR: Unable to find spherical surface." << std::endl;
      exit(-1);
   }
   
   //
   // Do flat multiresolution morphing
   //
   BrainModelSurfaceMultiresolutionMorphing bmsmm(&brainSet,
                                                  fiducialSurface,
                                                  sphereSurface,
                                                  BrainModelSurfaceMorphing::MORPHING_SURFACE_SPHERICAL);
   try {
      bmsmm.execute();   
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << "ERROR: Spherical multi-resolution morphing failed." << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
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
   return 0;
}


/*----------------------------------------------------------------------------------------
 * Sulcal Depth.
 */
static int
operationSurfaceSulcalDepth(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE SULCAL DEPTH GENERATION", "-surface-sulcal-depth");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE SULCAL DEPTH GENERATION" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-sulcal-depth \\ " << std::endl
         << "         <input-spec-file> <input-closed-topo-file>\\" << std::endl
         << "         <input-fiducial-coord-file>  <input-hull-vtk-file> \\" << std::endl
         << "         <output-surface-shape-file> <output-hull-coord-file>" << std::endl
         << "         " << std::endl
         << "      Generate sulcal depth." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString specFileName;
   CommandLineUtilities::getNextParameter("Input Spec File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          specFileName);
                                          
   QString topoFileName;
   CommandLineUtilities::getNextParameter("Input Closed Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          topoFileName);

   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Input Fiducial Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);
   
   QString hullVtkFileName;
   CommandLineUtilities::getNextParameter("Hull VTK File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          hullVtkFileName);
   
   QString outputShapeFileName;
   CommandLineUtilities::getNextParameter("Output Surface Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputShapeFileName);
   
   QString outputHullCoordFileName;
   CommandLineUtilities::getNextParameter("Output Hull Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputHullCoordFileName);
   
   //
   // Read spec file
   //
   SpecFile specFile;
   try {
      specFile.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: reading spec file " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Set the selected files
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::cutTopoFileTag, topoFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::fiducialCoordFileTag, fiducialCoordFileName, 
                          "", SpecFile::SPEC_FALSE);
    
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, specFileName, errorMessage)) {
      std::cout << "ERROR: Reading spec file: " << std::endl
                << errorMessage.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Find the fiducial surface
   //
   BrainModelSurface* fiducialSurface = brainSet.getBrainModelSurfaceOfType(BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
   if (fiducialSurface == NULL) {
      std::cout << "ERROR: Unable to find fiducial surface." << std::endl;
      exit(-1);
   }
   
   SurfaceShapeFile outputShapeFile;
   CoordinateFile outputHullCoordFile;
      
   //
   // Do sulcal depth
   //
   BrainModelSurfaceSulcalDepthWithNormals sd(&brainSet,
                                              fiducialSurface,
                                              hullVtkFileName,
                                              &outputShapeFile,
                                              5,
                                              100,
                                              BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_CREATE_NEW,
                                              BrainModelSurfaceSulcalDepthWithNormals::DEPTH_COLUMN_CREATE_NEW,
                                              "Depth",
                                              "Smoothed Depth",
                                              &outputHullCoordFile);

   try {
      sd.execute();   
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << "ERROR: Sulcal depth generation failed." << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Write the surface shape file
   //
   try {
      outputShapeFile.writeFile(outputShapeFileName);
      specFile.addToSpecFile(SpecFile::surfaceShapeFileTag,
                             outputShapeFileName,
                             "",
                             true);
   }
   catch (FileException& e) {
      std::cout << "ERROR: writing " << outputShapeFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
   }
   
   //
   // Write the hull coord file file
   //
   try {
      outputHullCoordFile.writeFile(outputHullCoordFileName);
      specFile.addToSpecFile(SpecFile::hullCoordFileTag,
                             outputHullCoordFileName,
                             "",
                             true);
   }
   catch (FileException& e) {
      std::cout << "ERROR: writing " << outputHullCoordFileName.toAscii().constData() << std::endl;
      std::cout << "       " << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Disconnect nodes in the topology file that are identify by the paint file, column
 * number, and paint name.
 */
static int
operationSurfaceTopologyDisconnectPaintNodes(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE TOPOLOGY DISCONNECT PAINT NODES", "-surface-topology-disconnect-paint-nodes");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE TOPOLOGY DISCONNECT PAINT NODES" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-topology-disconnect-paint-nodes \\ " << std::endl
         << "         <input-topo-file> <input-paint-file> \\" << std::endl
         << "         <input-paint-column-number> <input-paint-name>" << std::endl
         << "         <output-topo-file>" << std::endl
         << "      " << std::endl
         << "      Disconnect all nodes that appear in the specified paint file column" << std::endl
         << "      which use the specified paint name." << std::endl
         << "      " << std::endl
         << "      Note: Paint file column numbers begin at 1." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputTopoFileName;
   CommandLineUtilities::getNextParameter("Input Topology File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputTopoFileName);
                                          
   QString paintFileName;
   CommandLineUtilities::getNextParameter("Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintFileName);
                                          
   int paintColumnNumber;
   CommandLineUtilities::getNextParameter("Paint Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintColumnNumber);
                                          
   QString paintName;
   CommandLineUtilities::getNextParameter("Paint Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintName);
                                          
   QString outputTopoFileName;
   CommandLineUtilities::getNextParameter("Output Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputTopoFileName);
                                          
   //
   // Read the topology file
   //  
   TopologyFile topoFile;
   try {
      topoFile.readFile(inputTopoFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Read the paint file
   //  
   PaintFile paintFile;
   try {
      paintFile.readFile(paintFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
      
   //
   // Disconnect nodes
   //
   try {
      topoFile.disconnectNodesUsingPaint(paintFile,
                                         (paintColumnNumber - 1),
                                         paintName);
      if (topoFile.getTopologyType() == TopologyFile::TOPOLOGY_TYPE_CLOSED) {
         topoFile.setTopologyType(TopologyFile::TOPOLOGY_TYPE_OPEN);
      }
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }

   //
   // Write the topology file
   //
   try {
      topoFile.writeFile(outputTopoFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * List of neighbors generation.
 */
static int
operationSurfaceTopologyNeighbors(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE TOPOLOGY NEIGHBOR LIST", "-surface-topology-neighbors");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE TOPOLOGY NEIGHBOR LIST" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-topology-neighbors \\ " << std::endl
         << "         <input-topo-file> <output-text-file> \\" << std::endl
         << "         " << std::endl
         << "      For each node in the input topology file, generate a list  " << std::endl
         << "      of the nodes neighbors.  Each line contains the node number   " << std::endl
         << "      which is followed by the node's neighbors." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputTopoFileName;
   CommandLineUtilities::getNextParameter("Input Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputTopoFileName);
                                          
   QString outputTextFileName;
   CommandLineUtilities::getNextParameter("Output Text File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputTextFileName);
                                          
   //
   // Read the topology file
   //  
   TopologyFile topoFile;
   try {
      topoFile.readFile(inputTopoFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   TextFile outputTextFile;
   
   //
   // Get the topology helper
   //
   const TopologyHelper* th = topoFile.getTopologyHelper(true, true, true);
   const int numNodes = th->getNumberOfNodes();
   const QString blank(" ");
   for (int i = 0; i < numNodes; i++) {
      QString nodeLine(QString::number(i));
      std::vector<int> neighbors;
      th->getNodeNeighbors(i, neighbors);
      for (unsigned int j = 0; j < neighbors.size(); j++) {
         nodeLine += (blank + QString::number(neighbors[j]));
      }
      outputTextFile.appendLine(nodeLine);
   }
   
   //
   // Write the neighbors file
   //
   try {
      outputTextFile.writeFile(outputTextFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Surface to Volume Mask.
 */
static int
operationSurfacesToSegmentationVolumeMask(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACES TO SEGMENTATION VOLUME MASK", "-surfaces-to-segmentation-volume-mask");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACES TO SEGMENTATION VOLUME MASK" << std::endl
         << "      " << programName.toAscii().constData() << " -surfaces-to-segmentation-volume-mask \\ " << std::endl
         << "         <input-spec-file> <output-volume-file> \\" << std::endl
         << "         [dilation-iterations]" << std::endl
         << "      For each fiducial surface listed in the spec file, convert it" << std::endl
         << "      to a segmentation volume and then merge (union) all of these" << std::endl
         << "      segmentation volumes into a single volume.  Perform the specified" << std::endl
         << "      number of dilation iterations on the output volume prior to " << std::endl
         << "      saving it." << std::endl
         << "      " << std::endl
         << "      The output volume must exist and a volume may be created by this" << std::endl
         << "      program using the -volume-create option." << std::endl
         << "      " << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputSpecFileName;
   CommandLineUtilities::getNextParameter("Input Spec File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputSpecFileName);
                                          
   QString outputVolumeFileName;
   CommandLineUtilities::getNextParameter("Output Volume File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputVolumeFileName);
   
   int dilationIterations = 0;
   if (argc >= 5) {
      CommandLineUtilities::getNextParameter("Dilation Iterations",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             dilationIterations);
   }
   
   //
   // Save directory
   //
   const QString currentDirectoryName = QDir::currentPath();
   
   //
   // Create a spec file
   //
   SpecFile specFile;
   try {
      specFile.readFile(inputSpecFileName);
   }
   catch (FileException& e) {
      std::cout << "Spec File Read Error: " << e.whatQString().toAscii().constData() << std::endl;
      std::exit(-1);
   }
   specFile.setAllFileSelections(SpecFile::SPEC_TRUE);
                          
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, "", errorMessage)) {
      std::cout << "ERROR: Reading spec file: " << std::endl
                << errorMessage.toAscii().constData() << std::endl;
   }
   
   //
   // Reading brain set will change directory to one containing spec file
   //
   QDir::setCurrent(currentDirectoryName);
   
   //
   // Read the volume file and clear it
   //
   VolumeFile outputVolumeFile;
   readVolumeFile(outputVolumeFile, outputVolumeFileName);
   int dimensions[3];
   outputVolumeFile.getDimensions(dimensions);
   float spacing[3];
   outputVolumeFile.getSpacing(spacing);
   float origin[3];
   outputVolumeFile.getOrigin(origin);
   
   //
   // Loop through the surfaces
   //
   std::vector<QString> segmentationVolumeFileNames;
   for (int i = 0; i < brainSet.getNumberOfBrainModels(); i++) {
      //
      // See if model is fiducial surface
      //
      BrainModelSurface* bms = brainSet.getBrainModelSurface(i);
      if (bms->getSurfaceType() == BrainModelSurface::SURFACE_TYPE_FIDUCIAL) {
         //
         // Convert the surface to a segmentation volume
         //
         const float surfaceOffset[3] = { 0.0, 0.0, 0.0 };
         const float innerBoundary = -1.5;
         const float outerBoundary =  1.5;
         const float intersectionStep = 0.5;
         BrainModelSurfaceToVolumeConverter bmsv(&brainSet,
                                                 bms,
                                                 StereotaxicSpace::SPACE_UNKNOWN,
                                                 surfaceOffset,
                                                 dimensions,
                                                 spacing,
                                                 origin,
                                                 innerBoundary,
                                                 outerBoundary,
                                                 intersectionStep,
                                                 BrainModelSurfaceToVolumeConverter::CONVERT_TO_SEGMENTATION_VOLUME_USING_NODES);
         try {
            bmsv.execute();
            
            VolumeFile* vf = bmsv.getOutputVolume();
            if (vf != NULL) {
               const QString name("TempSegmentVolume_" 
                                  + QString::number(i)
                                  + SpecFile::getNiftiVolumeFileExtension());
               try {
                  vf->writeFile(name);
               }
               catch (FileException& e) {
                  std::cout << e.whatQString().toAscii().constData() << std::endl;
                  std::exit(-1);
               }
               segmentationVolumeFileNames.push_back(name);
               brainSet.deleteVolumeFile(vf);
            }
            else {
               std::cout << "ERROR: Failed to create volume for surface:"
                         << bms->getFileName().toAscii().constData() << std::endl;
               exit(-1);
            }
         }
         catch (BrainModelAlgorithmException& e) {
            std::cout << e.whatQString().toAscii().constData() << std::endl;
               exit(-1);
         }
      }
   }
   
   //
   // Create the mask
   //
   try {
      VolumeFile::createSegmentationMask(outputVolumeFileName,
                                         segmentationVolumeFileNames,
                                         dilationIterations);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      std::exit(-1);
   }
   
   //
   // If not debug delete temp volumes
   //
   if (DebugControl::getDebugOn() == false) {
      for (unsigned int i = 0; i < segmentationVolumeFileNames.size(); i++) {
         QFile::remove(segmentationVolumeFileNames[i]);
      }
   }
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Surface to Volume.
 */
static int
operationSurfaceToVolume(int argc, char* argv[], int argIndex)
{
   float innerBoundary = -1.5;
   float outerBoundary =  1.5;
   float intersectionStep = 0.5;
   
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE TO VOLUME", "-surface-to-volume");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE TO VOLUME" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-to-volume \\ " << std::endl
         << "         <input-topo-file> <input-coord-file> \\" << std::endl
         << "         <input-metric-paint-or-shape-file> <i-input-column-number> \\" << std::endl
         << "         <output-volume-file> \\ " << std::endl
         << "         [f-inner-boundary] [f-outer-boundary] [f-intersection-step]" << std::endl
         << "         " << std::endl
         << "      Intersect a surface with a volume and assign the specified  " << std::endl
         << "      column number's data of the paint, metric, or shape file " << std::endl
         << "      to the volume.  THE COLUMN NUMBERS START AT ONE." << std::endl
         << "      " << std::endl
         << "      The volume file must already exist and it must be in the same " << std::endl
         << "      stereotaxic space as the surface.  A volume file may be created" << std::endl
         << "      by using this program's \"-volume-create\" command." << std::endl
         << "      " << std::endl
         << "      The inner boundary, outer boundary, and intersection step parameters" << std::endl
         << "      are optional.  The default values are: " << std::endl
         << "         inner boundary: " << innerBoundary << std::endl
         << "         outer boundary: " << outerBoundary << std::endl
         << "         intersection step: " << intersectionStep << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputTopoFileName;
   CommandLineUtilities::getNextParameter("Input Topo File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputTopoFileName);
                                          
   QString inputCoordFileName;
   CommandLineUtilities::getNextParameter("Input Coord File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputCoordFileName);

   QString inputDataFileName;
   CommandLineUtilities::getNextParameter("Input Metric, Paint, or Shape Data File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputDataFileName);
   
   int inputDataFileColumnNumber;
   CommandLineUtilities::getNextParameter("Data File Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputDataFileColumnNumber);
   inputDataFileColumnNumber--;  // column numbers entered by user start at one!!
   
   QString outputVolumeFileName;
   CommandLineUtilities::getNextParameter("Output Volume File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputVolumeFileName);
   
   if (argc >= 8) {
      CommandLineUtilities::getNextParameter("Inner Boundary",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             innerBoundary);
   }
   
   if (argc >= 9) {
      CommandLineUtilities::getNextParameter("Outer Boundary",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             outerBoundary);
   }
   
   if (argc >= 10) {
      CommandLineUtilities::getNextParameter("Intersection Step",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             intersectionStep);
   }
   
   //
   // Create a spec file
   //
   SpecFile specFile;
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::closedTopoFileTag, inputTopoFileName, 
                          "", SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::fiducialCoordFileTag, inputCoordFileName, 
                          "", SpecFile::SPEC_FALSE);
                          
   BrainModelSurfaceToVolumeConverter::CONVERSION_MODE conversionMode;
   if (inputDataFileName.endsWith(SpecFile::getMetricFileExtension())) {
      specFile.addToSpecFile(SpecFile::metricFileTag, inputDataFileName, 
                             "", SpecFile::SPEC_FALSE);
      conversionMode = BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_METRIC_INTERPOLATE;
   }
   else if (inputDataFileName.endsWith(SpecFile::getPaintFileExtension())) {
      specFile.addToSpecFile(SpecFile::paintFileTag, inputDataFileName, 
                             "", SpecFile::SPEC_FALSE);
      conversionMode = BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_PAINT;
   }
   else if (inputDataFileName.endsWith(SpecFile::getSurfaceShapeFileExtension())) {
      specFile.addToSpecFile(SpecFile::surfaceShapeFileTag, inputDataFileName, 
                             "", SpecFile::SPEC_FALSE);
      conversionMode = BrainModelSurfaceToVolumeConverter::CONVERT_TO_ROI_VOLUME_USING_SURFACE_SHAPE;
   }
   else {
      std::cout << "ERROR: Input data file name does not have a valid extension that identifies the file type." << std::endl;
      return -1;
   }
   
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet(true);
   if (brainSet.readSpecFile(specFile, "", errorMessage)) {
      std::cout << "ERROR: Reading spec file: " << std::endl
                << errorMessage.toAscii().constData() << std::endl;
   }
   
   //
   // Get the surface
   //
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: reading topology or coordinate file (no surface)." << std::endl;
      exit(-1);
   }
   
   //
   // Read the volume file
   //
   VolumeFile outputVolumeFile;
   readVolumeFile(outputVolumeFile, outputVolumeFileName);
   int dimensions[3];
   outputVolumeFile.getDimensions(dimensions);
   float spacing[3];
   outputVolumeFile.getSpacing(spacing);
   float origin[3];
   outputVolumeFile.getOrigin(origin);

   //
   // Convert the surface to a segmentation volume
   //
   const float surfaceOffset[3] = { 0.0, 0.0, 0.0 };
   BrainModelSurfaceToVolumeConverter bmsv(&brainSet,
                                           bms,
                                           StereotaxicSpace::SPACE_UNKNOWN,
                                           surfaceOffset,
                                           dimensions,
                                           spacing,
                                           origin,
                                           innerBoundary,
                                           outerBoundary,
                                           intersectionStep,
                                           conversionMode);
   bmsv.setNodeAttributeColumn(inputDataFileColumnNumber);
   try {
      bmsv.execute();
      
      VolumeFile* vf = bmsv.getOutputVolume();
      if (vf != NULL) {
         writeVolumeFile(*vf, outputVolumeFileName, "");
      }
      else {
         std::cout << "ERROR: Failed to create volume." << std::endl;
         exit(-1);
      }
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Map Gyri associated with depth ROI.
 */
static int
operationSurfaceMapGyriWithDepthROI(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("SURFACE MAP GYRI WITH DEPTH ROI", "-surface-map-gyri-with-depth-roi");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   SURFACE MAP GYRI WITH DEPTH ROI" << std::endl
         << "      " << programName.toAscii().constData() << " -surface-map-gyri-with-depth-roi \\" << std::endl
         << "         <output-metric-file> <std-mesh-topo-file> \\" << std::endl
         << "         <std-mesh-paint-file> <paint-column-number> <paint-name> \\" << std::endl
         << "         <std-mesh-fiducial-coord-file-1> <std-mesh-cerebral-hull-coord-file-1>\\" << std::endl
         << "         <std-mesh-fiducial-coord-file-2> <std-mesh-cerebral-hull-coord-file-2>\\" << std::endl
         << "                       ...                           ..." << std::endl
         << "         <std-mesh-fiducial-coord-file-N> <std-mesh-cerebral-hull-coord-file-N> " << std::endl
         << "         " << std::endl
         << "      Determine gyral regions associated with sulcal depth ROI." << std::endl
         << "         " << std::endl
         << "            1) Identify the nodes in the ROI and only process nodes" << std::endl
         << "               that are in the ROI." << std::endl
         << "            2) For each of the ROI nodes in the hull, find the nearest" << std::endl
         << "               node in the fiducial surface using using Euclidean distance." << std::endl
         << "            3) Increment the metric value for the nearest fiducial " << std::endl
         << "               surface node." << std::endl
         << "            4) Repeat steps 2 thru 3 for each fiducial/hull pair." << std::endl
         << "         " << std::endl
         << "         Note: Paint column numbers start at 1." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   // 
   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);
                                          
   QString stdMeshTopoFileName;
   CommandLineUtilities::getNextParameter("Std Mesh Topo File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          stdMeshTopoFileName);
                                          
   QString stdMeshPaintFileName;
   CommandLineUtilities::getNextParameter("Std Mesh Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          stdMeshPaintFileName);
                                          
   int paintColumnNumber;
   CommandLineUtilities::getNextParameter("Paint Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintColumnNumber);
                                          
   QString paintName;
   CommandLineUtilities::getNextParameter("Paint Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          paintName);
                                          

   //
   // Read in the paint file
   //
   PaintFile paintFile;
   try {
      paintFile.readFile(stdMeshPaintFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Identify the nodes in the ROI using the paint file information
   //
   const int numNodes = paintFile.getNumberOfNodes();
   if (numNodes <= 0) {
      std::cout << "Paint file contains not columns." << std::endl;
   }
   if ((paintColumnNumber < 1) ||
       (paintColumnNumber > paintFile.getNumberOfColumns())) {
      std::cout << "Invalid paint column number=" << paintColumnNumber << std::endl;
      exit(-1);
   }
   paintColumnNumber--;   // paint column numbers start at one for user but are an array index
   const int paintNameIndex = paintFile.getPaintIndexFromName(paintName);
   if (paintNameIndex < 0) {
      std::cout << "Paint name=" << paintName.toAscii().constData() << " not found in paint file." << std::endl;
      exit(-1);
   }
   std::vector<bool> roiNodeFlags(numNodes, false);
   bool roiNodeFound = false;
   for (int i = 0; i < numNodes; i++) {
      if (paintFile.getPaint(i, paintColumnNumber) == paintNameIndex) {
         roiNodeFlags[i] = true;
         roiNodeFound = true;
      }
   }
   if (roiNodeFound == false) {
      std::cout << "No nodes are in the ROI." << std::endl;
      exit(-1);
   }
   
   //
   // Create the metric file
   //
   MetricFile outputMetricFile;
   outputMetricFile.setNumberOfNodesAndColumns(numNodes, 1);
   outputMetricFile.setColumnName(0, "Map Gyri with Depth ROI");
   
   //
   // Loop through the fiducial/hull file name pairs
   //
   int fiducialCount = 1;
   while (argIndex < (argc - 1)) {
      QString stdMeshFiducialCoordFileName;
      CommandLineUtilities::getNextParameter("Std Mesh Fiducial Coord File Name " + QString::number(fiducialCount),
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             stdMeshFiducialCoordFileName);
                                             
      QString stdMeshCerebralHullCoordFileName;
      CommandLineUtilities::getNextParameter("Std Mesh Cerebral Hull Coord File Name " + QString::number(fiducialCount),
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             stdMeshCerebralHullCoordFileName);
      fiducialCount++;
      
      std::cout << "Processing fiducial " << stdMeshFiducialCoordFileName.toAscii().constData()
                << " and hull " << stdMeshCerebralHullCoordFileName.toAscii().constData() << std::endl;

      //
      // Create a brain set for the fiducial surface
      //
      BrainSet fiducialBrainSet(stdMeshTopoFileName, stdMeshFiducialCoordFileName);
      const BrainModelSurface* fiducialSurface = fiducialBrainSet.getBrainModelSurface(0);
      //const CoordinateFile* fiducialCoords = fiducialSurface->getCoordinateFile();
      if (fiducialSurface == NULL) {
         std::cout << "Problem creating a surface from "
                   << stdMeshTopoFileName.toAscii().constData()
                   << " and "
                   << stdMeshFiducialCoordFileName.toAscii().constData()
                   << std::endl;
         exit(-1);
      }
      if (fiducialSurface->getNumberOfNodes() != numNodes) {
         std::cout << "Fiducial surface has " << fiducialSurface->getNumberOfNodes()
                   << " but paint file has " << numNodes << " nodes." << std::endl;
         exit(-1);
      }
      
      //
      // Create a brain set for the hull
      //
      BrainSet hullBrainSet(stdMeshTopoFileName, stdMeshCerebralHullCoordFileName);
      const BrainModelSurface* hullSurface = hullBrainSet.getBrainModelSurface(0);
      const CoordinateFile* hullCoords = hullSurface->getCoordinateFile();
      if (hullSurface == NULL) {
         std::cout << "Problem creating a surface from "
                   << stdMeshTopoFileName.toAscii().constData()
                   << " and "
                   << stdMeshCerebralHullCoordFileName.toAscii().constData()
                   << std::endl;
         exit(-1);
      }
      
      //
      // Create a point projector for projecting nodes from hull to fiducial
      //
      BrainModelSurfacePointProjector fiducialProjector(fiducialSurface,
                                                        BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                                                        false);

      //
      // loop through the nodes
      //
      for (int i = 0; i < numNodes; i++) {
         //
         // Is this node in the ROI
         //
         if (roiNodeFlags[i]) {
            //
            // Get hull coordinate of node
            //
            const float* hullXYZ = hullCoords->getCoordinate(i);
            
            //
            // Project to fiducial surface
            //
            int nearestNodeNumber = -1;
            int tileNodes[3];
            float barycentricAreas[3];
            const int nearestTile = fiducialProjector.projectBarycentric(hullXYZ,
                                                                          nearestNodeNumber,
                                                                          tileNodes,
                                                                          barycentricAreas,
                                                                          true);
            //
            // Increment the metric value for the nearest node
            //
            if ((nearestTile != 0) && (nearestNodeNumber >= 0)) {
               outputMetricFile.setValue(nearestNodeNumber, 0, 
                                         outputMetricFile.getValue(nearestNodeNumber, 0) + 1.0);
            }
         }
      }
   }
   
   //
   // Write the metric file
   //
   try {
      outputMetricFile.writeFile(outputMetricFileName);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * update deformation map paths.
 */
static int
operationDeformationMapPathUpdate(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("DEFORMATION MAP FILE SOURCE/TARGET PATH UPDATE", "-def-map-path");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   DEFORMATION MAP FILE SOURCE/TARGET PATH UPDATE" << std::endl
         << "      " << programName.toAscii().constData() << " -def-map-path \\ " << std::endl
         << "         <deformation-map-file> <source-path> <target-path>" << std::endl
         << "         " << std::endl
         << "      Update the source (individual) and target (atlas) paths in" << std::endl
         << "      a deformation map file." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString defMapFileName;
   CommandLineUtilities::getNextParameter("Deformation Map File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          defMapFileName);
                                          
   QString sourcePath;
   CommandLineUtilities::getNextParameter("Source Path",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          sourcePath);

   QString targetPath;
   CommandLineUtilities::getNextParameter("Target Path",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          targetPath);
                                          
   try {
      DeformationMapFile defMapFile;
      
      defMapFile.readFile(defMapFileName);
      
      defMapFile.setSourceDirectory(sourcePath);
      defMapFile.setTargetDirectory(targetPath);
      
      defMapFile.writeFile(defMapFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Metric file set column name
 */
static int
operationMetricFileSetColumnName(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE SET COLUMN NAME", "-metric-or-shape-set-column-name");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE SET COLUMN NAME" << std::endl
         << "      " << programName.toAscii().constData() << " -metric-or-shape-set-column-name \\ " << std::endl
         << "         <column-number> \\ " << std::endl
         << "         <new-column-name> \\" << std::endl
         << "         <metric-or-shape-file>  " << std::endl
         << "         " << std::endl
         << "      Set the name of a column in the metric/shape file." << std::endl
         << "         " << std::endl
         << "      Note: Column numbers start at one." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int columnNumber;
   CommandLineUtilities::getNextParameter("Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          columnNumber);
   QString newColumnName;
   CommandLineUtilities::getNextParameter("New Column Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          newColumnName);
   QString metricFileName;
   CommandLineUtilities::getNextParameter("Metric/Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricFileName);

   columnNumber--;
   try {
      MetricFile metricFile;
      metricFile.readFile(metricFileName);
      if ((columnNumber >= 0) && (columnNumber < metricFile.getNumberOfColumns())) {
         metricFile.setColumnName(columnNumber, newColumnName);
      }
      else {
         throw FileException("Invalid column number.");
      }
      metricFile.writeFile(metricFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Metric file info
 */
static int
operationMetricFileInfo(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE INFORMATION", "-metric-or-shape-file-info");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE INFORMATION" << std::endl
         << "      " << programName.toAscii().constData() << " -metric-or-shape-file-info \\ " << std::endl
         << "         <metric-or-shape-file>  " << std::endl
         << "         " << std::endl
         << "      Print information about contents of a metric or shape file." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString metricFileName;
   CommandLineUtilities::getNextParameter("Metric/Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricFileName);

   try {
      std::cout << "Filename: " 
                << FileUtilities::basename(metricFileName).toAscii().constData()
                << std::endl;
      MetricFile metricFile;
      metricFile.readFile(metricFileName);
      
      const int numNodes = metricFile.getNumberOfNodes();
      const int numCols  = metricFile.getNumberOfColumns();
      std::cout << "Number of Nodes: " << numNodes << std::endl;
      std::cout << "Number of Columns: " << numCols << std::endl;
      
      std::cout << QString("column").rightJustified(6).toAscii().constData() << " "
                << QString("min-value").rightJustified(12).toAscii().constData() << " "
                << QString("max-value").rightJustified(12).toAscii().constData() << "   "
                << "column-name"
                << std::endl;
      for (int i = 0; i < numCols; i++) {
         float minVal, maxVal;
         metricFile.getDataColumnMinMax(i, minVal, maxVal);
         const QString colString = QString::number(i + 1).rightJustified(6);
         const QString minString = QString::number(minVal, 'f', 3).rightJustified(12);
         const QString maxString = QString::number(maxVal, 'f', 3).rightJustified(12);
         std::cout << colString.toAscii().constData() << " "
                   << minString.toAscii().constData() << " "
                   << maxString.toAscii().constData() << "   "
                   << metricFile.getColumnName(i).toAscii().constData()
                   << std::endl;
      }
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Metric file set column to scalar value
 */
static int
operationMetricFileSetColumnToScalar(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE SET COLUMN TO VALUE", "-metric-or-shape-set-column-to-value");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE SET COLUMN TO VALUE" << std::endl
         << "      " << programName.toAscii().constData() << " -metric-or-shape-set-column-to-value \\ " << std::endl
         << "         <column-number> \\ " << std::endl
         << "         <new-value> \\" << std::endl
         << "         <metric-or-shape-file>  " << std::endl
         << "         " << std::endl
         << "      Set the value for all nodes in a column." << std::endl
         << "         " << std::endl
         << "      Note: Column numbers start at one." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int columnNumber;
   CommandLineUtilities::getNextParameter("Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          columnNumber);
   float value;
   CommandLineUtilities::getNextParameter("New Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          value);
   QString metricFileName;
   CommandLineUtilities::getNextParameter("Metric/Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricFileName);

   columnNumber--;
   try {
      MetricFile metricFile;
      metricFile.readFile(metricFileName);
      if ((columnNumber >= 0) && (columnNumber < metricFile.getNumberOfColumns())) {
         metricFile.setColumnAllNodesToScalar(columnNumber, value);
      }
      else {
         throw FileException("Invalid column number.");
      }
      metricFile.writeFile(metricFileName);
      std::cout << metricFile.getColumnName(columnNumber).toAscii().constData()
                << " has been set to "
                << value
                << std::endl;
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Metric file creation
 */
static int
operationMetricFileCreate(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE CREATE", "-metric-or-shape-create");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE CREATE" << std::endl
         << "      " << programName.toAscii().constData() << " -metric-or-shape-create \\ " << std::endl
         << "         <number-of-nodes> \\ " << std::endl
         << "         <number-of-columns> \\" << std::endl
         << "         <output-metric-or-shape-file>  " << std::endl
         << "         " << std::endl
         << "      Create a metric or shape file with the specified number of nodes and" << std::endl
         << "      columns.  All values are initialized to zero." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   int numberOfNodes;
   CommandLineUtilities::getNextParameter("Number of Nodes",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfNodes);
   int numberOfColumns;
   CommandLineUtilities::getNextParameter("Number of Columns",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfColumns);
   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric/Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   try {
      MetricFile outputMetricFile(numberOfNodes, numberOfColumns);
      outputMetricFile.writeFile(outputMetricFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Metric correlation coefficient map
 */
static int
operationMetricCorrelationCoefficientMap(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE CORRELATION COEFFICIENT MAP", "-metric-or-shape-correlation-map");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE CORRELATION COEFFICIENT MAP" << std::endl
         << "      " << programName.toAscii().constData() << " -metric-or-shape-correlation-map \\ " << std::endl
         << "         <input-metric-or-shape-file-A> \\ " << std::endl
         << "         <input-metric-or-shape file-B> \\" << std::endl
         << "         <output-metric-or-shape-file>  " << std::endl
         << "         " << std::endl
         << "      For each node, compute a correlation coefficient from the node's values" << std::endl
         << "      in the two input metric files.  The two input files must have the same" << std::endl
         << "      number of columns and column 'x' in the two files should contain data" << std::endl
         << "      for the same subject." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputMetricFileNameA;
   CommandLineUtilities::getNextParameter("Input Metric/Shape File A",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputMetricFileNameA);
   QString inputMetricFileNameB;
   CommandLineUtilities::getNextParameter("Input Metric/Shape File B",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputMetricFileNameB);
   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric/Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   try {
      MetricFile metricFileA, metricFileB;
      metricFileA.readFile(inputMetricFileNameA);
      metricFileB.readFile(inputMetricFileNameB);
      MetricFile* outputMetricFile = MetricFile::computeCorrelationCoefficientMap(&metricFileA,
                                                                                  &metricFileB);
      outputMetricFile->writeFile(outputMetricFileName);
      delete outputMetricFile;
      outputMetricFile = NULL;
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Composite columns in metric files
 */
static int
operationMetricComposite(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE COMPOSITE ALL COLUMNS", "-metric-or-shape-composite");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE COMPOSITE ALL COLUMNS" << std::endl
         << "      " << programName.toAscii().constData() << " -metric-or-shape-composite \\ " << std::endl
         << "         <output-metric-or-shape-file> <input-metric-or-shape files...>" << std::endl
         << "         " << std::endl
         << "      Concatenate all columns from the input metric/shape files and place" << std::endl
         << "      them into the output metric/shape file." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric/Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   std::vector<QString> inputMetricFileNames;   
   while (argIndex < (argc - 1)) {
      QString inputName;
      CommandLineUtilities::getNextParameter("Input Metric/Shape File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             inputName);
      inputMetricFileNames.push_back(inputName);
   }
                                          
   try {
      MetricFile::concatenateColumnsFromFiles(outputMetricFileName,
                                              inputMetricFileNames,
                                              MetricFile::CONCATENATE_COLUMNS_MODE_ALL,
                                              "");
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Copy columns from a metric file and place into a new metric file
 */
static int
operationMetricCopyColumnsToNewFile(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE COPY COLUMNS TO NEW FILE", 
                        "-metric-or-shape-copy-columns-to-new-file");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE COPY COLUMNS TO NEW FILE" << std::endl
         << "      " << programName.toAscii().constData() << " -metric-or-shape-copy-columns-to-new-file \\ " << std::endl
         << "         <input-metric-file> <output-metric-file> <columns-to-copy>" << std::endl
         << "         " << std::endl
         << "      Copy columns from a metric file into a new metric file." << std::endl
         << "      " << std::endl
         << "      The <columns-to-copy> specifies the columns that are to be copied" << std::endl
         << "      from the input file.  You may specify single columns or a range of" << std::endl
         << "      columns separated by a comma.  Column numbers start at one." << std::endl
         << "      Example:" << std::endl
         << "            1,3-5,7-9,12" << std::endl
         << "         This above specifies that columns 1, 3, 4, 5, 7, 8, 9, and 12" << std::endl
         << "         will be copied to the new file." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString inputFileName;
   CommandLineUtilities::getNextParameter("Input Metric/Shape Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputFileName);

   QString outputFileName;
   CommandLineUtilities::getNextParameter("Output Metric/Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputFileName);

   QString copyString;
   CommandLineUtilities::getNextParameter("Columns to Copy",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          copyString);

   //
   // Convert copyString to a vector of column numbers
   //
   std::vector<int> columnsToCopy;
   QStringList sl = copyString.split(',', QString::SkipEmptyParts);
   for (int i = 0; i < sl.count(); i++) {
      QString s = sl.at(i);
      if (s.indexOf("-") > 0) {
         QStringList slDash = s.split('-', QString::SkipEmptyParts);
         if (slDash.count() != 2) {
            std::cout << "ERROR: Invalid range specified: " << s.toAscii().constData() << std::endl;
            exit(-1);
         }
         bool ok = false;
         const int start = slDash.at(0).toInt(&ok);
         if (ok == false) {
            std::cout << "ERROR: Invalid range specified: " << s.toAscii().constData() << std::endl;
            exit(-1);
         }
         const int stop  = slDash.at(1).toInt(&ok);
         if (ok == false) {
            std::cout << "ERROR: Invalid range specified: " << s.toAscii().constData() << std::endl;
            exit(-1);
         }
         
         for (int m = start; m <= stop; m++) {
            columnsToCopy.push_back(m - 1);
         }
      }
      else {
         bool ok = false;
         const int num  = s.toInt(&ok);
         if (ok == false) {
            std::cout << "ERROR: Invalid column specified: " << s.toAscii().constData() << std::endl;
            exit(-1);
         }
         columnsToCopy.push_back(num - 1);
      }
   }
   
   //
   // Extract the columns
   //
   try {
      MetricFile::extractColumnsFromFile(inputFileName, outputFileName, columnsToCopy);
   }
   catch (FileException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Append columns in metric files but only those with the specified name
 */
static int
operationMetricCompositeNamedColumn(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE COMPOSITE ALL COLUMNS", "-metric-or-shape-composite-named-column");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE COMPOSITE ALL COLUMNS" << std::endl
         << "      " << programName.toAscii().constData() << " -metric-or-shape-composite-named-column \\ " << std::endl
         << "         <name-of-column-that-is-case-sensitive> \\" << std::endl
         << "         <output-metric-or-shape-file> <input-metric-or-shape files...>" << std::endl
         << "         " << std::endl
         << "      Concatenate selected columns from the input metric/shape files and place" << std::endl
         << "      them into the output metric/shape file.  Only those columns that match" << std::endl
         << "      the specified column name are appended.  If there are more than one" << std::endl
         << "      columns with the name in the file, ALL will be appended." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString columnName;
   CommandLineUtilities::getNextParameter("Column Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          columnName);

   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric/Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   std::vector<QString> inputMetricFileNames;   
   while (argIndex < (argc - 1)) {
      QString inputName;
      CommandLineUtilities::getNextParameter("Input Metric/Shape File",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             inputName);
      inputMetricFileNames.push_back(inputName);
   }
                                          
   try {
      MetricFile::concatenateColumnsFromFiles(outputMetricFileName,
                                              inputMetricFileNames,
                                              MetricFile::CONCATENATE_COLUMNS_MODE_NAME_EXACT,
                                              columnName);
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 *
 */
static int
operationMetricStatisticsPairedTTest(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE STATISTICS PAIRED T-TEST", "-metric-or-shape-stat-paired-t-test");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE STATISTICS PAIRED T-TEST" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-stat-paired-t-test \\ " << std::endl
         << "        <metric-or-shape-file-A> <metric-or-shape-file-B> \\" << std::endl
         << "        <fiducial-coord-file> <open-topo-file> \\" << std::endl
         << "        <distortion-metric-shape-file> <distortion-column-number> \\" << std::endl
         << "        <output-tmap-metric-shape-file> \\" << std::endl
         << "        <output-shuffled-tmap-metric-shape-file> \\" << std::endl
         << "        <output-paint-file> <output-metric-file> <output-report-text-file> \\" << std::endl
         << "        <f-negative-threshold> <f-positive-threshold> \\" << std::endl
         << "        <f-p-value> \\ " << std::endl
         << "        <i-variance-smoothing-iterations> <f-variance-smoothing-strength> \\" << std::endl
         << "        <i-iterations> \\" << std::endl
         << "        <number-of-threads> " << std::endl
         << "        " << std::endl
         << "     Perform a paired T-Test on the metric/shape file." << std::endl
         << "     " << std::endl
         << "     The distortion column number starts at 1." << std::endl
         << "     " << std::endl
         << "     Number of threads is the number of concurrent processes run" << std::endl
         << "     during the cluster search of the shuffled metric/shape file." << std::endl
         << "     Use \"1\" if you are running on a single processor system." << std::endl
         << "     Users on systems with multiple processors or multi-core systems" << std::endl
         << "     should set the number of threads to the number of processors" << std::endl
         << "     and/or cores to reduce execution time." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   QString metricShapeFileA;
   CommandLineUtilities::getNextParameter("Metric or Shape File A",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricShapeFileA);

   QString metricShapeFileB;
   CommandLineUtilities::getNextParameter("Metric or Shape File B",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricShapeFileB);

   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Fiducial Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);

   QString openTopoFileName;
   CommandLineUtilities::getNextParameter("Open Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          openTopoFileName);

   QString distortionMetricShapeFileName;
   CommandLineUtilities::getNextParameter("Distortion Metric/Shape File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionMetricShapeFileName);

   int distortionColumnNumber = 1;
   CommandLineUtilities::getNextParameter("Distortion Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionColumnNumber);
   if (distortionColumnNumber <= 0) {
      std::cout << "Distortion column number must be >= 1." << std::endl;
      exit(-1);
   }
   distortionColumnNumber--;   // algorithm starts at zero

   QString outputTMapFileName;
   CommandLineUtilities::getNextParameter("Output T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputTMapFileName);

   QString outputShuffledTMapFileName;
   CommandLineUtilities::getNextParameter("Output Shuffled T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputShuffledTMapFileName);

   QString outputPaintFileName;
   CommandLineUtilities::getNextParameter("Output Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputPaintFileName);

   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   QString outputReportFileName;
   CommandLineUtilities::getNextParameter("Output Report File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputReportFileName);

   float negativeThreshold;
   CommandLineUtilities::getNextParameter("Negative Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          negativeThreshold);

   float positiveThreshold;
   CommandLineUtilities::getNextParameter("Positive Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          positiveThreshold);

   float pValue;
   CommandLineUtilities::getNextParameter("P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pValue);

   int varianceSmoothingIterations;
   CommandLineUtilities::getNextParameter("Variance Smoothing Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceSmoothingIterations);

   float varianceSmoothingStrength;
   CommandLineUtilities::getNextParameter("Variance Smoothing Strength",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceSmoothingStrength);

   int iterations;
   CommandLineUtilities::getNextParameter("Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);

   int numberOfThreads;
   CommandLineUtilities::getNextParameter("Number of Threads",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfThreads);

   std::vector<QString> metricAndShapeFiles;
   metricAndShapeFiles.push_back(metricShapeFileA);
   metricAndShapeFiles.push_back(metricShapeFileB);
   
   BrainSet bs;
   BrainModelSurfaceMetricOneAndPairedTTest
      oneSample(&bs,
                BrainModelSurfaceMetricOneAndPairedTTest::T_TEST_MODE_PAIRED,
                metricAndShapeFiles,
                fiducialCoordFileName,
                openTopoFileName,
                distortionMetricShapeFileName,
                outputTMapFileName,
                outputShuffledTMapFileName,
                outputPaintFileName,
                outputMetricFileName,
                outputReportFileName,
                distortionColumnNumber,
                negativeThreshold,
                positiveThreshold,
                pValue,
                varianceSmoothingIterations,
                varianceSmoothingStrength,
                iterations,
                0.0,
                numberOfThreads);
                
   try {
      oneSample.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
               
   return 0;
}

/*----------------------------------------------------------------------------------------
 *
 */
static int
operationMetricStatisticsOneSampleTTest(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE STATISTICS ONE SAMPLE T-TEST", "-metric-or-shape-stat-one-sample-t-test");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE STATISTICS ONE SAMPLE T-TEST" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-stat-one-sample-t-test \\ " << std::endl
         << "        <metric-or-shape-file> \\" << std::endl
         << "        <fiducial-coord-file> <open-topo-file> \\" << std::endl
         << "        <distortion-metric-shape-file> <distortion-column-number> \\" << std::endl
         << "        <output-tmap-metric-shape-file> \\" << std::endl
         << "        <output-shuffled-tmap-metric-shape-file> \\" << std::endl
         << "        <output-paint-file> <output-metric-file> <output-report-text-file> \\" << std::endl
         << "        <f-negative-threshold> <f-positive-threshold> \\" << std::endl
         << "        <f-p-value> \\ " << std::endl
         << "        <i-variance-smoothing-iterations> <f-variance-smoothing-strength> \\" << std::endl
         << "        <i-iterations> <f-t-test-constant> \\" << std::endl
         << "        <number-of-threads> " << std::endl
         << "        " << std::endl
         << "     Perform a one-sample T-Test on the metric/shape file." << std::endl
         << "     " << std::endl
         << "     The distortion column number starts at 1." << std::endl
         << "     " << std::endl
         << "     Number of threads is the number of concurrent processes run" << std::endl
         << "     during the cluster search of the shuffled metric/shape file." << std::endl
         << "     Use \"1\" if you are running on a single processor system." << std::endl
         << "     Users on systems with multiple processors or multi-core systems" << std::endl
         << "     should set the number of threads to the number of processors" << std::endl
         << "     and/or cores to reduce execution time." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   QString metricShapeFile;
   CommandLineUtilities::getNextParameter("Metric or Shape File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricShapeFile);

   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Fiducial Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);

   QString openTopoFileName;
   CommandLineUtilities::getNextParameter("Open Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          openTopoFileName);

   QString distortionMetricShapeFileName;
   CommandLineUtilities::getNextParameter("Distortion Metric/Shape File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionMetricShapeFileName);

   int distortionColumnNumber = 1;
   CommandLineUtilities::getNextParameter("Distortion Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionColumnNumber);
   if (distortionColumnNumber <= 0) {
      std::cout << "Distortion column number must be >= 1." << std::endl;
      exit(-1);
   }
   distortionColumnNumber--;   // algorithm starts at zero

   QString outputTMapFileName;
   CommandLineUtilities::getNextParameter("Output T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputTMapFileName);

   QString outputShuffledTMapFileName;
   CommandLineUtilities::getNextParameter("Output Shuffled T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputShuffledTMapFileName);

   QString outputPaintFileName;
   CommandLineUtilities::getNextParameter("Output Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputPaintFileName);

   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   QString outputReportFileName;
   CommandLineUtilities::getNextParameter("Output Report File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputReportFileName);

   float negativeThreshold;
   CommandLineUtilities::getNextParameter("Negative Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          negativeThreshold);

   float positiveThreshold;
   CommandLineUtilities::getNextParameter("Positive Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          positiveThreshold);

   float pValue;
   CommandLineUtilities::getNextParameter("P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pValue);

   int varianceSmoothingIterations;
   CommandLineUtilities::getNextParameter("Variance Smoothing Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceSmoothingIterations);

   float varianceSmoothingStrength;
   CommandLineUtilities::getNextParameter("Variance Smoothing Strength",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceSmoothingStrength);

   int iterations;
   CommandLineUtilities::getNextParameter("Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);

   float tTestConstant;
   CommandLineUtilities::getNextParameter("T-Test Constant",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          tTestConstant);

   int numberOfThreads;
   CommandLineUtilities::getNextParameter("Number of Threads",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfThreads);

   std::vector<QString> metricAndShapeFiles;
   metricAndShapeFiles.push_back(metricShapeFile);
   
   BrainSet bs;
   BrainModelSurfaceMetricOneAndPairedTTest
      oneSample(&bs,
                BrainModelSurfaceMetricOneAndPairedTTest::T_TEST_MODE_ONE_SAMPLE,
                metricAndShapeFiles,
                fiducialCoordFileName,
                openTopoFileName,
                distortionMetricShapeFileName,
                outputTMapFileName,
                outputShuffledTMapFileName,
                outputPaintFileName,
                outputMetricFileName,
                outputReportFileName,
                distortionColumnNumber,
                negativeThreshold,
                positiveThreshold,
                pValue,
                varianceSmoothingIterations,
                varianceSmoothingStrength,
                iterations,
                tTestConstant,
                numberOfThreads);
                
   try {
      oneSample.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
               
   return 0;
}

/*----------------------------------------------------------------------------------------
 *
 */
static int
operationMetricStatisticsInterhemisphericClusters(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE STATISTICS INTERHEMISPHERIC CLUSTERS", "-metric-or-shape-stat-interhemispheric-clusters");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE STATISTICS INTERHEMISPHERIC CLUSTERS" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-stat-interhemispheric-clusters \\ " << std::endl
         << "        <metric-or-shape-file-right-A> <metric-or-shape-file-right-B> \\" << std::endl
         << "        <metric-or-shape-file-left-A> <metric-or-shape-file-left-B> \\" << std::endl
         << "        <fiducial-coord-file> <open-topo-file> \\" << std::endl
         << "        <distortion-metric-shape-file> <distortion-column-number> \\" << std::endl
         << "        <output-right-tmap-metric-shape-file> \\ " << std::endl
         << "        <output-left-tmap-metric-shape-file> \\" << std::endl
         << "        <output-right-shuffled-tmap-metric-shape-file> \\ " << std::endl
         << "        <output-left-shuffled-tmap-metric-shape-file> \\" << std::endl
         << "        <output-tmap-metric-shape-file> \\ " << std::endl
         << "        <output-shuffled-tmap-metric-shape-file> \\ " << std::endl
         << "        <output-paint-file> <output-metric-file> <output-report-text-file> \\" << std::endl
         << "        <i-iterations-shuffled-tmap> <i-iterations-right-left-shuffled-tmap> \\" << std::endl 
         << "        <f-negative-threshold> <f-positive-threshold> <f-p-value> \\" << std::endl
         << "        <i-variance-smoothing-iterations> <f-variance-smoothing-strength> \\" << std::endl
         << "        <b-do-tmap-DOF> <b-do-tmap-pvalue> \\" << std::endl
         << "        <number-of-threads> " << std::endl
         << "        " << std::endl
         << "     Search for interhemispheric clusters." << std::endl
         << "     " << std::endl
         << "     The distortion column number starts at 1." << std::endl
         << "     " << std::endl
         << "     Number of threads is the number of concurrent processes run" << std::endl
         << "     during the cluster search of the shuffled metric/shape file." << std::endl
         << "     Use \"1\" if you are running on a single processor system." << std::endl
         << "     Users on systems with multiple processors or multi-core systems" << std::endl
         << "     should set the number of threads to the number of processors" << std::endl
         << "     and/or cores to reduce execution time." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }

   QString rightMetricShapeFileA;
   CommandLineUtilities::getNextParameter("Right Metric or Shape File A",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          rightMetricShapeFileA);

   QString rightMetricShapeFileB;
   CommandLineUtilities::getNextParameter("Right Metric or Shape File B",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          rightMetricShapeFileB);

   QString leftMetricShapeFileA;
   CommandLineUtilities::getNextParameter("Left Metric or Shape File A",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          leftMetricShapeFileA);

   QString leftMetricShapeFileB;
   CommandLineUtilities::getNextParameter("Left Metric or Shape File B",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          leftMetricShapeFileB);

   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Fiducial Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);

   QString openTopoFileName;
   CommandLineUtilities::getNextParameter("Open Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          openTopoFileName);

   QString distortionMetricShapeFileName;
   CommandLineUtilities::getNextParameter("Distortion Metric/Shape File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionMetricShapeFileName);

   int distortionColumnNumber = 1;
   CommandLineUtilities::getNextParameter("Distortion Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionColumnNumber);
   if (distortionColumnNumber <= 0) {
      std::cout << "Distortion column number must be >= 1." << std::endl;
      exit(-1);
   }
   distortionColumnNumber--;   // algorithm starts at zero

   QString outputRightTMapFileName;
   CommandLineUtilities::getNextParameter("Output Right T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputRightTMapFileName);

   QString outputLeftTMapFileName;
   CommandLineUtilities::getNextParameter("Output Left T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputLeftTMapFileName);

   QString outputRightShuffledTMapFileName;
   CommandLineUtilities::getNextParameter("Output Right Shuffled T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputRightShuffledTMapFileName);

   QString outputLeftShuffledTMapFileName;
   CommandLineUtilities::getNextParameter("Output Left Shuffled T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputLeftShuffledTMapFileName);

   QString outputTMapFileName;
   CommandLineUtilities::getNextParameter("Output T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputTMapFileName);

   QString outputShuffledTMapFileName;
   CommandLineUtilities::getNextParameter("Output Shuffled T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputShuffledTMapFileName);

   QString outputPaintFileName;
   CommandLineUtilities::getNextParameter("Output Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputPaintFileName);

   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   QString outputReportFileName;
   CommandLineUtilities::getNextParameter("Output Report File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputReportFileName);

   int iterations;
   CommandLineUtilities::getNextParameter("Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);

   int rightLeftIterations;
   CommandLineUtilities::getNextParameter("Right/Left Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          rightLeftIterations);

   float negativeThreshold;
   CommandLineUtilities::getNextParameter("Negative Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          negativeThreshold);

   float positiveThreshold;
   CommandLineUtilities::getNextParameter("Positive Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          positiveThreshold);

   float pValue;
   CommandLineUtilities::getNextParameter("P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pValue);

   int varianceSmoothingIterations;
   CommandLineUtilities::getNextParameter("Variance Smoothing Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceSmoothingIterations);

   float varianceSmoothingStrength;
   CommandLineUtilities::getNextParameter("Variance Smoothing Strength",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceSmoothingStrength);

   bool doTMapDOF;
   CommandLineUtilities::getNextParameter("Do T-Map DOF",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          doTMapDOF);

   bool doTMapPValue;
   CommandLineUtilities::getNextParameter("Do T-Map P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          doTMapPValue);

   int numberOfThreads;
   CommandLineUtilities::getNextParameter("Number of Threads",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfThreads);
                                          
   BrainSet bs;
   BrainModelSurfaceMetricInterHemClusters
      interHem(&bs,
               rightMetricShapeFileA,
               rightMetricShapeFileB,
               leftMetricShapeFileA,
               leftMetricShapeFileB,
               fiducialCoordFileName,
               openTopoFileName,
               distortionMetricShapeFileName,
               outputRightTMapFileName,
               outputLeftTMapFileName,
               outputRightShuffledTMapFileName,
               outputLeftShuffledTMapFileName,
               outputTMapFileName,
               outputShuffledTMapFileName,
               outputPaintFileName,
               outputMetricFileName,
               outputReportFileName,
               distortionColumnNumber,
               iterations,
               rightLeftIterations,
               negativeThreshold,
               positiveThreshold,
               pValue,
               varianceSmoothingIterations,
               varianceSmoothingStrength,
               doTMapDOF,
               doTMapPValue,
               numberOfThreads);
   try {
      interHem.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 *
 */
static int
operationMetricStatisticsAnovaOneWay(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE STATISTICS ONE-WAY ANOVA", 
                        "-metric-or-shape-stat-anova-one-way");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE STATISTICS ONE-WAY ANOVA" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-stat-anova-one-way \\ " << std::endl
         << "        <fiducial-coord-file> <open-topo-file> \\" << std::endl
         << "        <distortion-metric-shape-file> <distortion-column-number> \\" << std::endl
         << "        <output-f-map-metric-shape-file> \\" << std::endl
         << "        <output-shuffled-f-map-metric-shape-file> \\" << std::endl
         << "        <output-paint-file> <output-metric-file> <output-report-text-file> \\" << std::endl
         << "        <i-iterations> <f-positive-threshold> \\" << std::endl
         << "        <f-p-value> \\" << std::endl
         << "        <b-do-fmap-DOF> <b-do-fmap-pvalue> \\" << std::endl
         << "        <number-of-threads> \\" << std::endl
         << "        <metric-or-shape-files> " << std::endl
         << "        " << std::endl
         << "     Perform a one-way analysis of variance on the input metric files. " << std::endl
         << "     There must be at least two \"metric-or-shape-files\" and each of the" << std::endl
         << "     metric or shape files contains all of the subjects for one factor level." << std::endl
         << "     " << std::endl
         << "     The distortion column number starts at 1." << std::endl
         << "     " << std::endl
         << "     Number of threads is the number of concurrent processes run" << std::endl
         << "     during the cluster search of the shuffled metric/shape file." << std::endl
         << "     Use \"1\" if you are running on a single processor system." << std::endl
         << "     Users on systems with multiple processors or multi-core systems" << std::endl
         << "     should set the number of threads to the number of processors" << std::endl
         << "     and/or cores to reduce execution time." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Fiducial Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);

   QString openTopoFileName;
   CommandLineUtilities::getNextParameter("Open Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          openTopoFileName);

   QString distortionMetricShapeFileName;
   CommandLineUtilities::getNextParameter("Distortion Metric/Shape File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionMetricShapeFileName);

   int distortionColumnNumber = 1;
   CommandLineUtilities::getNextParameter("Distortion Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionColumnNumber);
   if (distortionColumnNumber <= 0) {
      std::cout << "Distortion column number must be >= 1." << std::endl;
      exit(-1);
   }
   distortionColumnNumber--;   // algorithm starts at zero

   QString outputFMapFileName;
   CommandLineUtilities::getNextParameter("Output F-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputFMapFileName);

   QString outputShuffledFMapFileName;
   CommandLineUtilities::getNextParameter("Output Shuffled F-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputShuffledFMapFileName);

   QString outputPaintFileName;
   CommandLineUtilities::getNextParameter("Output Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputPaintFileName);

   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   QString outputReportFileName;
   CommandLineUtilities::getNextParameter("Output Report File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputReportFileName);

   int iterations;
   CommandLineUtilities::getNextParameter("Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);

   float positiveThreshold;
   CommandLineUtilities::getNextParameter("Positive Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          positiveThreshold);

   float pValue;
   CommandLineUtilities::getNextParameter("P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pValue);

   bool doFMapDOF;
   CommandLineUtilities::getNextParameter("Do F-Map DOF",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          doFMapDOF);

   bool doFMapPValue;
   CommandLineUtilities::getNextParameter("Do F-Map P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          doFMapPValue);

   int numberOfThreads;
   CommandLineUtilities::getNextParameter("Number of Threads",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfThreads);

   std::vector<QString> metricShapeFiles;
   for (int i = argIndex; i < (argc - 1); i++) {
      QString filename;
      CommandLineUtilities::getNextParameter("Metric or Shape File",
                                                         argc,
                                                         argv,
                                                         true,
                                                         argIndex,
                                                         filename);
      metricShapeFiles.push_back(filename);
   }

   BrainSet bs;
   BrainModelSurfaceMetricAnovaOneWay
      anova(&bs,
                metricShapeFiles,
                fiducialCoordFileName,
                openTopoFileName,
                distortionMetricShapeFileName,
                outputFMapFileName,
                outputShuffledFMapFileName,
                outputPaintFileName,
                outputMetricFileName,
                outputReportFileName,
                distortionColumnNumber,
                iterations,
                positiveThreshold,
                pValue,
                doFMapDOF,
                doFMapPValue,
                numberOfThreads);
                
   try {
      anova.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
               
   return 0;
}


/*----------------------------------------------------------------------------------------
 *
 */
static int
operationMetricStatisticsTwoSampleTTest(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE STATISTICS TWO SAMPLE T-TEST and WILCOXON", "-metric-or-shape-stat-two-sample-t-test");
         return 0;
      }
      printSeparatorLine(); 
      std::cout
         << "   METRIC/SHAPE STATISTICS TWO SAMPLE T-TEST and WILCOXON" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-stat-two-sample-t-test \\ " << std::endl
         << "        <data-transform-mode-name> <variance-mode-name> \\ " << std::endl
         << "        <metric-or-shape-file-A> <metric-or-shape-file-B> \\" << std::endl
         << "        <fiducial-coord-file> <open-topo-file> \\" << std::endl
         << "        <distortion-metric-shape-file> <distortion-column-number> \\" << std::endl
         << "        <output-tmap-metric-shape-file> \\" << std::endl
         << "        <output-shuffled-tmap-metric-shape-file> \\" << std::endl
         << "        <output-paint-file> <output-metric-file> <output-report-text-file> \\" << std::endl
         << "        <i-iterations> <f-negative-threshold> <f-positive-threshold> \\" << std::endl
         << "        <f-p-value> \\" << std::endl
         << "        <i-variance-smoothing-iterations> <f-variance-smoothing-strength> \\" << std::endl
         << "        <b-do-tmap-DOF> <b-do-tmap-pvalue> \\" << std::endl
         << "        <number-of-threads> " << std::endl
         << "        " << std::endl
         << "     Perform a two-sample T-Test or perform a Wilcoxon Rank-Sum of the " << std::endl
         << "     data and then perform the T-Test." << std::endl
         << "     " << std::endl
         << "     data-transform-mode-name one of:" << std::endl
         << "        NO_TRANSFORM" << std::endl
         << "        WILCOXON_TRANSFORM  (Wilcoxon Rank-Sum input data then T-Test)" << std::endl
         << "     " << std::endl
         << "     variance-mode-name is one of:" << std::endl
         << "        SIGMA      - Use Donna's SIGMA method" << std::endl
         << "        POOLED     - Use pooled variance in T computation" << std::endl
         << "        UNPOOLED   - Use unpooled variance in T computation" << std::endl
         << "     " << std::endl
         << "     The distortion column number starts at 1." << std::endl
         << "     " << std::endl
         << "     Number of threads is the number of concurrent processes run" << std::endl
         << "     during the cluster search of the shuffled metric/shape file." << std::endl
         << "     Use \"1\" if you are running on a single processor system." << std::endl
         << "     Users on systems with multiple processors or multi-core systems" << std::endl
         << "     should set the number of threads to the number of processors" << std::endl
         << "     and/or cores to reduce execution time." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   QString dataTransformModeName;
   CommandLineUtilities::getNextParameter("Data Transform Mode Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          dataTransformModeName);
   BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_MODE dataTransformMode;
   if (dataTransformModeName == "NO_TRANSFORM") {
      dataTransformMode = BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_NONE;
   }
   else if (dataTransformModeName == "WILCOXON_TRANSFORM") {
      dataTransformMode = BrainModelSurfaceMetricTwoSampleTTest::DATA_TRANSFORM_WILCOXON_RANK_SUM_THEN_TWO_SAMPLE_T_TEST;
   }
   else {
      std::cout << "ERROR Invalid data transform mode: " << dataTransformModeName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   QString varianceModeName;
   CommandLineUtilities::getNextParameter("Variance Mode Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceModeName);
   BrainModelSurfaceMetricTwoSampleTTest::VARIANCE_MODE varianceMode;
   if (varianceModeName == "POOLED") {
      varianceMode = BrainModelSurfaceMetricTwoSampleTTest::VARIANCE_MODE_POOLED;
   }
   else if (varianceModeName == "UNPOOLED") {
      varianceMode = BrainModelSurfaceMetricTwoSampleTTest::VARIANCE_MODE_UNPOOLED;
   }
   else if (varianceModeName == "SIGMA") {
      varianceMode = BrainModelSurfaceMetricTwoSampleTTest::VARIANCE_MODE_SIGMA;
   }
   else {
      std::cout << "ERROR Invalid variance mode: " << varianceModeName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   QString metricShapeFileA;
   CommandLineUtilities::getNextParameter("Metric or Shape File A",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricShapeFileA);

   QString metricShapeFileB;
   CommandLineUtilities::getNextParameter("Metric or Shape File B",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricShapeFileB);

   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Fiducial Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);

   QString openTopoFileName;
   CommandLineUtilities::getNextParameter("Open Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          openTopoFileName);

   QString distortionMetricShapeFileName;
   CommandLineUtilities::getNextParameter("Distortion Metric/Shape File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionMetricShapeFileName);

   int distortionColumnNumber = 1;
   CommandLineUtilities::getNextParameter("Distortion Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionColumnNumber);
   if (distortionColumnNumber <= 0) {
      std::cout << "Distortion column number must be >= 1." << std::endl;
      exit(-1);
   }
   distortionColumnNumber--;   // algorithm starts at zero

   QString outputTMapFileName;
   CommandLineUtilities::getNextParameter("Output T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputTMapFileName);

   QString outputShuffledTMapFileName;
   CommandLineUtilities::getNextParameter("Output Shuffled T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputShuffledTMapFileName);

   QString outputPaintFileName;
   CommandLineUtilities::getNextParameter("Output Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputPaintFileName);

   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   QString outputReportFileName;
   CommandLineUtilities::getNextParameter("Output Report File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputReportFileName);

   int iterations;
   CommandLineUtilities::getNextParameter("Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);

   float negativeThreshold;
   CommandLineUtilities::getNextParameter("Negative Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          negativeThreshold);

   float positiveThreshold;
   CommandLineUtilities::getNextParameter("Positive Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          positiveThreshold);

   float pValue;
   CommandLineUtilities::getNextParameter("P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pValue);

   int varianceSmoothingIterations;
   CommandLineUtilities::getNextParameter("Variance Smoothing Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceSmoothingIterations);

   float varianceSmoothingStrength;
   CommandLineUtilities::getNextParameter("Variance Smoothing Strength",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          varianceSmoothingStrength);

   bool doTMapDOF;
   CommandLineUtilities::getNextParameter("Do T-Map DOF",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          doTMapDOF);

   bool doTMapPValue;
   CommandLineUtilities::getNextParameter("Do T-Map P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          doTMapPValue);

   int numberOfThreads;
   CommandLineUtilities::getNextParameter("Number of Threads",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfThreads);

   BrainSet bs;
   BrainModelSurfaceMetricTwoSampleTTest
      twoSample(&bs,
                dataTransformMode,
                varianceMode,
                metricShapeFileA,
                metricShapeFileB,
                fiducialCoordFileName,
                openTopoFileName,
                distortionMetricShapeFileName,
                outputTMapFileName,
                outputShuffledTMapFileName,
                outputPaintFileName,
                outputMetricFileName,
                outputReportFileName,
                distortionColumnNumber,
                iterations,
                negativeThreshold,
                positiveThreshold,
                pValue,
                varianceSmoothingIterations,
                varianceSmoothingStrength,
                doTMapDOF,
                doTMapPValue,
                numberOfThreads);
                
   try {
      twoSample.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
               
   return 0;
}
/*----------------------------------------------------------------------------------------
 *
 * Compute difference between all possible unordered pairs of metric columns
 */
static int
operationMetricInGroupDifference(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE IN GROUP DIFFERENCE", 
                        "-metric-or-shape-in-group-diff");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE IN GROUP DIFFERENCE" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-in-group-diff \\ " << std::endl
         << "        <input-metric-or-shape-file> <output-metric-or-shape-file> \\" << std::endl
         << "        <b-abs-value-flag> " << std::endl
         << "        " << std::endl
         << "     Compute the difference between each column in the metric/shape file." << std::endl
         << "     and all other columns in the metric shape file.  The number of columns" << std::endl
         << "     in the output metric file is [N! / (K! * (N - K)!)] where N is the." << std::endl
         << "     number of columns in the input metric/shape file and K is 2." << std::endl
         << "     " << std::endl
         << "     The abs-value-flag is either \"true\" or \"false\".  If the " << std::endl
         << "     abs-value-flag is true, the output is all absolute values of" << std::endl
         << "     the differences." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   QString inputMetricFileName;
   CommandLineUtilities::getNextParameter("Input Metric/Shape File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputMetricFileName);

   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric/Shape File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   bool absValueFlag;
   CommandLineUtilities::getNextParameter("Absolute Value Flag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          absValueFlag);
   
   BrainSet bs;
   BrainModelSurfaceMetricInGroupDifference diffAlg(&bs,
                                                    inputMetricFileName,
                                                    outputMetricFileName,
                                                    absValueFlag);
   try {
      diffAlg.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 *
 */
static int
operationMetricStatisticsCoordinateDifference(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE STATISTICS COORDINATE DIFFERENCE", "-metric-or-shape-stat-coord-diff");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE STATISTICS COORDINATE DIFFERENCE" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-stat-coord-diff \\ " << std::endl
         << "        <mode> <fiducial-coord-file> <open-topo-file> \\" << std::endl
         << "        <distortion-metric-shape-file> <distortion-column-number> \\" << std::endl
         << "        <output-distance-metric-shape-file> \\" << std::endl
         << "        <output-shuffled-distance-metric-shape-file> \\" << std::endl
         << "        <output-paint-file> <output-metric-file> <output-report-text-file> \\" << std::endl
         << "        <i-iterations> <f-threshold> <f-p-value> <i-number-of-threads> \\" << std::endl
         << "        -groupA <one-or-more-coord-files-from-group-A> \\" << std::endl
         << "        -groupB <one-or-more-coord-files-from-group-B> " << std::endl
         << "        " << std::endl
         << "     Find clusters in groups of coordinate files." << std::endl
         << "     " << std::endl
         << "     mode is one of:" << std::endl
         << "        COORD_DIFF - search for clusters in coordinate difference." << std::endl
         << "        TMAP_DIFF  - search for clusters using T-Test of coordinate difference." << std::endl
         << "     " << std::endl
         << "     The distortion column number starts at 1." << std::endl
         << "     " << std::endl
         << "     Number of threads is the number of concurrent processes run" << std::endl
         << "     during the cluster search of the shuffled metric/shape file." << std::endl
         << "     Use \"1\" if you are running on a single processor system." << std::endl
         << "     Users on systems with multiple processors or multi-core systems" << std::endl
         << "     should set the number of threads to the number of processors" << std::endl
         << "     and/or cores to reduce execution time." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   QString modeName;
   CommandLineUtilities::getNextParameter("Mode",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          modeName);
   BrainModelSurfaceMetricCoordinateDifference::MODE mode;
   if (modeName == "COORD_DIFF") {
      mode = BrainModelSurfaceMetricCoordinateDifference::MODE_COORDINATE_DIFFERENCE;
   }
   else if (modeName == "TMAP_DIFF") {
      mode = BrainModelSurfaceMetricCoordinateDifference::MODE_TMAP_DIFFERENCE;
   }
   else {
      std::cout << "ERROR INVALID MODE: " << modeName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   QString fiducialCoordFileName;
   CommandLineUtilities::getNextParameter("Fiducial Coordinate File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          fiducialCoordFileName);

   QString openTopoFileName;
   CommandLineUtilities::getNextParameter("Open Topology File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          openTopoFileName);

   QString distortionMetricShapeFileName;
   CommandLineUtilities::getNextParameter("Distortion Metric/Shape File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionMetricShapeFileName);

   int distortionColumnNumber = 1;
   CommandLineUtilities::getNextParameter("Distortion Column Number",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          distortionColumnNumber);
   if (distortionColumnNumber <= 0) {
      std::cout << "Distortion column number must be >= 1." << std::endl;
      exit(-1);
   }
   distortionColumnNumber--;   // algorithm starts at zero

   QString outputDistanceFileName;
   CommandLineUtilities::getNextParameter("Output Distance or T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputDistanceFileName);

   QString outputShuffledDistanceFileName;
   CommandLineUtilities::getNextParameter("Output Shuffled Distance or T-Map File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputShuffledDistanceFileName);

   QString outputPaintFileName;
   CommandLineUtilities::getNextParameter("Output Paint File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputPaintFileName);

   QString outputMetricFileName;
   CommandLineUtilities::getNextParameter("Output Metric File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputMetricFileName);

   QString outputReportFileName;
   CommandLineUtilities::getNextParameter("Output Report File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputReportFileName);

   int iterations;
   CommandLineUtilities::getNextParameter("Iterations",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          iterations);

   float threshold;
   CommandLineUtilities::getNextParameter("Threshold",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          threshold);

   float pValue;
   CommandLineUtilities::getNextParameter("P-Value",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          pValue);

   int numberOfThreads;
   CommandLineUtilities::getNextParameter("Number of Threads",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          numberOfThreads);
                                          
   std::vector<QString> coordFileNamesGroupA, coordFileNamesGroupB;
   bool doingGroupA = false;
   bool doingGroupB = false;
   for (int i = (argIndex + 1); i < argc; i++) {
      const QString arg(argv[i]);
      if (arg == "-groupA") {
         doingGroupA = true;
         doingGroupB = false;
      }
      else if (arg == "-groupB") {
         doingGroupB = true;
         doingGroupA = false;
      }
      else if (doingGroupA) {
         coordFileNamesGroupA.push_back(arg);
      }
      else if (doingGroupB) {
         coordFileNamesGroupB.push_back(arg);
      }
      else {
         std::cout << "Invalid parameter in groups of files: " << arg.toAscii().constData() << std::endl;
         exit(-1);
      }
   }

   BrainSet bs;
   
   BrainModelSurfaceMetricCoordinateDifference 
      coordDiffAlg(&bs,
                   mode,
                   coordFileNamesGroupA,
                   coordFileNamesGroupB,
                   fiducialCoordFileName,
                   openTopoFileName,
                   distortionMetricShapeFileName,
                   outputDistanceFileName,
                   outputShuffledDistanceFileName,
                   outputPaintFileName,
                   outputMetricFileName,
                   outputReportFileName,
                   distortionColumnNumber,
                   iterations,
                   threshold,
                   pValue,
                   numberOfThreads);
   try {
      coordDiffAlg.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 *  Twin comparison
 */
static int
operationMetricTwinComparison(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE TWIN COMPARISON", "-metric-or-shape-twin-comparison");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE TWIN COMPARISON" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-twin-comparison \\ " << std::endl
         << "     <input-metric-or-shape-file-A> <input-metric-or-shape-file-B> \\" << std::endl
         << "     <output-metric-shape-file>" << std::endl
         << std::endl
         << "     Metric/Shape File A and Metric/Shape File B contain twin subjects" << std::endl
         << "     such that one of the twins is in File A and its corresonding twin" << std::endl
         << "     is in the same column but in File B." << std::endl
         << "     The output file contains the twin-paired expected variance," << std::endl
         << "     the expected variance of all non-twin pairs, and the the difference" << std::endl
         << "     of the two expected variances." << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   QString metricFileAName;
   CommandLineUtilities::getNextParameter(" Metric/Shape File A Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricFileAName);

   QString metricFileBName;
   CommandLineUtilities::getNextParameter(" Metric/Shape File B File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricFileBName);

   QString metricFileOutputName;
   CommandLineUtilities::getNextParameter("Output Metric File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          metricFileOutputName);

   try {
      BrainSet bs;
      BrainModelSurfaceMetricTwinComparison bmsmtc(&bs,
                                                   metricFileAName,
                                                   metricFileBName,
                                                   metricFileOutputName);
      bmsmtc.execute();
   }
   catch (BrainModelAlgorithmException& e) {
      std::cout << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }

   return 0;
}

/*----------------------------------------------------------------------------------------
 *
 */
static int
operationMetricTwinPairedDataDiffs(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("METRIC/SHAPE FILE PAIRED DATA DIFFERENCES", "-metric-or-shape-twin-paired-data-diffs");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   METRIC/SHAPE FILE PAIRED DATA DIFFERENCES" << std::endl
         << "     " << programName.toAscii().constData() << " -metric-or-shape-twin-paired-data-diffs \\ " << std::endl
         << "     <input-metric-or-shape-file-group-A> <input-metric-or-shape-file-group-B> \\" << std::endl
         << "     <directory-for-diffs>" << std::endl
         << "     Compare the two data files, assuming a paired relationship in the" << std::endl
         << "     same column numbers in the two data files, e.g., the first pair of twins" << std::endl
         << "     are both the first column in data file A and data file B, respectively" << std::endl
         << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   
   QString inputAName;
   CommandLineUtilities::getNextParameter("Group A Input Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputAName);
   QString inputBName;
   CommandLineUtilities::getNextParameter("Group B Input Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          inputBName);
   QString outputDir;
   CommandLineUtilities::getNextParameter("Diffs Output Directory",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          outputDir);
   
   //
   // Process the files
   //
   try {
      char mycharbuff[50];
      MetricFile a, b, outtwin, outnontwin;
      a.readFile(inputAName);
      b.readFile(inputBName);
      //
      // Check assumption that both files have the same number of subjects
      // Reasonable assumption for twin/nontwin comparison, makes little sense to abandon it, though it is
      //  possible with a little rewriting
      // WARNING: if this assumption is abandoned, rewrite the loop comparing all subjects within the same
      //          files to two loops, one loop for each file.
      //
      if (a.getNumberOfColumns() != b.getNumberOfColumns()) throw FileException("Files must have the same number of subjects.");
      std::vector<float> adata, bdata;
      //
      // Allocate array neccesary to write to files
      //
      float* diff;
      GiftiDataArray* diffarray;
      int nodes;
      //
      // Loop through all columns in composite file A
      //
      for(int i = 0; i < a.getNumberOfColumns(); ++i)
      {
         a.getColumnForAllNodes(i, adata);
         nodes = adata.size();
         std::vector<int> nodesvect;
         nodesvect.clear();
         nodesvect.push_back(nodes);
	 //
	 // Loop through all columns in composite file B
         for(int j = 0; j < b.getNumberOfColumns(); ++j)
         {
	    //
	    // Allocate memory for a new column in the output files
	    //
            diffarray = new GiftiDataArray(NULL, "Shape");
            diffarray->setDataType(GiftiDataArray::DATA_TYPE_FLOAT32);
            diffarray->setDimensions(nodesvect);
            diff = diffarray->getDataPointerFloat();
            b.getColumnForAllNodes(j, bdata);
            //
	    // Check assumption that both files have the same number of nodes
	    // Note: because of how this is tested through each loop, it makes absolutely sure that
	    //       All columns have the same number of nodes, in both files.
	    //       This is partially for peace of mind, as the file format does not allow
	    //        different size columns
	    //
	    if (nodes != static_cast<int>(bdata.size())) throw FileException("Files must have the same number of nodes!");
	    //
	    // Compare all nodes between the two columns
	    // Both files must have the same number of nodes, in the same order
	    // Data must be registered to have the same # node mean the same place on the brain
	    //
	    for (int k = 0; k < nodes; ++k)
            {
               diff[k] = adata[k]-bdata[k];
            }
	    //
	    // Generate the column label, workaround for changing an int to a string
	    //
	    QString label;
            //
	    // Separate twin from nontwin data
	    //
	    if (i == j)
            {
               outtwin.addDataArray(diffarray);
	       sprintf(mycharbuff, "%d", i + 1);
	       label = "twins #";
	       label = label + (QString)mycharbuff;
               outtwin.setColumnName(i, label);
            } else {
               outnontwin.addDataArray(diffarray);
	       sprintf(mycharbuff, "%d", i + 1);
               label = "subjects ";
	       label = label + (QString)mycharbuff;
               label = label + "A and ";
	       sprintf(mycharbuff, "%d", j + 1);
               label = label + (QString)mycharbuff;
               label = label + "B";
               outnontwin.setColumnName(outnontwin.getNumberOfColumns() - 1, label);
            }
         }
      }
      //
      // Compare the subjects in each file against the others in the same file
      // Both composite files must have the same number of subjects, again with
      // the same number of nodes
      //
      // Allocate 2 more data columns to analyse both sets at the same time
      // If the assumption that the composite files have the same number of subjects
      //   is abandoned, rewrite to analyse each file separately
      //
      std::vector<float> cdata, ddata;
      //
      // Loop through all the subjects
      //
      for(int i = 0; i < a.getNumberOfColumns(); ++i)
      {
         a.getColumnForAllNodes(i, adata);
         b.getColumnForAllNodes(i, bdata);
         nodes = adata.size();
         std::vector<int> nodesvect;
         nodesvect.clear();
         nodesvect.push_back(nodes);
	 //
	 // Loop through all subjects below the current subject
	 //
         for(int j = i + 1; j < a.getNumberOfColumns(); ++j)
	 {
            diffarray = new GiftiDataArray(NULL, "Shape");
            diffarray->setDataType(GiftiDataArray::DATA_TYPE_FLOAT32);
            diffarray->setDimensions(nodesvect);
            diff = diffarray->getDataPointerFloat();
            a.getColumnForAllNodes(j, cdata);
            b.getColumnForAllNodes(j, ddata);
            //
	    // Compare the two subjects at those indexes in file A
	    //
	    for (int k = 0; k < nodes; ++k)
            {
               diff[k] = adata[k]-cdata[k];
            }
	    //
	    // Record data, label column, reallocate array
	    //
            outnontwin.addDataArray(diffarray);
	    sprintf(mycharbuff, "%d", i + 1);
            QString label = "subjects ";
	    label = label + (QString)mycharbuff;
            label = label + "A and ";
	    sprintf(mycharbuff, "%d", j + 1);
            label = label + (QString)mycharbuff;
            label = label + "A";
            outnontwin.setColumnName(outnontwin.getNumberOfColumns() - 1, label);
            diffarray = new GiftiDataArray(NULL, "Shape");
            diffarray->setDataType(GiftiDataArray::DATA_TYPE_FLOAT32);
            diffarray->setDimensions(nodesvect);
            diff = diffarray->getDataPointerFloat();
            //
	    // Compare the two subjects at those indexes in file B
	    // WARNING: the indexes are generated due to the number of subjects in file A ONLY
	    //          comparing two files with the second having fewer columns will cause a
	    //          null pointer error below at ddata[k], where if the second file is
	    //          bigger, not all comparisons in it will be made.
	    //
	    for (int k = 0; k < nodes; ++k)
            {
               diff[k] = bdata[k]-ddata[k];
            }
	    //
	    // Record data, label column
	    //
            outnontwin.addDataArray(diffarray);
	    sprintf(mycharbuff, "%d", i + 1);
            label = "subjects ";
	    label = label + (QString)mycharbuff;
            label = label + "B and ";
	    sprintf(mycharbuff, "%d", j + 1);
            label = label + (QString)mycharbuff;
            label = label + "B";
            outnontwin.setColumnName(outnontwin.getNumberOfColumns() - 1, label);
         }
      }
      //
      // Write output difference files
      // Note: writing outtwin can be moved to before the comparisons within files
      //       move above if you want more of an idea on progress
      //       when the cross comparison is finished, about 2/3 of the processing is done
      //
      outtwin.writeFile(outputDir + "/twin_diff.surface_shape");
      outnontwin.writeFile(outputDir + "/non_twin_diff.surface_shape");
   }
   catch (FileException& e) {
      std::cout << "ERROR" << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Display a QImage in a window
 */
static int
displayQImage(const QImage& qimage)
{
   //
   // Show the image
   //
   QLabel* label = new QLabel;
   label->setPixmap(QPixmap::fromImage(qimage));

   //
   // There is a limitation with the Mac OSX version of QT in that the program
   // must be built as an application for scrollbars to work correctly.  So, on
   // Mac OSX, simply show the image.
   //   
#ifdef Q_OS_MAC
   label->setFixedSize(qimage.width(), qimage.height());
   label->show();
#else // Q_OS_MAC
   QScrollArea* scrollArea = new QScrollArea;
   scrollArea->setWidget(label);
   scrollArea->show();
#endif // Q_OS_MAC

   //
   // Allow the event loop to execute
   //
   return myApplication->exec();
}

/*----------------------------------------------------------------------------------------
 * Compare two image files.
 */
static int
operationImageCompare(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("IMAGE COMPARE", "-image-compare");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   IMAGE COMPARE" << std::endl
         << "      " << programName.toAscii().constData() << " -image-compare \\" << std::endl
         << "         <image-file-name-1> <image-file-name-2> " << std::endl
         << std::endl
         << "      Compare two image files to see if the pixels are exactly the same." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString imageFileName1;
   CommandLineUtilities::getNextParameter("Image File Name 1",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          imageFileName1);
                                          
   QString imageFileName2;
   CommandLineUtilities::getNextParameter("Image File Name 2",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          imageFileName2);
                                          
   //
   // Read the images
   //
   ImageFile imageFile1, imageFile2;
   try {
      imageFile1.readFile(imageFileName1);
      imageFile2.readFile(imageFileName2);
   }
   catch (FileException& e) {
      std::cout << "Image Compare ERROR: " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Compare the images
   //
   QString comparisonMessage;
   const bool theSame = imageFile1.compareFileForUnitTesting(&imageFile2,
                                                             1.0,
                                                             comparisonMessage);
   std::cout << "IMAGE COMPARISON for "
             << FileUtilities::basename(imageFileName1).toAscii().constData()
             << " and "
             << FileUtilities::basename(imageFileName2).toAscii().constData()
             << " ";

   if (theSame) {
      std::cout << "successful." << std::endl;
   }
   else {
      std::cout << "FAILED." << std::endl;
      std::cout << "   " << comparisonMessage.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * View an image file.
 */
static int
operationImageView(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("IMAGE VIEW", "-image-view");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   IMAGE VIEW" << std::endl
         << "      " << programName.toAscii().constData() << " -image-view <image-file-name> " << std::endl
         << std::endl
         << "      View an image file." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString imageFileName;
   CommandLineUtilities::getNextParameter("Image File Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          imageFileName);
                                          
   //
   // Read the image
   //
   QImage image;
   if (image.load(imageFileName) == false) {
      std::cout << "ERROR: Unable to load \"" << imageFileName.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Show the image
   //
   return displayQImage(image);
}


/*----------------------------------------------------------------------------------------
 * Compare Caret data files.
 */
static int
operationCaretDataFileCompare(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("CARET DATA FILE COMPARISON", "-caret-data-file-compare");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   CARET DATA FILE COMPARISON" << std::endl
         << "      " << programName.toAscii().constData() << " -caret-data-file-compare <file-1-name> <file-2-name> [f-tolerance]" << std::endl
         << std::endl
         << "      Compare two Caret data files of the same type." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   // 
   QString dataFileName1;
   CommandLineUtilities::getNextParameter("Data File 1 Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          dataFileName1);
                                          
   QString dataFileName2;
   CommandLineUtilities::getNextParameter("Data File 2 Name",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          dataFileName2);
                                          
   float tolerance = 0.0;
   if (argc >= 5) {
      CommandLineUtilities::getNextParameter("Tolerance",
                                             argc,
                                             argv,
                                             true,
                                             argIndex,
                                             tolerance);
   }
   
   QString errorMessage;
   AbstractFile* af1 = AbstractFile::readAnySubClassDataFile(dataFileName1, false, errorMessage);
   if (af1 == NULL) {
      std::cout << "ERROR: " << errorMessage.toAscii().constData() << std::endl;
      exit(-1);
   }

   errorMessage = "";
   AbstractFile* af2 = AbstractFile::readAnySubClassDataFile(dataFileName2, false, errorMessage);
   if (af2 == NULL) {
      std::cout << "ERROR: " << errorMessage.toAscii().constData() << std::endl;
      delete af1;
      exit(-1);
   }
   
   QString diffMessage;
   const bool theSame = af1->compareFileForUnitTesting(af2, tolerance, diffMessage);
   
   std::cout << "---------------------------------------------------------------------" << std::endl;
   std::cout << "Comparison of " << FileUtilities::basename(dataFileName1).toAscii().constData() << " and " << std::endl
             << "              " << FileUtilities::basename(dataFileName2).toAscii().constData() << std::endl;
   if (theSame) {
      std::cout << "OK" << std::endl;
   }
   else {
      std::cout << "FAILED" << std::endl;
      std::cout << diffMessage.toAscii().constData() << std::endl;
      delete af1;
      delete af2;
      exit(-1);
   }
   std::cout << std::endl;
   
   delete af1;
   delete af2;
   
   return 0;
}

/*----------------------------------------------------------------------------------------
 * Unit testing of statistical algorithms
 */
static int
operationStatisticalUnitTesting(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("STATISTICS UNIT TESTING", "-statistic-unit-test");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   STATISTICS UNIT TESTING" << std::endl
         << "      " << programName.toAscii().constData() << " -statistic-unit-test <b-show-values-flag>" << std::endl
         << std::endl
         << "      Perform unit testing on statistics code. " << std::endl
         << std::endl
         << "      <b-show-values-flag> if \"true\" all test values are shown.  If " << std::endl
         << "      \"false\", only errors are displayed." << std::endl
         << std::endl;
      return 0;
   }
   
   bool showValuesFlag;
   CommandLineUtilities::getNextParameter("Show values flag",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          showValuesFlag);
   
   StatisticUnitTesting sut(showValuesFlag);
   try {
      sut.execute();
   }
   catch (StatisticException& e) {
      std::cout << e.whatStdString();
      exit(-1);
   }

   if (sut.getProblemsOccurredDuringTesting()) {
      exit(-1);
   }

   return 0;
}

/*----------------------------------------------------------------------------------------
 * Extract tables from an XHTML file.
 */
static int
operationFociXhtmlTableExtractor(int argc, char* argv[], int argIndex)
{
   if ((argIndex < 0) || (argc == 2)) {
      if (argIndex == HELP_BRIEF) {
         printBriefHelp("FOCI TABLE EXTRACTION FROM XHTML", "-foci-xhtml-table");
         return 0;
      }
      printSeparatorLine();
      std::cout
         << "   FOCI TABLE EXTRACTION FROM XHTML" << std::endl
         << "      " << programName.toAscii().constData() << " -foci-xhtml-table <xhtml-file> <csv-file>" << std::endl
         << std::endl
         << "      Read an XHTML file, extract the tables, and place " << std::endl
         << "      the tables into a Comma Separated Value File." << std::endl
         << std::endl;
      return 0;
   }
   
   //
   // Get the parameters
   //
   QString xhtmlFileName; 
   CommandLineUtilities::getNextParameter("XHTML File", 
                                          argc, 
                                          argv,
                                          true,
                                          argIndex,
                                          xhtmlFileName);
                                        
   QString csvFileName;
   CommandLineUtilities::getNextParameter("CSV File",
                                          argc,
                                          argv,
                                          true,
                                          argIndex,
                                          csvFileName);

   XhtmlTableExtractorFile xhtmlFile;
   try {
      xhtmlFile.readFile(xhtmlFileName);
      xhtmlFile.setFileWriteType(XhtmlTableExtractorFile::FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE);
      xhtmlFile.writeFile(csvFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
   }
   
   return 0;
}
                                          
/*----------------------------------------------------------------------------------------
 * Print help information.
 */
static void
printHelp(int argc, char* argv[], const int helpNumber)
{
   std::cout
      << std::endl
      << "NAME" << std::endl
      << "   " << programName.toAscii().constData()
      << "   v" << CaretVersion::getCaretVersionAsString().toAscii().constData() 
      << " (" << __DATE__ << ")" 
      << std::endl
      << "" << std::endl
      << "   For usage information run with \"-help\" or \"-help-full\"." << std::endl
      << "" << std::endl
      << "NOTE ABOUT PARAMETERS" << std::endl
      << "   Parameters beginning with \"b-\" are boolean parameters and should" << std::endl
      << "   have a value of either \"true\" or \"false\"." << std::endl
      << "" << std::endl
      << "   Parameters beginning with \"f-\" are floating point numbers." << std::endl
      << "" << std::endl
      << "   Parameters beginning with \"i-\" are integer parameters." << std::endl
      << "" << std::endl
      << "   Parameters containing \"name\" are names of files." << std::endl
      << "" << std::endl
      << "   All other parameters are descibed in the usage information." << std::endl
      << "" << std::endl
      << "   The input and output file names may be the same." << std::endl
      << "" << std::endl
      << "NOTE ABOUT OUTPUT VOLUME " << std::endl
      << "   The output volume file may be named the same as the input volume file." << std::endl
      << "   Volumes are typically written in LPI orientation." << std::endl
      << std::endl
      << "   If desired, a label for the output volume may be prepended to the output" << std::endl
      << "   volume name with the label and file name separated by \""
      <<                               outputVolumeDelimeter.toAscii().constData() << "\"." << std::endl
      << "   The label may not contain any spaces." << std::endl
      << "   Example:      the_label:::the_file_name.HEAD" << std::endl
      << std::endl
      << "   The type of output volume is determined by the output file's extension." << std::endl
      << "   Extension   Type of Volume Written" << std::endl
      << "   " << std::endl
      << "   .hdr        Analyze (SPM/MEDx)" << std::endl
      << "   .HEAD       AFNI" << std::endl
      << "   .ifh        Washington University Format" << std::endl
      << "   .mnc        MINC" << std::endl
      << "   .nii        NIFTI" << std::endl
      << "   " << std::endl
      << "DESCRIPTION OF OPERATIONS" << std::endl
      << "" << std::endl
      << "   HELP" << std::endl
      << "      " << programName.toAscii().constData() << " -help" << std::endl
      << "         Displays brief description of all available operations for this program." << std::endl
      << "" << std::endl
      << "      " << programName.toAscii().constData() << " -help-full" << std::endl
      << "         Displays full description of all available operations for this program." << std::endl
      << "" << std::endl;
   
   std::cout
      << "  For best results, run this program from the directory containing the files." << std::endl
      << "" << std::endl
      << "" << std::endl;

   operationCaretDataFileCompare(argc, argv, helpNumber);
   
   operationDeformationMapPathUpdate(argc, argv, helpNumber);
   
   operationFociXhtmlTableExtractor(argc, argv, helpNumber);
   
   operationImageCompare(argc, argv, helpNumber);
   
   operationImageView(argc, argv, helpNumber);
   
   operationMetricComposite(argc, argv, helpNumber);
   
   operationMetricCompositeNamedColumn(argc, argv, helpNumber);
   
   operationMetricCopyColumnsToNewFile(argc, argv, helpNumber);
   
   operationMetricInGroupDifference(argc, argv, helpNumber);
   
   operationMetricStatisticsCoordinateDifference(argc, argv, helpNumber);
   
   operationMetricCorrelationCoefficientMap(argc, argv, helpNumber);
   
   operationMetricFileInfo(argc, argv, helpNumber);
   
   operationMetricFileSetColumnName(argc, argv, helpNumber);
   
   operationMetricFileSetColumnToScalar(argc, argv, helpNumber);
   
   operationMetricFileCreate(argc, argv, helpNumber);
   
   operationMetricStatisticsAnovaOneWay(argc, argv, helpNumber);
   
   operationMetricStatisticsInterhemisphericClusters(argc, argv, helpNumber);
   
   operationMetricStatisticsOneSampleTTest(argc, argv, helpNumber);
   
   operationMetricStatisticsPairedTTest(argc, argv, helpNumber);
   
   operationMetricStatisticsTwoSampleTTest(argc, argv, helpNumber);
   
   operationMetricTwinComparison(argc, argv, helpNumber);
   
   operationMetricTwinPairedDataDiffs(argc, argv, helpNumber);
   
   operationShowScene(argc, argv, helpNumber);
   
   operationShowSurface(argc, argv, helpNumber);
   
   operationShowVolume(argc, argv, helpNumber);
   
   operationSpecFileAdd(argc, argv, helpNumber);
   
   operationSpecFileCreate(argc, argv, helpNumber);

   operationStatisticalUnitTesting(argc, argv, helpNumber);
   
   operationSurfaceDeformationApply(argc, argv, helpNumber);
   
   operationSurfaceAverage(argc, argv, helpNumber);
   
   operationSurfaceBorderExtrema(argc, argv, helpNumber);
   
   operationSurfaceBorderNibbler(argc, argv, helpNumber);
   
   operationSurfaceBorderProjection(argc, argv, helpNumber);
   
   operationSurfaceBorderUnprojection(argc, argv, helpNumber);
   
   operationSurfaceBorderVariability(argc, argv, helpNumber);
   
   operationSurfaceCellProjection(argc, argv, helpNumber, true);
   
   operationSurfaceCellUnprojection(argc, argv, helpNumber, true);
   
   operationSurfaceCrossoverCheck(argc, argv, helpNumber);
   
   operationSurfaceFlatMultiResMorphing(argc, argv, helpNumber); 
   
   operationSurfaceCellProjection(argc, argv, helpNumber, false); // foci
   
   operationSurfaceCellUnprojection(argc, argv, helpNumber, false); // foci 
   
   operationSurfaceInflatedEllipsoid(argc, argv, helpNumber);
   
   operationSurfaceIdentifySulci(argc, argv, helpNumber);
   
   operationSurfaceInformation(argc, argv, helpNumber);
   
   operationSurfaceMapGyriWithDepthROI(argc, argv, helpNumber);
   
   operationSurfaceSmoothArea(argc, argv, helpNumber);
   
   operationSurfaceRegistrationSpherical(argc, argv, helpNumber);
   
   operationSurfaceRoiCoordReport(argc, argv, helpNumber);
   
   operationSurfaceSphericalMultiResMorphing(argc, argv, helpNumber);
   
   operationSurfaceSulcalDepth(argc, argv, helpNumber);
   
   operationSurfaceTopologyDisconnectPaintNodes(argc, argv, helpNumber);
   
   operationSurfaceTopologyNeighbors(argc, argv, helpNumber);
   
   operationSurfaceToSegmentationVolume(argc, argv, helpNumber);
   
   operationSurfaceToVolume(argc, argv, helpNumber);
   
   operationSurfacesToSegmentationVolumeMask(argc, argv, helpNumber);
   
   operationVolumeAnatomyPeaks(argc, argv, helpNumber);
   
   operationVolumeBiasCorrection(argc, argv, helpNumber);
   
   operationVolumeBlur(argc, argv, helpNumber);
   
   operationVolumeClassifyIntensities(argc, argv, helpNumber);
   
   operationVolumeReplaceVoxelsWithVectorMagnitude(argc, argv, helpNumber);
   
   //operationVolumeClosing(argc, argv, helpNumber);
   
   operationVolumeConvertVectorToVolume(argc, argv, helpNumber);
   
   operationVolumeCopy(argc, argv, helpNumber);
   
   //operationVolumeCorrection(argc, argv, helpNumber);
   
   operationVolumeCreate(argc, argv, helpNumber);
   
   operationVolumeDilate(argc, argv, helpNumber);
   
   operationVolumeDilateErode(argc, argv, helpNumber);
   
   operationVolumeDilateErodeWithinMask(argc, argv, helpNumber);
   
   operationVolumeErode(argc, argv, helpNumber);
  
   operationVolumeEulerCount(argc, argv, helpNumber);
   
   operationVolumeFillBiggestObject(argc, argv, helpNumber);
   
   operationVolumeFillHoles(argc, argv, helpNumber);
   
   operationVolumeFillSlice(argc, argv, helpNumber);
   
   operationVolumeFindLimits(argc, argv, helpNumber);
   
   operationVolumeFloodFill(argc, argv, helpNumber);
   
   operationVolumeGradient(argc, argv, helpNumber);
   
   operationVolumeHistogram(argc, argv, helpNumber);
   
   operationVolumeInformation(argc, argv, helpNumber);
   
   operationVolumeIntersectVolumeWithSurface(argc, argv, helpNumber);
   
   operationVolumeMakePlane(argc, argv, helpNumber);
   
   operationVolumeMakeRectangle(argc, argv, helpNumber);
   
   operationVolumeMakeShell(argc, argv, helpNumber);
   
   operationVolumeMakeSphere(argc, argv, helpNumber);
   
   operationVolumeMathematics(argc, argv, helpNumber);
   
   operationVolumeMathematicsUnary(argc, argv, helpNumber);
   
   operationVolumeMaskVolume(argc, argv, helpNumber);
   
   operationVolumeNearToPlane(argc, argv, helpNumber);
    
   operationVolumePadVolume(argc, argv, helpNumber);
   
   operationVolumeRemoveIslands(argc, argv, helpNumber);
   
   operationVolumeResample(argc, argv, helpNumber);
   
   operationVolumeResize(argc, argv, helpNumber);
   
   operationVolumeRescaleVoxels(argc, argv, helpNumber);
   
   operationVolumeScalePercent0to255(argc, argv, helpNumber);
   
   operationVolumeScale0to255(argc, argv, helpNumber);
   
   operationVolumeSculpt(argc, argv, helpNumber);
   
   operationVolumeSegmentation(argc, argv, helpNumber);
   
   operationVolumeSegmentationStereotaxicSpace(argc, argv, helpNumber);
   
   operationVolumeSegmentMaskCreation(argc, argv, helpNumber);
   
   operationVolumeSetOrientation(argc, argv, helpNumber);
   
   operationVolumeSetOrigin(argc, argv, helpNumber);
   
   operationVolumeSetSpacing(argc, argv, helpNumber);
   
   operationVolumeShiftAxis(argc, argv, helpNumber);
   
   operationVolumeSmearAxis(argc, argv, helpNumber);
   
   operationVolumeThreshold(argc, argv, helpNumber);
   
   operationVolumeThresholdDual(argc, argv, helpNumber);
   
   operationVolumeThresholdInverse(argc, argv, helpNumber);
   
   operationVolumeVectorCombine(argc, argv, helpNumber);
   
   operationVolumeReplaceVectorMagnitudeWithVolume(argc, argv, helpNumber);
   
   std::cout  << "" << std::endl;
}

/*----------------------------------------------------------------------------------------
 * Unexpected handler
 */
void unexpectedHandler()
{
  std::cout << "WARNING: caret_command will be terminating due to an unexpected exception." << std::endl
            << "abort() will be called and a core file may be created." << std::endl;
  abort();
}

/*----------------------------------------------------------------------------------------
 * New handler
 */
void newHandler()
{
   std::cout << "\n"
             << "OUT OF MEMORY\n"
             << "\n"
             << "This means that Caret is unable to get memory that it needs.\n"
             << "Possible causes:\n"
             << "   (1) Your computer lacks sufficient RAM.\n"
             << "   (2) Swap space is too small (you might increase it)."
             << "   (3) Something is wrong with Caret."
             << std::endl;
   
   abort();
}

/*----------------------------------------------------------------------------------------
 * The main function.
 */
int
main(int argc, char* argv[])
{
   programName = FileUtilities::basename(argv[0]);
   
   if (argc < 2) {
      std::cout << "To see a list of all commands, run with \"-help\"." << std::endl
                << "To see a list of all commands with usage information run with \"-help-full\"."
                << "To see help for a specific command, specify the command with no parameters." << std::endl;
      exit(-1);
   }
   else {
      std::cerr << std::endl;
      std::cerr << "INFO: To see command line options, Run with \"-help\"" << std::endl;
      std::cerr << std::endl;
   }
   
   //
   // Set handlers in case sh*t happens
   //
   std::set_unexpected(unexpectedHandler);
   std::set_new_handler(newHandler);
   //
   // Check for debugging
   //
   DebugControl::setDebugOnWithEnvironmentVariable("CARET_DEBUG");
   const bool debugIsOn = DebugControl::getDebugOn();
   if (debugIsOn) {
      for (int i = 0; i < argc; i++) {
         std::cout << "arg " << i << ": " << argv[i] << std::endl;
      }
   }

   //
   // Get image plugins so JPEGs can be loaded
   //   
   Q_IMPORT_PLUGIN(QJpegPlugin)
   Q_IMPORT_PLUGIN(QGifPlugin)

   //
   // Command line QT application
   //
   bool guiFlag = false;
   const QString operation(argv[1]);   
   if ((operation == "-image-view") ||
       (operation == "-show-surface") ||
       (operation == "-show-scene")   ||
       (operation == "-show-volume")) {
      guiFlag = true;
   }
   QApplication app(argc, argv, guiFlag);
   myApplication = &app;
   
   //
   // Create a brain set
   //
   BrainSet brain;
   
   //
   // Brain set may turn debugging on via preferences file but do not let this happen
   //
   DebugControl::setDebugOn(debugIsOn);
   
   int result = 0;
   
   try {
      // 
      // Determine the operation
      //
      if ((operation == "-h") || (operation == "-help")) {
         printHelp(argc, argv, HELP_BRIEF);
         exit(0);
      }
      if ((operation == "-help-full")) {
         printHelp(argc, argv, HELP_FULL);
         exit(0);
      }
      else if (operation == "-caret-data-file-compare") {
         result = operationCaretDataFileCompare(argc, argv, 1);
      }
      else if (operation == "-def-map-path") {
         result = operationDeformationMapPathUpdate(argc, argv, 1);
      }
      else if (operation == "-foci-xhtml-table") {
         result = operationFociXhtmlTableExtractor(argc, argv, 1);
      }
      else if (operation == "-image-compare") {
         result = operationImageCompare(argc, argv, 1);
      }
      else if (operation == "-image-view") {
         result = operationImageView(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-composite") {
         result = operationMetricComposite(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-composite-named-column") {
         result = operationMetricCompositeNamedColumn(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-copy-columns-to-new-file") {
         result = operationMetricCopyColumnsToNewFile(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-correlation-map") {
         result = operationMetricCorrelationCoefficientMap(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-file-info") {
         result = operationMetricFileInfo(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-set-column-name") {
         result = operationMetricFileSetColumnName(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-set-column-to-value") {
         result = operationMetricFileSetColumnToScalar(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-create") {
         result = operationMetricFileCreate(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-stat-anova-one-way") {
         result = operationMetricStatisticsAnovaOneWay(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-stat-interhemispheric-clusters") {
         result = operationMetricStatisticsInterhemisphericClusters(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-stat-coord-diff") {
         result = operationMetricStatisticsCoordinateDifference(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-in-group-diff") {
         result = operationMetricInGroupDifference(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-stat-one-sample-t-test") {
         result = operationMetricStatisticsOneSampleTTest(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-stat-paired-t-test") {
         result = operationMetricStatisticsPairedTTest(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-stat-two-sample-t-test") {
         result = operationMetricStatisticsTwoSampleTTest(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-twin-comparison") {
         result = operationMetricTwinComparison(argc, argv, 1);
      }
      else if (operation == "-metric-or-shape-twin-paired-data-diffs") {
         result = operationMetricTwinPairedDataDiffs(argc, argv, 1);
      }
      else if (operation == "-spec-file-add") {
         result = operationSpecFileAdd(argc, argv, 1);
      }
      else if (operation == "-spec-file-create") {
         result = operationSpecFileCreate(argc, argv, 1);
      }
      else if (operation == "-statistic-unit-test") {
         result = operationStatisticalUnitTesting(argc, argv, 1);
      }
      else if (operation == "-surface-average") {
         result = operationSurfaceAverage(argc, argv, 1);
      }
      else if (operation == "-surface-border-extrema") {
         result = operationSurfaceBorderExtrema(argc, argv, 1);
      }
      else if (operation == "-surface-border-nibbler") {
         result = operationSurfaceBorderNibbler(argc, argv, 1);
      }
      else if (operation == "-surface-border-projection") {
         result = operationSurfaceBorderProjection(argc, argv, 1);
      }
      else if (operation == "-surface-border-unprojection") {
         result = operationSurfaceBorderUnprojection(argc, argv, 1);
      }
      else if (operation == "-surface-border-variability") {
         result = operationSurfaceBorderVariability(argc, argv, 1);
      }
      else if (operation == "-surface-cell-projection") {
         result = operationSurfaceCellProjection(argc, argv, 1, true);
      }
      else if (operation == "-surface-cell-unprojection") {
         result = operationSurfaceCellUnprojection(argc, argv, 1, true);
      }
      else if (operation == "-surface-crossover-check") {
         result = operationSurfaceCrossoverCheck(argc, argv, 1);
      }
      else if (operation == "-surface-deformation-apply") {
         result = operationSurfaceDeformationApply(argc, argv, 1);
      }
      else if (operation == "-surface-foci-projection") {
         result = operationSurfaceCellProjection(argc, argv, 1, false);
      }
      else if (operation == "-surface-foci-unprojection") {
         result = operationSurfaceCellUnprojection(argc, argv, 1, false);
      }
      else if (operation == "-surface-flat-multi-morph") {
         result = operationSurfaceFlatMultiResMorphing(argc, argv, 1);
      }
      else if (operation == "-surface-sphere-multi-morph") {
         result = operationSurfaceSphericalMultiResMorphing(argc, argv, 1);
      }
      else if (operation == "-surface-generate") {
         result = operationSurfaceInflatedEllipsoid(argc, argv, 1);
      }
      else if (operation == "-surface-identify-sulci") {
         result = operationSurfaceIdentifySulci(argc, argv, 1);
      }
      else if (operation == "-surface-information") {
         result = operationSurfaceInformation(argc, argv, 1);
      }
      else if (operation == "-surface-map-gyri-with-depth-roi") {
         result = operationSurfaceMapGyriWithDepthROI(argc, argv, 1);
      }
      else if (operation == "-surface-register-sphere") {
         result = operationSurfaceRegistrationSpherical(argc, argv, 1);
      }
      else if (operation == "-surface-to-volume") {
         result = operationSurfaceToVolume(argc, argv, 1);
      }
      else if (operation == "-surface-to-segmentation-volume") {
         result = operationSurfaceToSegmentationVolume(argc, argv, 1);
      }
      else if (operation == "-surfaces-to-segmentation-volume-mask") {
         result = operationSurfacesToSegmentationVolumeMask(argc, argv, 1);
      }
      else if (operation == "-show-surface") {
         result = operationShowSurface(argc, argv, 1);
      }
      else if (operation == "-show-volume") {
         result = operationShowVolume(argc, argv, 1);
      }
      else if (operation == "-show-scene") {
         result = operationShowScene(argc, argv, 1);
      }
      else if (operation == "-surface-roi-coord-report") {
         result = operationSurfaceRoiCoordReport(argc, argv, 1);
      }
      else if (operation == "-surface-smooth-area") {
         result = operationSurfaceSmoothArea(argc, argv, 1);
      }
      else if (operation == "-surface-sulcal-depth") {
         result = operationSurfaceSulcalDepth(argc, argv, 1);
      }
      else if (operation == "-surface-topology-disconnect-paint-nodes") {
         result = operationSurfaceTopologyDisconnectPaintNodes(argc, argv, 1);
      }
      else if (operation == "-surface-topology-neighbors") {
         result = operationSurfaceTopologyNeighbors(argc, argv, 1);
      }
      else if (operation == "-volume-anatomy-peaks") {
         result = operationVolumeAnatomyPeaks(argc, argv, 1);
      }
      else if (operation == "-volume-bias-correction") {
         result = operationVolumeBiasCorrection(argc, argv, 1);
      }
      else if (operation == "-volume-blur") {
         result = operationVolumeBlur(argc, argv, 1);
      }
      else if (operation == "-volume-classify-intensity") {
         result = operationVolumeClassifyIntensities(argc, argv, 1);
      }
      else if (operation == "-volume-convert-vector-to-volume") {
         result = operationVolumeConvertVectorToVolume(argc, argv, 1);
      }
      else if (operation == "-volume-closing") {
         result = operationVolumeClosing(argc, argv, 1);
      }
      else if (operation == "-volume-copy") {
         result = operationVolumeCopy(argc, argv, 1);
      }
      else if (operation == "-volume-copy-vector-mag") {
         result = operationVolumeReplaceVoxelsWithVectorMagnitude(argc, argv, 1);
      }
      else if (operation == "-volume-create") {
         result = operationVolumeCreate(argc, argv, 1);
      }
      else if (operation == "-volume-dilate") {
         result = operationVolumeDilate(argc, argv, 1);
      }
      else if (operation == "-volume-erode") {
         result = operationVolumeErode(argc, argv, 1);
      }
      else if (operation == "-volume-dilate-erode") {
         result = operationVolumeDilateErode(argc, argv, 1);
      }
      else if (operation == "-volume-dilate-erode-mask") {
         result = operationVolumeDilateErodeWithinMask(argc, argv, 1);
      }
      else if (operation == "-volume-euler") {
         result = operationVolumeEulerCount(argc, argv, 1);
      }
      else if (operation == "-volume-fill-biggest-object") {
         result = operationVolumeFillBiggestObject(argc, argv, 1);
      }
      else if (operation == "-volume-fill-holes") {
         result = operationVolumeFillHoles(argc, argv, 1);
      }
      else if (operation == "-volume-fill-slice") {
         result = operationVolumeFillSlice(argc, argv, 1);
      }
      else if (operation == "-volume-find-limits") {
         result = operationVolumeFindLimits(argc, argv, 1);
      }
      else if (operation == "-volume-flood-fill") {
         result = operationVolumeFloodFill(argc, argv, 1);
      }
      else if (operation == "-volume-gradient") {
         result = operationVolumeGradient(argc, argv, 1);
      }
      else if (operation == "-volume-histogram") {
         result = operationVolumeHistogram(argc, argv, 1);
      }
      else if ((operation == "-volume-info") || (operation == "-volume-information")) {
         result = operationVolumeInformation(argc, argv, 1);
      }
      else if (operation == "-volume-intersect-volume-with-surface") {
         result = operationVolumeIntersectVolumeWithSurface(argc, argv, 1);
      }
      else if (operation == "-volume-make-plane") {
         result = operationVolumeMakePlane(argc, argv, 1);
      }
      else if (operation == "-volume-make-rectangle") {
         result = operationVolumeMakeRectangle(argc, argv, 1);
      }
      else if (operation == "-volume-make-shell") {
         result = operationVolumeMakeShell(argc, argv, 1);
      }
      else if (operation == "-volume-make-sphere") {
         result = operationVolumeMakeSphere(argc, argv, 1);
      }
      else if (operation == "-volume-mask-volume") {
         result = operationVolumeMaskVolume(argc, argv, 1);
      }
      else if (operation == "-volume-math") {
         result = operationVolumeMathematics(argc, argv, 1);
      }
      else if (operation == "-volume-math-unary") {
         result = operationVolumeMathematicsUnary(argc, argv, 1);
      }
      else if (operation == "-volume-near-to-plane") {
         result = operationVolumeNearToPlane(argc, argv, 1);
      }
      else if (operation == "-volume-pad-volume") {
         result = operationVolumePadVolume(argc, argv, 1);
      }
      else if (operation == "-volume-remove-islands") {
         result = operationVolumeRemoveIslands(argc, argv, 1);
      }
      else if (operation == "-volume-resample") {
         result = operationVolumeResample(argc, argv, 1);
      }
      else if (operation == "-volume-resize") {
         result = operationVolumeResize(argc, argv, 1);
      }
      else if (operation == "-volume-rescale") {
         result = operationVolumeRescaleVoxels(argc, argv, 1);
      }
      else if (operation == "-volume-scale-percent-min-max-255") {
         result = operationVolumeScalePercent0to255(argc, argv, 1);
      }
      else if (operation == "-volume-scale-255") {
         result = operationVolumeScale0to255(argc, argv, 1);
      }
      else if (operation == "-volume-sculpt") {
         result = operationVolumeSculpt(argc, argv, 1);
      }
      else if (operation == "-volume-segment") {
         result = operationVolumeSegmentation(argc, argv, 1);
      }
      else if (operation == "-volume-segment-stereo-space") {
         result = operationVolumeSegmentationStereotaxicSpace(argc, argv, 1);
      }
      else if (operation == "-volume-segment-mask-creation") {
         result = operationVolumeSegmentMaskCreation(argc, argv, 1);
      }
      else if (operation == "-volume-set-orientation") {
         result = operationVolumeSetOrientation(argc, argv, 1);
      }
      else if (operation == "-volume-set-origin") {
         result = operationVolumeSetOrigin(argc, argv, 1);
      }
      else if (operation == "-volume-set-spacing") {
         result = operationVolumeSetSpacing(argc, argv, 1);
      }
      else if (operation == "-volume-shift-axis") {
         result = operationVolumeShiftAxis(argc, argv, 1);
      }
      else if (operation == "-volume-smear-axis") {
         result = operationVolumeSmearAxis(argc, argv, 1);
      }
      else if (operation == "-volume-threshold") {
         result = operationVolumeThreshold(argc, argv, 1);
      }
      else if (operation == "-volume-threshold-dual") {
         result = operationVolumeThresholdDual(argc, argv, 1);
      }
      else if (operation == "-volume-threshold-inverse") {
         result = operationVolumeThresholdInverse(argc, argv, 1);
      }
      else if (operation == "-volume-vector-combine") {
         result = operationVolumeVectorCombine(argc, argv, 1);
      }
      else if (operation == "-volume-vector-replace-mag-volume") {
         result = operationVolumeReplaceVectorMagnitudeWithVolume(argc, argv, 1);
      }
      else {
         std::cout << "ERROR: Unrecognized operation: " << operation.toAscii().constData() << std::endl;
         std::cout << std::endl;
         exit(-1);
      }
   }   
   catch (BrainModelAlgorithmException& e) {      
      QString msg("PROGRAM ERROR: BrainModelAlgorithmException not caught (this should not happen):\n"
                  "Please copy the contents of this message and send to john@brainvis.wustl.edu.");
      msg.append(e.whatQString());      
      std::cerr << msg.toAscii().constData() << std::endl;  
      result = -1; 
   }
   catch (FileException& e) {      
      QString msg("PROGRAM ERROR: FileException not caught (this should not happen):\n"
                  "Please copy the contents of this message and send to john@brainvis.wustl.edu.");
      msg.append(e.whatQString());
      std::cerr << msg.toAscii().constData() << std::endl;
      result = -1; 
   }
   catch (std::exception& e) {
      QString msg("PROGRAM ERROR: std::exception not caught (this should not happen):\n"
                  "Please copy the contents of this message and send to john@brainvis.wustl.edu.");
      msg.append(e.what());
      std::cerr << msg.toAscii().constData() << std::endl;
      result = -1; 
   }
      
   return result;
}

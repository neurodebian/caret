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

#define DEFORMATION_MAP_FILE_DEFINE
#include "DeformationMapFile.h"
#undef  DEFORMATION_MAP_FILE_DEFINE

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

#include "FileUtilities.h"
#include "SpecFile.h"

/**
 * Constructor
 */
DeformMapNodeData::DeformMapNodeData()
{
   tileNodes[0] = -1;
   tileNodes[1] = -1;
   tileNodes[2] = -1;
   tileBarycentric[0] = 0.0;
   tileBarycentric[1] = 0.0;
   tileBarycentric[2] = 0.0;
}

/**
 * Constructor
 */
DeformationMapFile::DeformationMapFile()
   : AbstractFile("Deformation Map File", 
                  SpecFile::getDeformationMapFileExtension(),
                  true, 
                  FILE_FORMAT_ASCII, 
                  FILE_IO_READ_AND_WRITE, 
                  FILE_IO_READ_AND_WRITE, 
                  FILE_IO_NONE,
                  FILE_IO_NONE)
{
   clear();
}

/**
 * Destructor
 */
DeformationMapFile::~DeformationMapFile()
{
   clear();
}

/**
 * compare a file for unit testing (returns true if "within tolerance").
 */
bool 
DeformationMapFile::compareFileForUnitTesting(const AbstractFile* af,
                                              const float tolerance,
                                              QString& messageOut) const
{
   messageOut = ""; 
    
   const DeformationMapFile* dmf = dynamic_cast<const DeformationMapFile*>(af);
   if (dmf == NULL) {
      messageOut += "ERROR: File for comparison is not a Deformation Map File.\n";
      return false;
   }
   
   const int numNodes = getNumberOfNodes();
   if (numNodes != dmf->getNumberOfNodes()) {
      messageOut += "ERROR number of nodes do not match for file comparison.";
      return false;
   }
   
   for (int i = 0; i < numNodes; i++) {
      int tilesMe[3], tilesOther[3];
      float areasMe[3], areasOther[3];
      getDeformDataForNode(i, tilesMe, areasMe);
      dmf->getDeformDataForNode(i, tilesOther, areasOther);
      
      for (int i = 0; i < 3; i++) {
         if (tilesMe[i] != tilesOther[i]) {
            messageOut += "ERROR: There are tiles that do not match between the files.\n";
            return false;
         }
         const float diff = std::fabs(areasMe[i] != areasOther[i]);
         if (diff > tolerance) {
            messageOut +="ERROR: tile areas are not within tolerance.";
            return false;
         }
      }
   }
   
   return true;
}

/**
 * clear the contents of the file
 */
void
DeformationMapFile::clear()
{
   clearAbstractFile();
   
   flatOrSphereSelection       = DEFORMATION_TYPE_SPHERE;
   sphereResolution            = 4610;   //18434;
   borderResampleType          = BORDER_RESAMPLING_VALUE;
   borderResampleValue         = 10.0;   //15.0;
   sphericalNumberOfCycles     = 3; //1;
   for (int i = 0; i < MAX_SPHERICAL_CYCLES; i++) {
      smoothingStrength[i]           = 1.0;
      smoothingCycles[i]             = 100;
      smoothingIterations[i]         = 20;
      smoothingNeighborIterations[i] = 10;
      smoothingFinalIterations[i]    = 5;
      morphingCycles[i]              = 1;
      morphingLinearForce[i]         = 0.3;
      morphingAngularForce[i]        = 0.4;
      morphingStepSize[i]            = 0.5;
      morphingLandmarkStepSize[i]    = 0.5;
      morphingIterations[i]          = 80;
      morphingSmoothIterations[i]    = 5;
   }   

   smoothingStrength[0]           = 1.0;
   smoothingCycles[0]             = 100;
   smoothingIterations[0]         = 20;
   smoothingNeighborIterations[0] = 10;
   smoothingFinalIterations[0]    = 30;
   morphingCycles[0]              = 1;
   morphingLinearForce[0]         = 0.3;
   morphingAngularForce[0]        = 0.6;
   morphingStepSize[0]            = 0.5;
   morphingLandmarkStepSize[0]    = 0.5;
   morphingIterations[0]          = 300;
   morphingSmoothIterations[0]    = 20;

   smoothingStrength[1]           = 1.0;
   smoothingCycles[1]             = 100;
   smoothingIterations[1]         = 20;
   smoothingNeighborIterations[1] = 10;
   smoothingFinalIterations[1]    = 5;
   morphingCycles[1]              = 1;
   morphingLinearForce[1]         = 0.3;
   morphingAngularForce[1]        = 0.6;
   morphingStepSize[1]            = 0.5;
   morphingLandmarkStepSize[1]    = 0.5;
   morphingIterations[1]          = 300;
   morphingSmoothIterations[1]    = 5;

   smoothingStrength[2]           = 1.0;
   smoothingCycles[2]             = 50;
   smoothingIterations[2]         = 20;
   smoothingNeighborIterations[2] = 10;
   smoothingFinalIterations[2]    = 1;
   morphingCycles[2]              = 1;
   morphingLinearForce[2]         = 0.3;
   morphingAngularForce[2]        = 0.6;
   morphingStepSize[2]            = 0.5;
   morphingLandmarkStepSize[2]    = 0.5;
   morphingIterations[2]          = 300;
   morphingSmoothIterations[2]    = 2;

   fiducialSphereRatio = 0.5;
   fiducialSphereRatioEnabled = true;
   
   flatSubSamplingTiles = 900;
   flatBeta = 0.00001;
   flatVarMult = 1.0;
   flatNumIters = 20;
   
   fileVersion = 2;
   
   deformData.clear();
 
   const QString noValue("");
   setSourceDirectory(noValue);
   setSourceSpecFileName(noValue);
   setSourceBorderFileName(noValue, BORDER_FILE_UNKNOWN);
   setSourceClosedTopoFileName(noValue);
   setSourceCutTopoFileName(noValue);
   setSourceFiducialCoordFileName(noValue);
   setSourceSphericalCoordFileName(noValue);
   setSourceFlatCoordFileName(noValue);
   setSourceDeformedSphericalCoordFileName(noValue);
   setSourceDeformedFlatCoordFileName(noValue);
   setSourceResampledFlatCoordFileName(noValue);
   setSourceResampledDeformedFlatCoordFileName(noValue);
   setSourceResampledCutTopoFileName(noValue);
   
   setTargetDirectory(noValue);
   setTargetSpecFileName(noValue);
   setTargetBorderFileName(noValue, BORDER_FILE_UNKNOWN);
   setTargetClosedTopoFileName(noValue);
   setTargetCutTopoFileName(noValue);
   setTargetFiducialCoordFileName(noValue);
   setTargetSphericalCoordFileName(noValue);
   setTargetFlatCoordFileName(noValue);
   
   setOutputSpecFileName(noValue);
   
   inverseDeformationFlag = false;
   deleteIntermediateFilesFlag = true;
   deformBothWaysFlag = true;
   metricDeformationType = METRIC_DEFORM_NEAREST_NODE;
   deformedFileNamePrefix = "deformed_";
   deformedColumnNamePrefix = "deformed_";
   smoothDeformedSurfacesFlag = true;
}

/**
 * Get the deformation for a node.
 */
void 
DeformationMapFile::getDeformDataForNode(const int nodeNumber, 
                                        int tileNodesOut[3],
                                        float tileBarycentricOut[3]) const
{
   for (int i = 0; i < 3; i++) {
      tileNodesOut[i]       = deformData[nodeNumber].tileNodes[i];
      tileBarycentricOut[i] = deformData[nodeNumber].tileBarycentric[i];
   }
}

/**
 * Set the deformation for a node.
 */
void 
DeformationMapFile::setDeformDataForNode(const int nodeNumber, 
                                        const int tileNodesIn[3],
                                        const float tileBarycentricIn[3])
{
   for (int i = 0; i < 3; i++) {
      deformData[nodeNumber].tileNodes[i]       = tileNodesIn[i];
      deformData[nodeNumber].tileBarycentric[i] = tileBarycentricIn[i];
   }
   setModified();
}

/**
 * Set the number of nodes for the file.
 */
void
DeformationMapFile::setNumberOfNodes(const int numNodesIn)
{
   deformData.resize(numNodesIn);
   setModified();
}

/**
 * Make a filname relative to that "path".
 */
void
DeformationMapFile::makeFileRelative(const QString& path,
                                     QString& name)
{
   if (name.isEmpty() == false) {
      //printf("... %s is now: ", name);
      if (name[0] != '/') {
         return;
      }
      const QString filename(FileUtilities::basename(name));
      QString filepath(FileUtilities::dirname(name));
      QString result;
      FileUtilities::relativePath(filepath, path, result);
      if (result.isEmpty() == false) {
          QString s(result);
          s.append("/");
          s.append(filename);
          name = s;
      }
      else {
         name = filename;
      }
      //printf("%s\n", name);
   }
}

/**
 * Make the source files relative to a path.
 */
void
DeformationMapFile::makeSourceFilesRelativeToPath(const QString& path)
{
   makeFileRelative(path, sourceSpecFileName);
   makeFileRelative(path, sourceBorderFileName);
   makeFileRelative(path, sourceClosedTopoFileName);
   makeFileRelative(path, sourceCutTopoFileName);
   makeFileRelative(path, sourceFiducialCoordFileName);
   makeFileRelative(path, sourceSphericalCoordFileName);
   makeFileRelative(path, sourceDeformedSphericalCoordFileName);
   makeFileRelative(path, sourceDeformedFlatCoordFileName);
   makeFileRelative(path, sourceFlatCoordFileName);
   makeFileRelative(path, sourceResampledFlatCoordFileName);
   makeFileRelative(path, sourceResampledDeformedFlatCoordFileName);
   makeFileRelative(path, sourceResampledCutTopoFileName);
   setModified();
}
 
/**
 * Make the target files relative to a path.
 */
void
DeformationMapFile::makeTargetFilesRelativeToPath(const QString& path)
{
   makeFileRelative(path, targetSpecFileName);
   makeFileRelative(path, targetBorderFileName);
   makeFileRelative(path, targetClosedTopoFileName);
   makeFileRelative(path, targetCutTopoFileName);
   makeFileRelative(path, targetFiducialCoordFileName);
   makeFileRelative(path, targetSphericalCoordFileName);
   makeFileRelative(path, targetFlatCoordFileName);
   makeFileRelative(path, outputSpecFileName);
   setModified();
}
 
/**
 * make file name's relative paths.
 */
void 
DeformationMapFile::makePathsRelative(const QString& path)
{
   makeSourceFilesRelativeToPath(path);
   makeTargetFilesRelativeToPath(path);
}

/**
 * Swap the source files with the target files.
 */
void
DeformationMapFile::swapSourceAndTargetFiles()
{
   std::swap(sourceDirectoryName,          targetDirectoryName);
   std::swap(sourceSpecFileName,           targetSpecFileName);
   std::swap(sourceBorderFileName,         targetBorderFileName);
   std::swap(sourceBorderFileType,         targetBorderFileType);
   std::swap(sourceClosedTopoFileName,     targetClosedTopoFileName);
   std::swap(sourceCutTopoFileName,        targetCutTopoFileName);
   std::swap(sourceFiducialCoordFileName,  targetFiducialCoordFileName);
   std::swap(sourceSphericalCoordFileName, targetSphericalCoordFileName);
   std::swap(sourceFlatCoordFileName,      targetFlatCoordFileName);
}

/**
 * Read the deformation map file's data.
 */
void
DeformationMapFile::readFileData(QFile& /*file*/, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) 
                                                    throw (FileException)
{
   //
   // first section of deformation file contains a list of files that
   // were used to perform deformation.
   //
   bool readingFiles = true;
   QString lastLineRead;
   while(readingFiles) {
      //
      // Read a line from the file
      //
      QString tag, tagValue;
      readTagLine(stream, lastLineRead, tag, tagValue);
      
      if (tag == startOfDataTag) {
         readingFiles = false;
      }
      else if (tag == deformMapFileVersion) {
         fileVersion = QString(tagValue).toInt();
      }
      else if (tag == deformedFileNamePrefixTag) {
         setDeformedFileNamePrefix(tagValue);
      }
      else if (tag == deformedColumnNamePrefixTag) {
         setDeformedColumnNamePrefix(tagValue);
      }
      else if (tag == sourceDirectoryTag) {
         setSourceDirectory(tagValue);
      }
      else if (tag == sourceSpecTag) {
         setSourceSpecFileName(tagValue);
      }
      else if (tag == sourceBorderTag) {
         setSourceBorderFileName(tagValue, BORDER_FILE_UNKNOWN);
      }
      else if (tag == sourceClosedTopoTag) {
         setSourceClosedTopoFileName(tagValue);
      }
      else if (tag == sourceCutTopoTag) {
         setSourceCutTopoFileName(tagValue);
      }
      else if (tag == sourceFiducialCoordTag) {
         setSourceFiducialCoordFileName(tagValue);
      }
      else if (tag == sourceSphericalCoordTag) {
         setSourceSphericalCoordFileName(tagValue);
      }
      else if (tag == sourceDeformedSphericalCoordTag) {
         setSourceDeformedSphericalCoordFileName(tagValue);
      }
      else if (tag == sourceDeformedFlatCoordTag) {
         setSourceDeformedFlatCoordFileName(tagValue);
      }
      else if (tag == sourceFlatCoordTag) {
         setSourceFlatCoordFileName(tagValue);
      }
      else if (tag == sourceResampledFlatCoordTag) {
         setSourceResampledFlatCoordFileName(tagValue);
      }
      else if (tag == sourceResampledDeformedFlatCoordTag) {
         setSourceResampledDeformedFlatCoordFileName(tagValue);
      }
      else if (tag == sourceResampledCutTopoTag) {
         setSourceResampledCutTopoFileName(tagValue);
      }
      else if (tag == targetDirectoryTag) {
         setTargetDirectory(tagValue);
      }
      else if (tag == targetSpecTag) {
         setTargetSpecFileName(tagValue);
      }
      else if (tag == targetBorderTag) {
         setTargetBorderFileName(tagValue, BORDER_FILE_UNKNOWN);
      }
      else if (tag == targetClosedTopoTag) {
         setTargetClosedTopoFileName(tagValue);
      }
      else if (tag == targetCutTopoTag) {
         setTargetCutTopoFileName(tagValue);
      }
      else if (tag == targetFiducialCoordTag) {
         setTargetFiducialCoordFileName(tagValue);
      }
      else if (tag == targetSphericalCoordTag) {
         setTargetSphericalCoordFileName(tagValue);
      }
      else if (tag == targetFlatCoordTag) {
         setTargetFlatCoordFileName(tagValue);
      }
      else if (tag == outputSpecFileTag) {
         setOutputSpecFileName(tagValue);
      }
      else if (tag == sphereResolutionTag) {
         sphereResolution = QString(tagValue).toInt();
      }
      else if (tag == flatOrSphereSelectionTag) {
         if (tagValue == DeformationFlatValue) {
            flatOrSphereSelection = DEFORMATION_TYPE_FLAT;
         }
         else if (tagValue == DeformationSphereValue) {
            flatOrSphereSelection = DEFORMATION_TYPE_SPHERE;
         }
         else {
            QString msg("ERROR invalid deformation flat/sphere value ");
            msg.append(tagValue);
            throw FileException(filename, msg);
         }
      }
      else if (tag == borderResampleTag) {
         QTextStream textStream(&tagValue, QIODevice::ReadOnly);
         int resampleTypeInt;
         textStream >> resampleTypeInt
              >> borderResampleValue;
         borderResampleType = static_cast<BORDER_RESAMPLING_TYPE>(resampleTypeInt);
      }
      else if (tag == sphericalNumberOfCyclesTag) {
         QTextStream textStream(&tagValue, QIODevice::ReadOnly);
         textStream >> sphericalNumberOfCycles;
      }
      else if (tag == smoothingParamtersTag) {
         if (fileVersion > 0) {
            int cycleNumber;
            float strength;
            int cycles;
            int iterations;
            int neighborIterations;
            int finalIterations;

            QTextStream textStream(&tagValue, QIODevice::ReadOnly);
            textStream >> cycleNumber
                 >> strength
                 >> cycles
                 >> iterations
                 >> neighborIterations
                 >> finalIterations;

            smoothingStrength[cycleNumber]           = strength;
            smoothingCycles[cycleNumber]             = cycles;
            smoothingIterations[cycleNumber]         = iterations;
            smoothingNeighborIterations[cycleNumber] = neighborIterations;
            smoothingFinalIterations[cycleNumber]    = finalIterations;
         }
         else {
            QTextStream textStream(&tagValue, QIODevice::ReadOnly);
            textStream >> smoothingStrength[0]
                 >> smoothingCycles[0]
                 >> smoothingIterations[0]
                 >> smoothingNeighborIterations[0]
                 >> smoothingFinalIterations[0];
         }
      }
      else if (tag == morphingParametersTag) {
         if (fileVersion > 0) {
            int cycleNumber;
            int cycles;
            float linearForce;
            float angularForce;
            float stepSize;
            float landmarkStepSize;
            int iterations;
            int smoothIterations;

            QTextStream textStream(&tagValue, QIODevice::ReadOnly);
            textStream >> cycleNumber
                 >> cycles
                 >> linearForce
                 >> angularForce
                 >> stepSize
                 >> landmarkStepSize
                 >> iterations
                 >> smoothIterations;

            morphingCycles[cycleNumber] = cycles;
            morphingLinearForce[cycleNumber] = linearForce;
            morphingAngularForce[cycleNumber] = angularForce;
            morphingStepSize[cycleNumber] = stepSize;
            morphingLandmarkStepSize[cycleNumber] = landmarkStepSize;
            morphingIterations[cycleNumber] = iterations;
            morphingSmoothIterations[cycleNumber] = smoothIterations;
         }
         else {
            //
            // Some older files may only have 5 parameters
            //
            morphingCycles[0] = 1;
            morphingLandmarkStepSize[0] = morphingStepSize[0];
            morphingSmoothIterations[0] = 0;
            
            QTextStream textStream(&tagValue, QIODevice::ReadOnly);
            textStream >> morphingCycles[0]
                 >> morphingLinearForce[0]
                 >> morphingAngularForce[0]
                 >> morphingStepSize[0]
                 >> morphingLandmarkStepSize[0]
                 >> morphingIterations[0]
                 >> morphingSmoothIterations[0];
         }
      }
      else if (tag == inverseDeformationFlagTag) {
         inverseDeformationFlag = (tagValue == "true");
      }
      else if (tag == flatParametersTag) {
         QTextStream textStream(&tagValue, QIODevice::ReadOnly);
         textStream >> flatSubSamplingTiles
               >> flatBeta
               >> flatVarMult
               >> flatNumIters;
      }
      else if (tag == startOfDataTag) {
            readingFiles = false;
      }
      else if (tag == sphereFiducialSphereRatioTag) {
         QTextStream istr(&tagValue, QIODevice::ReadOnly);
         QString enabled;
         istr >> enabled >> fiducialSphereRatio;
         fiducialSphereRatioEnabled = (enabled == "true");
      }
      else if (tag == smoothDeformedSurfaceTag) {
         smoothDeformedSurfacesFlag = (tagValue == "true");
      }
      else {
         std::cout << "WARNING unrecognzied deformation map file tag: " 
                   << tag.toAscii().constData() << std::endl;
      }
   }
   
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            int numNodes;
            stream >> numNodes;
            setNumberOfNodes(numNodes);
            
            for (int i = 0; i < numNodes; i++) {
               int nodeNumber;
               int nodes[3];
               float areas[3];
               stream >> nodeNumber
                      >> nodes[0]
                      >> nodes[1]
                      >> nodes[2]
                      >> areas[0]
                      >> areas[1]
                      >> areas[2];
               setDeformDataForNode(nodeNumber, nodes, areas);
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         {
#ifdef QT4_FILE_POS_BUG
            qint64 offset = findBinaryDataOffsetQT4Bug(file, lastLineRead.toAscii().constData());
            if (offset > 0) {
               offset++;
               file.seek(offset);
            }
#endif // QT4_FILE_POS_BUG
            //
            // Needed for QT 4.2.2
            //
            binStream.device()->seek(stream.pos());
            
            int numNodes;
            binStream >> numNodes;
            setNumberOfNodes(numNodes);
            
            for (int i = 0; i < numNodes; i++) {
               int nodes[3];
               float areas[3];
               binStream >> nodes[0]
                         >> nodes[1]
                         >> nodes[2]
                         >> areas[0]
                         >> areas[1]
                         >> areas[2];
               setDeformDataForNode(i, nodes, areas);
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Reading in XML format not supported.");           
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Reading Comma Separated Value File Format not supported.");
         break;
   }
}

/**
 * Write a tag/value pair and make it relative to a directory
 */
void
DeformationMapFile::writeFileTagRelative(QTextStream& stream,
                                         const QString& directory,
                                         const QString& tag,
                                         const QString& valueIn)
{
   QString value = valueIn;
   
   //
   // Only do so for version 2 and later files
   //
   if (fileVersion >= 2) {
      if (directory.isEmpty() == false) {
         makeFileRelative(directory, value);
      }
   }
   stream << tag << " "
          << value << "\n";
}

/**
 * Write the file's data.
 */
void
DeformationMapFile::writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   stream << deformMapFileVersion << " " << fileVersion << "\n";

   switch(flatOrSphereSelection) {
      case DEFORMATION_TYPE_FLAT:
         stream << flatOrSphereSelectionTag << " "
                << DeformationFlatValue << "\n";
         break;
      case DEFORMATION_TYPE_SPHERE:
      default:
         stream << flatOrSphereSelectionTag << " "
                << DeformationSphereValue << "\n";
         break;
   }
   
   stream << deformedFileNamePrefixTag << " " << deformedFileNamePrefix << "\n";
   stream << deformedColumnNamePrefixTag << " " << deformedColumnNamePrefix << "\n";
   
   stream << sourceDirectoryTag << " "
          << sourceDirectoryName << "\n";

   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceSpecTag, sourceSpecFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceBorderTag, sourceBorderFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceClosedTopoTag, sourceClosedTopoFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceCutTopoTag, sourceCutTopoFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceFiducialCoordTag, sourceFiducialCoordFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceSphericalCoordTag, sourceSphericalCoordFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceDeformedSphericalCoordTag, sourceDeformedSphericalCoordFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceDeformedFlatCoordTag, sourceDeformedFlatCoordFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceFlatCoordTag, sourceFlatCoordFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceResampledFlatCoordTag, sourceResampledFlatCoordFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceResampledDeformedFlatCoordTag, sourceResampledDeformedFlatCoordFileName);
   writeFileTagRelative(stream, sourceDirectoryName, 
                        sourceResampledCutTopoTag, sourceResampledCutTopoFileName);
   
   stream << sphereResolutionTag << " " 
          << sphereResolution << "\n";
   stream << borderResampleTag << " " 
          << static_cast<int>(borderResampleType) << " " 
          << borderResampleValue << "\n";
   stream << sphericalNumberOfCyclesTag << " " 
          << sphericalNumberOfCycles << "\n";
   for (int i = 0; i < sphericalNumberOfCycles; i++) {
      stream << smoothingParamtersTag << " " << i << " " 
             << smoothingStrength[i] << " " 
             << smoothingCycles[i] << " " 
             << smoothingIterations[i] << " " 
             << smoothingNeighborIterations[i] << " " 
             << smoothingFinalIterations[i] << "\n";
      stream << morphingParametersTag << " " << i << " " 
             << morphingCycles[i] << " " 
             << morphingLinearForce[i] << " " 
             << morphingAngularForce[i] << " " 
             << morphingStepSize[i] << " " 
             << morphingLandmarkStepSize[i] << " " 
             << morphingIterations[i] << " " 
             << morphingSmoothIterations[i] << "\n";
   }
   stream << flatParametersTag << " " 
          << flatSubSamplingTiles << " " 
          << flatBeta << " " 
          << flatVarMult << " " 
          << flatNumIters << "\n";
   
   stream << targetDirectoryTag << " "
          << targetDirectoryName << "\n";
   
   stream << smoothDeformedSurfaceTag << (smoothDeformedSurfacesFlag ? " true" : " false") << "\n";
   
   writeFileTagRelative(stream, targetDirectoryName, 
                        targetSpecTag, targetSpecFileName);
   writeFileTagRelative(stream, targetDirectoryName, 
                        targetBorderTag, targetBorderFileName);
   writeFileTagRelative(stream, targetDirectoryName, 
                        targetClosedTopoTag, targetClosedTopoFileName);
   writeFileTagRelative(stream, targetDirectoryName, 
                        targetCutTopoTag, targetCutTopoFileName);
   writeFileTagRelative(stream, targetDirectoryName, 
                        targetSphericalCoordTag, targetSphericalCoordFileName);
   writeFileTagRelative(stream, targetDirectoryName, 
                        targetFiducialCoordTag, targetFiducialCoordFileName);
   writeFileTagRelative(stream, targetDirectoryName, 
                        targetFlatCoordTag, targetFlatCoordFileName);
   writeFileTagRelative(stream, targetDirectoryName, 
                        outputSpecFileTag, outputSpecFileName);
      
   QString ratioString("false");
   if (fiducialSphereRatioEnabled) {
      ratioString = "true";
   }
   stream << sphereFiducialSphereRatioTag << " "
          << ratioString << " "
          << fiducialSphereRatio << "\n";
          
   if (inverseDeformationFlag) {
      stream << inverseDeformationFlagTag << " true\n";
   }
   else {
      stream << inverseDeformationFlagTag << " false\n";
   }
   stream << startOfDataTag << "\n";
   
   const int numNodes = getNumberOfNodes();

   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         stream << numNodes << "\n";
         
         for (int i = 0; i < numNodes; i++) {
            int nodes[3];
            float areas[3];
            getDeformDataForNode(i, nodes, areas);
            stream << i << " "  
                   << nodes[0] << " "  
                   << nodes[1] << " "  
                   << nodes[2] << " " 
                   << areas[0] << " " 
                   << areas[1] << " " 
                   << areas[2] << "\n";
         }
         break;
      case FILE_FORMAT_BINARY:
#ifdef QT4_FILE_POS_BUG
         setBinaryFilePosQT4Bug();
#else  // QT4_FILE_POS_BUG
         //
         // still a bug in QT 4.2.2
         //
         setBinaryFilePosQT4Bug();
#endif // QT4_FILE_POS_BUG
         binStream << static_cast<qint32>(numNodes);
         
         for (int i = 0; i < numNodes; i++) {
            int nodes[3];
            float areas[3];
            getDeformDataForNode(i, nodes, areas);
            binStream << static_cast<qint32>(nodes[0])  
                      << static_cast<qint32>(nodes[1])  
                      << static_cast<qint32>(nodes[2]) 
                      << areas[0]
                      << areas[1] 
                      << areas[2];
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Writing in XML format not supported.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Writing Comma Separated Value File Format not supported.");
         break;
   }
}

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

#include <sstream>

#include <QDateTime>

#include "BrainModelSurface.h"
#include "BrainModelVolumeToSurfacePaintMapper.h"
#include "CaretVersion.h"
#include "FileUtilities.h"
#include "PaintFile.h"
#include "StringUtilities.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "VolumeFile.h"

/**
 * Constructor for a volume file in memory.
 */
BrainModelVolumeToSurfacePaintMapper::BrainModelVolumeToSurfacePaintMapper(
                                     BrainSet* bs,
                                     BrainModelSurface* surfaceIn,
                                     VolumeFile* volumeFileIn,
                                     PaintFile* paintFileIn,
                                     const int paintColumnIn,
                                     const QString& paintColumnNameIn)
   : BrainModelAlgorithm(bs)
{
   volumeMode      = MODE_VOLUME_IN_MEMORY;
   surface         = surfaceIn;
   volumeFile      = volumeFileIn;
   paintFile       = paintFileIn;
   paintColumn     = paintColumnIn;
   paintColumnName = paintColumnNameIn;
}

/**
 * Constructor for a volume file that needs to be read.
 */
BrainModelVolumeToSurfacePaintMapper::BrainModelVolumeToSurfacePaintMapper(
                                     BrainSet* bs,
                                     BrainModelSurface* surfaceIn,
                                     const QString& volumeFileNameIn,
                                     PaintFile* paintFileIn,
                                     const int paintColumnIn,
                                     const QString& paintColumnNameIn)
   : BrainModelAlgorithm(bs)
{
   volumeMode      = MODE_VOLUME_ON_DISK;
   surface         = surfaceIn;
   volumeFileName  = volumeFileNameIn;
   paintFile       = paintFileIn;
   paintColumn     = paintColumnIn;
   paintColumnName = paintColumnNameIn;
}

/**
 * Destructor.
 */
BrainModelVolumeToSurfacePaintMapper::~BrainModelVolumeToSurfacePaintMapper()
{
}

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeToSurfacePaintMapper::execute() throw (BrainModelAlgorithmException)
{
   //
   // Get surface stuff
   //   
   if (surface == NULL) {
      throw BrainModelAlgorithmException("No surface provided.");
   }
   
   const CoordinateFile* cf = surface->getCoordinateFile();
   const int numberOfNodes = cf->getNumberOfCoordinates();
   if (numberOfNodes <= 0) {
      throw BrainModelAlgorithmException("Surface contains no nodes.");
   }
   const float* allCoords = cf->getCoordinate(0);
   
   //
   // Get the topology file
   //
   TopologyFile* tf = surface->getTopologyFile();
   if (tf == NULL) {
      throw BrainModelAlgorithmException("Surface has no topology.");
   }
   
   //
   // Create topology helper so that only connected nodes are mapped
   //
   const TopologyHelper* topologyHelper = tf->getTopologyHelper(false, true, false);
   
   //
   // Reset paint column if necessary
   //
   int paintColumnNumber = 0;
   if ((paintColumn < 0) || (paintColumn >= paintFile->getNumberOfColumns())) {
      paintColumn = -1;
   }
   if (paintColumn >= 0) {
      paintColumnNumber = paintColumn;
   }
   int columnsToAdd = 0;
   
   //
   // Mapping using a file that needs to be read
   //
   VolumeFile volumeFileOnDisk;
   int numberOfSubVolumes = 1;
   switch (volumeMode) {
      case MODE_VOLUME_ON_DISK:
         //
         // Determine the number of sub volumes in the volume by reading the header
         //
         volumeFile = &volumeFileOnDisk;
         try {
            volumeFile->readFile(volumeFileName,
                                 VolumeFile::VOLUME_READ_HEADER_ONLY);
            numberOfSubVolumes = volumeFile->getNumberOfSubVolumes();
            
            //
            // Determine the number of columns that need to be added to the paint file
            //
            if (paintColumn < 0) {
               columnsToAdd = numberOfSubVolumes;
               paintColumnNumber = paintFile->getNumberOfColumns();
            }
            else {
               columnsToAdd = (paintColumn + numberOfSubVolumes)
                           - paintFile->getNumberOfColumns();
               paintColumnNumber = paintColumn;
            }
         }
         catch (FileException& e) {
            throw BrainModelAlgorithmException(e.whatQString());
         }
         break;
      case MODE_VOLUME_IN_MEMORY:
         if (paintColumn < 0) {
            columnsToAdd = 1;
            paintColumnNumber = paintFile->getNumberOfColumns();
         }
         break;
   }
   
   //
   // Get volume information
   //
   if (volumeFile == NULL) {
      throw BrainModelAlgorithmException("No volume provided.");
   }
   
   //
   // Add space, if needed, to the paint file
   //
   if (columnsToAdd > 0) {
      //
      // Add a column onto the paint file
      //
      if (paintFile->getNumberOfColumns() == 0) {
         paintFile->setNumberOfNodesAndColumns(numberOfNodes, columnsToAdd);
      }
      else {
         paintFile->addColumns(columnsToAdd);
      }
   }
   
   const int questionIndex = paintFile->addPaintName("???");
   
   //
   // Process the regions
   //
   std::vector<int> regionIndexInPaintFile;
   const int numRegionNames = volumeFile->getNumberOfRegionNames();
   for (int i = 0; i < numRegionNames; i++) {
      const QString name(volumeFile->getRegionNameFromIndex(i));
      regionIndexInPaintFile.push_back(paintFile->addPaintName(name));
   }
   
   for (int j = 0; j < numberOfSubVolumes; j++) {
      switch (volumeMode) {
         case MODE_VOLUME_ON_DISK:
            //
            // Read the sub volume
            //
            try {
               volumeFile->readFile(volumeFileName, j);
            }
            catch (FileException& e) {
               throw BrainModelAlgorithmException(e.whatQString());
            }
            break;
         case MODE_VOLUME_IN_MEMORY:
            break;
      }
      int dim[3];
      volumeFile->getDimensions(dim);
      if ((dim[0] <= 0) || (dim[1] <= 0) || (dim[2] <= 0)) {
         throw BrainModelAlgorithmException("Volume is empty - dimensions all zero.");
      }
      
      QString columnName(paintColumnName);
      if (volumeMode == MODE_VOLUME_ON_DISK) {
         if (columnName.isEmpty()) {
            std::vector<QString> subVolumeNames;
            if (j < static_cast<int>(subVolumeNames.size())) {
               columnName = subVolumeNames[j];
            }
         }
         if (columnName.isEmpty()) {
            std::ostringstream str;
            str << " #" << (j + 1);
            columnName.append(str.str().c_str());
         }
      }
      paintFile->setColumnName(paintColumnNumber, columnName);
      
      //
      // Map the data
      //
      for (int i = 0; i < numberOfNodes; i++) {
         float value = 0.0;
         
         if (topologyHelper->getNodeHasNeighbors(i)) {
               
            int ijk[3];
            float pcoords[3];
            if (volumeFile->convertCoordinatesToVoxelIJK(&allCoords[i*3], 
                                                                     ijk, pcoords)) {
               value = volumeFile->getVoxel(ijk, 0);  
            }
         }
         
         int paintIndex = static_cast<int>(value);
         if ((paintIndex >= 0) && (paintIndex < numRegionNames)) {
            paintIndex = regionIndexInPaintFile[paintIndex];
         }
         else {
            paintIndex = questionIndex;
         }
         paintFile->setPaint(i, paintColumnNumber, static_cast<int>(value));       
      }
      
      //
      // Set the metadata link
      //
      paintFile->setColumnStudyMetaDataLink(paintColumnNumber, 
                                            volumeFile->getStudyMetaDataLink());
      
      //
      // Set the comment
      //
      QString comment;
      comment.append("CARET v");
      comment.append(CaretVersion::getCaretVersionAsString());
      comment.append("\n");
      comment.append("Mapped to surface: ");
      comment.append(FileUtilities::basename(cf->getFileName()));
      comment.append("\n");
      comment.append("Mapped from volume: ");
      comment.append(FileUtilities::basename(volumeFile->getFileName()));
      comment.append("\n");
      comment.append("Subvolume: ");
      comment.append(StringUtilities::fromNumber(j));
      comment.append("\n");
      comment.append("\n");
      comment.append("Date Mapped: ");
      comment.append(QDateTime::currentDateTime().toString(Qt::TextDate));
      comment.append("\n");
      paintFile->setColumnComment(paintColumnNumber, comment);
      
      //
      // Prepare for mapping next column
      //
      paintColumnNumber++;
   }
      
}

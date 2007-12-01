
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

#include <QApplication>
#include <QProgressDialog>

#include "BrainModelAlgorithmMultiThreadExecutor.h"
#include "BrainSet.h"
#include "BrainSetDataFileReader.h"
#include "BrainSetMultiThreadedSpecFileReader.h"
#include "SpecFile.h"

/**
 * constructor.
 */
BrainSetMultiThreadedSpecFileReader::BrainSetMultiThreadedSpecFileReader(BrainSet* brainSetIn)
{
   brainSet = brainSetIn;
}

/**
 * destructor.
 */
BrainSetMultiThreadedSpecFileReader::~BrainSetMultiThreadedSpecFileReader()
{
   clearFileReaders();
}

/**
 * clear the file readers.
 */
void 
BrainSetMultiThreadedSpecFileReader::clearFileReaders()
{
   for (unsigned int i = 0; i < fileReaders.size(); i++) {
      delete fileReaders[i];
      fileReaders[i] = NULL;
   }
   fileReaders.clear();
}

/**
 * add data files to the algorithm.
 */
void 
BrainSetMultiThreadedSpecFileReader::addDataFiles(const SpecFile::Entry& specFileEntry)
{
   for (int i = (specFileEntry.getNumberOfFiles() - 1); i >= 0; i--) {
      if (specFileEntry.getSelected(i) == SpecFile::SPEC_TRUE) {
         fileReaders.push_back(
            new BrainSetDataFileReader(brainSet,
                                       specFileEntry.getSpecFileTag(),
                                       specFileEntry.getFileName(i)));
                                                          
      }
   }
}

/**
 * update the progress dialog.
 */
void 
BrainSetMultiThreadedSpecFileReader::updateProgressDialog(const QString& message)
{
   if (progressDialog != NULL) {
      if (message.isEmpty() == false) {
         QApplication::processEvents();
         progressDialog->setValue(progressDialog->value() + 1);
         progressDialog->setLabelText(message);
         progressDialog->show();
         QApplication::processEvents();
      }
   }
}

/**
 * read files.
 */
void 
BrainSetMultiThreadedSpecFileReader::readFiles(const int numberOfThreads,
                                               std::vector<QString>& errorMessagesOut)
{
   BrainModelAlgorithmMultiThreadExecutor executor(fileReaders,
                                                   numberOfThreads,
                                                   false);
   QObject::connect(&executor, SIGNAL(algorithmStartedDescription(const QString&)),
                    this, SLOT(updateProgressDialog(const QString&)));
   executor.startExecution();
   std::vector<QString> messages;
   executor.getExceptionMessages(messages);
   errorMessagesOut.insert(errorMessagesOut.end(),
                           messages.begin(), messages.end());
   
   clearFileReaders();
}

/**
 * read the files.
 */
void 
BrainSetMultiThreadedSpecFileReader::readDataFiles(const int numberOfThreads,
                                              const SpecFile& specFile,
                                              QProgressDialog* progressDialogIn,
                                              std::vector<QString>& errorMessagesOut)
{
   errorMessagesOut.clear();
   
   progressDialog = progressDialogIn;
   
   //
   // Read Topology Files (must be read before coordinates)
   //
   addDataFiles(specFile.unknownTopoFile);
   addDataFiles(specFile.lobarCutTopoFile);
   addDataFiles(specFile.cutTopoFile);
   addDataFiles(specFile.openTopoFile);
   addDataFiles(specFile.closedTopoFile);
   readFiles(numberOfThreads, errorMessagesOut);
   brainSet->setSelectedTopologyFiles();
   
   //
   // Read Anatomy volume and Coordinate files
   // anatomy volume is read here so that it is will be the underlay
   //
   addDataFiles(specFile.volumeAnatomyFile);
   addDataFiles(specFile.rawCoordFile);
   addDataFiles(specFile.fiducialCoordFile);
   addDataFiles(specFile.inflatedCoordFile);
   addDataFiles(specFile.veryInflatedCoordFile);
   addDataFiles(specFile.sphericalCoordFile);
   addDataFiles(specFile.ellipsoidCoordFile);
   addDataFiles(specFile.compressedCoordFile);
   addDataFiles(specFile.flatCoordFile);
   addDataFiles(specFile.lobarFlatCoordFile);
   addDataFiles(specFile.hullCoordFile);
   addDataFiles(specFile.unknownCoordFile);
   readFiles(numberOfThreads, errorMessagesOut);
   
   //
   // Read Surface files
   //
   addDataFiles(specFile.rawSurfaceFile);
   addDataFiles(specFile.fiducialSurfaceFile);
   addDataFiles(specFile.inflatedSurfaceFile);
   addDataFiles(specFile.veryInflatedSurfaceFile);
   addDataFiles(specFile.sphericalSurfaceFile);
   addDataFiles(specFile.ellipsoidSurfaceFile);
   addDataFiles(specFile.compressedSurfaceFile);
   addDataFiles(specFile.flatSurfaceFile);
   addDataFiles(specFile.lobarFlatSurfaceFile);
   addDataFiles(specFile.hullSurfaceFile);
   addDataFiles(specFile.unknownSurfaceFile);
   readFiles(numberOfThreads, errorMessagesOut);
   
   //
   // Read Volume Files except anatomy (do not read until after coordinates)
   //
   addDataFiles(specFile.volumeFunctionalFile);
   addDataFiles(specFile.volumePaintFile);
   addDataFiles(specFile.volumeProbAtlasFile);
   addDataFiles(specFile.volumeRgbFile);
   addDataFiles(specFile.volumeSegmentationFile);
   addDataFiles(specFile.volumeVectorFile);
   readFiles(numberOfThreads, errorMessagesOut);
   
   //
   // Read Contour files 
   //
   addDataFiles(specFile.contourFile);
   readFiles(numberOfThreads, errorMessagesOut);

   //
   // Sort the brain models since their order will vary upon
   // which reading threads finish first
   //
   brainSet->sortBrainModels();
   
   //
   // Read All other files (after all "BrainModel" files are read)
   // Read "larger" files first
   //
   addDataFiles(specFile.fociProjectionFile);
   addDataFiles(specFile.surfaceShapeFile);
   addDataFiles(specFile.metricFile);
   addDataFiles(specFile.arealEstimationFile);
   addDataFiles(specFile.borderProjectionFile);
   addDataFiles(specFile.paintFile);
   addDataFiles(specFile.atlasFile);
   addDataFiles(specFile.rgbPaintFile);
   addDataFiles(specFile.studyMetaDataFile);
   addDataFiles(specFile.areaColorFile);
   addDataFiles(specFile.vocabularyFile);
   addDataFiles(specFile.wustlRegionFile);
   addDataFiles(specFile.topographyFile);
   addDataFiles(specFile.geodesicDistanceFile);
   addDataFiles(specFile.latLonFile);
   addDataFiles(specFile.paramsFile);
   addDataFiles(specFile.sceneFile);
   addDataFiles(specFile.sectionFile);
   addDataFiles(specFile.deformationFieldFile);
   addDataFiles(specFile.volumeBorderFile);
   addDataFiles(specFile.imageFile);
   addDataFiles(specFile.vtkModelFile);
   addDataFiles(specFile.paletteFile);
   addDataFiles(specFile.surfaceVectorFile);
   addDataFiles(specFile.borderColorFile);
   addDataFiles(specFile.rawBorderFile);
   addDataFiles(specFile.fiducialBorderFile);
   addDataFiles(specFile.inflatedBorderFile);
   addDataFiles(specFile.veryInflatedBorderFile);
   addDataFiles(specFile.sphericalBorderFile);
   addDataFiles(specFile.ellipsoidBorderFile);
   addDataFiles(specFile.compressedBorderFile);
   addDataFiles(specFile.flatBorderFile);
   addDataFiles(specFile.lobarFlatBorderFile);
   addDataFiles(specFile.hullBorderFile);
   addDataFiles(specFile.unknownBorderFile);
   addDataFiles(specFile.cellColorFile);
   addDataFiles(specFile.cellFile);
   addDataFiles(specFile.volumeCellFile);
   addDataFiles(specFile.cellProjectionFile);
   addDataFiles(specFile.cocomacConnectivityFile);
   addDataFiles(specFile.contourCellFile);
   addDataFiles(specFile.contourCellColorFile);
   addDataFiles(specFile.cutsFile);
   addDataFiles(specFile.fociColorFile);
   addDataFiles(specFile.fociFile);
   addDataFiles(specFile.volumeFociFile);
   addDataFiles(specFile.transformationMatrixFile);
   addDataFiles(specFile.transformationDataFile);
   addDataFiles(specFile.deformationMapFile);
   addDataFiles(specFile.cerebralHullFile);
   readFiles(numberOfThreads, errorMessagesOut);
}

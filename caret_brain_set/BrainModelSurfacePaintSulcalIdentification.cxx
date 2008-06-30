
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

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfacePaintSulcalIdentification.h"
#include "BrainModelSurfaceSulcalIdentificationProbabilistic.h"
#include "BrainSet.h"
#include "FociColorFile.h"
#include "FociProjectionFile.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "SurfaceShapeFile.h"
#include "VocabularyFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
BrainModelSurfacePaintSulcalIdentification::BrainModelSurfacePaintSulcalIdentification(
                                   BrainSet* bs,
                                   const BrainModelSurface* fiducialSurfaceIn,
                                   const BrainModelSurface* inflatedSurfaceIn,
                                   const BrainModelSurface* veryInflatedSurfaceIn,
                                   PaintFile* paintFileInOut,
                                   const int paintFileGeographyColumnNumberIn,
                                   AreaColorFile* areaColorFileInOut,
                                   const SurfaceShapeFile* depthSurfaceShapeFileIn,
                                   const int depthSurfaceShapeFileColumnNumberIn,
                                   VocabularyFile* vocabularyFileOut)
   : BrainModelAlgorithm(bs),
     fiducialSurface(fiducialSurfaceIn),
     inflatedSurface(inflatedSurfaceIn),
     veryInflatedSurface(veryInflatedSurfaceIn),
     paintFile(paintFileInOut),
     paintFileGeographyColumnNumber(paintFileGeographyColumnNumberIn),
     areaColorFile(areaColorFileInOut),
     depthSurfaceShapeFile(depthSurfaceShapeFileIn),
     depthSurfaceShapeFileColumnNumber(depthSurfaceShapeFileColumnNumberIn),
     vocabularyFile(vocabularyFileOut)
{
   metricFile = NULL;
}
                                   
/**
 * destructor.
 */
BrainModelSurfacePaintSulcalIdentification::~BrainModelSurfacePaintSulcalIdentification()
{
   if (metricFile != NULL) {
      delete metricFile;
      metricFile = NULL;
   }
}

/**
 * get the name of the Sulcus ID paint column.
 */
QString 
BrainModelSurfacePaintSulcalIdentification::getSulcusIdPaintColumnName() 
{ 
   return BrainModelSurfaceSulcalIdentificationProbabilistic::getSulcusIdPaintColumnName();
}
      
/**
 * execute the algorithm.
 */
void 
BrainModelSurfacePaintSulcalIdentification::execute() throw (BrainModelAlgorithmException)
{
   if (fiducialSurface == NULL) {
      throw BrainModelAlgorithmException("The fiducial surface is invalid.");
   }
   if (inflatedSurface == NULL) {
      throw BrainModelAlgorithmException("The inflated surface is invalid.");
   }
   if (veryInflatedSurface == NULL) {
      throw BrainModelAlgorithmException("The very inflated is invalid.");
   }
   if (depthSurfaceShapeFile == NULL) {
      throw BrainModelAlgorithmException("The surfac shape file is invalid.");
   }
   if ((depthSurfaceShapeFileColumnNumber < 0) ||
       (depthSurfaceShapeFileColumnNumber >= depthSurfaceShapeFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Surface Shape File Depth column is invalid.");
   }
   
   //
   // Verify left or right only
   //
   leftHemisphereFlag = false;
   switch (fiducialSurface->getStructure().getType()) {
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT:  
         leftHemisphereFlag = true;
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         leftHemisphereFlag = false;
         break;
      case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
      case Structure::STRUCTURE_TYPE_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
      case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
      case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
      case Structure::STRUCTURE_TYPE_INVALID:
         throw ("Structure must be left or right hemisphere.");
         break;
   }
   
   //
   // Validate geography paint column
   //   
   if (paintFile == NULL) {
      throw BrainModelAlgorithmException("Geography Paint File is invalid.");
   }
   if ((paintFileGeographyColumnNumber < 0) ||
       (paintFileGeographyColumnNumber >= paintFile->getNumberOfColumns())) {
      throw BrainModelAlgorithmException("Geography Paint File column is invalid.");
   }
  
   const QString probabilisticDirectoryFileName =
      (leftHemisphereFlag 
         ? "left/PALS_B12.LEFT.PROBABILISTIC.FILE_DIRECTORY.csv" 
         : "right/PALS_B12.RIGHT.PROBABILISTIC.FILE_DIRECTORY.csv");
   const QString probabilisticSulciVolumeDirectoryFilePath =
      (brainSet->getCaretHomeDirectory()
       + "/data_files/sulcal_identification/"
       + probabilisticDirectoryFileName);
       
   //
   // Generate the probabilistic identification
   //
   BrainModelSurfaceSulcalIdentificationProbabilistic 
      sid(brainSet,
          fiducialSurface,
          inflatedSurface,
          veryInflatedSurface,
          paintFile,
          paintFileGeographyColumnNumber,
          depthSurfaceShapeFile,
          depthSurfaceShapeFileColumnNumber,
          probabilisticSulciVolumeDirectoryFilePath);
   sid.execute();
   
   //
   // Remove any paints from input paint that have Sulcal ID column names
   //
   const PaintFile* sulcalIdPaintFile = sid.getOutputPaintFile();
   for (int i = 0; i < sulcalIdPaintFile->getNumberOfColumns(); i++) {
      const QString columnName = sulcalIdPaintFile->getColumnName(i);
      const int indx = paintFile->getColumnWithName(columnName);
      if (columnName >= 0) {
         paintFile->removeColumn(indx);
      }
   }
   
   //
   // Get the output files of the probabilistic identification
   //
   metricFile = new MetricFile(*sid.getOutputMetricFile());
   paintFile->append(*sulcalIdPaintFile);
   areaColorFile->append(*sid.getOutputAreaColorFile());
   vocabularyFile->append(*sid.getOutputVocabularyFile());
}


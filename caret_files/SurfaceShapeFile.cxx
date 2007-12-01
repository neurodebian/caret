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

#define  _SURFACE_SHAPE_MAIN_
#include "SurfaceShapeFile.h"
#undef   _SURFACE_SHAPE_MAIN_

#include "CoordinateFile.h"
#include "FileUtilities.h"
#include "FreeSurferCurvatureFile.h"
#include "GiftiCommon.h"
#include "SpecFile.h"

/**
 * Constructor
 */
SurfaceShapeFile::SurfaceShapeFile() 
   : MetricFile("Surface Shape File", GiftiCommon::intentShape, SpecFile::getSurfaceShapeFileExtension()) 
{
}

/**
 * Destructor
 */
SurfaceShapeFile::~SurfaceShapeFile()
{
}

/**
 * Find column containing areal distortion
 */
int 
SurfaceShapeFile::getArealDistortionColumnNumber() const
{
   return getNamedColumnNumber(arealDistortionColumnName);
}

/**
 * Find column containing gaussian curvature
 */
int 
SurfaceShapeFile::getGaussianCurvatureColumnNumber() const
{
   return getNamedColumnNumber(gaussianCurvatureColumnName);
}

/**
 * Find column containing linear distortion
 */
int 
SurfaceShapeFile::getLinearDistortionColumnNumber() const
{
   return getNamedColumnNumber(linearDistortionColumnName);
}

/**
 * Find column containing mean curvature
 */
int 
SurfaceShapeFile::getMeanCurvatureColumnNumber() const
{
   return getNamedColumnNumber(meanCurvatureColumnName);
}

/**
 * Find column containing sulcal depth
 */
int 
SurfaceShapeFile::getSulcalDepthColumnNumber() const
{
   return getNamedColumnNumber(sulcalDepthColumnName);
}

/**
 * Find column containing smoothed sulcal depth
 */
int 
SurfaceShapeFile::getSulcalDepthSmoothedColumnNumber() const
{
   return getNamedColumnNumber(sulcalDepthSmoothedColumnName);
}

/**
 * Export free surfer ascii curvature file.
 */
void
SurfaceShapeFile::exportFreeSurferAsciiCurvatureFile(const int columnNumber,
                                                     const CoordinateFile* cf,
                                                     const QString& filename) throw (FileException)
{
   if ((columnNumber >= 0) && (columnNumber < getNumberOfColumns())) {
      const int numNodes = getNumberOfNodes();
      FreeSurferCurvatureFile fscl;
      fscl.setNumberOfVertices(numNodes);
      for (int i = 0; i < numNodes; i++) {
         float xyz[3];
         cf->getCoordinate(i, xyz);
         fscl.setCurvature(i, xyz, -getValue(i, columnNumber));
      }
      fscl.writeFile(filename);
   }
   else {
      throw FileException(filename, "Invalid surface shape column selected for export.");
   }
}

/**
 * Import free surfer curvature file.
 */
void
SurfaceShapeFile::importFreeSurferCurvatureFile(const int numNodes,
                                                const QString& filename,
                                                const FILE_FORMAT fileFormat) throw (FileException)
{
   if (numNodes == 0) {
      throw FileException(filename, "Number of nodes must be set prior to importing a \n"
                                     "FreeSurfer curvature file.  This is usually\n"
                                     "accomplished by importing an \"orig\" surface prior\n"
                                     "to importing curvature data.");
   }

   //
   // Add a column to this surface shape file
   //
   if (getNumberOfColumns() == 0) {
      setNumberOfNodesAndColumns(numNodes, 1);
   }
   else {
      addColumns(1);
   }
   const int columnNumber = getNumberOfColumns() - 1;
   
   //
   // Set the name of the column to the name of the curvature file
   //
   setColumnName(columnNumber, FileUtilities::basename(filename));
   
   setModified();
   
   //
   // Read the curvature file
   //
   FreeSurferCurvatureFile fscf;
   fscf.setFileReadType(fileFormat);
   fscf.readFile(filename);
   
   //
   // File must have same number of nodes as surface
   //
   const int numItems = fscf.getNumberOfVertices();
   if (numItems != numNodes) {
      throw FileException(filename, "Has different number of nodes than currently loaded surface.");
   }
   
   //
   // Read until end of file
   //
   float xyz[3], curve;
   for (int i = 0; i < numItems; i++) {
      fscf.getCurvature(i, xyz, curve);
      setValue(i, columnNumber, -curve);
   }
         
   //
   // Set the minimum and maximum for color mapping
   //
   float minValue, maxValue;
   getDataColumnMinMax(columnNumber, minValue, maxValue);
   setColumnColorMappingMinMax(columnNumber, minValue, maxValue);

   appendToFileComment(" Imported from ");
   appendToFileComment(FileUtilities::basename(filename));
}


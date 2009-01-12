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

#ifndef __VE_SURFACE_SHAPE_FILE_H__
#define __VE_SURFACE_SHAPE_FILE_H__

#include "MetricFile.h"

class CoordinateFile;

/// Class extends metric for storing surface shape 
class SurfaceShapeFile : public MetricFile {
   public:
      static const QString arealDistortionColumnName;
      static const QString gaussianCurvatureColumnName;
      static const QString linearDistortionColumnName;
      static const QString meanCurvatureColumnName;
      static const QString sulcalDepthColumnName;
      static const QString sulcalDepthSmoothedColumnName;

      /// constructor
      SurfaceShapeFile();
      
      /// destructor
      ~SurfaceShapeFile();

      /// find column containing areal distortion
      int getArealDistortionColumnNumber() const;

      /// find column containing gaussian curvature
      int getGaussianCurvatureColumnNumber() const;

      /// find column containing linear distortion
      int getLinearDistortionColumnNumber() const;

      /// find column containing mean curvature
      int getMeanCurvatureColumnNumber() const;

      /// find column containing sulcal depth
      int getSulcalDepthColumnNumber() const;

      /// find column containing smoothed sulcal depth
      int getSulcalDepthSmoothedColumnNumber() const;     

      /// Export free surfer ascii curvature file.
      void exportFreeSurferAsciiCurvatureFile(const int columnNumber,
                                              const CoordinateFile* cf,
                                              const QString& filename) throw (FileException);
                                                    
      /// Import free surfer curvature file.
      void importFreeSurferCurvatureFile(const int numNodes,
                                         const QString& filename,
                                         const FILE_FORMAT fileFormat = AbstractFile::FILE_FORMAT_ASCII) throw (FileException);
};

#endif // __VE_SURFACE_SHAPE_FILE_H__

#ifdef _SURFACE_SHAPE_MAIN_
   const QString SurfaceShapeFile::arealDistortionColumnName 
                                                    = "Areal Distortion";
   const QString SurfaceShapeFile::gaussianCurvatureColumnName 
                                                    = "Gaussian Curvature";
   const QString SurfaceShapeFile::linearDistortionColumnName 
                                                    = "Linear Distortion";
   const QString SurfaceShapeFile::meanCurvatureColumnName 
                                                   = "Folding (Mean Curvature)";
   const QString SurfaceShapeFile::sulcalDepthColumnName 
                                                    = "Depth";
   const QString SurfaceShapeFile::sulcalDepthSmoothedColumnName 
                                                     = "Depth Smoothed";
#endif // _SURFACE_SHAPE_MAIN_


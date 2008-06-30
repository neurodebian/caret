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


#ifndef __COORDINATE_FILE_H__
#define __COORDINATE_FILE_H__

#include "GiftiNodeDataFile.h"
#include "BrainVoyagerFile.h"
#include "FileException.h"
#include "FreeSurferSurfaceFile.h"

class MetricFile;
class MniObjSurfaceFile;
class TransformationMatrix;
class vtkPolyData;

/// Coordinate File
/**
 * File for storing, reading, and writing a Caret coordinate file
 */
class CoordinateFile : public GiftiNodeDataFile {
   public:
      // constructor
      CoordinateFile();
      
      // copy constructor
      CoordinateFile(const CoordinateFile& cf);
      
      // destructor
      ~CoordinateFile();
      
      // assignment operator
      CoordinateFile& operator=(const CoordinateFile& cf);
      
      // add a coordinate to the coordinate file
      void addCoordinate(const float xyz[3]);

      // Apply transformation matrix to coordinate file
      void applyTransformationMatrix(const TransformationMatrix& tm);

      // clear the coordinate file
      void clear();
      
      // get the coordinates out of a brain voyager file
      void importFromBrainVoyagerFile(const BrainVoyagerFile& bvf);
      
      // get the coordinates out of a free surfer surface file
      void importFromFreeSurferSurfaceFile(const FreeSurferSurfaceFile& fssf,
                                 const int numNodes = -1) throw (FileException);
      
      // add the coordinates to of a free surfer surface file
      void exportToFreeSurferSurfaceFile(FreeSurferSurfaceFile& fssf);
      
      // Get the coordinates out of a vtkPolyData object
      void importFromVtkFile(vtkPolyData* polyData);
      
      /// get the coordinates from a MNI OBJ surface file
      void importFromMniObjSurfaceFile(const MniObjSurfaceFile& mni) throw (FileException);
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfNodes() == 0); }
      
      // get the bounds of the coordindate file
      void getBounds(float bounds[6]) const;
      
      // get a coordinate
      void getCoordinate(const int coordinateNumber, 
                         float& x, float& y, float& z) const;

      // get a coordinate
      void getCoordinate(const int coordinateNumber, 
                         double& x, double& y, double& z) const;

      // get a coordinate
      void getCoordinate(const int coordinateNumber, 
                         float xyz[3]) const;

      // get a coordinate as doubles
      void getCoordinate(const int coordinateNumber, 
                         double xyz[3]) const;

      // get a coordinate
      float* getCoordinate(const int coordinateNumber); 
      
      // get a coordinate (const method)
      const float* getCoordinate(const int coordinateNumber) const; 
      
      // get all coordinates (3 elements per coordinate)
      void getAllCoordinates(std::vector<float>& coordsOut) const;
      
      // get all coordinates (allocate coords to 3 * getNumberOfCoordinates())
      void getAllCoordinates(float* coordsOut) const;
      
      // set all coordinates (3 elements per coordinate)
      void setAllCoordinates(const std::vector<float>& coordsIn);
      
      // set all coordinates (allocate coords to 3 * getNumberOfCoordinates())
      void setAllCoordinates(const float* coordsIn);
      
      // set a coordinate                         
      void setCoordinate(const int coordinateNumber, 
                         const float x, const float y, const float z);

      // set a coordinate                         
      void setCoordinate(const int coordinateNumber, 
                         const double x, const double y, const double z);

      // set a coordinate                         
      void setCoordinate(const int coordinateNumber, 
                         const float xyz[3]);

      // set a coordinate                         
      void setCoordinate(const int coordinateNumber, 
                         const double xyz[3]);

      // get coordinate closest to a point
      int getCoordinateIndexClosestToPoint(
                    const float x, const float y, const float z,
                    const int startSearchAtCoordinateIndex = 0) const;
                    
      // get coordinate closest to a point
      int getCoordinateIndexClosestToPoint(
                    const float xyz[3],
                    const int startSearchAtCoordinateIndex = 0) const;
                    
      /// get the number of coordinates
      int getNumberOfCoordinates() const { return getNumberOfNodes(); }
      
      /// set the number of coordinates
      void setNumberOfCoordinates(const int numCoordinates);
      
      // get the distance between two coordinates
      float getDistanceBetweenCoordinates(const int c1, const int c2) const;
      
      // get the distance squared between two coordinates 
      float getDistanceBetweenCoordinatesSquared(const int c1, const int c2) const;
      
      // get the distance from a 3D point to a coordinate
      float getDistanceToPoint(const int coordIndex, 
                               const float* point3D) const;
      
      // get the distance squared from a 3D point to a coordinate
      float getDistanceToPointSquared(const int coordIndex, 
                                      const float* point3D) const;
      
      // compute shuffled average coordinate files
      static void createShuffledAverageCoordinatesFiles(const std::vector<CoordinateFile*>& files,
                                                        const int numberInGroup1,
                                                        CoordinateFile& coordFileOut1,
                                                        CoordinateFile& coordFileOut2)
                                             throw (FileException);
                                             
      // compute an average coordinate file
      static void createAverageCoordinateFile(const std::vector<CoordinateFile*>& files,
                                              CoordinateFile& averageFile,
                                              MetricFile* ssf = NULL)
                                 throw (FileException);

      // deform "this" node data file placing the output in "deformedFile".
      virtual void deformFile(const DeformationMapFile& dmf, 
                              GiftiNodeDataFile& deformedFile,
                              const DEFORM_TYPE dt) const throw (FileException);
      
      // apply GIFTI transformation matrix
      void applyGiftiTransformationMatrix();
      
   protected:
      // copy helper used by assignment operator and copy constructor
      void copyHelperCoordinate(const CoordinateFile& mf);
      
      // read coordinate file data
      void readLegacyNodeFileData(QFile& file, QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      // write coordinate file data
      void writeLegacyNodeFileData(QTextStream& stream, QDataStream& binStream) throw (FileException);
      
      //
      // NOTE: If any variables are added be sure to update the copy constructor.
      //

   friend class BrainModelSurface;
};

#endif // __COORDINATE_FILE_H__



#ifndef __CARET_VECTOR_FILE_H__
#define __CARET_VECTOR_FILE_H__

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


#include "AbstractFile.h"
#include "FileException.h"

class VolumeFile;

/// class for storing vectors
class VectorFile : public AbstractFile {
   public:
      /// Combine with volume operations
      enum COMBINE_VOLUME_OPERATION {
         COMBINE_VOLUME_REPLACE_MAGNITUDE_WITH_VOLUME,
         COMBINE_VOLUME_MULTIPLY_MAGNITUDE_WITH_VOLUME
      };
      
      /// combine operations
      enum COMBINE_OPERATION {
         COMBINE_OPERATION_DOT_SQRT_RECT_MINUS,  // was 1
         COMBINE_OPERATION_2_VEC_NORMAL,  // was 2
         COMBINE_OPERATION_2_VEC  // was 3
      };
      
      /// Constructor
      VectorFile(const int xdim, const int ydim, const int zdim);
      
      /// Constructor
      VectorFile();
      
      /// Destructor
      ~VectorFile();
      
      /// clear the file
      void clear();
      
      /// Initialize the file to specified size with all zero values.
      void initialize(const int xdim, const int ydim, const int zdim);
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const;

      /// get the dimensions
      void getDimensions(int dim[3]) const;
      
      /// get the dimensions
      void getDimensions(int& dimX, int& dimY, int& dimZ) const;
      
      /// get a vector
      void getVector(const int i, const int j, const int k, float vector[3]) const;
      
      /// get magnitude
      float getMagnitude(const int i, const int j, const int k) const;
      
      /// get a vector
      void getVectorWithFlatIndex(const int indx, float& xOut, float& yOut, float& zOut) const;
      
      /// get a vector
      void getVectorWithFlatIndex(const int indx, float xyzOut[3]) const;
      
      /// get a magnitude
      float getMagnitudeWithFlatIndex(const int indx) const { return magnitude[indx]; }
      
      /// set a vector
      void setVectorWithFlatIndex(const int indx, const float xIn, 
                     const float yIn, const float zIn);
      
      /// set a vector
      void setVectorWithFlatIndex(const int indx, float xyzIn[3]);
      
      /// set a magnitude
      void setMagnitudeWithFlatIndex(const int indx, const float mag) { magnitude[indx] = mag; }
      
      /// get a pointer to the X data
      float* getWithFlatIndexValueX(const int indx);
      
      /// get a pointer to the Y data
      float* getWithFlatIndexValueY(const int indx);
      
      /// get a pointer to the Z data
      float* getWithFlatIndexValueZ(const int indx);
      
      /// combine a volume with this file's magnitude
      void combineWithVolumeOperation(const COMBINE_VOLUME_OPERATION operation,
                            const VolumeFile* vf) throw (FileException);
           
      /// copy magnitude to a volume's voxels (assumes volume properly allocated)
      void copyMagnitudeToVolume(VolumeFile* vf) const throw (FileException);
      
      /// Combine vector files
      static void combineVectorFiles (const bool maskingFlag, 
                               const COMBINE_OPERATION operation,
                               VectorFile* vec1,
                               const VectorFile* vec2,
                               const VolumeFile* maskVolume,
                               VectorFile* out) throw (FileException);
      
      /// multiply x,y,z by magnitude
      void multiplyXYZByMagnitude();
      
   protected:
      /// read the file's data (header has already been read)
      void readFileData(QFile& file,
                        QTextStream& stream,
                        QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                         QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);

      /// get data index
      int getDataIndex(const int ijk[3]) const;
      
      /// get flat data 
      int getDataIndex(const int i, const int j, const int k) const;
      
      /// dimensions of vector file
      int dimensions[3];
      
      /// x-vector component
      std::vector<float> x;
      
      /// y-vector component
      std::vector<float> y;
      
      /// z-vector component
      std::vector<float> z;
      
      /// magnitude 
      std::vector<float> magnitude;
      
      /// number of elements
      int numElements;
};

#endif // __CARET_VECTOR_FILE_H__


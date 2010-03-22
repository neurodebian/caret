#ifndef __VECTOR_FILE_H__
#define	__VECTOR_FILE_H__

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

#include "GiftiDataArrayFile.h"

class TransformationMatrix;
class VolumeFile;

/// class for storing vectors
class VectorFile : public GiftiDataArrayFile {
   public:
      // constructor
      VectorFile();

      // copy constructor
      VectorFile(const VectorFile& nndf);

      // create vectors from FSL volume files contains x/y/z/magnitude
      static VectorFile* createVectorFileFromFSLVectorVolumes(
                   const VolumeFile& xVectorVolume,
                   const VolumeFile& yVectorVolume,
                   const VolumeFile& zVectorVolume,
                   const VolumeFile& magnitudeVolume,
                   const VolumeFile& destinationSpaceVolume,
                   const TransformationMatrix& fslMatrix,
                   TransformationMatrix& inputToOutputSpaceTransformationMatrixOut,
                   const float defaultColorRGB[3] = NULL) throw (FileException);

      // assignment operator
      VectorFile& operator=(const VectorFile& nndf);

      // destructor
      ~VectorFile();

      // append a data array file to this one
      virtual void append(const VectorFile& naf) throw (FileException);

      // Clear the node data file.
      virtual void clear();

      // add a vector (color and radius are optional)
      void addVector(const float xyzOriginIn[3],
                     const float xyzComponentsIn[3],
                     const float magnitudeIn,
                     const int nodeNumberIn = -1,
                     const float rgbaColorsIn[4] = NULL,
                     const float radiusIn = 1.0);

      // get number of vector
      int getNumberOfVectors() const;

      // get the vector's data
      void getVectorData(const int vectorIndex,
                         float xyzOriginOut[3],
                         float xyzComponentsOut[3],
                         float& magnitudeOut,
                         int& nodeNumberOut,
                         float rgbaColorsOut[4],
                         float& radiusOut) const;

      // set the number of vectors (clears any existing data)
      void setNumberOfVectors(int numberOfVectors);

      // get the vector's origin
      void getVectorOrigin(const int vectorIndex,
                           float xyzOriginOut[3]) const;

      // set the vector's origin
      void setVectorOrigin(const int vectorIndex,
                           const float xyzOriginIn[3]);

      // get the unit vector's XYZ components
      void getVectorUnitComponents(const int vectorIndex,
                               float xyzVectorOut[3]) const;

      // set the unit vector's XYZ components
      void setVectorUnitComponents(const int vectorIndex,
                               const float xyzVectorIn[3]);

      // get the vector's magnitude
      float getVectorMagnitude(const int vectorIndex) const;

      // set the vector's magnitude
      void setVectorMagnitude(const int vectorIndex,
                              const float magnitudeIn);

      // get the vectors radius
      float getVectorRadius(const int vectorIndex) const;

      // set the vector's radius
      void setVectorRadius(const int vectorIndex,
                           const float radiusIn);

      // get the vector's rgba colors
      void getVectorColorRGBA(const int vectorIndex,
                              float rgbaOut[4]) const;

      // set the vector's rgba colors
      void setVectorColorRGBA(const int vectorIndex,
                              const float rgbaIn[4]);

      // get the vector's node number
      int getVectorNodeNumber(const int vectorIndex) const;

      // set the vector's node number
      void setVectorNodeNumber(const int vectorIndex,
                               const int nodeNumberIn);

      // set the vector's data 
      void setVectorData(const int vectorIndex,
                         const float xyzOriginIn[3],
                         const float xyzComponentsIn[3],
                         const float magnitudeIn,
                         const int nodeNumberIn = -1,
                         const float rgbaColorsIn[4] = NULL,
                         const float radiusIn = 1.0);

      // get PubMedID's of all linked studies
      void getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs) const;

      // Apply transformation matrix to vector file.
      void applyTransformationMatrix(const TransformationMatrix& tmIn);

      /// get the default color
      static void getDefaultColorRGBA(float rgba[4]);

      /// write the file's memory in caret6 format to the specified name
      virtual QString writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException);

   protected:
      /// indices of vector data
      enum DATA_INDEX {
         INDEX_NODE_NUMBER  = 0,
         INDEX_X_COORDINATE = 1,
         INDEX_Y_COORDINATE = 2,
         INDEX_Z_COORDINATE = 3,
         INDEX_X_COMPONENT  = 4,
         INDEX_Y_COMPONENT  = 5,
         INDEX_Z_COMPONENT  = 6,
         INDEX_MAGNITUDE    = 7,
         INDEX_RADIUS       = 8,
         INDEX_COLOR_RED    = 9,
         INDEX_COLOR_GREEN  = 10,
         INDEX_COLOR_BLUE   = 11,
         INDEX_COLOR_ALPHA  = 12,
         NUMBER_OF_ARRAYS   = 13
      };

      // get the data array description
      static QString getDataArrayDescription(const DATA_INDEX dataIndex);

      // copy helper
      void copyHelperVectorFile(const VectorFile& nndf);

      /// set a data array value
      void setDataValue(const int arrayIndex,
                        const int vectorIndex,
                        const float value);

      /// get a data array value
      float getDataValue(const int arrayIndex,
                         const int vectorIndex) const;

      // validate the data arrays (optional for subclasses)
      virtual void validateDataArrays() throw (FileException);

      /// the default color
      static const float defaultColor[4];

};

#ifdef __VECTOR_FILE_MAIN__
   const float VectorFile::defaultColor[4] = { 1.0, 0.5, 0.5, 1.0 };
#endif // __VECTOR_FILE_MAIN__

#endif	/* __VECTOR_FILE_H__ */


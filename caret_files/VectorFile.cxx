
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
#include <iostream>

#include "FileUtilities.h"
#include "GiftiCommon.h"
#include "GiftiMetaData.h"
#include "MathUtilities.h"
#include "SpecFile.h"
#include "TransformationMatrixFile.h"
#define __VECTOR_FILE_MAIN__
#include "VectorFile.h"
#undef __VECTOR_FILE_MAIN__
#include "VolumeFile.h"

/**
 * constructor.
 */
VectorFile::VectorFile()
        : GiftiDataArrayFile("Vector File",
                             GiftiCommon::intentVectors,
                             GiftiDataArray::DATA_TYPE_FLOAT32,
                             SpecFile::getGiftiVectorFileExtension(),
                             AbstractFile::FILE_FORMAT_XML,
                             AbstractFile::FILE_IO_NONE,
                             AbstractFile::FILE_IO_NONE,
                             AbstractFile::FILE_IO_NONE,
                             AbstractFile::FILE_IO_NONE,
                             false)
{
}

/**
 * destructor.
 */
VectorFile::~VectorFile()
{
   /* nothing to do at this time. */
}

/**
 * copy constructor.
 */
VectorFile::VectorFile(const VectorFile& vf)
   : GiftiDataArrayFile(vf)
{
   copyHelperVectorFile(vf);
}

/**
 * assignment operator.
 */
VectorFile&
VectorFile::operator=(const VectorFile& vf)
{
   if (this != &vf) {
      GiftiDataArrayFile::operator=(vf);
      copyHelperVectorFile(vf);
   }

   return *this;
}

/**
 * create vectors from FSL volume files contains x/y/z/magnitude.
 */
VectorFile*
VectorFile::createVectorFileFromFSLVectorVolumes(
                       const VolumeFile& xVectorVolume,
                       const VolumeFile& yVectorVolume,
                       const VolumeFile& zVectorVolume,
                       const VolumeFile& magnitudeVolume,
                       const VolumeFile& destinationSpaceVolume,
                       const TransformationMatrix& fslMatrixIn,
                       TransformationMatrix& inputToOutputSpaceTransformationMatrixOut,
                       const float defaultColorRGB[3]) throw (FileException)
{
   //
   // Get info from X-Vector volume
   //
   int dim[3];
   float origin[3];
   float spacing[3];
   magnitudeVolume.getDimensions(dim);
   magnitudeVolume.getOrigin(origin);
   magnitudeVolume.getSpacing(spacing);

   //
   // Verify that Y, Z, and magnitude volumes are of same dimensions as X
   //
   int xDim[3];
   xVectorVolume.getDimensions(xDim);
   if ((dim[0] != xDim[0]) ||
       (dim[1] != xDim[1]) ||
       (dim[2] != xDim[2])) {
      throw FileException("X and Magnitude volumes have different dimensions.");
   }
   int yDim[3];
   yVectorVolume.getDimensions(yDim);
   if ((dim[0] != yDim[0]) ||
       (dim[1] != yDim[1]) ||
       (dim[2] != yDim[2])) {
      throw FileException("Y and Magnitude volumes have different dimensions.");
   }
   int zDim[3];
   zVectorVolume.getDimensions(zDim);
   if ((dim[0] != zDim[0]) ||
       (dim[1] != zDim[1]) ||
       (dim[2] != zDim[2])) {
      throw FileException("Z and Magnitude volumes have different dimensions.");
   }

   //
   // Get the Ref SFORM matrix from the destination space volume
   // and remove scaling
   //
   TransformationMatrix refSformMatrix = 
           destinationSpaceVolume.getNiftiSFormTransformationMatrix();
   refSformMatrix.setMatrixElement(0, 0, 1.0);
   refSformMatrix.setMatrixElement(1, 1, 1.0);
   refSformMatrix.setMatrixElement(2, 2, 1.0);

   //
   // Matrix to flip about X-Axis
   //
   float destVolumeSpacing[3];
   destinationSpaceVolume.getSpacing(destVolumeSpacing);
   TransformationMatrix xFlipMatrix;
   xFlipMatrix.identity();
   if (destVolumeSpacing[0] > 0) {
      xFlipMatrix.setMatrixElement(0, 0, -1.0);
   }
   
   //
   // Create a "swap" matrix that flips from radiological to
   // neurological orientation (Wref)
   //
   // NOTE: This does not work for flipping which needs to be done after SFORM
   // See XFLIP matrix below
   //
   TransformationMatrix referenceSwapMatrix;
   referenceSwapMatrix.identity();
   //referenceSwapMatrix.setMatrixElement(0, 0, -1.0);
   //referenceSwapMatrix.setMatrixElement(0, 3, dim[0] - 1);

   //
   // Create a matrix for reference volume scaling (which in caret land is 1mm)
   // so it is just the identify matrix, at least for now. (Sref)
   //
   TransformationMatrix refScalingMatrix;
   refScalingMatrix.identity();
   refScalingMatrix.inverse();

   //
   // The matrix from FSL
   //
   TransformationMatrix fslMatrix = fslMatrixIn;
   //fslMatrix.inverse();
   
   //
   // Create a matrix for input volume scaling which is the absolute values of
   // the voxels sizes.  (Sin)
   //
   TransformationMatrix inputScalingMatrix;
   inputScalingMatrix.identity();
   inputScalingMatrix.setMatrixElement(0, 0, std::fabs(spacing[0]));
   inputScalingMatrix.setMatrixElement(1, 1, std::fabs(spacing[1]));
   inputScalingMatrix.setMatrixElement(2, 2, std::fabs(spacing[2]));

   //
   // Create the swap matrix for the input volume which transforms it into
   // radiological orientation.  Since the input volumes are always in
   // radiological orientation, this is just the identity matrix
   //
   TransformationMatrix inputSwapMatrix;
   inputSwapMatrix.identity();

   //
   // Source SFORM matrix
   //
   //
   // Get the SFORM matrix
   //
   TransformationMatrix sourceSformMatrix =
      magnitudeVolume.getNiftiSFormTransformationMatrix();
   sourceSformMatrix.inverse();

   //
   // Construct the transformation matrix for placing the vectors
   // into reference (caret) space
   //
   TransformationMatrix matrix;
   matrix.identity();
   matrix.postMultiply(xFlipMatrix);
   matrix.postMultiply(refSformMatrix);
   matrix.postMultiply(referenceSwapMatrix);
   matrix.postMultiply(refScalingMatrix);
   matrix.postMultiply(fslMatrix);
   matrix.postMultiply(inputScalingMatrix);
   matrix.postMultiply(inputSwapMatrix);
   matrix.postMultiply(sourceSformMatrix);

   //
   // Determine if any flipping is need of the vector components that might
   // be caused by storing the vectors in orientation other than
   // LPI (X=Left-to-Right, Y=Posterior-to-Anterior, Z=Inferior-to-Superior).
   //
   float xFlip = ((spacing[0] < 0) ? -1.0 : 1.0);
   float yFlip = ((spacing[1] < 0) ? -1.0 : 1.0);
   float zFlip = ((spacing[2] < 0) ? -1.0 : 1.0);
   //
   // Get the origin and the vector components
   //
   std::vector<float> x, y, z, vx, vy, vz, magnitude;
   for (int i = 0; i < dim[0]; i++) {
      for (int j = 0; j < dim[1]; j++) {
         for (int k = 0; k < dim[2]; k++) {
            const float mag = magnitudeVolume.getVoxel(i, j, k);
            if (mag > 0.0) {
               float xyz[3];
               xVectorVolume.getVoxelCoordinate(i, j, k, xyz);
               x.push_back(xyz[0]);
               y.push_back(xyz[1]);
               z.push_back(xyz[2]);

               vx.push_back(xVectorVolume.getVoxel(i, j, k) * xFlip);
               vy.push_back(yVectorVolume.getVoxel(i, j, k) * yFlip);
               vz.push_back(zVectorVolume.getVoxel(i, j, k) * zFlip);

               magnitude.push_back(mag);
            }
         }
      }
   }

   //
   // Verify that there are vectors
   //
   int numVectors = static_cast<int>(vx.size());
   if (numVectors <= 0) {
      throw FileException("No vectors with a positive magnitude were found.");
   }

   //
   // Set the default coloring and use user provided coloring
   //
   float rgba[4];
   VectorFile::getDefaultColorRGBA(rgba);
   if (defaultColorRGB != NULL) {
      rgba[0] = defaultColorRGB[0];
      rgba[1] = defaultColorRGB[1];
      rgba[2] = defaultColorRGB[2];
   }

   //
   // Create and fill the vector file
   //
   VectorFile* vectorFile = new VectorFile();
   vectorFile->setNumberOfVectors(numVectors);
   for (int i = 0; i < numVectors; i++) {
      const float xyz[3] = { x[i], y[i], z[i] };
      const float vector[3] = { vx[i], vy[i], vz[i] };
      const float mag = magnitude[i];
      vectorFile->setVectorData(i, xyz, vector, mag, -1, rgba, 1.0);
   }

   //
   // Apply the matrix to the vector file
   //
   vectorFile->applyTransformationMatrix(matrix);

   inputToOutputSpaceTransformationMatrixOut = matrix;

   return vectorFile;
}

/**
 * copy helper.
 */
void
VectorFile::copyHelperVectorFile(const VectorFile& /*vf*/)
{
   /* nothing to do at this time. */
}

/**
 *
 */
void
VectorFile::clear()
{
   GiftiDataArrayFile::clear();
}

/**
 * append a data array file to this one.
 */
void
VectorFile::append(const VectorFile& vf) throw (FileException)
{
   if (this->getNumberOfVectors() <= 0) {
      *this = vf;
   }
   else {
      int num = vf.getNumberOfVectors();
      for (int i = 0; i < num; i++) {
         float origin[3], vector[3], mag, rgba[4], radius;
         int nodeNumber;
         vf.getVectorData(i, origin, vector, mag, nodeNumber, rgba, radius);
         this->addVector(origin, vector, mag, nodeNumber, rgba, radius);
      }
   }
}

/**
 * add a vector (color and radius are optional).
 */
void
VectorFile::addVector(const float xyzOriginIn[3],
                      const float xyzComponentsIn[3],
                      const float magnitude,
                      const int nodeNumberIn,
                      const float rgbaColorsIn[4],
                      const float radiusIn)
{
   int numDataArrays = this->getNumberOfDataArrays();
   if (numDataArrays <= 0) {
      this->setNumberOfVectors(1);
   }
   else {

      for (int i = 0; i < numDataArrays; i++) {
         GiftiDataArray* gda = this->getDataArray(i);
         gda->addRows(1);
      }
   }

   const int vectorIndex = this->getNumberOfVectors() - 1;
   this->setVectorData(vectorIndex,
                       xyzOriginIn,
                       xyzComponentsIn,
                       magnitude,
                       nodeNumberIn,
                       rgbaColorsIn,
                       radiusIn);

   this->setModified();
}

/**
 * get number of vector.
 */
int
VectorFile::getNumberOfVectors() const
{
   if (this->getNumberOfDataArrays() > 0) {
      return this->dataArrays[0]->getDimension(0);
   }

   return 0;
}

/**
 * get the vector's data (pass NULL for undesired items).
 */
void
VectorFile::getVectorData(const int vectorIndex,
                          float xyzOriginOut[3],
                          float xyzComponentsOut[3],
                          float& magnitudeOut,
                          int& nodeNumberOut,
                          float rgbaColorsOut[4],
                          float& radiusOut) const
{
   this->getVectorOrigin(vectorIndex, xyzOriginOut);
   this->getVectorUnitComponents(vectorIndex, xyzComponentsOut);
   magnitudeOut = this->getVectorMagnitude(vectorIndex);
   nodeNumberOut = this->getVectorNodeNumber(vectorIndex);
   this->getVectorColorRGBA(vectorIndex, rgbaColorsOut);
   radiusOut = this->getVectorRadius(vectorIndex);
}

/**
 * Get a data array value.
 */
float
VectorFile::getDataValue(const int arrayIndex,
                         const int vectorIndex) const
{
   float* data = this->dataArrays[arrayIndex]->getDataPointerFloat();
   float value = data[vectorIndex];
   return value;
}

/**
 * set the number of vectors (clears any existing data).
 */
void
VectorFile::setNumberOfVectors(int numberOfVectors)
{
   const int nda = getNumberOfDataArrays();
   for (int i = 0; i < nda; i++) {
      delete dataArrays[i];
   }
   dataArrays.clear();

   if (numberOfVectors > 0){
       std::vector<int> dim;
       dim.push_back(numberOfVectors);
       for (int i = 0; i < NUMBER_OF_ARRAYS; i++) {
          GiftiDataArray* gda = new GiftiDataArray(this,
                                                   getDefaultDataArrayIntent(),
                                                   defaultDataType,
                                                   dim);
          gda->getMetaData()->set(GiftiCommon::tagName,
                                     VectorFile::getDataArrayDescription(
                                        static_cast<DATA_INDEX>(i)));
          this->addDataArray(gda);
       }
   }

   setModified();
}

/**
 * get the vector's origin.
 */
void
VectorFile::getVectorOrigin(const int vectorIndex,
                            float xyzOriginOut[3]) const
{
   xyzOriginOut[0] =
           this->getDataValue(VectorFile::INDEX_X_COORDINATE, vectorIndex);
   xyzOriginOut[1] =
        this->getDataValue(VectorFile::INDEX_Y_COORDINATE, vectorIndex);
   xyzOriginOut[2] =
        this->getDataValue(VectorFile::INDEX_Z_COORDINATE, vectorIndex);
}

/**
 * set the vector's origin.
 */
void
VectorFile::setVectorOrigin(const int vectorIndex,
                            const float xyzOriginIn[3])
{
   this->setDataValue(INDEX_X_COORDINATE, vectorIndex, xyzOriginIn[0]);
   this->setDataValue(INDEX_Y_COORDINATE, vectorIndex, xyzOriginIn[1]);
   this->setDataValue(INDEX_Z_COORDINATE, vectorIndex, xyzOriginIn[2]);
}

/**
 * get the vector's XYZ components.
 */
void
VectorFile::getVectorUnitComponents(const int vectorIndex,
                                    float xyzVectorOut[3]) const
{
   xyzVectorOut[0] =
        this->getDataValue(VectorFile::INDEX_X_COMPONENT, vectorIndex);
   xyzVectorOut[1] =
        this->getDataValue(VectorFile::INDEX_Y_COMPONENT, vectorIndex);
   xyzVectorOut[2] =
        this->getDataValue(VectorFile::INDEX_Z_COMPONENT, vectorIndex);
}

/**
 * set the vector's XYZ components.
 */
void
VectorFile::setVectorUnitComponents(const int vectorIndex,
                                    const float xyzVectorIn[3])
{
   this->setDataValue(INDEX_X_COMPONENT, vectorIndex, xyzVectorIn[0]);
   this->setDataValue(INDEX_Y_COMPONENT, vectorIndex, xyzVectorIn[1]);
   this->setDataValue(INDEX_Z_COMPONENT, vectorIndex, xyzVectorIn[2]);
}

/**
 * get the vectors radius.
 */
float
VectorFile::getVectorRadius(const int vectorIndex) const
{
   return this->getDataValue(VectorFile::INDEX_RADIUS, vectorIndex);
}

/**
 * set the vector's radius.
 */
void
VectorFile::setVectorRadius(const int vectorIndex,
                            const float radiusIn)
{
   this->setDataValue(INDEX_RADIUS, vectorIndex, radiusIn);
}

/**
 * get the vector's rgba colors.
 */
void
VectorFile::getVectorColorRGBA(const int vectorIndex,
                               float rgbaOut[4]) const
{
  rgbaOut[0] = this->getDataValue(VectorFile::INDEX_COLOR_RED, vectorIndex);
  rgbaOut[1] = this->getDataValue(VectorFile::INDEX_COLOR_GREEN, vectorIndex);
  rgbaOut[2] = this->getDataValue(VectorFile::INDEX_COLOR_BLUE, vectorIndex);
  rgbaOut[3] = this->getDataValue(VectorFile::INDEX_COLOR_ALPHA, vectorIndex);
}

/**
 * set the vector's rgba colors.
 */
void
VectorFile::setVectorColorRGBA(const int vectorIndex,
                               const float rgbaIn[4])
{
  this->setDataValue(INDEX_COLOR_RED, vectorIndex, rgbaIn[0]);
  this->setDataValue(INDEX_COLOR_GREEN, vectorIndex, rgbaIn[1]);
  this->setDataValue(INDEX_COLOR_BLUE, vectorIndex, rgbaIn[2]);
  this->setDataValue(INDEX_COLOR_ALPHA, vectorIndex, rgbaIn[3]);
}

/**
 * get the vector's node number.
 */
int
VectorFile::getVectorNodeNumber(const int vectorIndex) const
{
   return static_cast<int>(
              this->getDataValue(VectorFile::INDEX_NODE_NUMBER, vectorIndex));
}

/**
 * set the vector's node number.
 */
void
VectorFile::setVectorNodeNumber(const int vectorIndex,
                       const int nodeNumberIn)
{
   this->setDataValue(INDEX_NODE_NUMBER, vectorIndex, nodeNumberIn);
}

/**
 * get the vector's magnitude.
 */
float
VectorFile::getVectorMagnitude(const int vectorIndex) const
{
   return this->getDataValue(INDEX_MAGNITUDE, vectorIndex);
}

/**
 * set the vector's magnitude.
 */
void
VectorFile::setVectorMagnitude(const int vectorIndex,
                               const float magnitudeIn)
{
   this->setDataValue(INDEX_MAGNITUDE, vectorIndex, magnitudeIn);
}

/**
 * set the vector's data (pass NULL for undesired items).
 */
void
VectorFile::setVectorData(const int vectorIndex,
                          const float xyzOriginIn[3],
                          const float xyzComponentsIn[3],
                          const float magnitudeIn,
                          const int nodeNumberIn,
                          const float rgbaColorsIn[4],
                          const float radiusIn)
{
   bool setColorFlag = true;
   float rgba[4];
   VectorFile::getDefaultColorRGBA(rgba);
   if (rgbaColorsIn != NULL) {
      rgba[0] = rgbaColorsIn[0];
      rgba[1] = rgbaColorsIn[1];
      rgba[2] = rgbaColorsIn[2];
      rgba[3] = rgbaColorsIn[3];
   }
   else if (this->getDataValue(INDEX_COLOR_ALPHA, vectorIndex) != 0.0) {
      setColorFlag = false;
   }

   this->setVectorOrigin(vectorIndex, xyzOriginIn);
   this->setVectorUnitComponents(vectorIndex, xyzComponentsIn);
   this->setVectorMagnitude(vectorIndex, magnitudeIn);
   this->setVectorNodeNumber(vectorIndex, nodeNumberIn);
   if (setColorFlag) {
      this->setVectorColorRGBA(vectorIndex, rgba);
   }
   this->setVectorRadius(vectorIndex, radiusIn);
}

/**
 * set a data array value.
 */
void
VectorFile::setDataValue(const int arrayIndex,
                         const int vectorIndex,
                         const float value)
{
   float* data = this->dataArrays[arrayIndex]->getDataPointerFloat();
   data[vectorIndex] = value;
   setModified();
}

/**
 * }
 * get PubMedID's of all linked studies.
 */
void
VectorFile::getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs) const
{

}

/**
 * get the default color.
 */
void
VectorFile::getDefaultColorRGBA(float rgba[4])
{
   rgba[0] = VectorFile::defaultColor[0];
   rgba[1] = VectorFile::defaultColor[1];
   rgba[2] = VectorFile::defaultColor[2];
   rgba[3] = VectorFile::defaultColor[3];
}

/**
 * Apply transformation matrix to vector file.
 */
void
VectorFile::applyTransformationMatrix(const TransformationMatrix& tmIn)
{
   //
   // Get the transformation matrix and create a second transformation
   // matrix without rotation.
   //
   TransformationMatrix& tm = const_cast<TransformationMatrix&>(tmIn);
   TransformationMatrix tmNoTranslate = tm;
   tmNoTranslate.setTranslation(0.0, 0.0, 0.0);

   const int num = this->getNumberOfVectors();
   for (int i = 0; i < num; i++) {
      //
      // Get the origin and the vector components
      //
      float origin[3];
      float vector[3];
      this->getVectorOrigin(i, origin);
      this->getVectorUnitComponents(i, vector);

      //
      // Create a point at the end of the vector
      //
      float mag = this->getVectorMagnitude(i);
      float endPoint[3] = {
         origin[0] + (vector[0] * mag),
         origin[1] + (vector[1] * mag),
         origin[2] + (vector[2] * mag),
      };

      //
      // Transform the origin
      //
      tm.multiplyPoint(origin);

      //
      // Transform the vector using the without rotation matrix
      // Also normalize the vector
      //
      tmNoTranslate.multiplyPoint(vector);
      MathUtilities::normalize(vector);

      //
      // Store the results
      //
      this->setVectorOrigin(i, origin);
      this->setVectorUnitComponents(i, vector);

      //
      // Transform the endpoint and compare it to the new endpoint
      //
      tm.multiplyPoint(endPoint);
      float endVector[3];
      MathUtilities::subtractVectors(endPoint, origin, endVector);
      MathUtilities::normalize(endVector);
      const float endPointTest[3] = {
         origin[0] + (endVector[0] * mag),
         origin[1] + (endVector[1] * mag),
         origin[2] + (endVector[2] * mag)
      };
      const float endTest[3] = {
         origin[0] + (vector[0] * mag),
         origin[1] + (vector[1] * mag),
         origin[2] + (vector[2] * mag),
      };
      const float diff = MathUtilities::distance3D(endTest, endPointTest);
      if (diff > 0.001) {
         std::cout << "Vector Transform: vector rotation difference: "
                   << diff
                   << std::endl;
      }

   }
   setModified();
}

/**
 * get the data array description.
 */
QString
VectorFile::getDataArrayDescription(const DATA_INDEX dataIndex)
{
   QString s("Unknown");

   switch (dataIndex) {
      case INDEX_NODE_NUMBER:
         s = "Node Number";
         break;
      case INDEX_X_COORDINATE:
         s = "Origin-X";
         break;
      case INDEX_Y_COORDINATE:
         s = "Origin-Y";
         break;
      case INDEX_Z_COORDINATE:
         s = "Origin-Z";
         break;
      case INDEX_X_COMPONENT:
         s = "Unit Vector-X";
         break;
      case INDEX_Y_COMPONENT:
         s = "Unit Vector-Y";
         break;
      case INDEX_Z_COMPONENT:
         s = "Unit Vector-Z";
         break;
      case INDEX_MAGNITUDE:
         s = "Magnitude";
         break;
      case INDEX_RADIUS:
         s = "Radius";
         break;
      case INDEX_COLOR_RED:
         s = "Red";
         break;
      case INDEX_COLOR_GREEN:
         s = "Green";
         break;
      case INDEX_COLOR_BLUE:
         s = "Blue";
         break;
      case INDEX_COLOR_ALPHA:
         s = "Alpha";
         break;
      case NUMBER_OF_ARRAYS:
         s = "Number of Arrays";
         break;
   }

   return s;
}

/**
 * validate the data arrays (optional for subclasses).
 */
void
VectorFile::validateDataArrays() throw (FileException)
{
   if (this->getNumberOfDataArrays() != VectorFile::NUMBER_OF_ARRAYS) {
      throw FileException("The number of data arrays is invalid.  Is the file "
                          "an obsolete version?");
   }
}

/**
 * Write the file's memory in caret6 format to the specified name.
 */
QString
VectorFile::writeFileInCaret6Format(const QString& filenameIn, Structure structure,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException)
{
   int numVectors = this->getNumberOfVectors();
   if (numVectors <= 0) {
      return "";
   }

   QString description;
   for (int i = 0; i < NUMBER_OF_ARRAYS; i++) {
      description += (QString("(") + QString::number(i) + ") "
                      + getDataArrayDescription((DATA_INDEX)i)
                      + ", ");
   }

   QString name = filenameIn;
   if (useCaret6ExtensionFlag) {
      name = FileUtilities::replaceExtension(filenameIn,
                                     SpecFile::getGiftiVectorFileExtension(),
                                     SpecFile::getGiftiVectorFileExtension());
   }

   std::vector<int> dims;
   dims.push_back(numVectors); 
   dims.push_back(NUMBER_OF_ARRAYS);

   GiftiDataArrayFile gdaf;
   GiftiDataArray* gda = new GiftiDataArray(&gdaf,
                                           "CARET_VECTOR",
                                           GiftiDataArray::DATA_TYPE_FLOAT32,
                                           dims);
   gda->getMetaData()->set("Array Columns", description);
   
   gdaf.addDataArray(gda);

   int varIndex[2];
   for (int i = 0; i < numVectors; i++) {
      varIndex[0] = i;

      varIndex[1] = INDEX_NODE_NUMBER;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_X_COORDINATE;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_Y_COORDINATE;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_Z_COORDINATE;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_X_COMPONENT;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_Y_COMPONENT;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_Z_COMPONENT;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_MAGNITUDE;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_RADIUS;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_COLOR_RED;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_COLOR_GREEN;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_COLOR_BLUE;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
      varIndex[1] = INDEX_COLOR_ALPHA;
      gda->setDataFloat32(varIndex, this->getDataValue(varIndex[1], i));
   }

   gdaf.setFileWriteType(AbstractFile::FILE_FORMAT_XML_GZIP_BASE64);
   gdaf.writeFile(name);

   return name;
}

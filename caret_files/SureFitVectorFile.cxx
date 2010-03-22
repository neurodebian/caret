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

#include "DebugControl.h"
#include "SpecFile.h"
#include "SureFitVectorFile.h"
#include "VolumeFile.h"

#include "vtkMath.h"

/**
 * Constructor.
 */
SureFitVectorFile::SureFitVectorFile(const int xdim, const int ydim, const int zdim)
 : AbstractFile("Vector File", 
                SpecFile::getSureFitVectorFileExtension(),
                true,
                AbstractFile::FILE_FORMAT_BINARY,
                FILE_IO_READ_AND_WRITE,
                FILE_IO_READ_AND_WRITE,
                FILE_IO_NONE,
                FILE_IO_NONE)
{
   initialize(xdim, ydim, zdim);
}

/**
 * Constructor.
 */
SureFitVectorFile::SureFitVectorFile()
 : AbstractFile("Vector File", 
                SpecFile::getSureFitVectorFileExtension(),
                true,
                AbstractFile::FILE_FORMAT_BINARY,
                FILE_IO_READ_AND_WRITE,
                FILE_IO_READ_AND_WRITE,
                FILE_IO_NONE,
                FILE_IO_NONE)
{
   initialize(0, 0, 0);
}

/**
 * Destructor.
 */
SureFitVectorFile::~SureFitVectorFile()
{
   clear();
}

/**
 * Initialize the file to specified size with all zero values.
 */
void
SureFitVectorFile::initialize(const int xdim, const int ydim, const int zdim)
{
   dimensions[0] = xdim;
   dimensions[1] = ydim;
   dimensions[2] = zdim;
      
   x.clear();
   y.clear();
   z.clear();
   magnitude.clear();

   numElements = xdim * ydim * zdim;
   if (numElements > 0) {
      x.resize(numElements, 0.0);
      y.resize(numElements, 0.0);
      z.resize(numElements, 0.0);
      magnitude.resize(numElements, 0.0);
   }
}

/**
 * clear the file.
 */
void 
SureFitVectorFile::clear()
{
   clearAbstractFile();
   x.clear();
   y.clear();
   z.clear();
   magnitude.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
SureFitVectorFile::empty() const
{
   return x.empty();
}

/**
 * get the dimensions.
 */
void 
SureFitVectorFile::getDimensions(int dim[3]) const
{
   dim[0] = dimensions[0];
   dim[1] = dimensions[1];
   dim[2] = dimensions[2];
}

/**
 * get the dimensions.
 */
void 
SureFitVectorFile::getDimensions(int& dimX, int& dimY, int& dimZ) const
{
   dimX = dimensions[0];
   dimY = dimensions[1];
   dimZ = dimensions[2];
}

/**
 * get a vector.
 */
void 
SureFitVectorFile::getVectorWithFlatIndex(const int indx, float& xOut, float& yOut, float& zOut) const
{
   xOut = x[indx];
   yOut = y[indx];
   zOut = z[indx];
}

/**
 * get a vector.
 */
void 
SureFitVectorFile::getVectorWithFlatIndex(const int indx, float xyzOut[3]) const
{
   xyzOut[0] = x[indx];
   xyzOut[1] = y[indx];
   xyzOut[2] = z[indx];
}
      
/**
 * set a vector.
 */
void 
SureFitVectorFile::setVectorWithFlatIndex(const int indx, const float xIn,
                      const float yIn, const float zIn)
{
   x[indx] = xIn;
   y[indx] = yIn;
   z[indx] = zIn;
}

/**
 * set a vector.
 */
void 
SureFitVectorFile::setVectorWithFlatIndex(const int indx, float xyzIn[3])
{
   x[indx] = xyzIn[0];
   y[indx] = xyzIn[1];
   z[indx] = xyzIn[2];
}

/**
 * combine a volume with this file's magnitude.
 */
void 
SureFitVectorFile::combineWithVolumeOperation(const COMBINE_VOLUME_OPERATION operation,
                                       const VolumeFile* vf) throw (FileException)
{
   int volDim[3];
   vf->getDimensions(volDim);
   for (int i = 0; i < 3; i++) {
      if (volDim[i] != dimensions[i]) {
         throw FileException(
            "Volume and Vector File dimensions do not match in VectorFile::combineWithVolumeOperation");
      }
   }
   
   const int num = dimensions[0] * dimensions[1] * dimensions[2];
   for (int i = 0; i < num; i++) {
      float value = 0.0;
      const float voxel = vf->getVoxelWithFlatIndex(i);
      switch (operation) {
         case COMBINE_VOLUME_REPLACE_MAGNITUDE_WITH_VOLUME:
            value = voxel;
            break;
         case COMBINE_VOLUME_MULTIPLY_MAGNITUDE_WITH_VOLUME:
            value = voxel * magnitude[i];
            break;
      }
      magnitude[i] = value;
   }
}                            

/**
 * copy magnitude to a volume's voxels (assumes volume properly allocated).
 */
void 
SureFitVectorFile::copyMagnitudeToVolume(VolumeFile* vf) const throw (FileException)
{
   int volDim[3];
   vf->getDimensions(volDim);
   for (int i = 0; i < 3; i++) {
      if (volDim[i] != dimensions[i]) {
         throw FileException(
            "Volume and Vector File dimensions do not match in VectorFile::copyMagnitudeToVolume");
      }
   }
   
   const int num = dimensions[0] * dimensions[1] * dimensions[2];
   for (int i = 0; i < num; i++) {
      vf->setVoxelWithFlatIndex(i, 0, magnitude[i]);
   }
}

/**
 * Combine vector files
 */
void 
SureFitVectorFile::combineVectorFiles (const bool maskingFlag,
                                const COMBINE_OPERATION operation,
                                SureFitVectorFile* vec1,
                                const SureFitVectorFile* vec2,
                                const VolumeFile* maskVolume,
                                SureFitVectorFile* out) throw (FileException)
{
   int dim2[3];
   vec2->getDimensions(dim2);
   
   int dim[3];
   vec1->getDimensions(dim);

   int outDim[3];
   out->getDimensions(outDim);
   
   for (int i = 0; i < 3; i++) {
      if ((dim2[i] != dim[i]) || (dim2[i] != outDim[i])) {
         throw FileException(
            "Vector File dimensions do not match in VectorFile::combineVectorFiles");
      }
   }
   
   
   
   const int num = dim[0] * dim[1] * dim[2];
   out->initialize(dim[0], dim[1], dim[2]);
   
   switch (operation) {
      case 	COMBINE_OPERATION_DOT_SQRT_RECT_MINUS: // dotsqrtrectminus_secondnormal
         for (int i = 0; i < num; i++) {
            vec1->magnitude[i] *= -1;
         }
         for (int i = 0; i < num; i++) {
            if (DebugControl::getDebugOn()) {
               if ((i % 1000000) == 0) {
                  std::cout << "\t" << i << " of " << num 
                            << ": " << 100.0*((float)i/(float)(num))
                            << std::endl;
               }
            }
            if (((maskingFlag) && (maskVolume->getVoxelWithFlatIndex(i) != 0)) || 
                  (maskingFlag == false)){
               float vector1[3], vector2[3];
               vector1[0] = vec1->x[i] * vec1->magnitude[i];
               vector1[1] = vec1->y[i] * vec1->magnitude[i];
               vector1[2] = vec1->z[i] * vec1->magnitude[i];
               vector2[0] = vec2->x[i] * vec2->magnitude[i];
               vector2[1] = vec2->y[i] * vec2->magnitude[i];
               vector2[2] = vec2->z[i] * vec2->magnitude[i];
               out->magnitude[i] = vtkMath::Dot(vector1, vector2); 
               out->magnitude[i] = std::max(out->magnitude[i], 0.0f);
               out->magnitude[i] = sqrt(out->magnitude[i]);
            }
            else {
               out->magnitude[i] = 0.0;
            }
         }
         for (int i = 0; i < num; i++) {
            if (DebugControl::getDebugOn()) {
               if ((i % 1000000) == 0) {
                  std::cout << "\t" << i << " of " << num 
                            << ": " << 100.0*((float)i/(float)(num))
                            << std::endl;
               }
            }
            if (((maskingFlag) && (maskVolume->getVoxelWithFlatIndex(i) != 0)) || 
                (maskingFlag == false)){
               // HAD 7.14.99 Changed via DVE instructions 
               out->x[i] = vec2->x[i];
               out->y[i] = vec2->y[i];
               out->z[i] = vec2->z[i];
            }else{
               out->x[i] = 0.0; 
               out->y[i] = 0.0; 
               out->z[i] = 0.0; 
            }
         }
         break;
	   case COMBINE_OPERATION_2_VEC_NORMAL: // 2vec_secondnormal
         for (int i = 0; i < num; i++){
            if (DebugControl::getDebugOn()) {
               if ((i % 1000000) == 0) {
                  std::cout << "\t" << i << " of " << num 
                            << ": " << 100.0*((float)i/(float)(num))
                            << std::endl;
               }
            }
            if (((maskingFlag) && (maskVolume->getVoxelWithFlatIndex(i) != 0)) ||
                (maskingFlag == false)){
               float vector1[3], vector2[3];
               vector1[0] = vec1->x[i];
               vector1[1] = vec1->y[i];
               vector1[2] = vec1->z[i];
               vector2[0] = vec2->x[i];
               vector2[1] = vec2->y[i];
               vector2[2] = vec2->z[i];
               // HAD 7.14.99 Changed via DVE instructions
               if (vec1->magnitude[i] > vec2->magnitude[i])
                  out->magnitude[i] = vec1->magnitude[i];
               else
                  out->magnitude[i] = vec2->magnitude[i];
               out->x[i] = vec2->x[i];        
               out->y[i] = vec2->y[i];        
               out->z[i] = vec2->z[i];        
            }
         }
         break;
      case COMBINE_OPERATION_2_VEC: // 2vec
         for (int i = 0; i < num; i++) {
            if (DebugControl::getDebugOn()) {
               if ((i % 1000000) == 0) {
                  std::cout << "\t" << i << " of " << num 
                            << ": " << 100.0*((float)i/(float)(num))
                            << std::endl;
               }
            }
            if (((maskingFlag) && (maskVolume->getVoxelWithFlatIndex(i) != 0)) || 
                (maskingFlag == false)){
               float vector1[3], vector2[3];
               vector1[0] = vec1->x[i];
               vector1[1] = vec1->y[i];
               vector1[2] = vec1->z[i];
               vector2[0] = vec2->x[i];
               vector2[1] = vec2->y[i];
               vector2[2] = vec2->z[i];
               const float mag1 = vtkMath::Norm(vector1);
               const float mag2 = vtkMath::Norm(vector2);
               if (vec1->magnitude[i] > vec2->magnitude[i]) {
                  out->magnitude[i] = vec1->magnitude[i];
                  out->x[i] = vec1->x[i];        
                  out->y[i] = vec1->y[i];        
                  out->z[i] = vec1->z[i];        
               } 
               else { 
                  out->magnitude[i] = vec2->magnitude[i];
                  out->x[i] = vec2->x[i];        
                  out->y[i] = vec2->y[i];        
                  out->z[i] = vec2->z[i];        
                  const float temp = vtkMath::Dot(vector1, vector2); 
                  float tempAbs = temp;
                  if (tempAbs < 0.0) {
                     tempAbs = -tempAbs;
                  }
                  const float temp2 = temp / tempAbs;
                  out->x[i] *= temp2;
                  out->y[i] *= temp2;
                  out->z[i] *= temp2;
               }
               if (mag1 == 0.0){ 
                  out->x[i] = vec2->x[i];        
                  out->y[i] = vec2->y[i];        
                  out->z[i] = vec2->z[i];        
               }
               if (mag2 == 0.0){ 
                  out->x[i] = vec1->x[i];        
                  out->y[i] = vec1->y[i];        
                  out->z[i] = vec1->z[i];        
               }
            }
         }
         break;
	}
}

/**
 * multiply x,y,z by magnitude.
 */
void 
SureFitVectorFile::multiplyXYZByMagnitude()
{
   const int num = dimensions[0] * dimensions[1] * dimensions[2];
   for (int i = 0; i < num; i++) {
      x[i] *= magnitude[i];
      y[i] *= magnitude[i];
      z[i] *= magnitude[i];
   }
}      
      
/**
 * get a pointer to the X data.
 */
float* 
SureFitVectorFile::getWithFlatIndexValueX(const int indx)
{
   return &x[indx];
}

/**
 * get a pointer to the Y data.
 */
float* 
SureFitVectorFile::getWithFlatIndexValueY(const int indx)
{
   return &y[indx];
}

/**
 * get a pointer to the Z data.
 */
float* 
SureFitVectorFile::getWithFlatIndexValueZ(const int indx)
{
   return &z[indx];
}

/**
 * get a vector.
 */
void 
SureFitVectorFile::getVector(const int i, const int j, const int k, float vector[3]) const
{
   const int indx = getDataIndex(i, j, k);
   vector[0] = x[indx];
   vector[1] = y[indx];
   vector[2] = z[indx];
}

/**
 * get magnitude.
 */
float 
SureFitVectorFile::getMagnitude(const int i, const int j, const int k) const
{
   const int indx = getDataIndex(i, j, k);
   return magnitude[indx];
}

/**
 * get data index.
 */
int 
SureFitVectorFile::getDataIndex(const int ijk[3]) const
{
   return getDataIndex(ijk[0], ijk[1], ijk[2]);
}

/**
 * get flat data.
 */
int 
SureFitVectorFile::getDataIndex(const int i, const int j, const int k) const
{      
   const int indx = i + j * dimensions[0] + k * dimensions[0] * dimensions[1];
   return indx;
}

/**
 * read the file's data (header has already been read).
 */
void 
SureFitVectorFile::readFileData(QFile&,
                         QTextStream& stream,
                         QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            int dim[3];
            stream >> dim[0] >> dim[1] >> dim[2];
            initialize(dim[0], dim[1], dim[2]);
            
            for (int i = 0; i < numElements; i++) {
               stream >> x[i];
               stream >> y[i];
               stream >> z[i];
               stream >> magnitude[i];
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         {
            int dim[3];
            binStream >> dim[0] >> dim[1] >> dim[2];
            initialize(dim[0], dim[1], dim[2]);
            
            for (int i = 0; i < numElements; i++) {
               binStream >> x[i];
            }
            for (int i = 0; i < numElements; i++) {
               binStream >> y[i];
            }
            for (int i = 0; i < numElements; i++) {
               binStream >> z[i];
            }
            for (int i = 0; i < numElements; i++) {
               binStream >> magnitude[i];
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Vector File does not support XML for reading.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Vector File does not support \"Other\" for reading.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }   
}

/**
 * Write the file's data (header has already been written).
 */
void 
SureFitVectorFile::writeFileData(QTextStream& stream,
                          QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         {
            stream << dimensions[0] << " "
                   << dimensions[1] << " "
                   << dimensions[2] << " "
                   << "\n";
            
            for (int i = 0; i < numElements; i++) {
               stream << x[i] << " "
                      << y[i] << " "
                      << z[i] << " "
                      << magnitude[i]
                      << "\n";
            }            
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
         {
            binStream << dimensions[0] << dimensions[1] << dimensions[2];
            
            for (int i = 0; i < numElements; i++) {
               binStream << x[i];
            }
            for (int i = 0; i < numElements; i++) {
               binStream << y[i];
            }
            for (int i = 0; i < numElements; i++) {
               binStream << z[i];
            }
            for (int i = 0; i < numElements; i++) {
               binStream << magnitude[i];
            }
         }
         break;
      case FILE_FORMAT_XML:
         throw FileException(filename, "Vector File does not support XML for writing.");
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Vector File does not support \"Other\" for writing.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
}

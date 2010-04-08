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

#define _TRANSFORMATION_FILE_MAIN_
#include "TransformationMatrixFile.h"
#undef _TRANSFORMATION_FILE_MAIN_

#include "MathUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

//--------------------------------------------------------------------------

/*
 * Constructor
 */
TransformationMatrix::TransformationMatrix()
{
   matrixFile = NULL;
   clear();
}

/*
 * Copy Constructor
 */
TransformationMatrix::TransformationMatrix(const TransformationMatrix& tm)
{
   copyData(tm);
}

/*
 * assignment operator
 */
TransformationMatrix& 
TransformationMatrix::operator=(const TransformationMatrix& tm)
{
   copyData(tm);
   return *this;
}

/*
 * copy all data from another object to this object
 */
void 
TransformationMatrix::copyData(const TransformationMatrix& tm)
{
   matrixFile = NULL;
   clear();
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrix[i][j] = tm.matrix[i][j];
      }
   }
   matrixFile = NULL;  //will get set when added to file tm.matrixFile;
   
   setMatrixName(tm.getMatrixName());
   setMatrixComment(tm.getMatrixComment());
   
   int x, y, z;
   tm.getMatrixTargetACCoords(x, y, z);
   setMatrixTargetACCoords(x, y, z);
   
   tm.getMatrixTargetVolumeDimensions(x, y, z);
   setMatrixTargetVolumeDimensions(x, y, z);
   
   setMatrixTargetVolumeFileName(tm.getMatrixTargetVolumeFileName());
   setMatrixFiducialCoordFileName(tm.getMatrixFiducialCoordFileName());
   
   showAxes = tm.showAxes;
   axesLength = tm.axesLength;
}
   
/*
 * Destructor
 */
TransformationMatrix::~TransformationMatrix()
{
}

/**
 * Set the matrix file that contains this matrix as modified.
 */
void
TransformationMatrix::setMatrixFileModified()
{
   if (matrixFile != NULL) {
      matrixFile->setModified();
   }
}

/*
 * clear the matrix file
 */
void 
TransformationMatrix::clear()
{
   identity();
   std::ostringstream str;
   str << "Matrix "
       << matrixNumberCounter;
   matrixNumberCounter++;
   name = str.str().c_str();
   comment = "";
   targetFiducialCoordFileName = "";
   targetVolumeFileName = "";
   acCoords[0] = -1;
   acCoords[1] = -1;
   acCoords[2] = -1;
   volumeDimensions[0] = -1;
   volumeDimensions[1] = -1;
   volumeDimensions[2] = -1;
   setMatrixFileModified();
   showAxes = false;
   axesLength = 100.0;
   
}

/**
 * get the translation from the matrix.
 */
void
TransformationMatrix::getTranslation(double& tx, double& ty, double& tz) const
{
   vtkTransform* tm = vtkTransform::New();
   getMatrix(tm);
   double  pos[3];
   tm->GetPosition(pos);
   tx = pos[0];
   ty = pos[1];
   tz = pos[2];
   tm->Delete();
}

/**
 * get the translation from the matrix.
 */
void 
TransformationMatrix::getTranslation(float& tx, float& ty, float& tz) const
{
   double txd, tyd, tzd;
   getTranslation(txd, tyd, tzd);
   tx = txd;
   ty = tyd;
   tz = tzd;
}

/**
 * sets (overrides) the translation in the matrix.
 */
void 
TransformationMatrix::setTranslation(const double tx, const double ty, const double tz)
{
   matrix[0][3] = tx;
   matrix[1][3] = ty;
   matrix[2][3] = tz;
}

/**
 * sets (overrides) the translation in the matrix.
 */
void 
TransformationMatrix::setTranslation(const float tx, const float ty, const float tz)
{
   matrix[0][3] = tx;
   matrix[1][3] = ty;
   matrix[2][3] = tz;
}
      
/**
 * get the rotation angles from the matrix.
 */
void 
TransformationMatrix::getRotationAngles(double& rx, double& ry, double& rz) const
{
   vtkTransform* tm = vtkTransform::New();
   getMatrix(tm);
   double  angles[3];
   tm->GetOrientation(angles);
   rx = angles[0];
   ry = angles[1];
   rz = angles[2];
   tm->Delete();
}

/**
 * get the rotation angles from the matrix.
 */
void 
TransformationMatrix::getRotationAngles(float& rx, float& ry, float& rz) const
{
   double rxd, ryd, rzd;
   getRotationAngles(rxd, ryd, rzd);
   rx = rxd;
   ry = ryd;
   rz = rzd;
}

/**
 * get the scaling from the matrix (incorrect in some circumstances).
 */
void 
TransformationMatrix::getScaling(double& sx, double& sy, double& sz) const
{
   vtkTransform* tm = vtkTransform::New();
   getMatrix(tm);
   double  scale[3];
   tm->GetScale(scale);
   MathUtilities::sign(scale[0], matrix[0][0]);
   MathUtilities::sign(scale[1], matrix[1][1]);
   MathUtilities::sign(scale[2], matrix[2][2]);
   sx = scale[0];
   sy = scale[1];
   sz = scale[2];
   tm->Delete();
}

/**
 * get the scaling from the matrix (incorrect in some circumstances).
 */
void 
TransformationMatrix::getScaling(float& sx, float& sy, float& sz) const
{
   double sxd, syd, szd;
   getScaling(sxd, syd, szd);
   sx = sxd;
   sy = syd;
   sz = szd;
}
      
/*
 * get the entire matrix
 */
void 
TransformationMatrix::getMatrix(double m[4][4]) const
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         m[i][j] = matrix[i][j];
      }
   }
}

/**
 * get the entire matrix.
 */
void 
TransformationMatrix::getMatrix(float m[4][4]) const
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         m[i][j] = static_cast<float>(matrix[i][j]);
      }
   }
   TransformationMatrix tm = *this;
}
 
/**
 * set the matrix.
 */
void 
TransformationMatrix::setMatrix(const double translateV[3],
                                const double rotateV[3],
                                const double scaleV[3])
{
   vtkTransform* transform = vtkTransform::New();
   transform->PreMultiply();
   transform->Translate(translateV);
   transform->RotateZ(rotateV[2]);
   transform->RotateX(rotateV[0]);
   transform->RotateY(rotateV[1]);
   transform->Scale(scaleV);
   setMatrix(transform);
   transform->Delete();
}
                     
/**
 * set the entire matrix.
 */
void 
TransformationMatrix::setMatrix(const float m[16])
{
   matrix[0][0] = m[0];
   matrix[1][0] = m[1];
   matrix[2][0] = m[2];
   matrix[3][0] = m[3];
   matrix[0][1] = m[4];
   matrix[1][1] = m[5];
   matrix[2][1] = m[6];
   matrix[3][1] = m[7];
   matrix[0][2] = m[8];
   matrix[1][2] = m[9];
   matrix[2][2] = m[10];
   matrix[3][2] = m[11];
   matrix[0][3] = m[12];
   matrix[1][3] = m[13];
   matrix[2][3] = m[14];
   matrix[3][3] = m[15];
   setMatrixFileModified();
}
      
/**
 * set the entire matrix.
 */
void 
TransformationMatrix::setMatrix(const double m[16])
{
   matrix[0][0] = m[0];
   matrix[1][0] = m[1];
   matrix[2][0] = m[2];
   matrix[3][0] = m[3];
   matrix[0][1] = m[4];
   matrix[1][1] = m[5];
   matrix[2][1] = m[6];
   matrix[3][1] = m[7];
   matrix[0][2] = m[8];
   matrix[1][2] = m[9];
   matrix[2][2] = m[10];
   matrix[3][2] = m[11];
   matrix[0][3] = m[12];
   matrix[1][3] = m[13];
   matrix[2][3] = m[14];
   matrix[3][3] = m[15];
   setMatrixFileModified();
}
      
/**
 * get the entire matrix.
 */
void 
TransformationMatrix::getMatrix(float m[16]) const
{
   m[0]  = matrix[0][0];
   m[1]  = matrix[1][0];
   m[2]  = matrix[2][0];
   m[3]  = matrix[3][0];
   m[4]  = matrix[0][1];
   m[5]  = matrix[1][1];
   m[6]  = matrix[2][1];
   m[7]  = matrix[3][1];
   m[8]  = matrix[0][2];
   m[9]  = matrix[1][2];
   m[10] = matrix[2][2];
   m[11] = matrix[3][2];
   m[12] = matrix[0][3];
   m[13] = matrix[1][3];
   m[14] = matrix[2][3];
   m[15] = matrix[3][3];
}
      
/**
 * get the entire matrix.
 */
void 
TransformationMatrix::getMatrix(double m[16]) const
{
   m[0]  = matrix[0][0];
   m[1]  = matrix[1][0];
   m[2]  = matrix[2][0];
   m[3]  = matrix[3][0];
   m[4]  = matrix[0][1];
   m[5]  = matrix[1][1];
   m[6]  = matrix[2][1];
   m[7]  = matrix[3][1];
   m[8]  = matrix[0][2];
   m[9]  = matrix[1][2];
   m[10] = matrix[2][2];
   m[11] = matrix[3][2];
   m[12] = matrix[0][3];
   m[13] = matrix[1][3];
   m[14] = matrix[2][3];
   m[15] = matrix[3][3];
}
      
/*
 * get an element from the matrix
 */
double 
TransformationMatrix::getMatrixElement(const int row, 
                                       const int column) const
{
   return matrix[row][column];
}

/*
 * set the entire matrix
 */
void 
TransformationMatrix::setMatrix(const double m[4][4])
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrix[i][j] = m[i][j];
      }
   }
   setMatrixFileModified();
}

/*
 * set the entire matrix
 */
void 
TransformationMatrix::setMatrix(const float m[4][4])
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrix[i][j] = m[i][j];
      }
   }
   setMatrixFileModified();
}

/*
 * set an element of the matrix
 */
void 
TransformationMatrix::setMatrixElement(const int row, 
                                           const int column,
                                           const double value)
{
   matrix[row][column] = value;
   setMatrixFileModified();
}

/*
 * get the volume dimeensions
 */
void 
TransformationMatrix::getMatrixTargetVolumeDimensions(int& x, int& y, 
                                                      int& z) const
{
   x = volumeDimensions[0];
   y = volumeDimensions[1];
   z = volumeDimensions[2];
}

/*
 * set the AC coords
 */
void TransformationMatrix::setMatrixTargetACCoords(const int x, const int y, 
                                                   const int z)
{
   acCoords[0] = x;
   acCoords[1] = y;
   acCoords[2] = z;
   setMatrixFileModified();
}
      
/*
 * get the AC coords
 */
void 
TransformationMatrix::getMatrixTargetACCoords(int& x, int& y, int& z) const
{
   x = acCoords[0];
   y = acCoords[1];
   z = acCoords[2];
}
      
 
/*   
 * set the volume dimeensions
 */
void 
TransformationMatrix::setMatrixTargetVolumeDimensions(const int x, const int y,
                                                       const int z)
{
   volumeDimensions[0] = x;
   volumeDimensions[1] = y;
   volumeDimensions[2] = z;
   setMatrixFileModified();
}

/*
 * read the 4x4 matrix
 */
void
TransformationMatrix::readMatrixData(QTextStream& stream,
                                     const QString& firstLine,
                                     const QString& filename) throw (FileException)
{
   for (int i = 0; i < 4; i++) {
      QString line;
      std::vector<QString> tokens;
      if ((i == 0) && (firstLine.isEmpty() == false)) {
         line = firstLine;
         StringUtilities::token(line, " \t", tokens);
      }
      else {
         AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      }
      
      if (tokens.size() < 4) {
         QString msg("Matrix line missing data: ");
         msg.append(line);
         throw FileException(filename, msg);
      }
      else {
         setMatrixElement(i, 0, tokens[0].toFloat());
         setMatrixElement(i, 1, tokens[1].toFloat());
         setMatrixElement(i, 2, tokens[2].toFloat());
         setMatrixElement(i, 3, tokens[3].toFloat());
      }
   }
}
   
/*
 * read a transformation matrix
 */
void
TransformationMatrix::readMatrix(QTextStream& stream,
                                 const QString& filename) throw (FileException)
{
   bool readingTags = true;
   while (readingTags) {
      QString tag, value;
      AbstractFile::readTagLine(filename, stream, tag, value);

      if (tag == tagMatrixName) {
         name = value;
      }
      else if (tag == tagMatrixComment) {
         comment = value;
      }
      else if (tag == tagMatrixTargetVolumeFileName) {
         targetVolumeFileName = value;
      }
      else if (tag == tagMatrixFiducialCoordFileName) {
         targetFiducialCoordFileName = value;
      }
      else if (tag == tagMatrixTargetVolumeDimensions) {
         std::vector<QString> tokens;
         StringUtilities::token(value, " ", tokens);
         if (tokens.size() < 3) {
            QString msg("Error reading line with volume dimensions ");
            msg.append(value);
            throw FileException(filename, msg);
         }
         volumeDimensions[0] = tokens[0].toInt();
         volumeDimensions[1] = tokens[1].toInt();
         volumeDimensions[2] = tokens[2].toInt();
      }
      else if (tag == tagMatrixTargetACCoords) {
         std::vector<QString> tokens;
         StringUtilities::token(value, " ", tokens);
         if (tokens.size() < 3) {
            QString msg("Error reading line with ac coords ");
            msg.append(value);
            throw FileException(filename, msg);
         }
         acCoords[0] = tokens[0].toInt();
         acCoords[1] = tokens[1].toInt();
         acCoords[2] = tokens[2].toInt();
      }
      else if (tag == tagMatrixBegin) {
         readingTags = false;
      }
   }
   
   return readMatrixData(stream, "", filename);
}  

/*
 * write a transformation matrix
 */
void
TransformationMatrix::writeMatrix(QTextStream& stream)
{
   stream << tagMatrixName << " " << name << "\n";
   
   stream << tagMatrixComment << " " << comment << "\n";
   
   stream << tagMatrixTargetVolumeFileName << " " << 
                          targetVolumeFileName << "\n";
   
   stream << tagMatrixTargetVolumeDimensions << " " << 
           volumeDimensions[0] << " " <<  volumeDimensions[1] << " " <<  volumeDimensions[2] << "\n";
           
   stream << tagMatrixFiducialCoordFileName << " " << 
                          targetFiducialCoordFileName << "\n";
   
   stream << tagMatrixTargetACCoords << " " << 
           acCoords[0] << " " << acCoords[1] << " " << acCoords[2] << "\n";
           
   stream << tagMatrixBegin << "\n";
   
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         stream << getMatrixElement(i, j) << " ";
      }
      stream << "\n";
   }
}

                
/*
 * set the matrix to the identity matrix
 */
void
TransformationMatrix::identity()
{
   matrix[0][0] = 1.0;
   matrix[0][1] = 0.0;
   matrix[0][2] = 0.0;
   matrix[0][3] = 0.0;
   matrix[1][0] = 0.0;
   matrix[1][1] = 1.0;
   matrix[1][2] = 0.0;
   matrix[1][3] = 0.0;
   matrix[2][0] = 0.0;
   matrix[2][1] = 0.0;
   matrix[2][2] = 1.0;
   matrix[2][3] = 0.0;
   matrix[3][0] = 0.0;
   matrix[3][1] = 0.0;
   matrix[3][2] = 0.0;
   matrix[3][3] = 1.0;
   setMatrixFileModified();
}

/**
 * get the matrix as a VTK 4x4 matrix.
 */
void 
TransformationMatrix::getMatrix(vtkMatrix4x4* m) const
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         m->SetElement(i, j, matrix[i][j]);
      }
   }
}

/**
 * set the matrix from a VTK 4x4 matrix.
 */
void 
TransformationMatrix::setMatrix(const vtkMatrix4x4* m)
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         matrix[i][j] = m->GetElement(i, j);
      }
   }
   setMatrixFileModified();
}
      
/**
 * get the matrix as a VTK transform matrix.
 */
void 
TransformationMatrix::getMatrix(vtkTransform* m) const
{
   vtkMatrix4x4* m44 = vtkMatrix4x4::New();
   getMatrix(m44);
   m->SetMatrix(m44);
   m44->Delete();
}

/**
 * set the matrix from a VTK transform matrix.
 */
void 
TransformationMatrix::setMatrix(const vtkTransform* m)
{
   vtkMatrix4x4* m44 = vtkMatrix4x4::New();
   ((vtkTransform*)m)->GetMatrix(m44);
   setMatrix(m44);
   setMatrixFileModified();
   m44->Delete();
}
      
/*
 * invert the matrix
 */
void
TransformationMatrix::inverse()
{
   vtkTransform* m = vtkTransform::New();
   getMatrix(m);
   m->Inverse();
   setMatrix(m);
   setMatrixFileModified();
   m->Delete();
}

/*
 * transpose the matrix
 */
void
TransformationMatrix::transpose()
{
   std::swap(matrix[1][0], matrix[0][1]);
   std::swap(matrix[2][0], matrix[0][2]);
   std::swap(matrix[2][1], matrix[1][2]);
   std::swap(matrix[3][0], matrix[0][3]);
   std::swap(matrix[3][1], matrix[1][3]);
   std::swap(matrix[3][2], matrix[2][3]);
   setMatrixFileModified();
}

//
// Multiply by the matrix in the specified file
//
// int
// TransformationMatrix::multiply(const string& matrixFilename)
// {
//    TransformationMatrixFile tmf(matrixFilename);
//    if (tmf.isBad()) {
//       return -1;
//    }
//    multiply(tmf);
//    return 0;
// }

/*
 * Multiply by the TransformationMatrix
 */
void
TransformationMatrix::preMultiply(const TransformationMatrix& tm)
{
   double matrixOut[4][4];
   for (int row = 0; row < 4; row++) {
         matrixOut[row][0] = tm.matrix[row][0] * matrix[0][0]
                           + tm.matrix[row][1] * matrix[1][0]
                           + tm.matrix[row][2] * matrix[2][0]
                           + tm.matrix[row][3] * matrix[3][0];
         matrixOut[row][1] = tm.matrix[row][0] * matrix[0][1]
                           + tm.matrix[row][1] * matrix[1][1]
                           + tm.matrix[row][2] * matrix[2][1]
                           + tm.matrix[row][3] * matrix[3][1];
         matrixOut[row][2] = tm.matrix[row][0] * matrix[0][2]
                           + tm.matrix[row][1] * matrix[1][2]
                           + tm.matrix[row][2] * matrix[2][2]
                           + tm.matrix[row][3] * matrix[3][2];
         matrixOut[row][3] = tm.matrix[row][0] * matrix[0][3]
                           + tm.matrix[row][1] * matrix[1][3]
                           + tm.matrix[row][2] * matrix[2][3]
                           + tm.matrix[row][3] * matrix[3][3];
   }
   setMatrix(matrixOut);
   setMatrixFileModified();
}

/*
 * Post-Multiply by the TransformationMatrix
 */
void
TransformationMatrix::postMultiply(const TransformationMatrix& tm)
{
   double matrixOut[4][4];
   for (int row = 0; row < 4; row++) {
         matrixOut[row][0] = matrix[row][0] * tm.matrix[0][0]
                           + matrix[row][1] * tm.matrix[1][0]
                           + matrix[row][2] * tm.matrix[2][0]
                           + matrix[row][3] * tm.matrix[3][0];
         matrixOut[row][1] = matrix[row][0] * tm.matrix[0][1]
                           + matrix[row][1] * tm.matrix[1][1]
                           + matrix[row][2] * tm.matrix[2][1]
                           + matrix[row][3] * tm.matrix[3][1];
         matrixOut[row][2] = matrix[row][0] * tm.matrix[0][2]
                           + matrix[row][1] * tm.matrix[1][2]
                           + matrix[row][2] * tm.matrix[2][2]
                           + matrix[row][3] * tm.matrix[3][2];
         matrixOut[row][3] = matrix[row][0] * tm.matrix[0][3]
                           + matrix[row][1] * tm.matrix[1][3]
                           + matrix[row][2] * tm.matrix[2][3]
                           + matrix[row][3] * tm.matrix[3][3];
   }
   setMatrix(matrixOut);
   setMatrixFileModified();
}

/*
 * Apply the transformation matrix to a point
 */
void
TransformationMatrix::multiplyPoint(double p[3]) const
{
   double pout[3];
   for (int row = 0; row < 3; row++) {
      pout[row] = matrix[row][0] * p[0]
                + matrix[row][1] * p[1]
                + matrix[row][2] * p[2]
                + matrix[row][3];
   }
   p[0] = pout[0];
   p[1] = pout[1];
   p[2] = pout[2];
}

/**
 * Apply the transformation matrix to a point.
 */
void 
TransformationMatrix::multiplyPoint(float p[3]) const
{
   double d[3] = { p[0], p[1], p[2] };
   multiplyPoint(d);
   p[0] = d[0];
   p[1] = d[1];
   p[2] = d[2];
}

/**
 * Apply the inverse of a transformation matrix to a point.
 */
void 
TransformationMatrix::inverseMultiplyPoint(float p[3]) const
{
   double d[3] = { p[0], p[1], p[2] };
   inverseMultiplyPoint(d);
   p[0] = d[0];
   p[1] = d[1];
   p[2] = d[2];
}

/*
 * Apply the inverse of the transformation matrix to a point
 */
void
TransformationMatrix::inverseMultiplyPoint(double p[3]) const
{
   TransformationMatrix inverseMatrix = *this;
   inverseMatrix.inverse();
   inverseMatrix.multiplyPoint(p);
}

/**
 * rotate using the three angles relative to "relativeMatrix".
 */
void 
TransformationMatrix::rotate(const double angles[3], 
                             vtkTransform* relativeMatrix)
{
/*
   double rotX = angles[0];
   double rotY = angles[1];
   double rotZ = angles[2];
   if (relativeMatrix != NULL) {
      float dt[4] = { rotX, rotY, rotZ, 1.0 };
      float dt2[4];
      relativeMatrix->MultiplyPoint(dt, dt2);
      rotX = dt2[0];
      rotY = dt2[1];
      rotZ = dt2[2];
   }
   
   vtkTransform* m = vtkTransform::New();
   m->PostMultiply();
   const double t[3] = { matrix[0][3], matrix[1][3], matrix[2][3] };
   matrix[0][3] = 0.0;
   matrix[1][3] = 0.0;
   matrix[2][3] = 0.0;
   getMatrix(m);
   if (rotZ != 0.0) {
      m->RotateZ(rotZ);
   }
   if (rotX != 0.0) {
      m->RotateX(rotX);
   }
   if (rotY != 0.0) {
      m->RotateY(rotY);
   }
   setMatrix(m);
   matrix[0][3] = t[0];
   matrix[1][3] = t[1];
   matrix[2][3] = t[2];
   setMatrixFileModified();
   m->Delete();
*/   
/*
   if (rotX != 0.0) {
      rotateX(rotX);
   }
   if (rotY != 0.0) {
      rotateY(rotY);
   }
   if (rotZ != 0.0) {
      rotateZ(rotZ);
   }
*/
   double rx = angles[0];
   double ry = angles[1];
   double rz = angles[2];
   if (relativeMatrix != NULL) {
      double dt[4] = { rx, ry, rz, 1.0 };
      double dt2[4];
      relativeMatrix->MultiplyPoint(dt, dt2);
   /*
      vtkMatrix4x4* m = vtkMatrix4x4::New();
      relativeMatrix->GetInverse(m);
      m->MultiplyPoint(dt, dt2);
      m->Delete();
   */
      rx = dt2[0];
      ry = dt2[1];
      rz = dt2[2];
   }
   rotateZ(rz);
   rotateX(rx);
   rotateY(ry);
}
            
/**
 * Multiply by the matrix with the specified translation relative to "relativeMatrix".
 */
void 
TransformationMatrix::translate(const double translateXYZ[3], vtkTransform* relativeMatrix)
{
   double tx = translateXYZ[0];
   double ty = translateXYZ[1];
   double tz = translateXYZ[2];
   if (relativeMatrix != NULL) {
      double dt[4] = { tx, ty, tz, 1.0 };
      double dt2[4];
      
      relativeMatrix->MultiplyPoint(dt, dt2);
   /*
      vtkMatrix4x4* m = vtkMatrix4x4::New();
      relativeMatrix->GetInverse(m);
      m->MultiplyPoint(dt, dt2);
      m->Delete();
   */
      tx = dt2[0];
      ty = dt2[1];
      tz = dt2[2];
   }
   translate(tx, ty, tz);
}
      
/**
 * rotate about the X axis.
 */
void 
TransformationMatrix::rotateX(const double rotateDegrees)
{
   rotate(ROTATE_X_AXIS, rotateDegrees);
}

/**
 * rotate about the Y axis.
 */
void 
TransformationMatrix::rotateY(const double rotateDegrees)
{
   rotate(ROTATE_Y_AXIS, rotateDegrees);
}

/**
 * rotate about the Z axis.
 */
void 
TransformationMatrix::rotateZ(const double rotateDegrees)
{
   rotate(ROTATE_Z_AXIS, rotateDegrees);
}
      
/*
 * Multiply by the matrix containing the specified rotation
 */
void
TransformationMatrix::rotate(const TransformationMatrix::ROTATE_AXIS rotationAxis,
                             const double rotateDegrees)
{
   double axis[3] = { 0.0, 0.0, 0.0 };
   switch(rotationAxis) {
      case ROTATE_X_AXIS:
         axis[0] = 1.0;
         break;
      case ROTATE_Y_AXIS:
         axis[1] = 1.0;
         break;
      case ROTATE_Z_AXIS:
         axis[2] = 1.0;
         break;
   }
   rotate(rotateDegrees, axis);
   setMatrixFileModified();
}

/**
 * Multiply by the matrix containing the specified rotation.
 * about the specified axis.
 */
void 
TransformationMatrix::rotate(const double rotateDegrees, const double axis[3])
{
   vtkTransform* m = vtkTransform::New();
   getMatrix(m);
   m->RotateWXYZ(rotateDegrees, axis);
   setMatrix(m);
   setMatrixFileModified();
   m->Delete();
}

/*
 * Multiply by the matrix with the specified scaling
 */
void
TransformationMatrix::scale(const double scaleX,
                            const double scaleY,
                            const double scaleZ)
{
   TransformationMatrix m;
   m.matrix[0][0] = scaleX;
   m.matrix[1][1] = scaleY;
   m.matrix[2][2] = scaleZ;
   preMultiply(m);
   setMatrixFileModified();
}

/*
 * Multiply by the matrix with the specified scaling
 */
void
TransformationMatrix::scale(const double scaleXYZ[3])
{
   scale(scaleXYZ[0], scaleXYZ[1], scaleXYZ[2]);
   setMatrixFileModified();
}

/*
 * Multiply by the matrix with the specified scaling
 */
void
TransformationMatrix::scale(const float scaleX,
                            const float scaleY,
                            const float scaleZ)
{
   scale(static_cast<double>(scaleX), 
         static_cast<double>(scaleY), 
         static_cast<double>(scaleZ));
   setMatrixFileModified();
}

/*
 * Multiply by the matrix with the specified scaling
 */
void
TransformationMatrix::scale(const float scaleXYZ[3])
{
   scale(scaleXYZ[0], scaleXYZ[1], scaleXYZ[2]);
   setMatrixFileModified();
}

/*
 * Multiply by the matrix with the specified translation
 */
void
TransformationMatrix::translate(const double translateX,
                                const double translateY,
                                const double translateZ)
{
   TransformationMatrix m;
   m.matrix[0][3] = translateX;
   m.matrix[1][3] = translateY;
   m.matrix[2][3] = translateZ;
   preMultiply(m);
   setMatrixFileModified();
}

/*
 * Multiply by the matrix with the specified translation
 */
void
TransformationMatrix::translate(const float translateX,
                                const float translateY,
                                const float translateZ)
{
   translate(static_cast<double>(translateX),
             static_cast<double>(translateY), 
             static_cast<double>(translateZ));
   setMatrixFileModified();
}

/*
 * Multiply by the matrix with the specified translation
 */
void
TransformationMatrix::translate(const double translateXYZ[3])
{
   translate(translateXYZ[0], translateXYZ[1], translateXYZ[2]);
   setMatrixFileModified();
}

/*
 * Multiply by the matrix with the specified translation
 */
void
TransformationMatrix::translate(const float translateXYZ[3])
{
   translate(translateXYZ[0], translateXYZ[1], translateXYZ[2]);
   setMatrixFileModified();
}

/**
 * see if the matrix is the identity matrix.
 */
bool 
TransformationMatrix::isIdentity() const
{
   for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
         if (i == j) {
            if (matrix[i][j] != 1.0) {
               return false;
            }
         }
         else {
            if (matrix[i][j] != 0.0) {
               return false;
            }
         }
      }
   }
   
   return true;
}      

//--------------------------------------------------------------------------

/*
 * Constructors
 */
TransformationMatrixFile::TransformationMatrixFile()
   : AbstractFile("Transformation Matrix File",
                  SpecFile::getTransformationMatrixFileExtension())
{
   clear();
}

/*
 * destructor
 */
TransformationMatrixFile::~TransformationMatrixFile()
{
}

/*
 * Find out if file is emtpy
 */
bool
TransformationMatrixFile::empty() const 
{
   return (getNumberOfMatrices() == 0);
}

/*
 * add a transformation matrix
 */
void 
TransformationMatrixFile::addTransformationMatrix(
                                  const TransformationMatrix& tm)
{
   matrices.push_back(tm);
   TransformationMatrix* theMatrix = getTransformationMatrix(getNumberOfMatrices() - 1);
   theMatrix->matrixFile = this;
   setModified();
}

/*
 * append a transformation matrix file
 */
void 
TransformationMatrixFile::append(TransformationMatrixFile& tmf)
{
   for (int i = 0; i < tmf.getNumberOfMatrices(); i++) {
      TransformationMatrix* tm = tmf.getTransformationMatrix(i);
      addTransformationMatrix(*tm);
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(tmf);
}

/*
 * clear the matrix file
 */
void 
TransformationMatrixFile::clear()
{
   clearAbstractFile();
   
   matrices.clear();
   selectedAxesIndex = -1;
   
   //TransformationMatrix tm;
   //addTransformationMatrix(tm);
   
   clearModified();
}

/*
 * delete a matrix
 */
void 
TransformationMatrixFile::deleteMatrix(const int matrixNumber)
{
   for (int i = matrixNumber; i < ((int)matrices.size() - 1); i++) {
      matrices[i] = matrices[i + 1];
   }
   matrices.pop_back();
   setModified();
}
  
/*
 * get a transformation matrix for modifying
 */
TransformationMatrix* 
TransformationMatrixFile::getTransformationMatrix(const int i)
{
   if (i < static_cast<int>(matrices.size())) {
      return &matrices[i];
   }
   return NULL;
}

/*
 * get a transformation matrix for modifying
 */
const TransformationMatrix* 
TransformationMatrixFile::getTransformationMatrix(const int i) const
{
   if (i < static_cast<int>(matrices.size())) {
      return &matrices[i];
   }
   return NULL;
}

/// get a transformation matrix by int name for modifying
TransformationMatrix* 
TransformationMatrixFile::getTransformationMatrixWithName(const QString& name)
{
   for (int i = 0; i < getNumberOfMatrices(); i++) {
      TransformationMatrix* tm = getTransformationMatrix(i);
      if (tm->getMatrixName() == name) {
         return tm;
      }
   }
   return NULL;
}

/// get a transformation matrix by its name
const TransformationMatrix* 
TransformationMatrixFile::getTransformationMatrixWithName(const QString& name) const
{
   for (int i = 0; i < getNumberOfMatrices(); i++) {
      const TransformationMatrix* tm = getTransformationMatrix(i);
      if (tm->getMatrixName() == name) {
         return tm;
      }
   }
   return NULL;
}
      
/*
 * read matrix file version 2
 */
void 
TransformationMatrixFile::readFileVersion_2(QTextStream& stream) throw (FileException)
{
   int numMatrices = -1;
   bool readingTags = true;
   
   while (readingTags) {
      QString tag, value;
      readTagLine(stream, tag, value);
      
      if (tag == tagNumberOfMatrices) {
         numMatrices = value.toInt();
      }
      else if (tag == tagEndOfTags) {
         readingTags = false;
      }
   }
   
   if (numMatrices > 0) {
      matrices.clear();
   }
   
   for (int i = 0; i < numMatrices; i++) {
      TransformationMatrix tm;
      tm.readMatrix(stream, getFileName());
      addTransformationMatrix(tm);
   }
}

/*
 * read matrix file version 1
 */
void 
TransformationMatrixFile::readFileVersion_1(QTextStream& stream) throw (FileException)
{
   TransformationMatrix tm;
   tm.readMatrix(stream, getFileName());
   addTransformationMatrix(tm);
}

/*
 * Read the matrix file's contents 
 */
void
TransformationMatrixFile::readFileData(QFile& /*file*/, QTextStream& stream, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   // get starting position of data
   //const QIODevice::Offset startOfMatrixData = file.at();
   
   int version = 0;
   
   QString line, tag, value;
   readTagLine(stream, line, tag, value);
   
   if (tag == tagMatrixFileVersion) {
      switch(value.toInt()) {
         case 1:
            version = 1;
            break;
         case 2:
            version = 2;
            break;
         default:
            {
               QString msg("Unknown version of matrix file ");
               msg.append(value);
               throw FileException(getFileName(), msg);
            }
      }
   }
   else {
      version = 0;
   }
   
   switch(version) {
      case 2:
         readFileVersion_2(stream);
         break;
      case 1:
         readFileVersion_1(stream);
         break;
      default:
         {
            // set file position as data will now be read
            //file.at(startOfMatrixData);
            TransformationMatrix tm;
            tm.readMatrixData(stream, line, getFileName());
            addTransformationMatrix(tm);
         }
         break;
   }
}

/*
 * write matrix file
 */
void 
TransformationMatrixFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   stream << tagMatrixFileVersion << " 2" << "\n";
   
   stream << tagNumberOfMatrices << " " << getNumberOfMatrices() << "\n";
   
   stream << tagEndOfTags << "\n";
   
   for (int i = 0; i < getNumberOfMatrices(); i++) {
      matrices[i].writeMatrix(stream);
   }
}

/**
 * see if this matrix is valid.
 */
int 
TransformationMatrixFile::getMatrixIndex(const TransformationMatrix* tm) const
{
   if (tm == NULL) {
      return -1;
   }

   for (int i = 0; i < getNumberOfMatrices(); i++) {
      if (getTransformationMatrix(i) == tm) {
         return i;
      }
   }
   return -1;
}
      
/**
 * see if matrix is still valid (could have been deleted).
 */
bool 
TransformationMatrixFile::getMatrixValid(const TransformationMatrix* tm) const
{
   for (int i = 0; i < getNumberOfMatrices(); i++) {
      if (getTransformationMatrix(i) == tm) {
         return true;
      }
   }
   return false;
}
      
/**
 * read the spec file from a scene.
 */
void 
TransformationMatrixFile::showScene(const SceneFile::Scene& scene, QString& errorMessage)
{
   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "TransformationMatrixFile") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName   = si->getName();
            const QString matrixName = si->getModelName();
            
            TransformationMatrix* tm = getTransformationMatrixWithName(matrixName);
            if (tm != NULL) {            
               if (infoName == "showAxes") {
                  si->getValue(tm->showAxes);
               }
               else if (infoName == "axesLength") {
                  si->getValue(tm->axesLength);
               }
            }
            else { 
               errorMessage.append("Unable to find matrix with name ");
               errorMessage.append(matrixName);
               errorMessage.append("\n");
            }
         }
      }
   }
}

/**
 * write the spec file to a scene.
 */
void 
TransformationMatrixFile::saveScene(SceneFile::Scene& scene, const bool /*selectedFilesOnlyFlag*/)
{
   const int num = getNumberOfMatrices();
   if (num <= 0) {
      return;
   }
   
   SceneFile::SceneClass sc("TransformationMatrixFile");
   for (int i = 0; i < num; i++) {
      const TransformationMatrix* tm = getTransformationMatrix(i);
      sc.addSceneInfo(SceneFile::SceneInfo("showAxes", tm->getMatrixName(), tm->showAxes));
      sc.addSceneInfo(SceneFile::SceneInfo("axesLength", tm->getMatrixName(), tm->axesLength));
      
   }
   if (sc.getNumberOfSceneInfo() > 0) {
      scene.addSceneClass(sc);
   }
}


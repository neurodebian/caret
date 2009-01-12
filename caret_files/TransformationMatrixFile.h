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


#ifndef __TRANSFORMATION_MATRIX_FILE_H__
#define __TRANSFORMATION_MATRIX_FILE_H__

#include "AbstractFile.h"
#include "SceneFile.h"

class TransformationMatrixFile;
class vtkMatrix4x4;
class vtkTransform;

/// This class contains a transformation matrix.  This matrix is Post Multiply
/// to be compatible with OpenGL.
class TransformationMatrix {
   public:
      /// Rotation axes
      enum ROTATE_AXIS {
         ROTATE_X_AXIS,
         ROTATE_Y_AXIS,
         ROTATE_Z_AXIS
      };
      
   private:
      /// set the matrix file that contains this matrix as modified
      void setMatrixFileModified();
      
      /// the transformation matrix
      double matrix[4][4];
   
      /// anterior commissure coordinates
      int acCoords[3];
      
      /// dimension of the volume
      int volumeDimensions[3];
      
      /// name of this matrix
      QString name;
      
      /// comment associated with this matrix
      QString comment;
      
      /// fiducial coordinate file associated with this matrix
      QString targetFiducialCoordFileName;
      
      /// volume associated with this matrix
      QString targetVolumeFileName;
      
      /// transformation matrix file this matrix is associated with
      TransformationMatrixFile* matrixFile;
      
      /// show the transform's axes
      bool showAxes;
      
      /// axes length
      float axesLength;
      
      static const QString tagMatrixName;
      static const QString tagMatrixComment;
      static const QString tagMatrixTargetVolumeFileName;
      static const QString tagMatrixTargetVolumeDimensions;
      static const QString tagMatrixTargetACCoords;
      static const QString tagMatrixFiducialCoordFileName;
      static const QString tagMatrixBegin;

      /// matrix number counter
      static int matrixNumberCounter;
      
      /// copy data to this from another TransformationMatrix 
      void copyData(const TransformationMatrix& tm);
      
   public:
      /// constructor
      TransformationMatrix();
      
      /// copy constructor
      TransformationMatrix(const TransformationMatrix& tm);
      
      /// destructor
      ~TransformationMatrix();
      
      /// assignment operator
      TransformationMatrix& operator=(const TransformationMatrix& tm);
      
      /// clear the matrix
      void clear();
      
      /// get the matrix number counter
      static int getMatrixNumberCounter() { return matrixNumberCounter; }

      /// get show the transform matrix's axes
      bool getShowAxes() const { return showAxes; }
      
      /// set show the transform matrix's axes
      void setShowAxes(const bool sa) { showAxes = sa; }
      
      /// get the axes length
      float getAxesLength() const { return axesLength; }
      
      /// set the axes length
      void setAxesLength(const float len) { axesLength = len; }
      
      /// get the translation from the matrix
      void getTranslation(double& tx, double& ty, double& tz) const;
      
      /// get the translation from the matrix
      void getTranslation(float& tx, float& ty, float& tz) const;
      
      /// sets (overrides) the translation in the matrix
      void setTranslation(const double tx, const double ty, const double tz);
      
      /// sets (overrides) the translation in the matrix
      void setTranslation(const float tx, const float ty, const float tz);
      
      /// get the rotation angles from the matrix
      void getRotationAngles(double& rx, double& ry, double& rz) const;
      
      /// get the rotation angles from the matrix
      void getRotationAngles(float& rx, float& ry, float& rz) const;
      
      /// get the scaling from the matrix (incorrect in some circumstances)
      void getScaling(double& sx, double& sy, double& sz) const;
      
      /// get the scaling from the matrix (incorrect in some circumstances)
      void getScaling(float& sx, float& sy, float& sz) const;
      
      /// get the matrix as a VTK 4x4 matrix
      void getMatrix(vtkMatrix4x4* m) const;
      
      /// set the matrix from a VTK 4x4 matrix
      void setMatrix(const vtkMatrix4x4* m);
      
      /// get the matrix as a VTK transform matrix
      void getMatrix(vtkTransform* m) const;
      
      /// set the matrix from a VTK transform matrix
      void setMatrix(const vtkTransform* m);
      
      /// set the matrix 
      void setMatrix(const double translate[3],
                     const double rotate[3],
                     const double scale[3]);
                     
      /// get the entire matrix
      void getMatrix(double m[4][4]) const;
      
      /// get the entire matrix
      void getMatrix(float m[4][4]) const;
      
      /// get the entire matrix
      void getMatrix(float m[16]) const;
      
      /// get the entire matrix
      void getMatrix(double m[16]) const;
      
      /// get the AC coords
      void getMatrixTargetACCoords(int& x, int& y, int& z) const;
      
      /// get an element from the matrix
      double getMatrixElement(const int row, 
                              const int column) const;
      
      /// get the comment
      QString getMatrixComment() const { return comment; }
      
      /// get the matrix name
      QString getMatrixName() const { return name; }
      
      /// get the target volume file name
      QString getMatrixTargetVolumeFileName() const 
                             { return targetVolumeFileName; }
      
      /// get the volume dimeensions
      void getMatrixTargetVolumeDimensions(int& x, int& y, int& z) const;
      
      /// get the target fiducial file name
      QString getMatrixFiducialCoordFileName() const 
                             { return targetFiducialCoordFileName; }
      
      // read the matrix name, comment, and 4x4 matrix
      void readMatrix(QTextStream& stream, const QString& filename) throw (FileException);
      
      /// read the 4x4 matrix
      void readMatrixData(QTextStream& stream, const QString& firstLine,
                          const QString& filename) throw (FileException);
      
      /// set the entire matrix
      void setMatrix(const double m[4][4]);
      
      /// set the entire matrix
      void setMatrix(const float m[4][4]);
      
      /// set the entire matrix
      void setMatrix(const double m[16]);
      
      /// set the entire matrix
      void setMatrix(const float m[16]);
      
      /// set the AC coords
      void setMatrixTargetACCoords(const int x, const int y, const int z);
      
      /// set an element of the matrix
      void setMatrixElement(const int row, 
                            const int column,
                            const double value);
                     
      /// set the comment
      void setMatrixComment(const QString& s) { comment = s; }
      
      /// set the name
      void setMatrixName(const QString& s) { name = s; }
             
      /// set the name of the target volume
      void setMatrixTargetVolumeFileName(const QString& s) 
                                        { targetVolumeFileName = s; }
      
      /// set the name of the target volume
      void setMatrixFiducialCoordFileName(const QString& s) 
                                        { targetFiducialCoordFileName = s; }
      
      /// set the volume dimeensions
      void setMatrixTargetVolumeDimensions(const int x, const int y, const int z);
      
      /// write the matrix
      void writeMatrix(QTextStream& stream);
      
      /// see if the matrix is the identity matrix
      bool isIdentity() const;
      
      //--------------------------------------------------------
      // Operations on the matrix
      //--------------------------------------------------------
      
      /// set the matrix to the identity matrix
      void identity();
      
      /// set the matrix to the inverse of itself
      void inverse();
      
//      // Multiply by the matrix in the specified file
//      int multiply(const string& matrixFilename);
      
      /// Multiply by the TransformationMatrix
      void multiply(const TransformationMatrix& tmf);
      
      /// Apply the transformation matrix to a point
      void multiplyPoint(double p[3]) const;

      /// Apply the transformation matrix to a point
      void multiplyPoint(float p[3]) const;

      /// Apply the transformation matrix to a point
      void inverseMultiplyPoint(float p[3]) const;

      /// Apply the inverse of the transformation matrix to a point
      void inverseMultiplyPoint(double p[3]) const;

      /// Multiply by the matrix containing the specified rotation
      void rotate(const TransformationMatrix::ROTATE_AXIS rotationAxis,
                  const double rotateDegrees);
      
      /// Multiply by the matrix containing the specified rotation
      /// about the specified axis
      void rotate(const double rotateDegrees, const double axis[3]);

      /// rotate about the X axis
      void rotateX(const double rotateDegrees);
      
      /// rotate about the Y axis
      void rotateY(const double rotateDegrees);
      
      /// rotate about the Z axis
      void rotateZ(const double rotateDegrees);
      
      /// rotate using the three angles relative to "relativeMatrix"
      void rotate(const double angles[3], 
                  vtkTransform* relativeMatrix);
                  
      /// Multiply by the matrix with the specified scaling
      void scale(const double scaleX,
                 const double scaleY,
                 const double scaleZ);
                 
      /// Multiply by the matrix with the specified scaling
      void scale(const double scaleXYZ[3]);
      
      /// Multiply by the matrix with the specified scaling
      void scale(const float scaleX,
                 const float scaleY,
                 const float scaleZ);
                 
      /// Multiply by the matrix with the specified scaling
      void scale(const float scaleXYZ[3]);
      
      /// Multiply by the matrix with the specified translation relative to "relativeMatrix"
      void translate(const double translateXYZ[3], vtkTransform* relativeMatrix);
      
      /// Multiply by the matrix with the specified translation
      void translate(const double translateX,
                     const double translateY,
                     const double translateZ);
                     
      /// Multiply by the matrix with the specified translation
      void translate(const float translateX,
                     const float translateY,
                     const float translateZ);
                     
      /// Multiply by the matrix with the specified translation
      void translate(const double translateXYZ[3]);
      
      /// Multiply by the matrix with the specified translation
      void translate(const float translateXYZ[3]);
      
      /// set the matrix to the transpose of itself
      void transpose();
      
   friend class TransformationMatrixFile;
};

#ifdef _TRANSFORMATION_FILE_MAIN_
    const QString TransformationMatrix::tagMatrixName = 
                                     "tag-matrix-name";
    const QString TransformationMatrix::tagMatrixComment = 
                                     "tag-matrix-comment";
    const QString TransformationMatrix::tagMatrixTargetVolumeFileName = 
                                     "tag-matrix-target-volume-name";
    const QString TransformationMatrix::tagMatrixTargetVolumeDimensions = 
                                     "tag-matrix-target-volume-dimensions";
    const QString TransformationMatrix::tagMatrixTargetACCoords = 
                                     "tag-matrix-target-AC-coords";
    const QString TransformationMatrix::tagMatrixFiducialCoordFileName = 
                                     "tag-matrix-fiducial-coord-name";
    const QString TransformationMatrix::tagMatrixBegin = 
                                     "tag-matrix-begin";
    int TransformationMatrix::matrixNumberCounter = 1;
#endif // _TRANSFORMATION_FILE_MAIN_

/// File that contains one or more TransoformationMatrix
class TransformationMatrixFile : public AbstractFile {
   private:
      /// The transformation matrices
      std::vector<TransformationMatrix> matrices;
      
      /// index of selected tranformation matrix axes
      int selectedAxesIndex;
      
      static const QString tagMatrixFileVersion;
      static const QString tagNumberOfMatrices;
      static const QString tagEndOfTags;
      
      /// read the transformation matrix file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// read a version 1 file
      void readFileVersion_1(QTextStream& stream) throw (FileException);
      
      /// read a version 2 file
      void readFileVersion_2(QTextStream& stream) throw (FileException);
      
      /// write the transformation matrix file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
   public:
      /// constructor
      TransformationMatrixFile();
      
      /// destructor
      ~TransformationMatrixFile();
      
      /// Append a transformation matrix file to this one
      void append(TransformationMatrixFile& tmf);
      
      /// clear the matrix file
      void clear();

      /// add a transformation matrix
      void addTransformationMatrix(const TransformationMatrix& tm);
      
      /// delete a matrix
      void deleteMatrix(const int matrixNumber);
      
      /// find out if file is isEmpty
      bool empty() const;       

      /// get a transformation matrix for modifying
      TransformationMatrix* getTransformationMatrix(const int i);
      
      /// get a transformation matrix 
      const TransformationMatrix* getTransformationMatrix(const int i) const;
      
      /// get a transformation matrix by int name for modifying
      TransformationMatrix* getTransformationMatrixWithName(const QString& name);
      
      /// get a transformation matrix by its name
      const TransformationMatrix* getTransformationMatrixWithName(const QString& name) const;
      
      /// get the number of matrices
      int getNumberOfMatrices() const { return (int)matrices.size(); }
      
      /// get the selected transformation axes
      int getSelectedTransformationAxesIndex() const { return selectedAxesIndex; }
      
      /// set the selected transformation axes
      void setSelectedTransformationAxesIndex(const int indx) { selectedAxesIndex = indx; }
      
      /// get the index of a matrix (-1 if matrix invalid)
      int getMatrixIndex(const TransformationMatrix* tm) const;
      
      /// see if matrix is still valid (could have been deleted)
      bool getMatrixValid(const TransformationMatrix* tm) const;
      
      /// read the spec file from a scene
      void showScene(const SceneFile::Scene& scene, QString& errorMessage);
      
      /// write the spec file to a scene
      void saveScene(SceneFile::Scene& scene, const bool selectedFilesOnlyFlag);
      
   friend class TransformationMatrix;
};

#endif  //  __TRANSFORMATION_MATRIX_FILE_H__

#ifdef _TRANSFORMATION_FILE_MAIN_
    const QString TransformationMatrixFile::tagMatrixFileVersion = 
                                     "tag-transformation-matrix-file-version";
    const QString TransformationMatrixFile::tagNumberOfMatrices =
                                     "tag-number-of-matrices";
    const QString TransformationMatrixFile::tagEndOfTags =
                                     "tag-end-of-tags";
#endif  //  _TRANSFORMATION_FILE_MAIN_

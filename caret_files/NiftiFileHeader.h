
#ifndef  __NIFTI_FILE_HEADER_H__
#define	__NIFTI_FILE_HEADER_H__

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
/*****===================================================================*****/
/*****     Sample functions to deal with NIFTI-1 and ANALYZE files       *****/
/*****...................................................................*****/
/*****            This code is released to the public domain.            *****/
/*****...................................................................*****/
/*****  Author: Robert W Cox, SSCC/DIRP/NIMH/NIH/DHHS/USA/EARTH          *****/
/*****  Date:   August 2003                                              *****/
/*****...................................................................*****/
/*****  Neither the National Institutes of Health (NIH), nor any of its  *****/
/*****  employees imply any warranty of usefulness of this software for  *****/
/*****  any purpose, and do not assume any liability for damages,        *****/
/*****  incidental or otherwise, caused by any use of this document.     *****/
/*****===================================================================*****/

#include "TransformationMatrixFile.h"
#include "nifti1.h"
#include "VolumeFile.h"
#include "zlib.h"

/// class for storing a NIFTI header and its information.
class NiftiFileHeader {
    public:
       enum {
          NIFTI_L2R = 1,    /* Left to Right         */
          NIFTI_R2L = 2,    /* Right to Left         */
          NIFTI_P2A = 3,    /* Posterior to Anterior */
          NIFTI_A2P = 4,    /* Anterior to Posterior */
          NIFTI_I2S = 5,    /* Inferior to Superior  */
          NIFTI_S2I = 6     /* Superior to Inferior  */
       };

       /// type of stereotaxic coordinate
       enum STEREOTAXIC_TYPE {
          /// no type (qform == sform == 0)
          STEREOTAXIC_TYPE_NONE,
          /// use qform
          STEREOTAXIC_TYPE_QFORM,
          /// use sform
          STEREOTAXIC_TYPE_SFORM
       };

       // constructor
       NiftiFileHeader();

       // constructor
       NiftiFileHeader(const nifti_1_header niftiHeaderStructIn);

       // destructor
       ~NiftiFileHeader();

       // read the header
       void readHeader(const QString& filename) throw (FileException);

       // read the header from a compressed file
       void readHeader(gzFile dataFile,
                       const QString& filenameForErrorMessage) throw (FileException);

       // get the coordinate of a voxel (returns true if coord valid)
       bool getVoxelCoordinate(const int ijk[3],
                               const STEREOTAXIC_TYPE stereotaxicType,
                               float xyzOut[3]) const;

       // get a copy of the NIFTI header struct
       nifti_1_header getNiftiHeaderStruct() const;

       // set the NIFTI header struct
       void setNiftiHeaderStruct(const nifti_1_header niftiHeaderStructIn);

       // get the data type as a string
       QString getDataTypeAsString() const;

       // get a full description of the header's contents
       QString getDescriptionOfHeader() const;

       // get the sform transformation matrix
       TransformationMatrix getSFormTransformationMatrix() const;

       // get the qform transformation matrix
       TransformationMatrix getQFormTransformationMatrix() const;

       // get the sform orientation
       void getSFormOrientation(VolumeFile::ORIENTATION orientationOut[3]) const;

       // get the qform orientation
       void getQFormOrientation(VolumeFile::ORIENTATION orientationOut[3]) const;

       // get the sform orientation as a string
       QString getSFormOrientationAsString() const;

       // get the qform orientation as a string
       QString getQFormOrientationAsString() const;

       // get the qform code as a string
       QString getQFormCodeAsString() const;

       // get the sform code as a string
       QString getSFormCodeAsString() const;

       // get the spacing units as a string
       QString getSpacingUnitsAsString() const;

       // get the time units as a string
       QString getTimeUnitsAsString() const;

       // get the NIFTI intention (such as statistical parameters) information.
       void getNiftiIntentionInformation(QString& niftiIntentCodeAndParam,
                                         QString& niftiIntentName) const;

       // analyze HDR file is actually a NIFTI HDR file.
       static bool hdrIsNiftiFile(const QString& hdrFileName);

       // See if the data needs byte swapping
       bool doesDataNeedByteSwapping() const { return dataNeedsByteSwapping; }

       // set the read extension flag
       void setReadExtensionFlag(const bool flag) { readExtensionFlag = flag; }

   protected:
      typedef struct {                   /** 4x4 matrix struct **/
        float m[4][4] ;
      } mat44 ;
      typedef struct {                   /** 3x3 matrix struct **/
        float m[3][3] ;
      } mat33 ;

      // read NIFTI extension from the header
      void readHeaderExtension(gzFile dataFile,
                              nifti_1_header& hdr,
                              const bool byteSwapFlag) throw (FileException);

      // Methods below are caret unique
      static void mat44ToCaretOrientation(const TransformationMatrix& tm,
                                          VolumeFile::ORIENTATION& xOrient,
                                          VolumeFile::ORIENTATION& yOrient,
                                          VolumeFile::ORIENTATION& zOrient);

      // convert NIFTI orientation to Caret orientation
      static VolumeFile::ORIENTATION niftiOrientationToCaretOrientation(
                                             const int niftiOrientation);

      // get the qform or sform code as a string.
      QString getQOrSFormCodeAsString(const int qsForm) const;

      // Convert quaternions to a transformation matrix.
      TransformationMatrix nifti_quatern_to_mat44( float qb, float qc, float qd,
                                     float qx, float qy, float qz,
                                     float dx, float dy, float dz, float qfac );

      // get orientation from a 4x4 transform
      static void nifti_mat44_to_orientation( mat44 R , int *icod, int *jcod, int *kcod );

      // 3x3 determinant
      static float nifti_mat33_determ( mat33 R ) ;

      // 3x3 multiplication
      static mat33 nifti_mat33_mul( mat33 A , mat33 B ) ;

      // transformation matrix used by SFORM
      TransformationMatrix sFormTransformationMatrix;

      // transformation matrix used by QFORM
      TransformationMatrix qFormTransformationMatrix;

      // the NIFTI header struct
      nifti_1_header niftiHeaderStruct;

      // qform's qfac
      float qFormQFac;

      bool dataNeedsByteSwapping;

      QString extensionInformation;

      bool readExtensionFlag;
};

#endif	/* __NIFTI_HEADER_H__ */


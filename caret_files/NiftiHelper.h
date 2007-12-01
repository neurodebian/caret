#ifndef __NIFTI_HELPER_H__
#define __NIFTI_HELPER_H__

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

#include "VolumeFile.h"

/// class for help with NIFTI files.  Code stolen from nifti1_io.c/h that is part of nifticlib
/// available from http://sourceforge.net/projects/niftilib
class NiftiHelper {
   public:
      enum {
         NIFTI_L2R = 1,    /* Left to Right         */
         NIFTI_R2L = 2,    /* Right to Left         */
         NIFTI_P2A = 3,    /* Posterior to Anterior */
         NIFTI_A2P = 4,    /* Anterior to Posterior */
         NIFTI_I2S = 5,    /* Inferior to Superior  */
         NIFTI_S2I = 6     /* Superior to Inferior  */
      };

      typedef struct {                   /** 4x4 matrix struct **/
        float m[4][4] ;
      } mat44 ;
      typedef struct {                   /** 3x3 matrix struct **/
        float m[3][3] ;
      } mat33 ;
      
      static void nifti_mat44_to_orientation( mat44 R , int *icod, int *jcod, int *kcod ) ;

      static float nifti_mat33_determ( mat33 R ) ;


      static mat33 nifti_mat33_mul( mat33 A , mat33 B ) ;

      static mat44 nifti_quatern_to_mat44( float qb, float qc, float qd,
                                           float qx, float qy, float qz,
                                           float dx, float dy, float dz, float qfac );
                                     
      //-------------------------------------------------------
      // Methods below are caret unique
      static void mat44ToCaretOrientation(mat44 R, 
                                          VolumeFile::ORIENTATION& xOrient,   
                                          VolumeFile::ORIENTATION& yOrient,   
                                          VolumeFile::ORIENTATION& zOrient);

      // convert NIFTI orientation to Caret orientation
      static VolumeFile::ORIENTATION niftiOrientationToCaretOrientation(const int niftiOrientation);  
      
      // get the NIFTI intention (such as statistical parameters) information
      static void getNiftiIntentionInformation(const nifti_1_header& hdr,
                                               QString& niftiIntentCodeAndParam,
                                               QString& niftiIntentName);
      
      // analyze HDR file is actually a NIFTI HDR file
      static bool hdrIsNiftiFile(const QString& hdrFileName);
};

#endif // __NIFTI_HELPER_H__

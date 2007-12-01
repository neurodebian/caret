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

#include <cstdio>  // used for NULL
#include <cmath>

#include "NiftiHelper.h"
#include "nifti1.h"

/**
 * Methods below are caret unique.
 */
void 
NiftiHelper::mat44ToCaretOrientation(mat44 R, 
                                     VolumeFile::ORIENTATION& xOrient,   
                                     VolumeFile::ORIENTATION& yOrient,   
                                     VolumeFile::ORIENTATION& zOrient)
{
   int x, y, z;
   nifti_mat44_to_orientation(R, &x, &y, &z);
   
   xOrient = niftiOrientationToCaretOrientation(x);
   yOrient = niftiOrientationToCaretOrientation(y);
   zOrient = niftiOrientationToCaretOrientation(z);
}

/**
 * convert NIFTI orientation to Caret orientation.
 */
VolumeFile::ORIENTATION 
NiftiHelper::niftiOrientationToCaretOrientation(const int niftiOrientation)
{
  VolumeFile::ORIENTATION orient = VolumeFile::ORIENTATION_UNKNOWN;
  
  switch(niftiOrientation) {
     case NIFTI_L2R:
        orient = VolumeFile::ORIENTATION_LEFT_TO_RIGHT;
        break;
     case NIFTI_R2L:
        orient = VolumeFile::ORIENTATION_RIGHT_TO_LEFT;
        break;
     case NIFTI_P2A:
        orient = VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR;
        break;
     case NIFTI_A2P:
        orient = VolumeFile::ORIENTATION_ANTERIOR_TO_POSTERIOR;
        break;
     case NIFTI_I2S:
        orient = VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR;
        break;
     case NIFTI_S2I:
        orient = VolumeFile::ORIENTATION_SUPERIOR_TO_INFERIOR;
        break;
  }
  
  return orient;
}

void 
NiftiHelper::nifti_mat44_to_orientation( mat44 R , int *icod, int *jcod, int *kcod )
{
   float xi,xj,xk , yi,yj,yk , zi,zj,zk , val,detQ,detP ;
   mat33 P , Q , M ;
   int i,j,k=0,p,q,r , ibest,jbest,kbest,pbest,qbest,rbest ;
   float vbest ;

   if( icod == NULL || jcod == NULL || kcod == NULL ) return ; /* bad */

   *icod = *jcod = *kcod = 0 ; /* error returns, if sh*t happens */

   /* load column vectors for each (i,j,k) direction from matrix */

   /*-- i axis --*/ /*-- j axis --*/ /*-- k axis --*/

   xi = R.m[0][0] ; xj = R.m[0][1] ; xk = R.m[0][2] ;
   yi = R.m[1][0] ; yj = R.m[1][1] ; yk = R.m[1][2] ;
   zi = R.m[2][0] ; zj = R.m[2][1] ; zk = R.m[2][2] ;

   /* normalize column vectors to get unit vectors along each ijk-axis */

   /* normalize i axis */

   val = std::sqrt( xi*xi + yi*yi + zi*zi ) ;
   if( val == 0.0 ) return ;                 /* stupid input */
   xi /= val ; yi /= val ; zi /= val ;

   /* normalize j axis */

   val = std::sqrt( xj*xj + yj*yj + zj*zj ) ;
   if( val == 0.0 ) return ;                 /* stupid input */
   xj /= val ; yj /= val ; zj /= val ;

   /* orthogonalize j axis to i axis, if needed */

   val = xi*xj + yi*yj + zi*zj ;    /* dot product between i and j */
   if( std::fabs(val) > 1.e-4 ){
     xj -= val*xi ; yj -= val*yi ; zj -= val*zi ;
     val = std::sqrt( xj*xj + yj*yj + zj*zj ) ;  /* must renormalize */
     if( val == 0.0 ) return ;              /* j was parallel to i? */
     xj /= val ; yj /= val ; zj /= val ;
   }

   /* normalize k axis; if it is zero, make it the cross product i x j */

   val = std::sqrt( xk*xk + yk*yk + zk*zk ) ;
   if( val == 0.0 ){ xk = yi*zj-zi*yj; yk = zi*xj-zj*xi ; zk=xi*yj-yi*xj ; }
   else            { xk /= val ; yk /= val ; zk /= val ; }

   /* orthogonalize k to i */

   val = xi*xk + yi*yk + zi*zk ;    /* dot product between i and k */
   if( std::fabs(val) > 1.e-4 ){
     xk -= val*xi ; yk -= val*yi ; zk -= val*zi ;
     val = std::sqrt( xk*xk + yk*yk + zk*zk ) ;
     if( val == 0.0 ) return ;      /* bad */
     xk /= val ; yk /= val ; zk /= val ;
   }

   /* orthogonalize k to j */

   val = xj*xk + yj*yk + zj*zk ;    /* dot product between j and k */
   if( std::fabs(val) > 1.e-4 ){
     xk -= val*xj ; yk -= val*yj ; zk -= val*zj ;
     val = std::sqrt( xk*xk + yk*yk + zk*zk ) ;
     if( val == 0.0 ) return ;      /* bad */
     xk /= val ; yk /= val ; zk /= val ;
   }

   Q.m[0][0] = xi ; Q.m[0][1] = xj ; Q.m[0][2] = xk ;
   Q.m[1][0] = yi ; Q.m[1][1] = yj ; Q.m[1][2] = yk ;
   Q.m[2][0] = zi ; Q.m[2][1] = zj ; Q.m[2][2] = zk ;

   /* at this point, Q is the rotation matrix from the (i,j,k) to (x,y,z) axes */

   detQ = nifti_mat33_determ( Q ) ;
   if( detQ == 0.0 ) return ; /* shouldn't happen unless user is a DUFIS */

   /* Build and test all possible +1/-1 coordinate permutation matrices P;
      then find the P such that the rotation matrix M=PQ is closest to the
      identity, in the sense of M having the smallest total rotation angle. */

   /* Despite the formidable looking 6 nested loops, there are
      only 3*3*3*2*2*2 = 216 passes, which will run very quickly. */

   vbest = -666.0 ; ibest=pbest=qbest=rbest=1 ; jbest=2 ; kbest=3 ;
   for( i=1 ; i <= 3 ; i++ ){     /* i = column number to use for row #1 */
    for( j=1 ; j <= 3 ; j++ ){    /* j = column number to use for row #2 */
     if( i == j ) continue ;
      for( k=1 ; k <= 3 ; k++ ){  /* k = column number to use for row #3 */
       if( i == k || j == k ) continue ;
       P.m[0][0] = P.m[0][1] = P.m[0][2] =
        P.m[1][0] = P.m[1][1] = P.m[1][2] =
         P.m[2][0] = P.m[2][1] = P.m[2][2] = 0.0 ;
       for( p=-1 ; p <= 1 ; p+=2 ){    /* p,q,r are -1 or +1      */
        for( q=-1 ; q <= 1 ; q+=2 ){   /* and go into rows #1,2,3 */
         for( r=-1 ; r <= 1 ; r+=2 ){
           P.m[0][i-1] = p ; P.m[1][j-1] = q ; P.m[2][k-1] = r ;
           detP = nifti_mat33_determ(P) ;           /* sign of permutation */
           if( detP * detQ <= 0.0 ) continue ;  /* doesn't match sign of Q */
           M = nifti_mat33_mul(P,Q) ;

           /* angle of M rotation = 2.0*acos(0.5*sqrt(1.0+trace(M)))       */
           /* we want largest trace(M) == smallest angle == M nearest to I */

           val = M.m[0][0] + M.m[1][1] + M.m[2][2] ; /* trace */
           if( val > vbest ){
             vbest = val ;
             ibest = i ; jbest = j ; kbest = k ;
             pbest = p ; qbest = q ; rbest = r ;
           }
   }}}}}}

   /* At this point ibest is 1 or 2 or 3; pbest is -1 or +1; etc.

      The matrix P that corresponds is the best permutation approximation
      to Q-inverse; that is, P (approximately) takes (x,y,z) coordinates
      to the (i,j,k) axes.

      For example, the first row of P (which contains pbest in column ibest)
      determines the way the i axis points relative to the anatomical
      (x,y,z) axes.  If ibest is 2, then the i axis is along the y axis,
      which is direction P2A (if pbest > 0) or A2P (if pbest < 0).

      So, using ibest and pbest, we can assign the output code for
      the i axis.  Mutatis mutandis for the j and k axes, of course. */

   switch( ibest*pbest ){
     case  1: i = NIFTI_L2R ; break ;
     case -1: i = NIFTI_R2L ; break ;
     case  2: i = NIFTI_P2A ; break ;
     case -2: i = NIFTI_A2P ; break ;
     case  3: i = NIFTI_I2S ; break ;
     case -3: i = NIFTI_S2I ; break ;
   }

   switch( jbest*qbest ){
     case  1: j = NIFTI_L2R ; break ;
     case -1: j = NIFTI_R2L ; break ;
     case  2: j = NIFTI_P2A ; break ;
     case -2: j = NIFTI_A2P ; break ;
     case  3: j = NIFTI_I2S ; break ;
     case -3: j = NIFTI_S2I ; break ;
   }

   switch( kbest*rbest ){
     case  1: k = NIFTI_L2R ; break ;
     case -1: k = NIFTI_R2L ; break ;
     case  2: k = NIFTI_P2A ; break ;
     case -2: k = NIFTI_A2P ; break ;
     case  3: k = NIFTI_I2S ; break ;
     case -3: k = NIFTI_S2I ; break ;
   }

   *icod = i ; *jcod = j ; *kcod = k ; return ;
}

/*----------------------------------------------------------------------*/
/*! compute the determinant of a 3x3 matrix
*//*--------------------------------------------------------------------*/
float 
NiftiHelper::nifti_mat33_determ( mat33 R )   /* determinant of 3x3 matrix */
{
   double r11,r12,r13,r21,r22,r23,r31,r32,r33 ;
                                                       /*  INPUT MATRIX:  */
   r11 = R.m[0][0]; r12 = R.m[0][1]; r13 = R.m[0][2];  /* [ r11 r12 r13 ] */
   r21 = R.m[1][0]; r22 = R.m[1][1]; r23 = R.m[1][2];  /* [ r21 r22 r23 ] */
   r31 = R.m[2][0]; r32 = R.m[2][1]; r33 = R.m[2][2];  /* [ r31 r32 r33 ] */

   return r11*r22*r33-r11*r32*r23-r21*r12*r33
         +r21*r32*r13+r31*r12*r23-r31*r22*r13 ;
}

/*----------------------------------------------------------------------*/
/*! multiply 2 3x3 matrices
*//*--------------------------------------------------------------------*/
NiftiHelper::mat33 
NiftiHelper::nifti_mat33_mul( mat33 A , mat33 B )  /* multiply 2 3x3 matrices */
{
   mat33 C ; int i,j ;
   for( i=0 ; i < 3 ; i++ )
    for( j=0 ; j < 3 ; j++ )
      C.m[i][j] =  A.m[i][0] * B.m[0][j]
                 + A.m[i][1] * B.m[1][j]
                 + A.m[i][2] * B.m[2][j] ;
   return C ;
}

NiftiHelper::mat44 
NiftiHelper::nifti_quatern_to_mat44( float qb, float qc, float qd,
                                     float qx, float qy, float qz,
                                     float dx, float dy, float dz, float qfac )
{
   mat44 R ;
   double a,b=qb,c=qc,d=qd , xd,yd,zd ;

   /* last row is always [ 0 0 0 1 ] */

   R.m[3][0]=R.m[3][1]=R.m[3][2] = 0.0 ; R.m[3][3]= 1.0 ;

   /* compute a parameter from b,c,d */

   a = 1.0l - (b*b + c*c + d*d) ;
   if( a < 1.e-7l ){                   /* special case */
     a = 1.0l / sqrt(b*b+c*c+d*d) ;
     b *= a ; c *= a ; d *= a ;        /* normalize (b,c,d) vector */
     a = 0.0l ;                        /* a = 0 ==> 180 degree rotation */
   } else{
     a = std::sqrt(a) ;                     /* angle = 2*arccos(a) */
   }

   /* load rotation matrix, including scaling factors for voxel sizes */

   xd = (dx > 0.0) ? dx : 1.0l ;       /* make sure are positive */
   yd = (dy > 0.0) ? dy : 1.0l ;
   zd = (dz > 0.0) ? dz : 1.0l ;

   if( qfac < 0.0 ) zd = -zd ;         /* left handedness? */

   R.m[0][0] =        (a*a+b*b-c*c-d*d) * xd ;
   R.m[0][1] = 2.0l * (b*c-a*d        ) * yd ;
   R.m[0][2] = 2.0l * (b*d+a*c        ) * zd ;
   R.m[1][0] = 2.0l * (b*c+a*d        ) * xd ;
   R.m[1][1] =        (a*a+c*c-b*b-d*d) * yd ;
   R.m[1][2] = 2.0l * (c*d-a*b        ) * zd ;
   R.m[2][0] = 2.0l * (b*d-a*c        ) * xd ;
   R.m[2][1] = 2.0l * (c*d+a*b        ) * yd ;
   R.m[2][2] =        (a*a+d*d-c*c-b*b) * zd ;

   /* load offsets */

   R.m[0][3] = qx ; R.m[1][3] = qy ; R.m[2][3] = qz ;

   return R ;
}

      
/**
 * get the NIFTI intention (such as statistical parameters) information.
 */
void
NiftiHelper::getNiftiIntentionInformation(const nifti_1_header& hdr,
                                          QString& niftiIntentCodeAndParam,
                                          QString& niftiIntentName)
{
   niftiIntentName = ("NIFTI_INTENT_NOT_RECOGNIZED_CODE_"
                      + QString::number(hdr.intent_code));
   
   QString intentDescription;
   QString p1;
   QString p2;
   QString p3;
   
   switch (hdr.intent_code) {
     case NIFTI_INTENT_CORREL:     
        niftiIntentName = "NIFTI_INTENT_CORREL";
        intentDescription = "Correlation statistic" ;
        p1 = "DOF";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_TTEST:      
        niftiIntentName = "NIFTI_INTENT_TTEST";
        intentDescription = "T-statistic" ;
        p1 = "DOF";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_FTEST:      
        niftiIntentName = "NIFTI_INTENT_FTEST";
        intentDescription = "F-statistic" ;
        p1 = "Numerator DOF";
        p2 = "Denorminator DOF";
        p3 = "";
        break;
     case NIFTI_INTENT_ZSCORE:     
        niftiIntentName = "NIFTI_INTENT_ZSCORE";
        intentDescription = "Z-score"     ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_CHISQ:      
        niftiIntentName = "NIFTI_INTENT_CHISQ";
        intentDescription = "Chi-squared distribution" ;
        p1 = "DOF";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_BETA:       
        niftiIntentName = "NIFTI_INTENT_BETA";
        intentDescription = "Beta distribution" ;
        p1 = "a";
        p2 = "b";
        p3 = "";
        break;
     case NIFTI_INTENT_BINOM:      
        niftiIntentName = "NIFTI_INTENT_BINOM";
        intentDescription = "Binomial distribution" ;
        p1 = "Number of Trials";
        p2 = "Probability per Trial";
        p3 = "";
        break;
     case NIFTI_INTENT_GAMMA:      
        niftiIntentName = "NIFTI_INTENT_GAMMA";
        intentDescription = "Gamma distribution" ;
        p1 = "Shape";
        p2 = "Scale";
        p3 = "";
        break;
     case NIFTI_INTENT_POISSON:    
        niftiIntentName = "NIFTI_INTENT_POISSON";
        intentDescription = "Poisson distribution" ;
        p1 = "Mean";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_NORMAL:     
        niftiIntentName = "NIFTI_INTENT_NORMAL";
        intentDescription = "Normal distribution" ;
        p1 = "Mean";
        p2 = "Standard Deviation";
        p3 = "";
        break;
     case NIFTI_INTENT_FTEST_NONC: 
        niftiIntentName = "NIFTI_INTENT_FTEST_NONC";
        intentDescription = "F-statistic noncentral" ;
        p1 = "Numerator DOF";
        p2 = "Denominator DOF";
        p3 = "Numerator Noncentrality Parameter";
        break;
     case NIFTI_INTENT_CHISQ_NONC: 
        niftiIntentName = "NIFTI_INTENT_CHISQ_NONC";
        intentDescription = "Chi-squared noncentral" ;
        p1 = "DOF";
        p2 = "Noncentrality Parameter";
        p3 = "";
        break;
     case NIFTI_INTENT_LOGISTIC:   
        niftiIntentName = "NIFTI_INTENT_LOGISTIC";
        intentDescription = "Logistic distribution" ;
        p1 = "Location";
        p2 = "Scale";
        p3 = "";
        break;
     case NIFTI_INTENT_LAPLACE:    
        niftiIntentName = "NIFTI_INTENT_LAPLACE";
        intentDescription = "Laplace distribution" ;
        p1 = "Location";
        p2 = "Scale";
        p3 = "";
        break;
     case NIFTI_INTENT_UNIFORM:    
        niftiIntentName = "NIFTI_INTENT_UNIFORM";
        intentDescription = "Uniform distribition" ;
        p1 = "Lower End";
        p2 = "Upper End";
        p3 = "";
        break;
     case NIFTI_INTENT_TTEST_NONC: 
        niftiIntentName = "NIFTI_INTENT_TTEST_NONC";
        intentDescription = "T-statistic noncentral" ;
        p1 = "DOF";
        p2 = "Noncentrality Parameter";
        p3 = "";
        break;
     case NIFTI_INTENT_WEIBULL:    
        niftiIntentName = "NIFTI_INTENT_WEIBULL";
        intentDescription = "Weibull distribution" ;
        p1 = "Location";
        p2 = "Scale";
        p3 = "Power";
        break;
     case NIFTI_INTENT_CHI:        
        niftiIntentName = "NIFTI_INTENT_CHI";
        intentDescription = "Chi distribution" ;
        p1 = "Half Normal Distribution";
        p2 = "Rayleigh Distritibution";
        p3 = "Maxwell-Boltzmann Distribution";
        break;
     case NIFTI_INTENT_INVGAUSS:   
        niftiIntentName = "NIFTI_INTENT_INVGAUSS";
        intentDescription = "Inverse Gaussian distribution" ;
        p1 = "MU";
        p2 = "Lambda";
        p3 = "";
        break;
     case NIFTI_INTENT_EXTVAL:     
        niftiIntentName = "NIFTI_INTENT_EXTVAL";
        intentDescription = "Extreme Value distribution" ;
        p1 = "Location";
        p2 = "Scale";
        p3 = "";
        break;
     case NIFTI_INTENT_PVAL:       
        niftiIntentName = "NIFTI_INTENT_PVAL";
        intentDescription = "P-value" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_LOGPVAL:    
        niftiIntentName = "NIFTI_INTENT_LOGPVAL";
        intentDescription = "Log P-value" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_LOG10PVAL:  
        niftiIntentName = "NIFTI_INTENT_LOG10PVAL";
        intentDescription = "Log10 P-value" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_ESTIMATE:   
        niftiIntentName = "NIFTI_INTENT_ESTIMATE";
        intentDescription = "Estimate" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_LABEL:      
        niftiIntentName = "NIFTI_INTENT_LABEL";
        intentDescription = "Label index" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_NEURONAME:  
        niftiIntentName = "NIFTI_INTENT_NEURONAME";
        intentDescription = "NeuroNames index" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_GENMATRIX:  
        niftiIntentName = "NIFTI_INTENT_GENMATRIX";
        intentDescription = "General matrix" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_SYMMATRIX:  
        niftiIntentName = "NIFTI_INTENT_SYMMATRIX";
        intentDescription = "Symmetric matrix" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_DISPVECT:   
        niftiIntentName = "NIFTI_INTENT_DISPVECT";
        intentDescription = "Displacement vector" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_VECTOR:     
        niftiIntentName = "NIFTI_INTENT_VECTOR";
        intentDescription = "Vector" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_POINTSET:   
        niftiIntentName = "NIFTI_INTENT_POINTSET";
        intentDescription = "Pointset" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_TRIANGLE:   
        niftiIntentName = "NIFTI_INTENT_TRIANGLE";
        intentDescription = "Triangle" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_QUATERNION: 
        niftiIntentName = "NIFTI_INTENT_QUATERNION";
        intentDescription = "Quaternion" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
     case NIFTI_INTENT_DIMLESS:    
        niftiIntentName = "NIFTI_INTENT_DIMLESS";
        intentDescription = "Dimensionless number" ;
        p1 = "";
        p2 = "";
        p3 = "";
        break;
   }

   QString s;
   
   if (intentDescription.isEmpty() == false) {
      s += intentDescription;
      
      if (p1.isEmpty() == false) {
         s += ("  "
               + p1
               + "="
               + QString::number(hdr.intent_p1, 'f', 2));
      }
      
      if (p2.isEmpty() == false) {
         s += ("  "
               + p2
               + "="
               + QString::number(hdr.intent_p2, 'f', 2));
      }
      
      if (p3.isEmpty() == false) {
         s += ("  "
               + p3
               + "="
               + QString::number(hdr.intent_p3, 'f', 2));
      }
   }
   
   niftiIntentCodeAndParam = s;
}
      
/**
 * analyze HDR file is actually a NIFTI HDR file.
 */
bool 
NiftiHelper::hdrIsNiftiFile(const QString& hdrFileName)
{
   QFile file(hdrFileName);
   if (file.open(QIODevice::ReadOnly)) {
      //
      // read bytes 348
      //
      int numBytesToRead = 348;
      char bytes[numBytesToRead];
      QDataStream stream(&file);
      const bool errorFlag = (stream.readRawData(bytes, numBytesToRead) != numBytesToRead);
      file.close();
      if (errorFlag) {
         return false;
      }
      
      //
      // Is this the NIFTI code "ni1"?
      //
      if ((bytes[344] == 'n') &&
          (bytes[345] == 'i') &&
          (bytes[346] == '1')) {
         return true;
      }
   }
   
   return false;
}

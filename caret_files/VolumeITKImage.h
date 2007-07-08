
#ifndef __VOLUME_ITK_IMAGE_H__
#define __VOLUME_ITK_IMAGE_H__

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

#ifdef HAVE_ITK
#include "itkImage.h"
#endif // HAVE_ITK

// class for holding an ITK Image in memory
class VolumeITKImage {
   public:      
      // constructor
      VolumeITKImage();
      
      // destructor
      ~VolumeITKImage();
   
#ifdef HAVE_ITK
      /// determine if ITK is supported
      static bool itkSupported() { return true; }
      
      /// A 3 dimensional floating point volume type
      typedef itk::Image<float,3> ImageTypeFloat3;
      
      /// Smart pointer to a 3 dimensional floating point volume type
      typedef ImageTypeFloat3::Pointer ImagePointer;
      
      /// the image storage
      ImagePointer image;
      
#else  // HAVE_ITK
      /// determine if ITK is supported
      static bool itkSupported() { return false; }
      
#endif // HAVE_ITK
};

#endif // __VOLUME_ITK_IMAGE_H__


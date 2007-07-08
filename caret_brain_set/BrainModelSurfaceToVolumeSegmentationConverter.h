
#ifndef __BRAIN_MODEL_SURFACE_TO_VOLUME_SEGMENTATION_CONVERTER_H__
#define __BRAIN_MODEL_SURFACE_TO_VOLUME_SEGMENTATION_CONVERTER_H__

/****************************************************************************
**
** Copyright (C) 1992-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class VolumeFile;

/// create a segmentation volume from a surface
class BrainModelSurfaceToVolumeSegmentationConverter : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceToVolumeSegmentationConverter(BrainSet* bs,
                                                     BrainModelSurface* inputSurfaceIn,
                                                     VolumeFile* outputSegmentationVolumeIn,
                                                     const bool fillCavitiesFlagIn,
                                                     const bool fillHandlesFlagIn);

      // destructor
      ~BrainModelSurfaceToVolumeSegmentationConverter();
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      /// the input surface
      BrainModelSurface* inputSurface;
      
      /// the segmentation volume
      VolumeFile* outputSegmentationVolume;
      
      /// fill any cavities in the segmentation
      bool fillCavitiesFlag;
      
      /// fill any detected handles in the segmentation
      bool fillHandlesFlag;
};

#endif // __BRAIN_MODEL_SURFACE_TO_VOLUME_SEGMENTATION_CONVERTER_H__


#ifndef __BRAIN_MODEL_SURFACE_ROI_H__
#define __BRAIN_MODEL_SURFACE_ROI_H__

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

#include <vector>

#include "BorderFile.h"
#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class BrainModelSurfaceROINodeSelection;
class TopologyHelper;

/// base class for performing region of interest operations on a surface
class BrainModelSurfaceROIOperation : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceROIOperation(BrainSet* bs,
                           const BrainModelSurface* bmsIn,
                           const BrainModelSurfaceROINodeSelection* inputSurfaceROIIn);
      
      // destructor
      ~BrainModelSurfaceROIOperation();
      
      // execute the operation
      void execute() throw (BrainModelAlgorithmException);
      
      /// get the text report
      QString getReportText() const { return reportText; }
      
   protected:     
      // execute the operation
      virtual void executeOperation() throw (BrainModelAlgorithmException) = 0;
       
      // get the topology helper
      const TopologyHelper* getTopologyHelper() const;
      
      /// get the separator character
      static QString getSeparatorCharacter() { return ";"; }
      
      // Create the report header
      void createReportHeader(float& roiAreaOut);
             
      // set the header text
      void setHeaderText(const QString& headerTextIn);
      
      /// surface on which to perform operation
      const BrainModelSurface* bms;
      
      /// the ROI selection
      BrainModelSurfaceROINodeSelection* operationSurfaceROI;
      
      /// report text 
      QString reportText;
      
      /// area of each tile
      std::vector<float> tileArea;
      
      /// tile in ROI flags
      std::vector<bool> tileInROI;
            
      /// the separator character
      static const QString separatorCharacter;
      
   private:
      /// the header text
      QString headerText;
      
      /// the input region of interest
      const BrainModelSurfaceROINodeSelection* inputSurfaceROI;
      
};

#endif // __BRAIN_MODEL_SURFACE_ROI_H__


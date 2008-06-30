
#ifndef __NODE_REGION_OF_INTEREST_FILE_H__
#define __NODE_REGION_OF_INTEREST_FILE_H__

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

#include "PaintFile.h"

class CoordinateFile;
class TopologyFile;
class VolumeFile;

/// class for storing the nodes in a region of interest
class NodeRegionOfInterestFile : public PaintFile {
   public:
      // constructor
      NodeRegionOfInterestFile();
      
      // destructor
      ~ NodeRegionOfInterestFile();
      
      // set number of nodes
      void setNumberOfNodes(const int numNodes);
      
      // get node selected
      bool getNodeSelected(const int nodeNumber) const;
      
      // set node selected
      void setNodeSelected(const int nodeNumber,
                           const bool status);
            
      // get the ROI description
      QString getRegionOfInterestDescription() const;
      
      // set the ROI description (number of nodes must have been set)
      void setRegionOfInterestDescription(const QString& s);
      
      // assign nodes in ROI by intersecting with a volume file
      void assignSelectedNodesWithVolumeFile(const VolumeFile* vf,
                                             const CoordinateFile* cf,
                                             const TopologyFile* tf) throw (FileException);
   protected:
      /// set the number of nodes and columns in the file
      virtual void setNumberOfNodesAndColumns(const int numNodes, 
                                              const int numCols,
                                              const int numElementsPerCol = 1);
                                              
      /// paint name index for selected nodes
      int selectedPaintIndex;
      
      /// paint name index for deselected nodes;
      int deselectedPaintIndex;
};

#endif // __NODE_REGION_OF_INTEREST_FILE_H__



#ifndef __BRAIN_MODEL_SURFACE_CONNECTED_PAINT_SEARCH_H__
#define __BRAIN_MODEL_SURFACE_CONNECTED_PAINT_SEARCH_H__


#include "BrainModelSurfaceConnectedSearch.h"

class PaintFile;

/// Class that searches for connected nodes with a paint values
class BrainModelSurfaceConnectedSearchPaint : public BrainModelSurfaceConnectedSearch {
   public:
      /// Constructor
      BrainModelSurfaceConnectedSearchPaint(BrainSet* bs, 
                           BrainModelSurface* bmsIn,
                           const int startNodeIn,
                           const PaintFile* paintFileIn,
                           const int paintColumnIn,
                           const int paintIndexIn,
                           const std::vector<int>* limitToTheseNodesIn = NULL);
                                       
      /// Destructor
      virtual ~BrainModelSurfaceConnectedSearchPaint();
      
   protected:
      /// accept a node 
      virtual bool acceptNode(const int nodeNumber);
   
      /// paint file being searched
      const PaintFile* paintFile;
      
      /// paint column for connection search
      int paintColumn;
      
      /// paint index for search
      int paintIndex;
};

#endif // __BRAIN_MODEL_SURFACE_CONNECTED_PAINT_SEARCH_H__


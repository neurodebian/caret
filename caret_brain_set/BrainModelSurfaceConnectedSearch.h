
#ifndef __BRAIN_MODEL_SURFACE_CONNECTED_SEARCH_H__
#define __BRAIN_MODEL_SURFACE_CONNECTED_SEARCH_H__

#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;

/// class that searches and identifies topologically connected nodes
class BrainModelSurfaceConnectedSearch : public BrainModelAlgorithm {
   public:
      /// Constructor
      BrainModelSurfaceConnectedSearch(BrainSet* bs, 
                                       const BrainModelSurface* bmsIn,
                                       const int startNodeIn,
                                       const std::vector<int>* limitToTheseNodesIn = NULL);
      
      /// Destructor
      virtual ~BrainModelSurfaceConnectedSearch();
      
      /// execute the search
      virtual void execute() throw (BrainModelAlgorithmException);
      
      /// see if a node is connected to the start node
      bool getNodeConnected(const int nodeNumber) const;
      
   protected:
      /// accept a node (override this to accept/reject nodes during the connected search)
      virtual bool acceptNode(const int nodeNumber);
      
      /// the brain model surface for searching
      const BrainModelSurface* bms;
      
      /// starting node for search
      int startNode;
      
      /// limit the search to these nodes
      const std::vector<int>* limitToTheseNodes;
      
      /// number of nodes in the surface
      int numNodes;
      
      /// node visited flag used during search
      std::vector<int> visited;
      
      /// node connected flag queried by user after search
      std::vector<int> nodeConnected;
      
};

#endif // __BRAIN_MODEL_SURFACE_CONNECTED_SEARCH_H__

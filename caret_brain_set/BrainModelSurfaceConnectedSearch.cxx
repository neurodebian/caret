
#include <algorithm>
#include <stack>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceConnectedSearch.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"

/**
 * Constructor.
 */
BrainModelSurfaceConnectedSearch::BrainModelSurfaceConnectedSearch(BrainSet* bs, 
                                       const BrainModelSurface* bmsIn,
                                       const int startNodeIn,
                                       const std::vector<int>* limitToTheseNodesIn)
   : BrainModelAlgorithm(bs), 
     bms(bmsIn),
     startNode(startNodeIn),
     limitToTheseNodes(limitToTheseNodesIn),
     numNodes(0)
{
}

/**
 * Destructor.
 */
BrainModelSurfaceConnectedSearch::~BrainModelSurfaceConnectedSearch()
{
   visited.clear();
   nodeConnected.clear();
}

/**
 * execute the search.
 */
void
BrainModelSurfaceConnectedSearch::execute() throw (BrainModelAlgorithmException)
{
   numNodes = bms->getNumberOfNodes();
   if (numNodes <= 0) {
      return;
   }
   
   visited.resize(numNodes);
   nodeConnected.resize(numNodes);
   for (int i = 0; i < numNodes; i++) {
      visited[i] = false;
      nodeConnected[i] = false;
   }
   
   //
   // See if search is limited to a subset of nodes
   //
   if (limitToTheseNodes != NULL) {
      //
      // Mark nodes that are not to be searched as already visited
      //
      const int numLimitNodes = static_cast<int>(limitToTheseNodes->size());
      for (int i = 0; i < numLimitNodes; i++) {
         if (i < numNodes) {
            if ((*limitToTheseNodes)[i] == false) {
               visited[i] = true;
            }
         }
      }
   }
      
   //
   // Use a topology helper to get node neighbor information
   //
   const TopologyFile* tf = bms->getTopologyFile();
   //TopologyHelper th(tf, false, true, false);
   const TopologyHelper* th = tf->getTopologyHelper(false, true, false);
   
   //
   // Mark the starting node as connected.
   //
   nodeConnected[startNode] = true;
    
   //
   // Use a stack to help with connected node search.
   //
   std::stack<int> st;
   st.push(startNode);
  
   while(!st.empty()) {
      const int nodeNumber = st.top();
      st.pop();

      //
      // Is node unvisited ?
      //
      if (visited[nodeNumber] == false) {
         visited[nodeNumber] = true;
         
         //
         // Does node meet criteria for acceptance
         // 
         bool useNode = false;
         if (nodeNumber == startNode) {
            useNode = true;
         }
         else if (acceptNode(nodeNumber)) {
            useNode = true;
         }
         
         if (useNode) {
            nodeConnected[nodeNumber] = true;
            
            //
            // Get neighbors of this node
            //
            int numNeighbors = 0;
            const int* neighbors = th->getNodeNeighbors(nodeNumber, numNeighbors);
            
            //
            // add neighbors to search
            //
            for (int i = 0; i < numNeighbors; i++) {
               const int neighborNode = neighbors[i];
               if (visited[neighborNode] == false) {
                  st.push(neighborNode);
               }
            }
         }
      }
   }
}

/**
 * accept/reject a node for continued searching.
 */
bool
BrainModelSurfaceConnectedSearch::acceptNode(const int nodeNumber)
{
   if (nodeNumber >= 0) {
      return true;
   }
   return false;
}

/**
 * Find out if a node is connected.  Call after "execute()".
 */
bool
BrainModelSurfaceConnectedSearch::getNodeConnected(const int nodeNumber) const 
{
   if (nodeNumber < numNodes) {
      return nodeConnected[nodeNumber];
   }
   return false;
}



#include "BrainModelSurfaceConnectedSearchPaint.h"
#include "BrainSet.h"
#include "PaintFile.h"

/**
 * Constructor
 */
BrainModelSurfaceConnectedSearchPaint::BrainModelSurfaceConnectedSearchPaint(
                                          BrainSet* bs, 
                                          BrainModelSurface* bmsIn,
                                          const int startNodeIn,
                                          const PaintFile* paintFileIn,
                                          const int paintColumnIn,
                                          const int paintIndexIn,
                                          const std::vector<int>* limitToTheseNodesIn)
   : BrainModelSurfaceConnectedSearch(bs, bmsIn, startNodeIn, limitToTheseNodesIn)
   , paintFile(paintFileIn)
{
   paintColumn  = paintColumnIn;
   paintIndex   = paintIndexIn;
}

/**
 * Destructor
 */
BrainModelSurfaceConnectedSearchPaint::~BrainModelSurfaceConnectedSearchPaint()
{
}

/**
 * See if nodes paint matches the correct index
 */
bool
BrainModelSurfaceConnectedSearchPaint::acceptNode(const int nodeNumber)
{
   const float indx = paintFile->getPaint(nodeNumber, paintColumn);
   if (paintIndex == indx) {
      return true;
   }
   
   return false;
}


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

#include "BrainModel.h"
#include "BrainModelContours.h"
#include "BrainModelOpenGLSelectedItem.h"
#include "BrainModelSurface.h"
#include "BrainModelVolume.h"

/**
 * Constructor.
 */
BrainModelOpenGLSelectedItem::BrainModelOpenGLSelectedItem()
{
   reset(NULL, NULL, -1);
}

/**
 * Destructor.
 */
BrainModelOpenGLSelectedItem::~BrainModelOpenGLSelectedItem()
{
}

/**
 * Reinitialize this item.
 */
void
BrainModelOpenGLSelectedItem::reset(BrainSet* brainSetIn,
                                    BrainModel* brainModelIn,
                                    const int viewingWindowNumberIn)
{
   brainSet = brainSetIn;
   brainModel = brainModelIn;
   viewingWindowNumber = viewingWindowNumberIn;
   
   itemType = ITEM_TYPE_NONE;   
   index1 = -1;
   index2 = -1;
   index3 = -1;
   index4 = -1;
   index5 = -1;
   index6 = -1;
}

/**
 * Replace current item if parameters in are "closer".
 */
bool
BrainModelOpenGLSelectedItem::replaceIfCloser(const float depthIn,
                                                   const float distIn,
                                                   const ITEM_TYPE itemTypeIn,
                                                   const int index1In,
                                                   const int index2In,
                                                   const int index3In,
                                                   const int index4In,
                                                   const int index5In,
                                                   const int index6In)
{
   bool replaceIt = false;
   
   if (itemType == ITEM_TYPE_NONE) {
      replaceIt = true;
   }
   else if (depthIn < depth) {
      replaceIt = true;
   }
   else if (depthIn == depth) {
      if (distIn < distToPick) {
         replaceIt = true;
      }
   }
   
   if (replaceIt) {
      depth = depthIn;
      distToPick  = distIn;
      itemType = itemTypeIn;
      index1 = index1In;
      index2 = index2In;
      index3 = index3In;
      index4 = index4In;
      index5 = index5In;
      index6 = index6In;
   }
   
   return replaceIt;
}

/**
 * get the brain model surface.
 */
BrainModelSurface* 
BrainModelOpenGLSelectedItem::getBrainModelSurface() 
{ 
   return dynamic_cast<BrainModelSurface*>(brainModel); 
}

/**
 * get the brain model volume.
 */
BrainModelVolume* 
BrainModelOpenGLSelectedItem::getBrainModelVolume() 
{ 
   return dynamic_cast<BrainModelVolume*>(brainModel); 
}

/**
 * get the brain model contours.
 */
BrainModelContours* 
BrainModelOpenGLSelectedItem::getBrainModelContours() 
{ 
   return dynamic_cast<BrainModelContours*>(brainModel); 
}
      

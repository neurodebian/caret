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



#include "BrainSetNodeAttribute.h"

/**
 * Constructor
 */
BrainSetNodeAttribute::BrainSetNodeAttribute()
{
   reset();
}

/**
 * Constructor
 */
BrainSetNodeAttribute::BrainSetNodeAttribute(const int row, 
                                                               const int col, const int node)
{
   reset();
   
   morphRow    = row;
   morphColumn = col;
   morphNode   = node;
}

/**
 * Destructor
 */
BrainSetNodeAttribute::~BrainSetNodeAttribute()
{
}

/**
 * Reset all parameters
 */
void
BrainSetNodeAttribute::reset()
{
   displayFlag    = true;
   morphRow       = -1;
   morphColumn    = -1;
   morphNode      = -1;
   morphTile      = -1;
   morphTileAreas[0] = 0.0;
   morphTileAreas[1] = 0.0;
   morphTileAreas[2] = 0.0;
   morphTileNodes[0] = 0;
   morphTileNodes[1] = 0;
   morphTileNodes[2] = 0;
   highlight      = HIGHLIGHT_NODE_NONE;
   classification = CLASSIFICATION_TYPE_INTERIOR;
   crossover      = CROSSOVER_NO;
   nodeInROI      = false;
   visited        = false;
   for (int i = 0; i < 3; i++) {
      linearForce[i]  = 0.0;
      angularForce[i] = 0.0;
      totalForce[i]   = 0.0;
   }
}

/**
 * get flat morphing attributes
 */
void 
BrainSetNodeAttribute::getFlatMorphAttributes(int& row, int& col, int& node) const
{
   row  = morphRow;
   col  = morphColumn;
   node = morphNode;
}

/**
 * set flat morphing attributes
 */
void 
BrainSetNodeAttribute::setFlatMorphAttributes(const int row, const int col, const int node)
{
   morphRow    = row;
   morphColumn = col;
   morphNode   = node;
}

/**
 * get the spherical morphing attributes
 */
void
BrainSetNodeAttribute::getSphericalMorphingAttributes(int& node, int& tile, int tileNodes[3],
                                                      float tileAreas[3]) const
{
   node = morphNode;
   tile = morphTile;
   tileNodes[0] = morphTileNodes[0];
   tileNodes[1] = morphTileNodes[1];
   tileNodes[2] = morphTileNodes[2];
   tileAreas[0] = morphTileAreas[0];
   tileAreas[1] = morphTileAreas[1];
   tileAreas[2] = morphTileAreas[2];
}

/**
 * set the spherical morphing attributes
 */
void
BrainSetNodeAttribute::setSphericalMorphingAttributes(const int node, const int tile, 
                                                      const int tileNodes[3],
                                                      const float tileAreas[3])
{
   morphNode = node;
   morphTile = tile;
   morphTileNodes[0] = tileNodes[0];
   morphTileNodes[1] = tileNodes[1];
   morphTileNodes[2] = tileNodes[2];
   morphTileAreas[0] = tileAreas[0];
   morphTileAreas[1] = tileAreas[1];
   morphTileAreas[2] = tileAreas[2];
}

/**
 * get morphing forces
 */
void 
BrainSetNodeAttribute::getMorphingForces(float linear[3], float angular[3], float total[3]) const
{
   for (int i = 0; i < 3; i++) {
      linear[i]  = linearForce[i];
      angular[i] = angularForce[i];
      total[i]   = totalForce[i];
   }
}
      
/**
 * set morphing forces
 */
void 
BrainSetNodeAttribute::setMorphingForces(const float linear[3], const float angular[3], 
                                                  const float total[3])
{
   for (int i = 0; i < 3; i++) {
      linearForce[i]  = linear[i];
      angularForce[i] = angular[i];
      totalForce[i]   = total[i];
   }
}

/**
 * Set node highlighting
 */
void
BrainSetNodeAttribute::setHighlighting(const HIGHLIGHT_NODE_TYPE hl)
{
   //
   // If node already highlighted, clear its highlighting
   //
   if ((highlight != HIGHLIGHT_NODE_NONE)) {
      highlight = HIGHLIGHT_NODE_NONE;
   }
   else {
      highlight = hl;
   }
}


      

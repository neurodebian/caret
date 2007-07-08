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



#ifndef __VE_BRAIN_SET_NODE_ATTRIBUTE_H__
#define __VE_BRAIN_SET_NODE_ATTRIBUTE_H__

/// class for node attributes that are common to all surfaces
class BrainSetNodeAttribute { 
   public:
      /// type for highlighting nodes
      enum HIGHLIGHT_NODE_TYPE {
         HIGHLIGHT_NODE_NONE,
         HIGHLIGHT_NODE_LOCAL,
         HIGHLIGHT_NODE_REMOTE,
      };
      
      enum CLASSIFICATION_TYPE {
         CLASSIFICATION_TYPE_INTERIOR,
         CLASSIFICATION_TYPE_EDGE,
         CLASSIFICATION_TYPE_CORNER
      };
      
      enum CROSSOVER_STATUS {
         CROSSOVER_YES,
         CROSSOVER_NO,
         CROSSOVER_DEGENERATE_EDGE
      };
      
      /// Constructor
      BrainSetNodeAttribute();
      
      /// Constructor
      BrainSetNodeAttribute(const int row, const int col, const int node);
            
      /// Destructor
      ~BrainSetNodeAttribute();
      
      /// get flat morphing attributes
      void getFlatMorphAttributes(int& row, int& column, int& node) const;
      
      /// set flat morphing attributes
      void setFlatMorphAttributes(const int row, const int col, const int node);
      
      /// get the spherical morphing attributes
      void getSphericalMorphingAttributes(int &node, int& tile, int tileNodes[3],
                                          float tileAreas[3]) const;
      
      /// set the spherical morphing attributes
      void setSphericalMorphingAttributes(const int node, const int tile, const int tileNodes[3],
                                          const float tileAreas[3]);
      
      /// get morphing forces
      void getMorphingForces(float linear[3], float angular[3], float total[3]) const;
      
      /// set morphing forces
      void setMorphingForces(const float linear[3], const float angular[3], const float total[3]);
      
      /// set node highlighting
      void setHighlighting(const HIGHLIGHT_NODE_TYPE hl);
      
      /// get node highlighting
      HIGHLIGHT_NODE_TYPE getHighlighting() const { return highlight; }
      
      /// set the node classification
      void setClassification(const CLASSIFICATION_TYPE ct) { classification = ct; }
      
      /// get the node classification
      CLASSIFICATION_TYPE getClassification() const { return classification; }
      
      /// get crossover status
      CROSSOVER_STATUS getCrossover() const { return crossover; }
      
      /// set crossover status
      void setCrossover(const CROSSOVER_STATUS status) { crossover = status; }
      
      /// get node in ROI
      bool getNodeInROI() const { return nodeInROI; }
      
      /// set node in ROI flag
      void setNodeInROI(const bool inROI) { nodeInROI = inROI; }
      
      /// get visited flag
      bool getVisited() const { return visited; }
      
      /// set visited flag
      void setVisited(const bool value) { visited = value; }
      
      /// reset all parameters to default value
      void reset();
      
      /// get display flag
      inline bool getDisplayFlag() const { return displayFlag; }
      
      /// set display flag
      void setDisplayFlag(const bool df) { displayFlag = df; }
      
   private:
      /// the display flag
      bool displayFlag;
      
      /// subsampled grid row (used in flat multiresolution morphing)
      int morphRow;
      
      /// subsampeld grid column (used in flat multiresolution morphing)
      int morphColumn;
      
      /// nearest node in the surface that was subsampled
      int morphNode;
      
      /// tile projected to (used in spherical multiresolution morphing)
      int morphTile;
      
      /// areas of tile projected to (used in spherical multiresolution morphing)
      float morphTileAreas[3];
      
      /// nodes of tile projected to (used in spherical multiresolution morphing)
      int morphTileNodes[3];
      
      /// linear morphing force
      float linearForce[3];
      
      /// angular morphing force
      float angularForce[3];
      
      /// total morphing force
      float totalForce[3];
      
      /// node highlighting
      HIGHLIGHT_NODE_TYPE highlight;
      
      /// node classification
      CLASSIFICATION_TYPE classification;
      
      /// crossover status
      CROSSOVER_STATUS crossover;
      
      /// node in ROI flag
      bool nodeInROI;
      
      /// visited flag
      bool visited;
      
   friend class BrainSet;
   friend class BrainModelSurface;
   friend class BrainModelSurfaceMultiresolutionMorphing;
};

#endif // __VE_BRAIN_MODEL_SURFACE_NODE_ATTRIBUTE_H__


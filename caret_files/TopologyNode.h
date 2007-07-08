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



#ifndef __VE_TOPOLOGY_NODE_H__
#define __VE_TOPOLOGY_NODE_H__

/// Store topology related data
/**
 * Stores topology related data (section, category) for nodes
 */
class TopologyNode {
   public:
      /// Node categories
      enum CATEGORIES {
         INTERIOR   = 0,
         EDGE       = 1,
         CORNER     = 2,
         STRUCTURAL = 3
      };
   private:
      /// node's section number
      int section;
      
      /// Nodes category
      CATEGORIES category;
      
   public:
      /// Constructor
      TopologyNode(const int sectionIn, const CATEGORIES categoryIn) {
         setData(sectionIn, categoryIn);
      }
      
      /// get topology related data
      void getData(int& sectionOut, CATEGORIES& categoryOut) const {
         sectionOut  = section;
         categoryOut = category;
      }
      
      /// set topology related data
      void setData(const int sectionIn, const CATEGORIES categoryIn) {
         section  = sectionIn;
         category = categoryIn;
      }
      
      /// get the section number
      int getSectionNumber() const { return section; }
};

#endif


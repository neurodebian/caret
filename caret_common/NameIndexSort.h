
#ifndef __NAME_INDEX_SORT_H__
#define __NAME_INDEX_SORT_H__

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

#include <QString>
#include <vector>

/// name and index sort
class NameIndexSort {
   protected:
      /// class for storing names and indices
      class NameIndexPair {
         public:
            /// constructor
            NameIndexPair(const int indexIn, const QString& nameIn);
            
            /// the name
            QString name;
            
            /// the index
            int         indx;
            
            /// less than operator
            bool operator<(const NameIndexPair& nip) const;
      };
      
   public:
      /// Constructor (must call sortByName after adding items);
      NameIndexSort();
      
      /// Constructor
      NameIndexSort(const std::vector<int>& indicesIn,
                    const std::vector<QString>& namesIn);
                    
      /// Constructor
      ~NameIndexSort();
      
      /// add a pair of items (must call sortByName when done adding items)
      void add(const int indx,
               const QString& name);
               
      /// get number of name indice pairs
      int getNumberOfItems() const;
      
      /// get name and index for an item
      void getNameAndIndex(const int itemNum,
                           int& indexOut,
                           QString& nameOut) const;
      
      /// sort by name
      void sortByName();
      
   protected:
      /// the names and indices
      std::vector<NameIndexPair> names;
      
};

#endif // __NAME_INDEX_SORT_H__


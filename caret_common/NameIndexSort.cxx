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

#include <algorithm>

#include "NameIndexSort.h"

/**
 * Constructor.
 */
NameIndexSort::NameIndexSort()
{
}

/**
 * Constructor.
 */
NameIndexSort::NameIndexSort(const std::vector<int>& indicesIn,
                             const std::vector<QString>& namesIn)
{
   for (unsigned int i = 0; i < indicesIn.size(); i++) {
      add(indicesIn[i], namesIn[i]);
   }
   
   sortByNameCaseSensitive();
}
              
/**
 * Constructor.
 */
NameIndexSort::~NameIndexSort()
{
}

/**
 * add a pair of items (must call sort when done adding items).
 */
void 
NameIndexSort::add(const int indx,
                   const QString& name)
{
   names.push_back(NameIndexPair(indx, name));
}
         
/**
 * sort by name case sensitive.
 */
void 
NameIndexSort::sortByNameCaseSensitive()
{
   std::sort(names.begin(), names.end());
}

/**
 * sort by name case insensitive.
 */
void 
NameIndexSort::sortByNameCaseInsensitive()
{
   std::sort(names.begin(), names.end(), NameIndexPair::lessThanCaseInsensitive);
}

/**
 * get number of name indice pairs.
 */
int 
NameIndexSort::getNumberOfItems() const 
{ 
   return names.size(); 
}

/**
 * get name and index for an item after sorting.
 */
void 
NameIndexSort::getSortedNameAndIndex(const int itemNum,
                                     int& indexOut,
                                     QString& nameOut) const
{
   indexOut = names[itemNum].indx;
   nameOut  = names[itemNum].name;
}
                           
/**
 * get index for an item after sorting.
 */
int 
NameIndexSort::getSortedIndex(const int itemNum) const
{
   return names[itemNum].indx;
}
      
/**
 * get name for an item after sorting.
 */
QString 
NameIndexSort::getSortedName(const int itemNum) const
{
   return names[itemNum].name;
}
      
/**
 * constructor.
 */
NameIndexSort::NameIndexPair::NameIndexPair(const int indexIn, const QString& nameIn)
{
   indx = indexIn;
   name = nameIn;
}

/**
 * less than operator.
 */
bool 
NameIndexSort::NameIndexPair::operator<(const NameIndexPair& nip) const
{
   if (name == nip.name) {
      return (indx < nip.indx);
   }
   return (name < nip.name);
}

//=================================================================
/**
 * compare case insensitive.
 */
bool 
NameIndexSort::NameIndexPair::lessThanCaseInsensitive(const NameIndexPair& nip1,
                                                      const NameIndexPair& nip2)
{
   return (QString::compare(nip1.name, nip2.name, Qt::CaseInsensitive) < 0);
}

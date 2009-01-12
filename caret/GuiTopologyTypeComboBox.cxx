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

#include "GuiTopologyTypeComboBox.h"

/**
 * constructor.
 */
GuiTopologyTypeComboBox::GuiTopologyTypeComboBox(const bool showUnknown,
                                               QWidget* parent)
   : QComboBox(parent)
{
   addItem(TopologyFile::getPerimeterIDFromTopologyType(TopologyFile::TOPOLOGY_TYPE_CLOSED), 
           static_cast<int>(TopologyFile::TOPOLOGY_TYPE_CLOSED));
   addItem(TopologyFile::getPerimeterIDFromTopologyType(TopologyFile::TOPOLOGY_TYPE_OPEN), 
           static_cast<int>(TopologyFile::TOPOLOGY_TYPE_OPEN));
   addItem(TopologyFile::getPerimeterIDFromTopologyType(TopologyFile::TOPOLOGY_TYPE_CUT), 
           static_cast<int>(TopologyFile::TOPOLOGY_TYPE_CUT));
   addItem(TopologyFile::getPerimeterIDFromTopologyType(TopologyFile::TOPOLOGY_TYPE_LOBAR_CUT), 
           static_cast<int>(TopologyFile::TOPOLOGY_TYPE_LOBAR_CUT));
   if (showUnknown) {
      addItem(TopologyFile::getPerimeterIDFromTopologyType(TopologyFile::TOPOLOGY_TYPE_UNKNOWN),  
              static_cast<int>(TopologyFile::TOPOLOGY_TYPE_UNKNOWN));
      setTopologyType(TopologyFile::TOPOLOGY_TYPE_UNKNOWN);
   }
}
          
/**
 * constructor.
 */
GuiTopologyTypeComboBox::GuiTopologyTypeComboBox(const std::vector<TopologyFile::TOPOLOGY_TYPES> showTheseTypes,
                                               QWidget* parent)
   : QComboBox(parent)
{
   for (unsigned int i = 0; i < showTheseTypes.size(); i++) {
      const TopologyFile::TOPOLOGY_TYPES st = showTheseTypes[i];
      addItem(TopologyFile::getPerimeterIDFromTopologyType(st),
              static_cast<int>(st));
   }
}

/**
 * destructor.
 */
GuiTopologyTypeComboBox::~GuiTopologyTypeComboBox()
{
}

/**
 * set topology type.
 */
void 
GuiTopologyTypeComboBox::setTopologyType(const TopologyFile::TOPOLOGY_TYPES st)
{
   for (int i = 0; i < count(); i++) {
      if (itemData(i).toInt() == static_cast<int>(st)) {
         setCurrentIndex(i);
         return;
      }
   }
}

/**
 * get topology type.
 */
TopologyFile::TOPOLOGY_TYPES 
GuiTopologyTypeComboBox::getTopologyType() const
{
   return static_cast<TopologyFile::TOPOLOGY_TYPES>(itemData(currentIndex()).toInt());
}

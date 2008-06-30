
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

#include <QWidget>

#include "WuQWidgetGroup.h"

/**
 * constructor.
 */
WuQWidgetGroup::WuQWidgetGroup(QWidget* parent)
   : QObject(parent)
{
}

/**
 * destructor.
 */
WuQWidgetGroup::~WuQWidgetGroup()
{
   //
   // Note: Do not "delete" the widgets !!!!
   //
   widgets.clear();
}

/**
 * add a widget to the group.
 */
void 
WuQWidgetGroup::addWidget(QWidget* w)
{
   widgets.push_back(w);
}

/**
 * enable the group's widgets.
 */
void 
WuQWidgetGroup::setEnabled(bool enable)
{
   for (int i = 0; i < widgets.size(); i++) {
      widgets.at(i)->setEnabled(enable);
   }
}

/**
 * disable the group's widgets.
 */
void 
WuQWidgetGroup::setDisabled(bool disable)
{
   for (int i = 0; i < widgets.size(); i++) {
      widgets.at(i)->setDisabled(disable);
   }
}

/**
 * make the group's widgets visible.
 */
void 
WuQWidgetGroup::setVisible(bool makeVisible)
{
   for (int i = 0; i < widgets.size(); i++) {
      QWidget* w = widgets.at(i);
      w->setVisible(makeVisible);
   }
}

/**
 * make the group's widgets hidden.
 */
void 
WuQWidgetGroup::setHidden(bool hidden)
{
   setVisible(! hidden);
}

/**
 * block signals.
 */
void 
WuQWidgetGroup::blockSignals(bool blockTheSignals)
{
   for (int i = 0; i < widgets.size(); i++) {
      widgets.at(i)->blockSignals(blockTheSignals);
   }
}
      
/** 
 * make all of the widgets in the group the same size as size hint
 * of largest widget.
 */
void 
WuQWidgetGroup::resizeAllToLargestSizeHint()
{
   int largestWidth  = -1;
   int largestHeight = -1;
   
   for (int i = 0; i < widgets.size(); i++) {
      const QSize size = widgets.at(i)->sizeHint();
      if (size.width() > largestWidth) {
         largestWidth = size.width();
      }
      if (size.height() > largestHeight) {
         largestHeight = size.height();
      }
   }
   
   if ((largestWidth > 0) &&
       (largestHeight > 0)) {
      QSize newSize(largestWidth, largestHeight);
      
      for (int i = 0; i < widgets.size(); i++) {
         widgets.at(i)->setFixedSize(newSize);
      }
   }
}
      


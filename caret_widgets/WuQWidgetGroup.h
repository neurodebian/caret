
#ifndef __QT_WIDGET_GROUP_H__
#define __QT_WIDGET_GROUP_H__

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

#include <QObject>
#include <QVector>

class QWidget;

/// class for grouping widgets for ease of disabling and hiding
class WuQWidgetGroup : public QObject {
   Q_OBJECT
   
   public:
      // constructor
      WuQWidgetGroup(QWidget* parent);
      
      // destructor
      ~WuQWidgetGroup();
      
      // add a widget to the group
      void addWidget(QWidget* w);
      
   public slots:
      // block signals
      void blockSignals(bool blockTheSignals);
      
      // enable the group's widgets
      void setEnabled(bool enable);
      
      // disable the group's widgets
      void setDisabled(bool disable);
      
      // make the group's widgets visible
      void setVisible(bool makeVisible);
      
      // make the group's widgets hidden
      void setHidden(bool hidden);
      
      // make all of the widgets in the group the same size as size hint
      // of largest widget
      void resizeAllToLargestSizeHint();
      
      // set status of all checkboxes
      void setAllCheckBoxesChecked(const bool b);
      
   protected:
      /// keeps track of the widgets
      QVector<QWidget*> widgets;
};

#endif // __QT_WIDGET_GROUP_H__

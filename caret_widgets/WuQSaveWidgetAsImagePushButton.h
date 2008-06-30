
#ifndef __WU_QSAVE_WIDGET_AS_IMAGE_H___
#define __WU_QSAVE_WIDGET_AS_IMAGE_H___

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

#include <QPushButton>

/// class for saving the contents of a widget as an image
class WuQSaveWidgetAsImagePushButton : public QPushButton {
   Q_OBJECT
   
   public:
      /// Constructor
      WuQSaveWidgetAsImagePushButton(const QString& text,
                                     QWidget* widgetToSaveImageOfIn,
                                     QWidget* parent = 0);
                                    
      /// Destructor
      ~WuQSaveWidgetAsImagePushButton();
      
   protected slots:
      /// called when button pressed to save the image
      void slotClicked();
      
   protected:
      /// widget that is to be saved
      QWidget* widgetToSaveImageOf;
};

#endif // __WU_QSAVE_WIDGET_AS_IMAGE_H___


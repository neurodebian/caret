
#ifndef __CARET_TIPS_H__
#define __CARET_TIPS_H__

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

/// a class for storing tips on how to use Caret
class CaretTips {
   public:
      // constructor
      CaretTips();
      
      // destructor
      ~CaretTips();
      
      // get the number of Caret tips
      int getNumberOfCaretTips() const;
      
      // get a tip
      void getTip(const int tipNumber,
                  QString& tipText,
                  QString& helpPage) const;
      
   protected:

      /// a tip
      class Tip {
         public:
            // constructor
            Tip(const QString& tipIn, const QString& helpPageIn = "");
            
            // destructor
            ~Tip();
            
            /// the tip
            QString tip;
            
            /// the help page
            QString helpPage;
      };
            
      /// the tips
      std::vector<Tip> tips;
      

      
};

#endif // __CARET_TIPS_H__


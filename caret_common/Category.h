
#ifndef __CATEGORY_H__
#define __CATEGORY_H__

#include <QString>
#include <vector>

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

#include <vector>

#include <QString>

/// valid categories
class Category {
   public:
      /// category type
      enum TYPE {
         /// atlas
         TYPE_ATLAS,
         /// individual
         TYPE_INDIVIDUAL,
         /// tutorial
         TYPE_TUTORIAL
      };
      
      // constructor
      Category();
      
      // constructor
      Category(const TYPE& t);
      
      // constructor
      Category(const QString& name);
      
      // destructor
      ~Category();
      
      /// equality operator
      bool operator==(const Category& cat) { return (type == cat.type); }
      
      /// inequality operator
      bool operator!=(const Category& cat) { return (type != cat.type); }
      
      // reset
      void reset();
      
      // get category name
      QString getName() const;
      
      /// get the category type
      TYPE getType() const { return type; }
      
      // set the category using type
      void setUsingType(const TYPE typeIn);
      
      // set the category using name
      void setUsingName(const QString& nameIn);
      
      // get a list of categories and types
      static void getAllCategoryTypesAndNames(std::vector<TYPE>& typesOut,
                                              std::vector<QString>& namesOut);
      
   protected:
      /// category type
      TYPE type;
};

#endif // __CATEGORY_H__


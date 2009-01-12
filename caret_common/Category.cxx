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

#include "Category.h"

/**
 * constructor.
 */
Category::Category()
{
   reset();
}

/**
 * constructor.
 */
Category::Category(const TYPE& t)
{
   reset();
   setUsingType(t);
}

/**
 * constructor.
 */
Category::Category(const QString& name)
{
   reset();
   setUsingName(name);
}

/**
 * destructor.
 */
Category::~Category()
{
}

/**
 * reset.
 */
void 
Category::reset()
{
   type = TYPE_INDIVIDUAL;
}

/**
 * get category name.
 */
QString 
Category::getName() const
{
   QString name;
   
   std::vector<TYPE> types;
   std::vector<QString> names;
   getAllCategoryTypesAndNames(types, names);
   
   for (unsigned int i = 0; i < types.size(); i++) {
      if (types[i] == type) {
         name = names[i];
         break;
      }
   }

   return name;
}

/**
 * set the category using type.
 */
void 
Category::setUsingType(const TYPE typeIn)
{
   type = typeIn;
}

/**
 * set the category using name.
 */
void 
Category::setUsingName(const QString& nameIn)
{
   type = TYPE_INDIVIDUAL;
   
   std::vector<TYPE> types;
   std::vector<QString> names;
   getAllCategoryTypesAndNames(types, names);
   
   for (unsigned int i = 0; i < types.size(); i++) {
      if (names[i].toLower() == nameIn.toLower()) {
         type = types[i];
         break;
      }
   }
}

/**
 * get a list of categories and types.
 */
void 
Category::getAllCategoryTypesAndNames(std::vector<TYPE>& typesOut,
                                      std::vector<QString>& namesOut)
{
   typesOut.clear();
   namesOut.clear();
   
   typesOut.push_back(TYPE_ATLAS);       namesOut.push_back("Atlas");
   typesOut.push_back(TYPE_INDIVIDUAL);  namesOut.push_back("Individual");
   typesOut.push_back(TYPE_TUTORIAL);    namesOut.push_back("Tutorial");
}


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

#include "Structure.h"
#include "StringUtilities.h"

/**
 * constructor.
 */
Structure::Structure()
{
   structure = STRUCTURE_TYPE_INVALID;
}

/**
 * constructor.
 */
Structure::Structure(const STRUCTURE_TYPE ht)
{
   structure = ht;
}

/**
 * constructor.
 */
Structure::Structure(const QString& structureName)
{
   structure = convertStringToType(structureName);
}
      
/**
 * destructor.
 */
Structure::~Structure()
{
}

/**
 * operator equals.
 */
bool 
Structure::operator==(const Structure& st) const
{
   return (structure == st.structure);
}
      
/**
 * operator not equals.
 */
bool 
Structure::operator!=(const Structure& st) const
{
   return (structure != st.structure);
}
      
/**
 * get the structure type.
 */
Structure::STRUCTURE_TYPE 
Structure::getType() const
{
   return structure;
}

/**
 * get the structure type as a string.
 */
QString 
Structure::getTypeAsString() const
{
   return convertTypeToString(structure);
}

/**
 * get the structure type as a string.
 */
QString 
Structure::getTypeAsAbbreviatedString() const
{
   return convertTypeToAbbreviatedString(structure);
}

/**
 * set the structure type.
 */
void 
Structure::setType(const STRUCTURE_TYPE st)
{
   structure = st;
}

/**
 * set the structure type from a string (returns true if valid).
 */
bool 
Structure::setTypeFromString(const QString& hts)
{
   structure = convertStringToType(hts);
   return (structure != STRUCTURE_TYPE_INVALID);
}

/**
 * convert string to type.
 */
Structure::STRUCTURE_TYPE 
Structure::convertStringToType(const QString& sin)
{
   const QString s(StringUtilities::makeLowerCase(sin));
   
   STRUCTURE_TYPE st = STRUCTURE_TYPE_INVALID;
   
   if (s == getInvalidAsString()) {
      st = STRUCTURE_TYPE_INVALID;
   }
   else if ((s == getCortextLeftAsString()) ||
            (s == "left") ||
            (s == "l")) {
      st = STRUCTURE_TYPE_CORTEX_LEFT;
   }
   else if ((s == getCortexRightAsString()) ||
            (s == "right") ||
            (s == "r")) {
      st = STRUCTURE_TYPE_CORTEX_RIGHT;
   }
   else if ((s == getCerebellumAsString()) ||
            (s == "c")) {
      st = STRUCTURE_TYPE_CEREBELLUM;
   }
   else if ((s == getCortexBothAsString()) ||
            (s == "b")) {
      st = STRUCTURE_TYPE_CORTEX_BOTH;
   }
   
   return st;
}
      
/**
 * convert type to string.
 */
QString 
Structure::convertTypeToString(const STRUCTURE_TYPE st)
{
   QString s(getInvalidAsString());
   
   switch (st) {
      case STRUCTURE_TYPE_INVALID:
         s = getInvalidAsString();
         break;
      case STRUCTURE_TYPE_CORTEX_LEFT:
         s = getCortextLeftAsString();
         break;
      case STRUCTURE_TYPE_CORTEX_RIGHT:
         s = getCortexRightAsString();
         break;
      case STRUCTURE_TYPE_CORTEX_BOTH:
         s = getCortexBothAsString();
         break;
      case STRUCTURE_TYPE_CEREBELLUM:
         s = getCerebellumAsString();
         break;
   }
   
   return s;
}

/**
 * convert type to string.
 */
QString 
Structure::convertTypeToAbbreviatedString(const STRUCTURE_TYPE st)
{
   QString s("U");
   
   switch (st) {
      case STRUCTURE_TYPE_INVALID:
         s = "U";
         break;
      case STRUCTURE_TYPE_CORTEX_LEFT:
         s = "L";
         break;
      case STRUCTURE_TYPE_CORTEX_RIGHT:
         s = "R";
         break;
      case STRUCTURE_TYPE_CORTEX_BOTH:
         s = "LR";
         break;
      case STRUCTURE_TYPE_CEREBELLUM:
         s = "C";
         break;
   }
   
   return s;
}

/**
 * get all structure types and names (does not include invalid type).
 */
void 
Structure::getAllTypesAndNames(std::vector<STRUCTURE_TYPE>& types,
                               std::vector<QString>& names,
                               const bool includeInvalid)
{
   types.clear();
   names.clear();
   
   types.push_back(STRUCTURE_TYPE_CEREBELLUM);
   names.push_back(getCerebellumAsString());
   
   types.push_back(STRUCTURE_TYPE_CORTEX_LEFT);
   names.push_back(getCortextLeftAsString());
   
   types.push_back(STRUCTURE_TYPE_CORTEX_BOTH);
   names.push_back(getCortexBothAsString());
   
   types.push_back(STRUCTURE_TYPE_CORTEX_RIGHT);
   names.push_back(getCortexRightAsString());
   
   if (includeInvalid) {
      types.push_back(STRUCTURE_TYPE_INVALID);
      names.push_back(getInvalidAsString());
   }
   
}

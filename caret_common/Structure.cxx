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
   else if (s == getCerebrumAndCerebellumAsString()) {
      st = STRUCTURE_TYPE_CEREBRUM_CEREBELLUM;
   }
   else if (s == getCerebellumOrLeftCerebralAsString()) {
      st = STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT;
   }
   else if (s == getCerebellumOrRightCerebralAsString()) {
      st = STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT;
   }
   else if (s == getLeftCerebralOrCerebellumAsString()) {
      st = STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM;
   }
   else if (s == getRightCerebralOrCerebellumAsString()) {
      st = STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM;
   }
   else if (s == getSubCorticalAsString()) {
      st = STRUCTURE_TYPE_SUBCORTICAL;
   }
   else if (s == getAllAsString()) {
      st = STRUCTURE_TYPE_ALL;
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
      case STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
         s = getCerebrumAndCerebellumAsString();
         break;
      case STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
         s = getCerebellumOrLeftCerebralAsString();
         break;
      case STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
         s = getCerebellumOrRightCerebralAsString();
         break;
      case STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
         s = getLeftCerebralOrCerebellumAsString();
         break;
      case STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
         s = getRightCerebralOrCerebellumAsString();
         break;
      case STRUCTURE_TYPE_SUBCORTICAL:
         s = getSubCorticalAsString();
         break;
      case STRUCTURE_TYPE_ALL:
         s = getAllAsString();
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
      case STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
         s = "CC";
         break;
      case STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
         s = "CL";
         break;
      case STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
         s = "CR";
         break;
      case STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
         s = "LC";
         break;
      case STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
         s = "RC";
         break;
      case STRUCTURE_TYPE_SUBCORTICAL:
         s = "S";
         break;
      case STRUCTURE_TYPE_ALL:
         s = "A";
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
                               const bool includeInvalid,
                               const bool includeAmbiguousCerebralOrCerebellum)
{
   types.clear();
   names.clear();
   
   types.push_back(STRUCTURE_TYPE_CEREBELLUM);
   names.push_back(getCerebellumAsString());
   
   if (includeAmbiguousCerebralOrCerebellum) {
      types.push_back(STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT);
      names.push_back(getCerebellumOrLeftCerebralAsString());
      
      types.push_back(STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT);
      names.push_back(getCerebellumOrRightCerebralAsString());
   }
   
   types.push_back(STRUCTURE_TYPE_CORTEX_LEFT);
   names.push_back(getCortextLeftAsString());
   
   types.push_back(STRUCTURE_TYPE_CORTEX_BOTH);
   names.push_back(getCortexBothAsString());
   
   types.push_back(STRUCTURE_TYPE_CORTEX_RIGHT);
   names.push_back(getCortexRightAsString());
   
   types.push_back(STRUCTURE_TYPE_CEREBRUM_CEREBELLUM);
   names.push_back(getCerebrumAndCerebellumAsString());
   
   if (includeAmbiguousCerebralOrCerebellum) {
      types.push_back(STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM);
      names.push_back(getLeftCerebralOrCerebellumAsString());
      
      types.push_back(STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM);
      names.push_back(getRightCerebralOrCerebellumAsString());
   }
   
   types.push_back(STRUCTURE_TYPE_SUBCORTICAL);
   names.push_back(getSubCorticalAsString());

   types.push_back(STRUCTURE_TYPE_ALL);
   names.push_back(getAllAsString());

   if (includeInvalid) {
      types.push_back(STRUCTURE_TYPE_INVALID);
      names.push_back(getInvalidAsString());
   }
   
}

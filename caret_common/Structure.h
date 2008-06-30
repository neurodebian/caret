
#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__

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

/// class for structure information
class Structure {
   public:
      /// anatomical structure
      enum STRUCTURE_TYPE {
         /// left cerebral cortex
         STRUCTURE_TYPE_CORTEX_LEFT,
         /// right cerebral cortex
         STRUCTURE_TYPE_CORTEX_RIGHT,
         /// left and right cerebral cortex
         STRUCTURE_TYPE_CORTEX_BOTH,
         /// cerebrellum
         STRUCTURE_TYPE_CEREBELLUM,
         /// cerebellum or left cerebral cortex (used with foci that could be associated with either)
         STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT,
         /// cerebellum or right cerebral cortex (used with foci that could be associated with either)
         STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT,
         /// left cerebral cortex or cerebellum (used with foci that could be associated with either)
         STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM,
         /// right cerebral cortex or cerebellum (used with foci that could be associated with either)
         STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM,
         /// invalid
         STRUCTURE_TYPE_INVALID
      };
      
      // constructor
      Structure();
      
      // constructor
      Structure(const STRUCTURE_TYPE st);
      
      // constructor
      Structure(const QString& structureName);
      
      // operator equals
      bool operator==(const Structure& st) const;
      
      // operator not equals
      bool operator!=(const Structure& st) const;
      
      // destructor
      ~Structure();
      
      // get the structure type
      STRUCTURE_TYPE getType() const;
      
      // get the structure type as a string
      QString getTypeAsString() const;
      
      // get the type as a one or two character string
      QString getTypeAsAbbreviatedString() const;
      
      // is left cortex structure
      bool isLeftCortex() const { return (structure == STRUCTURE_TYPE_CORTEX_LEFT); }
      
      // is right cortex structure
      bool isRightCortex() const { return (structure == STRUCTURE_TYPE_CORTEX_RIGHT); }
      
      // is both cortex structure
      bool isBothCortex() const { return (structure == STRUCTURE_TYPE_CORTEX_BOTH); }
      
      // is cerebellum structure
      bool isCerebellum() const { return (structure == STRUCTURE_TYPE_CEREBELLUM); }
      
      // is cerebellum or left cerebral structure
      bool isCerebellumOrLeftCerebral() const { return ((structure == STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT) ||
                                                        (structure == STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM)); }
      
      // is cerebellum or right cerebral structure
      bool isCerebellumOrRightCerebral() const { return ((structure == STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT) ||
                                                         (structure == STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM)); }
      
      // is invalid structure
      bool isInvalid() const { return (structure == STRUCTURE_TYPE_INVALID); }
      
      // is valid structure
      bool isValid() const { return (structure != STRUCTURE_TYPE_INVALID); }
      
      // set the structure type
      void setType(const STRUCTURE_TYPE st);
      
      // set the structure type from a string (returns true if valid)
      bool setTypeFromString(const QString& hts);
      
      // convert string to type
      static STRUCTURE_TYPE convertStringToType(const QString& s);
      
      // convert type to string
      static QString convertTypeToString(const STRUCTURE_TYPE ht);
      
      // convert type to abbreviated string
      static QString convertTypeToAbbreviatedString(const STRUCTURE_TYPE ht);

      /// get the string value for left cortex structure
      static QString getCortextLeftAsString() { return "left"; }
      
      /// get the string value for right cortex structure
      static QString getCortexRightAsString() { return "right"; }
      
      /// get the string value for cerebellum structure
      static QString getCerebellumAsString() { return "cerebellum"; }
      
      /// get the string value for cerebellum or left cerebral structure
      static QString getCerebellumOrLeftCerebralAsString() { return "cerebellum_or_left_cerebral"; }
      
      /// get the string value for cerebellum or right cerebral structure
      static QString getCerebellumOrRightCerebralAsString() { return "cerebellum_or_right_cerebral"; }
      
      /// get the string value for cerebellum or left cerebral structure
      static QString getLeftCerebralOrCerebellumAsString() { return "left_cerebral_or_cerebellum"; }
      
      /// get the string value for cerebellum or right cerebral structure
      static QString getRightCerebralOrCerebellumAsString() { return "right_cerebral_or_cerebellum"; }
      
      /// get the string value for both (right&left) cortex structure
      static QString getCortexBothAsString() { return "both"; }
      
      /// get the string value for invalid structure
      static QString getInvalidAsString() { return "invalid"; }
      
      /// get all structure types and names
      static void getAllTypesAndNames(std::vector<STRUCTURE_TYPE>& types,
                                      std::vector<QString>& names,
                                      const bool includeInvalid,
                                      const bool includeAmbiguousCerebralOrCerebellum = false);
                                      
   protected:
      /// the structure
      STRUCTURE_TYPE structure;
};

#endif // __STRUCTURE_H__


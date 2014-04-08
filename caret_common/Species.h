
#ifndef __SPECIES_H__
#define __SPECIES_H__

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

/// defines valid species
class Species {
   public:
      /// type of species
      enum TYPE {
         /// unknown
         TYPE_UNKNOWN,
         /// baboon
         TYPE_BABOON,
         /// chimpanzee
         TYPE_CHIMPANZEE,
         /// ferret
         TYPE_FERRET,
         /// galago
         TYPE_GALAGO,
         /// gibbon
         TYPE_GIBBON,
         /// gorilla
         TYPE_GORILLA,
         /// human
         TYPE_HUMAN,
         /// macaque monkey
         TYPE_MACAQUE,
         /// marmoset
         TYPE_MARMOSET,
         /// mouse
         TYPE_MOUSE,
         /// orangutan
         TYPE_ORANGUTAN,
         /// rat
         TYPE_RAT,
         /// other
         TYPE_OTHER
      };
      
      // get a list of valid species types and names
      static void getAllSpeciesTypesAndNames(std::vector<TYPE>& speciesTypesOut,
                                             std::vector<QString>& speciesNamesOut);
      
      // constructor
      Species();
      
      // constructor
      Species(const TYPE t);
      
      // constructor
      Species(const QString& name);
      
      // destructor
      ~Species();
      
      /// equality operator
      bool operator==(const Species& ss) { return (type == ss.type); }
      
      /// inequality operator
      bool operator!=(const Species& ss) { return (type != ss.type); }
      
      // reset
      void reset();
      
      // get species name
      QString getName() const;
      
      /// get the species type
      TYPE getType() const { return type; }
      
      // set the species using type
      void setUsingType(const TYPE typeIn);
      
      // set the species using name
      void setUsingName(const QString& nameIn);
      
      // is human
      bool isHuman() const { return type == TYPE_HUMAN; }
      
      // is valid (not unknown)
      bool isValid() const { return type != TYPE_UNKNOWN; }
      
   protected:
      /// the species type
      TYPE type;
};

#endif // __SPECIES_H__


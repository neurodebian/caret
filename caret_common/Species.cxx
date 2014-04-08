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

#include "Species.h"

/**
 * get a list of valid species types and names.
 */
void 
Species::getAllSpeciesTypesAndNames(std::vector<TYPE>& speciesTypesOut,
                                    std::vector<QString>& speciesNamesOut)
{
   speciesTypesOut.clear(); speciesNamesOut.clear();

   speciesTypesOut.push_back(TYPE_UNKNOWN);    speciesNamesOut.push_back("Unknown");
   speciesTypesOut.push_back(TYPE_BABOON);     speciesNamesOut.push_back("Baboon");
   speciesTypesOut.push_back(TYPE_CHIMPANZEE); speciesNamesOut.push_back("Chimpanzee");
   speciesTypesOut.push_back(TYPE_FERRET);     speciesNamesOut.push_back("Ferret");
   speciesTypesOut.push_back(TYPE_GALAGO);     speciesNamesOut.push_back("Galago");
   speciesTypesOut.push_back(TYPE_GIBBON);     speciesNamesOut.push_back("Gibbon");
   speciesTypesOut.push_back(TYPE_GORILLA);    speciesNamesOut.push_back("Gorilla");
   speciesTypesOut.push_back(TYPE_HUMAN);      speciesNamesOut.push_back("Human");
   speciesTypesOut.push_back(TYPE_MACAQUE);    speciesNamesOut.push_back("Macaque");
   speciesTypesOut.push_back(TYPE_MARMOSET);    speciesNamesOut.push_back("Marmoset");
   speciesTypesOut.push_back(TYPE_MOUSE);      speciesNamesOut.push_back("Mouse");
   speciesTypesOut.push_back(TYPE_ORANGUTAN);  speciesNamesOut.push_back("Orangutan");
   speciesTypesOut.push_back(TYPE_RAT);        speciesNamesOut.push_back("Rat");
   speciesTypesOut.push_back(TYPE_OTHER);      speciesNamesOut.push_back("Other");
}

/**
 * constructor.
 */
Species::Species()
{
   reset();
}

/**
 * constructor.
 */
Species::Species(const TYPE t)
{
   reset();
   setUsingType(t);
}

/**
 * constructor.
 */
Species::Species(const QString& name)
{
   reset();
   setUsingName(name);
}

/**
 * destructor.
 */
Species::~Species()
{
}

/**
 * reset.
 */
void 
Species::reset()
{
   type = TYPE_UNKNOWN;
}
      
/**
 * get species name.
 */
QString 
Species::getName() const
{
   std::vector<TYPE> speciesTypes;
   std::vector<QString> speciesNames;
   getAllSpeciesTypesAndNames(speciesTypes, speciesNames);
   
   QString name("Unknown");
   const int num = static_cast<int>(speciesTypes.size());
   for (int i = 0; i < num; i++) {
      if (type == speciesTypes[i]) {
         name = speciesNames[i];
         break;
      }
   }
   
   return name;
}

/**
 * set the species using type.
 */
void 
Species::setUsingType(const TYPE typeIn)
{
   type = typeIn;
}

/**
 * set the species using name.
 */
void 
Species::setUsingName(const QString& nameIn)
{
   const QString name(nameIn.toLower());
   
   std::vector<TYPE> speciesTypes;
   std::vector<QString> speciesNames;
   getAllSpeciesTypesAndNames(speciesTypes, speciesNames);
   
   type = TYPE_UNKNOWN;
   const int num = static_cast<int>(speciesTypes.size());
   for (int i = 0; i < num; i++) {
      if (name == speciesNames[i].toLower()) {
         type = speciesTypes[i];
         break;
      }
   }
}
      

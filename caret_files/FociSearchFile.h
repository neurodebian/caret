
#ifndef __FOCI_SEARCH_FILE_H__
#define __FOCI_SEARCH_FILE_H__

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

#include "AbstractFile.h"

class FociProjectionFile;
class FociSearch;
class FociSearchSet;
class StudyMetaDataFile;

/// class for saving foci searches
class FociSearchFile : public AbstractFile {
   public:
      // constructor
      FociSearchFile();
      
      // copy constructor
      FociSearchFile(const FociSearchFile& fs);
      
      // destructor
      ~FociSearchFile();
      
      // assignment operator
      FociSearchFile& operator=(const FociSearchFile& fs);
      
      // append a file
      void append(const FociSearchFile& fsf);
      
      // add a default empty search
      void addDefaultSearch();
      
      // clear method.
      void clear();

      // clear foci searches
      void clearFociSearches();
      
      // copy a search set to a new search set
      // returns positive number of new search set, else -1 if error
      int copySearchSetToNewSearchSet(const int copySearchSetNumber);
      
      // returns true if the file is isEmpty (contains no data)
      bool empty() const;
      
      /// get number of search sets
      int getNumberOfFociSearchSets() const { return fociSearchSets.size(); }
      
      /// get a search set
      FociSearchSet* getFociSearchSet(const int indx);
      
      /// get a search set (const method)
      const FociSearchSet* getFociSearchSet(const int indx) const;
      
      // delete a search set
      void deleteFociSearchSet(const int indx);
      
      // add a search set
      void addFociSearchSet(FociSearchSet* fs);
      
      // insert a foci search set
      void insertFociSearchSet(FociSearchSet* fs,
                               const int afterIndex);
                            
   protected:
      // copy helper
      void copyHelper(const FociSearchFile& fs);

      // Read the contents of the file (header has already been read)
      void readFileData(QFile& file,
                        QTextStream& stream,
                        QDataStream& binStream,
                        QDomElement& rootElement) throw (FileException);

      // Write the file's data (header has already been written)
      void writeFileData(QTextStream& stream,
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);
                         
      /// the foci search sets
      std::vector<FociSearchSet*> fociSearchSets;

      // NOTE: If additional members are added, update "copyHelper()".
};

/// class for a set of searches
class FociSearchSet {
   public:
      /// search mode
      enum SEARCH_MODE {
         /// search all foci
         SEARCH_MODE_ALL_FOCI,
         /// search only those marked as searched
         SEARCH_MODE_DISPLAYED_FOCI
      };
      
      // constructor
      FociSearchSet();
      
      // copy constructor
      FociSearchSet(const FociSearchSet& fs);
      
      // destructor
      ~FociSearchSet();
      
      // assignment operator
      FociSearchSet& operator=(const FociSearchSet& fs);
      
      // clear the search set
      void clear();
      
      // set parent foci search file
      void setParentFociSearchFile(FociSearchFile* fsf);
      
      /// get number of searches
      int getNumberOfFociSearches() const { return fociSearches.size(); }
      
      /// get a search
      FociSearch* getFociSearch(const int indx) { return fociSearches[indx]; }
      
      /// get a search const method
      const FociSearch* getFociSearch(const int indx) const { return fociSearches[indx]; }
      
      // delete a search
      void deleteFociSearch(const int indx);
      
      // add a search
      void addFociSearch(FociSearch* fs);
      
      // insert a foci search
      void insertFociSearch(FociSearch* fs,
                            const int afterIndex);
                            
      /// get the name of the search
      QString getName() const { return name; }
      
      // set the name of the search
      void setName(const QString& s);
      
      // set the file's modified status
      void setModified();
      
      // called to read from an XML structure
      void readXML(QDomNode& node) throw (FileException);
      
      // called to write to an XML structure
      void writeXML(QDomDocument& xmlDoc,
                    QDomElement&  parentElement);
  
   protected:
      // copy helper
      void copyHelper(const FociSearchSet& fs);

      /// foci search file this search belongs to
      FociSearchFile* parentFociSearchFile;
      
      /// the foci searches
      std::vector<FociSearch*> fociSearches;

      /// name of search set
      QString name;
      
      // NOTE: If additional members are added, update "copyHelper()".
      
      
      /// tag for reading and writing XML
      static const QString tagFociSearchSet;

      /// tag for reading and writing XML
      static const QString tagFociSearchSetName;

      /// FociSearchFile is a friend
      friend class FociSearchFile;
};

#ifdef __FOCI_SEARCH_FILE_MAIN__
const QString FociSearchSet::tagFociSearchSet = "FociSearchSet";
const QString FociSearchSet::tagFociSearchSetName = "Name";
#endif // __FOCI_SEARCH_FILE_MAIN__

/// class for a single foci search
class FociSearch {
   public:
      /// logic for searching
      enum LOGIC {
         /// union
         LOGIC_UNION,
         /// intersection
         LOGIC_INTERSECTION
      };
      
      //
      // If the order in the ATTRIBUTE enumerated type changes,
      // FociSearch::getAttributeTypesAndNames() must be updated.
      //
      /// attribute for searching
      enum ATTRIBUTE {
         /// all attributes
         ATTRIBUTE_ALL,  // Must be first for BrainModelSurfaceFociSearch::applySearchToFocus
         /// focus area
         ATTRIBUTE_FOCUS_AREA, // IF MOVED update BrainModelSurfaceFociSearch::applySearchToFocus
         /// study authors
         ATTRIBUTE_STUDY_AUTHORS,
         /// study citation
         ATTRIBUTE_STUDY_CITATION,         
         /// focus class
         ATTRIBUTE_FOCUS_CLASS,
         /// focus comment
         ATTRIBUTE_FOCUS_COMMENT,
         /// study comment
         ATTRIBUTE_STUDY_COMMENT,
         /// study data format
         ATTRIBUTE_STUDY_DATA_FORMAT,
         /// study data type
         ATTRIBUTE_STUDY_DATA_TYPE,
         /// focus geography
         ATTRIBUTE_FOCUS_GEOGRAPHY,
         /// study keywords
         ATTRIBUTE_STUDY_KEYWORDS,
         /// study Medical Subject Headings terms
         ATTRIBUTE_STUDY_MESH_TERMS,
         /// study name
         ATTRIBUTE_STUDY_NAME,
         /// focus ROI
         ATTRIBUTE_FOCUS_ROI,
         /// focus spatial search
         ATTRIBUTE_FOCUS_SPATIAL,
         /// study species
         ATTRIBUTE_STUDY_SPECIES,
         /// focus structure
         ATTRIBUTE_FOCUS_STRUCTURE,
         /// study stereotaxic space
         ATTRIBUTE_STUDY_STEREOTAXIC_SPACE,
         /// study table header
         ATTRIBUTE_STUDY_TABLE_HEADER,
         /// study table sub-header
         ATTRIBUTE_STUDY_TABLE_SUBHEADER,
         /// study title
         ATTRIBUTE_STUDY_TITLE,
         /// number of attributes (NOT USED IN SEARCH)
         ATTRIBUTE_NUMBER_OF
      };
      
      /// matching for searches
      enum MATCHING {
         /// matching 
         MATCHING_ANY_OF,
         /// matching 
         MATCHING_ALL_OF,
         /// matching 
         MATCHING_NONE_OF,
         /// matching 
         MATCHING_EXACT_PHRASE
      };
      
      // constructor
      FociSearch();
      
      // copy constructor
      FociSearch(const FociSearch& fs);
      
      // destructor
      ~FociSearch();
      
      // assignment operator
      FociSearch& operator=(const FociSearch& fs);
      
      // set parent foci search set
      void setParentFociSearchSet(FociSearchSet* fss);
      
      /// get the logic for the search
      LOGIC getLogic() const { return logic; }
      
      // set the logic for the search
      void setLogic(const LOGIC lg);
      
      // convert a logic type to name
      static QString convertLogicTypeToName(const LOGIC lg);
      
      // convert a logic name to type
      static LOGIC convertLogicNameToType(const QString& s);
       
      /// get the attribute
      ATTRIBUTE getAttribute() const { return attribute; }
      
      // set the attribute
      void setAttribute(const ATTRIBUTE a);
      
      // convert an attribute type to name
      static QString convertAttributeTypeToName(const ATTRIBUTE att);
      
      // convert an attribute name to type
      static ATTRIBUTE convertAttributeNameToType(const QString& s);
       
      /// get the matching
      MATCHING getMatching() const { return matching; }
      
      // set the matching
      void setMatching(const MATCHING m);
      
      // convert a matching type to name
      static QString convertMatchingTypeToName(const MATCHING m);
      
      // convert a matching name to type
      static MATCHING convertMatchingNameToType(const QString& s);
       
      /// get the search text
      QString getSearchText() const { return searchText; }
      
      /// set the search text
      void setSearchText(const QString& st);
      
      // get types and names for logic
      static void getLogicTypesAndNames(std::vector<LOGIC>& typesOut,
                                        std::vector<QString>& namesOut);

      // get types and names for attributes
      static void getAttributeTypesAndNames(std::vector<ATTRIBUTE>& typesOut,
                                            std::vector<QString>& namesOut);

      // get types and names for matching
      static void getMatchingTypesAndNames(std::vector<MATCHING>& typesOut,
                                           std::vector<QString>& namesOut);

      // called to read from an XML structure
      void readXML(QDomNode& node) throw (FileException);
      
      // called to write to an XML structure
      void writeXML(QDomDocument& xmlDoc,
                    QDomElement&  parentElement);
  
   protected:
      // copy helper
      void copyHelper(const FociSearch& fs);

      // set the file's modified status
      void setModified();
      
      /// foci search file this search belongs to
      FociSearchSet* parentFociSearchSet;
      
      /// the search logic
      LOGIC logic;
      
      /// the attribute to search
      ATTRIBUTE attribute;
      
      /// the type of matching
      MATCHING matching;
      
      /// the text for search
      QString searchText;
      
      // NOTE: If additional members are added, update "copyHelper()".



      /// tag for reading and writing XML
      static const QString tagFociSearch;
      /// tag for reading and writing XML
      static const QString tagFociSearchLogic;
      /// tag for reading and writing XML
      static const QString tagFociSearchAttribute;
      /// tag for reading and writing XML
      static const QString tagFociSearchMatching;
      /// tag for reading and writing XML
      static const QString tagFociSearchText;
      
      friend class FociSearchSet;
};

#endif // __FOCI_SEARCH_FILE_H__

#ifdef __FOCI_SEARCH_FILE_MAIN__
const QString FociSearch::tagFociSearch = "FociSearch";
const QString FociSearch::tagFociSearchLogic = "Logic";
const QString FociSearch::tagFociSearchAttribute = "Attribute";
const QString FociSearch::tagFociSearchMatching = "Matching";
const QString FociSearch::tagFociSearchText = "Text";
#endif // __FOCI_SEARCH_FILE_MAIN__

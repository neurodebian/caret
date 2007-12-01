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


#ifndef __CELL_FILE_H__
#define __CELL_FILE_H__

#include "AbstractFile.h"
#include "CellBase.h"
#include "CellClass.h"
#include "CellStudyInfo.h"
#include "SpecFile.h"

class CellFile;
class ColorFile;
class QDomDocument;
class QDomElement;
class QDomNode;
class TransformationMatrix;

/// This class holds a single cell
class CellData : public CellBase {
   public: 
      /// Constructor
      CellData(const QString& nameIn,
               const float xIn, const float yIn, const float zIn,
               const int sectionIn = 0,
               const QString& classNameIn = "", const int studyNumberIn = -1,
               const int colorIndexIn = -1);
       
      /// Constructor
      CellData();
               
      /// Destructor
      virtual ~CellData();
      
      /// Set the cell's class name
      void setClassName(const QString& name);
      
      /// called to read from an XML structure
      virtual void readXML(QDomNode& node) throw (FileException);
      
      /// called to write to an XML structure
      virtual void writeXML(QDomDocument& xmlDoc,
                            QDomElement&  parentElement,
                            const int cellNumber);
                    
      /// initialize the cell
      void initialize();
      
   protected:
      /// called when this cell is modified
      void setModified();
      
      /// Cell File this cell belongs to
      CellFile* cellFile;
      
      /// tag for reading and writing cells
      static const QString tagCellData;
      
      /// tag for reading and writing cells
      static const QString tagCellNumber;
      
      /// tag for reading and writing cells
      static const QString tagClassName;
      
      friend class CellFile;
      friend class CellFileProjector;
};

#ifdef __CELL_DATA_MAIN__
      const QString CellData::tagCellData = "CellData";
      const QString CellData::tagCellNumber = "cellNumber";
      const QString CellData::tagClassName = "className";
#endif // __CELL_DATA_MAIN__

/// Class for stroring cells
class CellFile : public AbstractFile {
   private:
      static const QString tagFileVersion;
      static const QString tagNumberOfCells;
      static const QString tagNumberOfComments;
      static const QString tagCommentUrl;
      static const QString tagCommentKeyWords;
      static const QString tagCommentTitle;
      static const QString tagCommentAuthors;
      static const QString tagCommentCitation;
      static const QString tagCommentStereotaxicSpace;

      /// The Cells
      std::vector<CellData> cells;
      
      /// studies for the cells
      std::vector<CellStudyInfo> studyInfo;
            
      /// The cell classes
      std::vector<CellClass> cellClasses;
      
      /// Add a cell class
      int addCellClass(const QString& className);
      
      /// read the version 0 cell file
      void readFileVersion0(QTextStream & stream, const QString& lineIn) throw (FileException);
      
      /// read the version 1 cell file
      void readFileVersion1(QTextStream& stream, const int numCells,
                                                const int numStudyInfo) throw (FileException);

      /// read the version 2 cell file
      void readFileVersion2(QFile& file, QTextStream& stream, 
                            const int numCells) throw (FileException);

      /// read a cell file's data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                        QDomElement& /* rootElement */) throw (FileException);
      
      /// write a cell file's data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                         QDomElement& /* rootElement */) throw (FileException);
      
      /// write a version 1 cell file's data
      void writeFileVersion1(QTextStream& stream) throw (FileException);
      
      /// write a version 2 cell file's data
      void writeFileVersion2(QTextStream& stream) throw (FileException);
      
   public:
   
      /// constructor 
      CellFile(const QString& descriptiveName = "Cell File",
               const QString& defaultExtensionIn = SpecFile::getCellFileExtension());
      
      /// destructor
      virtual ~CellFile();
      
      /// clear cells
      void clear();
      
      /// compare a file for unit testing (returns true if "within tolerance")
      bool compareFileForUnitTesting(const AbstractFile* af,
                                     const float tolerance,
                                     QString& messageOut) const;
                                     
      /// add a cell
      void addCell(const CellData& cd);
      
      /// append the contents of a cell file to this file
      void append(CellFile& cf);
      
      /// apply a transformation matrix to the cells
      void applyTransformationMatrix(const int sectionLow, 
                                     const int sectionHigh,
                                     const float matrix[16],
                                     const bool onlySpecialCells);

      /// apply a transformation matrix to the cells
      void applyTransformationMatrix(const int sectionLow, 
                                     const int sectionHigh,
                                     const TransformationMatrix& matrix,
                                     const bool onlySpecialCells);

      /// apply a transformation matrix to the cells
      void applyTransformationMatrix(const TransformationMatrix& matrix,
                                     const bool onlySpecialCells = false);

      /// set the special flag for all cells within the section numbers and bounds
      void setSpecialFlags(const int sectionLow,
                           const int sectionHigh,
                           const float bounds[4]);
                           
      /// assign colors to these cells
      void assignColors(const ColorFile& cf, 
                        const CellBase::CELL_COLOR_MODE colorMode);

      /// clear all special flags
      void clearAllSpecialFlags();
      
      /// delete a cell
      void deleteCell(const int indexNumber);
      
      /// get number of cell classes
      int getNumberOfCellClasses() const { return cellClasses.size(); }
      
      /// get cell class index by its name
      int getCellClassIndexByName(const QString& name) const;

      /// get cell class selected by index
      bool getCellClassSelectedByIndex(const int index) const;
      
      /// set cell class selected by index
      void setCellClassSelectedByIndex(const int index, const bool sel);
      
      /// get cell class selected by name
      bool getCellClassSelectedByName(const QString& name) const;
      
      /// get cell class name
      QString getCellClassNameByIndex(const int index) const;
      
      /// set the status of all cell classes
      void setAllCellClassStatus(const bool selected);
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfCells() == 0); }
      
      /// get pointer to a cell
      CellData* getCell(const int cellNumber);
      
      /// get pointer to a cell (const method)
      const CellData* getCell(const int cellNumber) const;
      
      /// get the number of cells
      int getNumberOfCells() const { return cells.size(); }
      
      /// get number of study info
      int getNumberOfStudyInfo() const { return studyInfo.size(); } 
      
      /// get the study info index from the study info's value
      int getStudyInfoIndexFromValue(const CellStudyInfo& studyInfo) const;
      
      /// get a study info
      const CellStudyInfo* getStudyInfo(const int index) const;
      
      /// get a study info
      CellStudyInfo* getStudyInfo(const int index);
      
      /// add a study info
      int addStudyInfo(const CellStudyInfo& studyInfo);
      
      /// delete all study info and clear links to study info
      void deleteAllStudyInfo();
      
      /// delete study info
      void deleteStudyInfo(const int indx);
      
      /// set a study info
      void setStudyInfo(const int index, const CellStudyInfo& studyInfo);
            
      /// find out if comma separated file conversion supported
      virtual void getCommaSeparatedFileSupport(bool& readFromCSV,
                                                bool& writeToCSV) const;
                                        
      /// write the file's data into a comma separated values file (throws exception if not supported)
      virtual void writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException);
      
      /// read the file's data from a comma separated values file (throws exception if not supported)
      virtual void readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException);
      
      
   friend class CellData;
};

#ifdef __CELL_FILE_MAIN__
   const QString CellFile::tagFileVersion = "tag-version";
   const QString CellFile::tagNumberOfCells = 
                                         "tag-number-of-cells";
   const QString CellFile::tagNumberOfComments = "tag-number-of-comments";
   const QString CellFile::tagCommentUrl = "tag-url";
   const QString CellFile::tagCommentKeyWords = "tag-key-words";
   const QString CellFile::tagCommentTitle = "tag-title";
   const QString CellFile::tagCommentAuthors = "tag-authors";
   const QString CellFile::tagCommentCitation = "tag-citation";
   const QString CellFile::tagCommentStereotaxicSpace = "tag-space";
#endif // __CELL_FILE_MAIN__

#endif // __CELL_FILE_H__

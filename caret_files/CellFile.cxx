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

#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>

#define __CELL_FILE_MAIN__
#define __CELL_DATA_MAIN__
#include "CellFile.h"
#undef __CELL_DATA_MAIN__
#undef __CELL_FILE_MAIN__

#include "CellStudyInfo.h"
#include "ColorFile.h"
#include "CommaSeparatedValueFile.h"
#include "DebugControl.h"
#include "StringTable.h"
#include "StringUtilities.h"
#include "TransformationMatrixFile.h"

/**
 * Initialize a cell.
 */
void
CellData::initialize()
{
   CellBase::initialize();
   cellFile = NULL;
}

/**
 * Constructor.
 */
CellData::CellData(const QString& nameIn,
                   const float xIn, const float yIn, const float zIn,
                   const int sectionIn,
                   const QString& classNameIn, const int studyNumberIn,
                   const int colorIndexIn)
{
   initialize();
   setName(nameIn);
   setXYZ(xIn, yIn, zIn);
   setSearchXYZ(0.0, 0.0, 0.0);
   setSectionNumber(sectionIn);
   className = classNameIn;
   //if (className.isEmpty()) {
   //   className = "???";
   //}
   setStudyNumber(studyNumberIn);
   setColorIndex(colorIndexIn);
}
         
/**
 * Constructor.
 */
CellData::CellData()
{
   initialize();
}
         
/**
 * Destructor.
 */
CellData::~CellData()
{
}

/**
 * called when this cell is modified.
 */
void 
CellData::setModified()
{
   if (cellFile != NULL) {
      cellFile->setModified();
   }
}
      
/**
 * Set the cell's class name
 */
void
CellData::setClassName(const QString& name)
{
   className = name;
   if (cellFile != NULL) {
      classIndex = cellFile->addCellClass(name);
   }
   else {
      classIndex = -1;  // 0;
   }
   setModified();
}

/**
 * called to read from an XML structure.
 */
void 
CellData::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != tagCellData) {
      QString msg("Incorrect element type passed to CellData::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == tagCellNumber) {
         }
         else if (elem.tagName() == tagClassName) {
            className = AbstractFile::getXmlElementFirstChildAsString(elem);
            if (className == "???") {
               className = "";
            }
         }
         else if (elem.tagName() == tagCellBase) {
            CellBase::readXMLWithDOM(node);
         }
         else {
            std::cout << "WARNING: unrecognized CellData element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      node = node.nextSibling();
   }
}
      
/**
 * called to write to an XML structure.
 */
void 
CellData::writeXML(QDomDocument& xmlDoc,
                   QDomElement&  parentElement,
                   const int cellNumber)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement cellDataElement = xmlDoc.createElement(CellData::tagCellData);

   //
   // cell number
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellDataElement, 
                                   tagCellNumber, cellNumber);
   
   //
   // name of cell
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellDataElement, tagClassName, className);
   
   //
   // Write the base class' data
   //
   CellBase::writeXML(xmlDoc, cellDataElement);

   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(cellDataElement);
}

//
//-------------------------------------------------------------------------------------------
//

/**
 * Constructor.
 */
CellFile::CellFile(const QString& descriptiveName,
               const QString& defaultExtensionIn)
   : AbstractFile(descriptiveName, 
                  defaultExtensionIn,
                  true,
                  FILE_FORMAT_XML,
                  FILE_IO_READ_ONLY,
                  FILE_IO_NONE,
                  FILE_IO_READ_AND_WRITE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_READ_AND_WRITE)
{
   clear();
}

/** 
 * Destructor.
 */
CellFile::~CellFile()
{
   clear();
}

/**
 * Clear the cell file.
 */
void 
CellFile::clear()
{
   clearAbstractFile();
   cells.clear();
   studyInfo.clear();
   cellClasses.clear();
   //addCellClass("???");
}

/**
 * Append a cell file to this cell file.
 */
void
CellFile::append(CellFile& cf)
{
   const int numCells = cf.getNumberOfCells();
   const int origNumberOfStudyInfo = getNumberOfStudyInfo();
   
   //
   // Transfer the cells
   //
   for (int i = 0; i < numCells; i++) {
      CellData* cd = cf.getCell(i);
      //
      // Update study info indexes
      //
      if (cd->studyNumber >= 0) {
         cd->studyNumber += origNumberOfStudyInfo;
      }
      addCell(*cd);
   }
   
   //
   // Transfer the study info
   //
   for (int j = 0; j < cf.getNumberOfStudyInfo(); j++) {
      addStudyInfo(*(cf.getStudyInfo(j)));
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(cf);

   setModified();
}

/**
 * Add a cell to the cell file.
 */
void 
CellFile::addCell(const CellData& cd)
{
   cells.push_back(cd);
   const int index = static_cast<int>(cells.size()) - 1;
   cells[index].cellFile = this;
   cells[index].classIndex = addCellClass(cd.className);
   setModified();
}

/**
 * set the special flag for all cells in the section range and box
 */
void 
CellFile::setSpecialFlags(const int sectionLow,
                             const int sectionHigh,
                             const float bounds[4])
{
   const int num = getNumberOfCells();
   
   const float minX = std::min(bounds[0], bounds[2]);
   const float maxX = std::max(bounds[0], bounds[2]);
   const float minY = std::min(bounds[1], bounds[3]);
   const float maxY = std::max(bounds[1], bounds[3]);

   for (int i = 0; i < num; i++) {
      if ((cells[i].sectionNumber >= sectionLow) &&
          (cells[i].sectionNumber <= sectionHigh)) {
         const float x = cells[i].xyz[0];
         const float y = cells[i].xyz[1];
         
         if ((x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY)) {
            cells[i].specialFlag = true;
         }
      }
   }            
}

/**
 * Apply a transformation matrix to the cells.
 */
void 
CellFile::applyTransformationMatrix(const int sectionLow, 
                                    const int sectionHigh,
                                    const float matrix[16],
                                    const bool onlySpecialCells)
{
   const int num = getNumberOfCells();
   
   for (int i = 0; i < num; i++) {
      if ((cells[i].sectionNumber >= sectionLow) &&
          (cells[i].sectionNumber <= sectionHigh)) {
         bool transformIt = true;
         if (onlySpecialCells && (cells[i].specialFlag == false)) {
            transformIt = false;
         }

         if (transformIt) {         
            const float x = cells[i].xyz[0];
            const float y = cells[i].xyz[1];
            const float z = cells[i].xyz[2];

            cells[i].xyz[0] = x*matrix[0]
                       + y*matrix[4]
                       + z*matrix[8]
                       + matrix[12];
            cells[i].xyz[1] = x*matrix[1]
                       + y*matrix[5]
                       + z*matrix[9]
                       + matrix[13];
            cells[i].xyz[2] = x*matrix[2]
                       + y*matrix[6]
                       + z*matrix[10]
                       + matrix[14];
         }
      }
   }
   setModified();
}

/**
 * apply a transformation matrix to the cells.
 */
void 
CellFile::applyTransformationMatrix(const TransformationMatrix& matrix,
                                    const bool onlySpecialCells)
{
   applyTransformationMatrix(std::numeric_limits<int>::min(),
                             std::numeric_limits<int>::max(),
                             matrix,
                             onlySpecialCells);
}

/**
 * Apply a transformation matrix to the cells.
 */
void 
CellFile::applyTransformationMatrix(const int sectionLow, 
                                    const int sectionHigh,
                                    const TransformationMatrix& matrixIn,
                                    const bool onlySpecialCells)
{
   TransformationMatrix& matrix = const_cast<TransformationMatrix&>(matrixIn);
   const int num = getNumberOfCells();
   
   for (int i = 0; i < num; i++) {
      if ((cells[i].sectionNumber >= sectionLow) &&
          (cells[i].sectionNumber <= sectionHigh)) {
         bool transformIt = true;
         if (onlySpecialCells && (cells[i].specialFlag == false)) {
            transformIt = false;
         }

         if (transformIt) {         
            double p[4] = { cells[i].xyz[0], cells[i].xyz[1], cells[i].xyz[2], 1.0 };
            matrix.multiplyPoint(p);
            cells[i].xyz[0] = p[0];
            cells[i].xyz[1] = p[1];
            cells[i].xyz[2] = p[2];
         }
      }
   }
   setModified();
}

/**
 * Assign colors to these cells.
 */
void 
CellFile::assignColors(const ColorFile& colorFile, 
                       const CellBase::CELL_COLOR_MODE colorMode)
{
   const int numCells = getNumberOfCells();
   for (int i = 0; i < numCells; i++) {
      CellData* cd = getCell(i);
      bool match;
      switch (colorMode) {
         case CellBase::CELL_COLOR_MODE_NAME:
            cd->setColorIndex(colorFile.getColorIndexByName(cd->getName(), match));
            break;
         case CellBase::CELL_COLOR_MODE_CLASS:
            cd->setColorIndex(colorFile.getColorIndexByName(cd->getClassName(), match));
            break;
      }
   }
}

/**
 * Clear all of the cell's special flags.
 */
void
CellFile::clearAllSpecialFlags()
{
   const int numCells = getNumberOfCells();
   for (int i = 0; i < numCells; i++) {
      cells[i].specialFlag = false;
   }
}

/**
 * Delete a cell.
 */
void 
CellFile::deleteCell(const int indexNumber)
{
   const int newSize = static_cast<int>(cells.size()) - 1;
   for (int i = indexNumber; i < newSize; i++) {
      cells[i] = cells[i+1];
   }
   cells.resize(newSize);
   setModified();
}

/**
 * Get a cell.
 */
CellData* CellFile::getCell(const int cellNumber)
{
   return &cells[cellNumber];
}

/**
 * Get a cell (const method).
 */
const CellData* 
CellFile::getCell(const int cellNumber) const
{
   return &cells[cellNumber];
}

/**
 * Get the study info index based upon the study info's value.
 */
int 
CellFile::getStudyInfoIndexFromValue(const CellStudyInfo& csi) const
{
   const int num = getNumberOfStudyInfo();
   for (int i = 0; i < num; i++) {
      if ((*getStudyInfo(i)) == csi) {
         return i;
      }
   }
   return -1;
}


/**
 * Get a study info.
 */
const CellStudyInfo*
CellFile::getStudyInfo(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      return &studyInfo[indx];
   }
   return NULL;
}

/**
 * Get a study info.
 */
CellStudyInfo*
CellFile::getStudyInfo(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      return &studyInfo[indx];
   }
   return NULL;
}

/**
 * Add a study info.
 */
int
CellFile::addStudyInfo(const CellStudyInfo& csi)
{
   studyInfo.push_back(csi);
   const int index = studyInfo.size() - 1;
   return index;
}

/**
 * delete all study info and clear links to study info
 */
void 
CellFile::deleteAllStudyInfo()
{
   const int num = getNumberOfCells();
   for (int i = 0; i < num; i++) {
      CellData* cd = getCell(i);
      cd->setStudyNumber(-1);
   }
   studyInfo.clear();
}
      
/**
 * delete study info.
 */
void 
CellFile::deleteStudyInfo(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      for (int i = 0; i < getNumberOfCells(); i++) {
         const int studyNum = cells[i].getStudyNumber();
         if (studyNum == indx) {
            cells[i].setStudyNumber(-1);
         }
         else if (studyNum > indx) {
            cells[i].setStudyNumber(studyNum - 1);
         }
      }
      
      studyInfo.erase(studyInfo.begin() + indx);
   }
}
      
/**
 * Set a study info.
 */
void
CellFile::setStudyInfo(const int index, const CellStudyInfo& csi)
{
   studyInfo[index] = csi;
}

/**
 * get cell class selected by index.
 */
bool 
CellFile::getCellClassSelectedByIndex(const int index) const
{
   if (index < getNumberOfCellClasses()) {
      return cellClasses[index].selected;
   }
   return false;
}

/**
 * Set cell class selected by index.
 */
void
CellFile::setCellClassSelectedByIndex(const int index, const bool sel)
{
   if (index < getNumberOfCellClasses()) {
      cellClasses[index].selected = sel;
   }
}

/**
 * Get a class index from its name
 */
int
CellFile::getCellClassIndexByName(const QString& name) const
{
   const int numClasses = getNumberOfCellClasses();
   for (int i = 0; i < numClasses; i++) {
      if (name == cellClasses[i].name) {
         return i;
      }
   }
   return -1;
}

/**
 * get cell class selected by name.
 */
bool 
CellFile::getCellClassSelectedByName(const QString& name) const
{
   const int index = getCellClassIndexByName(name);
   if (index >= 0) {
      return cellClasses[index].selected;
   }
   return false;
}

/**
 * get cell class name.
 */
QString 
CellFile::getCellClassNameByIndex(const int index) const
{
   if (index < getNumberOfCellClasses()) {
      return cellClasses[index].name;
   }
   return ""; // "???";
}

/**
 * Add a cell class (if class already exists its selection status
 * is set to true).
 */
int
CellFile::addCellClass(const QString& className) 
{
   if (className.isEmpty()) {
      return -1;
   }
   
   const int index = getCellClassIndexByName(className);
   if (index >= 0) {
      cellClasses[index].selected = true;
      return index;
   }
   
   cellClasses.push_back(CellClass(className));
   return (getNumberOfCellClasses() - 1);
}

/**
 * set the status of all cell classes.
 */
void 
CellFile::setAllCellClassStatus(const bool selected)
{
   const int numClasses = getNumberOfCellClasses();
   for (int i = 0; i < numClasses; i++) {
      cellClasses[i].selected = selected;
   }
}
      
/**
 * compare a file for unit testing (returns true if "within tolerance").
 */
bool 
CellFile::compareFileForUnitTesting(const AbstractFile* af,
                                    const float tolerance,
                                    QString& messageOut) const
{
   messageOut = "";
   const CellFile* cf = dynamic_cast<const CellFile*>(af);
   if (cf == NULL) {
      messageOut = "File for comparison is not a Cell/Foci File";
      return false;
   }
   
   const int numCells = getNumberOfCells();
   if (numCells != cf->getNumberOfCells()) {
      messageOut = "Number of cells does not match.";
   }
   
   for (int i = 0; i < numCells; i++) {
      const CellData* cd1 = getCell(i);
      const CellData* cd2 = cf->getCell(i);
      if (cd1->getName() != cd2->getName()) {
         messageOut = "Cells " + QString::number(i) + " have a different name.";
         return false;
      }
      
      const float* xyz1 = cd1->getXYZ();
      const float* xyz2 = cd2->getXYZ();
      for (int j = 0; j < 3; j++) {
         if (std::fabs(xyz1[j] - xyz2[j]) > tolerance) {
            messageOut = "Cells " + QString::number(i) + " coordinates do no match.";
            return false;
         }
      }
   }
   
   return true;
}

/**
 * find out if comma separated file conversion supported.
 */
void 
CellFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                       bool& writeToCSV) const
{
   readFromCSV = true;
   writeToCSV  = true;
}
                                        
/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
CellFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException)
{
   csv.clear();
   
   const int numCells = getNumberOfCells();
   if (numCells <= 0) {
      return;
   }
   
   //
   // Column numbers for data
   //
   int numCols = 0;
   const int cellNumberCol = numCols++;
   const int xCol = numCols++;
   const int yCol = numCols++;
   const int zCol = numCols++;
   const int sectionNumberCol = numCols++;
   const int nameCol = numCols++;
   const int studyNumberCol = numCols++;
   const int geographyCol = numCols++;
   const int areaCol = numCols++;
   const int sizeCol = numCols++;
   const int statisticCol = numCols++;
   const int commentCol = numCols++;
   const int structureCol = numCols++;
   const int classNameCol = numCols++;
   const int sumsIDNumberCol = numCols++;
   const int sumsRepeatNumberCol = numCols++;
   const int sumsParentCellBaseIDCol = numCols++;
   
   const int studyMetaPubMedCol = numCols++;
   const int studyMetaTableCol = numCols++;
   const int studyMetaTableSubHeaderCol = numCols++;
   const int studyMetaFigureCol = numCols++;
   const int studyMetaFigurePanelCol = numCols++;
   const int studyMetaPageNumberCol = numCols++;
   
   //
   // Create and add to string table
   //
   StringTable* ct = new StringTable(numCells, numCols, "Cells");
   ct->setColumnTitle(cellNumberCol, "Cell Number");
   ct->setColumnTitle(xCol, "X");
   ct->setColumnTitle(yCol, "Y");
   ct->setColumnTitle(zCol, "Z");
   ct->setColumnTitle(sectionNumberCol, "Section");
   ct->setColumnTitle(nameCol, "Name");
   ct->setColumnTitle(studyNumberCol, "Study Number");
   ct->setColumnTitle(geographyCol, "Geography");
   ct->setColumnTitle(areaCol, "Area");
   ct->setColumnTitle(sizeCol, "Size");
   ct->setColumnTitle(statisticCol, "Statistic");
   ct->setColumnTitle(commentCol, "Comment");
   ct->setColumnTitle(structureCol, "Structure");
   ct->setColumnTitle(classNameCol, "Class Name");
   ct->setColumnTitle(sumsIDNumberCol, "SuMS ID Number");
   ct->setColumnTitle(sumsRepeatNumberCol, "SuMS Repeat Number");
   ct->setColumnTitle(sumsParentCellBaseIDCol, "SuMS Parent Cell Base ID");
   ct->setColumnTitle(studyMetaPubMedCol, "Study PubMed ID");
   ct->setColumnTitle(studyMetaTableCol, "Study Table Number");
   ct->setColumnTitle(studyMetaTableSubHeaderCol, "Study Table Subheader");
   ct->setColumnTitle(studyMetaFigureCol, "Study Figure Number");
   ct->setColumnTitle(studyMetaFigurePanelCol, "Study Figure Panel");
   ct->setColumnTitle(studyMetaPageNumberCol, "Study Page Number");
   
   for (int i = 0; i < numCells; i++) {
      const CellData* cd = getCell(i);
      const float* xyz = cd->getXYZ();
      ct->setElement(i, cellNumberCol, i);
      ct->setElement(i, xCol, xyz[0]);
      ct->setElement(i, yCol, xyz[1]);
      ct->setElement(i, zCol, xyz[2]);
      ct->setElement(i, sectionNumberCol, cd->getSectionNumber());
      ct->setElement(i, nameCol, cd->getName());
      ct->setElement(i, studyNumberCol, cd->getStudyNumber());
      ct->setElement(i, geographyCol, cd->getGeography());
      ct->setElement(i, areaCol, cd->getArea());
      ct->setElement(i, sizeCol, cd->getSize());
      ct->setElement(i, statisticCol, cd->getStatistic());
      ct->setElement(i, commentCol, cd->getComment());
      ct->setElement(i, structureCol, Structure::convertTypeToString(cd->getCellStructure()));
      ct->setElement(i, classNameCol, cd->getClassName());
      ct->setElement(i, sumsIDNumberCol, cd->getSumsIDNumber());
      ct->setElement(i, sumsRepeatNumberCol, cd->getSumsRepeatNumber());
      ct->setElement(i, sumsParentCellBaseIDCol, cd->getSumsParentCellBaseID());
      
      const StudyMetaDataLinkSet smdls = cd->getStudyMetaDataLinkSet();
      StudyMetaDataLink smdl;
      if (smdls.getNumberOfStudyMetaDataLinks() > 1) {
         const QString msg("Cell[" 
                           + QString::number(i)
                           + "] named \""
                           + cd->getName()
                           + "\" has more than one Study Metadata Link so it "
                             "cannot be written as a Comma Separated Value File");
         throw FileException(msg);
      }
      else if (smdls.getNumberOfStudyMetaDataLinks() == 1) {
         smdl = smdls.getStudyMetaDataLink(0);
      }
      ct->setElement(i, studyMetaPubMedCol, smdl.getPubMedID());
      ct->setElement(i, studyMetaTableCol, smdl.getTableNumber());
      ct->setElement(i, studyMetaTableSubHeaderCol, smdl.getTableSubHeaderNumber());
      ct->setElement(i, studyMetaFigureCol, smdl.getFigureNumber());
      ct->setElement(i, studyMetaFigurePanelCol, smdl.getFigurePanelNumberOrLetter());
      ct->setElement(i, studyMetaPageNumberCol, smdl.getPageNumber());
   }

   //
   // Create table of study info
   //
/*
   const int numStudyInfo = getNumberOfStudyInfo();
   for (int i = 0; i < numStudyInfo; i++) {
sdfstudyInfo[i].writeXML(xmlDoc, rootElement, i);
   }
*/

   StringTable* headerTable = new StringTable(0, 0);
   writeHeaderDataIntoStringTable(*headerTable);
   csv.addDataSection(headerTable);
   csv.addDataSection(ct);
   StringTable* studyInfoTable = new StringTable(0, 0);
   CellStudyInfo::writeDataIntoStringTable(studyInfo, *studyInfoTable);
   csv.addDataSection(studyInfoTable);
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
CellFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException)
{
   clear();
   
   const StringTable* ct = csv.getDataSectionByName("Cells");   
   if (ct == NULL) {
      throw FileException("No cells found");
   }
   
   int numCols = ct->getNumberOfColumns();
   
   int cellNumberCol = -1;
   int xCol = -1;
   int yCol = -1;
   int zCol = -1;
   int sectionNumberCol = -1;
   int nameCol = -1;
   int studyNumberCol = -1;
   int geographyCol = -1;
   int areaCol = -1;
   int sizeCol = -1;
   int statisticCol = -1;
   int commentCol = -1;
   int structureCol = -1;
   int classNameCol = -1;
   int sumsIDNumberCol = -1;
   int sumsRepeatNumberCol = -1;
   int sumsParentCellBaseIDCol = -1;
   
   int studyMetaPubMedCol = -1;
   int studyMetaTableCol = -1;
   int studyMetaTableSubHeaderCol = -1;
   int studyMetaFigureCol = -1;
   int studyMetaFigurePanelCol = -1;
   int studyMetaPageNumberCol = -1;
   
   for (int i = 0; i < numCols; i++) {
      const QString columnTitle = ct->getColumnTitle(i).toLower();
      if (columnTitle == "cell number") {
         cellNumberCol = i;
      }
      else if (columnTitle == "x") {
         xCol = i;
      }
      else if (columnTitle == "y") {
         yCol = i;
      }
      else if (columnTitle == "z") {
         zCol = i;
      }
      else if (columnTitle == "section") {
         sectionNumberCol = i;
      }
      else if (columnTitle == "name") {
         nameCol = i;
      }
      else if (columnTitle == "study number") {
         studyNumberCol = i;
      }
      else if (columnTitle == "geography") {
         geographyCol = i;
      }
      else if (columnTitle == "area") {
         areaCol = i;
      }
      else if (columnTitle == "size") {
         sizeCol = i;
      }
      else if (columnTitle == "statistic") {
         statisticCol = i;
      }
      else if (columnTitle == "comment") {
         commentCol = i;
      }
      else if (columnTitle == "structure") {
         structureCol = i;
      }
      else if (columnTitle == "class name") {
         classNameCol = i;
      }      
      else if (columnTitle == "study pubmed id") {
         studyMetaPubMedCol = i;
      }
      else if (columnTitle == "study table number") {
         studyMetaTableCol = i;
      }
      else if (columnTitle == "study table subheader") {
         studyMetaTableSubHeaderCol = i;
      }
      else if (columnTitle == "study figure number") {
         studyMetaFigureCol = i;
      }
      else if (columnTitle == "study figure panel") {
         studyMetaFigurePanelCol = i;
      }
      else if (columnTitle == "study page number") {
         studyMetaPageNumberCol = i;
      }
      else if (columnTitle == "sums id number") {
         sumsIDNumberCol = i;
      }
      else if (columnTitle == "sums repeat number") {
         sumsRepeatNumberCol = i;
      }
      else if (columnTitle == "sums parent cell base id") {
         sumsParentCellBaseIDCol = i;
      }
   }
   
   for (int i = 0; i < ct->getNumberOfRows(); i++) {
      float xyz[3] = { 0.0, 0.0, 0.0 };
      int section = 0;
      QString name;
      int studyNumber = -1;
      QString geography;
      QString area;
      float size = 1.0;
      QString statistic;
      QString comment;
      Structure::STRUCTURE_TYPE structure = Structure::STRUCTURE_TYPE_INVALID;
      QString className;
      StudyMetaDataLink smdl;
      QString sumsIDNumber = "-1";
      QString sumsRepeatNumber = "-1";
      QString sumsParentCellBaseID = "-1";
      
      if (xCol >= 0) {
         xyz[0] = ct->getElementAsFloat(i, xCol);
         if (structureCol < 0) {
            if (xyz[0] < 0.0) {
               structure = Structure::STRUCTURE_TYPE_CORTEX_LEFT;
            }
            else {
               structure = Structure::STRUCTURE_TYPE_CORTEX_RIGHT;
            }
         }
      }
      if (yCol >= 0) {
         xyz[1] = ct->getElementAsFloat(i, yCol);
      }
      if (zCol >= 0) {
         xyz[2] = ct->getElementAsFloat(i, zCol);
      }
      if (sectionNumberCol >= 0) {
         section = ct->getElementAsInt(i, sectionNumberCol);
      }
      if (nameCol >= 0) {
         name = ct->getElement(i, nameCol);
      }
      if (studyNumberCol >= 0) {
         studyNumber = ct->getElementAsInt(i, studyNumberCol);
      }
      if (geographyCol >= 0) {
         geography = ct->getElement(i, geographyCol);
      }
      if (areaCol >= 0) {
         area = ct->getElement(i, areaCol);
      }
      if (sizeCol >= 0) {
         size = ct->getElementAsFloat(i, sizeCol);
      }
      if (statisticCol >= 0) {
         statistic = ct->getElement(i, statisticCol);
      }
      if (commentCol >= 0) {
         comment = ct->getElement(i, commentCol);
      }
      if (structureCol >= 0) {
         structure = Structure::convertStringToType(ct->getElement(i, structureCol));
      }
      if (classNameCol >= 0) {
         className = ct->getElement(i, classNameCol);
         if (className == "???") {
            className = "";
         }
      }
      if (sumsIDNumberCol >= 0) {
         sumsIDNumber = ct->getElement(i, sumsIDNumberCol);
      }
      if (sumsRepeatNumberCol >= 0) {
         sumsRepeatNumber = ct->getElement(i, sumsRepeatNumberCol);
      }
      if (sumsParentCellBaseIDCol >= 0) {
         sumsParentCellBaseID = ct->getElement(i, sumsParentCellBaseIDCol);
      }
      
      if (studyMetaPubMedCol >= 0) {
         smdl.setPubMedID(ct->getElement(i, studyMetaPubMedCol));
      } 
      if (studyMetaTableCol >= 0) {
         smdl.setTableNumber(ct->getElement(i, studyMetaTableCol));
      } 
      if (studyMetaTableSubHeaderCol >= 0) {
         smdl.setTableSubHeaderNumber(ct->getElement(i, studyMetaTableSubHeaderCol));
      } 
      if (studyMetaFigureCol >= 0) {
         smdl.setFigureNumber(ct->getElement(i, studyMetaFigureCol));
      } 
      if (studyMetaFigurePanelCol >= 0) {
         smdl.setFigurePanelNumberOrLetter(ct->getElement(i, studyMetaFigurePanelCol));
      } 
      if (studyMetaPageNumberCol >= 0) {
         smdl.setPageNumber(ct->getElement(i, studyMetaPageNumberCol));
      } 
      
      CellData cd;
      cd.setXYZ(xyz);
      cd.setSectionNumber(section);
      cd.setName(name);
      cd.setStudyNumber(studyNumber);
      cd.setGeography(geography);
      cd.setArea(area);
      cd.setSize(size);
      cd.setStatistic(statistic);
      cd.setComment(comment);
      cd.setCellStructure(structure);
      cd.setClassName(className);
      cd.setSumsIDNumber(sumsIDNumber);
      cd.setSumsRepeatNumber(sumsRepeatNumber);
      cd.setSumsParentCellBaseID(sumsParentCellBaseID);
      StudyMetaDataLinkSet smdls;
      if (smdl.getPubMedID().isEmpty() == false) {
         smdls.addStudyMetaDataLink(smdl);
      }
      cd.setStudyMetaDataLinkSet(smdls);

      addCell(cd);
   }
   
   //
   // Do header
   //
   const StringTable* stHeader = csv.getDataSectionByName("header");
   if (stHeader != NULL) {
      readHeaderDataFromStringTable(*stHeader);
   }
    
   //
   // Do study info
   //
   const StringTable* stcsi = csv.getDataSectionByName("Cell Study Info");
   if (stcsi != NULL) {
      CellStudyInfo::readDataFromStringTable(studyInfo, *stcsi);
   }
}
      
/**
 * Read the data for a version 0 cell file.
 */
void
CellFile::readFileVersion0(QTextStream& stream, const QString& lineIn) throw (FileException)
{
   QString line = lineIn;
   
   // 1st line contains number of cells
   //readLine(stream, line);
   const int numCells = line.toInt();
   
   for (int i = 0; i < numCells; i++) {
      // line contains cell data
      readLine(stream, line);
      
      int cellNumber;
      QString unusedName, name;
      int section, node;
      float x, y, z;     
      QString className;
      int studyNumber = -1;
      
      QTextStream(&line, QIODevice::ReadOnly) >> cellNumber
                          >> section
                          >> node
                          >> name
                          >> x
                          >> y
                          >> z
                          >> className;
      CellData cd(name, x, y, z, section, 
                  className, studyNumber);
      addCell(cd);
   }
}

/**
 * Read the data for a version 1 cell file.
 */
void
CellFile::readFileVersion1(QTextStream& stream, const int numCells,
                           const int numStudyInfo) throw (FileException)
{
   for (int i = 0; i < numCells; i++) {
      QString line;
      std::vector<QString> tokens;
      readLineIntoTokens(stream, line, tokens);
      if (tokens.size() < 5) {
         QString s("reading line: ");
         s.append(line);
         throw FileException(filename, s);
      }
      
      const float x = tokens[1].toFloat();
      const float y = tokens[2].toFloat();
      const float z = tokens[3].toFloat();
      const QString name(tokens[4]);
      
      int studyInfoNumber = -1;
      if (tokens.size() >= 6) {
         studyInfoNumber = tokens[5].toInt();
      }
      
      int section = 0;
      if (tokens.size() >= 7) {
         section = tokens[6].toInt();
      }
      
      QString className;
      if (tokens.size() >= 8) {
         className = tokens[7];
         if (className == "???") {
            className = "";
         }
      }
      
      CellData cd(name, x, y, z, section, className, studyInfoNumber);
      if (x > 0.0) {
         cd.setCellStructure(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
      }
      else {
         cd.setCellStructure(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
      }
      addCell(cd);
   }
   
   for (int j = 0; j < numStudyInfo; j++) {
      QString line;
      readLine(stream, line);
      int firstBlank = line.indexOf(' ');
      if (firstBlank != -1) {
         line = line.mid(firstBlank + 1);
      }
      CellStudyInfo csi;
      csi.setTitle(StringUtilities::setupCommentForDisplay(line));
      addStudyInfo(csi);
   }
}

/**
 * Read the data for a version 2 cell file.
 */
void
CellFile::readFileVersion2(QFile& /*file*/, QTextStream& stream, 
                           const int numCells) throw (FileException)
{
   //
   // Read the cells
   //
   for (int i = 0; i < numCells; i++) {
      QString line;
      std::vector<QString> tokens;
      readLineIntoTokens(stream, line, tokens);
      if (tokens.size() < 5) {
         QString s("reading line: ");
         s.append(line);
         throw FileException(filename, s);
      }
      
      const float x = tokens[1].toFloat();
      const float y = tokens[2].toFloat();
      const float z = tokens[3].toFloat();
      const QString name(tokens[4]);
      
      int studyInfoNumber = -1;
      if (tokens.size() >= 6) {
         studyInfoNumber = tokens[5].toInt();
      }
      
      int section = 0;
      if (tokens.size() >= 7) {
         section = tokens[6].toInt();
      }
      
      QString className(""); //"???");
      if (tokens.size() >= 8) {
         className = tokens[7];
         if (className == "???") {
            className = "";
         }
      }
      
      CellData cd(name, x, y, z, section, className, studyInfoNumber);
      if (x > 0.0) {
         cd.setCellStructure(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
      }
      else {
         cd.setCellStructure(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
      }
      addCell(cd);
   }
   
   //
   // study info may not begin at zero.  So, this converts study numbers
   // to their numbers in this file
   //
   std::vector<int> studyIndexer;
   
   //
   // Read the study info
   //
   //for (int j = 0; j < numComments; j++) {
   while (stream.atEnd() == false) {
      //
      // Get number, tag, and value
      //
      int number;
      QString tag;
      QString tagValue;
      readNumberedTagLine(stream, number, tag, tagValue);
      
      if (tagValue.isEmpty() == false) {
         int index = -1;
         for (int k = 0; k < static_cast<int>(studyIndexer.size()); k++) {
            if (studyIndexer[k] == number) {
               index = k;
               break;
            }
         }
         if (index < 0) {
            CellStudyInfo csi;
            index = addStudyInfo(csi);
            studyIndexer.push_back(number);
         }
         
         tagValue = StringUtilities::setupCommentForDisplay(tagValue);
         if (tag == tagCommentUrl) {
            studyInfo[index].setURL(tagValue);
         }
         else if (tag == tagCommentKeyWords) {
            studyInfo[index].setKeywords(tagValue);
         }
         else if (tag == tagCommentTitle) {
            studyInfo[index].setTitle(tagValue);
         }
         else if (tag == tagCommentAuthors) {
            studyInfo[index].setAuthors(tagValue);
         }
         else if (tag == tagCommentCitation) {
            studyInfo[index].setCitation(tagValue);
         }
         else if (tag == tagCommentStereotaxicSpace) {
            studyInfo[index].setStereotaxicSpace(tagValue);
         }
         else {
            std::cout << "Unrecognized cell/foci tag: " << tag.toAscii().constData() << std::endl;
         }
      }
   }
   
   //
   // Update the cell study info indices
   //
   for (int i = 0; i < numCells; i++) {
      CellData* cd = getCell(i);
      
      for (int j = 0; j < static_cast<int>(studyIndexer.size()); j++) {
         if (cd->studyNumber == studyIndexer[j]) {
            cd->studyNumber = j;
            break;
         }
      }
   }
}

/**
 * Read the cell file data.
 */
void
CellFile::readFileData(QFile& file, 
                       QTextStream& stream, 
                       QDataStream&,
                       QDomElement& rootElement) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            bool readingTags = true;
            bool firstTag = true;
            int  numCells = -1;
            int  numStudyInfo = 0;
            int  readingFileVersion = -1;
            
            //const QIODevice::Offset pos = file.at();

            QString line;
            while(readingTags) {
               QString tag, value;
               readTagLine(stream, line, tag, value);
               if (tag == tagFileVersion) {
                  readingFileVersion = value.toInt();
               }
               else if (tag == tagNumberOfCells) {
                  numCells = value.toInt();
               }
               else if (tag == tagBeginData) {
                  readingTags = false;
               }
               else if (tag == tagNumberOfComments) {
                  numStudyInfo = value.toInt();
               }
               else if (firstTag) {
                  //
                  // Must be old version since there are no tags
                  //
                  readingFileVersion = 0;
                  //file.at(pos);
                  readingTags = false;
               }
               else {
                  std::cout << "WARNING: Unrecognized cell file tag " << tag.toAscii().constData() << std::endl;
               }
               firstTag = false;
            }
            
            switch (readingFileVersion) {
               case 0:
                  readFileVersion0(stream, line);
                  break;
               case 1:
                  readFileVersion1(stream, numCells, numStudyInfo);
                  break;
               case 2:
                  readFileVersion2(file, stream, numCells);
                  break;
               default:
                  {
                     std::ostringstream str;
                     str << "Unrecognized cell file version "
                         << readingFileVersion;
                     throw FileException(filename, str.str().c_str());
                  }
                  break;
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) { 
                  //
                  // Is this a "CellData" element
                  //
                  if (elem.tagName() == CellData::tagCellData) {
                     CellData cd;
                     cd.readXML(node);
                     addCell(cd);
                  }
                  else if (elem.tagName() == CellStudyInfo::tagCellStudyInfo) {
                     CellStudyInfo csi;
                     csi.readXML(node);
                     addStudyInfo(csi);
                  }
                  else if ((elem.tagName() == xmlHeaderOldTagName) ||
                           (elem.tagName() == xmlHeaderTagName)) {
                     // ignore, read by AbstractFile::readFile()
                  }
                  else {
                     if (elem.tagName() != "FileHeader") {
                        std::cout << "WARNING: unrecognized CellFile element: "
                         << elem.tagName().toAscii().constData()
                         << std::endl;
                     }
                  }
               }
               node = node.nextSibling();
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            csvf.readFromTextStream(file, stream);
            readDataFromCommaSeparatedValuesTable(csvf);
         }
         break;
   }
   
   const int numCells = getNumberOfCells();
   for (int i = 0; i < numCells; i++) {
      getCell(i)->updateInvalidCellStructureUsingXCoordinate();
   }
}

/**
 * Write a cell file's data.
 */
void
CellFile::writeFileData(QTextStream& stream, 
                        QDataStream&,
                        QDomDocument& xmlDoc,
                        QDomElement& rootElement) throw (FileException)
{
   //writeFileVersion2(stream);
   //writeFileVersion1(stream);
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Writing in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Writing in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            //
            // Write the cells
            //
            const int numCells = getNumberOfCells();
            for (int i = 0; i < numCells; i++) {
               CellData* cd = getCell(i);
               cd->writeXML(xmlDoc, rootElement, i);
            }
            
            //
            // Write the study info
            //
            const int numStudyInfo = getNumberOfStudyInfo();
            for (int i = 0; i < numStudyInfo; i++) {
               studyInfo[i].writeXML(xmlDoc, rootElement, i);
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Writing XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Writing XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            writeDataIntoCommaSeparatedValueFile(csvf);
            csvf.writeToTextStream(stream);
         }
         break;
   }
}

/**
 * Write a version 1 cell file's data.
 */
void
CellFile::writeFileVersion1(QTextStream& stream) throw (FileException)
{   
   const int numCells = getNumberOfCells();
   
   stream << tagFileVersion << " 1" << "\n";
   stream << tagNumberOfCells << " " << numCells << "\n";
   stream << tagNumberOfComments << " " << studyInfo.size() << "\n";
   stream << tagBeginData << "\n";
   
   for (int i = 0; i < numCells; i++) {
      CellData* cd = getCell(i);
      
      float xyz[3];
      cd->getXYZ(xyz);
      stream << i << " "
             << xyz[0] << " "
             << xyz[1] << " "
             << xyz[2] << " "
             << cd->getName() << " "
             << cd->getStudyNumber() << " "
             << cd->getSectionNumber() << " "
             << cd->getClassName() << "\n";
   }
   
   for (unsigned int j = 0; j < studyInfo.size(); j++) {
      stream << j << " " 
             << StringUtilities::setupCommentForStorage(studyInfo[j].getTitle())
             << "\n";
   }
}

/**
 * Write a version 2 cell file's data.
 */
void
CellFile::writeFileVersion2(QTextStream& stream) throw (FileException)
{   
   const int numCells = getNumberOfCells();
   
   stream << tagFileVersion << " 2" << "\n";
   stream << tagNumberOfCells << " " << numCells << "\n";
   stream << tagNumberOfComments << " " << studyInfo.size() << "\n";
   stream << tagBeginData << "\n";
   
   for (int i = 0; i < numCells; i++) {
      CellData* cd = getCell(i);
      
      float xyz[3];
      cd->getXYZ(xyz);
      QString className(cd->getClassName());
      if (className.isEmpty()) {
         className = "";  //"???";
      }
      
      stream << i << " "
             << xyz[0] << " "
             << xyz[1] << " "
             << xyz[2] << " "
             << cd->getName() << " "
             << cd->getStudyNumber() << " "
             << cd->getSectionNumber() << " "
             << className << "\n";
   }
   
   for (unsigned int j = 0; j < studyInfo.size(); j++) {
      stream << j << " " << tagCommentUrl 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getURL()) 
             << "\n";
      stream << j << " " << tagCommentKeyWords 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getKeywords()) 
             << "\n";
      stream << j << " " << tagCommentTitle 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getTitle()) 
             << "\n";
      stream << j << " " << tagCommentAuthors 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getAuthors()) 
             << "\n";
      stream << j << " " << tagCommentCitation 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getCitation()) 
             << "\n";
      stream << j << " " << tagCommentStereotaxicSpace
             << " " << studyInfo[j].getStereotaxicSpace()
             << "\n";
   }
}



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

#include <iostream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>

#include "AbstractFile.h"
#define __CELL_BASE_MAIN__
#include "CellBase.h"
#undef __CELL_BASE_MAIN__
#include "StringUtilities.h"

/**
 * constructor.
 */
CellBase::CellBase()
{
   initialize();
}

/**
 * destructor.
 */
CellBase::~CellBase()
{
}

/**
 * Initialize the data
 */
void
CellBase::initialize()
{
   xyz[0] = 0.0;
   xyz[1] = 0.0;
   xyz[2] = 0.0;
   searchXYZ[0] = 0.0;
   searchXYZ[1] = 0.0;
   searchXYZ[2] = 0.0;
   sectionNumber = -1;
   name = "";
   studyNumber = -1;
   studyMetaDataLinkSet.clear();
   geography = "";
   area = "";
   regionOfInterest = "";
   size = 0.0;
   statistic = "";
   comment = "";
   displayFlag = false;
   colorIndex = -1;
   className = "";  // "???";
   classIndex = -1;
   inSearchFlag = true;
   specialFlag = false;
   signedDistanceAboveSurface = 0.0;
   structure.setType(Structure::STRUCTURE_TYPE_INVALID);
   highlightFlag = false;
   sumsIDNumber = "-1";
   sumsRepeatNumber = "-1";
   sumsParentCellBaseID = "-1";
   sumsVersionNumber = "-1";
   sumsMSLID = "-1";
   attributeID = "-1";
}

/**
 * copy the data from another cell base class.
 */
void 
CellBase::copyData(const CellBase& cb)
{
   xyz[0] = cb.xyz[0];
   xyz[1] = cb.xyz[1];
   xyz[2] = cb.xyz[2];
   searchXYZ[0] = cb.searchXYZ[0];
   searchXYZ[1] = cb.searchXYZ[1];
   searchXYZ[2] = cb.searchXYZ[2];
   sectionNumber = cb.sectionNumber;
   name = cb.name;
   studyNumber = cb.studyNumber;
   studyMetaDataLinkSet = cb.studyMetaDataLinkSet;
   geography = cb.geography;
   area = cb.area;
   regionOfInterest = cb.regionOfInterest;
   size = cb.size;
   statistic = cb.statistic;
   comment = cb.comment;
   displayFlag = cb.displayFlag;
   colorIndex = cb.colorIndex;
   className = cb.className;
   classIndex = cb.classIndex;
   inSearchFlag = cb.inSearchFlag;
   specialFlag = cb.specialFlag;
   signedDistanceAboveSurface = cb.signedDistanceAboveSurface;
   structure = cb.structure;
   highlightFlag = cb.highlightFlag;
   sumsIDNumber = cb.sumsIDNumber;
   sumsRepeatNumber = cb.sumsRepeatNumber;
   sumsParentCellBaseID = cb.sumsParentCellBaseID;
   sumsVersionNumber = cb.sumsVersionNumber;
   sumsMSLID = cb.sumsMSLID;
   attributeID = cb.attributeID;
}

/**
 * get xyz.
 */
void 
CellBase::getXYZ(float xyzOut[3]) const
{
   xyzOut[0] = xyz[0];
   xyzOut[1] = xyz[1];
   xyzOut[2] = xyz[2];
}

/**
 * set xyz.
 */
void 
CellBase::setXYZ(const float xyzIn[3])
{
   xyz[0] = xyzIn[0];
   xyz[1] = xyzIn[1];
   xyz[2] = xyzIn[2];
   setModified();
}

/**
 * set xyz.
 */
void 
CellBase::setXYZ(const float x, const float y, const float z)
{
   xyz[0] = x;
   xyz[1] = y;
   xyz[2] = z;
   setModified();
}

/**
 * get search xyz.
 */
void 
CellBase::getSearchXYZ(float xyzOut[3]) const
{
   xyzOut[0] = searchXYZ[0];
   xyzOut[1] = searchXYZ[1];
   xyzOut[2] = searchXYZ[2];
}

/**
 * set search xyz.
 */
void 
CellBase::setSearchXYZ(const float xyzIn[3])
{
   searchXYZ[0] = xyzIn[0];
   searchXYZ[1] = xyzIn[1];
   searchXYZ[2] = xyzIn[2];
   setModified();
}

/**
 * set search xyz.
 */
void 
CellBase::setSearchXYZ(const float x, const float y, const float z)
{
   searchXYZ[0] = x;
   searchXYZ[1] = y;
   searchXYZ[2] = z;
   setModified();
}      

/**
 * set the SuMS ID number.
 */
void 
CellBase::setSumsIDNumber(const QString& s)
{
   sumsIDNumber = s;
   setModified();
}

/**
 * set the SuMS repeat number.
 */
void 
CellBase::setSumsRepeatNumber(const QString& s)
{
   sumsRepeatNumber = s;
   setModified();
}

/**
 * set the SuMS parent cell base ID.
 */
void 
CellBase::setSumsParentCellBaseID(const QString& s)
{
   sumsParentCellBaseID = s;
   setModified();
}
      
/**
 * set SuMS version number.
 */
void 
CellBase::setSumsVersionNumber(const QString& s)
{
   sumsVersionNumber = s;
   setModified();
}

/**
 * set mslid.
 */
void 
CellBase::setSumsMSLID(const QString& s)
{
   sumsMSLID = s;
   setModified();
}

/**
 * set the attribute ID.
 */
void 
CellBase::setAttributeID(const QString& s)
{
   attributeID = s;
   setModified();
}      
      
/**
 * update the cell's invalid structure using the X coordinate if it is not zero.
 */
void 
CellBase::updateInvalidCellStructureUsingXCoordinate()
{
   if (structure.getType() == Structure::STRUCTURE_TYPE_INVALID) {
      if (xyz[0] > 0.0) {
         structure.setType(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
      }
      else if (xyz[0] < 0.0) {
         structure.setType(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
      }
   }
}
      
/**
 * set name.
 */
void 
CellBase::setName(const QString& n)
{
   name = n;
   setModified();
}

/**
 * set section number.
 */
void 
CellBase::setSectionNumber(const int sn)
{
   sectionNumber = sn;
   setModified();
}

/**
 * set study number.
 */
void 
CellBase::setStudyNumber(const int sn)
{
   studyNumber = sn;
   setModified();
}

/**
 * set the study metadata link.
 */
void 
CellBase::setStudyMetaDataLinkSet(const StudyMetaDataLinkSet smdls)
{
   studyMetaDataLinkSet = smdls;
   setModified();
}
            
/**
 * set region of interest.
 */
void 
CellBase::setRegionOfInterest(const QString& roi)
{
   regionOfInterest = roi;
   setModified();
}
      
/**
 * set geography.
 */
void 
CellBase::setGeography(const QString& g)
{
   geography = g;
   setModified();
}

/**
 * set area.
 */
void 
CellBase::setArea(const QString& a)
{
   area = a;
   setModified();
}

/**
 * set size.
 */
void 
CellBase::setSize(const float s)
{
   size = s;
   setModified();
}

/**
 * set statistic.
 */
void 
CellBase::setStatistic(const QString& s)
{
   statistic = s;
   setModified();
}

/**
 * set comment.
 */
void 
CellBase::setComment(const QString& c)
{
   comment = c;
   setModified();
}

/**
 * set the display flag.
 */
void 
CellBase::setDisplayFlag(const bool df)
{
   displayFlag = df;
   // does NOT modify file    setModified();
} 

/**
 * set the color index.
 */
void 
CellBase::setColorIndex(const int ci) 
{ 
   colorIndex = ci; 
   // does NOT modify file    setModified();
}

/**
 * copy data.
 */
void 
CellBase::copyCellBaseData(const CellBase& cb, const bool copyXYZ)
{
   if (copyXYZ) {
      xyz[0] = cb.xyz[0];
      xyz[1] = cb.xyz[1];
      xyz[2] = cb.xyz[2];
   }
   searchXYZ[0]  = searchXYZ[0];
   searchXYZ[1]  = searchXYZ[1];
   searchXYZ[2]  = searchXYZ[2];
   sectionNumber = cb.sectionNumber;
   name          = cb.name;
   studyNumber   = cb.studyNumber;
   geography     = cb.geography;
   area          = cb.area;
   regionOfInterest = cb.regionOfInterest;
   size          = cb.size;
   statistic     = cb.statistic;
   comment       = cb.comment;
   displayFlag   = cb.displayFlag;
   colorIndex    = cb.colorIndex;
   structure     = cb.structure;
   highlightFlag = cb.highlightFlag;
   sumsIDNumber  = cb.sumsIDNumber;
   sumsRepeatNumber = cb.sumsRepeatNumber;
   sumsParentCellBaseID = cb.sumsParentCellBaseID;
   sumsVersionNumber = cb.sumsVersionNumber;
   sumsMSLID = cb.sumsMSLID;
   attributeID = cb.attributeID;
}      

/**
 * set the structure.
 */
void 
CellBase::setCellStructure(const Structure::STRUCTURE_TYPE cst) 
{ 
   structure.setType(cst);
   setModified(); 
}
      
/**
 * called to read from an XML DOM structure.
 */
void 
CellBase::readXMLWithDOM(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != tagCellBase) {
      QString msg("Incorrect element type passed to CellData::readXMLWithDOM() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         //std::cout << "CellBase: " << elem.tagName() << std::endl;
         
         if (elem.tagName() == tagXYZ) {
            const QString xyzString = AbstractFile::getXmlElementFirstChildAsString(elem);
            std::vector<QString> elems;
            StringUtilities::token(xyzString, " ", elems);
            if (elems.size() >= 3) {
               xyz[0] = StringUtilities::toFloat(elems[0]);
               xyz[1] = StringUtilities::toFloat(elems[1]);
               xyz[2] = StringUtilities::toFloat(elems[2]);
            }
         }
         else if (elem.tagName() == tagSearchXYZ) {
            const QString xyzString = AbstractFile::getXmlElementFirstChildAsString(elem);
            std::vector<QString> elems;
            StringUtilities::token(xyzString, " ", elems);
            if (elems.size() >= 3) {
               searchXYZ[0] = StringUtilities::toFloat(elems[0]);
               searchXYZ[1] = StringUtilities::toFloat(elems[1]);
               searchXYZ[2] = StringUtilities::toFloat(elems[2]);
            }
         }
         else if (elem.tagName() == tagSectionNumber) {
            sectionNumber = StringUtilities::toInt(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagName) {
            name = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagStudyNumber) {
            studyNumber = StringUtilities::toInt(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagGeography) {
            geography = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagArea) {
            area = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagRegionOfInterest) {
            regionOfInterest = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagSize) {
            size = StringUtilities::toFloat(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagStatistic) {
            statistic = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagComment) {
            comment = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if ((elem.tagName() == "hemisphere") ||
                  (elem.tagName() == tagStructure)) {
            structure.setTypeFromString(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == tagClassName) {
            className = AbstractFile::getXmlElementFirstChildAsString(elem);
            if (className == "???") {
               className = "";
            }
         }
         else if (elem.tagName() == tagSignedDistanceAboveSurface) {
            signedDistanceAboveSurface = AbstractFile::getXmlElementFirstChildAsString(elem).toFloat();
         }
         else if ((elem.tagName() == StudyMetaDataLink::tagStudyMetaDataLink) ||
                  (elem.tagName() == StudyMetaDataLinkSet::tagStudyMetaDataLinkSet)) {
            studyMetaDataLinkSet.readXML(node);
         }
         else if (elem.tagName() == tagSumsIDNumber) {
            sumsIDNumber = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagSumsRepeatNumber) {
            sumsRepeatNumber = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagSumsParentCellBaseID) {
            sumsParentCellBaseID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagSumsVersionNumber) {
            sumsVersionNumber = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagSumsMSLID) {
            sumsMSLID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == tagAttributeID) {
            attributeID = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else {
            std::cout << "WARNING: unrecognized CellBase element: "
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
CellBase::writeXML(QDomDocument& xmlDoc,
                   QDomElement&  parentElement)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement cellBaseElement = xmlDoc.createElement(tagCellBase);

   //
   // XYZ coordinates
   //
   std::vector<float> xyzVec;
   xyzVec.push_back(xyz[0]);
   xyzVec.push_back(xyz[1]);
   xyzVec.push_back(xyz[2]);
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement, tagXYZ,
                     StringUtilities::combine(xyzVec, " "));
   
   //
   // Search XYZ coordinates
   //
   xyzVec.clear();
   xyzVec.push_back(searchXYZ[0]);
   xyzVec.push_back(searchXYZ[1]);
   xyzVec.push_back(searchXYZ[2]);
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement, tagSearchXYZ,
                     StringUtilities::combine(xyzVec, " "));
   
   //
   // section number
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagSectionNumber, sectionNumber);
   
   //
   // name of cell
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    tagName, name);
   
   //
   // study number
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagStudyNumber, studyNumber);
   
   //
   // geography
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    tagGeography, geography);
   
   //
   // area
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    tagArea, area);
   
   //
   // region of interest
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    tagRegionOfInterest, regionOfInterest);
                                    
   //
   // size of cell
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagSize, size);
   
   //
   // statistic
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    tagStatistic, statistic);
   
   //
   // comment
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    tagComment, comment);
   
   //
   // class name
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    tagClassName, className);
   
   //
   // signed distance above surface
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    tagSignedDistanceAboveSurface, QString::number(signedDistanceAboveSurface, 'f', 6));
   
   //
   // SuMS ID, repeat number and parent cell base ID, version numer, mslid
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagSumsIDNumber, sumsIDNumber);
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagSumsRepeatNumber, sumsRepeatNumber);
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagSumsParentCellBaseID, sumsParentCellBaseID);
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagSumsVersionNumber, sumsVersionNumber);
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagSumsMSLID, sumsMSLID);
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagAttributeID, attributeID);
   
   //
   // structure
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   tagStructure, structure.getTypeAsString());
   
   //
   // study metadata
   //
   studyMetaDataLinkSet.writeXML(xmlDoc, cellBaseElement);
   
   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(cellBaseElement);
}


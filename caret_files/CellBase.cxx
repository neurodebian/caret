
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
   sectionNumber = -1;
   name = "";
   studyNumber = -1;
   studyMetaDataLinkSet.clear();
   geography = "";
   area = "";
   size = 0.0;
   statistic = "";
   comment = "";
   displayFlag = false;
   colorIndex = -1;
   className = "";  // "???";
   classIndex = -1;
   specialFlag = false;
   signedDistanceAboveSurface = 0.0;
   structure.setType(Structure::STRUCTURE_TYPE_INVALID);
   highlightFlag = false;
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
   sectionNumber = cb.sectionNumber;
   name = cb.name;
   studyNumber = cb.studyNumber;
   studyMetaDataLinkSet = cb.studyMetaDataLinkSet;
   geography = cb.geography;
   area = cb.area;
   size = cb.size;
   statistic = cb.statistic;
   comment = cb.comment;
   displayFlag = cb.displayFlag;
   colorIndex = cb.colorIndex;
   className = cb.className;
   classIndex = cb.classIndex;
   specialFlag = cb.specialFlag;
   signedDistanceAboveSurface = cb.signedDistanceAboveSurface;
   structure = cb.structure;
   highlightFlag = cb.highlightFlag;
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
   sectionNumber = cb.sectionNumber;
   name          = cb.name;
   studyNumber   = cb.studyNumber;
   geography     = cb.geography;
   area          = cb.area;
   size          = cb.size;
   statistic     = cb.statistic;
   comment       = cb.comment;
   displayFlag   = cb.displayFlag;
   colorIndex    = cb.colorIndex;
   structure     = cb.structure;
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
 * set base element from text (used by SAX XML parser).
 */
void 
CellBase::setBaseElementFromText(const QString& elementName,
                                 const QString& textValue)
{
   if (elementName == tagXYZ) {
      const QStringList sl = textValue.split(QRegExp("\\s+"), QString::SkipEmptyParts);
      const int numItems = sl.count();
      if (numItems == 3) {
         xyz[0] = sl.at(0).toFloat();
         xyz[1] = sl.at(1).toFloat();
         xyz[2] = sl.at(2).toFloat();
      }
   }
   else if (elementName == tagSectionNumber) {
      sectionNumber = textValue.toInt();
   }
   else if (elementName == tagName) {
      name = textValue;
   }
   else if (elementName == tagStudyNumber) {
      studyNumber = textValue.toInt();
   }
   else if (elementName == tagGeography) {
      geography = textValue;
   }
   else if (elementName == tagArea) {
      area = textValue;
   }
   else if (elementName == tagSize) {
      size = textValue.toFloat();
   }
   else if (elementName == tagStatistic) {
      statistic = textValue;
   }
   else if (elementName == tagComment) {
      comment = textValue;
   }
   else if ((elementName == "hemisphere") ||
            (elementName == tagStructure)) {
      structure.setTypeFromString(textValue);
   }
   else if (elementName == tagClassName) {
      className = textValue;
      if (className == "???") {
         className = "";
      }
   }
   else if (elementName == tagSignedDistanceAboveSurface) {
      signedDistanceAboveSurface = textValue.toFloat();
   }
   else if ((elementName == StudyMetaDataLink::tagStudyMetaDataLink) ||
            (elementName == StudyMetaDataLinkSet::tagStudyMetaDataLinkSet)) {
      //studyMetaDataLinkSet.readXML(node);
   }
   else {
      std::cout << "WARNING: unrecognized CellBase element: "
                << elementName.toAscii().constData()
                << std::endl;
   }
}

/**
 * called to read from an XML structure.
 */
void 
CellBase::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != tagCellBase) {
      QString msg("Incorrect element type passed to CellData::readXML() ");
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


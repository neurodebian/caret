
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
#include "CellBase.h"
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
   studyMetaDataLink.clear();
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
   studyMetaDataLink = cb.studyMetaDataLink;
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
CellBase::setStudyMetaDataLink(const StudyMetaDataLink smdl)
{
   studyMetaDataLink = smdl;
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
   if (elem.tagName() != "CellBase") {
      QString msg("Incorrect element type passed to CellData::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         //std::cout << "CellBase: " << elem.tagName() << std::endl;
         
         if (elem.tagName() == "xyz") {
            const QString xyzString = AbstractFile::getXmlElementFirstChildAsString(elem);
            std::vector<QString> elems;
            StringUtilities::token(xyzString, " ", elems);
            if (elems.size() >= 3) {
               xyz[0] = StringUtilities::toFloat(elems[0]);
               xyz[1] = StringUtilities::toFloat(elems[1]);
               xyz[2] = StringUtilities::toFloat(elems[2]);
            }
         }
         else if (elem.tagName() == "sectionNumber") {
            sectionNumber = StringUtilities::toInt(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == "name") {
            name = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "studyNumber") {
            studyNumber = StringUtilities::toInt(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == "geography") {
            geography = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "area") {
            area = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "size") {
            size = StringUtilities::toFloat(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == "statistic") {
            statistic = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if (elem.tagName() == "comment") {
            comment = AbstractFile::getXmlElementFirstChildAsString(elem);
         }
         else if ((elem.tagName() == "hemisphere") ||
                  (elem.tagName() == "structure")) {
            structure.setTypeFromString(AbstractFile::getXmlElementFirstChildAsString(elem));
         }
         else if (elem.tagName() == "className") {
            className = AbstractFile::getXmlElementFirstChildAsString(elem);
            if (className == "???") {
               className = "";
            }
         }
         else if (elem.tagName() == "signedDistanceAboveSurface") {
            signedDistanceAboveSurface = AbstractFile::getXmlElementFirstChildAsString(elem).toFloat();
         }
         else if (elem.tagName() == "StudyMetaDataLink") {
            studyMetaDataLink.readXML(node);
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
   QDomElement cellBaseElement = xmlDoc.createElement("CellBase");

   //
   // XYZ coordinates
   //
   std::vector<float> xyzVec;
   xyzVec.push_back(xyz[0]);
   xyzVec.push_back(xyz[1]);
   xyzVec.push_back(xyz[2]);
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement, "xyz",
                     StringUtilities::combine(xyzVec, " "));
   
   //
   // section number
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   "sectionNumber", sectionNumber);
   
   //
   // name of cell
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    "name", name);
   
   //
   // study number
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   "studyNumber", studyNumber);
   
   //
   // geography
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    "geography", geography);
   
   //
   // area
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    "area", area);
   
   //
   // size of cell
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   "size", size);
   
   //
   // statistic
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    "statistic", statistic);
   
   //
   // comment
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    "comment", comment);
   
   //
   // class name
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    "className", className);
   
   //
   // signed distance above surface
   //
   AbstractFile::addXmlCdataElement(xmlDoc, cellBaseElement,
                                    "signedDistanceAboveSurface", QString::number(signedDistanceAboveSurface, 'f', 6));
   
   //
   // structure
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellBaseElement,
                                   "structure", structure.getTypeAsString());
   
   //
   // study metadata
   //
   studyMetaDataLink.writeXML(xmlDoc, cellBaseElement);
   
   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(cellBaseElement);
}


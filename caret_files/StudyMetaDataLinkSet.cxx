
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
#include <QStringList>

#define __STUDY_META_DATA_LINK_SET_MAIN__
#include "StudyMetaDataLinkSet.h"
#undef __STUDY_META_DATA_LINK_SET_MAIN__

/**
 * constructor.
 */
StudyMetaDataLinkSet::StudyMetaDataLinkSet()
{
   clear();
}

/**
 * destructor.
 */
StudyMetaDataLinkSet::~StudyMetaDataLinkSet()
{
   clear();
}

/**
 * add a StudyMetaDataLink.
 */
void 
StudyMetaDataLinkSet::addStudyMetaDataLink(const StudyMetaDataLink& smdl)
{
   links.push_back(smdl);
}

/**
 * remove all links.
 */
void 
StudyMetaDataLinkSet::clear()
{
   links.clear();
}

/**
 * get a StudyMetaDataLink.
 */
StudyMetaDataLink 
StudyMetaDataLinkSet::getStudyMetaDataLink(const int indx) const
{
   return links[indx];
}

/**
 * get a pointer to a StudyMetaDataLink.
 */
StudyMetaDataLink* 
StudyMetaDataLinkSet::getStudyMetaDataLinkPointer(const int indx)
{
   if ((indx >= 0) &&
       (indx < getNumberOfStudyMetaDataLinks())) {
      return &links[indx];
   }
   
   return NULL;
}
      
/**
 * set a study meta data link.
 */
void 
StudyMetaDataLinkSet::setStudyMetaDataLink(const int indx, const StudyMetaDataLink& smdl)
{
   links[indx] = smdl;
}
      
/**
 * remove a study meta data link.
 */
void 
StudyMetaDataLinkSet::removeStudyMetaDataLink(const int indx)
{
   links.erase(links.begin() + indx);
}

/**
 * get the entire link set in an "coded" text form.
 */
QString 
StudyMetaDataLinkSet::getLinkSetAsCodedText() const
{
   QStringList sl;
   const int num = getNumberOfStudyMetaDataLinks();
   for (int i = 0; i < num; i++) {
      sl << getStudyMetaDataLink(i).getLinkAsCodedText();
   }

   const QString s = sl.join(encodedTextLinkSeparator);
      
   return s;
}

/**
 * set the link set from "coded" text form.
 */
void 
StudyMetaDataLinkSet::setLinkSetFromCodedText(const QString& txt)
{
   clear();
   const QStringList sl = txt.split(encodedTextLinkSeparator, QString::SkipEmptyParts);
   
   for (int i = 0; i < sl.count(); i++) {
      StudyMetaDataLink smdl;
      smdl.setLinkFromCodedText(sl.at(i));
      links.push_back(smdl);
   }
}

/**
 * called to read from an XML structure.
 */
void 
StudyMetaDataLinkSet::readXML(QDomNode& nodeIn) throw (FileException)
{
   clear();
   
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   
   if (elem.tagName() == tagStudyMetaDataLinkSet) {
      QDomNode node = nodeIn.firstChild();
      while (node.isNull() == false) {
         QDomElement elem = node.toElement();
         if (elem.isNull() == false) {
            if (elem.tagName() == StudyMetaDataLink::tagStudyMetaDataLink) {
               StudyMetaDataLink smdl;
               smdl.readXML(elem);
               links.push_back(smdl);
            }
            else {
               std::cout << "WARNING: unrecognized StudyMetaDataLinkSet element ignored: "
                         << elem.tagName().toAscii().constData()
                         << std::endl;
            }
         }
         node = node.nextSibling();
      }
   }
   else if (elem.tagName() == StudyMetaDataLink::tagStudyMetaDataLink) {
      StudyMetaDataLink smdl;
      smdl.readXML(elem);
      links.push_back(smdl);
   }
   else {
      QString msg("Incorrect element type passed to StudyMetaDataLinkSet::readXML() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
}

/**
 * called to write to an XML structure.
 */
void 
StudyMetaDataLinkSet::writeXML(QDomDocument& xmlDoc,
                               QDomElement&  parentElement) const throw (FileException)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement linkSetElement = xmlDoc.createElement(tagStudyMetaDataLinkSet);

   //
   // Write the links
   //
   const int num = getNumberOfStudyMetaDataLinks();
   for (int i = 0; i < num; i++) {
      StudyMetaDataLink smdl = getStudyMetaDataLink(i);
      smdl.writeXML(xmlDoc, linkSetElement);
   }
   
   //
   // Add to parent
   //
   parentElement.appendChild(linkSetElement);
}


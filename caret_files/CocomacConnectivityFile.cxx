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
#include <sstream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>

#include "CocomacConnectivityFile.h"
#include "SpecFile.h"

/**
 * The constructor.
 */
CocomacConnectivityFile::CocomacConnectivityFile()
   : AbstractFile("CoCoMac File", 
                  SpecFile::getCocomacConnectivityFileExtension(),
                  false, 
                  FILE_FORMAT_XML, 
                  FILE_IO_NONE, 
                  FILE_IO_NONE, 
                  FILE_IO_READ_ONLY,
                  FILE_IO_NONE)
{
   clear();
   setRootXmlElementTagName("CoCoMacExport");
}

/**
 * The destructor.
 */
CocomacConnectivityFile::~CocomacConnectivityFile()
{
   clear();
}

/** 
 * Append a CoCoMac file to this one
 */
void 
CocomacConnectivityFile::append(CocomacConnectivityFile& ccf, QString& errorMessage)
{
   errorMessage = "";
   
   for (int i = 0; i < ccf.getNumberOfCocomacProjections(); i++) {
      projections.push_back(*(ccf.getCocomacProjection(i)));
   }

   //
   // transfer the file's comment
   //
   appendFileComment(ccf);
}

/**
 * Clear the file.
 */
void
CocomacConnectivityFile::clear()
{
   clearAbstractFile();
   projections.clear();
   version = "";
   exportDate = "";
   dataType = "";
   comments = "";
}

/**
 * Read the file.
 */
void
CocomacConnectivityFile::readFileData(QFile& /*file*/, QTextStream& /*stream*/, QDataStream&,
                                  QDomElement& rootElement) throw (FileException)
{
/*
   //
   // Read the remainder of the file into a QString.
   //
   const QString remainderOfFile = stream.read();
   
   //
   // Place the file contents into a QDomDocument which will parse file.
   //
   QString errorMessage;
   int errorLine = 0, errorColumn = 0;
   QDomDocument doc("cocomac");
   if (doc.setContent(remainderOfFile, &errorMessage, &errorLine, &errorColumn) == false) {
      std::ostringstream str;
      str << "Error parsing at line " << errorLine << " column " << errorColumn << ".  ";
      str << errorMessage << std::ends;
      throw FileException(filename, str.str().c_str());
   }
   
   //
   // Traverse the direct children
   //
   QDomElement docElem = doc.documentElement();
   QDomNode node = docElem.firstChild();
*/   
   QDomNode node = rootElement.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {         
         if (elem.tagName() == "Header") {
            processHeaderNode(elem);
         }
         else if (elem.tagName() == "ProcessedConnectivityData") {
            processConnectivityNode(elem);
         }
         else {
            std::cerr << "Cocomac node not recognized in root " << elem.tagName().toAscii().constData() << std::endl;
         }
      }
      node = node.nextSibling();
   }
}

void
CocomacConnectivityFile::printNodeType(QDomNode& n)
{
   if (n.isNull() == false) {
      std::cout << " is type ";
      switch(n.nodeType()) {
         case QDomNode::ElementNode:
            std::cout << "ElementNode"; 
            break;
         case QDomNode::AttributeNode:
            std::cout << "AttributeNode"; 
            break;
         case QDomNode::TextNode:
            std::cout << "TextNode"; 
            break;
         case QDomNode::CDATASectionNode:
            std::cout << "CDATASectionNode"; 
            break;
         case QDomNode::EntityReferenceNode:
            std::cout << "EntityReferenceNode"; 
            break;
         case QDomNode::EntityNode:
            std::cout << "EntityNode"; 
            break;
         case QDomNode::ProcessingInstructionNode:
            std::cout << "ProcessingInstructionNode"; 
            break;
         case QDomNode::CommentNode:
            std::cout << "CommentNode"; 
            break;
         case QDomNode::DocumentNode:
            std::cout << "DocumentNode"; 
            break;
         case QDomNode::DocumentTypeNode:
            std::cout << "DocumentTypeNode"; 
            break;
         case QDomNode::DocumentFragmentNode:
            std::cout << "DocumentFragmentNode"; 
            break;
         case QDomNode::NotationNode:
            std::cout << "NotationNode"; 
            break;
         case QDomNode::BaseNode:
            std::cout << "BaseNode"; 
            break;
         case QDomNode::CharacterDataNode:
            std::cout << "CharacterDataNode"; 
            break;
         default:
            std::cout << "Unknown node type";
            break;
      }
      std::cout << std::endl;
   }
}

/**
 * Process the header node.
 */
void
CocomacConnectivityFile::processHeaderNode(QDomElement& headerNode) throw (FileException)
{
   QDomNode node = headerNode.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         
         if (elem.tagName() == "CoCoMacVersion") {
            QDomNode vNode = elem.firstChild();
            if (vNode.isNull() == false) {
               QDomText textNode = vNode.toText();
               if (textNode.isNull() == false) {
                  version = textNode.data();
               }
            } 
         }
         else if (elem.tagName() == "ExportDate") {
            QDomNode vNode = elem.firstChild();
            if (vNode.isNull() == false) {
               QDomText textNode = vNode.toText();
               if (textNode.isNull() == false) {
                  exportDate = textNode.data();
               }
            } 
         }
         else if (elem.tagName() == "DataType") {
            QDomNode vNode = elem.firstChild();
            if (vNode.isNull() == false) {
               QDomText textNode = vNode.toText();
               if (textNode.isNull() == false) {
                  dataType = textNode.data();
               }
            } 
         }
         else if (elem.tagName() == "Comments") {
            QDomNode vNode = elem.firstChild();
            if (vNode.isNull() == false) {
               QDomText textNode = vNode.toText();
               if (textNode.isNull() == false) {
                  comments = textNode.data();
               }
            } 
         }
      }
      node = node.nextSibling();
   }
}

/**
 * Process the connectivity data node
 */
void
CocomacConnectivityFile::processConnectivityNode(QDomElement& connectNode) throw (FileException)
{
   QDomNode node = connectNode.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {         
         if ((elem.tagName() == "PrimaryProjection") ||
             (elem.tagName() == "IntegratedPrimaryProjection") ||
             (elem.tagName() == "IntegratedResultingProjection")) {
            processPrimaryProjectionNode(elem);
         }
      }
      node = node.nextSibling();
   }
}

/**
 * Process a primary projection node.
 */
void
CocomacConnectivityFile::processPrimaryProjectionNode(QDomElement& connectNode) throw (FileException)
{
   CocomacProjection projection;
   QDomNode node = connectNode.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if ((elem.tagName() == "SourceSite") ||
             (elem.tagName() == "TargetSite")) {
            processSiteNode(elem, projection);
         }
         else if (elem.tagName() == "Density") {
            processDensityNode(elem, projection);
         }
      }
      node = node.nextSibling();
   }
   projections.push_back(projection);
}

/**
 * Process a "SourceSite" or "TargetSite" node.
 */
void
CocomacConnectivityFile::processSiteNode(QDomElement& siteNode,
                                         CocomacProjection& projection)
{
   const bool targetSiteNode = (siteNode.tagName() == "TargetSite");
   
   QDomNode node = siteNode.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "ID_BrainSite") {
            QDomNode vNode = elem.firstChild();
            if (vNode.isNull() == false) {
               QDomText textNode = vNode.toText();
               if (textNode.isNull() == false) {
                  if (targetSiteNode) {
                     projection.setTargetSite(textNode.data());
                  }
                  else {
                     projection.setSourceSite(textNode.data());
                  }
               }
            } 
         }
      }
      else if ((elem.tagName() == "Laminae") ||
               (elem.tagName() == "LaminaeA") ||
               (elem.tagName() == "LaminaeB")) {
         QDomNode vNode = elem.firstChild();
         if (vNode.isNull() == false) {
            QDomElement vElem = vNode.toElement();
            if (vElem.isNull() == false) {
               if (vElem.tagName() == "Pattern") {
                  vNode = vNode.firstChild();
                  if (vNode.isNull() == false) {
                     QDomText textNode = vNode.toText();
                     if (textNode.isNull() == false) {
                        if (targetSiteNode) {
                           projection.setLaminaeA(textNode.data());
                        }
                        else {
                           projection.setLaminaeB(textNode.data());
                        }
                     }
                  }
               }
            }
         } 
      }
      
      node = node.nextSibling();
   }
}

/**
 * Process a "Density" node.
 */
void
CocomacConnectivityFile::processDensityNode(QDomElement& siteNode,
                                            CocomacProjection& projection)
{
   QDomNode node = siteNode.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == "Degree") {
            QDomNode vNode = elem.firstChild();
            if (vNode.isNull() == false) {
               QDomText textNode = vNode.toText();
               if (textNode.isNull() == false) {
                  projection.setDensity(textNode.data());
               }
            } 
         }
      }
      node = node.nextSibling();
   }
}


void
CocomacConnectivityFile::writeFileData(QTextStream& /*stream*/, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                       QDomElement& /* rootElement */) throw(FileException)
{
   throw FileException(filename, "Writing CoCoMac is not supported");
}


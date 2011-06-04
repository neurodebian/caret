/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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
#ifndef __CIFTI_XML
#define __CIFTI_XML
#include "CiftiXMLElements.h"
#include "CiftiXMLReader.h"
#include "CiftiXMLWriter.h"
#include <QtCore>
class CiftiXML {
public:
   //TODO create initializers for various types of XML meta data (Dense Connectivity, Dense Time Series, etc)
   CiftiXML() { }
   CiftiXML(CiftiRootElement &xml_root) { m_root = xml_root; }
   CiftiXML(const QByteArray &bytes) { readXML(bytes); }
   CiftiXML(const QString &xml_string) { readXML(xml_string); }
   CiftiXML(QXmlStreamReader &xml_stream) { readXML(xml_stream); }
   
   void readXML(const QByteArray &bytes) { QString text(bytes);readXML(text);}
   void readXML(const QString &text) {QXmlStreamReader xml(text); readXML(xml);}
   void readXML(QXmlStreamReader &xml_stream) { parseCiftiXML(xml_stream,m_root);}
   void writeXML(QString &text) { QXmlStreamWriter xml(&text); writeCiftiXML(xml,m_root);}
   void writeXML(QByteArray &bytes) { QXmlStreamWriter xml(&bytes); writeCiftiXML(xml,m_root);}
   

   void setXMLRoot (CiftiRootElement &xml_root) { m_root = xml_root; }
   void getXMLRoot (CiftiRootElement &xml_root) { xml_root = m_root; }
protected:
   CiftiRootElement m_root;
};
#endif//__CIFTI_XML

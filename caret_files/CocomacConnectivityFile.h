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

//
// CoCoMac files are in XML format.   see www.cocomac.org for more information.
//
#ifndef __COCOMAC_CONNECTIVITY_FILE__
#define __COCOMAC_CONNECTIVITY_FILE__ 

class QDomElement;
class QDomNode;

#include "AbstractFile.h"

///  Class for storing a single CoCoMac projection.
class CocomacProjection {
   private:
      QString sourceSite;
      QString targetSite;
      QString laminaeA;
      QString laminaeB;
      QString density;
      
   public:
      CocomacProjection() {}
      
      QString getSourceSite() const { return sourceSite; }
      QString getTargetSite() const { return targetSite; }
      QString getLaminaeA()   const { return laminaeA; }
      QString getLaminaeB()   const { return laminaeB; }
      QString getDensity()    const { return density; }
      
      void setSourceSite(const QString& s) { sourceSite = s; }
      void setTargetSite(const QString& s) { targetSite = s; }
      void setLaminaeA(const QString& s)   { laminaeA = s; }
      void setLaminaeB(const QString& s)   { laminaeB = s; }
      void setDensity(const QString& s)    { density = s; }      
};

/// Class for read/write/storing CoCoMac file
class CocomacConnectivityFile : public AbstractFile {
   private:
      QString version;
      QString exportDate;
      QString dataType;
      QString comments;
      
      /// the CoCoMac projections
      std::vector<CocomacProjection> projections;
      
      /// print a node type (used for debugging)
      void printNodeType(QDomNode& n);
      
      /// process the header node 
      void processHeaderNode(QDomElement& headerNode) throw (FileException);
      
      /// process the connectivity data node
      void processConnectivityNode(QDomElement& connectNode) throw (FileException);
      
      /// process the primary projection node
      void processPrimaryProjectionNode(QDomElement& connectNode) throw (FileException);
      
      /// process a site node
      void processSiteNode(QDomElement& siteNode,
                           CocomacProjection& projection);
      /// process a density node
      void processDensityNode(QDomElement& siteNode,
                              CocomacProjection& projection);
                              
      /// read the file
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write the file
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);
      
   public:
   
      /// constructor
      CocomacConnectivityFile();
      
      /// destructor
      ~CocomacConnectivityFile();
      
      /// append a cocomac file to this one
      void append(CocomacConnectivityFile& ccf, QString& errorMessage);
      
      /// clear the cocomac file
      void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return (getNumberOfCocomacProjections() == 0); }
      
      /// get the comment
      QString getComments() const { return comments; }
      
      /// get the data type
      QString getDataType() const { return dataType; }
      
      /// get the export date
      QString getExportDate() const { return exportDate; }
      
      /// get the number of projections
      int getNumberOfCocomacProjections() const 
         { return (int)projections.size(); }
         
      /// get a projection by index
      CocomacProjection* getCocomacProjection(const int i)
         { return &projections[i]; }
         
      /// get version
      QString getVersion() const { return version; }
      
};

#endif

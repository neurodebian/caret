
#ifndef __NEUROLUCIDA_FILE_H__
#define __NEUROLUCIDA_FILE_H__

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
#include "CellFile.h"
#include "ColorFile.h"
#include "ContourFile.h"

class QDomNode;

// class for a Neurolucida File
class NeurolucidaFile : public AbstractFile {
   public:
      // constructor
      NeurolucidaFile();
      
      // destructor
      ~NeurolucidaFile();
      
      // clear the file
      void clear();
      
      // returns true if the file is isEmpty (contains no data)
      bool empty() const;
      
      // get the number of contours
      int getNumberOfContours() const { return contours.size(); }
      
      // get a contour
      CaretContour* getContour(const int indx);
      
      // get a contour
      const CaretContour* getContour(const int indx) const;
      
      // get the number of markers
      int getNumberOfMarkers() const { return markers.size(); }
      
      // get a marker
      CellData* getMarker(const int indx);
      
      // get a marker
      const CellData* getMarker(const int indx) const;
      
      /// get the number of marker colors
      int getNumberOfMarkerColors() const { return markerColors.size(); }
      
      // get a marker color
      ColorFile::ColorStorage* getMarkerColor(const int indx);

      // get a marker color
      const ColorFile::ColorStorage* getMarkerColor(const int indx) const;

   protected:
      
      // read the file
      void readFileData(QFile& file, 
                        QTextStream& stream, 
                        QDataStream& binStream,     
                        QDomElement& rootElement) throw (FileException);
      
      // write the file
      void writeFileData(QTextStream& stream, 
                         QDataStream& binStream,
                         QDomDocument& xmlDoc,
                         QDomElement& rootElement) throw (FileException);
      
      // parse the xml 
      void parseXML(QDomNode node) throw (FileException);
      
      // process a contour node
      void processContourNode(QDomNode node) throw (FileException);
      
      // process the point
      void processPointNode(QDomNode node,
                            float& xOut,
                            float& yOut,
                            float& zOut,
                            float& diameterOut,
                            QString& sidOut,
                            bool& pointValidOut) throw (FileException);
      
      // process the marker node
      void processMarkerNode(QDomNode node) throw (FileException);
      
      // process the  children
      //void processChildren(QDomNode node) throw (FileException);
      
      /// the contours
      std::vector<CaretContour> contours;
      
      /// the markers
      std::vector<CellData> markers;
      
      /// the marker colors
      std::vector<ColorFile::ColorStorage> markerColors;
      
      /// convert microns to millimeters
      static float micronsToMillimeters;
      
};

#endif // __NEUROLUCIDA_FILE_H__

#ifdef __NEUROLUCIDA_MAIN__
   float NeurolucidaFile::micronsToMillimeters = 0.001;
#endif // __NEUROLUCIDA_MAIN__

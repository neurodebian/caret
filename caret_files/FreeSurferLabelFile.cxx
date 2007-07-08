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
 
#include "FreeSurferLabelFile.h"
#include "SpecFile.h"

/**
 * Constructor.
 */
FreeSurferLabelFile::FreeSurferLabelFile()
   : AbstractFile("Free Surfer Label File", 
                  SpecFile::getFreeSurferLabelFileExtension(),
                  false)
{
}

/**
 * Destructor.
 */
FreeSurferLabelFile::~FreeSurferLabelFile()
{
}

/**
 * clear the file.
 */
void 
FreeSurferLabelFile::clear()
{
   clearAbstractFile();
   labelData.clear();
}

/**
 * returns true if the file is isEmpty (contains no data).
 */
bool 
FreeSurferLabelFile::empty() const
{
   return (getNumberOfLabelItems() == 0);
}

/**
 * get the number of vertices.
 */
int 
FreeSurferLabelFile::getNumberOfLabelItems() const
{
   return labelData.size();
}

/**
 * set number of vertices.
 */
void 
FreeSurferLabelFile::setNumberOfLabelItems(const int numLabelItems)
{
   labelData.resize(numLabelItems);
}

/**
 * get label item.
 */
void 
FreeSurferLabelFile::getLabelItem(const int index, int& vertexNumber,
                              float xyz[3]) const
{
   xyz[0] = labelData[index].xyz[0];
   xyz[1] = labelData[index].xyz[1];
   xyz[2] = labelData[index].xyz[2];
   vertexNumber  = labelData[index].vertexNumber;
}

/**
 * set label item.
 */
void 
FreeSurferLabelFile::setLabelItem(const int index, const int vertexNumber, const float xyz[3]) 
{
   labelData[index].xyz[0] = xyz[0];
   labelData[index].xyz[1] = xyz[1];
   labelData[index].xyz[2] = xyz[2];
   labelData[index].vertexNumber = vertexNumber;
}

/**
 * set label item.
 */
void 
FreeSurferLabelFile::addLabelItem(const int vertexNumber, const float xyz[3])
{
   LabelData ld(vertexNumber, xyz);
   labelData.push_back(ld);
}

/**
 * read  file.
 */
void 
FreeSurferLabelFile::readFileData(QFile& /*file*/, QTextStream& stream, QDataStream&,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Read comment line
   //
   QString commentLine;
   readLine(stream, commentLine);
   
   //
   // read number of items
   //
   QString itemsLine;
   readLine(stream, itemsLine);
   const int numItems = itemsLine.toInt();
   
   //
   // Read until end of file
   //
   int nodeNum;
   float xyz[3], w;
   for (int i = 0; i < numItems; i++) {
      stream >> nodeNum >> xyz[0] >> xyz[1] >> xyz[2] >> w;
      addLabelItem(nodeNum, xyz);
   }
}

/**
 * write  file.
 */
void 
FreeSurferLabelFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException)
{
   //
   // Write comment
   //
   stream << "#!ascii label, written by caret\n";
   
   //
   // Write the number of items
   //
   const int num = getNumberOfLabelItems();
   stream << num << "\n";
   
   //
   // Write the data
   //
   for (int i = 0; i < num; i++) {
      const LabelData& ld = labelData[i];
      stream << ld.vertexNumber << " " << ld.xyz[0] << " " << ld.xyz[1]
             << " " << ld.xyz[2] << " " << 0.0 << "\n";
   }
}

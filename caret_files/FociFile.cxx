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

#include "Caret6ProjectedItem.h"
#include "FileUtilities.h"
#include "GiftiLabelTable.h"
#include "FociFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"

/**
 * Constructor
 */
FociFile::FociFile()
 : CellFile("Foci File", SpecFile::getFociFileExtension())
{
}

/**
 * Destructor
 */
FociFile::~FociFile()
{
}

/**
 * Write the file's memory in caret6 format to the specified name.
 */
QString
FociFile::writeFileInCaret6Format(const QString& filenameIn, Structure structureIn,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException)
{
   int numFoci = this->getNumberOfCells();
   if (numFoci <= 0) {
      throw FileException("Contains no foci");
   }

   QString name = FileUtilities::filenameWithoutExtension(filenameIn)
                + SpecFile::getFociProjectionFileExtension();
   QFile file(name);
   if (file.open(QFile::WriteOnly) == false) {
      throw FileException("Unable to open for writing");
   }
   QTextStream stream(&file);

   XmlGenericWriter xmlWriter(stream);
   xmlWriter.writeStartDocument();

   XmlGenericWriterAttributes attributes;
   attributes.addAttribute("xmlns:xsi",
                           "http://www.w3.org/2001/XMLSchema-instance");
   attributes.addAttribute("xsi:noNamespaceSchemaLocation",
                           "http://brainvis.wustl.edu/caret6/xml_schemas/FociProjectionFileSchema.xsd");
   attributes.addAttribute("CaretFileType", "FociProjection");
   attributes.addAttribute("Version", "6.0");
   xmlWriter.writeStartElement("CaretDataFile", attributes);

   this->writeHeaderXMLWriter(xmlWriter);

   GiftiLabelTable labelTable;
   if (colorFileIn != NULL) {
      labelTable.createLabelsFromColors(*colorFileIn);
   }
   labelTable.writeAsXML(xmlWriter);

   for (int n = 0; n < numFoci; n++) {
      CellData* cd = this->getCell(n);
      Structure structure;
      structure.setType(Structure::STRUCTURE_TYPE_INVALID);
      if (structureIn.isInvalid() == false) {
         structure = structureIn;
      }

      //
      // Ignore foci not on cortex or cerebellum or ambiguous
      //
      if (structure.isInvalid()) {
         continue;
      }

      XmlGenericWriterAttributes attributes;
      attributes.addAttribute("Index", n);
      xmlWriter.writeStartElement("FocusProjection", attributes);

      xmlWriter.writeElementCData("Name", cd->getName());
      xmlWriter.writeElementCharacters("SearchXYZ",
              StringUtilities::combine(cd->getSearchXYZ(), 3, " "));
      xmlWriter.writeElementCData("Geography", cd->getGeography());
      xmlWriter.writeElementCData("Area", cd->getArea());
      xmlWriter.writeElementCData("RegionOfInterest", cd->getRegionOfInterest());
      xmlWriter.writeElementCharacters("Size", cd->getSize());
      xmlWriter.writeElementCData("Statistic", cd->getStatistic());
      xmlWriter.writeElementCData("Comment", cd->getComment());
      xmlWriter.writeElementCData("ClassName", cd->getClassName());
      xmlWriter.writeElementCData("SumsIDNumber", cd->getSumsIDNumber());
      xmlWriter.writeElementCData("SumsRepeatNumber", cd->getSumsRepeatNumber());
      xmlWriter.writeElementCData("SumsParentCellBaseID", cd->getSumsParentCellBaseID());
      xmlWriter.writeElementCData("SumsVersionNumber", cd->getSumsVersionNumber());
      xmlWriter.writeElementCData("SumsMSLID", cd->getSumsMSLID());
      xmlWriter.writeElementCData("AttributeID", cd->getAttributeID());


      Caret6ProjectedItem pi;
      pi.projectionType = Caret6ProjectedItem::UNPROJECTED;
      cd->getXYZ(pi.xyz);
      pi.structure = structure;

      pi.writeXML(xmlWriter);

      xmlWriter.writeEndElement();
   }

   xmlWriter.writeEndElement();

   xmlWriter.writeEndDocument();

   file.close();

   return name;
}

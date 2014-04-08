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
#include "Caret7ProjectedItem.h"
#include "FileUtilities.h"
#include "FociProjectionFile.h"
#include "GiftiLabelTable.h"
#include "MathUtilities.h"
#include "SpecFile.h"
#include "StringUtilities.h"
#include "XmlGenericWriter.h"

/**
 * Constructor
 */
FociProjectionFile::FociProjectionFile()
 : CellProjectionFile("Foci Projection File", SpecFile::getFociProjectionFileExtension())
{
}

/**
 * Destructor
 */
FociProjectionFile::~FociProjectionFile()
{
}

/**
 * Write the file's memory in caret6 format to the specified name.
 */
QString
FociProjectionFile::writeFileInCaret6Format(const QString& filenameIn, Structure structureIn,const ColorFile* colorFileIn, const bool useCaret6ExtensionFlag) throw (FileException)
{
   int numFoci = this->getNumberOfCellProjections();
   if (numFoci <= 0) {
      throw FileException("Contains no foci");
   }
   QFile file(filenameIn);
   if (AbstractFile::getOverwriteExistingFilesAllowed() == false) {
      if (file.exists()) {
         throw FileException("file exists and overwrite is prohibited.");
      }
   }
   if (file.open(QFile::WriteOnly) == false) {
      throw FileException("Unable to open for writing");
   }
   QTextStream stream(&file);

   XmlGenericWriter xmlWriter(stream);
   xmlWriter.writeStartDocument();

   XmlGenericWriterAttributes attributes;
   attributes.addAttribute("CaretFileType", "FociProjection");
   attributes.addAttribute("xmlns:xsi",
                           "http://www.w3.org/2001/XMLSchema-instance");
   attributes.addAttribute("xsi:noNamespaceSchemaLocation",
                           "http://brainvis.wustl.edu/caret6/xml_schemas/FociProjectionFileSchema.xsd");
   attributes.addAttribute("Version", "6.0");
   xmlWriter.writeStartElement("CaretDataFile", attributes);

   this->writeHeaderXMLWriter(xmlWriter);

   GiftiLabelTable labelTable;
   if (colorFileIn != NULL) {
      labelTable.createLabelsFromColors(*colorFileIn);
   }
   labelTable.writeAsXML(xmlWriter);

   int fociNumber = 0;
   for (int n = 0; n < numFoci; n++) {
      CellProjection* fp = this->getCellProjection(n);
      if (fp->getDuplicateFlag()) {
         continue;
      }
      Structure structure;
      structure.setType(Structure::STRUCTURE_TYPE_INVALID);
      if (fp->structure.isLeftCortex()) {
         structure.setType(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
      }
      else if (fp->structure.isRightCortex()) {
         structure.setType(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
      }
      else if (fp->structure.isCerebellum()) {
         structure.setType(Structure::STRUCTURE_TYPE_CEREBELLUM);
      }
      else if (structureIn.isInvalid() == false) {
         structure = structureIn;
      }

      //
      // Ignore foci not on cortex or cerebellum or ambiguous
      //
      //if (structure.isInvalid()) {
      //   continue;
      //}

      XmlGenericWriterAttributes attributes;
      attributes.addAttribute("Index", fociNumber);
      xmlWriter.writeStartElement("FocusProjection", attributes);

      xmlWriter.writeElementCData("Name", fp->getName());
      xmlWriter.writeElementCharacters("SearchXYZ",
              StringUtilities::combine(fp->searchXYZ, 3, " "));
      xmlWriter.writeElementCData("Geography", fp->getGeography());
      xmlWriter.writeElementCData("Area", fp->getArea());
      xmlWriter.writeElementCData("RegionOfInterest", fp->getRegionOfInterest());
      xmlWriter.writeElementCharacters("Size", fp->getSize());
      xmlWriter.writeElementCData("Statistic", fp->getStatistic());
      xmlWriter.writeElementCData("Comment", fp->getComment());
      xmlWriter.writeElementCData("ClassName", fp->getClassName());
      xmlWriter.writeElementCData("SumsIDNumber", fp->getSumsIDNumber());
      xmlWriter.writeElementCData("SumsRepeatNumber", fp->getSumsRepeatNumber());
      xmlWriter.writeElementCData("SumsParentFocusBaseID", fp->getSumsParentCellBaseID());
      xmlWriter.writeElementCData("SumsVersionNumber", fp->getSumsVersionNumber());
      xmlWriter.writeElementCData("SumsMSLID", fp->getSumsMSLID());
      xmlWriter.writeElementCData("AttributeID", fp->getAttributeID());
      fp->getStudyMetaDataLinkSet().writeXML(xmlWriter);

      Caret6ProjectedItem pi;
      pi.projectionType = Caret6ProjectedItem::UNPROJECTED;
      switch (fp->projectionType) {
         case CellProjection::PROJECTION_TYPE_UNKNOWN:
            pi.projectionType = Caret6ProjectedItem::UNPROJECTED;
            break;
         case CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE:
            pi.projectionType = Caret6ProjectedItem::BARYCENTRIC;
            //
            // Area and node indices do not match in Caret5 but
            // they do in Caret6
            //  In Caret5:  node-index area-index
            //                   0         1
            //                   1         2
            //                   2         0
            //
            // But caret5 was clockwise order and we want
            // counter clockwise so
            //
            //    node-index  area-index
            //        2            0
            //        1            2
            //        0            1
            //
            //
            pi.closestTriangleAreas[0] = fp->closestTileAreas[0];
            pi.closestTriangleAreas[1] = fp->closestTileAreas[2];
            pi.closestTriangleAreas[2] = fp->closestTileAreas[1];
            pi.closestTriangleVertices[0] = fp->closestTileVertices[2];
            pi.closestTriangleVertices[1] = fp->closestTileVertices[1];
            pi.closestTriangleVertices[2] = fp->closestTileVertices[0];
            pi.signedDistanceAboveSurface = fp->signedDistanceAboveSurface;
            break;
         case CellProjection::PROJECTION_TYPE_OUTSIDE_TRIANGLE:
            pi.projectionType = Caret6ProjectedItem::VANESSEN;
            pi.dR = fp->dR;
            for (int i = 0; i < 2; i++) {
               for (int j = 0; j < 3; j++) {
                  for (int k = 0; k < 3; k++) {
                     pi.triAnatomical[i][j][k] = fp->triFiducial[i][j][k];
                  }
                  pi.triVertices[i][j] = fp->triVertices[i][j];
                  pi.vertexAnatomical[i][j] = fp->vertexFiducial[i][j];
               }
               pi.vertex[i] = fp->vertex[i];
            }

            for (int i = 0; i < 3; i++) {
               pi.posAnatomical[i] = fp->posFiducial[i];
            }
            pi.thetaR = fp->thetaR;
            pi.phiR = fp->phiR;
            pi.fracRI = fp->fracRI;
            pi.fracRJ = fp->fracRJ;
            break;
      }

      for (int i = 0; i < 3; i++) {
         pi.volumeXYZ[i] = fp->volumeXYZ[i];
         pi.xyz[i] = fp->xyz[i];
      }

      pi.structure = structure;

      pi.writeXML(xmlWriter);

      xmlWriter.writeEndElement();

      fociNumber++;
   }

   xmlWriter.writeEndElement();

   xmlWriter.writeEndDocument();

   file.close();

   return filenameIn;
}

/**
 * Write the file's memory in caret7 format to the specified name.
 */
QString
FociProjectionFile::writeFileInCaret7Format(const QString& filenameIn, 
                                              Structure structureIn,
                                              const ColorFile* colorFileIn, 
                                              const bool useCaret7ExtensionFlag) throw (FileException)
{
    int numFoci = this->getNumberOfCellProjections();
    if (numFoci <= 0) {
        throw FileException("Contains no foci");
    }
    QString name = filenameIn;
    if (useCaret7ExtensionFlag) {
        name = FileUtilities::replaceExtension(filenameIn, ".fociproj",
                                               ".foci");
    }
    QFile file(name);
    if (AbstractFile::getOverwriteExistingFilesAllowed() == false) {
        if (file.exists()) {
            throw FileException("file exists and overwrite is prohibited.");
        }
    }
    if (file.open(QFile::WriteOnly) == false) {
        throw FileException("Unable to open for writing");
    }
    QTextStream stream(&file);
    
    XmlGenericWriter xmlWriter(stream);
    xmlWriter.writeStartDocument();
    
    XmlGenericWriterAttributes attributes;
    attributes.addAttribute("Version", "1.0");
    xmlWriter.writeStartElement("FociFile", attributes);
    
    this->writeHeaderXMLWriter(xmlWriter);
    
    GiftiLabelTable labelTable;
    if (colorFileIn != NULL) {
        labelTable.createLabelsFromColors(*colorFileIn);
    }
    labelTable.writeAsXML(xmlWriter);
    
    int fociNumber = 0;
    for (int n = 0; n < numFoci; n++) {
        CellProjection* fp = this->getCellProjection(n);
        if (fp->getDuplicateFlag()) {
            continue;
        }
        Structure structure;
        structure.setType(Structure::STRUCTURE_TYPE_INVALID);
        if (fp->structure.isLeftCortex()) {
            structure.setType(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
        }
        else if (fp->structure.isRightCortex()) {
            structure.setType(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
        }
        else if (fp->structure.isCerebellum()) {
            structure.setType(Structure::STRUCTURE_TYPE_CEREBELLUM);
        }
        else if (structureIn.isInvalid() == false) {
            structure = structureIn;
        }
        
        //
        // Ignore foci not on cortex or cerebellum or ambiguous
        //
        //if (structure.isInvalid()) {
        //   continue;
        //}
        
        XmlGenericWriterAttributes attributes;
        attributes.addAttribute("Index", fociNumber);
        xmlWriter.writeStartElement("Focus", attributes);
        
        xmlWriter.writeElementCData("Name", fp->getName());
        xmlWriter.writeElementCharacters("SearchXYZ",
                                         StringUtilities::combine(fp->searchXYZ, 3, " "));
        xmlWriter.writeElementCData("Geography", fp->getGeography());
        xmlWriter.writeElementCData("Area", fp->getArea());
        xmlWriter.writeElementCData("RegionOfInterest", fp->getRegionOfInterest());
        xmlWriter.writeElementCharacters("Size", fp->getSize());
        xmlWriter.writeElementCData("Statistic", fp->getStatistic());
        xmlWriter.writeElementCData("Comment", fp->getComment());
        xmlWriter.writeElementCData("ClassName", fp->getClassName());
        xmlWriter.writeElementCData("SumsIDNumber", fp->getSumsIDNumber());
        xmlWriter.writeElementCData("SumsRepeatNumber", fp->getSumsRepeatNumber());
        xmlWriter.writeElementCData("SumsParentFocusBaseID", fp->getSumsParentCellBaseID());
        xmlWriter.writeElementCData("SumsVersionNumber", fp->getSumsVersionNumber());
        xmlWriter.writeElementCData("SumsMSLID", fp->getSumsMSLID());
        xmlWriter.writeElementCData("AttributeID", fp->getAttributeID());
        fp->getStudyMetaDataLinkSet().writeXML(xmlWriter);
        
        Caret7ProjectedItem pi;
        pi.projectionType = Caret7ProjectedItem::UNPROJECTED;
        switch (fp->projectionType) {
            case CellProjection::PROJECTION_TYPE_UNKNOWN:
                pi.projectionType = Caret7ProjectedItem::UNPROJECTED;
                break;
            case CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE:
                pi.projectionType = Caret7ProjectedItem::BARYCENTRIC;
                //
                // Area and node indices do not match in Caret5 but
                // they do in Caret7
                //  In Caret5:  node-index area-index
                //                   0         1
                //                   1         2
                //                   2         0
                //
                // But caret5 was clockwise order and we want
                // counter clockwise so
                //
                //    node-index  area-index
                //        2            0
                //        1            2
                //        0            1
                //
                //
                pi.closestTriangleAreas[0] = fp->closestTileAreas[0];
                pi.closestTriangleAreas[1] = fp->closestTileAreas[2];
                pi.closestTriangleAreas[2] = fp->closestTileAreas[1];
                pi.closestTriangleVertices[0] = fp->closestTileVertices[2];
                pi.closestTriangleVertices[1] = fp->closestTileVertices[1];
                pi.closestTriangleVertices[2] = fp->closestTileVertices[0];
                pi.signedDistanceAboveSurface = fp->signedDistanceAboveSurface;
                
                if (pi.signedDistanceAboveSurface == 0.0) {
                    /*
                     * cdistance is obsolete distance above tile
                     */
                    const float dist = MathUtilities::normalize(fp->cdistance);
                    pi.signedDistanceAboveSurface = dist;
                }
                break;
            case CellProjection::PROJECTION_TYPE_OUTSIDE_TRIANGLE:
                pi.projectionType = Caret7ProjectedItem::VANESSEN;
                pi.dR = fp->dR;
                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 3; j++) {
                        for (int k = 0; k < 3; k++) {
                            pi.triAnatomical[i][j][k] = fp->triFiducial[i][j][k];
                        }
                        pi.triVertices[i][j] = fp->triVertices[i][j];
                        pi.vertexAnatomical[i][j] = fp->vertexFiducial[i][j];
                    }
                    pi.vertex[i] = fp->vertex[i];
                }
                
                for (int i = 0; i < 3; i++) {
                    pi.posAnatomical[i] = fp->posFiducial[i];
                }
                pi.thetaR = fp->thetaR;
                pi.phiR = fp->phiR;
                pi.fracRI = fp->fracRI;
                pi.fracRJ = fp->fracRJ;
                break;
        }
        
        for (int i = 0; i < 3; i++) {
            pi.volumeXYZ[i] = fp->volumeXYZ[i];
            pi.xyz[i] = fp->xyz[i];
        }
        
        pi.structure = structure;
        
        pi.writeXML(xmlWriter);
        
        xmlWriter.writeEndElement();
        
        fociNumber++;
    }
    
    xmlWriter.writeEndElement();
    
    xmlWriter.writeEndDocument();
    
    file.close();
    
    return name;    
}



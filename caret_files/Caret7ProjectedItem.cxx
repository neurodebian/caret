
#include <QTextStream>

#include "Caret7ProjectedItem.h"
#include "StringUtilities.h"
#include "XmlGenericWriter.h"

/**
 *  constructor.
 */
Caret7ProjectedItem::Caret7ProjectedItem() {
   projectionType = Caret7ProjectedItem::UNPROJECTED;
   for (int i = 0; i < 3; i++) {
      xyz[i] = 0.0;
      closestTriangleVertices[i] = 0.0;;
      closestTriangleAreas[i] = 0.0;;
      volumeXYZ[i] = 0.0;
      posAnatomical[i] = 0.0;
   }
   signedDistanceAboveSurface = 0.0;
   dR = 0.0;
   for (int i = 0; i < 2; i++) {
      vertex[i] = 0;
      for (int j = 0; j < 3; j++) {
         for (int k = 0; k < 3; k++) {
            triAnatomical[i][j][k] = 0.0;
         }
         triVertices[i][j] = 0.0;
         vertexAnatomical[i][j] = 0.0;
      }
   }
   thetaR = 0.0;
   phiR = 0.0;
   fracRI = 0.0;
   fracRJ = 0.0;
   structure.setType(Structure::STRUCTURE_TYPE_INVALID);
}

/**
 *  destructor.
 */
Caret7ProjectedItem::~Caret7ProjectedItem() {

}

/**
 *  write projected item in XML.
 */
void
Caret7ProjectedItem::writeXML(XmlGenericWriter& xmlWriter) {
   xmlWriter.writeStartElement("SurfaceProjectedItem");
   
   QString structureName = "Invalid";
   if (this->structure.isLeftCortex()) {
      structureName = "CORTEX_LEFT";
   }
   else if (this->structure.isRightCortex()) {
      structureName = "CORTEX_RIGHT";
   }
   else if (this->structure.isCerebellum()) {
      structureName = "CEREBELLUM";
   }
   xmlWriter.writeElementCharacters("Structure",
                                    structureName);

    if ((this->xyz[0] != 0.0)
        || (this->xyz[1] != 0.0)
        || (this->xyz[2] != 0.0)) {
        xmlWriter.writeElementCharacters("StereotaxicXYZ",
                                         this->xyz, 3);
    }
    
    if ((this->volumeXYZ[0] != 0.0)
        || (this->volumeXYZ[1] != 0.0)
        || (this->volumeXYZ[2] != 0.0)) {
        xmlWriter.writeElementCharacters("VolumeXYZ",
                                         this->volumeXYZ, 3);
    }
   
   switch (this->projectionType) {
      case Caret7ProjectedItem::UNPROJECTED:
         {
            //xmlWriter.writeElementCharacters("Unprojected", "");
         }
         break;
      case Caret7ProjectedItem::BARYCENTRIC:
         {
            xmlWriter.writeStartElement("ProjectionBarycentric");
            xmlWriter.writeElementCharacters("TriangleNodes",
                                             this->closestTriangleVertices, 3);
            xmlWriter.writeElementCharacters("TriangleAreas",
                                             this->closestTriangleAreas, 3);
             if (signedDistanceAboveSurface != 0.0) {
                 xmlWriter.writeElementCharacters("SignedDistanceAboveSurface",
                                                  signedDistanceAboveSurface);
             }
            xmlWriter.writeEndElement();
         }
         break;
      case Caret7ProjectedItem::VANESSEN:
            xmlWriter.writeStartElement("VanEssenProjection");
            xmlWriter.writeElementCharacters("DR",
                                             this->dR);
            xmlWriter.writeElementCharacters("TriAnatomical",
                                             (float*)this->triAnatomical, 18);
            xmlWriter.writeElementCharacters("ThetaR",
                                             this->thetaR);
            xmlWriter.writeElementCharacters("PhiR",
                                             this->phiR);
            xmlWriter.writeElementCharacters("TriVertices",
                                             (int*)this->triVertices, 6);
            xmlWriter.writeElementCharacters("Vertex",
                                             this->vertex, 2);
            xmlWriter.writeElementCharacters("VertexAnatomical",
                                             (float*)this->vertexAnatomical, 6);
            xmlWriter.writeElementCharacters("PosAnatomical",
                                             this->posAnatomical, 3);
            xmlWriter.writeElementCharacters("FracRI",
                                             this->fracRI);
            xmlWriter.writeElementCharacters("FracRJ",
                                             this->fracRJ);
            xmlWriter.writeEndElement();
         break;
   }

    xmlWriter.writeEndElement();
}



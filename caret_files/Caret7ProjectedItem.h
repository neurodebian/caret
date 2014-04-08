/* 
 * File:   Caret7ProjectedItem.h
 * Author: john
 *
 * Created on November 6, 2009, 10:10 AM
 */

#ifndef __CARET7_PROJECTED_ITEM_H__
#define	__CARET7_PROJECTED_ITEM_H__

#include "Structure.h"

class CellProjection;
class BorderProjection;
class XmlGenericWriter;


// stores projected item
class Caret7ProjectedItem {
   public:
      /// projection type
      enum ProjectionType {
         UNPROJECTED,
         BARYCENTRIC,
         VANESSEN
      };

      /// constructor
      Caret7ProjectedItem();

      /// destructor
      ~Caret7ProjectedItem();

      /// write projected item in XML
      void writeXML(XmlGenericWriter& xmlWriter);

      /// projection type
      ProjectionType projectionType;

      /// unprojected position
      float xyz[3];

      /// BARYCENTRIC TRIANGLE PROJECTION vertices
      int closestTriangleVertices[3];

      /// BARYCENTRIC TRIANGLE PROJECTION tile areas
      float closestTriangleAreas[3];

      /// BARYCENTRIC TRIANGLE PROJECTION distance above surface
      float signedDistanceAboveSurface;

      /// OUTSIDE TRIANGLE DR
      float dR;

      /// OUTSIDE TRIANGLE  anatomical coords
      float triAnatomical[2][3][3];

      /// OUTSIDE TRIANGLE theta
      float thetaR;

      /// OUTSIDE TRIANGLE phi
      float phiR;

      /// OUTSIDE TRIANGLE triangle vertices
      int   triVertices[2][3];

      /// OUTSIDE TRIANGLE vertices
      int   vertex[2];

      /// OUTSIDE TRIANGLE anatomical vertices
      float vertexAnatomical[2][3];

      /// OUTSIDE TRIANGLE anatomical position
      float posAnatomical[3];  // cell's Anatomical surface position

      /// OUTSIDE TRIANGLE fracRI
      float fracRI;

      /// OUTSIDE TRIANGLE fracRJ
      float fracRJ;

      /// position of focus in a volume
      float volumeXYZ[3];

      Structure structure;
};


#endif	/* __CARET7_PROJECTED_ITEM_H__ */


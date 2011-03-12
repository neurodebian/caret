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

#include <cmath>
#include <iostream>
#include <set>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>
#include <QXmlSimpleReader>

#define __CELL_PROJECTION_FILE_MAIN__
#define __CELL_PROJECTION_MAIN__
#include "CellProjectionFile.h"
#undef __CELL_PROJECTION_MAIN__
#undef __CELL_PROJECTION_FILE_MAIN__

#include "CellClass.h"
#include "CellFile.h"
#include "ColorFile.h"
#include "CommaSeparatedValueFile.h"
#include "CoordinateFile.h"
#include "MathUtilities.h"
#include "NameIndexSort.h"
#include "SpecFile.h"
#include "StringTable.h"
#include "StringUtilities.h"
#include "StudyMetaDataFile.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include "TransformationMatrixFile.h"
#include "vtkMath.h"

/**
 * Constructor.
 */
CellProjection::CellProjection(const QString& filenameIn)
{
   initialize(filenameIn);
}

/**
 * Constructor for placing cell projection at a node (no need to project).
 */
CellProjection::CellProjection(const QString& nameIn,
                               const CoordinateFile* fiducialCoordinateFile,
                               const int nodeNumber,
                               const Structure& structureIn)
{
   initialize();
   const float* fidXYZ = fiducialCoordinateFile->getCoordinate(nodeNumber);
   setXYZ(fidXYZ);
   setName(nameIn);
   
   //
   // Set fiducial position
   //
   posFiducial[0] = fidXYZ[0];
   posFiducial[1] = fidXYZ[1];
   posFiducial[2] = fidXYZ[2];

   //
   // save original hemisphere
   //
   if (structureIn.getType() != Structure::STRUCTURE_TYPE_INVALID) {
      structure = structureIn;
   }
   else {
      if (fidXYZ[0] < 0.0) {
         structure.setType(Structure::STRUCTURE_TYPE_CORTEX_LEFT);
      }
      else {
         structure.setType(Structure::STRUCTURE_TYPE_CORTEX_RIGHT);
      }
   }
   
   //
   // Project
   //
   setSignedDistanceAboveSurface(0.0);
   projectionType = CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE;

   closestTileVertices[0] = nodeNumber;
   closestTileVertices[1] = nodeNumber;
   closestTileVertices[2] = nodeNumber;
   closestTileAreas[0] = 0.33333;
   closestTileAreas[1] = 0.33333;
   closestTileAreas[2] = 0.33333;
   cdistance[0] = 0.0;
   cdistance[1] = 0.0;
   cdistance[2] = 0.0;
   
   volumeXYZ[0] = fidXYZ[0];
   volumeXYZ[1] = fidXYZ[1];
   volumeXYZ[2] = fidXYZ[2];
}
                                      
/**
 * Initialize the cell projection
 */
void
CellProjection::initialize(const QString& filenameIn)
{
   filename = filenameIn;
   CellBase::initialize();
   uniqueNameIndex = -1;
   cellProjectionFile = NULL;
   projectionType = PROJECTION_TYPE_UNKNOWN;
   duplicateFlag = false;
   
   for (int i = 0; i < 3; i++) {
      posFiducial[i] = 0.0;
      closestTileVertices[i] = 0;
      closestTileAreas[i] = 0.0;
      cdistance[i] = 0.0;
   }
   
   for (int j = 0; j < 2; j++) {
      vertex[j] = -1;
      for (int k = 0; k < 3; k++) {
         vertexFiducial[j][k] = 0.0;
         triVertices[j][k] = -1;
         for (int m = 0; m < 3; m++) {
            triFiducial[j][k][m] = 0.0;
         }
      }
   }
   
   dR = 0.0;
   thetaR = 0.0;
   phiR = 0.0;
   fracRI = 0.0;
   fracRJ = 0.0;
   
   volumeXYZ[0] = 0.0;
   volumeXYZ[1] = 0.0;
   volumeXYZ[2] = 0.0;
}

/**
 * update modification status
 */
void
CellProjection::setModified() 
{
   if (cellProjectionFile != NULL) {
      cellProjectionFile->setModified();
   }
}

/**
 * Destructor.
 */
CellProjection::~CellProjection()
{
}

/**
 * Get the projected position of this cell (returns true if valid projection).
 */
bool 
CellProjection::getProjectedPosition(const CoordinateFile* cf,
                                     const TopologyFile* tf,
                                     const bool fiducialSurfaceFlag,
                                     const bool flatSurfaceFlag,
                                     const bool pasteOntoSurfaceFlagIn,
                                     float xyzOut[3]) const
{
   //
   // Do not use paste onto surface if the surface is flat
   //
   bool pasteOntoSurfaceFlag = pasteOntoSurfaceFlagIn;
   if (flatSurfaceFlag) {
      pasteOntoSurfaceFlag = false;
   }
   
   bool valid = false;
   
   switch(projectionType) {
      case CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE:
         valid = unprojectInsideTriangle(*cf, *tf, pasteOntoSurfaceFlag, xyzOut);
         break;
      case CellProjection::PROJECTION_TYPE_OUTSIDE_TRIANGLE:
         valid = unprojectOutsideTriangle(*cf, *tf, pasteOntoSurfaceFlag, xyzOut);
         break;
      case CellProjection::PROJECTION_TYPE_UNKNOWN:
         if (fiducialSurfaceFlag) {
            if ((xyz[0] != 0.0) ||
                (xyz[1] != 0.0) ||
                (xyz[2] != 0.0)) {
               xyzOut[0] = xyz[0];
               xyzOut[1] = xyz[1];
               xyzOut[2] = xyz[2];
               valid = true;
            }
            else if ((posFiducial[0] != 0.0) ||
                     (posFiducial[1] != 0.0) ||
                     (posFiducial[2] != 0.0)) {
               xyzOut[0] = posFiducial[0];
               xyzOut[1] = posFiducial[1];
               xyzOut[2] = posFiducial[2];
               valid = true;
            }
         }
         break;
   }
   
   //
   // If flat surface and cells were to be pasted onto the surface, 
   // just push cells above flat surface
   //
   if (flatSurfaceFlag && pasteOntoSurfaceFlagIn) {
      xyzOut[2] = 1.0;
   }
   
   return valid;
}

/**
 * Unproject an outside triangle projections.
 */
bool 
CellProjection::unprojectOutsideTriangle(const CoordinateFile& cf,
                                         const TopologyFile& tf,
                                         const bool pasteOntoSurfaceFlag,
                                         float xyzOut[3]) const
{
   const int is = 0;
   const int js = 1;
   
   const TopologyHelper* th = tf.getTopologyHelper(false, true, false);
   if ((th->getNodeHasNeighbors(vertex[is]) == false) ||
       (th->getNodeHasNeighbors(vertex[js]) == false)) {
      return false;
   }

   float v[3], v_t1[3];
   MathUtilities::subtractVectors(vertexFiducial[js], vertexFiducial[is], v);
   MathUtilities::subtractVectors(posFiducial, vertexFiducial[is], v_t1);
   
   float s_t2 = MathUtilities::dotProduct(v, v);
   float s_t3 = MathUtilities::dotProduct(v_t1, v);
   float QR[3];
   for (int j = 0; j < 3; j++) {
      QR[j] = vertexFiducial[is][j] + ((s_t3/s_t2) * v[j]);
   }
   
   const int pis = vertex[0];
   const int pjs = vertex[1];
   
   const float* posPIS = cf.getCoordinate(pis);
   const float* posPJS = cf.getCoordinate(pjs);

   if (pasteOntoSurfaceFlag) {
      xyzOut[0] = (posPIS[0] + posPJS[0]) / 2.0;
      xyzOut[1] = (posPIS[1] + posPJS[1]) / 2.0;
      xyzOut[2] = (posPIS[2] + posPJS[2]) / 2.0;
      return true;
   }
   

   MathUtilities::subtractVectors(posPJS, posPIS, v);
   
   float QS[3];
   
   if ((fracRI <= 1.0) && (fracRJ <= 1.0)) {
      for (int j = 0; j < 3; j++) {
         QS[j] = posPIS[j] + fracRI * v[j];
      }
   }
   else if ((fracRI > 1.0) && (fracRI > fracRJ)) {
      MathUtilities::subtractVectors(QR, vertexFiducial[js], v_t1);
      s_t2 = MathUtilities::vectorLength(v_t1);
      
      MathUtilities::subtractVectors(posPJS, posPIS, v);
      s_t3 = MathUtilities::vectorLength(v);
      for (int j = 0; j < 3; j++) {
         QS[j] = posPJS[j] + s_t2 * (v[j]/s_t3);
      }
   }
   else if ((fracRJ > 1.0) && (fracRJ > fracRI)) {
      MathUtilities::subtractVectors(QR, vertexFiducial[is], v_t1);
      s_t2 = MathUtilities::vectorLength(v_t1);
      
      MathUtilities::subtractVectors(posPIS, posPJS, v);
      s_t3 = MathUtilities::vectorLength(v);
      for (int j = 0; j < 3; j++) {
         QS[j] = posPIS[j] + s_t2 * (v[j]/s_t3);
      }
   }
   else {
     // printf("CellProjectionFile: Unrecognized case for fracRI and fracRJ: "
     //        "%.2f %.2f\n", fracRI, fracRJ);
      return false;
   }
   
   if ((triVertices[0][0] < 0) || (triVertices[1][0] < 0)) {
      return false;
   }
   
   float normalB[3]; 
   MathUtilities::computeNormal((float*)cf.getCoordinate(triVertices[1][0]),
                  (float*)cf.getCoordinate(triVertices[1][1]),
                  (float*)cf.getCoordinate(triVertices[1][2]),
                  normalB);
   
   float normalA[3];
   MathUtilities::computeNormal((float*)cf.getCoordinate(triVertices[0][0]),
                  (float*)cf.getCoordinate(triVertices[0][1]),
                  (float*)cf.getCoordinate(triVertices[0][2]),
                  normalA);
             
   s_t2 = MathUtilities::dotProduct(normalA, normalB);
   s_t2 = std::min(s_t2, (float)1.0); // limit to <= 1.0
   float phiS = std::acos(s_t2);
   
   float thetaS = 0.0;
   if (phiR > 0.0) {
      thetaS = (thetaR/phiR) * phiS;
   }
   else {
      thetaS = 0.5 * phiS;
   }
   
   MathUtilities::subtractVectors(posPJS, posPIS, v);
   MathUtilities::normalize(v);
   
   float v_t3[3];
   MathUtilities::crossProduct(normalA, v, v_t3);
   
   float projection[3] = { 0.0, 0.0, 0.0 };
   computeProjectionPoint(projection);

   MathUtilities::subtractVectors(projection, QR, v_t1);
   MathUtilities::normalize(v_t1);
   
   MathUtilities::subtractVectors(vertexFiducial[js], vertexFiducial[is], v);
   MathUtilities::normalize(v);
   
   float normalA_3D[3];
   MathUtilities::computeNormal(triFiducial[0][0],
               triFiducial[0][1],
               triFiducial[0][2],
               normalA_3D);
   float v_t2[3];
   MathUtilities::crossProduct(normalA_3D, v, v_t2);
   s_t3 = MathUtilities::dotProduct(v_t1, v_t2);
   
   float TS[3];
   for (int k = 0; k < 3; k++) {
      TS[k] = QS[k] + (s_t3 * (dR * std::sin(thetaS)) * v_t2[k]);
   }
   
   MathUtilities::subtractVectors(posFiducial, projection, v);
   MathUtilities::normalize(v);
   
   s_t3 = MathUtilities::dotProduct(normalA_3D, v);
   
   for (int i = 0; i < 3; i++) {
      xyzOut[i] = TS[i] + (dR * s_t3 * std::cos(thetaS)) * normalA[i];
   }
   
   return true;
}

/**
 * Compute the projection point.
 */
void
CellProjection::computeProjectionPoint(float projection[3]) const
{
   float v[3], w[3], tnormal[3];
   MathUtilities::subtractVectors(triFiducial[0][1],
                                  triFiducial[0][0], v);
   MathUtilities::subtractVectors(triFiducial[0][1],
                    triFiducial[0][2], w);
   MathUtilities::crossProduct(w, v, tnormal);
   
   float a[3][3];
   for (int k = 0; k < 3; k++) {
      a[0][k] = v[k];
      a[1][k] = w[k];
      a[2][k] = tnormal[k];
   }
   
   float b[3];
   b[0] = MathUtilities::dotProduct(v, posFiducial);
   b[1] = MathUtilities::dotProduct(w, posFiducial);
   b[2] = MathUtilities::dotProduct(tnormal, triFiducial[0][2]);
   
   
   vtkMath::LinearSolve3x3(a, b, projection);
}

/**
 * Unproject an inside triangle projection.
 */
bool 
CellProjection::unprojectInsideTriangle(const CoordinateFile& cf,
                                        const TopologyFile& tf,
                                        const bool pasteOntoSurfaceFlag,
                                        float xyzOut[3]) const
{
   const float* v1 = cf.getCoordinate(closestTileVertices[0]);
   const float* v2 = cf.getCoordinate(closestTileVertices[1]);
   const float* v3 = cf.getCoordinate(closestTileVertices[2]);

   const TopologyHelper* th = tf.getTopologyHelper(true, true, true);
   if ((th->getNodeHasNeighbors(closestTileVertices[0]) == false) ||
       (th->getNodeHasNeighbors(closestTileVertices[1]) == false) ||
       (th->getNodeHasNeighbors(closestTileVertices[2]) == false)) {
      return false;
   }
   
   float t1[3], t2[3], t3[3];
   for (int i = 0; i < 3; i++) {
      t1[i] = closestTileAreas[0] * v3[i];
      t2[i] = closestTileAreas[1] * v1[i];
      t3[i] = closestTileAreas[2] * v2[i];
   }
   
   const float area = closestTileAreas[0] + closestTileAreas[1] 
                    + closestTileAreas[2];
                    
   float projection[3] = { 0.0, 0.0, 0.0 };
   if (area != 0) {
      for (int i = 0; i < 3; i++) {
         projection[i] = (t1[i] + t2[i] + t3[i]) / area;
      }                                     
   }
   
   //
   // Note: that does caret4 style clockwise orientation
   //
   float tileNormal[3];
   MathUtilities::computeNormal((float*)v3, (float*)v2, (float*)v1, tileNormal);
   
   //
   // Are all of the node the same (focus projects to a single node, not a tile)
   //
   if ((closestTileVertices[0] == closestTileVertices[1]) &&
       (closestTileVertices[1] == closestTileVertices[2])) {
      //
      // Compute node's normal by averaging its tiles' normals
      //
      float normalSum[3] = { 0.0, 0.0, 0.0 };

      int node = closestTileVertices[0];
      int numNeighbors = 0;
      const int* neighbors = th->getNodeNeighbors(node, numNeighbors);

      for (int i = 0; i < numNeighbors; i++) {
         int neigh1 = neighbors[i];
         int nextNeighborIndex = i + 1;
         if (nextNeighborIndex >= numNeighbors) {
            nextNeighborIndex = 0;
         }
         int nextNeighbor = neighbors[nextNeighborIndex];
         const float* c1 = cf.getCoordinate(node);
         const float* c2 = cf.getCoordinate(neigh1);
         const float* c3 = cf.getCoordinate(nextNeighbor);
         float neighborNormal[3];
         MathUtilities::computeNormal((float*)c1, (float*)c2, (float*)c3, neighborNormal);
         normalSum[0] += neighborNormal[0];
         normalSum[1] += neighborNormal[1];
         normalSum[2] += neighborNormal[2];
      }

      if (numNeighbors > 0) {
         tileNormal[0] = normalSum[0] / (float)numNeighbors;
         tileNormal[1] = normalSum[1] / (float)numNeighbors;
         tileNormal[2] = normalSum[2] / (float)numNeighbors;
         MathUtilities::normalize(tileNormal);
      }
   }

   for (int j = 0; j < 3; j++) {
      if (pasteOntoSurfaceFlag) {
         xyzOut[j] = projection[j];
      }
      else if (signedDistanceAboveSurface != 0.0) {
         xyzOut[j] = projection[j] + tileNormal[j] * signedDistanceAboveSurface;
      }
      else {
         xyzOut[j] = projection[j] + cdistance[j];
      }
   }
   
   return true;
}
 
/**
 * Set the cell's class name
 */
void
CellProjection::setClassName(const QString& name)
{
   className = name;
   if (cellProjectionFile != NULL) {
      classIndex = cellProjectionFile->addCellClass(name);
   }
   else {
      classIndex = -1; // 0;
   }
   setModified();
}

/**
 * Set the cell's name
 */
void
CellProjection::setName(const QString& name)
{
   if (cellProjectionFile != NULL) {
      uniqueNameIndex = cellProjectionFile->addCellUniqueName(name);
   }
   else {
      uniqueNameIndex = -1; // 0;
   }
   CellBase::setName(name);
   setModified();
}

/**
 * set the duplicate cell projection flag.
 */
void 
CellProjection::setDuplicateFlag(const bool b)
{
   duplicateFlag = b;
   setModified();
}
      
/**
 * get the volume position.
 */
void 
CellProjection::getVolumeXYZ(float xyzOut[3]) const
{
   xyzOut[0] = volumeXYZ[0];
   xyzOut[1] = volumeXYZ[1];
   xyzOut[2] = volumeXYZ[2];
}

/**
 * set the volume position.
 */
void 
CellProjection::setVolumeXYZ(const float xyzIn[3])
{
   volumeXYZ[0] = xyzIn[0];
   volumeXYZ[1] = xyzIn[1];
   volumeXYZ[2] = xyzIn[2];
   setModified();
}
      
/**
 * Read a version 1 cell projection.
 */
void
CellProjection::readFileDataVersion1(QTextStream& stream) throw (FileException)
{
   PROJECTION_TYPE tempProjectionType = PROJECTION_TYPE_UNKNOWN;
   projectionType = tempProjectionType;
   
   std::vector<QString> tokens;
   QString line;
   
   AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
   if (tokens.size() < 6) {
      QString s("Reading cell projection 1st line ");
      s.append(line);
      throw FileException(filename, s);
   }
   sectionNumber = tokens[1].toInt();
   name = tokens[2];
   const QString typeName(tokens[3]);
   studyNumber = tokens[4].toInt();
   structure.setTypeFromString(tokens[5]);
/*
   if (tokens[5] == tagLeftHemisphere) {
      hemisphere = ORIGINAL_HEMISPHERE_LEFT;
   }
   else if (tokens[5] == tagRightHemisphere) {
      hemisphere = ORIGINAL_HEMISPHERE_RIGHT;
   }
   else  {
      hemisphere = ORIGINAL_HEMISPHERE_UNKNOWN;
   }
*/
   
   signedDistanceAboveSurface = 0.0;
   if (tokens.size() >= 7) {
      signedDistanceAboveSurface = tokens[6].toFloat();
   }
   
   if (typeName == tagInsideTriangle) {
      tempProjectionType = PROJECTION_TYPE_INSIDE_TRIANGLE;
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 9) {
         QString s("Reading CellProjection INSIDE line ");
         s.append(line);
         throw FileException(filename, s);
      }
      closestTileVertices[0] = tokens[0].toInt();
      closestTileVertices[1] = tokens[1].toInt();
      closestTileVertices[2] = tokens[2].toInt();
      closestTileAreas[0] = tokens[3].toFloat();
      closestTileAreas[1] = tokens[4].toFloat();
      closestTileAreas[2] = tokens[5].toFloat(); 
      cdistance[0] = tokens[6].toFloat();
      cdistance[1] = tokens[7].toFloat();
      cdistance[2] = tokens[8].toFloat(); 
   }
   else if (typeName == tagOutsideTriangle) {
      tempProjectionType = PROJECTION_TYPE_OUTSIDE_TRIANGLE;
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 5) {
         QString s("reading CellProjection OUTSIDE 1st line ");
         s.append(line);
         throw FileException(filename, s); 
      }
      fracRI = tokens[0].toFloat();
      fracRJ = tokens[1].toFloat();
      dR     = tokens[2].toFloat();
      thetaR = tokens[3].toFloat();
      phiR   = tokens[4].toFloat();
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 18) {
         QString s("Reading CellProjection OUTSIDE line ");
         s.append(line);
         throw FileException(filename, s); 
      }
      
      int i = 0;
      int ctr = 0;
      for (i = 0; i < 2; i++) {
         for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
               triFiducial[i][j][k] = tokens[ctr].toFloat();
               ctr++;
            }
         }
      }
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 6) {
         QString s("reading CellProjection OUTSIDE 3rd line ");
         s.append(line); 
         throw FileException(filename, s);
      }
      ctr = 0;
      for (i = 0; i < 2; i++) {
         for (int j = 0; j < 3; j++) {
            triVertices[i][j] = tokens[ctr].toInt();
            ctr++;
         }
      }
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 6) {
         QString s("reading CellProjection OUTSIDE 4th line ");
         s.append(line);
         throw FileException(filename, line); 
      }
      ctr = 0;
      for (i = 0; i < 2; i++) {
         for (int j = 0; j < 3; j++) {
            vertexFiducial[i][j] = tokens[ctr].toFloat();
            ctr++;
         }
      }
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 5) {
         QString s("reading CellProjection OUTSIDE 5th line ");
         s.append(line);
         throw FileException(filename, s); 
      }
      vertex[0] = tokens[0].toInt();
      vertex[1] = tokens[1].toInt();
      posFiducial[0] = tokens[2].toFloat();
      posFiducial[1] = tokens[3].toFloat();
      posFiducial[2] = tokens[4].toFloat(); 
      volumeXYZ[0] = posFiducial[0];
      volumeXYZ[1] = posFiducial[1];
      volumeXYZ[2] = posFiducial[2];
   }
   else if (typeName == tagUnknownTriangle) {
      return;
   }
   else {
      QString s("reading CellProjection invalid projection type ");
      s.append(line);
      throw FileException(filename, line);
   }
   
   projectionType = tempProjectionType;
}

/**
 * Read a version 2 cell projection.
 */
void
CellProjection::readFileDataVersion2(QTextStream& stream) throw (FileException)
{
   PROJECTION_TYPE tempProjectionType = PROJECTION_TYPE_UNKNOWN;
   projectionType = tempProjectionType;
   
   std::vector<QString> tokens;
   QString line;
   
   AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
   if (tokens.size() < 6) {
      QString s("Reading cell projection 1st line ");
      s.append(line);
      throw FileException(filename, s);
   }
   sectionNumber = tokens[1].toInt();
   name = tokens[2];
   className = tokens[3];
   if (className == "???") {
      className = "";
   }
   const QString typeName(tokens[4]);
   studyNumber = tokens[5].toInt();
   structure.setTypeFromString(tokens[6]);
/*
   if (tokens[6] == tagLeftHemisphere) {
      hemisphere = ORIGINAL_HEMISPHERE_LEFT;
   }
   else if (tokens[6] == tagRightHemisphere) {
      hemisphere = ORIGINAL_HEMISPHERE_RIGHT;
   }
   else  {
      hemisphere = ORIGINAL_HEMISPHERE_UNKNOWN;
   }
*/
   signedDistanceAboveSurface = 0.0;
   if (tokens.size() >= 8) {
      signedDistanceAboveSurface = tokens[7].toFloat();
   }
   
   if (typeName == tagInsideTriangle) {
      tempProjectionType = PROJECTION_TYPE_INSIDE_TRIANGLE;
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 9) {
         QString s("Reading CellProjection INSIDE line ");
         s.append(line);
         throw FileException(filename, s);
      }
      closestTileVertices[0] = tokens[0].toInt();
      closestTileVertices[1] = tokens[1].toInt();
      closestTileVertices[2] = tokens[2].toInt();
      closestTileAreas[0] = tokens[3].toFloat();
      closestTileAreas[1] = tokens[4].toFloat();
      closestTileAreas[2] = tokens[5].toFloat(); 
      cdistance[0] = tokens[6].toFloat();
      cdistance[1] = tokens[7].toFloat();
      cdistance[2] = tokens[8].toFloat(); 
   }
   else if (typeName == tagOutsideTriangle) {
      tempProjectionType = PROJECTION_TYPE_OUTSIDE_TRIANGLE;
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 5) {
         QString s("reading CellProjection OUTSIDE 1st line ");
         s.append(line);
         throw FileException(filename, s); 
      }
      fracRI = tokens[0].toFloat();
      fracRJ = tokens[1].toFloat();
      dR     = tokens[2].toFloat();
      thetaR = tokens[3].toFloat();
      phiR   = tokens[4].toFloat();
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 18) {
         QString s("Reading CellProjection OUTSIDE line ");
         s.append(line);
         throw FileException(filename, s); 
      }
      
      int i = 0;
      int ctr = 0;
      for (i = 0; i < 2; i++) {
         for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
               triFiducial[i][j][k] = tokens[ctr].toFloat();
               ctr++;
            }
         }
      }
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 6) {
         QString s("reading CellProjection OUTSIDE 3rd line ");
         s.append(line); 
         throw FileException(filename, s);
      }
      ctr = 0;
      for (i = 0; i < 2; i++) {
         for (int j = 0; j < 3; j++) {
            triVertices[i][j] = tokens[ctr].toInt();
            ctr++;
         }
      }
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 6) {
         QString s("reading CellProjection OUTSIDE 4th line ");
         s.append(line);
         throw FileException(filename, line); 
      }
      ctr = 0;
      for (i = 0; i < 2; i++) {
         for (int j = 0; j < 3; j++) {
            vertexFiducial[i][j] = tokens[ctr].toFloat();
            ctr++;
         }
      }
      
      AbstractFile::readLineIntoTokens(filename, stream, line, tokens);
      if (tokens.size() < 5) {
         QString s("reading CellProjection OUTSIDE 5th line ");
         s.append(line);
         throw FileException(filename, s); 
      }
      vertex[0] = tokens[0].toInt();
      vertex[1] = tokens[1].toInt();
      posFiducial[0] = tokens[2].toFloat();
      posFiducial[1] = tokens[3].toFloat();
      posFiducial[2] = tokens[4].toFloat(); 
      volumeXYZ[0] = posFiducial[0];
      volumeXYZ[1] = posFiducial[1];
      volumeXYZ[2] = posFiducial[2];
   }
   else if (typeName == tagUnknownTriangle) {
      return;
   }
   else {
      QString s("reading CellProjection invalid projection type ");
      s.append(line);
      throw FileException(filename, line);
   }
   
   projectionType = tempProjectionType;
}

/**
 * Write a cell projection.
 */
void
CellProjection::writeFileData(QTextStream& stream, const int cellNumber) throw (FileException)
{
   char projStr[256];
   switch(projectionType) {
      case PROJECTION_TYPE_INSIDE_TRIANGLE:
         strcpy(projStr, tagInsideTriangle.toAscii().constData());
         break;
      case PROJECTION_TYPE_OUTSIDE_TRIANGLE:
         strcpy(projStr, tagOutsideTriangle.toAscii().constData());
         break;
      case PROJECTION_TYPE_UNKNOWN:
      default:
         strcpy(projStr, tagUnknownTriangle.toAscii().constData());
         break;
   }
   if (className.isEmpty()) {
      className = "???";
   }
   
   stream << cellNumber << " " 
          << sectionNumber << " " 
          << name << " " 
          << className << " "
          << projStr << " " 
          << studyNumber << " " 
          << StringUtilities::makeUpperCase(structure.getTypeAsString()) << " "
          << signedDistanceAboveSurface << "\n";
   
   switch(projectionType) {
      case PROJECTION_TYPE_INSIDE_TRIANGLE:
         stream << closestTileVertices[0] << " "
                << closestTileVertices[1] << " "
                << closestTileVertices[2] << " "
                << closestTileAreas[0] << " "
                << closestTileAreas[1] << " "
                << closestTileAreas[2] << " "
                << cdistance[0] << " "
                << cdistance[1] << " "
                << cdistance[2] << "\n"; 
         break;
      case PROJECTION_TYPE_OUTSIDE_TRIANGLE:
         {
            stream << fracRI << " "
                   << fracRJ << " "
                   << dR << " "
                   << thetaR << " "
                   << phiR << "\n";

            int i = 0;
            for (i = 0; i < 2; i++) {
               for (int j = 0; j < 3; j++) {
                  for (int k = 0; k < 3; k++) {
                     stream << triFiducial[i][j][k] << " ";
                  }
               }
            }
            stream << "\n";

            for (i = 0; i < 2; i++) {
               for (int j = 0; j < 3; j++) {
                  stream << triVertices[i][j] << " ";
               }
            }
            stream << "\n";

            for (i = 0; i < 2; i++) {
               for (int j = 0; j < 3; j++) {
                  stream << vertexFiducial[i][j] << " ";
               }
            }
            stream << "\n";

            stream << vertex[0] << " "
                   << vertex[1] << " "
                   << posFiducial[0] << " "
                   << posFiducial[1] << " "
                   << posFiducial[2] << "\n"; 
         }
         break;
      case PROJECTION_TYPE_UNKNOWN:
      default:
         break;
   }
}

/**
 * set element from text (used by SAX XML parser).
 */
void 
CellProjection::setElementFromText(const QString& elementName,
                                   const QString& textValue)
{
   const QStringList sl = textValue.split(QRegExp("\\s+"), QString::SkipEmptyParts);
   const int numItems = sl.count();
   
   if (elementName == CellProjection::tagCellProjNumber) {
      // ignore
   }
   else if (elementName == CellProjection::tagProjectionType) {
      if (textValue == tagInsideTriangle) {
         projectionType = PROJECTION_TYPE_INSIDE_TRIANGLE;
      }
      else if (textValue == tagOutsideTriangle) {
         projectionType = PROJECTION_TYPE_OUTSIDE_TRIANGLE;
      }
   }
   else if (elementName == CellProjection::tagClosestTileVertices) {
      if (numItems == 3) {
         for (int i = 0; i < 3; i++) {
            closestTileVertices[i] = sl.at(i).toInt();
         }
      }
   }
   else if (elementName == CellProjection::tagClosestTileAreas) {
      if (numItems == 3) {
         for (int i = 0; i < 3; i++) {
            closestTileAreas[i] = sl.at(i).toFloat();
         }
      }
   }
   else if (elementName == CellProjection::tagCDistance) {
      if (numItems == 3) {
         for (int i = 0; i < 3; i++) {
            cdistance[i] = sl.at(i).toFloat();
         }
      }
   }
   else if (elementName == tagDR) {
      dR = textValue.toFloat();
   }
   else if (elementName == CellProjection::tagTriFiducial) {
      if (numItems == 18) {
         float* data = (float*)triFiducial;
         for (int i = 0; i < 18; i++) {
            data[i] = sl.at(i).toFloat();
         }
      }
   }
   else if (elementName == CellProjection::tagThetaR) {
      thetaR = textValue.toFloat();
   }
   else if (elementName == CellProjection::tagPhiR) {
      phiR = textValue.toFloat();
   }
   else if (elementName == CellProjection::tagTriVertices) {
      if (numItems == 6) {
         int* data = (int*)triVertices;
         for (int i = 0; i < 6; i++) {
            data[i] = sl.at(i).toInt();
         }
      }
   }
   else if (elementName == CellProjection::tagVertex) {
      if (numItems == 2) {
         for (int i = 0; i < 2; i++) {
            vertex[i] = sl.at(i).toInt();
         }
      }
   }
   else if (elementName == CellProjection::tagVertexFiducial) {
      if (numItems == 6) {
         float* data = (float*)vertexFiducial;
         for (int i = 0; i < 6; i++) {
            data[i] = sl.at(i).toFloat();
         }
      }
   }
   else if (elementName == CellProjection::tagPosFiducial) {
      if (numItems == 3) {
         for (int i = 0; i < 3; i++) {
            posFiducial[i] = sl.at(i).toFloat();
         }
      }
   }
   else if (elementName == CellProjection::tagFracRI) {
      fracRI = textValue.toFloat();
   }
   else if (elementName == CellProjection::tagFracRJ) {
      fracRJ = textValue.toFloat();
   }
   else if (elementName == CellProjection::tagVolumeXYZ) {
      if (numItems == 3) {
         for (int i = 0; i < 3; i++) {
            volumeXYZ[i] = sl.at(i).toFloat();
         }
      }
   }
   else if (elementName == CellProjection::tagDuplicateFlag) {
      duplicateFlag = false;
      if (textValue == "true") {
         duplicateFlag = true;
      }
   }
   else {
      std::cout << "WARNING: Unrecognized child of CellProjection element "
                << elementName.toAscii().constData()
                << std::endl;
   }
}
                              
/**
 * called to read from an XML structure.
 */
void 
CellProjection::readXMLWithDOM(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != tagCellProjection) {
      QString msg("Incorrect element type passed to CellProjection::readXMLWithDOM() ");
      msg.append(elem.tagName());
      throw FileException("", msg);
   }
   
   projectionType = PROJECTION_TYPE_UNKNOWN;
   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == tagCellProjNumber) {
            // ignore
         }
         else if (elem.tagName() == tagProjectionType) {
            const QString value = AbstractFile::getXmlElementFirstChildAsString(elem);
            if (value == tagInsideTriangle) {
               projectionType = PROJECTION_TYPE_INSIDE_TRIANGLE;
            }
            else if (value == tagOutsideTriangle) {
               projectionType = PROJECTION_TYPE_OUTSIDE_TRIANGLE;
            }
         }
         else if (elem.tagName() == tagClosestTileVertices) {
            AbstractFile::getXmlElementFirstChildAsInt(elem, closestTileVertices, 3);
         }
         else if (elem.tagName() == tagClosestTileAreas) {
            AbstractFile::getXmlElementFirstChildAsFloat(elem, closestTileAreas, 3);
         }
         else if (elem.tagName() == tagCDistance) {
            AbstractFile::getXmlElementFirstChildAsFloat(elem, cdistance, 3);
         }
         else if (elem.tagName() == tagDR) {
            dR = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == tagTriFiducial) {
            AbstractFile::getXmlElementFirstChildAsFloat(elem, (float*)triFiducial, 18);
         }
         else if (elem.tagName() == tagThetaR) {
            thetaR = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == tagPhiR) {
            phiR = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == tagTriVertices) {
            AbstractFile::getXmlElementFirstChildAsInt(elem, (int*)triVertices, 6);
         }
         else if (elem.tagName() == tagVertex) {
            AbstractFile::getXmlElementFirstChildAsInt(elem, vertex, 2);
         }
         else if (elem.tagName() == tagVertexFiducial) {
            AbstractFile::getXmlElementFirstChildAsFloat(elem, (float*)vertexFiducial, 6);
         }
         else if (elem.tagName() == tagPosFiducial) {
            AbstractFile::getXmlElementFirstChildAsFloat(elem, posFiducial, 3);
         }
         else if (elem.tagName() == tagFracRI) {
            fracRI = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == tagFracRJ) {
            fracRJ = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == tagVolumeXYZ) {
            AbstractFile::getXmlElementFirstChildAsFloat(elem, volumeXYZ, 3);
         }
         else if (elem.tagName() == CellBase::tagSignedDistanceAboveSurface) {
            signedDistanceAboveSurface = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == CellBase::tagClassName) {
            className = AbstractFile::getXmlElementFirstChildAsString(elem);
            if (className == "???") {
               className = "";
            }
         }
         else if (elem.tagName() == CellProjection::tagDuplicateFlag) {
            duplicateFlag = false;
            if (AbstractFile::getXmlElementFirstChildAsString(elem) == "true") {
               duplicateFlag = true;
            }
         }
         else if (elem.tagName() == CellBase::tagCellBase) {
            CellBase::readXMLWithDOM(node);
         }
         else {
            std::cout << "WARNING: unrecognized CellProjection element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      node = node.nextSibling();
   }
}

/**
 * called to write to an XML structure.
 */
void 
CellProjection::writeXML(QDomDocument& xmlDoc,
                         QDomElement&  parentElement,
                         const int cellProjNumber)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement cellProjElement = xmlDoc.createElement(tagCellProjection);

   //
   // cell number
   //
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement, 
                                   tagCellProjNumber, cellProjNumber);
   
   switch(projectionType) {
      case PROJECTION_TYPE_INSIDE_TRIANGLE:
         AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                         tagProjectionType, tagInsideTriangle);
         break;
      case PROJECTION_TYPE_OUTSIDE_TRIANGLE:
         AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                         tagProjectionType, tagOutsideTriangle);
         break;
      case PROJECTION_TYPE_UNKNOWN:
      default:
         AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                         tagProjectionType, tagUnknownTriangle);
         break;
   }
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagClosestTileVertices, closestTileVertices, 3);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagClosestTileAreas, closestTileAreas, 3);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagCDistance, cdistance, 3);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagDR, dR);
                                
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagTriFiducial, (float*)(triFiducial), 18);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagThetaR, thetaR);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagPhiR, phiR);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagTriVertices, (int*)triVertices, 6);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagVertex, (int*)vertex, 2);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagVertexFiducial, (float*)vertexFiducial, 6);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagPosFiducial, posFiducial, 3);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagFracRI, fracRI);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagFracRJ, fracRJ);
                                   
   AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                   tagVolumeXYZ, volumeXYZ, 3);
                                   
   //AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
   //                                CellData::tagClassName, className);
                                   
   //AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
   //                                CellBase::tagSignedDistanceAboveSurface, signedDistanceAboveSurface);

   if (duplicateFlag) { 
      AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                      tagDuplicateFlag, "true");
   }
   else {
      AbstractFile::addXmlTextElement(xmlDoc, cellProjElement,
                                      tagDuplicateFlag, "false");
   }
   
   //
   // Write the base class' data
   //
   CellBase::writeXML(xmlDoc, cellProjElement);

   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(cellProjElement);
}
                    
//----------------------------------------------------------------------------
/**
 * Constructor.
 */
CellProjectionFile::CellProjectionFile(const QString& descriptiveName,
                                       const QString& defaultExtensionIn)
   : AbstractFile(descriptiveName, 
                  defaultExtensionIn,
                  true,
                  FILE_FORMAT_XML,
                  FILE_IO_READ_ONLY,
                  FILE_IO_NONE,
                  FILE_IO_READ_AND_WRITE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_NONE,
                  FILE_IO_READ_ONLY)
{
   readVersionNumberOnly = 0;
   clear();
   //if (DebugControl::getTestFlag2()) {
   //   setXmlVersionReadWithSaxParser(true);
   //}
}

/**
 * Destructor.
 */
CellProjectionFile::~CellProjectionFile()
{
   clear();
}

/**
 * append a fiducial cell file.
 */
void 
CellProjectionFile::appendFiducialCellFile(const CellFile& cf)
{
   const int origNumberOfStudyInfo = getNumberOfStudyInfo();
   const int num = cf.getNumberOfCells();

   for (int i = 0; i < num; i++) {
      CellProjection cp;
      const CellData* cd = cf.getCell(i);
      cp.copyData(*cd);
      cp.setVolumeXYZ(cd->getXYZ());
      
      //
      // Update study info indexes
      //
      int studyNum = cd->getStudyNumber();
      if (studyNum >= 0) {
         studyNum += origNumberOfStudyInfo;
      }
      cp.setStudyNumber(studyNum);
      addCellProjection(cp);
   }
   
   //
   // Transfer the study info
   //
   for (int j = 0; j < cf.getNumberOfStudyInfo(); j++) {
      addStudyInfo((*cf.getStudyInfo(j)));
   }
   
   //
   // transfer the file's comment
   //
   appendToFileComment(cf.getFileComment());
}
      
/**
 * get a cell file containing the origin (unprojected) coordinates.
 */
void 
CellProjectionFile::getCellFileOriginalCoordinates(CellFile& cellFileOut) const
{
   cellFileOut.clear();
   
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      const CellProjection* cp = getCellProjection(i);
      CellData cd;
      cd.copyData(*cp);
      cd.setXYZ(cp->getXYZ());
      cellFileOut.addCell(cd);
   }
   
   const int numStudyInfo = getNumberOfStudyInfo();
   for (int i = 0; i < numStudyInfo; i++) {
      cellFileOut.addStudyInfo(*getStudyInfo(i));
   }
   
   cellFileOut.setFileComment(getFileComment());
}
      
/**
 * get a cell file projecting to supplied coord/topo for left and right hemispheres.
 */
void 
CellProjectionFile::getCellFileForRightLeftFiducials(const CoordinateFile* leftCF,
                                                     const TopologyFile* leftTF,
                                                     const CoordinateFile* rightCF,
                                                     const TopologyFile* rightTF,
                                                     const CoordinateFile* cerebellumCF,
                                                     const TopologyFile* cerebellumTF,
                                                     CellFile& cellFileOut) const
{
   cellFileOut.clear();
   
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      const CellProjection* cp = getCellProjection(i);
      float xyz[3] = { 0.0, 0.0, 0.0 };
      switch (cp->getCellStructure()) {
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
         case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
            if (leftCF != NULL) {
               cp->getProjectedPosition(leftCF, leftTF, true, false, false, xyz);
            }
            break;
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
         case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
            if (rightCF != NULL) {
               cp->getProjectedPosition(rightCF, rightTF, true, false, false, xyz);
            }
            break;
         case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
         case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
            break;
         case Structure::STRUCTURE_TYPE_CEREBELLUM:
         case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
         case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
            if (cerebellumCF != NULL) {
               cp->getProjectedPosition(cerebellumCF, cerebellumTF, true, false, false, xyz);
            }
            break;
         case Structure::STRUCTURE_TYPE_SUBCORTICAL:
         case Structure::STRUCTURE_TYPE_ALL:
         case Structure::STRUCTURE_TYPE_INVALID:
            break;
      }
      CellData cd;
      cd.copyData(*cp);
      cd.setXYZ(xyz);
      cellFileOut.addCell(cd);
   }
   
   const int numStudyInfo = getNumberOfStudyInfo();
   for (int i = 0; i < numStudyInfo; i++) {
      cellFileOut.addStudyInfo(*getStudyInfo(i));
   }
   
   cellFileOut.setFileComment(getFileComment());
}
                       
/**
 * get first cell projection with specified name (const method)
 */
const CellProjection* 
CellProjectionFile::getFirstCellProjectionWithName(const QString& name) const
{
   const int numCellProj = getNumberOfCellProjections();
   for (int i = 0; i < numCellProj; i++) {
      const CellProjection* cp = getCellProjection(i);
      if (cp->getName() == name) {
         return cp;
      }
   }
   
   return NULL;
}

/**
 * get first cell projection with specified name.
 */
CellProjection* 
CellProjectionFile::getFirstCellProjectionWithName(const QString& name)
{
   const int numCellProj = getNumberOfCellProjections();
   for (int i = 0; i < numCellProj; i++) {
      CellProjection* cp = getCellProjection(i);
      if (cp->getName() == name) {
         return cp;
      }
   }
   
   return NULL;
}

/**
 * get last cell projection with specified name.
 */
CellProjection* 
CellProjectionFile::getLastCellProjectionWithName(const QString& name)
{
   const int numCellProj = getNumberOfCellProjections();
   for (int i = (numCellProj - 1); i >= 0; i--) {
      CellProjection* cp = getCellProjection(i);
      if (cp->getName() == name) {
         return cp;
      }
   }
   
   return NULL;
}
      
/**
 * get last cell projection with specified name (const method).
 */
const CellProjection* 
CellProjectionFile::getLastCellProjectionWithName(const QString& name) const
{
   const int numCellProj = getNumberOfCellProjections();
   for (int i = (numCellProj - 1); i >= 0; i--) {
      const CellProjection* cp = getCellProjection(i);
      if (cp->getName() == name) {
         return cp;
      }
   }
   
   return NULL;
}
/**
 * get a cell file projecting to supplied coord/topo.
 */
void 
CellProjectionFile::getCellFile(const CoordinateFile* cf,
                                const TopologyFile* tf,
                                const bool fiducialSurfaceFlag,
                                CellFile& cellFileOut) const
{
   cellFileOut.clear();
   
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      const CellProjection* cp = getCellProjection(i);
      float xyz[3];
      cp->getProjectedPosition(cf, tf, fiducialSurfaceFlag, false, false, xyz);
      CellData cd;
      cd.copyData(*cp);
      cd.setXYZ(xyz);
      cellFileOut.addCell(cd);
   }
   
   const int numStudyInfo = getNumberOfStudyInfo();
   for (int i = 0; i < numStudyInfo; i++) {
      cellFileOut.addStudyInfo(*getStudyInfo(i));
   }
   
   cellFileOut.setFileComment(getFileComment());
}

/**
 * Append a cell projection file to this cell projection file.
 */
void
CellProjectionFile::append(const CellProjectionFile& cpf)
{
   const int numCellProjections = cpf.getNumberOfCellProjections();
   const int origNumberOfStudyInfo = getNumberOfStudyInfo();
   
   //
   // Transfer the cell projections
   //
   for (int i = 0; i < numCellProjections; i++) {
      CellProjection cp = *(cpf.getCellProjection(i));
      //
      // Update study info indexes
      //
      if (cp.studyNumber >= 0) {
         cp.studyNumber += origNumberOfStudyInfo;
      }
      addCellProjection(cp);
   }
   
   //
   // Transfer the study info
   //
   for (int j = 0; j < cpf.getNumberOfStudyInfo(); j++) {
      addStudyInfo((*cpf.getStudyInfo(j)));
   }
   
   //
   // transfer the file's comment
   //
   appendFileComment(cpf);
}

/**
 * Apply a transformation matrix to the cells.
 */
/*
void 
CellProjectionFile::applyTransformationMatrix(const int sectionLow, 
                                    const int sectionHigh,
                                    const float matrix[16],
                                    const bool onlySpecialCells)
{
   const int num = getNumberOfCellProjections();
   
   for (int i = 0; i < num; i++) {
      if ((cellProjections[i].sectionNumber >= sectionLow) &&
          (cellProjections[i].sectionNumber <= sectionHigh)) {
         bool transformIt = true;
         if (onlySpecialCells && (cellProjections[i].specialFlag == false)) {
            transformIt = false;
         }

         if (transformIt) {         
            const float x = cellProjections[i].xyz[0];
            const float y = cellProjections[i].xyz[1];
            const float z = cellProjections[i].xyz[2];

            cellProjections[i].xyz[0] = x*matrix[0]
                       + y*matrix[4]
                       + z*matrix[8]
                       + matrix[12];
            cellProjections[i].xyz[1] = x*matrix[1]
                       + y*matrix[5]
                       + z*matrix[9]
                       + matrix[13];
            cellProjections[i].xyz[2] = x*matrix[2]
                       + y*matrix[6]
                       + z*matrix[10]
                       + matrix[14];
         }
      }
   }
   setModified();
}
*/
/**
 * apply a transformation matrix to the cells.
 */
/*
void 
CellProjectionFile::applyTransformationMatrix(const TransformationMatrix& matrix,
                                    const bool onlySpecialCells)
{
   applyTransformationMatrix(std::numeric_limits<int>::min(),
                             std::numeric_limits<int>::max(),
                             matrix,
                             onlySpecialCells);
}
*/
/**
 * Apply a transformation matrix to the cells.
 */
void 
CellProjectionFile::applyTransformationMatrix(const CoordinateFile* cf,
                                              const TopologyFile* tf,
                                              const bool fiducialSurfaceFlag,
                                              const int sectionLow, 
                                              const int sectionHigh,
                                              const TransformationMatrix& matrixIn,
                                              const bool onlySpecialCells)
{
   TransformationMatrix& matrix = const_cast<TransformationMatrix&>(matrixIn);
   const int num = getNumberOfCellProjections();
   
   for (int i = 0; i < num; i++) {
      CellProjection* cp = getCellProjection(i);
      if ((cp->sectionNumber >= sectionLow) &&
          (cp->sectionNumber <= sectionHigh)) {
         bool transformIt = true;
         if (onlySpecialCells && (cp->specialFlag == false)) {
            transformIt = false;
         }

         if (transformIt) { 
            float xyz[3] = { 0.0, 0.0, 0.0 };
            if (cp->getProjectedPosition(cf, tf, fiducialSurfaceFlag, false, false, xyz)) {
               matrix.multiplyPoint(xyz);
            }
            cp->setXYZ(xyz);
            cp->projectionType = CellProjection::PROJECTION_TYPE_UNKNOWN;
         }
      }
   }
   setModified();
}


/**
 * Add a cell projection to the file.
 */
void 
CellProjectionFile::addCellProjection(const CellProjection& cp)
{
   cellProjections.push_back(cp);
   const int indx = getNumberOfCellProjections() - 1;
   cellProjections[indx].cellProjectionFile = this;
   cellProjections[indx].classIndex = addCellClass(cp.className);
   cellProjections[indx].uniqueNameIndex = addCellUniqueName(cp.name);
   setModified();
}

/**
 * Add a cell class (if class already exists its selection status
 * is set to true).
 */
int
CellProjectionFile::addCellClass(const QString& className) 
{
   if (className.isEmpty()) {
      return -1;
   }
   
   const int index = getCellClassIndexByName(className);
   if (index >= 0) {
      cellClasses[index].selected = true;
      return index;
   }
   
   cellClasses.push_back(CellClass(className));
   return (getNumberOfCellClasses() - 1);
}

/**
 * Clear all of the cell's special flags.
 */
void
CellProjectionFile::clearAllSpecialFlags()
{
   const int numCells = getNumberOfCellProjections();
   for (int i = 0; i < numCells; i++) {
      cellProjections[i].specialFlag = false;
   }
}

/**
 * clear all highlight flags
 */
void 
CellProjectionFile::clearAllHighlightFlags()
{
   const int numCells = getNumberOfCellProjections();
   for (int i = 0; i < numCells; i++) {
      cellProjections[i].highlightFlag = false;
   }
}
      
/**
 * get cell class indices sorted by name case insensitive.
 */
void 
CellProjectionFile::getCellClassIndicesSortedByName(std::vector<int>& indicesSortedByNameOut,
                                          const bool reverseOrderFlag,
                                          const bool limitToDisplayedCellsFlag) const
{
   NameIndexSort nis;
   
   if (limitToDisplayedCellsFlag) {
      std::vector<int> displayedCellIndices;
      getIndicesOfDisplayedCells(displayedCellIndices);
      
      std::set<QString> classNames;
      const int numDisplayedCells = static_cast<int>(displayedCellIndices.size());
      for (int i = 0; i < numDisplayedCells; i++) {
         const int indx = displayedCellIndices[i];
         classNames.insert(getCellProjection(indx)->getClassName());
      }
      
      for (std::set<QString>::const_iterator iter = classNames.begin();
           iter != classNames.end();
           iter++) {
         const QString s(*iter);
         const int indx = getCellClassIndexByName(s);
         if (indx >= 0) {
            nis.add(indx, s);
         }
      }
   }
   else {
      const int num = getNumberOfCellClasses();
      for (int i = 0; i < num; i++) {
         nis.add(i, cellClasses[i].name);
      }
   }
   
   nis.sortByNameCaseInsensitive();
   const int num = nis.getNumberOfItems();
   
   indicesSortedByNameOut.resize(num, 0);
   for (int i = 0; i < num; i++) {
      indicesSortedByNameOut[i] = nis.getSortedIndex(i);
   }
   
   if (reverseOrderFlag) {
      std::reverse(indicesSortedByNameOut.begin(), indicesSortedByNameOut.end());
   }
}
      
/**
 * get cell class selected by index.
 */
bool 
CellProjectionFile::getCellClassSelectedByIndex(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfCellClasses())) {
      return cellClasses[indx].selected;
   }
   
   //
   // If no class, just enable it for display
   //
   return true;
}

/**
 * Set cell class selected by index.
 */
void
CellProjectionFile::setCellClassSelectedByIndex(const int index, const bool sel)
{
   if (index < getNumberOfCellClasses()) {
      cellClasses[index].selected = sel;
   }
}

/**
 * Get a class index from its name
 */
int
CellProjectionFile::getCellClassIndexByName(const QString& name) const
{
   const int numClasses = getNumberOfCellClasses();
   for (int i = 0; i < numClasses; i++) {
      if (name == cellClasses[i].name) {
         return i;
      }
   }
   return -1;
}

/**
 * get cell class selected by name.
 */
bool 
CellProjectionFile::getCellClassSelectedByName(const QString& name) const
{
   if (name.isEmpty()) {
      return false;
   }
   
   const int index = getCellClassIndexByName(name);
   if (index >= 0) {
      return cellClasses[index].selected;
   }
   return false;
}

/**
 * get cell class name.
 */
QString 
CellProjectionFile::getCellClassNameByIndex(const int index) const
{
   if (index < getNumberOfCellClasses()) {
      return cellClasses[index].name;
   }
   return "";  //"???";
}

/**
 * set the status of all cell classes.
 */
void 
CellProjectionFile::setAllCellClassStatus(const bool selected)
{
   const int numClasses = getNumberOfCellClasses();
   for (int i = 0; i < numClasses; i++) {
      cellClasses[i].selected = selected;
   }
}
      
/**
 * set the search status of all cells.
 */
void 
CellProjectionFile::setAllSearchStatus(const bool inSearchFlag)
{
   const int numCells = getNumberOfCellProjections();
   for (int i = 0; i < numCells; i++) {
      cellProjections[i].inSearchFlag = inSearchFlag;
   }
}
      
/**
 * update cell class if linked to table subheader.
 */
void 
CellProjectionFile::updateCellClassWithLinkedStudyTableSubheaderShortNames(const StudyMetaDataFile* smdf)
{
   const int numStudyMetaData = smdf->getNumberOfStudyMetaData();
   for (int i = 0; i < numStudyMetaData; i++) {
      const StudyMetaData* smd = smdf->getStudyMetaData(i);
      const QString pmid = smd->getPubMedID();
      if (pmid.isEmpty() == false) {
         const int numTables = smd->getNumberOfTables();
         for (int j = 0; j < numTables; j++) {
            const StudyMetaData::Table* table = smd->getTable(j);
            const QString tableNumber = table->getNumber();
            if (tableNumber.isEmpty() == false) {
               const int numSubHeaders = table->getNumberOfSubHeaders();
               for (int k = 0; k < numSubHeaders; k++) {
                  const StudyMetaData::SubHeader* sh = table->getSubHeader(k);
                  const QString subHeaderNumber = sh->getNumber();
                  if (subHeaderNumber.isEmpty() == false) {
                     const QString shortName = sh->getShortName();
                     if (shortName.isEmpty() == false) {
                        StudyMetaDataLink smdl;
                        smdl.setPubMedID(pmid);
                        smdl.setTableSubHeaderNumber(subHeaderNumber);
                        smdl.setTableNumber(tableNumber);
                        transferTableSubHeaderShortNameToCellClass(smdl, shortName);
                     }
                  }
               }
            }
         }
      }
   }
}
      
/**
 * update cell class with linked tabel subheader name, linked figure panel task
 * description, or page reference subheader short name.
 */
void 
CellProjectionFile::updateCellClassWithLinkedTableFigureOrPageReference(const StudyMetaDataFile* smdf)
{
   const int numCells = getNumberOfCellProjections();
   for (int i = 0; i < numCells; i++) {
      CellProjection* cp = getCellProjection(i);
      const StudyMetaDataLinkSet cellSMDLS = cp->getStudyMetaDataLinkSet();
      const int numSMDL = cellSMDLS.getNumberOfStudyMetaDataLinks();
      for (int j = 0; j < numSMDL; j++) {
         const StudyMetaDataLink cellSMDL = cellSMDLS.getStudyMetaDataLink(j);
         const int studyIndex = smdf->getStudyIndexFromLink(cellSMDL);
         if (studyIndex >= 0) {
            const StudyMetaData* smd = smdf->getStudyMetaData(studyIndex);
            const QString figNum = cellSMDL.getFigureNumber();
            const QString pageRefNum = cellSMDL.getPageReferencePageNumber();
            const QString tableNum = cellSMDL.getTableNumber();
            
            
            if (figNum.isEmpty() == false) {
               const StudyMetaData::Figure* figure = smd->getFigureByFigureNumber(figNum);
               if (figure != NULL) {
                  const StudyMetaData::Figure::Panel* panel = 
                     figure->getPanelByPanelNumberOrLetter(cellSMDL.getFigurePanelNumberOrLetter());
                  if (panel != NULL) {
                     const QString txt = panel->getTaskDescription();
                     if (txt.isEmpty() == false) {
                        cp->setClassName(txt);
                     }
                  }
               }
            }
            if (pageRefNum.isEmpty() == false) {
               const StudyMetaData::PageReference* pageRef = smd->getPageReferenceByPageNumber(pageRefNum);
               if (pageRef != NULL) {
                  const StudyMetaData::SubHeader* subHeader = 
                     pageRef->getSubHeaderBySubHeaderNumber(cellSMDL.getPageReferenceSubHeaderNumber());
                  if (subHeader != NULL) {
                     const QString txt = subHeader->getShortName();
                     if (txt.isEmpty() == false) {
                        cp->setClassName(txt);
                     }
                  }
               }
            }
            if (tableNum.isEmpty() == false) {
               const StudyMetaData::Table* table = smd->getTableByTableNumber(tableNum);
               if (table != NULL) {
                  const StudyMetaData::SubHeader* subHeader = 
                     table->getSubHeaderBySubHeaderNumber(cellSMDL.getTableSubHeaderNumber());
                  if (subHeader != NULL) {
                     const QString txt = subHeader->getShortName();
                     if (txt.isEmpty() == false) {
                        cp->setClassName(txt);
                     }
                  }
               }
            }
         }
      }
   }
}
      
/**
 * transfer table subheader short name to cell classes.
 */
void 
CellProjectionFile::transferTableSubHeaderShortNameToCellClass(const StudyMetaDataLink& smdl,
                                                               const QString& shortName)
{
   const int numCells = getNumberOfCellProjections();
   for (int i = 0; i < numCells; i++) {
      CellProjection* cp = getCellProjection(i);
      const StudyMetaDataLinkSet cellSMDLS = cp->getStudyMetaDataLinkSet();
      const int numSMDL = cellSMDLS.getNumberOfStudyMetaDataLinks();
      for (int j = 0; j < numSMDL; j++) {
         const StudyMetaDataLink cellSMDL = cellSMDLS.getStudyMetaDataLink(j);
         if ((smdl.getPubMedID() == cellSMDL.getPubMedID()) &&
             (smdl.getTableNumber() == cellSMDL.getTableNumber()) &&
             (smdl.getTableSubHeaderNumber() == cellSMDL.getTableSubHeaderNumber())) {
            cp->setClassName(shortName);
         }
      }
   }
}
      
/**
 * Add a cell unique name.
 */
int 
CellProjectionFile::addCellUniqueName(const QString& uniqueName)
{
   if (uniqueName.isEmpty()) {
      return -1;
   }

   const int index = getCellUniqueNameIndexByName(uniqueName);
   if (index >= 0) {
      cellUniqueNames[index].selected = true;
      return index;
   }
   
   cellUniqueNames.push_back(CellClass(uniqueName));
   return (getNumberOfCellUniqueNames() - 1);
}
      
/**
 * get indices of displayed cell projections.
 */
void 
CellProjectionFile::getIndicesOfDisplayedCells(std::vector<int>& indicesOut) const
{
   indicesOut.clear();
   
   const int numCells = getNumberOfCellProjections();
   for (int i = 0; i < numCells; i++) {
      if (getCellProjection(i)->getDisplayFlag()) {
         indicesOut.push_back(i);
      }
   }
}
      
/**
 * get cell unique name indices sorted by name case insensitive.
 */
void 
CellProjectionFile::getCellUniqueNameIndicesSortedByName(std::vector<int>& indicesSortedByNameOut,
                                                         const bool reverseOrderFlag,
                                                         const bool limitToDisplayedCellsFlag) const
{
   NameIndexSort nis;
   
   if (limitToDisplayedCellsFlag) {
      std::vector<int> displayedCellIndices;
      getIndicesOfDisplayedCells(displayedCellIndices);
      
      std::set<QString> names;
      const int numDisplayedCells = static_cast<int>(displayedCellIndices.size());
      for (int i = 0; i < numDisplayedCells; i++) {
         const int indx = displayedCellIndices[i];
         names.insert(getCellProjection(indx)->getName());
      }
      
      for (std::set<QString>::const_iterator iter = names.begin();
           iter != names.end();
           iter++) {
         const QString s(*iter);
         const int indx = getCellUniqueNameIndexByName(s);
         if (indx >= 0) {
            nis.add(indx, s);
         }
      }
   }
   else {
      const int numUN = getNumberOfCellUniqueNames();
      for (int i = 0; i < numUN; i++) {
         nis.add(i, getCellUniqueNameByIndex(i));
      }
   }
   
   nis.sortByNameCaseInsensitive();
   const int numUniqueNames = nis.getNumberOfItems();
   indicesSortedByNameOut.resize(numUniqueNames, 0);
   for (int i = 0; i < numUniqueNames; i++) {
      indicesSortedByNameOut[i] = nis.getSortedIndex(i);
   }
   
   if (reverseOrderFlag) {
      std::reverse(indicesSortedByNameOut.begin(), indicesSortedByNameOut.end());
   }
}
      
/**
 * get cell unique name index by its name.
 */
int 
CellProjectionFile::getCellUniqueNameIndexByName(const QString& name) const
{
   const int numNames = getNumberOfCellUniqueNames();
   for (int i = 0; i < numNames; i++) {
      if (name == cellUniqueNames[i].name) {
         return i;
      }
   }
   return -1;
}

/**
 * get cell unique name selected by index.
 */
bool 
CellProjectionFile::getCellUniqueNameSelectedByIndex(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfCellUniqueNames())) {
      return cellUniqueNames[indx].selected;
   }
   
   //
   // If no unique name, just enable it for display
   //
   return true;
}

/**
 * set cell unique name selected by index.
 */
void 
CellProjectionFile::setCellUniqueNameSelectedByIndex(const int index, const bool sel)
{
   if (index < getNumberOfCellUniqueNames()) {
      cellUniqueNames[index].selected = sel;
   }
}

/**
 * get cell unique name selected by name.
 */
bool 
CellProjectionFile::getCellUniqueNameSelectedByName(const QString& name) const
{
   if (name.isEmpty()) {
      return false;
   }
   
   const int index = getCellUniqueNameIndexByName(name);
   if (index >= 0) {
      return cellUniqueNames[index].selected;
   }
   return false;
}

/**
 * get cell unique name name.
 */
QString 
CellProjectionFile::getCellUniqueNameByIndex(const int index) const
{
   if (index < getNumberOfCellUniqueNames()) {
      return cellUniqueNames[index].name;
   }
   return "";  //"???";
}

/**
 * set the status of all cell unique names.
 */
void 
CellProjectionFile::setAllCellUniqueNameStatus(const bool selected)
{
   const int numUniqueNames = getNumberOfCellUniqueNames();
   for (int i = 0; i < numUniqueNames; i++) {
      cellUniqueNames[i].selected = selected;
   }
}
      
/**
 * get all cell areas.
 */
void 
CellProjectionFile::getAllCellAreas(std::vector<QString>& allAreasOut) const
{
   allAreasOut.clear();
   
   std::set<QString> areaSet;
   
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      const CellProjection* cp = getCellProjection(i);
      const QString s = cp->getArea();
      if (s.isEmpty() == false) {
         const QStringList sl = s.split(';', QString::SkipEmptyParts);
         for (int k = 0; k < sl.size(); k++) {
            const QString area = sl.at(k).trimmed();
            if (area.isEmpty() == false) {
               areaSet.insert(area);
            }
         }
      }
   }
   
   allAreasOut.insert(allAreasOut.end(),
                      areaSet.begin(), areaSet.end());
}

/**
 * get all cell geography.
 */
void 
CellProjectionFile::getAllCellGeography(std::vector<QString>& allGeographyOut) const
{
   allGeographyOut.clear();
   
   std::set<QString> geographySet;
   
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      const CellProjection* cp = getCellProjection(i);
      const QString s = cp->getGeography();
      if (s.isEmpty() == false) {
         const QStringList sl = s.split(';', QString::SkipEmptyParts);
         for (int k = 0; k < sl.size(); k++) {
            const QString geography = sl.at(k).trimmed();
            if (geography.isEmpty() == false) {
               geographySet.insert(geography);
            }
         }
      }
   }
   
   allGeographyOut.insert(allGeographyOut.end(),
                          geographySet.begin(), geographySet.end());
}
      
/**
 * get all comments.
 */
void 
CellProjectionFile::getAllCellComments(std::vector<QString>& allCommentsOut) const
{
   allCommentsOut.clear();
   
   std::set<QString> commentSet;
   
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      const CellProjection* cp = getCellProjection(i);
      const QString s = cp->getComment();
      if (s.isEmpty() == false) {
         const QStringList sl = s.split(';', QString::SkipEmptyParts);
         for (int k = 0; k < sl.size(); k++) {
            const QString comment = sl.at(k).trimmed();
            if (comment.isEmpty() == false) {
               commentSet.insert(comment);
            }
         }
      }
   }
   
   allCommentsOut.insert(allCommentsOut.end(),
                          commentSet.begin(), commentSet.end());
}

/**
 * get all cell regions of interest.
 */
void 
CellProjectionFile::getAllCellRegionsOfInterest(std::vector<QString>& allRegionsOfInterestOut) const
{
   allRegionsOfInterestOut.clear();
   
   std::set<QString> regionsOfInterestSet;
   
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      const CellProjection* cp = getCellProjection(i);
      const QString s = cp->getRegionOfInterest();
      if (s.isEmpty() == false) {
         const QStringList sl = s.split(';', QString::SkipEmptyParts);
         for (int k = 0; k < sl.size(); k++) {
            const QString regionsOfInterest = sl.at(k).trimmed();
            if (regionsOfInterest.isEmpty() == false) {
               regionsOfInterestSet.insert(regionsOfInterest);
            }
         }
      }
   }
   
   allRegionsOfInterestOut.insert(allRegionsOfInterestOut.end(),
                          regionsOfInterestSet.begin(), regionsOfInterestSet.end());
}

/**
 * set the special flag for all cells in the section range and box
 */
void 
CellProjectionFile::setSpecialFlags(const int sectionLow,
                             const int sectionHigh,
                             const float bounds[4])
{
   const int num = getNumberOfCellProjections();
   
   const float minX = std::min(bounds[0], bounds[2]);
   const float maxX = std::max(bounds[0], bounds[2]);
   const float minY = std::min(bounds[1], bounds[3]);
   const float maxY = std::max(bounds[1], bounds[3]);

   for (int i = 0; i < num; i++) {
      if ((cellProjections[i].sectionNumber >= sectionLow) &&
          (cellProjections[i].sectionNumber <= sectionHigh)) {
         const float x = cellProjections[i].xyz[0];
         const float y = cellProjections[i].xyz[1];
         
         if ((x >= minX) && (x <= maxX) && (y >= minY) && (y <= maxY)) {
            cellProjections[i].specialFlag = true;
         }
      }
   }            
}

/**
 * Assign colors to these cells.
 */
void 
CellProjectionFile::assignColors(ColorFile& colorFile, 
                                 const CellBase::CELL_COLOR_MODE colorMode)
{
   const int numCellProjections = getNumberOfCellProjections();
   for (int i = 0; i < numCellProjections; i++) {
      CellProjection* cp = getCellProjection(i);
      bool match;
      switch (colorMode) {
         case CellBase::CELL_COLOR_MODE_NAME:
            cp->setColorIndex(colorFile.getColorIndexByName(cp->getName(), match));
            break;
         case CellBase::CELL_COLOR_MODE_CLASS:
            cp->setColorIndex(colorFile.getColorIndexByName(cp->getClassName(), match));
            break;
      }
   }
}

/**
 * Clear the cell projection file.
 */
void
CellProjectionFile::clear()
{
   clearAbstractFile();
   cellProjections.clear();
   studyInfo.clear();
   cellClasses.clear();
   cellUniqueNames.clear();
   versionNumber = -1;
}

/**
 * delete cell projections with name.
 */
void 
CellProjectionFile::deleteCellProjectionsWithName(const QString& name)
{
   const int num = getNumberOfCellProjections();
   for (int i = (num - 1); i >= 0; i--) {
      CellProjection* cp = getCellProjection(i);
      if (cp->getName() == name) {
         deleteCellProjection(i);
      }
   }
}
      
/**
 * delete all duplicate cell projections.
 */
void 
CellProjectionFile::deleteAllDuplicateCellProjections()
{
   const int num = getNumberOfCellProjections();
   for (int i = (num - 1); i >= 0; i--) {
      CellProjection* cp = getCellProjection(i);
      if (cp->getDuplicateFlag()) {
         deleteCellProjection(i);
      }
   }
}
      
/**
 * Delete the cell projection at the specified index
 */
void
CellProjectionFile::deleteCellProjection(const int index)
{
   if (index < getNumberOfCellProjections()) {
      cellProjections.erase(cellProjections.begin() + index);
      setModified();
   }
}

/**
 * assign class to all displayed foci.
 */
void 
CellProjectionFile::assignClassToDisplayedFoci(const QString& className)
{
   //
   // Loop through the cell projections and add them to "this" file
   //
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      //
      // Get the cell projection
      //
      CellProjection* cp = getCellProjection(i);
      
      //
      // Is this cell projection displayed
      //
      if (cp->getDisplayFlag()) {
         //
         // Set its class
         //
         cp->setClassName(className);
      }
   }
}

/**
 * delete cell projections whose indices are not specified.
 */
void 
CellProjectionFile::deleteAllButTheseCellProjections(const int* indices,
                                                     const int numIndices)
{
   if ((getNumberOfCellProjections() <= 0) ||
       (numIndices <= 0)) {
      return;
   }
   
   //
   // Place indices of cells that are to be kept into a sorted vector
   //
   std::vector<int> cellIndicesToKeep;
   for (int i = 0; i < numIndices; i++) {
      cellIndicesToKeep.push_back(indices[i]);
   }
   std::sort(cellIndicesToKeep.begin(), cellIndicesToKeep.end());
   
   //
   // Copy this cell projection file
   //
   CellProjectionFile cpf = *this;
   
   //
   // Clear out "this" file's data
   //
   cellProjections.clear();
   studyInfo.clear();
   cellClasses.clear();
   cellUniqueNames.clear();
   
   //
   // Keep track of which study info are needed
   //
   const int numStudyInfo = cpf.getNumberOfStudyInfo();
   std::vector<int> studyInfoTransferIndex;
   if (numStudyInfo > 0) {
      studyInfoTransferIndex.resize(numStudyInfo, -1);
   }
   
   //
   // Loop through the cell projections and add them to "this" file
   //
   for (int i = 0; i < static_cast<int>(cellIndicesToKeep.size()); i++) {
      //
      // Copy the cell projection
      //
      CellProjection cp = *(cpf.getCellProjection(cellIndicesToKeep[i]));
      
      //
      // See if the study info is used
      //
      const int studyInfoIndex = cp.getStudyNumber();
      if ((studyInfoIndex >= 0) && (studyInfoIndex < numStudyInfo)) {
         //
         // study info has not been transferred
         //
         if (studyInfoTransferIndex[studyInfoIndex] < 0) {
            //
            // add the study info to "this" file and keep track of its new index
            // 
            studyInfoTransferIndex[studyInfoIndex] = addStudyInfo(*(cpf.getStudyInfo(studyInfoIndex)));
         }
         
         //
         // Set the index for the study info
         //
         cp.setStudyNumber(studyInfoTransferIndex[studyInfoIndex]);
      }
      
      //
      // Add the cell projection to "this" file
      //
      addCellProjection(cp);
   }
}

/**
 * delete all cell projections whose display flag is false.
 */
void 
CellProjectionFile::deleteAllNonDisplayedCellProjections(const Structure& keepThisStructureOnly)
{
   std::vector<int> indicesToKeep;
   
   //
   // find cell projections that are to be kept
   //
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      //
      // Copy the cell projection
      //
      const CellProjection* cp = getCellProjection(i);
      
      //
      // Is this cell projection displayed
      //
      if (cp->getDisplayFlag()) {
         
         bool isLeftCellFlag = false;
         bool isRightCellFlag = false;
         bool isCerebellumCellFlag = false;
         switch (cp->getCellStructure()) {
            case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
               isLeftCellFlag = true;
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
               isRightCellFlag = true;
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
               isCerebellumCellFlag = true;
               break;
            case Structure::STRUCTURE_TYPE_CEREBELLUM:
               isCerebellumCellFlag = true;
               break;
            case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
               isCerebellumCellFlag = true;
               break;
            case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
               isCerebellumCellFlag = true;
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
               isLeftCellFlag = true;
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
               isRightCellFlag = true;
               break;
            case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
               break;
            case Structure::STRUCTURE_TYPE_SUBCORTICAL:
               break;
            case Structure::STRUCTURE_TYPE_ALL:
               break;
            case Structure::STRUCTURE_TYPE_INVALID:
               break;
         }
         
         //
         // Limit to a specific structure
         //
         switch (keepThisStructureOnly.getType()) {
            case Structure::STRUCTURE_TYPE_CORTEX_LEFT:
               if (isLeftCellFlag == false) {
                  continue;
               }
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_RIGHT:
               if (isRightCellFlag == false) {
                  continue;
               }
               break;
            case Structure::STRUCTURE_TYPE_CORTEX_BOTH:
            case Structure::STRUCTURE_TYPE_CEREBELLUM:
               if (isCerebellumCellFlag == false) {
                  continue;
               }
               break;
            case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_LEFT:
            case Structure::STRUCTURE_TYPE_CEREBELLUM_OR_CORTEX_RIGHT:
            case Structure::STRUCTURE_TYPE_CORTEX_LEFT_OR_CEREBELLUM:
            case Structure::STRUCTURE_TYPE_CORTEX_RIGHT_OR_CEREBELLUM:
            case Structure::STRUCTURE_TYPE_CEREBRUM_CEREBELLUM:
            case Structure::STRUCTURE_TYPE_SUBCORTICAL:
            case Structure::STRUCTURE_TYPE_ALL:
            case Structure::STRUCTURE_TYPE_INVALID:
               break;
         }

/*         
         isdff (keepThisStructureOnly.isLeftCortex() ||
             keepThisStructureOnly.isRightCortex()) {
            if (cp->getCellStructure() != keepThisStructureOnly.getType()) {
               continue;
            }
         }
*/         
         //
         // Keep this one
         //
         indicesToKeep.push_back(i);
      }
   }
   
   const int numToKeep = static_cast<int>(indicesToKeep.size());
   if (numToKeep > 0) {
      deleteAllButTheseCellProjections(&indicesToKeep[0], 
                                       numToKeep);
   }
/*
   if (getNumberOfCellProjections() <= 0) {
      return;
   }
   
   //
   // Copy this cell projection file
   //
   CellProjectionFile cpf = *this;
   
   //
   // Clear out "this" file's data
   //
   cellProjections.clear();
   studyInfo.clear();
   cellClasses.clear();
   cellUniqueNames.clear();
   
   //
   // Keep track of which study info are needed
   //
   const int numStudyInfo = cpf.getNumberOfStudyInfo();
   std::vector<int> studyInfoTransferIndex;
   if (numStudyInfo > 0) {
      studyInfoTransferIndex.resize(numStudyInfo, -1);
   }
   
   //
   // Loop through the cell projections and add them to "this" file
   //
   const int num = cpf.getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      //
      // Copy the cell projection
      //
      CellProjection cp = *(cpf.getCellProjection(i));
      
      //
      // Is this cell projection displayed
      //
      if (cp.getDisplayFlag()) {
         
         //
         // Limit to a specific structure
         //
         if (keepThisStructureOnly.isLeftCortex() ||
             keepThisStructureOnly.isRightCortex()) {
            if (cp.getCellStructure() != keepThisStructureOnly.getType()) {
               continue;
            }
         }
         
         //
         // See if the study info is used
         //
         const int studyInfoIndex = cp.getStudyNumber();
         if ((studyInfoIndex >= 0) && (studyInfoIndex < numStudyInfo)) {
            //
            // study info has not been transferred
            //
            if (studyInfoTransferIndex[studyInfoIndex] < 0) {
               //
               // add the study info to "this" file and keep track of its new index
               // 
               studyInfoTransferIndex[studyInfoIndex] = addStudyInfo(*(cpf.getStudyInfo(studyInfoIndex)));
            }
            
            //
            // Set the index for the study info
            //
            cp.setStudyNumber(studyInfoTransferIndex[studyInfoIndex]);
         }
         
         //
         // Add the cell projection to "this" file
         //
         addCellProjection(cp);
      }
   }
*/
}
      
/**
 * Get the study info index based upon the study info's value.
 */
int 
CellProjectionFile::getStudyInfoFromValue(const CellStudyInfo& csi) const
{
   const int num = getNumberOfStudyInfo();
   for (int i = 0; i < num; i++) {
      if ((*getStudyInfo(i)) == csi) {
         return i;
      }
   }
   return -1;
}

/**
 * Get a study info (const method).
 */
const CellStudyInfo*
CellProjectionFile::getStudyInfo(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      return &studyInfo[indx];
   }
   return NULL;
}

/**
 * Get a study info.
 */
CellStudyInfo*
CellProjectionFile::getStudyInfo(const int indx) 
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      return &studyInfo[indx];
   }
   return NULL;
}

/**
 * Add a study info.
 */
int
CellProjectionFile::addStudyInfo(const CellStudyInfo& csi)
{
   studyInfo.push_back(csi);
   const int index = studyInfo.size() - 1;
   return index;
}

/**
 * delete all study info and clear links to study info
 */
void 
CellProjectionFile::deleteAllStudyInfo()
{
   const int num = getNumberOfCellProjections();
   for (int i = 0; i < num; i++) {
      CellProjection* cp = getCellProjection(i);
      cp->setStudyNumber(-1);
   }
   studyInfo.clear();
}
      
/**
 * delete study info.
 */
void 
CellProjectionFile::deleteStudyInfo(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfStudyInfo())) {
      for (int i = 0; i < getNumberOfCellProjections(); i++) {
         const int studyNum = cellProjections[i].getStudyNumber();
         if (studyNum == indx) {
            cellProjections[i].setStudyNumber(-1);
         }
         else if (studyNum > indx) {
            cellProjections[i].setStudyNumber(studyNum - 1);
         }
      }
      
      studyInfo.erase(studyInfo.begin() + indx);
   }
}
      
/**
 * Set a study info.
 */
void
CellProjectionFile::setStudyInfo(const int index, const CellStudyInfo& csi)
{
   studyInfo[index] = csi;
}

/**
 * get indices to all linked studies.
 */
void 
CellProjectionFile::getPubMedIDsOfAllLinkedStudyMetaData(std::vector<QString>& studyPMIDs,
                                                         const bool displayedFociOnlyFlag) const
{
   std::set<QString> pmidSet;
   const int numCells = getNumberOfCellProjections();
   for (int i = 0; i < numCells; i++) {
      const CellProjection* cp = getCellProjection(i);
      bool useIt = true;
      if (displayedFociOnlyFlag) {
         useIt = cp->getDisplayFlag();
      }
      if (useIt) {
         const StudyMetaDataLinkSet smdl = cp->getStudyMetaDataLinkSet();
         std::vector<QString> pmids;
         smdl.getAllLinkedPubMedIDs(pmids);
         pmidSet.insert(pmids.begin(), pmids.end());
      }
   }
   studyPMIDs.clear();
   studyPMIDs.insert(studyPMIDs.end(),
                     pmidSet.begin(), pmidSet.end());
}

/**
 * update cell name with linked study name.
 */
void 
CellProjectionFile::updateCellNameWithStudyNameForMatchingPubMedIDs(const StudyMetaDataFile* smdf)
{
   const int numCells = getNumberOfCellProjections();
   for (int i = 0; i < numCells; i++) {
      CellProjection* cp = getCellProjection(i);
      //
      // Get the links
      //
      StudyMetaDataLinkSet smdls = cp->getStudyMetaDataLinkSet();
      //
      // Loop through the cell's links
      //
      for (int m = 0; m < smdls.getNumberOfStudyMetaDataLinks(); m++) {
         //
         // Get a link
         //
         StudyMetaDataLink smdl = smdls.getStudyMetaDataLink(m);
         
         //
         // Get cell PubMed ID and see if there is a study with the same PubMedID
         //
         const QString cellPubMedID = smdl.getPubMedID();
         int studyNum = smdf->getStudyIndexFromPubMedID(cellPubMedID);
         if (studyNum >= 0) {
            const StudyMetaData* smd = smdf->getStudyMetaData(studyNum);
            if (smd != NULL) {
               cp->setName(smd->getName());
               break;
            }
         }
      }
   }
}
      
/**
 * update cell PubMed ID if cell name matches study name.
 */
void 
CellProjectionFile::updatePubMedIDIfCellNameMatchesStudyName(const StudyMetaDataFile* smdf)
{
   const int numCells = getNumberOfCellProjections();
   const int numStudies = smdf->getNumberOfStudyMetaData();
   for (int i = 0; i < numCells; i++) {
      CellProjection* cp = getCellProjection(i);
      const QString name = cp->getName().trimmed();
      for (int j = 0; j < numStudies; j++) {
         const StudyMetaData* smd = smdf->getStudyMetaData(j);
         //
         // Does study name match cell name?
         //
         if (name == smd->getName().trimmed()) {
            //
            // Get the links
            //
            StudyMetaDataLinkSet smdls = cp->getStudyMetaDataLinkSet();
            bool linkModified = false;
            
            //
            // If the focus has no links add a link
            //
            if (smdls.getNumberOfStudyMetaDataLinks() <= 0) {
               StudyMetaDataLink smdl;
               smdls.addStudyMetaDataLink(smdl);
            }
            
            //
            // Loop through the cell's links
            //
            for (int m = 0; m < smdls.getNumberOfStudyMetaDataLinks(); m++) {
               //
               // Get a link
               //
               StudyMetaDataLink smdl = smdls.getStudyMetaDataLink(m);
               
               //
               // Only update if PubMed ID is different
               //
               const QString studyPubMedID = smd->getPubMedID();
               if (smdl.getPubMedID() != studyPubMedID) {
                  //
                  // Is the link to the study's Project ID?
                  //
                  if (smdl.getPubMedID() == smd->getProjectID()) {
                     //
                     // Do nothing
                     //
                  }
                  else {
                     //
                     // Clear study to clear out links to figures and tables since different study
                     //
                     // 08/10/2007  smdl.clear();
                  }
                  //
                  // Update only the link's PubMed ID
                  //
                  smdl.setPubMedID(studyPubMedID);
                  smdls.setStudyMetaDataLink(m, smdl);
                  linkModified = true;
               }
            }
            
            if (linkModified) {
               cp->setStudyMetaDataLinkSet(smdls);
            }
         }
      }
   }
}      

/**
 * find out if comma separated file conversion supported.
 */
void 
CellProjectionFile::getCommaSeparatedFileSupport(bool& readFromCSV,
                                       bool& writeToCSV) const
{
   readFromCSV = true;
   writeToCSV  = false;
}
                                        
/**
 * write the file's data into a comma separated values file (throws exception if not supported).
 */
void 
CellProjectionFile::writeDataIntoCommaSeparatedValueFile(CommaSeparatedValueFile& csv) throw (FileException)
{
   csv.clear();
   
   const int numCells = getNumberOfCellProjections();
   if (numCells <= 0) {
      return;
   }
   
   //
   // Column numbers for data
   //
   int numCols = 0;
   const int cellNumberCol = numCols++;
   const int xCol = numCols++;
   const int yCol = numCols++;
   const int zCol = numCols++;
   const int sectionNumberCol = numCols++;
   const int nameCol = numCols++;
   const int studyNumberCol = numCols++;
   const int geographyCol = numCols++;
   const int areaCol = numCols++;
   const int sizeCol = numCols++;
   const int statisticCol = numCols++;
   const int commentCol = numCols++;
   const int structureCol = numCols++;
   const int classNameCol = numCols++;
   const int volumeXYZCol = numCols++;
   const int duplicateFlagCol = numCols++;
   
   const int studyMetaPubMedCol = numCols++;
   const int studyMetaTableCol = numCols++;
   const int studyMetaTableSubHeaderCol = numCols++;
   const int studyMetaFigureCol = numCols++;
   const int studyMetaFigurePanelCol = numCols++;
   //const int studyMetaPageNumberCol = numCols++;
   
   // new stuff
   const int signedDistCol = numCols++;
   const int projTypeCol   = numCols++;
   const int closestTileVerticesCol = numCols++;
   const int closestTileAreaCol = numCols++;
   const int cDistanceCol = numCols++;
   const int drCol = numCols++;
   const int triFiducialCol = numCols++;
   const int thetaRCol = numCols++;
   const int phiRCol = numCols++;
   const int triVerticesCol = numCols++;
   const int vertexCol = numCols++;
   const int vertexFiducialCol = numCols++;
   const int posFiducialCol= numCols++;
   const int fracRICol = numCols++;
   const int fracRJCol = numCols++;
   
   //
   // Create and add to string table
   //
   StringTable* ct = new StringTable(numCells, numCols, "Cells");
   ct->setColumnTitle(cellNumberCol, "Cell Number");
   ct->setColumnTitle(xCol, "X");
   ct->setColumnTitle(yCol, "Y");
   ct->setColumnTitle(zCol, "Z");
   ct->setColumnTitle(sectionNumberCol, "Section");
   ct->setColumnTitle(nameCol, "Name");
   ct->setColumnTitle(studyNumberCol, "Study Number");
   ct->setColumnTitle(geographyCol, "Geography");
   ct->setColumnTitle(areaCol, "Area");
   ct->setColumnTitle(sizeCol, "Size");
   ct->setColumnTitle(statisticCol, "Statistic");
   ct->setColumnTitle(commentCol, "Comment");
   ct->setColumnTitle(structureCol, "Structure");
   ct->setColumnTitle(classNameCol, "Class Name");
   ct->setColumnTitle(volumeXYZCol, "Volume XYZ");
   ct->setColumnTitle(duplicateFlagCol, "Duplicate Flag");

   ct->setColumnTitle(studyMetaPubMedCol, "Study PubMed ID");
   ct->setColumnTitle(studyMetaTableCol, "Study Table Number");
   ct->setColumnTitle(studyMetaTableSubHeaderCol, "Study Table Subheader");
   ct->setColumnTitle(studyMetaFigureCol, "Study Figure Number");
   ct->setColumnTitle(studyMetaFigurePanelCol, "Study Figure Panel");
   //ct->setColumnTitle(studyMetaPageNumberCol, "Study Page Number");
   
   ct->setColumnTitle(signedDistCol, "Signed Dist");
   ct->setColumnTitle(projTypeCol, "Proj Type");
   ct->setColumnTitle(closestTileVerticesCol, "Closest Tile Vert");
   ct->setColumnTitle(closestTileAreaCol, "Closest Tile Areas");
   ct->setColumnTitle(cDistanceCol, "C Distance");
   ct->setColumnTitle(drCol, "dR");
   ct->setColumnTitle(triFiducialCol, "Tri Fiducial");
   ct->setColumnTitle(thetaRCol, "thetaR");
   ct->setColumnTitle(phiRCol, "phiR");
   ct->setColumnTitle(triVerticesCol, "Tri Vertices");
   ct->setColumnTitle(vertexCol, "Vertex");
   ct->setColumnTitle(vertexFiducialCol, "Vertex Fiducial");
   ct->setColumnTitle(posFiducialCol, "Pos Fiducial");
   ct->setColumnTitle(fracRICol, "fracRI");
   ct->setColumnTitle(fracRJCol, "fracRJ");

   for (int i = 0; i < numCells; i++) {
      const CellProjection* cd = getCellProjection(i);
      const float* xyz = cd->getXYZ();
      ct->setElement(i, cellNumberCol, i);
      ct->setElement(i, xCol, xyz[0]);
      ct->setElement(i, yCol, xyz[1]);
      ct->setElement(i, zCol, xyz[2]);
      ct->setElement(i, sectionNumberCol, cd->getSectionNumber());
      ct->setElement(i, nameCol, cd->getName());
      ct->setElement(i, studyNumberCol, cd->getStudyNumber());
      ct->setElement(i, geographyCol, cd->getGeography());
      ct->setElement(i, areaCol, cd->getArea());
      ct->setElement(i, sizeCol, cd->getSize());
      ct->setElement(i, statisticCol, cd->getStatistic());
      ct->setElement(i, commentCol, cd->getComment());
      ct->setElement(i, structureCol, Structure::convertTypeToString(cd->getCellStructure()));
      ct->setElement(i, classNameCol, cd->getClassName());
      ct->setElement(i, volumeXYZCol, (float*)cd->volumeXYZ, 3);
      if (cd->getDuplicateFlag()) {
         ct->setElement(i, duplicateFlagCol, "true");
      }
      else {
         ct->setElement(i, duplicateFlagCol, "false");
      }
      ct->setElement(i, signedDistCol, cd->getSignedDistanceAboveSurface());
      switch (cd->projectionType) {
         case CellProjection::PROJECTION_TYPE_UNKNOWN:
            ct->setElement(i, projTypeCol, "UNKNOWN");
            break;
         case CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE:
            ct->setElement(i, projTypeCol, "INSIDE");
            break;
         case CellProjection::PROJECTION_TYPE_OUTSIDE_TRIANGLE:
            ct->setElement(i, projTypeCol, "OUTSIDE");
            break;
      }
      ct->setElement(i, closestTileVerticesCol, (int*)(cd->closestTileVertices), 3);
      ct->setElement(i, closestTileAreaCol, cd->closestTileAreas, 3);
      ct->setElement(i, cDistanceCol, cd->cdistance, 3);
      ct->setElement(i, drCol, cd->dR);
      ct->setElement(i, triFiducialCol, (float*)cd->triFiducial, 2*3*3);
      ct->setElement(i, thetaRCol, cd->thetaR);
      ct->setElement(i, phiRCol, cd->phiR);
      ct->setElement(i, triVerticesCol, (int*)cd->triVertices, 2*3);
      ct->setElement(i, vertexCol, (int*)cd->vertex, 2);
      ct->setElement(i, vertexFiducialCol, (float*)cd->vertexFiducial, 6);
      ct->setElement(i, posFiducialCol, (float*)cd->posFiducial, 3);
      ct->setElement(i, fracRICol, cd->fracRI);
      ct->setElement(i, fracRJCol, cd->fracRJ);
      
      const StudyMetaDataLinkSet smdls = cd->getStudyMetaDataLinkSet();
      StudyMetaDataLink smdl;
      if (smdls.getNumberOfStudyMetaDataLinks() > 1) {
         const QString msg("Cell[" 
                           + QString::number(i)
                           + "] named \""
                           + cd->getName()
                           + "\" has more than one Study Metadata Link so it "
                             "cannot be written as a Comma Separated Value File");
         throw FileException(msg);
      }
      else if (smdls.getNumberOfStudyMetaDataLinks() == 1) {
         smdl = smdls.getStudyMetaDataLink(0);
      }

      ct->setElement(i, studyMetaPubMedCol, smdl.getPubMedID());
      ct->setElement(i, studyMetaTableCol, smdl.getTableNumber());
      ct->setElement(i, studyMetaTableSubHeaderCol, smdl.getTableSubHeaderNumber());
      ct->setElement(i, studyMetaFigureCol, smdl.getFigureNumber());
      ct->setElement(i, studyMetaFigurePanelCol, smdl.getFigurePanelNumberOrLetter());
      //ct->setElement(i, studyMetaPageNumberCol, smdl.getPageNumber());
   }

   StringTable* headerTable = new StringTable(0, 0);
   writeHeaderDataIntoStringTable(*headerTable);
   csv.addDataSection(headerTable);
   csv.addDataSection(ct);
   StringTable* studyInfoTable = new StringTable(0, 0);
   CellStudyInfo::writeDataIntoStringTable(studyInfo, *studyInfoTable);
   csv.addDataSection(studyInfoTable);
}

/**
 * read the file's data from a comma separated values file (throws exception if not supported).
 */
void 
CellProjectionFile::readDataFromCommaSeparatedValuesTable(const CommaSeparatedValueFile& csv) throw (FileException)
{
   clear();
   
   const StringTable* ct = csv.getDataSectionByName("Cells");   
   if (ct == NULL) {
      throw FileException("No cells found");
   }
   
   int numCols = ct->getNumberOfColumns();
   
   int cellNumberCol = -1;
   int xCol = -1;
   int yCol = -1;
   int zCol = -1;
   int sectionNumberCol = -1;
   int nameCol = -1;
   int studyNumberCol = -1;
   int geographyCol = -1;
   int areaCol = -1;
   int sizeCol = -1;
   int statisticCol = -1;
   int commentCol = -1;
   int structureCol = -1;
   int classNameCol = -1;
   int volumeXYZCol = -1;
   int duplicateFlagCol = -1;
   
   int studyMetaPubMedCol = -1;
   int studyMetaTableCol = -1;
   int studyMetaTableSubHeaderCol = -1;
   int studyMetaFigureCol = -1;
   int studyMetaFigurePanelCol = -1;
   //int studyMetaPageNumberCol = -1;
   
   int signedDistCol = -1;
   int projTypeCol   = -1;
   int closestTileVerticesCol = -1;
   int closestTileAreaCol = -1;
   int cDistanceCol = -1;
   int drCol = -1;
   int triFiducialCol = -1;
   int thetaRCol = -1;
   int phiRCol = -1;
   int triVerticesCol = -1;
   int vertexCol = -1;
   int vertexFiducialCol = -1;
   int posFiducialCol= -1;
   int fracRICol = -1;
   int fracRJCol = -1;

   for (int i = 0; i < numCols; i++) {
      const QString columnTitle = ct->getColumnTitle(i).toLower();
      if (columnTitle == "cell number") {
         cellNumberCol = i;
      }
      else if (columnTitle == "x") {
         xCol = i;
      }
      else if (columnTitle == "y") {
         yCol = i;
      }
      else if (columnTitle == "z") {
         zCol = i;
      }
      else if (columnTitle == "section") {
         sectionNumberCol = i;
      }
      else if (columnTitle == "name") {
         nameCol = i;
      }
      else if (columnTitle == "study number") {
         studyNumberCol = i;
      }
      else if (columnTitle == "geography") {
         geographyCol = i;
      }
      else if (columnTitle == "area") {
         areaCol = i;
      }
      else if (columnTitle == "size") {
         sizeCol = i;
      }
      else if (columnTitle == "statistic") {
         statisticCol = i;
      }
      else if (columnTitle == "comment") {
         commentCol = i;
      }
      else if (columnTitle == "structure") {
         structureCol = i;
      }
      else if (columnTitle == "class name") {
         classNameCol = i;
      }   
      else if (columnTitle == "volume xyz") {
         volumeXYZCol = i;
      }
      else if (columnTitle == "duplicate flag") {
         duplicateFlagCol = i;
      }
      else if (columnTitle == "signed dist") {   
         signedDistCol = i;
      }   
      else if (columnTitle == "proj type") {   
         projTypeCol   = i;
      }   
      else if (columnTitle == "closest tile vert") {   
         closestTileVerticesCol = i;
      }   
      else if (columnTitle == "closest tile areas") {   
         closestTileAreaCol = i;
      }   
      else if (columnTitle == "c distance") {   
         cDistanceCol = i;
      }   
      else if (columnTitle == "dr") {   
         drCol = i;
      }   
      else if (columnTitle == "tri fiducial") {   
         triFiducialCol = i;
      }   
      else if (columnTitle == "thetar") {   
         thetaRCol = i;
      }   
      else if (columnTitle == "phir") {   
         phiRCol = i;
      }   
      else if (columnTitle == "tri vertices") {   
         triVerticesCol = i;
      }   
      else if (columnTitle == "vertex") {   
         vertexCol = i;
      }   
      else if (columnTitle == "vertex fiducial") {   
         vertexFiducialCol = i;
      }   
      else if (columnTitle == "pos fiducial") {   
         posFiducialCol= i;
      }   
      else if (columnTitle == "fracri") {   
         fracRICol = i;
      }   
      else if (columnTitle == "fracrj") {   
         fracRJCol = i;
      }
      else if (columnTitle == "study pubmed id") {
         studyMetaPubMedCol = i;
      }
      else if (columnTitle == "study table number") {
         studyMetaTableCol = i;
      }
      else if (columnTitle == "study table subheader") {
         studyMetaTableSubHeaderCol = i;
      }
      else if (columnTitle == "study figure number") {
         studyMetaFigureCol = i;
      }
      else if (columnTitle == "study figure panel") {
         studyMetaFigurePanelCol = i;
      }
      //else if (columnTitle == "study page number") {
      //   studyMetaPageNumberCol = i;
      //}
   }
   
   for (int i = 0; i < ct->getNumberOfRows(); i++) {
      CellProjection cd(csv.getFileName());
      float xyz[3] = { 0.0, 0.0, 0.0 };
      int section = 0;
      QString name;
      int studyNumber = -1;
      QString geography;
      QString area;
      float size = 0.0;
      QString statistic;
      QString comment;
      Structure::STRUCTURE_TYPE structure = Structure::STRUCTURE_TYPE_INVALID;
      QString className;
      StudyMetaDataLink smdl;
      
      if (xCol >= 0) {
         xyz[0] = ct->getElementAsFloat(i, xCol);
      }
      if (yCol >= 0) {
         xyz[1] = ct->getElementAsFloat(i, yCol);
      }
      if (zCol >= 0) {
         xyz[2] = ct->getElementAsFloat(i, zCol);
      }
      if (sectionNumberCol >= 0) {
         section = ct->getElementAsInt(i, sectionNumberCol);
      }
      if (nameCol >= 0) {
         name = ct->getElement(i, nameCol);
      }
      if (studyNumberCol >= 0) {
         studyNumber = ct->getElementAsInt(i, studyNumberCol);
      }
      if (geographyCol >= 0) {
         geography = ct->getElement(i, geographyCol);
      }
      if (areaCol >= 0) {
         area = ct->getElement(i, areaCol);
      }
      if (sizeCol >= 0) {
         size = ct->getElementAsFloat(i, sizeCol);
      }
      if (statisticCol >= 0) {
         statistic = ct->getElement(i, statisticCol);
      }
      if (commentCol >= 0) {
         comment = ct->getElement(i, commentCol);
      }
      if (structureCol >= 0) {
         structure = Structure::convertStringToType(ct->getElement(i, structureCol));
      }
      if (classNameCol >= 0) {
         className = ct->getElement(i, classNameCol);
         if (className == "???") {
            className = "";
         }
      }
      if (volumeXYZCol >= 0) {
         ct->getElement(i, volumeXYZCol, (float*)cd.volumeXYZ, 3);
      }
      if (duplicateFlagCol >= 0) {
         cd.setDuplicateFlag(false);
         if (ct->getElement(i, duplicateFlagCol) == "true") {
            cd.setDuplicateFlag(true);
         }
      }
      
      if (signedDistCol >= 0) {
         cd.signedDistanceAboveSurface = ct->getElementAsFloat(i, signedDistCol);
      }
      if (projTypeCol >= 0) {
         cd.projectionType = CellProjection::PROJECTION_TYPE_UNKNOWN;
         if (ct->getElement(i, projTypeCol) == "UNKNOWN") {
            cd.projectionType = CellProjection::PROJECTION_TYPE_UNKNOWN;
         }
         if (ct->getElement(i, projTypeCol) == "INSIDE") {
            cd.projectionType = CellProjection::PROJECTION_TYPE_INSIDE_TRIANGLE;
         }
         if (ct->getElement(i, projTypeCol) == "OUTSIDE") {
            cd.projectionType = CellProjection::PROJECTION_TYPE_OUTSIDE_TRIANGLE;
         }
      }
      if (closestTileVerticesCol >= 0) {
         ct->getElement(i, closestTileVerticesCol, (int*)cd.closestTileVertices, 3);
      }
      if (closestTileAreaCol >= 0) {
         ct->getElement(i, closestTileAreaCol, (float*)cd.closestTileAreas, 3);
      }
      if (cDistanceCol >= 0) {
         ct->getElement(i, cDistanceCol, cd.cdistance, 3);
      }
      if (drCol >= 0) {
         cd.dR = ct->getElementAsFloat(i, drCol);
      }
      if (triFiducialCol >= 0) {
         ct->getElement(i, triFiducialCol, (float*)cd.triFiducial, 2*3*3);
      }
      if (thetaRCol >= 0) {
         cd.thetaR = ct->getElementAsFloat(i, thetaRCol);
      }
      if (phiRCol >= 0) {
         cd.phiR = ct->getElementAsFloat(i, phiRCol);
      }
      if (triVerticesCol >= 0) {
         ct->getElement(i, triVerticesCol, (int*)cd.triVertices, 2*3);
      }
      if (vertexCol >= 0) {
         ct->getElement(i, vertexCol, (int*)cd.vertex, 2);
      }
      if (vertexFiducialCol >= 0) {
         ct->getElement(i, vertexFiducialCol, (float*)cd.vertexFiducial, 2*3);
      }
      if (posFiducialCol >= 0) {
         ct->getElement(i, posFiducialCol, (float*)cd.posFiducial, 3);
      }
      if (fracRICol >= 0) {
         cd.fracRI = ct->getElementAsFloat(i, fracRICol);
      }
      if (fracRJCol >= 0) {
         cd.fracRJ = ct->getElementAsFloat(i, fracRJCol);
      }
      if (studyMetaPubMedCol >= 0) {
         smdl.setPubMedID(ct->getElement(i, studyMetaPubMedCol));
      } 
      if (studyMetaTableCol >= 0) {
         smdl.setTableNumber(ct->getElement(i, studyMetaTableCol));
      } 
      if (studyMetaTableSubHeaderCol >= 0) {
         smdl.setTableSubHeaderNumber(ct->getElement(i, studyMetaTableSubHeaderCol));
      } 
      if (studyMetaFigureCol >= 0) {
         smdl.setFigureNumber(ct->getElement(i, studyMetaFigureCol));
      } 
      if (studyMetaFigurePanelCol >= 0) {
         smdl.setFigurePanelNumberOrLetter(ct->getElement(i, studyMetaFigurePanelCol));
      } 
      //if (studyMetaPageNumberCol >= 0) {
      //   smdl.setPageNumber(ct->getElement(i, studyMetaPageNumberCol));
      //} 
      
      cd.setXYZ(xyz);
      cd.setSectionNumber(section);
      cd.setName(name);
      cd.setStudyNumber(studyNumber);
      cd.setGeography(geography);
      cd.setArea(area);
      cd.setSize(size);
      cd.setStatistic(statistic);
      cd.setComment(comment);
      cd.setCellStructure(structure);
      cd.setClassName(className);
      StudyMetaDataLinkSet smdls;
      if (smdl.getPubMedID().isEmpty() == false) {
         smdls.addStudyMetaDataLink(smdl);
      }
      cd.setStudyMetaDataLinkSet(smdls);
      
      addCellProjection(cd);
   }
   
   //
   // Do header
   //
   const StringTable* stHeader = csv.getDataSectionByName("header");
   if (stHeader != NULL) {
      readHeaderDataFromStringTable(*stHeader);
   }
    
   //
   // Do study info
   //
   const StringTable* stcsi = csv.getDataSectionByName("Cell Study Info");
   if (stcsi != NULL) {
      CellStudyInfo::readDataFromStringTable(studyInfo, *stcsi);
   }
}
      
/**
 * Read a cell projection file's version number.
 */
int 
CellProjectionFile::readFilesVersionNumber(const QString& filename)
{
   CellProjectionFile cpf;
   cpf.readVersionNumberOnly = 1;
   cpf.readFile(filename);
   cpf.readVersionNumberOnly = 0;
   return cpf.versionNumber;
}

/**
 * Read a cell projection version 1 file.
 */
void
CellProjectionFile::readFileVersion1(QTextStream& stream, const int numProjections,
                                     const int numStudyInfo) throw (FileException)
{
   for (int i = 0; i < numProjections; i++) {
      CellProjection cp(getFileName());
      cp.readFileDataVersion1(stream);
      addCellProjection(cp);
   }
   
   for (int j = 0; j < numStudyInfo; j++) {
      QString line;
      readLine(stream, line);
      int firstBlank = line.indexOf(' ');
      if (firstBlank != -1) {
         line = line.mid(firstBlank + 1);
      }
      CellStudyInfo csi;
      csi.setTitle(StringUtilities::setupCommentForDisplay(line));
      addStudyInfo(csi);
   }
}

/**
 * Read a version 2 cell projection file.
 */
void
CellProjectionFile::readFileVersion2(QTextStream& stream, const int numProjections,
                                     const int numStudyInfo) throw (FileException)
{
   for (int i = 0; i < numProjections; i++) {
      CellProjection cp(getFileName());
      cp.readFileDataVersion2(stream);
      addCellProjection(cp);
   }
   
   for (int j = 0; j < numStudyInfo; j++) {
      QString line;
      readLine(stream, line);
      int firstBlank = line.indexOf(' ');
      if (firstBlank != -1) {
         line = line.mid(firstBlank + 1);
      }
      CellStudyInfo csi;
      csi.setTitle(StringUtilities::setupCommentForDisplay(line));
      addStudyInfo(csi);
   }
}

/**
 * Read a version 3 cell projection file.
 */
void
CellProjectionFile::readFileVersion3(QFile& /*file*/, QTextStream& stream, 
                                     const int numProjections) throw (FileException)
{
   for (int i = 0; i < numProjections; i++) {
      CellProjection cp(getFileName());
      cp.readFileDataVersion2(stream);
      addCellProjection(cp);
   }
   
   //
   // studies may not begin at zero.  So, this converts study numbers
   // to their numbers in this file
   //
   std::vector<int> studyIndexer;
   
   while (stream.atEnd() == false) {
      //
      // Get number, tag, and value
      //
      int number;
      QString tag;
      QString tagValue;
      readNumberedTagLine(stream, number, tag, tagValue);
      
      if (tagValue.isEmpty() == false) {
         int index = -1;
         for (int k = 0; k < static_cast<int>(studyIndexer.size()); k++) {
            if (studyIndexer[k] == number) {
               index = k;
               break;
            }
         }
         if (index < 0) {
            CellStudyInfo csi;
            index = addStudyInfo(csi);
            studyIndexer.push_back(number);
         }
         
         tagValue = StringUtilities::setupCommentForDisplay(tagValue);
         if (tag == tagCommentUrl) {
            studyInfo[index].setURL(tagValue);
         }
         else if (tag == tagCommentKeyWords) {
            studyInfo[index].setKeywords(tagValue);
         }
         else if (tag == tagCommentTitle) {
            studyInfo[index].setTitle(tagValue);
         }
         else if (tag == tagCommentAuthors) {
            studyInfo[index].setAuthors(tagValue);
         }
         else if (tag == tagCommentCitation) {
            studyInfo[index].setCitation(tagValue);
         }
         else if (tag == tagCommentStereotaxicSpace) {
            studyInfo[index].setStereotaxicSpace(tagValue);
         }
         else {
            std::cout << "Unrecognized cell/foci tag: " << tag.toAscii().constData() << std::endl;
         }
      }
   }
}

/**
 * read the file with an xml stream reader.
 */
void 
CellProjectionFile::readFileWithXmlStreamReader(QFile& /*file*/) throw (FileException)
{
/*
   //
   // Move to beginning of file
   //
   file.reset();
   
   QXmlSimpleReader reader;
   CellProjectionFileSaxReader saxReader(this);
   reader.setContentHandler(&saxReader);
   reader.setErrorHandler(&saxReader);
 
   //
   // Some constant to determine how to read a file based upon the file's size
   //
   const int oneMegaByte = 1048576;
   const qint64 bigFileSize = 25 * oneMegaByte;
   
   if (file.size() < bigFileSize) {
      //
      // This call reads the entire file at once but this is a problem
      // since the XML files can be very large and will cause the 
      // QT XML parsing to crash
      //
      if (reader.parse(&file) == false) {
         throw FileException(filename, saxReader.getErrorMessage());
      }
   }
   else {
      //
      // The following code reads the XML file in pieces
      // and hopefully will prevent QT from crashing when
      // reading large files
      //
      
      //
      // Create a data stream
      //   
      QDataStream stream(&file);
	  stream.setVersion(QDataStream::Qt_4_3);
      
      //
      // buffer for data read
      //
      const int bufferSize = oneMegaByte;
      char buffer[bufferSize];
      
      //
      // the XML input source
      //
      QXmlInputSource xmlInput;

      int totalRead = 0;
      
      bool firstTime = true;
      while (stream.atEnd() == false) {
         int numRead = stream.readRawData(buffer, bufferSize);
         totalRead += numRead;
         if (DebugControl::getDebugOn()) {
            std::cout << "Cell Projection large file read, total: " << numRead << ", " << totalRead << std::endl;
         }
         
         //
         // Place the input data into the XML input
         //
         xmlInput.setData(QByteArray(buffer, numRead));
         
         //
         // Process the data that was just read
         //
         if (firstTime) {
            if (reader.parse(&xmlInput, true) == false) {
               throw FileException(filename, saxReader.getErrorMessage());            
            }
         }
         else {
            if (reader.parseContinue() == false) {
               throw FileException(filename, saxReader.getErrorMessage());
            }
         }
         
         firstTime = false;
      }
      
      //
      // Tells parser that there is no more data
      //
      xmlInput.setData(QByteArray());
      if (reader.parseContinue() == false) {
         throw FileException(filename, saxReader.getErrorMessage());
      }
   }
*/
}
                                 
/**
 * Read the cell projection file.
 */
void
CellProjectionFile::readFileData(QFile& file, QTextStream& stream, QDataStream&,
                                  QDomElement& rootElement) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   versionNumber = -1;
   
   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         {
            bool readingTags = true;
            bool firstTag = true;
            int  numProjections = -1;
            int  numStudyInfo = 0;
            qint64 pos = stream.pos(); //file.pos();
            
            while(readingTags) {
               QString tag, value;
               readTagLine(stream, tag, value);
               if (tag == tagFileVersion) {
                  versionNumber = value.toInt();
               }
               else if (tag == tagNumberOfCellProjections) {
                  numProjections = value.toInt();
               }
               else if (tag == tagBeginData) {
                  readingTags = false;
               }
               else if (tag == tagNumberOfComments) {
                  numStudyInfo = value.toInt();
               }
               else if (firstTag) {
                  //
                  // Must be old version since there are no tags
                  //
                  versionNumber = 0;
                  file.seek(pos);
                  stream.seek(pos);
                  readingTags = false;
               }
               else {
                  std::cerr << "WARNING: Unrecognized cell projection file tag " << tag.toAscii().constData() << std::endl;
               }
               firstTag = false;
            }
            
            if (readVersionNumberOnly) {
               return;
            }
               
            switch (versionNumber) {
               case 0:
                  throw FileException(filename, "Version 0 projection file no longer supported."
                                        "  Reproject your original cell file.");
                  break;
               case 1:
                  readFileVersion1(stream, numProjections, numStudyInfo);
                  break;
               case 2:
                  readFileVersion2(stream, numProjections, numStudyInfo);
                  break;
               case 3:
                  readFileVersion3(file, stream, numProjections);
                  break;
               default:
                  throw FileException(filename, "Unrecognized cell projection file version");
                  break;
            }
         }
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         if (DebugControl::getTestFlag2()) {
            readFileWithXmlStreamReader(file);
         }
         else {
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) { 
                  //
                  // Is this a "CellData" element
                  //
                  if (elem.tagName() == CellProjection::tagCellProjection) {
                     CellProjection cp(getFileName());
                     cp.readXMLWithDOM(node);
                     addCellProjection(cp);
                  }
                  else if (elem.tagName() == CellStudyInfo::tagCellStudyInfo) {
                     CellStudyInfo csi;
                     csi.readXML(node);
                     addStudyInfo(csi);
                  }
                  else if ((elem.tagName() == xmlHeaderOldTagName) ||
                           (elem.tagName() == xmlHeaderTagName)) {
                     // ignore, read by AbstractFile::readFile()
                  }
                  else {
                     std::cout << "WARNING: unrecognized CellFile element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
                  }
               }
               node = node.nextSibling();
            }
         }
         versionNumber = 10;
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Reading XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Reading XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Reading XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            csvf.readFromTextStream(file, stream);
            readDataFromCommaSeparatedValuesTable(csvf);
         }
         break;
   }
   
   const int numProj = getNumberOfCellProjections();
   for (int i = 0; i < numProj; i++) {
      CellProjection* cp = getCellProjection(i);
      cp->updateInvalidCellStructureUsingXCoordinate();
      const int studyNumber = cp->getStudyNumber();
      if (studyNumber >= 0) {
         if (studyNumber >= getNumberOfStudyInfo()) {
            cp->setStudyNumber(-1);
         }
      }
   }
}

/**
 * Write the cell projection file.
 */
void
CellProjectionFile::writeFileData(QTextStream& stream, QDataStream&,
                                 QDomDocument& xmlDoc,
                                  QDomElement& rootElement) throw (FileException)
{
   //writeFileVersion2(stream);
   //writeFileVersion3(stream);
   
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Writing in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Writing in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            //
            // Write the cell projections
            //
            const int numCellProjections = getNumberOfCellProjections();
            for (int i = 0; i < numCellProjections; i++) {
               CellProjection* cp = getCellProjection(i);
               cp->writeXML(xmlDoc, rootElement, i);
            }
            
            //
            // Write the study info
            //
            const int numStudyInfo = getNumberOfStudyInfo();
            for (int i = 0; i < numStudyInfo; i++) {
               studyInfo[i].writeXML(xmlDoc, rootElement, i);
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "Writing XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "Writing XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         {
            CommaSeparatedValueFile csvf;
            writeDataIntoCommaSeparatedValueFile(csvf);
            csvf.writeToTextStream(stream);
         }
         break;
   }
}

/**
 * Write a version 2 cell file.
 */
void
CellProjectionFile::writeFileVersion2(QTextStream& stream) throw (FileException)
{
   stream << tagFileVersion << " 2\n";
   stream << tagNumberOfCellProjections << " " <<cellProjections.size() << "\n";
   stream << tagNumberOfComments << " " << studyInfo.size() << "\n";
   stream << tagBeginData << "\n";
   
   for (unsigned int i = 0; i < cellProjections.size(); i++) {
      cellProjections[i].writeFileData(stream, i);
   }
   for (unsigned int j = 0; j < studyInfo.size(); j++) {
      stream << j << " " 
             << StringUtilities::setupCommentForStorage(studyInfo[j].getTitle()) 
             << "\n";
   }
}

/**
 * Write a version 3 cell file.
 */
void
CellProjectionFile::writeFileVersion3(QTextStream& stream) throw (FileException)
{
   stream << tagFileVersion << " 3\n";
   stream << tagNumberOfCellProjections << " " <<cellProjections.size() << "\n";
   stream << tagNumberOfComments << " " << studyInfo.size() << "\n";
   stream << tagBeginData << "\n";
   
   for (unsigned int i = 0; i < cellProjections.size(); i++) {
      cellProjections[i].writeFileData(stream, i);
   }
   for (unsigned int j = 0; j < studyInfo.size(); j++) {
      stream << j << " " << tagCommentUrl 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getURL()) 
             << "\n";
      stream << j << " " << tagCommentKeyWords 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getKeywords()) 
             << "\n";
      stream << j << " " << tagCommentTitle 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getTitle()) 
             << "\n";
      stream << j << " " << tagCommentAuthors 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getAuthors()) 
             << "\n";
      stream << j << " " << tagCommentCitation 
             << " " << StringUtilities::setupCommentForStorage(studyInfo[j].getCitation()) 
             << "\n";
      stream << j << " " << tagCommentStereotaxicSpace
             << " " << studyInfo[j].getStereotaxicSpace()
             << "\n";
   }
}


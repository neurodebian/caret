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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <cmath>
#include <limits>

#include <QRegExp>

#include "BorderFile.h"
#include "BorderToTopographyConverter.h"
#include "BrainModelSurface.h"
#include "PaintFile.h"
#include "TopographyFile.h"

/*
static QString topographyNames[] = {
                                          "Emean",
                                          "Elow",
                                          "Ehigh",
                                          "Pmean",
                                          "Plow",
                                          "Phigh",
                                          "None" 
                                       };
*/
                                       
/**
 * constructor.
 */
BorderToTopographyConverter::BorderToTopographyConverter(BrainSet* bs,
                              BrainModelSurface* surfaceIn,
                              BorderFile* borderFileIn,
                              PaintFile* paintFileIn,
                              TopographyFile* topographyFileIn,
                              const int topographyFileColumnIn,
                              const QString topographyFileColumnNameIn)
   : BrainModelAlgorithm(bs)
{
   surface = surfaceIn;
   borderFile = borderFileIn;
   paintFile  = paintFileIn;
   topographyFile = topographyFileIn;
   topographyFileColumn = topographyFileColumnIn;
   topographyFileColumnName = topographyFileColumnNameIn;
}

/**
 * destructor.
 */
BorderToTopographyConverter::~BorderToTopographyConverter()
{
}

/**
 * execute the algorithm
 */
void 
BorderToTopographyConverter::execute() throw (BrainModelAlgorithmException)
{
   if (surface == NULL) {
      throw BrainModelAlgorithmException("No surface provided.");
   }
   CoordinateFile* coords = surface->getCoordinateFile();
   const int numNodes = coords->getNumberOfCoordinates();
   if (numNodes <= 0) {
      throw BrainModelAlgorithmException("Surface has no coordinates.");
   }

   if (borderFile == NULL) {
      throw BrainModelAlgorithmException("No borders provided.");
   }
   const int numBorders = borderFile->getNumberOfBorders();
   if (numBorders <= 0) {
      throw BrainModelAlgorithmException("Border file is empty.");
   }

   if (paintFile == NULL) {
      throw BrainModelAlgorithmException("No paint file provided.");
   }
   if (paintFile->getNumberOfColumns() != 6) {
      throw BrainModelAlgorithmException("Paint file must have six columns.");
   }
   
   //
   // use a regular expression to match borders
   // borders to match are of the form  area.type.number
   // examples: v1.Pmean.2   v2.Ehigh.8
   //
   QString topoBorderStringRE = "(.+)\\.(.+)\\.([0-9]+)$";
   QRegExp topoBorderRE(topoBorderStringRE);
   if (topoBorderRE.isValid() == false) {
      QString msg("Regular expression error: ");
      msg.append(topoBorderStringRE);
      throw BrainModelAlgorithmException(msg);
   }
   
   //
   // use a regular expression to match node names
   // node names to match are of the form area.type_number1_number2
   // examples: V1.Emean_1_2 V2.Ehigh_5_6
   //
   QString areaBorderStringRE = "(.+)\\.(.+)_([0-9]+)_([0-9]+)$";
   QRegExp areaBorderRE(areaBorderStringRE);
    if (areaBorderRE.isValid() == false) {
      QString msg("Regular expression error: ");
      msg.append(areaBorderStringRE);
      throw BrainModelAlgorithmException(msg);
   }
   
   // loop through borders to get its area name and type
   //
   TOPOGRAPHY_TYPES* borderType = new TOPOGRAPHY_TYPES[numBorders];
   QString* borderAreaName = new QString[numBorders];
   int* borderAreaNumber = new int[numBorders];
   bool haveTopographyValue = false;
   bool haveTopographyBorders = false;
   for (int j = 0; j < numBorders; j++) {
      Border* b = borderFile->getBorder(j);
      
      getBorderAreaAndType(b->getName(), topoBorderRE, 
                           borderAreaName[j], borderType[j], 
                           borderAreaNumber[j]);
   
      /*
      printf("Border %d: TYPE = %s  AREA NAME = %s NUMBER = %d "
             "TOPOGRAPHY_VALUE = %f\n", 
             j, topographyNames[static_cast<int>(borderType[j])], 
             borderAreaName[j], borderAreaNumber[j], b.topographyValue);
      */
             
      if (borderType[j] != TYPE_NONE) {
         haveTopographyBorders = true;
         if (b->getTopographyValue() != 0.0) {
            haveTopographyValue = true;
         }
      }
   }
   
   if (haveTopographyBorders == false) {
      throw BrainModelAlgorithmException(
            "No Borders are of topography type like \"v1.Pmean.2\"");
      delete[] borderType;
      delete[] borderAreaName;
      return;
   }
   if (haveTopographyValue == false) {
      throw BrainModelAlgorithmException(
            "All topography borders have topography value equal to zero");
      delete[] borderType;
      delete[] borderAreaName;
      return;
   }
   
   bool*   topoValid    = new bool[numNodes];
   QString* topoAreaName = new QString[numNodes];
   float*  topoEmean    = new float[numNodes];
   float*  topoElow     = new float[numNodes];
   float*  topoEhigh    = new float[numNodes];
   float*  topoPmean    = new float[numNodes];
   float*  topoPlow     = new float[numNodes];
   float*  topoPhigh    = new float[numNodes];
   for (int m = 0; m < numNodes; m++) {
      topoValid[m] = false;
      topoEmean[m] = 0.0;
      topoElow[m]  = 0.0;
      topoEhigh[m] = 0.0;
      topoPmean[m] = 0.0;
      topoPlow[m]  = 0.0;
      topoPhigh[m] = 0.0;
   }
      
   //
   // loop through 6 Topography Types (Emean, Elow, Ehigh, Pmean, Plow, Phigh)
   //
   for (int paintIndex = 0; paintIndex < 6; paintIndex++ ) { 
      for (int j = 0; j < numNodes; j++) { 
         //
         // see if point has a valid name
         //
         const QString nodeName = paintFile->getPaintNameFromIndex(paintFile->getPaint(j, paintIndex));
         if (nodeName == "???") {
         
            QString nodeAreaName;
            TOPOGRAPHY_TYPES nodeTopographyType;
            int nodeArea1, nodeArea2;
            getNodeAreaTypeAndBorders(nodeName, areaBorderRE,
                                      nodeAreaName, nodeTopographyType,
                                      nodeArea1, nodeArea2);
            
            //
            // if node is of type being tested 
            //                       
            //if (nodeTopographyType == topographyTest) {
            if (nodeTopographyType != TYPE_NONE) {
               int closestBorder1 = -1;
               int closestBorder2 = -1;
               float closestBorderDist1 = std::numeric_limits<float>::max();
               float closestBorderDist2 = std::numeric_limits<float>::max();
               // 
               // loop through borders
               //
               for (int i = 0; i < numBorders; i++) {
                  Border* b = borderFile->getBorder(i);
                  //
                  // if border is of type being checked
                  //
                  if (nodeTopographyType == borderType[i]) {
                     //
                     // see if border's area number matches one of the
                     // nodes border area numbers
                     //
                     if ((borderAreaNumber[i] == nodeArea1) ||
                         (borderAreaNumber[i] == nodeArea2)) {
                        //
                        // if border area name is in node's area name
                        //
                        if (nodeAreaName.indexOf(borderAreaName[i]) != 
                            -1) {
                           topoAreaName[j] = borderAreaName[i];  // nodeAreaName;
                           const float dist = getClosestBorderPointDistance(
                                                           b, coords->getCoordinate(j));
                           if (dist < closestBorderDist1) {
                              closestBorderDist2 = closestBorderDist1;
                              closestBorder2     = closestBorder1;
                              closestBorderDist1 = dist;
                              closestBorder1     = i;
                           }
                           else if (dist < closestBorderDist2) {
                              closestBorderDist2 = dist;
                              closestBorder2     = i;
                           }
                        }  // matched border name
                     }   // matched border number
                  }   // matched border type
               }   // number of borders

               if (closestBorder2 >= 0) {
                  topoValid[j] = true;
                  const Border* b1 = borderFile->getBorder(closestBorder1);
                  const Border* b2 = borderFile->getBorder(closestBorder2);
                  const float value = interpolateBorderTopoValues(
                                        closestBorderDist1, closestBorderDist2,
                                        b1->getTopographyValue(),
                                        b2->getTopographyValue());
                  switch(nodeTopographyType) {
                     case TYPE_EMEAN:
                        topoEmean[j] = value;
                        break;
                     case TYPE_ELOW:
                        topoElow[j] = value;
                        break;
                     case TYPE_EHIGH:
                        topoEhigh[j] = value;
                        break;
                     case TYPE_PMEAN:
                        topoPmean[j] = value;
                        break;
                     case TYPE_PLOW:
                        topoPlow[j] = value;
                        break;
                     case TYPE_PHIGH:
                        topoPhigh[j] = value;
                        break;
                     case TYPE_NONE:
                        break;
                  }
               }
               /*
               else if (closestBorder1 >= 0) {
                  printf("ERROR: Only matched one border for node %d: %s\n",
                         j, nodeName);
               }
               else {
                  printf("ERROR: Unable to match borders for node %d: %s\n",
                         j, nodeName);
               }
               */

            }   // matched node type
         }   // node name not "???"
      }   // s.num_points
   }  // for k = 1 to 6

   //
   // Get access to surface's Areal Estimation File
   //
   if (topographyFile->getNumberOfColumns() <= 0) {
      topographyFile->setNumberOfNodesAndColumns(numNodes, 1);
      topographyFileColumn = topographyFile->getNumberOfColumns() - 1;
   }
   else {
      if ((topographyFileColumn < 0) || 
          (topographyFileColumn >= topographyFile->getNumberOfColumns())) {
         topographyFile->addColumns(1);
         topographyFileColumn = topographyFile->getNumberOfColumns() - 1;
      }
   }
   topographyFile->setColumnName(topographyFileColumn, topographyFileColumnName);
   
   for (int i = 0; i < numNodes; i++) {
      if (topoValid[i]) {
         NodeTopography nt(topoEmean[i], topoElow[i], topoEhigh[i],
                           topoPmean[i], topoPlow[i], topoPhigh[i], 
                           topoAreaName[i]);
         topographyFile->setNodeTopography(i, topographyFileColumn, nt);
      }
   }

/*   
   float em[2], el[2], eh[2], pm[2], pl[2], ph[2];
   s.topographyFile.getMinMaxTopography(em, el, eh, pm, pl, ph);
   printf("Eccentricity Mean Minimum: %f\n", em[0]);
   printf("Eccentricity Mean Maximum: %f\n", em[1]);
   printf("Eccentricity Low Minimum:  %f\n", el[0]);
   printf("Eccentricity Low Maximum:  %f\n", el[1]);
   printf("Eccentricity High Minimum: %f\n", eh[0]);
   printf("Eccentricity High Maximum: %f\n", eh[1]);
   printf("Polar Angle Mean Minimum:  %f\n", pm[0]);
   printf("Polar Angle Mean Maximum:  %f\n", pm[1]);
   printf("Polar Angle Low Minimum:   %f\n", pl[0]);
   printf("Polar Angle Low Maximum:   %f\n", pl[1]);
   printf("Polar Angle High Minimum:  %f\n", ph[0]);
   printf("Polar Angle High Maximum:  %f\n", ph[1]);
*/
   
   delete[] topoAreaName;
   delete[] topoValid;
   delete[] topoEmean;
   delete[] topoElow;
   delete[] topoEhigh;
   delete[] topoPmean;
   delete[] topoPlow;
   delete[] topoPhigh;
   
   delete[] borderType;
   delete[] borderAreaName;
   delete[] borderAreaNumber;
}

/**
 * Extract data from border names like "V1.Emean.0" where "V1" is the area,
 * "Emean" is the type, and "0" is the border id.
 */
void
BorderToTopographyConverter::getBorderAreaAndType(
                     const QString borderName, QRegExp& regExpr,
                     QString& areaNameOut, TOPOGRAPHY_TYPES& topographyTypeOut,
                     int& areaNumberOut)
{
   topographyTypeOut = TYPE_NONE;
      
   if (regExpr.indexIn(borderName) < 0) {
      return;
   }
   const int numMatches = regExpr.numCaptures();
   if (numMatches < 4) {
      return;
   }
      
   const QString areaName(regExpr.cap(1));
   areaNameOut = areaName;
    
   const QString typeName(regExpr.cap(2));

   if (typeName.indexOf("Emean") != -1) {
      topographyTypeOut = TYPE_EMEAN;
   }
   else if (typeName.indexOf("Elow") != -1) {
      topographyTypeOut = TYPE_ELOW;
   }
   else if (typeName.indexOf("Ehigh") != -1) {
      topographyTypeOut = TYPE_EHIGH;
   }
   else if (typeName.indexOf("Pmean") != -1) {
      topographyTypeOut = TYPE_PMEAN;
   }
   else if (typeName.indexOf("Plow") != -1) {
      topographyTypeOut = TYPE_PLOW;
   }
   else if (typeName.indexOf("Phigh") != -1) {
      topographyTypeOut = TYPE_PHIGH;
   }
   
   const QString areaNumberChars(regExpr.cap(3));
   areaNumberOut = areaNumberChars.toInt();
}

/**
 * Extract the data from the node's name which comes from paint/atlas file.
 * Looking for names like "V1.Emean_0_1" where "V1" is the area, "Emean" is
 * the type, and "0" and "1" are the border ids.  The matching borders are
 * "V1.Emean.0" and "V1.Emean.1".
 */
void
BorderToTopographyConverter::getNodeAreaTypeAndBorders(
                      const QString& nodeName, QRegExp& regExpr,
                      QString& areaNameOut, TOPOGRAPHY_TYPES& topographyTypeOut,
                      int& areaNumber1Out, int& areaNumber2Out)
{
   topographyTypeOut = TYPE_NONE;
   
   if (regExpr.indexIn(nodeName) < 0) {
      return;
   }

   if (regExpr.numCaptures() < 4) {
      return;
   }
   
   areaNameOut = regExpr.cap(1);
   
   const QString typeName(regExpr.cap(2));
   if (typeName.indexOf("Emean") != -1) {
      topographyTypeOut = TYPE_EMEAN;
   }
   else if (typeName.indexOf("Elow") != -1) {
      topographyTypeOut = TYPE_ELOW;
   }
   else if (typeName.indexOf("Ehigh") != -1) {
      topographyTypeOut = TYPE_EHIGH;
   }
   else if (typeName.indexOf("Pmean") != -1) {
      topographyTypeOut = TYPE_PMEAN;
   }
   else if (typeName.indexOf("Plow") != -1) {
      topographyTypeOut = TYPE_PLOW;
   }
   else if (typeName.indexOf("Phigh") != -1) {
      topographyTypeOut = TYPE_PHIGH;
   }
   
   areaNumber1Out = regExpr.cap(3).toInt();

   areaNumber2Out = regExpr.cap(4).toInt();
}

/**
 * Find the distance to the link that is closest to the nodes' position
 */
float
BorderToTopographyConverter::getClosestBorderPointDistance(const Border* b, 
                                                           const float nodeXYZ[3]) const
{
   float minDist = std::numeric_limits<float>::max();
   const int numLinks = b->getNumberOfLinks();
   for (int i = 0; i < numLinks; i++) {
      const float* linkPos = b->getLinkXYZ(i);
      const float dx = linkPos[0] - nodeXYZ[0];
      const float dy = linkPos[1] - nodeXYZ[1];
      const float dz = linkPos[2] - nodeXYZ[2];
      const float dist = dx*dx + dy*dy + dz*dz;
      if (dist < minDist) minDist = dist;
   }
   return std::sqrt(minDist);
}

/**
 * Interpolate the topography values between two points.
 */
float
BorderToTopographyConverter::interpolateBorderTopoValues(
                            const float nearDist, const float farDist,
                            const float nearValue, const float farValue) const
{
   const float totalDist = nearDist + farDist;
   const float interp = nearValue * (1.0 - (nearDist / totalDist))
                      + farValue  * (nearDist / totalDist);
   return interp;
}




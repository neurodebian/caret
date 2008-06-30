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

#include <QGlobalStatic>  // needed for Q_OS_WIN32
#ifdef Q_OS_WIN32     // required for M_PI in <cmath>
#define _USE_MATH_DEFINES
#define NOMINMAX
#endif

#include <algorithm>
#include <iostream>
#include <limits>
#include <set>

#include "vtkMath.h"
#include "vtkPolygon.h"

#include "AreaColorFile.h"
#include "BorderColorFile.h"
#include "BorderFile.h"
#include "BorderProjectionFile.h"
#include "BrainModelBorderSet.h"
#include "BrainModelSurfacePointProjector.h"
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "ColorFile.h"
#include "DebugControl.h"
#include "MathUtilities.h"
#include "StringUtilities.h"
#include "TransformationMatrixFile.h"

/*************************************************************************************/
/**
 ** BrainModelBorderSet  methods
 **/
/*************************************************************************************/

/**
 * Constructor.
 */
BrainModelBorderSet::BrainModelBorderSet(BrainSet* bs)
{
   brainSet = bs;
}

/**
 * Destructor.
 */
BrainModelBorderSet::~BrainModelBorderSet()
{
   deleteAllBorders();
}

/**
 * set default file names if they are empty.
 */
void 
BrainModelBorderSet::setDefaultFileNames()
{
   BrainModelBorderFileInfo* bpi = getBorderProjectionFileInfo();
   if (bpi->getFileName().isEmpty()) {
      BorderProjectionFile bpf;
      bpi->setFileName(bpf.getFileName());
   }
   
   for (int i = BrainModelSurface::SURFACE_TYPE_RAW; 
        i <= BrainModelSurface::SURFACE_TYPE_UNSPECIFIED;
        i++) {
      const BrainModelSurface::SURFACE_TYPES surfaceType =
                    static_cast<BrainModelSurface::SURFACE_TYPES>(i);
      BrainModelBorderFileInfo* bfi = getBorderFileInfo(surfaceType);
      if (bfi != NULL) {
         if (bfi->getFileName().isEmpty()) {
            BorderFile bf;
            bfi->setFileName(bf.getFileName());
         }
      }
   }
}

/**
 * add a brain model.
 */
void 
BrainModelBorderSet::addBrainModel(const BrainModel* bm)
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      BrainModelBorder* b = getBorder(i);
      b->addBrainModel(bm);
   }
}

/**
 * delete a brain model
 */
void 
BrainModelBorderSet::deleteBrainModel(const BrainModel* bm)
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      BrainModelBorder* b = getBorder(i);
      b->deleteBrainModel(bm);
   }
}
      
/**
 * Assign colors to the borders
 */
void
BrainModelBorderSet::assignColors()
{
   const AreaColorFile* acf = brainSet->getAreaColorFile();
   const BorderColorFile* bcf = brainSet->getBorderColorFile();
   
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      BrainModelBorder* b = getBorder(i);      
      bool match;
      b->setAreaColorFileIndex(acf->getColorIndexByName(b->getName(), match));
      b->setBorderColorFileIndex(bcf->getColorIndexByName(b->getName(), match));
   }
   volumeBorders.assignColors(*bcf);
}

/**
 * clear border highlighting.
 */
void 
BrainModelBorderSet::clearBorderHighlighting()
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      BrainModelBorder* b = getBorder(i);
      b->setHighlightFlag(false);
   }
}
      
/**
 * delete all borders (clears this data).
 */
void 
BrainModelBorderSet::deleteAllBorders()
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      delete borders[i];
   }
   borders.clear();
   
   fileInfoProjection.clear();
   fileInfoRaw.clear();
   fileInfoFiducial.clear();
   fileInfoInflated.clear();
   fileInfoVeryInflated.clear();
   fileInfoSpherical.clear();
   fileInfoEllipsoidal.clear();
   fileInfoCompMedWall.clear();
   fileInfoFlat.clear();
   fileInfoLobarFlat.clear();
   fileInfoHull.clear();
   fileInfoUnknown.clear();
             
   volumeBorders.clear();

   SpecFile* sf = brainSet->getLoadedFilesSpecFile();
   sf->rawBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->fiducialBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->inflatedBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->veryInflatedBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->sphericalBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->ellipsoidBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->compressedBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->flatBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->lobarFlatBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->unknownBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->hullBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->volumeBorderFile.setAllSelections(SpecFile::SPEC_FALSE);
   sf->borderProjectionFile.setAllSelections(SpecFile::SPEC_FALSE);
   
   setDefaultFileNames();
}

/**
 * get the border file info.
 */
BrainModelBorderFileInfo* 
BrainModelBorderSet::getBorderFileInfo(const BrainModelSurface::SURFACE_TYPES st) 
{
   switch (st) {  
      case BrainModelSurface::SURFACE_TYPE_RAW:
         return &fileInfoRaw;
         break;
      case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
         return &fileInfoFiducial;
         break;
      case BrainModelSurface::SURFACE_TYPE_INFLATED:
         return &fileInfoInflated;
         break;
      case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
         return &fileInfoVeryInflated;
         break;
      case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
         return &fileInfoSpherical;
         break;
      case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
         return &fileInfoEllipsoidal;
         break;
      case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
         return &fileInfoCompMedWall;
         break;
      case BrainModelSurface::SURFACE_TYPE_FLAT:
         return &fileInfoFlat;
         break;
      case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
         return &fileInfoLobarFlat;
         break;
      case BrainModelSurface::SURFACE_TYPE_HULL:
         return &fileInfoHull;
         break;
      case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
      case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
         return &fileInfoUnknown;
         break;
   }
   return NULL;
}

/**
 * get the border file info (const method).
 */
const BrainModelBorderFileInfo* 
BrainModelBorderSet::getBorderFileInfo(const BrainModelSurface::SURFACE_TYPES st) const
{
   switch (st) {  
      case BrainModelSurface::SURFACE_TYPE_RAW:
         return &fileInfoRaw;
         break;
      case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
         return &fileInfoFiducial;
         break;
      case BrainModelSurface::SURFACE_TYPE_INFLATED:
         return &fileInfoInflated;
         break;
      case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
         return &fileInfoVeryInflated;
         break;
      case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
         return &fileInfoSpherical;
         break;
      case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
         return &fileInfoEllipsoidal;
         break;
      case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
         return &fileInfoCompMedWall;
         break;
      case BrainModelSurface::SURFACE_TYPE_FLAT:
         return &fileInfoFlat;
         break;
      case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
         return &fileInfoLobarFlat;
         break;
      case BrainModelSurface::SURFACE_TYPE_HULL:
         return &fileInfoHull;
         break;
      case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
      case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
         return &fileInfoUnknown;
         break;
   }
   return NULL;
}

/**
 * set the border file info.
 */
void 
BrainModelBorderSet::setBorderFileInfo(const BrainModelSurface::SURFACE_TYPES st,
                                       const BrainModelBorderFileInfo& fileInfo)
{
   switch (st) {  
      case BrainModelSurface::SURFACE_TYPE_RAW:
         fileInfoRaw = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_FIDUCIAL:
         fileInfoFiducial = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_INFLATED:
         fileInfoInflated = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_VERY_INFLATED:
         fileInfoVeryInflated = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_SPHERICAL:
         fileInfoSpherical = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_ELLIPSOIDAL:
         fileInfoEllipsoidal = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_COMPRESSED_MEDIAL_WALL:
         fileInfoCompMedWall = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_FLAT:
         fileInfoFlat = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_FLAT_LOBAR:
         fileInfoLobarFlat = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_HULL:
         fileInfoHull = fileInfo;
         break;
      case BrainModelSurface::SURFACE_TYPE_UNKNOWN:
      case BrainModelSurface::SURFACE_TYPE_UNSPECIFIED:
         fileInfoUnknown = fileInfo;
         break;
   }
}

/**
 * Delete a border.
 */
void
BrainModelBorderSet::deleteBorder(const int borderIndex)
{
   if ((borderIndex >= 0) && (borderIndex < getNumberOfBorders())) {
      delete borders[borderIndex];
      borders.erase(borders.begin() + borderIndex, borders.begin() + borderIndex + 1);
   }
   setProjectionsModified(true);
}

/**
 * Delete a border link
 */
void
BrainModelBorderSet::deleteBorderLink(const int borderIndex, const int linkIndex)
{
   if ((borderIndex >= 0) && (borderIndex < getNumberOfBorders())) {
      borders[borderIndex]->deleteBorderLink(linkIndex);
   }   
   setProjectionsModified(true);
}

/**
 * delete all projection borders.
 */
void 
BrainModelBorderSet::deleteBorderProjections()
{
   std::vector<int> bordersToDelete;
   
   const int numBorders = getNumberOfBorders();
   for (int i = 0;  i < numBorders; i++) {
      if (getBorder(i)->getType() == BrainModelBorder::BORDER_TYPE_PROJECTION) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Will delete border " << i << std::endl;
         }
         bordersToDelete.push_back(i);
      }
   }
   for (int i = (static_cast<int>(bordersToDelete.size()) - 1); i >= 0; i--) {
      deleteBorder(bordersToDelete[i]);
   }

   fileInfoProjection.clear();  

   SpecFile* sf = brainSet->getLoadedFilesSpecFile();
   sf->borderProjectionFile.setAllSelections(SpecFile::SPEC_FALSE);
}
      
/**
 * update a border with a new segment.
 */
void 
BrainModelBorderSet::updateBorder(const BrainModelSurface* bms,
                                  const UPDATE_BORDER_MODE updateMode,
                                  Border* newBorderSegment,
                                  const float samplingDensity,
                                  const bool projectBorderFlag,
                                  QString& errorMessageOut)
{
   const int brainModelIndex = bms->getBrainModelIndex();
   if (brainModelIndex <= 0) {
      return;
   }
   const int numberOfBrainModels = brainSet->getNumberOfBrainModels();
   
   const int numberOfLinksInNewSegment = newBorderSegment->getNumberOfLinks();
   if (numberOfLinksInNewSegment <= 1) {
      return;
   }
   
   //
   // Links where new border segment is inserted
   //
   BrainModelBorder* bmb = NULL;
   int borderNumber = -1;
   int startLinkNumber = -1;
   int endLinkNumber   = -1;
   bool reverseOrderFlag = false;
   
   //
   // New segment must be this close to an existing border link
   //
   const float distanceTolerance = 25.0;
   
   //
   // Start and end based upon update mode
   //
   const bool eraseModeFlag = (updateMode == UPDATE_BORDER_MODE_ERASE);
   switch (updateMode) {
      case UPDATE_BORDER_MODE_NONE:
         errorMessageOut = "Update MODE is invalid.";
         return;
         break;
      case UPDATE_BORDER_MODE_REPLACE_SEGMENT_IN_MIDDLE_OF_BORDER:
      case UPDATE_BORDER_MODE_ERASE:
         {
            //
            // Find border link nearest start of updated links
            //
            if (findBorderAndLinkNearestCoordinate(bms,
                                                   newBorderSegment->getLinkXYZ(0),
                                                   borderNumber,
                                                   startLinkNumber) == false) {
               errorMessageOut = "Unable to find border near start of new links.";
               return;
            }
            
            //
            // Find the link nearest the end of updated links
            //
            bmb = getBorder(borderNumber);
            endLinkNumber = bmb->getLinkNearestCoordinate(brainModelIndex,
                                              newBorderSegment->getLinkXYZ(numberOfLinksInNewSegment - 1));
            if (endLinkNumber < 0) {
               errorMessageOut = "Unable to find border near end of new links.";
               return;
            }
            
            QString newEraseString("New");
            if (eraseModeFlag) {
               newEraseString = "Erase";
            }
            
            //
            // Make sure new segment starts and ends near new border
            //
            const float* borderStartXYZ = 
               bmb->getBorderLink(startLinkNumber)->getLinkPosition(brainModelIndex);
            const float distToStart = 
               MathUtilities::distance3D(borderStartXYZ, 
                                         newBorderSegment->getLinkXYZ(0));
            if (distToStart > distanceTolerance) {
               errorMessageOut = newEraseString 
                               + " segment does not start close to a border.";
               return;
            }
            const float* borderEndXYZ = 
               bmb->getBorderLink(endLinkNumber)->getLinkPosition(brainModelIndex);
            const float distToEnd = 
               MathUtilities::distance3D(borderEndXYZ, 
                                         newBorderSegment->getLinkXYZ(numberOfLinksInNewSegment - 1));
            if (distToEnd > distanceTolerance) {
               errorMessageOut = newEraseString
                               + " segment does not end close to a border.";
               return;
            }

            //
            // If needed, reverse links
            //
            if (startLinkNumber > endLinkNumber) {
               reverseOrderFlag = true;
               std::swap(startLinkNumber, endLinkNumber);
            }
         }
         break;
      case UPDATE_BORDER_MODE_EXTEND_BORDER_FROM_END:
         {
            //
            // Find border link nearest start of updated links
            //
            int nearestLinkNumber = -1;
            if (findBorderAndLinkNearestCoordinate(bms,
                                                   newBorderSegment->getLinkXYZ(0),
                                                   borderNumber,
                                                   nearestLinkNumber) == false) {
               errorMessageOut = "Unable to find border near start of new links.";
               return;
            }
            bmb = getBorder(borderNumber);
           
            //
            // Get distance from both ends of existing border to first
            // link in new segment
            //
            const float* borderStartXYZ = 
               bmb->getBorderLink(0)->getLinkPosition(brainModelIndex);
            const float distToStart = 
               MathUtilities::distance3D(borderStartXYZ, 
                                                newBorderSegment->getLinkXYZ(0));
            const float* borderEndXYZ = 
               bmb->getBorderLink(bmb->getNumberOfBorderLinks() - 1)->getLinkPosition(brainModelIndex);
            const float distToEnd = 
               MathUtilities::distance3D(borderEndXYZ, 
                                                newBorderSegment->getLinkXYZ(0));
            
            //
            // Add on to start or ending end of border
            //
            if (distToStart < distToEnd) {
               if (distToStart > distanceTolerance) {
                  errorMessageOut = "New segment is not close to a border.";
                  return;
               }
               endLinkNumber = nearestLinkNumber;
               reverseOrderFlag = true;
            }
            else {
               if (distToEnd > distanceTolerance) {
                  errorMessageOut = "New segment is not close to a border.";
                  return;
               }
               startLinkNumber = nearestLinkNumber;
            }
         }
         break;
   }
   
   
   //
   // Create a new border
   //
   BrainModelBorder* newBorder = new BrainModelBorder(*bmb);
   newBorder->borderLinks.clear();
   
   //
   // Add in links prior to updated links
   //
   if (startLinkNumber >= 0) {
      for (int i = 0; i < (startLinkNumber - 1); i++) {
         BrainModelBorderLink newLink(*(bmb->getBorderLink(i)));
         newBorder->addBorderLink(newLink);
      }
   }
   
   //
   // If not erasing
   //
   if (eraseModeFlag == false) {
      //
      // Add new links
      //
      if (reverseOrderFlag) {
         for (int i = (numberOfLinksInNewSegment - 1); i >= 0; i--) {
            const float* xyz = newBorderSegment->getLinkXYZ(i);
            BrainModelBorderLink newLink(numberOfBrainModels);
            newLink.setLinkPosition(brainModelIndex, xyz);
            newBorder->addBorderLink(newLink);
         }
      }
      else {
         for (int i = 0; i < numberOfLinksInNewSegment; i++) {
            const float* xyz = newBorderSegment->getLinkXYZ(i);
            BrainModelBorderLink newLink(numberOfBrainModels);
            newLink.setLinkPosition(brainModelIndex, xyz);
            newBorder->addBorderLink(newLink);
         }
      }
   }
   
   //
   // Add in links after updated links
   //
   if (endLinkNumber >= 0) {
      for (int i = (endLinkNumber + 1); i < bmb->getNumberOfBorderLinks(); i++) {
         BrainModelBorderLink newLink(*(bmb->getBorderLink(i)));
         newBorder->addBorderLink(newLink);
      }
   }
   
   //
   // Delete the old border
   //
   borders.erase(borders.begin() + borderNumber);
   
   //
   // Resample
   //
   int dummyInt;
   newBorder->resampleToDensity(bms,
                                samplingDensity,
                                2,
                                dummyInt);
   
   //
   // Add the new border
   //
   addBorder(newBorder);
   
   if (projectBorderFlag) {
      const int lastBorderNumber = getNumberOfBorders() - 1;
      projectBorders(bms,
                     true,
                     lastBorderNumber,
                     lastBorderNumber);
   }
}

/**
 * find border and links nearest 3D coordinate (returns true if found).
 */
bool 
BrainModelBorderSet::findBorderAndLinkNearestCoordinate(const BrainModelSurface* bms,
                                        const float xyz[3],
                                        int& borderNumberOut,
                                        int& borderLinkOut) const
{    
   borderNumberOut = -1;
   borderLinkOut = -1;
   float maxDist = std::numeric_limits<int>::max();
   
   const int brainModelIndex = bms->getBrainModelIndex();
   if (brainModelIndex <= 0) {
      return false;
   }

   const int numBorders = getNumberOfBorders();
   for (int i = 0;  i < numBorders; i++) {
      const BrainModelBorder* bmb = getBorder(i);
      const int nearestLink = bmb->getLinkNearestCoordinate(brainModelIndex,
                                                            xyz);
      if (nearestLink >= 0) {
         const BrainModelBorderLink* bmbl = bmb->getBorderLink(nearestLink);
         const float* pos = bmbl->getLinkPosition(brainModelIndex);
         if (pos != NULL) {
            const float dist = MathUtilities::distanceSquared3D(pos, xyz);
            if (dist < maxDist) {
               borderNumberOut = i;
               borderLinkOut   = nearestLink;
               maxDist = dist;
            }
         }
      }
   }
   
   return (borderNumberOut >= 0);
}

/**
 * Delete borders using any of the names.
 */
void
BrainModelBorderSet::deleteBordersWithNames(const std::vector<QString>& names)
{
   std::vector<int> bordersToDelete;
   
   const int numBorders = getNumberOfBorders();
   for (int i = 0;  i < numBorders; i++) {
      const QString name(getBorder(i)->getName());
      if (std::find(names.begin(), names.end(), name) != names.end()) {
         if (DebugControl::getDebugOn()) {
            std::cout << "Will delete border " << i << " with name "
                      << name.toAscii().constData() << std::endl;
         }
         bordersToDelete.push_back(i);
      }
   }
   for (int i = (static_cast<int>(bordersToDelete.size()) - 1); i >= 0; i--) {
      deleteBorder(bordersToDelete[i]);
   }
   
   bordersToDelete.clear();
   for (int i = 0; i < volumeBorders.getNumberOfBorders(); i++) {
      const QString name = volumeBorders.getBorder(i)->getName();
      if (std::find(names.begin(), names.end(), name) != names.end()) {
         bordersToDelete.push_back(i);
      }
   }
   volumeBorders.removeBordersWithIndices(bordersToDelete);
}

/**
 * get a border.
 */
BrainModelBorder* 
BrainModelBorderSet::getBorder(const int index)
{
   if (index < getNumberOfBorders()) {
      return borders[index];
   }
   return NULL;
}

/**
 * get a border (const method).
 */
const BrainModelBorder* 
BrainModelBorderSet::getBorder(const int index) const
{
   if (index < getNumberOfBorders()) {
      return borders[index];
   }
   return NULL;
}

/**
 * copy the borders from border file.
 */
void 
BrainModelBorderSet::copyBordersFromBorderFile(const BorderFile* borderFile,
                                               const BrainModelSurface::SURFACE_TYPES surfaceType)
{
   const int numBorders = borderFile->getNumberOfBorders();
   if (numBorders > 0) {
      for (int i = 0; i < numBorders; i++) {
         BrainModelBorder* border = new BrainModelBorder(brainSet,
                                                         borderFile->getBorder(i),
                                                         surfaceType);
         if (border->getNumberOfBorderLinks() > 0) {
            addBorder(border);
         }
         else {
            delete border;
         }
      }
      
      BrainModelBorderFileInfo* bfi = getBorderFileInfo(surfaceType);
      if (bfi != NULL) {
         bfi->loadFromBorderFile(*borderFile);
      }
   }
}
                     
/**
 * copy the borders from border file.
 */
void 
BrainModelBorderSet::copyBordersFromBorderFile(const BrainModelSurface* bms,
                                               const BorderFile* borderFile)
{
   const int numBorders = borderFile->getNumberOfBorders();
   if (numBorders > 0) {
      for (int i = 0; i < numBorders; i++) {
         BrainModelBorder* border = new BrainModelBorder(brainSet,
                                                         bms,
                                                         borderFile->getBorder(i));
         if (border->getNumberOfBorderLinks() > 0) {
            addBorder(border);
         }
         else {
            delete border;
         }
      }
      
      BrainModelBorderFileInfo* bfi = getBorderFileInfo(bms->getSurfaceType());
      if (bfi != NULL) {
         bfi->loadFromBorderFile(*borderFile);
      }
   }
}
                     
/**
 * copy the borders from a border projection file.
 */
void 
BrainModelBorderSet::copyBordersFromBorderProjectionFile(BorderProjectionFile* borderProjFile)
{
   const int numBorders = borderProjFile->getNumberOfBorderProjections();
   if (numBorders > 0) {
      for (int i = 0; i < numBorders; i++) {
         BrainModelBorder* border = new BrainModelBorder(brainSet,
                                                      borderProjFile->getBorderProjection(i));
         if (border->getNumberOfBorderLinks() > 0) {
            addBorder(border);
         }
         else {
            delete border;
         }
      }
      
      BrainModelBorderFileInfo* bfi = getBorderProjectionFileInfo();
      if (bfi != NULL) {
         bfi->loadFromBorderFile(*borderProjFile);
      }
   }
}

/**
 * copy borders to a border projection file.
 */
void 
BrainModelBorderSet::copyBordersToBorderFile(const BrainModelSurface* bms,
                                             BorderFile& borderFile) const
{
   //
   // Clear the border file
   //
   borderFile.clear();
   
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   //const float center[3] = { 0.0, 0.0, 0.0 };
   
   //
   // Check each border
   //
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      Border* border = getBorder(i)->copyToBorderFileBorder(bms);
      if (border->getNumberOfLinks() > 0) {
         borderFile.addBorder(*border);
      }
   }
   
   const BrainModelBorderFileInfo* bfi = getBorderFileInfo(bms->getSurfaceType());
   if (bfi != NULL) {
      bfi->loadIntoBorderFile(borderFile);
   }
}

/**
 * copy borders that are of the specified type.
 */
BorderFile*
BrainModelBorderSet::copyBordersOfSpecifiedType(const BrainModelSurface::SURFACE_TYPES surfaceType) const
{
   BorderFile* borderFile = new BorderFile;
   const int numBrainModels = brainSet->getNumberOfBrainModels();
   if (numBrainModels <= 0) {
      return NULL;
   }
   
   //
   // Find surfaces that are of the specified type
   //
   std::vector<bool> matchingSurfaceFlag(numBrainModels, false);
   for (int i = 0; i < numBrainModels; i++) {
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms->getSurfaceType() == surfaceType) {
            matchingSurfaceFlag[i] = true;
         }
      }
   }
   
   //
   // Check each border
   //
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      BrainModelSurface* surface = NULL;
      const BrainModelBorder* b = getBorder(i);
      bool useBorder = false;
      for (int i = 0; i < numBrainModels; i++) {
         if (matchingSurfaceFlag[i]) {
            if (b->getValidForBrainModel(i)) {
               useBorder = true;
               surface = brainSet->getBrainModelSurface(i);
               break;
            }
         }
      }
      
      //
      // Convert from BrainModelBorderSet border to BorderFile border
      //
      if (useBorder) {
         Border* border = b->copyToBorderFileBorder(surface);
         if (border->getNumberOfLinks() > 0) {
            borderFile->addBorder(*border);
         }
         delete border;
      }
   }
   
   const BrainModelBorderFileInfo* bfi = getBorderFileInfo(surfaceType);
   if (bfi != NULL) {
      bfi->loadIntoBorderFile(*borderFile);
   }
   
   return borderFile;
}                                 
                              
/**
 * copy borders to a border projection file.
 */
void 
BrainModelBorderSet::copyBordersToBorderProjectionFile(BorderProjectionFile& borderProjFile) const
{
   //
   // Clear the border projectionfile
   //
   borderProjFile.clear();

   const float center[3] = { 0.0, 0.0, 0.0 };
   
   //
   // Check each border
   //
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      //
      // Is border projected
      //
      const BrainModelBorder* b = getBorder(i);
      if (b->getType() == BrainModelBorder::BORDER_TYPE_PROJECTION) {
         //
         // Create the border for the border file
         //
         BorderProjection borderProj(b->getName(),
                                     center,
                                     b->getSamplingDensity(),
                                     b->getVariance(),
                                     b->getTopography(),
                                     b->getArealUncertainty());
            
         //
         // Copy the links to the border file's border
         //
         const int numLinks = b->getNumberOfBorderLinks();
         for (int j = 0; j < numLinks; j++) {
            const BrainModelBorderLink* link = b->getBorderLink(j);
            
            int vertices[3];
            float areas[3];
            link->getProjection(vertices, areas);
            BorderProjectionLink bpl(link->getSection(), vertices, areas, link->getRadius());
                                     
            borderProj.addBorderProjectionLink(bpl);
         }
      
         //
         // Add the border to the border file
         //
         if (borderProj.getNumberOfLinks() > 0) {
            borderProjFile.addBorderProjection(borderProj);
         }
      }
   }
   
   const BrainModelBorderFileInfo* bfi = getBorderProjectionFileInfo();
   if (bfi != NULL) {
      bfi->loadIntoBorderFile(borderProjFile);
   }
}                              

/**
 * add a border.
 */
void 
BrainModelBorderSet::addBorder(BrainModelBorder* border)
{
   borders.push_back(border);
   for (int i = 0; i < brainSet->getNumberOfBrainModels(); i++) {
      if (border->getValidForBrainModel(i)) {
         border->setModified(i, true);
      }
   }
}      
   
/**
 * Unproject the borders for a surface.
 */
void
BrainModelBorderSet::unprojectBorders(const BrainModelSurface* bms,
                                      const int firstBorderToProject,
                                      const int lastBorderToProject)
{
   //
   // Start & stop border indices
   //
   const int numBorders = getNumberOfBorders();
   int startIndex = 0;
   int endIndex = numBorders;   
   if (firstBorderToProject >= 0) {
      startIndex = firstBorderToProject;
      if (startIndex > numBorders) {
         startIndex = numBorders;
      }
   }
   if (lastBorderToProject >= 0) {
      endIndex = lastBorderToProject + 1;
      if (endIndex > numBorders) {
         endIndex = numBorders;
      }
   }
   
   for (int i = startIndex; i < endIndex; i++) {
      borders[i]->unprojectLinks(bms);
   }
}

/**
 * Unproject the borders for all surfaces.
 */
void
BrainModelBorderSet::unprojectBordersForAllSurfaces(const int firstBorderToProject,
                                                    const int lastBorderToProject)
{
   const int num = brainSet->getNumberOfBrainModels();
   for (int i = 0; i < num; i++) {
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
      if (bms != NULL) {
         unprojectBorders(bms,
                          firstBorderToProject,
                          lastBorderToProject);
      }
   }
   setAllModifiedStatus(false);
}

/**
 * Project the borders for a surface.
 */
void
BrainModelBorderSet::projectBorders(const BrainModelSurface* bms,
                                    const bool barycentricMode,
                                    const int firstBorderToProject,
                                    const int lastBorderToProject)
{
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   //
   // Create a point projector for projecting the borders
   //
   BrainModelSurfacePointProjector pointProjector(bms,
                           BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                           false);

   //
   // Keeps track of borders for keeping
   //
   const int numBorders = getNumberOfBorders();
   std::vector<bool> bordersToKeep(numBorders, true);
   
   //
   // Start & stop border indices
   //
   int startIndex = 0;
   int endIndex = numBorders;   
   if (firstBorderToProject >= 0) {
      startIndex = firstBorderToProject;
      if (startIndex > numBorders) {
         startIndex = numBorders;
      }
   }
   if (lastBorderToProject >= 0) {
      endIndex = lastBorderToProject + 1;
      if (endIndex > numBorders) {
         endIndex = numBorders;
      }
   }
   
   for (int i = startIndex; i < endIndex; i++) {
      //
      // Border is now a projected border
      //
      BrainModelBorder* b = getBorder(i);

      //
      // Only keep borders that are valid for this brain model
      //
      const bool validBorder = b->getValidForBrainModel(brainModelIndex);
      bordersToKeep[i] = validBorder;
      
      //
      // If this border should be projected
      //
      if (validBorder) {
      
         //
         // Border is now a projected border
         //
         b->setType(BrainModelBorder::BORDER_TYPE_PROJECTION);
      
         //
         // Keep track of which links are successfully projected
         //
         std::vector<bool> linksToKeep;
         
         //
         // Project each link
         //
         const int numLinks = b->getNumberOfBorderLinks();
         for (int j = 0; j < numLinks; j++) {
            BrainModelBorderLink* link = b->getBorderLink(j);
            
            //
            // Position of link for this surface
            //
            float xyz[3];
            link->getLinkPosition(brainModelIndex, xyz);

            //
            // Project the link for this surface
            //
            int vertices[3];
            float areas[3];
            bool validPoint = false;

            //
            // Points at origin were not successfully projected so ignore them
            //
            if ((xyz[0] != 0.0) || (xyz[1] != 0.0) || (xyz[2] != 0.0)) {
               if (barycentricMode) {
                  int nearestNode = -1;
                  const int tileNumber =
                     pointProjector.projectBarycentric(xyz, nearestNode, vertices, areas);
                  if ((nearestNode >= 0) && (tileNumber >= 0)) {
                     validPoint = true;
                  }
               }
               else {
                  const int nearestNode = pointProjector.projectToNearestNode(xyz);
                  if (nearestNode >= 0) {
                     vertices[0] = nearestNode;
                     vertices[1] = nearestNode;
                     vertices[2] = nearestNode;
                     areas[0] = 1.0;
                     areas[1] = 0.0;
                     areas[2] = 0.0;
                     validPoint = true;
                  }
               }
            }
            
            //
            //  keep track of valid projected links
            //
            linksToKeep.push_back(validPoint);
            
            //
            // Set the projection information for this link
            //
            if (validPoint) {
               link->setProjection(vertices, areas);
            }
         }
         
         //
         // Delete links that were not projected successfully
         //
         for (int j = (numLinks - 1); j >= 0; j--) {
            if (linksToKeep[j] == false) {
               b->deleteBorderLink(j);
            }
         }
      }
   }
   
   //
   // Delete borders that were not valid for this surface
   //
   for (int i = (numBorders - 1); i >= 0; i--) {
      if (bordersToKeep[i] == false) {
         deleteBorder(i);
      }
   }
   
   //
   // Unproject all borders
   //
   unprojectBordersForAllSurfaces(firstBorderToProject,
                                  lastBorderToProject);
   
   //
   // Set all projected borders modified
   //
   setProjectionsModified(true);

   //
   // Set projection file info using an empty file
   //
   BrainModelBorderFileInfo* bfi = getBorderProjectionFileInfo();
   if (bfi == NULL) {
      BorderProjectionFile bpf;
      bfi->loadFromBorderFile(bpf);
   }
   else {
      if (bfi->getFileName().isEmpty()) {
         bfi->clear();
         BorderProjectionFile bpf;
         bfi->loadFromBorderFile(bpf);
      }
   }
}

/**
 * Orient the borders displayed on this surface clockwise.
 */
void
BrainModelBorderSet::orientDisplayedBordersClockwise(const BrainModel* bm)
{
   const BrainModelSurface* bms = dynamic_cast<const BrainModelSurface*>(bm);
   if (bms != NULL) {
      const int numBorders = getNumberOfBorders();
      for (int i = 0; i < numBorders; i++) {
         BrainModelBorder* b = getBorder(i);
         if (b->getDisplayFlag()) {
            b->orientLinksClockwise(bms);
         }          
      }
   }
   else if (dynamic_cast<const BrainModelVolume*>(bm) != NULL) {
      volumeBorders.orientDisplayedBordersClockwise();
   }
}

/**
 * Reverse the borders displayed on this surface.
 */
void
BrainModelBorderSet::reverseDisplayedBorders(const BrainModel* bm)
{
   if (dynamic_cast<const BrainModelSurface*>(bm) != NULL) {
      const int numBorders = getNumberOfBorders();
      for (int i = 0; i < numBorders; i++) {
         BrainModelBorder* b = getBorder(i);
         if (b->getDisplayFlag()) {
            b->reverseLinks();
         }          
      }
   }
   else if (dynamic_cast<const BrainModelVolume*>(bm) != NULL) {
      volumeBorders.reverseDisplayedBorders();
   }
}

/**
 * Compute the flat normals for all border.
 */
void
BrainModelBorderSet::computeFlatNormals(const BrainModel* bm)
{
   const BrainModelSurface* bms = dynamic_cast<const BrainModelSurface*>(bm);
   if (bms != NULL) {
      const int numBorders = getNumberOfBorders();
      for (int i = 0; i < numBorders; i++) {
         getBorder(i)->computeFlatNormals(bms);
      }
   }
   else if (dynamic_cast<const BrainModelVolume*>(bm) != NULL) {
      volumeBorders.computeFlatNormals();
   }
}

/**
 * interpolate two borders to create new borders (input borders will be resampled too).
 */
void 
BrainModelBorderSet::createInterpolatedBorders(const BrainModelSurface* bms,
                                               const int border1Index,
                                               const int border2Index,
                                               const QString& namePrefix,
                                               const int numberOfNewBorders,
                                               const float sampling,
                                               QString& errorMessageOut)
{
   errorMessageOut = "";
   const int numBorders = getNumberOfBorders();
   if ((border1Index < 0) || (border1Index >= numBorders)) {
      errorMessageOut = "Border 1 index is invalid.";
      return;
   }
   if ((border2Index < 0) || (border2Index >= numBorders)) {
      errorMessageOut = "Border 2 index is invalid.";
      return;
   }
   if (bms == NULL) {
      errorMessageOut = "Surface is invalid.";
      return;
   }
   
   //
   // Get the input brain model borders
   //
   BrainModelBorder* brainModelBorder1 = getBorder(border1Index);
   BrainModelBorder* brainModelBorder2 = getBorder(border2Index);
   
   //
   // Convert to border file borders
   //
   Border* border1 = brainModelBorder1->copyToBorderFileBorder(bms);
   Border* border2 = brainModelBorder2->copyToBorderFileBorder(bms);
   
   std::vector<Border*> newBorders;
   try {
      //
      // User border file to create interpolated borders
      //
      Border::createInterpolatedBorders(border1,
                                        border2,
                                        namePrefix,
                                        numberOfNewBorders,
                                        sampling,
                                        newBorders);
                                        
      //
      // Convert all back to brain model borders
      //
      BrainModelBorder b1(brainSet, border1, bms->getSurfaceType());
      brainModelBorder1->initialize(brainSet);
      *brainModelBorder1 = b1;
      BrainModelBorder b2(brainSet, border2, bms->getSurfaceType());
      brainModelBorder2->initialize(brainSet);
      *brainModelBorder2 = b2;
      
      for (unsigned int i = 0; i < newBorders.size(); i++) {
         BrainModelBorder* border = new BrainModelBorder(brainSet,
                                                         newBorders[i],
                                                         bms->getSurfaceType());
         addBorder(border);
      }
   }
   catch (FileException& e) {
      errorMessageOut = e.whatQString();
   }
   
   //
   // Clean up
   //
   delete border1;
   delete border2;
   for (unsigned int i = 0; i < newBorders.size(); i++) {
      delete newBorders[i];
   }
}
                                     
/**
 * Resample displayed borders for the surface to the specified density.
 */
void
BrainModelBorderSet::resampleDisplayedBorders(const BrainModel* bm,
                                              const float density)
{
   const BrainModelSurface* bms = dynamic_cast<const BrainModelSurface*>(bm);
   if (bms != NULL) {
      const int numBorders = getNumberOfBorders();
      for (int i = 0;  i < numBorders; i++) {
         BrainModelBorder* border = getBorder(i);
         if (border->getDisplayFlag()) {
            int dummy = 0;
            border->resampleToDensity(bms, density, 2, dummy);
         }
      }
   }
   else if (dynamic_cast<const BrainModelVolume*>(bm) != NULL) {
      volumeBorders.resampleDisplayedBorders(density);
   }
}

/**
 * get indeces of borders with name.
 */
void 
BrainModelBorderSet::getAllBordersWithName(const QString& nameIn,
                                           std::vector<int>& indicesOut) const
{
   indicesOut.clear();
   
   const int numBorders = getNumberOfBorders();
   for (int i = 0;  i < numBorders; i++) {
      if (getBorder(i)->getName() == nameIn) {
         indicesOut.push_back(i);
      }
   }
}
                           
/**
 * copy a border.
 */
void 
BrainModelBorderSet::copyBorder(const int borderToCopyIndex,
                                const QString& nameForCopiedBorder)
{
   BrainModelBorder* b = new BrainModelBorder(*getBorder(borderToCopyIndex));
   b->setName(nameForCopiedBorder);
   addBorder(b);
}

/**
 * get the unique names of all borders.
 */
void 
BrainModelBorderSet::getAllBorderNames(std::vector<QString>& names,
                                       const bool reverseOrderFlag)
{
   std::set<QString> uniqueNames;
   const int numBorders = getNumberOfBorders();
   for (int i = 0;  i < numBorders; i++) {
      uniqueNames.insert(getBorder(i)->getName());
   }
   const int numVolumeBorders = volumeBorders.getNumberOfBorders();
   for (int i = 0; i < numVolumeBorders; i++) {
      uniqueNames.insert(volumeBorders.getBorder(i)->getName());
   }
   names.clear();
   names.insert(names.end(), uniqueNames.begin(), uniqueNames.end());
   
   StringUtilities::sortCaseInsensitive(names, reverseOrderFlag, false);
}

/** 
 * See if a surface's borders are modified.
 */
bool
BrainModelBorderSet::getSurfaceBordersModified(const BrainModelSurface* bms) const
{
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return false;
   }
   
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      if (getBorder(i)->getModified(brainModelIndex)) {
         return true;
      }
   }
   return false;
}

/** 
 * Set a surface's borders as modified.
 */
void
BrainModelBorderSet::setSurfaceBordersModified(const BrainModelSurface* bms,
                                               const bool mod) 
{
   if (bms == NULL) {
      return;  // must be a volume
   }
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      getBorder(i)->setModified(brainModelIndex, mod);
   }
}

/**
 * Set the modification status of all borders and the projections
 */
void
BrainModelBorderSet::setAllModifiedStatus(const bool mod)
{
   const int num = brainSet->getNumberOfBrainModels();
   for (int i = 0; i < num; i++) {
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
      setSurfaceBordersModified(bms, false);
   }
   setProjectionsModified(mod);
}

/**
 * Set display flag for borders with specified name.
 */
void 
BrainModelBorderSet::setNameDisplayFlagForBordersWithName(const QString& name,
                                                          const bool flag)
{
   const int num = getNumberOfBorders();
   for (int i = 0; i < num; i++) {
      BrainModelBorder* b = getBorder(i);
      if (b->getName() == name) {
         b->setNameDisplayFlag(flag);
      }
   }
   const int numVolumeBorders = volumeBorders.getNumberOfBorders();
   for (int i = 0; i < numVolumeBorders; i++) {
      Border* b = volumeBorders.getBorder(i);
      if (b->getName() == name) {
         b->setNameDisplayFlag(flag);
      }
   }
}

/**
 * See if any projections are modified.
 */
bool
BrainModelBorderSet::getProjectionsModified() const
{
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      if (getBorder(i)->getProjectionModified()) {
         return true;
      }
   }
   return false;
}

/**
 * Set projections modified.
 */
void
BrainModelBorderSet::setProjectionsModified(const bool mod)
{
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      getBorder(i)->setProjectionModified(mod);
   }
}

/**
 * apply a transformation matrix to a brain model's borders.
 */
void 
BrainModelBorderSet::applyTransformationMatrix(const BrainModelSurface* bms,
                                               const TransformationMatrix& tm)
{
   if (bms == NULL) {
      return;  // must be a volume
   }
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   const int numBorders = getNumberOfBorders();
   for (int i = 0; i < numBorders; i++) {
      getBorder(i)->applyTransformationMatrix(brainModelIndex, tm);
   }
}
                                     
/**
 * copy the volume borders to the fiducial borders.
 */
void 
BrainModelBorderSet::copyVolumeBordersToFiducialBorders()
{
   copyBordersFromBorderFile(&volumeBorders, BrainModelSurface::SURFACE_TYPE_FIDUCIAL);
}      

/*************************************************************************************/
/**
 ** BrainModelBorderFileInfo  methods
 **/
/*************************************************************************************/

/**
 * Get the comment.
 */
QString
BrainModelBorderFileInfo::getFileComment() const
{
   const QString headerTagComment("comment");
   const std::map<QString, QString>::const_iterator iter = 
         fileHeader.find(headerTagComment);
   if (iter != fileHeader.end()) {
      return iter->second;
   }
   return "";
}

/**
 * Set the comment.
 */
void
BrainModelBorderFileInfo::setFileComment(const QString& comm)
{
   fileHeader["comment"] = comm;
}

/**
 * Set the contents to a border file's values.
 */
void
BrainModelBorderFileInfo::loadFromBorderFile(const AbstractFile& af)
{
   fileName   = af.getFileName();
   fileTitle  = af.getFileTitle();
   fileHeader = af.getHeader();
   pubMedID   = af.getFilePubMedID();
}

/**
 * Load the contents into a border file's values.
 */
void
BrainModelBorderFileInfo::loadIntoBorderFile(AbstractFile& af) const
{
   af.setFileName(fileName);
   af.setFileTitle(fileTitle);
   af.setHeader(fileHeader);
   af.setFilePubMedID(pubMedID);
}

/**
 * Clear the contents.
 */
void
BrainModelBorderFileInfo::clear()
{
   fileName = "";
   fileTitle = "";
   fileHeader.clear();
   pubMedID = "";
}

/*************************************************************************************/
/**
 ** BrainModelBorder  methods
 **/
/*************************************************************************************/

/**
 * Constructor.
 */
BrainModelBorder::BrainModelBorder(BrainSet* bs,
                                   const QString& nameIn, const BORDER_TYPE typeIn,
                                   const BrainModelSurface::SURFACE_TYPES borderFileSurfaceTypeIn)
{
   initialize(bs);
   name = nameIn;
   borderType = typeIn;
   borderFileSurfaceType = borderFileSurfaceTypeIn;
}

/**
 * Copy constructor.
 */
BrainModelBorder::BrainModelBorder(const BrainModelBorder& bmb)
{
   brainSet = bmb.brainSet;
   borderType = bmb.borderType;
   borderFileSurfaceType = bmb.borderFileSurfaceType;
   name = bmb.name;
   samplingDensity = bmb.samplingDensity;
   variance = bmb.variance;
   topographyValue = bmb.topographyValue;
   arealUncertainty = bmb.arealUncertainty;
   borderFileColorIndex = bmb.borderFileColorIndex;
   areaColorFileIndex = bmb.areaColorFileIndex;
   brainModelValidity = bmb.brainModelValidity;
   brainModelModified = bmb.brainModelModified;
   projectionModified = bmb.projectionModified;
   borderLinks = bmb.borderLinks;
   displayFlag = bmb.displayFlag;
   highlightFlag = bmb.highlightFlag;
   nameDisplayFlag = bmb.nameDisplayFlag;
   for (int i = 0; i < getNumberOfBorderLinks(); i++) {
      BrainModelBorderLink* bmbl = getBorderLink(i);
      bmbl->setBrainModelBorder(this);
   }
}      

/**
 * assignment
 */
void
BrainModelBorder::operator=(const BrainModelBorder& bmb)
{
   brainSet = bmb.brainSet;
   borderType = bmb.borderType;
   borderFileSurfaceType = bmb.borderFileSurfaceType;
   name = bmb.name;
   samplingDensity = bmb.samplingDensity;
   variance = bmb.variance;
   topographyValue = bmb.topographyValue;
   arealUncertainty = bmb.arealUncertainty;
   borderFileColorIndex = bmb.borderFileColorIndex;
   areaColorFileIndex = bmb.areaColorFileIndex;
   brainModelValidity = bmb.brainModelValidity;
   brainModelModified = bmb.brainModelModified;
   projectionModified = bmb.projectionModified;
   borderLinks = bmb.borderLinks;
   displayFlag = bmb.displayFlag;
   nameDisplayFlag = bmb.nameDisplayFlag;
   highlightFlag = bmb.highlightFlag;
   for (int i = 0; i < getNumberOfBorderLinks(); i++) {
      BrainModelBorderLink* bmbl = getBorderLink(i);
      bmbl->setBrainModelBorder(this);
   }
}

/**
 * Constructor for a border from a border projection file
 */
BrainModelBorder::BrainModelBorder(BrainSet* bs,
                                   BorderProjection* bp)
{
   initialize(bs);
   float center[3];
   bp->getData(name, center, samplingDensity, variance, topographyValue, arealUncertainty);
   borderType = BORDER_TYPE_PROJECTION;
   
   //
   // Determine which brain models this border is valid for
   //
   const int numBrainModels = brainSet->getNumberOfBrainModels();
   for (int i = 0; i < numBrainModels; i++) {
      brainModelValidity[i] = false;      
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
      if (bms != NULL) {
         brainModelValidity[i] = true;
         setModified(i, true);
      }
   }
   
   //
   // Copy the links for each brain model
   //
   const int numLinks = bp->getNumberOfLinks();
   for (int j = 0; j < numLinks; j++) {
      BrainModelBorderLink bmbl(numBrainModels);      
      const BorderProjectionLink* bpl = bp->getBorderProjectionLink(j);
      int section, vertices[3];
      float areas[3];
      float radius;
      bpl->getData(section, vertices, areas, radius);
      bmbl.setSection(section);
      bmbl.setRadius(radius);
      bmbl.setProjection(vertices, areas);
      addBorderLink(bmbl);
   } 
   
   for (int i = 0; i < numBrainModels; i++) {
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
      if (bms != NULL) {
         unprojectLinks(bms);
      }
   }
}

/**
 * Constructor for a border from a border file.
 */
BrainModelBorder::BrainModelBorder(BrainSet* bs,
                                   const Border* b,
                                   const BrainModelSurface::SURFACE_TYPES surfaceTypeIn)
{
   initialize(bs);
   name = b->getName();
   borderType = BORDER_TYPE_NORMAL;
   borderFileSurfaceType = surfaceTypeIn;
   samplingDensity = b->getSamplingDensity();
   variance = b->getVariance();
   topographyValue = b->getTopographyValue();
   arealUncertainty = b->getArealUncertainty();
   
   //
   // Determine which brain models this border is valid for
   //
   const int numBrainModels = brainSet->getNumberOfBrainModels();
   for (int i = 0; i < numBrainModels; i++) {
      brainModelValidity[i] = false;      
      const BrainModelSurface* bms = brainSet->getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms->getSurfaceType() == borderFileSurfaceType) {
            brainModelValidity[i] = true;
            setModified(i, true);
         }
      }
   }
   
   //
   // Copy the links for each brain model
   //
   const int numLinks = b->getNumberOfLinks();
   for (int j = 0; j < numLinks; j++) {
      BrainModelBorderLink bmbl(numBrainModels);
      bmbl.setSection(b->getLinkSectionNumber(j));
      bmbl.setRadius(b->getLinkRadius(j));
      float xyz[3];
      b->getLinkXYZ(j, xyz);
      bmbl.setLinkFilePosition(xyz);
      for (int i = 0; i < numBrainModels; i++) {
         bmbl.setLinkPosition(i, xyz);
      }
      addBorderLink(bmbl);
   }
}         

/**
 * Constructor for a border from a border file.
 */
BrainModelBorder::BrainModelBorder(BrainSet* bs,
                                   const BrainModelSurface* bms,
                                   const Border* b)
{
   initialize(bs);
   name = b->getName();
   borderType = BORDER_TYPE_NORMAL;
   borderFileSurfaceType = bms->getSurfaceType();
   samplingDensity = b->getSamplingDensity();
   variance = b->getVariance();
   topographyValue = b->getTopographyValue();
   arealUncertainty = b->getArealUncertainty();
   
   //
   // Determine which brain models this border is valid for
   //
   const int numBrainModels = brainSet->getNumberOfBrainModels();
   for (int i = 0; i < numBrainModels; i++) {
      brainModelValidity[i] = false;      
      if (bms == brainSet->getBrainModelSurface(i)) {
         brainModelValidity[i] = true;
         setModified(i, true);
      }
   }
   
   //
   // Copy the links for each brain model
   //
   const int numLinks = b->getNumberOfLinks();
   for (int j = 0; j < numLinks; j++) {
      BrainModelBorderLink bmbl(numBrainModels);
      bmbl.setSection(b->getLinkSectionNumber(j));
      float xyz[3];
      b->getLinkXYZ(j, xyz);
      bmbl.setLinkFilePosition(xyz);
      for (int i = 0; i < numBrainModels; i++) {
         bmbl.setLinkPosition(i, xyz);
      }
      addBorderLink(bmbl);
   }
}         

/**
 * Destructor.
 */
BrainModelBorder::~BrainModelBorder()
{
}

/**
 * Add a border link to this border
 */
void
BrainModelBorder::addBorderLink(const BrainModelBorderLink& bmbl)
{
   borderLinks.push_back(bmbl);
   BrainModelBorderLink* link = getBorderLink(getNumberOfBorderLinks() - 1);
   link->setBrainModelBorder(this);
}

/**
 * Copy the border to a border file border (user should destroy returned Border*)
 */
Border*
BrainModelBorder::copyToBorderFileBorder(const BrainModelSurface* bms) const
{
   //
   // Create the border for the border file
   //
   const float center[3] = { 0.0, 0.0, 0.0 };
   Border* border = new Border(getName(),
                               center,
                               getSamplingDensity(),
                               getVariance(),
                               getTopography(),
                               getArealUncertainty());
   
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return border;
   }
   
   border->setBorderColorIndex(borderFileColorIndex);
   border->setDisplayFlag(displayFlag);
   
   if (getValidForBrainModel(brainModelIndex)) {
      //
      // Copy the links to the border file's border
      //
      const int numLinks = getNumberOfBorderLinks();
      for (int j = 0; j < numLinks; j++) {
         const BrainModelBorderLink* link = getBorderLink(j);
         border->addBorderLink(link->getLinkPosition(brainModelIndex),
                               link->getSection(), link->getRadius());
      }
   }
   
   return border;
}

/**
 * Update the border for the addition of a new brain model.
 */
void 
BrainModelBorder::addBrainModel(const BrainModel* bm)
{
   const BrainModelSurface* bms = dynamic_cast<const BrainModelSurface*>(bm);
   bool matchedSurfaceType = false;
   const CoordinateFile* cf = ((bms != NULL) ? bms->getCoordinateFile() : NULL);
   if (bms != NULL) {
      if (bms->getSurfaceType() == borderFileSurfaceType) {
         matchedSurfaceType = true;
      }
   }

   bool projectionFlag = false;
   switch (borderType) {
      case BORDER_TYPE_UNKNOWN:
         break;
      case BORDER_TYPE_NORMAL:
         break;
      case BORDER_TYPE_PROJECTION:
         if (bms != NULL) {
            projectionFlag = true;
         }
         break;
   }
   
   //
   // Border valid for this brain model
   //
   const bool valid = projectionFlag || matchedSurfaceType;
   brainModelValidity.push_back(valid);
   brainModelModified.push_back(false);
   
   //
   // Get the brain model index
   //
   const int brainModelIndex = brainSet->getBrainModelIndex(bm);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: invalid brain model index at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   //
   // Set link position for this border
   //
   if (valid) {
      const int num = getNumberOfBorderLinks();
      for (int i = 0; i < num; i++) {
         BrainModelBorderLink* bmbl = getBorderLink(i);
         float xyz[3];
         bmbl->getLinkFilePosition(xyz);
         bmbl->addBrainModel(xyz);
      
         if (projectionFlag) {
            bmbl->unprojectLink(cf, brainModelIndex);
         }
      }      
   }
}

/**
 * Update the border for the deletion of a brain model.
 */
void 
BrainModelBorder::deleteBrainModel(const BrainModel* bm)
{
   //
   // Get the brain model index
   //
   const int brainModelIndex = brainSet->getBrainModelIndex(bm);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: invalid brain model index at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   const int num = getNumberOfBorderLinks();
   for (int i = 0; i < num; i++) {
      BrainModelBorderLink* bmbl = getBorderLink(i);
      bmbl->deleteBrainModel(brainModelIndex);
   }
   brainModelValidity.erase(brainModelValidity.begin() + brainModelIndex,
                            brainModelValidity.begin() + brainModelIndex + 1);
   brainModelModified.erase(brainModelModified.begin() + brainModelIndex,
                            brainModelModified.begin() + brainModelIndex + 1);
}

/**
 * Delete a border link
 */
void
BrainModelBorder::deleteBorderLink(const int linkNumber)
{
   if ((linkNumber >= 0) && (linkNumber < getNumberOfBorderLinks())) {
      borderLinks.erase(borderLinks.begin() + linkNumber,
                        borderLinks.begin() + linkNumber + 1);
   }
}

/**
 * Initialize variables in a BrainModelBorder
 */
void
BrainModelBorder::initialize(BrainSet* bs)
{
   brainSet = bs;
   name = "";
   samplingDensity = 0.0;
   variance = 0.0;
   topographyValue = 0.0;
   arealUncertainty = 0.0;
   borderType = BORDER_TYPE_UNKNOWN;
   borderFileSurfaceType = BrainModelSurface::SURFACE_TYPE_UNKNOWN;
   borderFileColorIndex = -1;
   areaColorFileIndex = -1;
   highlightFlag = false;
   
   const int numBrainModels = brainSet->getNumberOfBrainModels();
   brainModelValidity.resize(numBrainModels);
   std::fill(brainModelValidity.begin(), brainModelValidity.end(), false);
   brainModelModified.resize(numBrainModels);
   std::fill(brainModelModified.begin(), brainModelModified.end(), false);
   projectionModified = false;
   displayFlag = true;
   nameDisplayFlag = true;
}

/**
 * Orient the links in the border clockwise assuming the borders are flat.
 */
void
BrainModelBorder::orientLinksClockwise(const BrainModelSurface* bms)
{
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   //
   // Is this border valid for this surface
   //
   if (getValidForBrainModel(brainModelIndex) == false) {
      return;
   }
   
   //
   // Find the link with the minimum X value
   //
   float minX = std::numeric_limits<float>::max();
   int minIndexX = -1;
   const int numLinks = getNumberOfBorderLinks();
   for (int i = 0; i < numLinks; i++) {
      const float* xyz = getBorderLink(i)->getLinkPosition(brainModelIndex);
      if (xyz[0] < minX) {
         minIndexX = i;
         minX      = xyz[0];
      }
   }
   
   if (minIndexX >= 0) {
      int iNext = minIndexX + 1;
      if (iNext >= numLinks) {
         iNext = 0;
      }
      const float* nextPos = getBorderLink(iNext)->getLinkPosition(brainModelIndex);
      const float* minPos  = getBorderLink(minIndexX)->getLinkPosition(brainModelIndex);
      
      //
      // If next link is below the one with the minimum X then the border must be counter-clockwise
      //
      if (nextPos[1] < minPos[1]) {
         reverseLinks();
      }
   }
}

/**
 * Reverse the links in the border.
 */
void
BrainModelBorder::reverseLinks()
{
   std::reverse(borderLinks.begin(), borderLinks.end());
}

/**
 * Compute the flat normals for a border.
 */
void
BrainModelBorder::computeFlatNormals(const BrainModelSurface* bms)
{
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   //
   // Is this border valid for this surface
   //
   if (getValidForBrainModel(brainModelIndex) == false) {
      return;
   }

   //
   // Loop through the links
   //
   const int numLinks = getNumberOfBorderLinks();
   for (int i = 0; i < numLinks; i++) {  
      int iNext = i + 1;
      if (iNext >= numLinks) iNext = 0;
      int iPrev = i - 1;
      if (iPrev < 0) iPrev = numLinks - 1;

      //
      // Calculate the normal
      //
      float vec[3];
      MathUtilities::subtractVectors(getBorderLink(iNext)->getLinkPosition(brainModelIndex), 
                                     getBorderLink(iPrev)->getLinkPosition(brainModelIndex), 
                                     vec);
      MathUtilities::normalize(vec);
      const float normal[3] = { vec[1], -vec[0], 0.0 };
      getBorderLink(i)->setFlatNormal(normal);
   }
   
}

/**
 * resample the border to the specified number of links.
 */
void 
BrainModelBorder::resampleToNumberOfLinks(const BrainModelSurface* bms,
                                          const int newNumberOfLinks)
{
   if (getNumberOfBorderLinks() == newNumberOfLinks) {
      return;
   }
   
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   //
   // Is this border valid for this surface
   //
   if (getValidForBrainModel(brainModelIndex) == false) {
      return;
   }

   Border* border = copyToBorderFileBorder(bms);
   if (border->getNumberOfLinks() > 0) {
      border->resampleBorderToNumberOfLinks(newNumberOfLinks);
      
      BrainModelBorder b(brainSet, border, bms->getSurfaceType());
      const int numBefore = getNumberOfBorderLinks();
      initialize(brainSet);
      *this = b;
      if (DebugControl::getDebugOn()) {
         std::cout << "Border named " << getName().toAscii().constData() << " has "
                   << getNumberOfBorderLinks() << " after resampling. "
                   << "Had " << numBefore << " links before." << std::endl;
      }
   }
   
   delete border;
}
      
/**
 * Resample the border to the specified density.
 */
void
BrainModelBorder::resampleToDensity(const BrainModelSurface* bms,
                                    const float density,
                                    const int minimumNumberOfLinks,
                                    int& newNumberOfLinks)
{
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   //
   // Is this border valid for this surface
   //
   if (getValidForBrainModel(brainModelIndex) == false) {
      return;
   }

   Border* border = copyToBorderFileBorder(bms);
   if (border->getNumberOfLinks() > 0) {
      border->resampleBorderToDensity(density, minimumNumberOfLinks, newNumberOfLinks);
      
      BrainModelBorder b(brainSet, border, bms->getSurfaceType());
      const int numBefore = getNumberOfBorderLinks();
      initialize(brainSet);
      *this = b;
      if (DebugControl::getDebugOn()) {
         std::cout << "Border named " << getName().toAscii().constData() << " has "
                   << getNumberOfBorderLinks() << " after resampling. "
                   << "Had " << numBefore << " links before." << std::endl;
      }
   }
   
   delete border;
}

/**
 * Set the type of a border.
 */
void
BrainModelBorder::setType(const BORDER_TYPE bt)
{
   borderType = bt;
}

/**
 * Unproject the border links for a surface
 */
void
BrainModelBorder::unprojectLinks(const BrainModelSurface* surface)
{
   const int brainModelIndex = brainSet->getBrainModelIndex(surface);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   if (borderType != BORDER_TYPE_PROJECTION) {
      return;
   }

   const CoordinateFile* cf = surface->getCoordinateFile();
   
   //
   // Border is valid for the surface
   //
   brainModelValidity[brainModelIndex] = true;
   
   const int num = getNumberOfBorderLinks();
   for (int i = 0; i < num; i++) {
      BrainModelBorderLink* link = getBorderLink(i);
      link->unprojectLink(cf, brainModelIndex);

/*
      int vertices[3];
      float areas[3];
      link->getProjection(vertices, areas);
      
      float xyz[3] = { 0.0, 0.0, 0.0 };
      if (vertices[0] >= 0) {
         const float totalArea = areas[0] + areas[1] + areas[2];
         if (totalArea > 0.0) {
            const float* v1 = cf->getCoordinate(vertices[0]);
            const float* v2 = cf->getCoordinate(vertices[1]);
            const float* v3 = cf->getCoordinate(vertices[2]);
      
            xyz[0] = (v1[0] * areas[1] + v2[0] * areas[2] + v3[0] * areas[0])
                           / totalArea;
            xyz[1] = (v1[1] * areas[1] + v2[1] * areas[2] + v3[1] * areas[0])
                           / totalArea;
            xyz[2] = (v1[2] * areas[1] + v2[2] * areas[2] + v3[2] * areas[0])
                           / totalArea;
         }
         else {
            cf->getCoordinate(vertices[0], xyz);
         }
      }
      
      link->setLinkPosition(brainModelIndex, xyz);
*/
   }
}

/**
 * set the name of the border.
 */
void 
BrainModelBorder::setName(const QString& s)
{
   name = s;
}

/**
 * set the sampling density.
 */
void 
BrainModelBorder::setSamplingDensity(const float density)
{
   samplingDensity = density;
}

/**
 * set the variance.
 */
void 
BrainModelBorder::setVariance(const float var)
{
   variance = var;
}

/**
 * set the topography value.
 */
void 
BrainModelBorder::setTopography(const float topography)
{
    topographyValue = topography;
}

/**
 * set the areal uncertainty.
 */
void 
BrainModelBorder::setArealUncertainty(const float uncertainty)
{
   arealUncertainty = uncertainty;
}

/**
 * get the border valid for a brain model.
 */
bool 
BrainModelBorder::getValidForBrainModel(const int brainModelIndex) const
{
   if (brainModelIndex < static_cast<int>(brainModelValidity.size())) {
      return brainModelValidity[brainModelIndex];
   }
   return false;
}
      
/**
 * get a border link.
 */
BrainModelBorderLink* 
BrainModelBorder::getBorderLink(const int index)
{
   if (index < getNumberOfBorderLinks()) {
      return &borderLinks[index];
   }
   return NULL;
}

/**
 * get a border link (const method).
 */
const BrainModelBorderLink* 
BrainModelBorder::getBorderLink(const int index) const
{
   if (index < getNumberOfBorderLinks()) {
      return &borderLinks[index];
   }
   return NULL;
}      

/**
 * Determine the bounds of a border for a surface.
 */
void
BrainModelBorder::getBounds(const BrainModelSurface* bms, double bounds[6]) const
{
   float floatBounds[6];
   getBounds(bms, floatBounds);
   bounds[0] = floatBounds[0];
   bounds[1] = floatBounds[1];
   bounds[2] = floatBounds[2];
   bounds[3] = floatBounds[3];
   bounds[4] = floatBounds[4];
   bounds[5] = floatBounds[5];
}

/**
 * Determine the bounds of a border for a surface.
 */
void
BrainModelBorder::getBounds(const BrainModelSurface* bms, float bounds[6]) const
{
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   bounds[0] =  std::numeric_limits<float>::max();
   bounds[1] = -std::numeric_limits<float>::max();
   bounds[2] =  std::numeric_limits<float>::max();
   bounds[3] = -std::numeric_limits<float>::max();
   bounds[4] =  std::numeric_limits<float>::max();
   bounds[5] = -std::numeric_limits<float>::max();
   
   //
   // Is this border valid for this surface
   //
   if (getValidForBrainModel(brainModelIndex) == false) {
      return;
   }

   const int numLinks = getNumberOfBorderLinks();
   for (int i = 0; i < numLinks; i++) {
      const float* pos = getBorderLink(i)->getLinkPosition(brainModelIndex);
      bounds[0] = std::min(bounds[0], pos[0]);
      bounds[1] = std::max(bounds[1], pos[0]);
      bounds[2] = std::min(bounds[2], pos[1]);
      bounds[3] = std::max(bounds[3], pos[1]);
      bounds[4] = std::min(bounds[4], pos[2]);
      bounds[5] = std::max(bounds[5], pos[2]);
   }   
}

/**
 * Determine if points are inside this border.
 * The border is assumed to be flat, in the X-Y plane.
 */
void
BrainModelBorder::pointsInsideBorder(const BrainModelSurface* bms,
                                     const float* points, const int numPoints,
                                     std::vector<bool>& insideFlags,
                                     const bool checkNonNegativeZPointsOnly) const
{
   if (static_cast<int>(insideFlags.size()) < numPoints) {
      insideFlags.resize(numPoints);
   }
   std::fill(insideFlags.begin(), insideFlags.end(), false);
   
   const int brainModelIndex = brainSet->getBrainModelIndex(bms);
   if (brainModelIndex < 0) {
      std::cout << "PROGRAM ERROR: Cannot find brain model at " << __LINE__
                << " in " << __FILE__ << std::endl;
      return;
   }
   
   //
   // Is this border valid for this surface
   //
   if (getValidForBrainModel(brainModelIndex) == false) {
      return;
   }
   
   //
   // Since the coordinates are typically less than 100, this can cause
   // problems with the point in polygon algorithm (it will report a
   // degenerate polygon).  Scaling all coordinates elminates this problem.
   // Using every other point in the border also helps.
   //
   const float scaleFactor = 1000.0;

   const int numLinks = getNumberOfBorderLinks();

#ifdef HAVE_VTK5      
   std::vector<double> polygon;
#else // HAVE_VTK5
   std::vector<float> polygon;
#endif // HAVE_VTK5
   int numToSkip = 1;
   int numInPolygon = 0;
   for (int i = 0; i < (numLinks - 1); i++) {
      const float* pos = getBorderLink(i)->getLinkPosition(brainModelIndex);
      polygon.push_back(pos[0] * scaleFactor);
      polygon.push_back(pos[1] * scaleFactor);
      polygon.push_back(pos[2] * scaleFactor);
      numInPolygon++;
      i += numToSkip; 
   }
   if (numInPolygon < 3) {
      return;
   }

#ifdef HAVE_VTK5      
   double normal[3] = { 0.0, 0.0, 1.0 };
   double bounds[6];
#else // HAVE_VTK5
   float normal[3] = { 0.0, 0.0, 1.0 };
   float bounds[6];
#endif // HAVE_VTK5
   getBounds(bms, bounds);
   bounds[0] *= scaleFactor;
   bounds[1] *= scaleFactor;
   bounds[2] *= scaleFactor;
   bounds[3] *= scaleFactor;
   bounds[4] = -1.0;
   bounds[5] =  1.0;
   
   
   for (int i = 0; i < numPoints; i++) {
#ifdef HAVE_VTK5   
      double xyz[3];   
#else // HAVE_VTK5
      float xyz[3];
#endif // HAVE_VTK5
      xyz[0] = points[i*3]   * scaleFactor;
      xyz[1] = points[i*3+1] * scaleFactor;
      xyz[2] = points[i*3+2] * scaleFactor;
      int result = 0;

      bool checkIt = true;
      if (checkNonNegativeZPointsOnly) {
         if (xyz[2] < 0.0) {
            checkIt = false;
         }
      }
      if (checkIt) {
#ifdef HAVE_VTK5   
         result = MathUtilities::pointInPolygon(xyz,
                                             numInPolygon,
                                             (double*)&polygon[0],
                                             bounds,
                                             normal);
#else // HAVE_VTK5
         result = MathUtilities::pointInPolygon(xyz,
                                             numInPolygon,
                                             (float*)&polygon[0],
                                             bounds,
                                             normal);
#endif // HAVE_VTK5
      }

      if (result > 0) {
         insideFlags[i] = true;
      }
      else if (result < 0)  {
         std::cerr << "Border polygon passed to MathUtilities::pointInPolygon "
                      " is degenerate." << std::endl;
         std::cerr << "Polygon: " << std::endl;
         for (int j = 0; j < numInPolygon; j++) {
            std::cerr << "   " 
                      << j << " "
                      << polygon[j*3] << " "
                      << polygon[j*3+1] << " "
                      << polygon[j*3+2] << std::endl;

         }
         break;
      }
   }
}

/**
 * set this brain model's link as modified.
 */
void 
BrainModelBorder::setModified(const int brainModelIndex, const bool mod)
{
   brainModelModified[brainModelIndex] = mod;
}

/**
 * Set this projection as modified.
 */
void 
BrainModelBorder::setProjectionModified(const bool mod)
{
   projectionModified = mod;
}      

/**
 * get this brain model's link as modified.
 */
bool 
BrainModelBorder::getModified(const int brainModelIndex) const
{
   //
   // Is this border valid for this surface
   //
   if (getValidForBrainModel(brainModelIndex) == false) {
      return false;
   }
   return brainModelModified[brainModelIndex];
}      

/**
 * get the link nearest the coordinate (returns -1 if found).
 */
int 
BrainModelBorder::getLinkNearestCoordinate(const int brainModelIndex,
                                               const float xyz[3]) const
{
   int nearestLinkNumber = -1;
   float maxDist = std::numeric_limits<int>::max();
   
   const int numLinks = getNumberOfBorderLinks();
   for (int j = 0; j < numLinks; j++) {
      const BrainModelBorderLink* bmbl = getBorderLink(j);
      const float* pos = bmbl->getLinkPosition(brainModelIndex);
      if (pos != NULL) {
         const float dist = MathUtilities::distanceSquared3D(pos, xyz);
         if (dist < maxDist) {
            nearestLinkNumber   = j;
            maxDist = dist;
         }
      }
   }
   
   return nearestLinkNumber;
}
                                    
/**
 * apply a transformation matrix to a brain model's borders.
 */
void 
BrainModelBorder::applyTransformationMatrix(const int brainModelIndex,
                                            const TransformationMatrix& tm)
{
   if (getValidForBrainModel(brainModelIndex)) {
      const int numLinks = getNumberOfBorderLinks();
      for (int i = 0; i < numLinks; i++) {
         BrainModelBorderLink* bmbl = getBorderLink(i);
         bmbl->applyTransformationMatrix(brainModelIndex, tm);
      }
   }
}
                                     
/*************************************************************************************/
/**
 ** BrainModelBorderLink  methods
 **/
/*************************************************************************************/

/**
 * Constructor.
 */
BrainModelBorderLink::BrainModelBorderLink(const int numBrainModels)
{
   areas[0]      = 0.0;
   areas[1]      = 0.0;
   areas[2]      = 0.0;
   section       = 0;
   radius        = 0.0;
   vertices[0]   = -1;
   vertices[1]   = -1;
   vertices[2]   = -1;
   flatNormal[0] = 0.0;
   flatNormal[1] = 1.0;
   flatNormal[2] = 0.0;
   fileXYZ[0]    = 0.0;
   fileXYZ[1]    = 0.0;
   fileXYZ[2]    = 0.0;
   for (int i = 0; i < numBrainModels; i++) {
      xyz.push_back(0.0);
      xyz.push_back(0.0);
      xyz.push_back(0.0);
   }
   brainModelBorder = NULL;
}

/**
 * Copy Constructor
 */
BrainModelBorderLink::BrainModelBorderLink(const BrainModelBorderLink& bmbl)
{
   section         = bmbl.section;
   radius          = bmbl.radius;
   vertices[0]     = bmbl.vertices[0];
   vertices[1]     = bmbl.vertices[1];
   vertices[2]     = bmbl.vertices[2];
   areas[0]        = bmbl.areas[0];
   areas[1]        = bmbl.areas[1];
   areas[2]        = bmbl.areas[2];
   fileXYZ[0]      = bmbl.fileXYZ[0];
   fileXYZ[1]      = bmbl.fileXYZ[1];
   fileXYZ[2]      = bmbl.fileXYZ[2];
   xyz             = bmbl.xyz;
   flatNormal[0]   = bmbl.flatNormal[0];
   flatNormal[1]   = bmbl.flatNormal[1];
   flatNormal[2]   = bmbl.flatNormal[2];
   brainModelBorder = NULL;
}      

/**
 * Destructor.
 */
BrainModelBorderLink::~BrainModelBorderLink()
{
}

/**
 * Add a brain model
 */
void
BrainModelBorderLink::addBrainModel(const float xyzIn[3])
{
   xyz.push_back(xyzIn[0]);
   xyz.push_back(xyzIn[1]);
   xyz.push_back(xyzIn[2]);
}

/**
 * Set the section number.
 */
void
BrainModelBorderLink::setSection(const int secNum)
{
   section = secNum;
}

/**
 * Set the radius.
 */
void
BrainModelBorderLink::setRadius(const float radiusIn)
{
   radius = radiusIn;
}

/**
 * get the projection information.
 */
void 
BrainModelBorderLink::getProjection(int verticesOut[3], float areasOut[3]) const
{
   verticesOut[0] = vertices[0];
   verticesOut[1] = vertices[1];
   verticesOut[2] = vertices[2];
   areasOut[0] = areas[0];
   areasOut[1] = areas[1];
   areasOut[2] = areas[2];
}

/**
 * get the projection information.
 */
void 
BrainModelBorderLink::setProjection(const int verticesIn[3], const float areasIn[3])
{
   vertices[0] = verticesIn[0];
   vertices[1] = verticesIn[1];
   vertices[2] = verticesIn[2];
   areas[0] = areasIn[0];
   areas[1] = areasIn[1];
   areas[2] = areasIn[2];
   setProjectionModified();
}

/**
 * get the link position for a brain model.
 */
const float* 
BrainModelBorderLink::getLinkPosition(const int brainModelIndex) const
{
   static const float zeros[3] = { 0.0, 0.0, 0.0 };
   
   const int index = brainModelIndex * 3;
   if (index < static_cast<int>(xyz.size())) {
      return &xyz[index];
   }
   
   return zeros;
}

/**
 * get the link position for a brain model (const method)
 */
void 
BrainModelBorderLink::getLinkPosition(const int brainModelIndex, float xyzOut[3]) const
{
   const int index = brainModelIndex * 3;
   if (index < static_cast<int>(xyz.size())) {
      xyzOut[0] = xyz[index];
      xyzOut[1] = xyz[index+1];
      xyzOut[2] = xyz[index+2];
      return;
   }
   std::cout << "PROGRAM ERROR: Invalid index in BrainModelBorderLink::getLinkPosition" 
             << std::endl;
   
   xyzOut[0] = 0.0;
   xyzOut[1] = 0.0;
   xyzOut[2] = 0.0;
}

/**
 * Set the link position for a brain model.
 */
void
BrainModelBorderLink::setLinkPosition(const int brainModelIndex, const float xyzIn[3])
{
   const int index = brainModelIndex * 3;
   if (index < static_cast<int>(xyz.size())) {
      xyz[index]   = xyzIn[0];
      xyz[index+1] = xyzIn[1];
      xyz[index+2] = xyzIn[2];
      setModified(brainModelIndex);
   }
   else {
      std::cout << "PROGRAM ERROR: Invalid index in BrainModelBorderLink::setLinkPosition" 
                << std::endl;
   }
}

/**
 * apply a transformation matrix to a brain model's borders.
 */
void 
BrainModelBorderLink::applyTransformationMatrix(const int brainModelIndex,
                                                const TransformationMatrix& tm)
{
   float xyz[3];
   getLinkPosition(brainModelIndex, xyz);
   tm.multiplyPoint(xyz);
   setLinkPosition(brainModelIndex, xyz);
}
                                     
/**
 * get the link position from the border file.
 */
void 
BrainModelBorderLink::getLinkFilePosition(float xyzOut[3]) const
{
   xyzOut[0] = fileXYZ[0];
   xyzOut[1] = fileXYZ[1];
   xyzOut[2] = fileXYZ[2];
}

/**
 * set the link position from the border file.
 */
void 
BrainModelBorderLink::setLinkFilePosition(const float xyzIn[3])
{
   fileXYZ[0] = xyzIn[0];
   fileXYZ[1] = xyzIn[1];
   fileXYZ[2] = xyzIn[2];
}
      
/** 
 * Delete a brain model from the links.
 */
void
BrainModelBorderLink::deleteBrainModel(const int brainModelIndex)
{
   const int index = brainModelIndex * 3;
   if (index < static_cast<int>(xyz.size())) {   
      xyz.erase(xyz.begin() + index, xyz.begin() + index + 3);
   }
   else {
      std::cout << "PROGRAM ERROR: Invalid index in BrainModelBorderLink::deleteBrainModel" 
                << std::endl;
   }
}

/**
 * Set the BrainModelBorder this link is part of.
 */
void
BrainModelBorderLink::setBrainModelBorder(BrainModelBorder* bmb)
{
   brainModelBorder = bmb;
}

/**
 * Unproject a border link.
 */
void
BrainModelBorderLink::unprojectLink(const CoordinateFile* cf, const int brainModelIndex)
{
   float xyz[3] = { 0.0, 0.0, 0.0 };
   if (vertices[0] >= 0) {
      const float totalArea = areas[0] + areas[1] + areas[2];
      if (totalArea > 0.0) {
         const float* v1 = cf->getCoordinate(vertices[0]);
         const float* v2 = cf->getCoordinate(vertices[1]);
         const float* v3 = cf->getCoordinate(vertices[2]);
   
         xyz[0] = (v1[0] * areas[1] + v2[0] * areas[2] + v3[0] * areas[0])
                        / totalArea;
         xyz[1] = (v1[1] * areas[1] + v2[1] * areas[2] + v3[1] * areas[0])
                        / totalArea;
         xyz[2] = (v1[2] * areas[1] + v2[2] * areas[2] + v3[2] * areas[0])
                        / totalArea;
      }
      else {
         cf->getCoordinate(vertices[0], xyz);
      }
   }
   
   setLinkPosition(brainModelIndex, xyz);
   setModified(brainModelIndex);
}

/**
 * set the flat normal.
 */
void 
BrainModelBorderLink::setFlatNormal(const float normal[3]) 
{
   flatNormal[0] = normal[0];
   flatNormal[1] = normal[1];
   flatNormal[2] = normal[2];
}

/**
 * Set the border holding this link as modified
 */
void
BrainModelBorderLink::setModified(const int brainModelIndex)
{
   if (brainModelBorder != NULL) {
      brainModelBorder->setModified(brainModelIndex, true);
   }
}

/**
 * Set the border holding this projection as modified
 */
void
BrainModelBorderLink::setProjectionModified()
{
   if (brainModelBorder != NULL) {
      brainModelBorder->setProjectionModified(true);
   }
}


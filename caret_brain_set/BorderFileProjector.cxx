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



#include <iostream>

#include <QApplication>
#include <QProgressDialog>

#include "BorderFile.h"
#include "BorderFileProjector.h"
#include "BorderProjectionFile.h"
#include "BrainModelSurfacePointProjector.h"
#include "DebugControl.h"

/**
 * Constructor.  If "barycentricModeIn" is true, the border file will be projected to
 * barycentric coordinates in tiles, otherwise, the border points will be projected to
 * the nearest nodes in the BrainModelSurface.
 */
BorderFileProjector::BorderFileProjector(const BrainModelSurface* bmsIn,
                                         const bool barycentricModeIn)
{
   barycentricMode = barycentricModeIn;
   pointProjector = new BrainModelSurfacePointProjector(bmsIn,
                                             BrainModelSurfacePointProjector::SURFACE_TYPE_HINT_OTHER,
                                             false);
}

/**
 * Destructor.
 */
BorderFileProjector::~BorderFileProjector()
{
}

/**
 * Project a border file to a border projection file using the BrainModelSurface passed to
 * the constructor.  If "progressDialogParent" is not NULL, a progress dialog will be 
 * displayed while borders are being projected.
 */
void
BorderFileProjector::projectBorderFile(const BorderFile* bf,
                                       BorderProjectionFile* bpf,
                                       QWidget* progressDialogParent)
{
   const int numBorders = bf->getNumberOfBorders();
   if (numBorders > 0) {
   
      //
      // Create a progress dialog 
      //
      QProgressDialog* progressDialog = NULL;
      if (progressDialogParent != NULL) {
         progressDialog = new QProgressDialog("Projecting Borders",
                                                0,
                                                0,
                                                numBorders + 1,
                                                progressDialogParent);
         progressDialog->setWindowTitle("Projecting Borders");
         progressDialog->setValue(0);
         progressDialog->show();
      }
      
      for (int i = 0; i < numBorders; i++) {
         const Border* b = bf->getBorder(i);
         const int numLinks = b->getNumberOfLinks();
         if (numLinks > 0) {
         
            if (progressDialog != NULL) {
               if (progressDialog->wasCanceled()) {
                  break;
               }
               progressDialog->setValue(i + 1);
               qApp->processEvents(); // note: qApp is global in QApplication
            }
            
            //
            // Transfer border attribute data from the border to the border projection
            //
            QString name;
            float center[3];
            float sampDensity;
            float variance;
            float topography;
            float arealUncertainty;
            b->getData(name, center, sampDensity, variance, topography, arealUncertainty);            
            BorderProjection bp(name, center, sampDensity, variance, topography, arealUncertainty);
            bp.setBorderColorIndex(b->getBorderColorIndex());
            
            //
            // Project the border link points to the border projection file
            //
            for (int j = 0; j < numLinks; j++) {
               float xyz[3];
               b->getLinkXYZ(j, xyz);
               
               int vertices[3];
               float areas[3];
               bool validPoint = false;
               if (barycentricMode) {
                  int nearestNode = -1;
                  const int tileNumber =
                     pointProjector->projectBarycentric(xyz, nearestNode, vertices, areas);
                  if ((nearestNode >= 0) && (tileNumber >= 0)) {
                     validPoint = true;
                  }
               }
               else {
                  const int nearestNode = pointProjector->projectToNearestNode(xyz);
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
               
               if (validPoint) {
                  BorderProjectionLink bpl(b->getLinkSectionNumber(j), vertices, areas,
                                           b->getLinkRadius(j));
                  bp.addBorderProjectionLink(bpl);
               }
               else if (DebugControl::getDebugOn()) {
                  std::cout << "INFO: Border Link (" << i << "," << j << ") in "<< name.toAscii().constData() 
                            << " does not project to a tile and has been discarded" << std::endl;
               }
            }
            
            if (bp.getNumberOfLinks() > 0) {
               bpf->addBorderProjection(bp);
            }
         }
      }
      
      //
      // Remove the progress dialog
      //
      if (progressDialog != NULL) {
         progressDialog->setValue(numBorders + 1);
         delete progressDialog;
      }
   }
}


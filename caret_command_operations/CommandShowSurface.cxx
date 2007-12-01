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

#include <QImage>

#include "BrainModelOpenGL.h"
#include "BrainModelSurface.h"
#include "BrainSet.h"
#include "CommandImageView.h"
#include "CommandShowSurface.h"
#include "FileFilters.h"
#include "OffScreenOpenGLWidget.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */ 
CommandShowSurface::CommandShowSurface()
   : CommandBase("-show-surface",
                 "SHOW SURFACE")
{
}

/**
 * destructor.
 */
CommandShowSurface::~CommandShowSurface()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandShowSurface::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   std::vector<QString> viewValues, viewNames;
   viewValues.push_back("ALL");   viewNames.push_back("All");
   viewValues.push_back("A");     viewNames.push_back("Anterior");
   viewValues.push_back("D");     viewNames.push_back("Dorsal");
   viewValues.push_back("L");     viewNames.push_back("Lateral");
   viewValues.push_back("M");     viewNames.push_back("Medial");
   viewValues.push_back("P");     viewNames.push_back("Posterior");
   viewValues.push_back("V");     viewNames.push_back("Ventral");
   
   paramsOut.clear();
   paramsOut.addFile("Coordinate File Name", 
                     FileFilters::getCoordinateGenericFileFilter());
   paramsOut.addFile("Topology File Name", 
                     FileFilters::getTopologyGenericFileFilter());
   paramsOut.addListOfItems("View", viewValues, viewNames);
   paramsOut.addInt("Image Width", 512, 0, 10000000);
   paramsOut.addInt("Image Height", 512, 0, 10000000);
   paramsOut.addVariableListOfParameters("Show Surface Options");
}

/**
 * get full help information.
 */
QString 
CommandShowSurface::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<coordinate-file>\n"
       + indent9 + "<topololgy-file>\n"
       + indent9 + "<view>\n"
       + indent9 + "<image-width>\n"
       + indent9 + "<image-height>\n"
       + indent9 + "[image-file-name]\n"
       + indent9 + "\n"
       + indent9 + "Render an image of the surface to a file or to the user's screen.\n"
       + indent9 + "\n"
       + indent9 + "      view is one of:  \n"
       + indent9 + "         A    => anterior view\n"
       + indent9 + "         ALL  => all views in one image\n"
       + indent9 + "         D    => dorsal view\n"
       + indent9 + "         L    => lateral view\n"
       + indent9 + "         M    => medial view\n"
       + indent9 + "         P    => posterior view\n"
       + indent9 + "         V    => ventral view\n" 
       + indent9 + "\n"
       + indent9 + "      NOTE: If the image file name is not specified, the image of\n"
       + indent9 + "       the surface will be shown in a window on the user's display."
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandShowSurface::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const int MAIN_WINDOW_INDEX = 0;
   
   //
   // Get the parameters
   //
   const QString coordFileName =
      parameters->getNextParameterAsString("Input Coord File");
   
   const QString topoFileName =
      parameters->getNextParameterAsString("Input Topo File");

   //
   // Get the view
   //
   QString viewName =
      parameters->getNextParameterAsString("View");
   BrainModel::STANDARD_VIEWS view;
   if (viewName == "L") {
      view = BrainModel::VIEW_LATERAL;
   }
   else if (viewName == "M") {
      view = BrainModel::VIEW_MEDIAL;
   } 
   else if (viewName == "D") {
      view = BrainModel::VIEW_DORSAL;
   } 
   else if (viewName == "V") {
      view = BrainModel::VIEW_VENTRAL;
   } 
   else if (viewName == "A") {
      view = BrainModel::VIEW_ANTERIOR;
   } 
   else if (viewName == "P") {
      view = BrainModel::VIEW_POSTERIOR;
   } 
   else if (viewName == "ALL") {
      view = BrainModel::VIEW_NONE;
   }
   else {
      throw CommandException("Invalid view \"" + viewName + "\".");
   }
   
   //
   // Get the image X size
   //
   const int imageWidth =
      parameters->getNextParameterAsInt("Image Width");
   if (imageWidth <= 0) {
      throw CommandException("Invalid image width = "
                             + QString::number(imageWidth));
   }
   
   
   //
   // Get the image Y size
   //
   const int imageHeight =
      parameters->getNextParameterAsInt("Image Height");
   if (imageHeight <= 0) {
      throw CommandException("Invalid image height = "
                             + QString::number(imageHeight));
   }

   bool saveImageToFile = false;
   QString imageFileName;
   if (parameters->getParametersAvailable()) {
      saveImageToFile = true;
      
      //
      // Image file name
      //
      imageFileName = parameters->getNextParameterAsString("Image File Name");      
   }
   
   //
   // Read the spec file into a brain set
   //
   BrainSet brainSet(topoFileName, coordFileName);
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      throw CommandException("Cannot find surface in BrainSet from "
                             + coordFileName
                             + " and " 
                             + topoFileName);
   }   
   
   //
   // setup the off screen renderer
   //
   QImage image;
   OffScreenOpenGLWidget opengl;
   
   //
   // "none" is all views in one image
   //
   if (view == BrainModel::VIEW_NONE) {
      const int xSize = imageWidth / 2;
      const int ySize = imageHeight / 3;
      opengl.setFixedSize(xSize, ySize);
      
      //
      // Render the surface into size images
      //
      QImage a, d, l, m, p, v;
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_ANTERIOR);
      opengl.drawToImage(&brainSet, bms, a);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_DORSAL);
      opengl.drawToImage(&brainSet, bms, d);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_MEDIAL);
      opengl.drawToImage(&brainSet, bms, m);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_LATERAL);
      opengl.drawToImage(&brainSet, bms, l);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_POSTERIOR);
      opengl.drawToImage(&brainSet, bms, p);
      bms->setToStandardView(MAIN_WINDOW_INDEX, BrainModel::VIEW_VENTRAL);
      opengl.drawToImage(&brainSet, bms, v);
      
      //
      // Assemble the images into one image
      //
      image = QImage(imageWidth, imageHeight, a.format());
      for (int i = 0; i < xSize; i++) {
         for (int j = 0; j < ySize; j++) {
            image.setPixel(i, j, l.pixel(i, j));
            image.setPixel(i + xSize, j, m.pixel(i, j));
            image.setPixel(i, j + ySize, d.pixel(i, j));
            image.setPixel(i + xSize, j + ySize, v.pixel(i, j));
            image.setPixel(i, j + 2*ySize, a.pixel(i, j));
            image.setPixel(i + xSize, j + 2*ySize, p.pixel(i, j));
         }
      }
      
   }
   else {
      opengl.setFixedSize(imageWidth, imageHeight);
      bms->setToStandardView(MAIN_WINDOW_INDEX, view);
   
      //
      // Render the image
      //
      opengl.drawToImage(&brainSet,
                         bms,
                         image);
   }
                      
   //
   // save the image file
   //
   if (saveImageToFile) {
      if (image.save(imageFileName, "jpg") == false) {
         throw CommandException("Unable to write image file: " + imageFileName);
      }
   }
   else {
      CommandImageView::displayQImage(image);
   }
}


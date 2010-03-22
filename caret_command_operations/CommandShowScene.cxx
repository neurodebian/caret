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
#include "BrainModelVolume.h"
#include "BrainSet.h"
#include "CommandImageView.h"
#include "CommandShowScene.h"
#include "DebugControl.h"
#include "FileFilters.h"
#include "ImageFile.h"
#include "OffScreenOpenGLWidget.h"
#include "PreferencesFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SceneFile.h"
#include "SpecFile.h"

/**
 * constructor.
 */
CommandShowScene::CommandShowScene()
   : CommandBase("-show-scene",
                 "SHOW SCENE")
{
}

/**
 * destructor.
 */
CommandShowScene::~CommandShowScene()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandShowScene::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Spec File Name",
                     FileFilters::getSpecFileFilter());
   paramsOut.addFile("Scene File Name",
                     FileFilters::getSceneFileFilter());
   paramsOut.addString("Scene Name or Number (1..N)", "1");
   paramsOut.addVariableListOfParameters("Optional parameters");
}

/**
 * get full help information.
 */
QString 
CommandShowScene::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<spec-file>\n"
       + indent9 + "<scene-file>\n"
       + indent9 + "<scene-name-or-number>\n"
       + indent9 + "[-image-file  image-file-name  images-per-row] \n"
       + indent9 + "\n"
       + indent9 + "Render an image of a scene into an image file.\n"
       + indent9 + "Note: the scene numbers start at one.\n"
       + indent9 + "\n"
       + indent9 + "If the \"scene-name-or-number\" is numeric it is interpreted\n"
       + indent9 + "to be the index of the scene (1..N) in the scene file.  \n"
       + indent9 + "Otherwise, it is interpreted to be the name of the scene.\n"
       + indent9 + "\n"
       + indent9 + "If the \"-image-file\" option is specified, the images of\n"
       + indent9 + "the main and viewing windows will be placed into an image\n"
       + indent9 + "file.  \"images-per-row\" specifies how the images (if there\n"
       + indent9 + "are viewing windows displayed in the scene) will be layed\n"
       + indent9 + "out.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandShowScene::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   int imageWidth = 512;
   int imageHeight = 512;
   
   //
   // Get the spec file name
   //
   const QString specFileName = 
      parameters->getNextParameterAsString("Spec File Name");

   //
   // Get the scene file name
   //
   const QString sceneFileName =
      parameters->getNextParameterAsString("Scene File Name");
   
   //
   // Get the scene name or number
   //
   const QString sceneNameOrNumber = 
      parameters->getNextParameterAsString("Scene Name or Number");
   
   //
   // Save image to a file ??
   //
   bool saveImageToFile = false;
   QString imageFileName;
   int imagesPerRow = 1;
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Parameter");

      //
      // Image file name
      //
      if (paramName == "-image-file") {
         imageFileName = parameters->getNextParameterAsString("Image File Name");
         imagesPerRow  = parameters->getNextParameterAsInt("Images Per Row");
         saveImageToFile = true;
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }
      
   //
   // Read in the spec file
   //
   SpecFile specFile;
   specFile.readFile(specFileName);
   
   //
   // Deselect all files in spec file and add scene file
   //
   specFile.setAllFileSelections(SpecFile::SPEC_FALSE);
   specFile.addToSpecFile(SpecFile::getSceneFileTag(),
                          sceneFileName,
                          "",
                          false);
                          
   //
   // Read the spec file into a brain set
   //
   QString errorMessage;
   BrainSet brainSet;
   if (brainSet.readSpecFile(specFile,
                             specFileName,
                             errorMessage)) {
      throw CommandException("reading spec file " 
                             + specFileName
                             + "\n"
                             + errorMessage.toAscii().constData());
   }
   
   //
   // Get the scene file
   //   
   SceneFile* sceneFile = brainSet.getSceneFile();
   const int numScenes = sceneFile->getNumberOfScenes();
   if (numScenes <= 0) {
      throw CommandException("Scene file contains no scenes.");
   }
   SceneFile::Scene* scene = NULL;
   
   //
   // Get the number of the scene
   //
   bool ok = false;
   int sceneNumber = sceneNameOrNumber.toInt(&ok);
   if (ok) {
      if ((sceneNumber < 1) ||
          (sceneNumber > numScenes)) {
         throw CommandException("Invalid scene number: " 
                                + QString::number(sceneNumber)
                                + "\n   Valid Scene Numbers range from 1 to " 
                                + QString::number(numScenes));
      }
      //
      // Users enter scene numbers 1 to N but C++ indexes 0 to N-1
      //
      sceneNumber--;   
      scene = sceneFile->getScene(sceneNumber);
   }
   else {
      //
      // get number of scene from name
      //
      scene = sceneFile->getSceneFromName(sceneNameOrNumber);
      if (scene == NULL) {
         throw CommandException("No scene named \""
                                + sceneNameOrNumber
                                + "\" was found.");
      }
   }
   
   //
   // Show number of scene
   //
   if (DebugControl::getDebugOn()) {
      std::cout << "Showing scene num=" << (sceneNumber + 1)
                <<": " << sceneFile->getScene(sceneNumber)->getName().toAscii().constData() << std::endl;
   }
   
   //
   // Setup the scene
   //
   QString sceneErrorMessage, sceneWarningMessage;
   brainSet.showScene(scene,
                      false,
                      sceneErrorMessage,
                      sceneWarningMessage);
      
       
   //
   // Contains images captured of all windows
   //
   std::vector<QImage> capturedImages;
   
   //
   // Loop through main and viewing windows
   //
   for (int windowNumber = 0; windowNumber < BrainModel::NUMBER_OF_BRAIN_MODEL_VIEW_WINDOWS; windowNumber++) {
      //
      // Get the brain model for the main window in the scene
      //
      QString modelError;
      int geometry[4];
      int glWidthWidthHeight[2];
      bool yokeFlag;
      BrainModel* brainModel = brainSet.showSceneGetBrainModel(scene,
                                                               windowNumber,
                                                               geometry,
                                                               glWidthWidthHeight,
                                                               yokeFlag,
                                                               modelError);
      //
      // Was there a brain window in "window number" window
      //
      if (brainModel == NULL) {
         continue;
      }
      if (modelError.isEmpty() == false) {
         sceneErrorMessage += modelError;
      }
      
      //
      // Copy the transforms from "windowNumber" to window number 0
      // since all image captures are performed in the "main window => 0"
      //
      brainModel->setTransformationsAsString(0, 
                  brainModel->getTransformationsAsString(windowNumber));
      BrainModelVolume* bmv = dynamic_cast<BrainModelVolume*>(brainModel);
      if (bmv != NULL) {
         int slices[3];
         bmv->getSelectedOrthogonalSlices(windowNumber, slices);
         bmv->setSelectedOrthogonalSlices(0, slices);
         bmv->setViewStereotaxicCoordinatesFlag(0,
                          bmv->getViewStereotaxicCoordinatesFlag(windowNumber));
         bmv->setObliqueTransformationsAsString(0,
                          bmv->getObliqueTransformationsAsString(windowNumber));
         bmv->getSelectedObliqueSliceOffsets(windowNumber, slices);
         bmv->setSelectedObliqueSliceOffsets(0, slices);
         bmv->setSelectedAxis(0, bmv->getSelectedAxis(windowNumber));
      }
      
      //
      // Use scene window size for image size
      // note that height includes the toolbar and window title bar so shrink height some
      //
      imageWidth  = geometry[2];
      imageHeight = geometry[3];
      if ((glWidthWidthHeight[0] > 0) && 
          (glWidthWidthHeight[1] > 0)) {
         imageWidth  = glWidthWidthHeight[0];
         imageHeight = glWidthWidthHeight[1];
      }
      else {
         if (imageHeight > 200) {
            imageHeight -= 100;
         }
      }
      
      //
      // setup the off screen renderer
      //
      QImage image;
      OffScreenOpenGLWidget opengl;
      opengl.setFixedSize(imageWidth, imageHeight);

      //
      // Render the image
      //
      opengl.drawToImage(&brainSet,
                         brainModel,
                         image);
                      
      //
      // Track the image
      //
      capturedImages.push_back(image);
   }
   
   //
   // Get background color
   //
   const PreferencesFile* pf = brainSet.getPreferencesFile();
   unsigned char r, g, b;
   pf->getSurfaceBackgroundColor(r, g, b);
   const int backgroundColor[3] = { r, g, b };
   
   //
   // Combine the images
   //
   QImage outputImage;
   ImageFile::combinePreservingAspectAndFillIfNeeded(capturedImages,
                                                     imagesPerRow,
                                                     backgroundColor,
                                                     outputImage);
   ImageFile outputImageFile;
   outputImageFile.setImage(outputImage);
                                                     
   //
   // Write the image file
   //
   if (saveImageToFile) {
      try {
         outputImageFile.writeFile(imageFileName);
      }
      catch (FileException& e) {
         sceneErrorMessage += e.whatQString();
      }
      //if (outputImageFile.getImage()->save(imageFileName, "jpg") == false) {
      //   throw CommandException("Unable to write image file: " + imageFileName);
      //}
   }
   else {
      CommandImageView::displayQImage(*outputImageFile.getImage());
   }
   
   if (sceneWarningMessage.isEmpty() == false) {
      std::cout << getShortDescription().toAscii().constData()
                << " WARNING: "
                << sceneWarningMessage.toAscii().constData()
                << std::endl;
   }
   if (sceneErrorMessage.isEmpty() == false) {
      throw CommandException(sceneErrorMessage);
   }   
   
}

      


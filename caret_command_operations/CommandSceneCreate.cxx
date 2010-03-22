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

#include "BrainModelOpenGL.h"
#include "BrainModelSurface.h"
#include "BrainModelSurfaceOverlay.h"
#include "BrainModelVolume.h"
#include "BrainModelVolumeVoxelColoring.h"
#include "BrainSet.h"
#include "CommandSceneCreate.h"
#include "CoordinateFile.h"
#include "DisplaySettingsBorders.h"
#include "DisplaySettingsCells.h"
#include "DisplaySettingsFoci.h"
#include "DisplaySettingsVolume.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "PaintFile.h"
#include "ProbabilisticAtlasFile.h"
#include "ProgramParameters.h"
#include "RgbPaintFile.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"
#include "VolumeFile.h"

/**
 * constructor.
 */
CommandSceneCreate::CommandSceneCreate()
   : CommandBase("-scene-create",
                 "SCENE CREATION")
{
}

/**
 * destructor.
 */
CommandSceneCreate::~CommandSceneCreate()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSceneCreate::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Spec File", FileFilters::getSpecFileFilter());
   paramsOut.addFile("Input Scene File Name", FileFilters::getSceneFileFilter());
   paramsOut.addFile("Output Scene File Name", FileFilters::getSceneFileFilter());
   paramsOut.addString("Scene Name", "scene");
   paramsOut.addVariableListOfParameters("Scene Parameters");
}

/**
 * get full help information.
 */
QString 
CommandSceneCreate::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<spec-file-name>\n"
       + indent9 + "<input-scene-file-name>\n"
       + indent9 + "<output-scene-file-name>\n"
       + indent9 + "<scene-name>\n"
       + indent9 + "[-surface-overlay <surface-overlay-name>\n"
       + indent9 + "          <surface-overlay-data-type-name>\n"
       + indent9 + "          <surface-overlay-data-display-column-name-or-number>\n"
       + indent9 + "          <surface-overlay-data-threshold-column-name-or-number> ]\n"
       + indent9 + "[-show-borders]\n"
       + indent9 + "[-show-cells]\n"
       + indent9 + "[-show-foci]\n"
       + indent9 + "[-volume-overlay <volume-overlay-name>\n"
       + indent9 + "                 <volume-overlay-data-type-name> ]\n"
       + indent9 + "[-volume-overlay-anatomy <volume-file-name>\n"
       + indent9 + "                         <sub-volume-number> ]\n"
       + indent9 + "[-volume-overlay-functional-view <volume-file-name>\n"
       + indent9 + "                                 <sub-volume-number> ]\n"
       + indent9 + "[-volume-overlay-functional-thresh <volume-file-name>\n"
       + indent9 + "                                   <sub-volume-number> ]\n"
       + indent9 + "[-volume-overlay-paint <volume-file-name>\n"
       + indent9 + "                       <sub-volume-number> ]\n"
       + indent9 + "[-volume-overlay-rgb <volume-file-name>\n"
       + indent9 + "                     <sub-volume-number> ]\n"
       + indent9 + "[-volume-overlay-segmentation <volume-file-name>\n"
       + indent9 + "                              <sub-volume-number> ]\n"
       + indent9 + "[-volume-overlay-vector  <volume-file-name>\n"
       + indent9 + "                         <sub-volume-number> ]\n"
       + indent9 + "[-window-surface-files  <window-name>  \n"
       + indent9 + "                        <graphics-window-size-x>\n"
       + indent9 + "                        <graphics-window-size-y>\n"
       + indent9 + "                        <coordinate-file-name>\n"
       + indent9 + "                        <topology-file-name>\n"
       + indent9 + "                        <surface-view-name> ]\n"
       + indent9 + "[-window-surface-types  <window-name>\n"
       + indent9 + "                        <graphics-window-size-x>\n"
       + indent9 + "                        <graphics-window-size-y>\n"
       + indent9 + "                        <coordinate-file-type>\n"
       + indent9 + "                        <topology-file-type>\n"
       + indent9 + "                        <surface-view-name> ]\n"
       + indent9 + "[-window-volume-slice <window-name>\n"
       + indent9 + "                      <graphics-window-size-x>\n"
       + indent9 + "                      <graphics-window-size-y>\n"
       + indent9 + "                      <volume-view-name>\n"
       + indent9 + "                      <parasagittal-slice-number>\n"
       + indent9 + "                      <coronal-slice-number>\n"
       + indent9 + "                      <horizontal-slice-number> ]\n"
       + indent9 + "[-window-volume-coord <window-name>\n"
       + indent9 + "                      <graphics-window-size-x>\n"
       + indent9 + "                      <graphics-window-size-y>\n"
       + indent9 + "                      <volume-view-name>\n"
       + indent9 + "                      <stereotaxic-x-coordinate>\n"
       + indent9 + "                      <stereotaxic-y-coordinate>\n"
       + indent9 + "                      <stereotaxic-z-coordinate> ]\n"
       + indent9 + "\n"
       + indent9 + "\n"
       + indent9 + "Create a scene.\n"
       + indent9 + "\n"
       + indent9 + "Note that ALL files in the spec file will be loaded\n"
       + indent9 + "when the scene is loaded.\n"
       + indent9 + "\n"
       + indent9 + "If the scene name already exists, that scene will be \n"
       + indent9 + "replaced.\n"
       + indent9 + "\n"
       + indent9 + "\"surface-overlay-name\" is one of:\n"
       + indent9 + "   PRIMARY\n"
       + indent9 + "   SECONDARY\n"
       + indent9 + "   TERTIARY\n"
       + indent9 + "   UNDERLAY\n"
       + indent9 + "\n"
       + indent9 + "\"surface-overlay-data-type-name\"\n"
       + indent9 + "   CROSSOVERS\n"
       + indent9 + "   METRIC\n"
       + indent9 + "   NONE\n"
       + indent9 + "   PAINT\n"
       + indent9 + "   PROBABILISTIC_ATLAS\n"
       + indent9 + "   RGB_PAINT\n"
       + indent9 + "   SURFACE_SHAPE\n"
       + indent9 + "\n"
       + indent9 + "Both \"-surface-overlay-data-display-column-name-or-number\"\n"
       + indent9 + "and \"-surface overlay-data-threshold-column-name-or-number\"\n"
       + indent9 + "may be either a column number, which starts at 1, or the name\n"
       + indent9 + "ofthe data column.  Since all files are loaded by this \n"
       + indent9 + "command, column numbers should not be used when there are\n"
       + indent9 + "are multiple files for a data type as the order in which\n"
       + indent9 + "the files are loaded may change affecting the column orders.\n"
       + indent9 + "\n"
       + indent9 + "If an overlay data type does not use the display or \n"
       + indent9 + "threshold column, enter any value such as \"-1\".\n"
       + indent9 + "\n"
       + indent9 + "\"volume-overlay-name\" is one of:\n"
       + indent9 + "   PRIMARY\n"
       + indent9 + "   SECONDARY\n"
       + indent9 + "   UNDERLAY\n"
       + indent9 + "\n"
       + indent9 + "\"volume-overlay-name\" is one of:\n"
       + indent9 + "   ANATOMY\n"
       + indent9 + "   FUNCTIONAL\n"
       + indent9 + "   NONE\n"
       + indent9 + "   PAINT\n"
       + indent9 + "   PROBABILISTIC_ATLAS\n"
       + indent9 + "   RGB\n"
       + indent9 + "   SEGMENTATION\n"
       + indent9 + "   VECTOR\n"
       + indent9 + "\n"
       + indent9 + "\"sub-volume-number\" ranges from 1 to the number of sub-\n"
       + indent9 + "volumes contained in the volume file.\n"
       + indent9 + "\n"
       + indent9 + "\"volume-view-name\" is one of:\n"
       + indent9 + "   PARASAGITTAL\n"
       + indent9 + "   CORONAL\n"
       + indent9 + "   HORIZONTAL\n"
       + indent9 + "\n"
       + indent9 + "Use \"-window-surface-files\" to specify specific \n"
       + indent9 + "coordinate and topology files for a surface.\n"
       + indent9 + "\n"
       + indent9 + "Use \"-window-surface-type\" to use the first coordinate\n"
       + indent9 + "and topoogy files of the specified types as sorted by date.\n"
       + indent9 + "\n"
       + indent9 + "\"window-name\" is one of:\n"
       + indent9 + "   WINDOW_MAIN\n"
       + indent9 + "   WINDOW_2\n"
       + indent9 + "   WINDOW_3\n"
       + indent9 + "   WINDOW_4\n"
       + indent9 + "   WINDOW_5\n"
       + indent9 + "   WINDOW_6\n"
       + indent9 + "   WINDOW_7\n"
       + indent9 + "   WINDOW_8\n"
       + indent9 + "   WINDOW_9\n"
       + indent9 + "   WINDOW_10\n"
       + indent9 + "\n"
       + indent9 + "\"coordinate-file-type\" is one of:\n"
       + indent9 + "   CMW\n"
       + indent9 + "   ELLIPSOIDAL\n"
       + indent9 + "   FIDUCIAL\n"
       + indent9 + "   FLAT\n"
       + indent9 + "   FLAT_LOBAR\n"
       + indent9 + "   HULL\n"
       + indent9 + "   INFLATED\n"
       + indent9 + "   RAW\n"
       + indent9 + "   SPHERICAL\n"
       + indent9 + "   VERY_INFLATED\n"
       + indent9 + "\n"
       + indent9 + "\"topology-file-type\" is one of:\n"
       + indent9 + "   CLOSED\n"
       + indent9 + "   CUT\n"
       + indent9 + "   OPEN\n"
       + indent9 + "\n"
       + indent9 + "\"surface-view-name\" is one of:\n"
       + indent9 + "   ANTERIOR\n"
       + indent9 + "   DEFAULT\n"
       + indent9 + "   DORSAL\n"
       + indent9 + "   LATERAL\n"
       + indent9 + "   MEDIAL\n"
       + indent9 + "   POSTERIOR\n"
       + indent9 + "   VENTRAL\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSceneCreate::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   const QString specFileName =
      parameters->getNextParameterAsString("Spec File Name");
   const QString inputSceneFileName =
      parameters->getNextParameterAsString("Input Scene File Name");
   const QString outputSceneFileName =
      parameters->getNextParameterAsString("Output Scene File Name");
   const QString sceneName =
      parameters->getNextParameterAsString("Scene Name");
   
   //
   // Read the spec file and select all files except scenes
   //
   SpecFile specFile;
   specFile.readFile(specFileName);
   specFile.setAllFileSelections(SpecFile::SPEC_TRUE);
   specFile.sceneFile.setAllSelections(SpecFile::SPEC_FALSE);
   if (inputSceneFileName.isEmpty() == false) {
      specFile.addToSpecFile(SpecFile::getSceneFileTag(),
                             inputSceneFileName,
                             "",
                             false);
   }
   
   //
   // Read all files in the spec file
   //
   QString errorMessage;
   BrainSet brainSet(true);
   brainSet.readSpecFile(specFile,
                         specFileName,
                         errorMessage);
   if (errorMessage.isEmpty() == false) {
      throw CommandException(errorMessage);
   }

   //
   // For each window, main and viewing
   //
   std::vector<SceneFile::SceneClass> windowSceneClasses;
            
   //
   // Check the optional parameters
   //
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Optional Parameter");
      if (paramName == "-surface-overlay") {
         const QString surfaceOverlayName = 
            parameters->getNextParameterAsString("Surface Overlay Name");
         const QString surfaceOverlayDataTypeName = 
            parameters->getNextParameterAsString("Surface Overlay Data Type Name");
         const QString surfaceOverlayDisplayColumnNameNumber = 
            parameters->getNextParameterAsString("Surface Overlay Display Columnn Name/Number");
         const QString surfaceOverlayThresholdColumnNameNumber = 
            parameters->getNextParameterAsString("Surface Overlay Threshold Column Name/Number");
      
         BrainModelSurfaceOverlay* surfaceOverlay = NULL;
         if (surfaceOverlayName == "PRIMARY") {
            surfaceOverlay = brainSet.getPrimarySurfaceOverlay();
         }
         else if (surfaceOverlayName == "SECONDARY") {
            surfaceOverlay = brainSet.getSecondarySurfaceOverlay();
         }
         else if (surfaceOverlayName == "TERTIARY") {
            if (brainSet.getNumberOfSurfaceOverlays() > 3) {
               surfaceOverlay = brainSet.getSurfaceOverlay(brainSet.getNumberOfSurfaceOverlays() - 3);
            }
            else {
               throw CommandException("Tertiary overlay not supported in this command.");
            }
         }
         else if (surfaceOverlayName == "UNDERLAY") {
            surfaceOverlay = brainSet.getSurfaceUnderlay();
         }
         else {
            throw CommandException("Invalid overlay name \""
                                   + surfaceOverlayName
                                   + "\"");
         }
         
         if (surfaceOverlayDataTypeName == "CROSSOVERS") {
            surfaceOverlay->setOverlay(-1, BrainModelSurfaceOverlay::OVERLAY_SHOW_CROSSOVERS);
         }
         else if (surfaceOverlayDataTypeName == "METRIC") {
            surfaceOverlay->setOverlay(-1, BrainModelSurfaceOverlay::OVERLAY_METRIC);
            surfaceOverlay->setDisplayColumnSelected(-1, 
               brainSet.getMetricFile()->getColumnFromNameOrNumber(
                                   surfaceOverlayDisplayColumnNameNumber, false));
            surfaceOverlay->setThresholdColumnSelected(-1, 
               brainSet.getMetricFile()->getColumnFromNameOrNumber(
                                   surfaceOverlayThresholdColumnNameNumber, false));
         }
         else if (surfaceOverlayDataTypeName == "NONE") {
            surfaceOverlay->setOverlay(-1, BrainModelSurfaceOverlay::OVERLAY_NONE);
         }
         else if (surfaceOverlayDataTypeName == "PAINT") {
            surfaceOverlay->setOverlay(-1, BrainModelSurfaceOverlay::OVERLAY_PAINT);
            surfaceOverlay->setDisplayColumnSelected(-1, 
               brainSet.getPaintFile()->getColumnFromNameOrNumber(
                                   surfaceOverlayDisplayColumnNameNumber, false));
         }
         else if (surfaceOverlayDataTypeName == "PROBABILISTIC_ATLAS") {
            surfaceOverlay->setOverlay(-1, BrainModelSurfaceOverlay::OVERLAY_PROBABILISTIC_ATLAS);
         }
         else if (surfaceOverlayDataTypeName == "RGB_PAINT") {
            surfaceOverlay->setOverlay(-1, BrainModelSurfaceOverlay::OVERLAY_RGB_PAINT);
            surfaceOverlay->setDisplayColumnSelected(-1, 
               brainSet.getRgbPaintFile()->getColumnFromNameOrNumber(
                                   surfaceOverlayDisplayColumnNameNumber, false));
         }
         else if (surfaceOverlayDataTypeName == "SURFACE_SHAPE") {
            surfaceOverlay->setOverlay(-1, BrainModelSurfaceOverlay::OVERLAY_SURFACE_SHAPE);
            surfaceOverlay->setDisplayColumnSelected(-1, 
               brainSet.getSurfaceShapeFile()->getColumnFromNameOrNumber(
                                   surfaceOverlayDisplayColumnNameNumber, false));
         }
         else {
            throw CommandException("Invalid overlay data type name \""
                                   + surfaceOverlayDataTypeName
                                   + "\"");
         }
      }
      else if (paramName == "-show-borders") {
         DisplaySettingsBorders* dsb = brainSet.getDisplaySettingsBorders();
         dsb->setDisplayBorders(true);
      }
      else if (paramName == "-show-cells") {
         DisplaySettingsCells* dsc = brainSet.getDisplaySettingsCells();
         dsc->setDisplayCells(true);
      }
      else if (paramName == "-show-foci") {
         DisplaySettingsFoci* dsf = brainSet.getDisplaySettingsFoci();
         dsf->setDisplayCells(true);
      }
      else if (paramName == "-volume-overlay") {
         const QString volumeOverlayName = 
            parameters->getNextParameterAsString("Volume Overlay Name");
         const QString volumeOverlayDataTypeName = 
            parameters->getNextParameterAsString("Volume Overlay Data Type Name");
            
         
         BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_TYPE volumeOverlay;
         if (volumeOverlayDataTypeName == "ANATOMY") {
            volumeOverlay = BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_ANATOMY;
         }
         else if (volumeOverlayDataTypeName == "FUNCTIONAL") {
            volumeOverlay = BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_FUNCTIONAL;
         }
         else if (volumeOverlayDataTypeName == "NONE") {
            volumeOverlay = BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_NONE;
         }
         else if (volumeOverlayDataTypeName == "PAINT") {
            volumeOverlay = BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PAINT;
         }
         else if (volumeOverlayDataTypeName == "PROBABILISTIC_ATLAS") {
            volumeOverlay = BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_PROB_ATLAS;
         }
         else if (volumeOverlayDataTypeName == "RGB") {
            volumeOverlay = BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_RGB;
         }
         else if (volumeOverlayDataTypeName == "SEGMENTATION") {
            volumeOverlay = BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_SEGMENTATION;
         }
         else if (volumeOverlayDataTypeName == "VECTOR") {
            volumeOverlay = BrainModelVolumeVoxelColoring::UNDERLAY_OVERLAY_VECTOR;
         }
         else {
            throw CommandException("Invalid Volume Overlay Data Type Name "
                                   + volumeOverlayDataTypeName);
         }
         
         BrainModelVolumeVoxelColoring* voxelColoring = brainSet.getVoxelColoring();
         if (volumeOverlayName == "PRIMARY") {
            voxelColoring->setPrimaryOverlay(volumeOverlay);
         }
         else if (volumeOverlayName == "SECONDARY") {
            voxelColoring->setSecondaryOverlay(volumeOverlay);
         }
         else if (volumeOverlayName == "UNDERLAY") {
            voxelColoring->setUnderlay(volumeOverlay);
         }
         else {
            throw CommandException("Invalid Volume Overlay Name "
                                   + volumeOverlayName);
         }
      }
      else if (paramName == "-volume-overlay-anatomy") {
         const QString volumeFileName = 
            parameters->getNextParameterAsString("Anatomy Volume File Name");
         const int subVolumeNumber =
            parameters->getNextParameterAsInt("Anatomy Sub-Volume Number");
         selectVolumeOfType(brainSet,
                            VolumeFile::VOLUME_TYPE_ANATOMY,
                            volumeFileName,
                            subVolumeNumber);                            
      }
      else if (paramName == "-volume-overlay-functional-view") {
         const QString volumeFileName = 
            parameters->getNextParameterAsString("Functional View Volume File Name");
         const int subVolumeNumber =
            parameters->getNextParameterAsInt("Functional View Sub-Volume Number");
         selectVolumeOfType(brainSet,
                            VolumeFile::VOLUME_TYPE_FUNCTIONAL,
                            volumeFileName,
                            subVolumeNumber);                            
      }
      else if (paramName == "-volume-overlay-functional-thresh") {
         const QString volumeFileName = 
            parameters->getNextParameterAsString("Functional Threshold Volume File Name");
         const int subVolumeNumber =
            parameters->getNextParameterAsInt("Functional Threshold Sub-Volume Number");
         selectVolumeOfType(brainSet,
                            VolumeFile::VOLUME_TYPE_FUNCTIONAL,
                            volumeFileName,
                            subVolumeNumber,
                            true);                            
      }
      else if (paramName == "-volume-overlay-paint") {
         const QString volumeFileName = 
            parameters->getNextParameterAsString("Paint Volume File Name");
         const int subVolumeNumber =
            parameters->getNextParameterAsInt("Paint Sub-Volume Number");
         selectVolumeOfType(brainSet,
                            VolumeFile::VOLUME_TYPE_PAINT,
                            volumeFileName,
                            subVolumeNumber);                            
      }
      else if (paramName == "-volume-overlay-rgb") {
         const QString volumeFileName = 
            parameters->getNextParameterAsString("RGB Volume File Name");
         const int subVolumeNumber =
            parameters->getNextParameterAsInt("RGB Sub-Volume Number");
         selectVolumeOfType(brainSet,
                            VolumeFile::VOLUME_TYPE_RGB,
                            volumeFileName,
                            subVolumeNumber);                            
      }
      else if (paramName == "-volume-overlay-segmentation") {
         const QString volumeFileName = 
            parameters->getNextParameterAsString("Segmentation Volume File Name");
         const int subVolumeNumber =
            parameters->getNextParameterAsInt("Segmentation Sub-Volume Number");
         selectVolumeOfType(brainSet,
                            VolumeFile::VOLUME_TYPE_SEGMENTATION,
                            volumeFileName,
                            subVolumeNumber);                            
      }
      else if (paramName == "-volume-overlay-vector") {
         const QString volumeFileName = 
            parameters->getNextParameterAsString("Vector Volume File Name");
         const int subVolumeNumber =
            parameters->getNextParameterAsInt("Vector Sub-Volume Number");
         selectVolumeOfType(brainSet,
                            VolumeFile::VOLUME_TYPE_VECTOR,
                            volumeFileName,
                            subVolumeNumber);                            
      }
      else if (paramName == "-window-surface-files") {
         const QString windowName = parameters->getNextParameterAsString("Window Name");
         const int graphicsSizeX = parameters->getNextParameterAsInt("Graphics Window Size X");
         const int graphicsSizeY = parameters->getNextParameterAsInt("Graphics Window Size Y");
         const QString coordFileName = parameters->getNextParameterAsString("Coordinate File Name");
         const QString topoFileName = parameters->getNextParameterAsString("Topology File Name");
         const QString viewName = parameters->getNextParameterAsString("View Name");
         
         //
         // Find the window and the brain model
         //
         const int windowNumber = getWindowNumber(windowName);
         BrainModelSurface* bms = findBrainModelSurface(brainSet, coordFileName, topoFileName);
         
         //
         // Set the view
         //
         const BrainModel::STANDARD_VIEWS stdView = getSurfaceStandardView(viewName);
         bms->setToStandardView(windowNumber, stdView);
         
         //
         // Window sizing
         //
         const int graphicsSize[2] = { graphicsSizeX, graphicsSizeY };
         const int windowGeometry[4] = {
            -1, -1, -1, -1
         };
   
         //
         // Set the default scaling
         //
         setWindowScaling(brainSet, graphicsSizeX, graphicsSizeY);
         
         //
         // create the part of the scene for the surface
         //
         SceneFile::SceneClass windowSceneClass("");
         brainSet.saveSceneForBrainModelWindow(windowNumber,
                                               windowGeometry,
                                               graphicsSize,
                                               bms,
                                               false,
                                               windowSceneClass);
         windowSceneClasses.push_back(windowSceneClass);
      }
      else if (paramName == "-window-surface-types") {
         const QString windowName = parameters->getNextParameterAsString("Window Name");
         const int graphicsSizeX = parameters->getNextParameterAsInt("Graphics Window Size X");
         const int graphicsSizeY = parameters->getNextParameterAsInt("Graphics Window Size Y");
         const QString coordFileType = parameters->getNextParameterAsString("Coordinate File Type");
         const QString topoFileType = parameters->getNextParameterAsString("Topology File Type");
         const QString viewName = parameters->getNextParameterAsString("View Name");
         
         //
         // Find the window and the brain model
         //
         const int windowNumber = getWindowNumber(windowName);
         BrainModelSurface* bms = findBrainModelSurfaceFromTypes(brainSet, coordFileType, topoFileType);
         
         //
         // Set the view
         //
         const BrainModel::STANDARD_VIEWS stdView = getSurfaceStandardView(viewName);
         bms->setToStandardView(windowNumber, stdView);
         
         //
         // Window sizing
         //
         const int graphicsSize[2] = { graphicsSizeX, graphicsSizeY };
         const int windowGeometry[4] = {
            -1, -1, -1, -1
         };
   
         //
         // Set the default scaling
         //
         setWindowScaling(brainSet, graphicsSizeX, graphicsSizeY);
         
         //
         // create the part of the scene for the surface
         //
         SceneFile::SceneClass windowSceneClass("");
         brainSet.saveSceneForBrainModelWindow(windowNumber,
                                               windowGeometry,
                                               graphicsSize,
                                               bms,
                                               false,
                                               windowSceneClass);
         windowSceneClasses.push_back(windowSceneClass);
      }
      else if ((paramName == "-window-volume-slice") ||
               (paramName == "-window-volume-coord")) {
         //
         // Find the volume
         //
         BrainModelVolume* bmv = brainSet.getBrainModelVolume();
         if (bmv == NULL) {
            throw CommandException("No volumes were loaded from the spec file.");
         }
         const VolumeFile* underlayVolume = bmv->getUnderlayVolumeFile();
         if (underlayVolume == NULL) {
            throw CommandException("Unable to find the underlay volume.");
         }

         const QString windowName = parameters->getNextParameterAsString("Window Name");
         const int graphicsSizeX = parameters->getNextParameterAsInt("Graphics Window Size X");
         const int graphicsSizeY = parameters->getNextParameterAsInt("Graphics Window Size Y");
         const QString volumeViewName = parameters->getNextParameterAsString("Volume View Name");
         int slices[3] = { -1, -1, -1 };
         if (paramName == "-window-volume-slice") {
            slices[0] = parameters->getNextParameterAsInt("Parasagittal Slice Number");
            slices[1] = parameters->getNextParameterAsInt("Coronal Slice Number");
            slices[2] = parameters->getNextParameterAsInt("Horizontal Slice Number");
            int dim[3];
            underlayVolume->getDimensions(dim);
            if (((slices[0] < 0) || (slices[0] > dim[0])) ||
                ((slices[1] < 0) || (slices[1] > dim[1])) ||
                ((slices[2] < 0) || (slices[2] > dim[2]))) {
               throw CommandException("Volume slices are not in the volume.");
            }
         }
         else {
            const float xyz[3] = {
               parameters->getNextParameterAsFloat("Stereotaxic X-Coordinate"),
               parameters->getNextParameterAsFloat("Stereotaxic Y-Coordinate"),
               parameters->getNextParameterAsFloat("Stereotaxic Z-Coordinate")
            };
            if (underlayVolume->convertCoordinatesToVoxelIJK(xyz, slices) == false) {
               throw CommandException("Volume stereotaxic coordinates are not in the volume.");
            }
         }
         
         //
         // Get the view
         //
         VolumeFile::VOLUME_AXIS volumeAxis;
         if (volumeViewName == "PARASAGITTAL") {
            volumeAxis = VolumeFile::VOLUME_AXIS_X;
         }
         else if (volumeViewName == "CORONAL") {
            volumeAxis = VolumeFile::VOLUME_AXIS_Y;
         }
         else if (volumeViewName == "HORIZONTAL") {
            volumeAxis = VolumeFile::VOLUME_AXIS_Z;
         }
         else {
            throw CommandException("Invalid volume view name: " + volumeViewName);
         }
         
         //
         // Find the window
         //
         const int windowNumber = getWindowNumber(windowName);
         
         //
         // Set the view
         //
         bmv->setSelectedAxis(windowNumber, volumeAxis);
         
         //
         // Set the slices
         //
         bmv->setSelectedOrthogonalSlices(windowNumber, slices);
         
         //
         // Window sizing
         //
         const int graphicsSize[2] = { graphicsSizeX, graphicsSizeY };
         const int windowGeometry[4] = {
            -1, -1, -1, -1
         };
   
         //
         // Set the default scaling
         //
         setWindowScaling(brainSet, graphicsSizeX, graphicsSizeY);
         
         //
         // create the part of the scene for the volume
         //
         SceneFile::SceneClass windowSceneClass("");
         brainSet.saveSceneForBrainModelWindow(windowNumber,
                                               windowGeometry,
                                               graphicsSize,
                                               bmv,
                                               false,
                                               windowSceneClass);
         windowSceneClasses.push_back(windowSceneClass);
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }
   
   if (windowSceneClasses.empty()) {
      throw CommandException("No windows were specified.");
   }
   
   //
   // See if scene already exists
   //
   SceneFile* sceneFile = brainSet.getSceneFile();
   const int sceneIndex = sceneFile->getSceneIndexFromName(sceneName);
   
   //
   // Create the scene
   //
   QString warningMessage;
   if (sceneIndex >= 0) {
      brainSet.replaceScene(sceneFile,
                       sceneIndex,
                       windowSceneClasses,
                       sceneName,
                       false,
                       errorMessage,
                       warningMessage);
   }
   else {
      brainSet.saveScene(brainSet.getSceneFile(),
                         windowSceneClasses,
                         sceneName,
                         false,
                         errorMessage,
                         warningMessage);
   }
   if (errorMessage.isEmpty() == false) {
      throw CommandException("ERROR saving scene: " + errorMessage);
   }
   
   if (warningMessage.isEmpty() == false) {
      std::cout << "WARNING: " << warningMessage.toAscii().constData() << std::endl;
   }
   
   //
   // Write the scene file
   //
   brainSet.writeSceneFile(outputSceneFileName);
}

/**
 * get the number of a window from its name.
 */
int 
CommandSceneCreate::getWindowNumber(const QString& windowName) const throw (CommandException)
{
   if (windowName == "WINDOW_MAIN") {
      return BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW;
   }
   else if (windowName == "WINDOW_2") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_2;
   }
   else if (windowName == "WINDOW_3") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_3;
   }
   else if (windowName == "WINDOW_4") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_4;
   }
   else if (windowName == "WINDOW_5") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_5;
   }
   else if (windowName == "WINDOW_6") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_6;
   }
   else if (windowName == "WINDOW_7") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_7;
   }
   else if (windowName == "WINDOW_8") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_8;
   }
   else if (windowName == "WINDOW_9") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_9;
   }
   else if (windowName == "WINDOW_10") {
      return BrainModel::BRAIN_MODEL_VIEW_AUX_WINDOW_10;
   }
   else {
      throw CommandException("Unrecognized window name: " + windowName);
   }
   
   return BrainModel::BRAIN_MODEL_VIEW_MAIN_WINDOW;
}  

/**
 * find a brain model surface from types.
 */
BrainModelSurface* 
CommandSceneCreate::findBrainModelSurfaceFromTypes(BrainSet& brainSet,
                                         const QString& coordFileType,
                                         const QString& topoFileType) 
                                            const throw (CommandException)
{
   const BrainModelSurface::SURFACE_TYPES surfaceType =
      BrainModelSurface::getSurfaceTypeFromConfigurationID(coordFileType);
   if (surfaceType == BrainModelSurface::SURFACE_TYPE_UNKNOWN) {
      throw CommandException("Unreognized coordinate file type name : "
                             + coordFileType);
   }
   
   const TopologyFile::TOPOLOGY_TYPES topologyType =
      TopologyFile::getTopologyTypeFromPerimeterID(topoFileType);
   if (topologyType == TopologyFile::TOPOLOGY_TYPE_UNKNOWN) {
      throw CommandException("Unreognized topology file type name : "
                             + topoFileType); 
   }
   
   //
   // Find the surface
   //
   BrainModelSurface* surface = NULL;
   const int numModels = brainSet.getNumberOfBrainModels();
   for (int i = 0; i < numModels; i++) {
      BrainModelSurface* bms = brainSet.getBrainModelSurface(i);
      if (bms != NULL) {
         if (bms->getSurfaceType() == surfaceType) {
            surface = bms;
            break;
         }
      }
   }
   
   if (surface == NULL) {
      throw CommandException("No surface found for type: " 
                             + coordFileType);
   }
   
   //
   // Find the topology file
   //
   const int numTopo = brainSet.getNumberOfTopologyFiles();
   for (int j = 0; j < numTopo; j++) {
      TopologyFile* tf = brainSet.getTopologyFile(j);
      if (tf->getTopologyType() == topologyType) {
         surface->setTopologyFile(tf);
         return surface;
      }
   }
   
   throw CommandException("No topology found for type: "
                          + topoFileType);
                          
   return NULL;
}

/**
 * find a brain model surface.
 */
BrainModelSurface* 
CommandSceneCreate::findBrainModelSurface(BrainSet& brainSet,
                                          const QString& coordFileName,
                                          const QString& topoFileName) 
                                                const throw (CommandException)
{
   //
   // Search for the files 
   //
   const int numModels = brainSet.getNumberOfBrainModels();
   for (int i = 0; i < numModels; i++) {
      BrainModelSurface* bms = brainSet.getBrainModelSurface(i);
      if (bms != NULL) {
         if (FileUtilities::basename(bms->getCoordinateFile()->getFileName()) ==
             FileUtilities::basename(coordFileName)) {
            //
            // Find the topology file
            //
            const int numTopo = brainSet.getNumberOfTopologyFiles();
            for (int j = 0; j < numTopo; j++) {
               TopologyFile* tf = brainSet.getTopologyFile(j);
               if (FileUtilities::basename(tf->getFileName()) ==
                   FileUtilities::basename(topoFileName)) {
                  bms = bms;
                  bms->setTopologyFile(tf);
                  return bms;
               }
            }
            
            throw CommandException("Unable to find topology file named "
                                   + topoFileName);
         }
      }
   }
   
   throw CommandException("Unable to find surface with coordinate file named "
                          + coordFileName);
}      

/**
 * get standard view from its name.
 */
BrainModel::STANDARD_VIEWS 
CommandSceneCreate::getSurfaceStandardView(const QString& surfaceViewName) const throw (CommandException)
{
   if (surfaceViewName == "ANTERIOR") {
      return BrainModel::VIEW_ANTERIOR;
   }
   else if (surfaceViewName == "DEFAULT") {
      return BrainModel::VIEW_RESET;
   }
   else if (surfaceViewName == "DORSAL") {
      return BrainModel::VIEW_DORSAL;
   }
   else if (surfaceViewName == "LATERAL") {
      return BrainModel::VIEW_LATERAL;
   }
   else if (surfaceViewName == "MEDIAL") {
      return BrainModel::VIEW_MEDIAL;
   }
   else if (surfaceViewName == "POSTERIOR") {
      return BrainModel::VIEW_POSTERIOR;
   }
   else if (surfaceViewName == "VENTRAL") {
      return BrainModel::VIEW_VENTRAL;
   }
   else {
      throw CommandException("Invalid surface-view-name: " + surfaceViewName);
   }
   
   return BrainModel::VIEW_DORSAL;
}

/**
 * select a volume of the specified type.
 */
void 
CommandSceneCreate::selectVolumeOfType(BrainSet& brainSet,
                                       const VolumeFile::VOLUME_TYPE volumeType,
                                       const QString& volumeFileNameIn,
                                       const int subVolumeNumber,
                                       const bool functionalThresholdFlag) throw (CommandException)
{
   const QString volumeFileName = FileUtilities::basename(volumeFileNameIn);
   
   std::vector<VolumeFile*> volumeFiles;
   
   switch (volumeType) {
      case VolumeFile::VOLUME_TYPE_ANATOMY:
         brainSet.getVolumeAnatomyFiles(volumeFiles);
         break;
      case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         brainSet.getVolumeFunctionalFiles(volumeFiles);
         break;
      case VolumeFile::VOLUME_TYPE_PAINT:
         brainSet.getVolumePaintFiles(volumeFiles);
         break;
      case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         brainSet.getVolumeProbAtlasFiles(volumeFiles);
         break;
      case VolumeFile::VOLUME_TYPE_RGB:
         brainSet.getVolumeRgbFiles(volumeFiles);
         break;
      case VolumeFile::VOLUME_TYPE_SEGMENTATION: 
         brainSet.getVolumeSegmentationFiles(volumeFiles);
         break;
      case VolumeFile::VOLUME_TYPE_VECTOR:
         brainSet.getVolumeVectorFiles(volumeFiles);
         break;
      case VolumeFile::VOLUME_TYPE_ROI:
      case VolumeFile::VOLUME_TYPE_UNKNOWN:
         break;
   }
   
   int fileIndex = -1;
   const int numFiles = static_cast<int>(volumeFiles.size());
   for (int i = 0; i < numFiles; i++) {
      const QString name(FileUtilities::basename(volumeFiles[i]->getFileName()));
      if (name == volumeFileName) {
         fileIndex = i + subVolumeNumber - 1;
         break;
      }
   }
   
   if ((fileIndex >= 0) &&
       (fileIndex < numFiles)) {
   }
   else {
      throw CommandException("Volume file named " 
                             + volumeFileName
                             + " and sub-volume number "
                             + QString::number(subVolumeNumber)
                             + " not found.");
   }
   
   DisplaySettingsVolume* dsv = brainSet.getDisplaySettingsVolume();
   switch (volumeType) {
      case VolumeFile::VOLUME_TYPE_ANATOMY:
         dsv->setSelectedAnatomyVolume(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_FUNCTIONAL:
         if (functionalThresholdFlag == false) {
            dsv->setSelectedFunctionalVolumeView(fileIndex);
         }
         dsv->setSelectedFunctionalVolumeThreshold(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_PAINT:
         dsv->setSelectedPaintVolume(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_PROB_ATLAS:
         break;
      case VolumeFile::VOLUME_TYPE_RGB:
         dsv->setSelectedRgbVolume(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_SEGMENTATION: 
         dsv->setSelectedSegmentationVolume(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_VECTOR:
         dsv->setSelectedVectorVolume(fileIndex);
         break;
      case VolumeFile::VOLUME_TYPE_ROI:
      case VolumeFile::VOLUME_TYPE_UNKNOWN:
         break;
   }
}

/**
 * set the default window scaling.
 */
void 
CommandSceneCreate::setWindowScaling(BrainSet& bs,
                                     const int sizeX,
                                     const int sizeY)
{
   double orthoRight, orthoTop;
   BrainModelOpenGL::getDefaultOrthoRightAndTop(sizeX,
                                                sizeY,
                                                orthoRight,
                                                orthoTop);
   bs.setDefaultScaling(orthoRight, orthoTop);
}
                            

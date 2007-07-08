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



//
// This file contains the main function
//

#include <cstring>
#include <iostream>

#include <qglobal.h>
#include <qapplication.h>
#include <qmessagebox.h>

#include "BrainSet.h"
#include "DebugControl.h"
#include "GuiMapFmriMainWindow.h"
#include "ParamsFile.h"
#include "PreferencesFile.h"
#include "VolumeFile.h"

#define _MAP_FMRI_MAIN_FLAG_
#include "map_fmri_global_variables.h"
#undef _MAP_FMRI_MAIN_FLAG_
#include "map_fmri_prototypes.h"

static char* atlasEnvironmentName = "CARET_ATLAS_DIRECTORIES";
static bool haveGUI = false;

/*
 * Display an error message
 */
void
displayErrorMessage(const std::string& msg)
{
   if (haveGUI) {
      QApplication::beep();
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(theMainWindow, 
                               "Mapping Error",
                               msg.c_str(),
                               "OK");
   }
   else {
      std::cout << msg << std::endl;
   }
}

/*
 * Display a progress message.  Returns true if execution should be aborted.
 */
bool
displayProgressMessage(const int progressCount, const int totalProgressCount)
{
   bool cancelled = false;
   if (haveGUI) {
      cancelled = theMainWindow->updateProgressDialog(progressCount, totalProgressCount, 
                                                      "Mapping fMRI Data");
   }
   return cancelled;
}

/*
 * Display an informational message
 */
void
displayInformationMessage(const std::string& msg)
{
   if (haveGUI) {
      QApplication::beep();
      QMessageBox::information(theMainWindow, 
                               "Map fMRI to Surface",
                               msg.c_str(),
                               "OK");
   }
   else {
      std::cout << msg << std::endl;
   }
}

/*
 * Display an error message
 */
void
displaySuccessMessage()
{
   std::string msg;
   if (haveGUI== false) {
      msg.append("\n");
   }
   msg.append("fMRI data successfully mapped.\n");
   msg.append("The following metric files were created:\n");
   for (int i = 0; i < fmriMapper.getNumberOfSurfaces(); i++) {
      msg.append("   ");
      FMRISurfaceInfo* fsi = fmriMapper.getSurface(i);
      msg.append(fsi->getMetricFileName());
      msg.append("\n");
   }
   displayInformationMessage(msg);
}


/*
 * OSX only, initialize class static variables
 */
static void
initializeStatic()
{
#ifdef Q_OS_MAC
   BrainSet::initializeDataFileStaticMembers();
#endif
}

/*
 * Read the environment variable CARET_ATLAS_PATH to get paths to atlases
 */
static void
readAtlasesFromEnvironment()
{
   const char* caretAtlasDirs = getenv(atlasEnvironmentName);
   if (caretAtlasDirs != NULL) {
      std::string s(caretAtlasDirs);
      
      std::vector<std::string> atlasDirs;
      StringUtilities::token(s, ":", atlasDirs);
      
      for (unsigned int i = 0; i < atlasDirs.size(); i++) {
         std::string errorMessage;
         if (fmriMapper.addAtlasDirectory(atlasDirs[i], false, errorMessage)) {
            displayErrorMessage(errorMessage);
         }
      }
   }
}


/*
 * Read the parameters file
 */
static bool
readParamsFile(const char* filename)
{
   ParamsFile pf;
   
   try {
      pf.readFile(filename);
   }
   catch (FileIOException& e) {
      std::cout << "ERROR reading params file " << filename 
                << ": " << e.what() << std::endl;
      return true;
   }
   
   //
   // Padding
   //
   std::string padding;
   if (pf.getParameter(ParamsFile::keyPadded, padding) == 0) {
      if (padding == ParamsFile::keyValueYes) {
         int negX = 0;
         int posX = 0;
         int negY = 0;
         int posY = 0;
         int negZ = 0;
         int posZ = 0;
         pf.getParameter(ParamsFile::keyOldPadNegX, negX);
         pf.getParameter(ParamsFile::keyOldPadPosX, posX);
         pf.getParameter(ParamsFile::keyOldPadNegY, negY);
         pf.getParameter(ParamsFile::keyOldPadPosY, posY);
         pf.getParameter(ParamsFile::keyOldPadNegZ, negZ);
         pf.getParameter(ParamsFile::keyOldPadPosZ, posZ);
         fmriMapper.setIndivPadding(true, negX, negY, negZ, posX, posY, posZ);
      }
   }
   
   //
   // Cropping
   //
   std::string cropping;
   if (pf.getParameter(ParamsFile::keyCropped, cropping) == 0) {
      if (cropping == ParamsFile::keyValueYes) {
         int x = 0, y = 0, z = 0;
         pf.getParameter(ParamsFile::keyXmin, x);
         pf.getParameter(ParamsFile::keyYmin, y);
         pf.getParameter(ParamsFile::keyZmin, z);
         fmriMapper.setIndivCroppingOffset(true, x, y, z);
      }
   }
   
   return false;
}

/*
 * Program usage information
 */
static void
usage()
{
   std::cout <<
"NAME\n"
"   map_fmri_to_surface - map fMRI volumes to surfaces\n"
"\n"
"SYNOPSIS\n"
"   map_fmri_to_surface [options]\n"
"\n"
"DESCRIPTION\n"
"   This program is used to map fMRI volume voxels to nodes in a surface\n"
"   producing a metric file.\n"
"\n"
"   map_fmri_to_surface has the following options:\n"
"\n"
"   -a algorithm\n"
"      Specifies the algorithm for mapping voxels to nodes.  Valid values are:\n"
"         AN  Use the Average Nodes algorithm\n"
"         GS  Use the Gaussian algorithm\n"
"         AV  Use the Average Voxel algorithm\n"
"         MV  Use the Maximum Voxel algorithm\n"
"         MB  Use the MCW Brainfish algorithm\n"
"\n"
"\n"
"   -b Y-or-N\n"
"      Specifies bytes swapping where \"Y-or-N\" is either \"Y\" to enable\n"
"      byte swapping or \"N\" to disable byte swapping.\n"
"\n"
"   -bf MD SF\n"
"      Specifies the MCW Brainfish algorithm parameters.\n"
"        MD value for max distance (float).\n"
"        SF value for splat factor (integer).\n"
"\n"
"   -d dim-X  dim-Y  dim-Z\n"
"      Specifies the dimensions of raw volumes.  Three numbers should follow\n"
"      the \"-d\" option listing the X, Y, and Z dimensions for the volumes.\n"
"\n"
"   -g SN ST NB NA TC\n"
"      Specifies parameters used in the Gaussan mapping algorithm.\n"
"        SN  value for sigma norm.\n"
"        ST  value for sigma tang.\n"
"        NB  value for norm below cutoff.\n"
"        NA  value for norm above cutoff.\n"
"        TC  value for tang cutoff.\n"
"\n"
"   -h\n"
"      Display help for this program.\n"
"\n"
"   -help\n"
"      Display help for this program.\n"
"\n"
"   -m metric-file-name(s)\n"
"      Specify the names for the output metric files, one per surface.  If\n"
"      there are fewer metric names than surfaces, the names of metric files\n"
"      for some surfaces will be the default names.  You must specify this\n"
"      option prior to any surfaces specified with the \"-s\" option.\n"
"\n"
"   -nogui\n"
"      Execute the mapper without launching the user interface.  This\n"
"      requires that you specify both volume and surface files.\n"
"\n"
"   -nl number\n"
"      Specifies the neighbor layers parameter used by the Gaussian,\n"
"      Average Voxel, and Maximum Voxel algorithms.\n"
"\n"
"   -p params-file-name\n"
"      Specify the params file containing the cropping and padding values\n"
"      used when mapping fMRI data to an individual's surface.\n"
"\n"
"   -s surface(s)\n"
"      Specifies one or more individual surface files.\n"
"\n"
"   -so XYZ\n"
"      Specifies the orientation of the indiv surface where \"X\" is either \n"
"      L (left) or R (right), \"Y\" is one of A (anterior) or \n"
"      P (posterior) and \"Z\" is one of I (inferior) or S (superior).\n"
"\n"
"   -sp spec-file-name(s)\n"
"      Specify the names for the output spec files, one per surface.  If\n"
"      there are fewer spec file names that surfaces, the names of spec files\n"
"      for some surfaces will be the default names.  You must specify this\n"
"      option prior to any surfaces specified with the \"-s\" option.\n"
"\n"
"   -st surface-type\n"
"      Specifies the surface type  Valid values are:\n"
"         VTK  Specifies the surfaces are VTK PolyData files.\n"
"\n"
"   -v volume(s)\n"
"      Specifies one or more volume files.  The volume type option \"-vt\"\n"
"      must be used prior to specifying volumes with this option.\n"
"\n"
"   -vo XYZ\n"
"      Specifies the orientation of the volume where \"X\" is either \n"
"      L (left) or R (right), \"Y\" is one of A (anterior) or \n"
"      P (posterior) and \"Z\" is one of I (inferior) or S (superior).\n"
"\n"
"   -vt volume-type\n"
"      Specifies the volume type.  This option must be specified prior\n"
"      to any volumes.  Valid values are:\n"
"         AFNI    Specifies the volumes are AFNI volumes.\n"
"         ANALYZE Specifies the volumes are Analyze volumes.\n"
"         SPM     Specifies the volumes are SPM volumes in LPI Orientation (SPM99).\n"
"         SPM_LPI Specifies the volumes are SPM volumes in LPI Orientation (SPM2).\n"
"         SPM_RPI Specifies the volumes are SPM volumes in RPI Orientation (SPM2).\n"
"         WUNIL   Specifies the volumes are Wash U. NeuroImaging Laboratory volumes.\n"
"         R8S     Specifies the volumes are Raw 8-bit signed volumes.\n"
"         R8U     Specifies the volumes are Raw 8-bit unsigned volumes.\n"
"         R16S    Specifies the volumes are Raw 16-bit signed volumes.\n"
"         R16U    Specifies the volumes are Raw 16-bit unsigned volumes.\n"
"         R32S    Specifies the volumes are Raw 32-bit signed volumes.\n"
"         R32U    Specifies the volumes are Raw 32-bit unsigned volumes.\n"
"         R64S    Specifies the volumes are Raw 64-bit signed volumes.\n"
"         R64U    Specifies the volumes are Raw 64-bit unsigned volumes.\n"
"         R32F    Specifies the volumes are Raw 32-bit floating point volumes.\n"
"         R64R    Specifies the volumes are Raw 64-bit floating point volumes.\n"
"\n"
"   EXAMPLES\n"
"\n";

}

/*
 * Process user's command line options
 * Returns:  -1  -  Error parsing options
 *            0  -  No errors
 *            1  -  No errors and execute mapper without a GUI
 */
static int
processOptions(int argc, char* argv[])
{
   std::cout << std::endl
             << "INFO: To see command line options, Run with \"-help\""
             << std::endl << std::endl;
   
   bool gotVolumeType = false;
   bool errorFlag = false;
   bool runWithoutGUIFlag = false;
   bool haveVolumeFiles = false;
   bool haveSurfaceFiles = false;
      
   for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-a") == 0) {
         i++;
         if (i < argc) {
            if (strcmp(argv[i], "AN") == 0) {
               fmriMapper.setMappingAlgorithm(FMRIDataMapper::ALGORITHM_AVERAGE_NODES);
            }
            else if (strcmp(argv[i], "GS") == 0) {
               fmriMapper.setMappingAlgorithm(FMRIDataMapper::ALGORITHM_GAUSSIAN);
            }
            else if (strcmp(argv[i], "AV") == 0) {
               fmriMapper.setMappingAlgorithm(FMRIDataMapper::ALGORITHM_AVERAGE_VOXEL);
            }
            else if (strcmp(argv[i], "MV") == 0) {
               fmriMapper.setMappingAlgorithm(FMRIDataMapper::ALGORITHM_MAX_VOXEL);
            }
            else if (strcmp(argv[i], "MB") == 0) {
               fmriMapper.setMappingAlgorithm(FMRIDataMapper::ALGORITHM_MCW_BRAINFISH);
            }
            else {
               std::cout << "ERROR: invalid type = " << argv[i] 
                         << " for algorithm \"-a\" option." << std::endl;
               errorFlag = true;
            }
         }
         else {
            std::cout << "ERROR: missing algorithm for \"-a\" option." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-b") == 0) {
         i++;
         if (i < argc) {
            bool byteSwap = false;
            if ((argv[i][0] == 'Y') || (argv[i][0] == 'n')) {
               byteSwap = true;
            }
            else if ((argv[i][0] == 'N') || (argv[i][0] == 'n')) {
               byteSwap = false;
            }
            else {
               std::cout << "ERROR: Invalid value " << argv[i] 
                         << " for \"-b\" byte swapping option" << std::endl;
               errorFlag = true;
            }
            fmriMapper.setVolumeByteSwapping(byteSwap);
         }
         else {
            std::cout << "ERROR: Missing value for \"-b\" byte swapping option." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-bf") == 0) {
         if ((i + 2) < argc) {
            i++;
            const float maxDist = atof(argv[i]);
            i++;
            const int   splat = atoi(argv[i]);
            fmriMapper.setBrainfishParameters(maxDist, splat);
         }
         else {
            std::cout << "ERROR: Missing arguments for \"-bf\" option.  Two "
                   "numbers should follow it." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-d") == 0) {
         if ((i + 3) < argc) {
            i++;
            const int x = atoi(argv[i]);
            i++;
            const int y = atoi(argv[i]);
            i++;
            const int z = atoi(argv[i]);
            fmriMapper.setRawVolumeDimensions(x, y, z);
         }
         else {
            std::cout << "ERROR: Missing arguments for \"-d\" option.  Three "
                   "numbers should follow it." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-g") == 0) {
         if ((i + 5) < argc) {
            fmriMapper.setGaussianParameters(atof(argv[i+1]),
                                         atof(argv[i+2]),
                                         atof(argv[i+3]),
                                         atof(argv[i+4]),
                                         atof(argv[i+5]));
            i += 5;
         }
         else {
            std::cout << "ERROR: missing value(s) for gaussian parameters \"-g\" "
                   "option." << std::endl;
         }
      }
      else if (strcmp(argv[i], "-h") == 0) {
         usage();
         exit(0);
      }
      else if (strcmp(argv[i], "-help") == 0) {
         usage();
         exit(0);
      }
      else if (strcmp(argv[i], "-m") == 0) {
         if (haveSurfaceFiles) {
            std::cout << "ERROR: The output metric file(s) naming option \"-m\" "
                   "must be specified prior to any surfaces" << std::endl;
            errorFlag = true;
         }
         
         std::vector<std::string> metricNames;
         i++;
         while (i < argc) {
            if (argv[i][0] == '-') {
               i--;
               break;
            }
            else {
               metricNames.push_back(argv[i]);
            }
            i++;
         }
         if (metricNames.size() > 0) {
            fmriMapper.setDefaultMetricFileNames(metricNames);
         }
      }
      else if (strcmp(argv[i], "-nogui") == 0) {
         runWithoutGUIFlag = true;
      }
      else if (strcmp(argv[i], "-nl") == 0) {
         i++;
         if (i < argc) {
            fmriMapper.setNeighborLayers(atoi(argv[i]));
         }
         else {
            std::cout << "ERROR: missing value for neighbor layers \"-nl\" option." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-p") == 0) {
         i++;
         if (i < argc) {
            errorFlag = readParamsFile(argv[i]);
            fmriMapper.setMappingSurfaceType(FMRIDataMapper::MAPPING_SURFACE_INDIVIDUAL);
         }
         else {
            std::cout << "ERROR: missing name of params file in \"-p\" option." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-s") == 0) {
         int numSurfaces = 0;
         i++;
         while (i < argc) {
            if (argv[i][0] == '-') {
               i--;
               break;
            }
            else {
               fmriMapper.addSurface("", argv[i], argv[i]);
               haveSurfaceFiles = true;
               numSurfaces++;
               i++;
            }
         }
         if (numSurfaces == 0) {
            std::cout << "ERROR: missing surfaces after \"-s\" option." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-so") == 0) {
         i++;
         if (i < argc) {
            if (strlen(argv[i]) > 2) {
               FMRIDataMapper::X_NEGATIVE_ORIENTATION_TYPE orientationX =
                               FMRIDataMapper::X_NEGATIVE_ORIENTATION_LEFT;
               FMRIDataMapper::Y_NEGATIVE_ORIENTATION_TYPE orientationY =
                               FMRIDataMapper::Y_NEGATIVE_ORIENTATION_POSTERIOR;
               FMRIDataMapper::Z_NEGATIVE_ORIENTATION_TYPE orientationZ =
                               FMRIDataMapper::Z_NEGATIVE_ORIENTATION_INFERIOR;
               if (argv[i][0] == 'L') {
                  orientationX = FMRIDataMapper::X_NEGATIVE_ORIENTATION_LEFT;
               }
               else if (argv[i][0] == 'R') {
                  orientationX = FMRIDataMapper::X_NEGATIVE_ORIENTATION_RIGHT;
               }
               if (argv[i][1] == 'A') {
                  orientationY = FMRIDataMapper::Y_NEGATIVE_ORIENTATION_ANTERIOR;
               }
               else if (argv[i][1] == 'P') {
                  orientationY = FMRIDataMapper::Y_NEGATIVE_ORIENTATION_POSTERIOR;
               }
               if (argv[i][2] == 'I') {
                  orientationZ = FMRIDataMapper::Z_NEGATIVE_ORIENTATION_INFERIOR;
               }
               else if (argv[i][2] == 'S') {
                  orientationZ = FMRIDataMapper::Z_NEGATIVE_ORIENTATION_SUPERIOR;
               }
               fmriMapper.setIndivSurfaceOrientation(orientationX, orientationY, 
                                                 orientationZ);
           }
         }
      }
      else if (strcmp(argv[i], "-sp") == 0) {
         if (haveSurfaceFiles) {
            std::cout << "ERROR: The output spec file(s) naming option \"-sp\" "
                   "must be specified prior to any surfaces." << std::endl;
            errorFlag = true;
         }
         
         std::vector<std::string> specNames;
         i++;
         while (i < argc) {
            if (argv[i][0] == '-') {
               i--;
               break;
            }
            else {
               specNames.push_back(argv[i]);
            }
            i++;
         }
         if (specNames.size() > 0) {
            fmriMapper.setDefaultSpecFileNames(specNames);
         }
      }
      else if (strcmp(argv[i], "-st") == 0) {
         i++;
         if (i < argc) {
            if (strcmp(argv[i], "VTK") == 0) {
               fmriMapper.setSurfaceFileType(
                              FMRIDataMapper::SURFACE_FILE_TYPE_VTK_POLYDATA);
            }
            else {
               std::cout << "ERROR: invalid type = " << argv[i] << " for surface type \"-st\" "
                      "option." << std::endl;
               errorFlag = true;
            }
         }
         else {
            std::cout << "ERROR: missing type for surface type \"-st\" option." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-v") == 0) {
         int numVolumes = 0;
         i++;
         while (i < argc) {
            if (argv[i][0] == '-') {
               i--;
               break;
            }
            else {
               std::string msg;
               if (fmriMapper.addVolume(argv[i], msg)) {
                  std::cout << "ERROR: " << msg << std::endl;
                  errorFlag = true;
               }
               else {
                  numVolumes++;
                  haveVolumeFiles = true;
                  i++;
               }
               if (gotVolumeType == false) {
                  std::cout << "ERROR: You must specifiy the volume type with the "
                         "\"-vt\" option prior to specifying any volumes." << std::endl;
                  errorFlag = true;
               }
            }
         }
         if (numVolumes == 0) {
            std::cout << "ERROR: missing volumes after \"-v\" option." << std::endl;
            errorFlag = true;
         }
      }
      else if (strcmp(argv[i], "-vo") == 0) {
         i++;
         if (i < argc) {
            if (strlen(argv[i]) > 2) {
               FMRIDataMapper::X_NEGATIVE_ORIENTATION_TYPE orientationX =
                               FMRIDataMapper::X_NEGATIVE_ORIENTATION_LEFT;
               FMRIDataMapper::Y_NEGATIVE_ORIENTATION_TYPE orientationY =
                               FMRIDataMapper::Y_NEGATIVE_ORIENTATION_POSTERIOR;
               FMRIDataMapper::Z_NEGATIVE_ORIENTATION_TYPE orientationZ =
                               FMRIDataMapper::Z_NEGATIVE_ORIENTATION_INFERIOR;
               if (argv[i][0] == 'L') {
                  orientationX = FMRIDataMapper::X_NEGATIVE_ORIENTATION_LEFT;
               }
               else if (argv[i][0] == 'R') {
                  orientationX = FMRIDataMapper::X_NEGATIVE_ORIENTATION_RIGHT;
               }
               if (argv[i][1] == 'A') {
                  orientationY = FMRIDataMapper::Y_NEGATIVE_ORIENTATION_ANTERIOR;
               }
               else if (argv[i][1] == 'P') {
                  orientationY = FMRIDataMapper::Y_NEGATIVE_ORIENTATION_POSTERIOR;
               }
               if (argv[i][2] == 'I') {
                  orientationZ = FMRIDataMapper::Z_NEGATIVE_ORIENTATION_INFERIOR;
               }
               else if (argv[i][2] == 'S') {
                  orientationZ = FMRIDataMapper::Z_NEGATIVE_ORIENTATION_SUPERIOR;
               }
               fmriMapper.setVolumeOrientation(orientationX, orientationY, 
                                                 orientationZ);
           }
         }
      }
      else if (strcmp(argv[i], "-vt") == 0) {
         i++;
         if (i < argc) {
            gotVolumeType = true;
            if (strcmp(argv[i], "AFNI") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_AFNI);
            }
            else if (strcmp(argv[i], "ANALYZE") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_ANALYZE);
            }
            else if (strcmp(argv[i], "SPM") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_SPM_LPI);
            }
            else if (strcmp(argv[i], "SPM_LPI") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_SPM_LPI);
            }
            else if (strcmp(argv[i], "SPM_RPI") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_SPM_RPI);
            }
            else if (strcmp(argv[i], "WUNIL") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_WUNIL);
            }
            else if (strcmp(argv[i], "R8S") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_8_BIT_SIGNED);
            }
            else if (strcmp(argv[i], "R8U") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_8_BIT_UNSIGNED);
            }
            else if (strcmp(argv[i], "R16S") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_16_BIT_SIGNED);
            }
            else if (strcmp(argv[i], "R16U") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_16_BIT_UNSIGNED);
            }
            else if (strcmp(argv[i], "R32S") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_SIGNED);
            }
            else if (strcmp(argv[i], "R32U") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_UNSIGNED);
            }
            else if (strcmp(argv[i], "R64S") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_SIGNED);
            }
            else if (strcmp(argv[i], "R64U") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_UNSIGNED);
            }
            else if (strcmp(argv[i], "R32F") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_32_BIT_FLOATING_POINT);
            }
            else if (strcmp(argv[i], "R64F") == 0) {
               fmriMapper.setVolumeType(FMRIDataMapper::VOLUME_TYPE_RAW_64_BIT_FLOATING_POINT);
            }
            else {
               std::cout << "ERROR: invalid volume type = " << argv[i] 
                         << " for \"-vt\" option." << std::endl;
               errorFlag = true;
            }
         }
         else {
            std::cout << "ERROR: missing volume type after \"-vt\" option." << std::endl;
            errorFlag = true;
         }
      }
      else {
         std::cout << "ERROR: processing option " << argv[i] << std::endl;
         errorFlag = true;
      }
   }
   
   if (errorFlag) {
      return -1;
   }
   if (runWithoutGUIFlag) {
      if (haveVolumeFiles && haveSurfaceFiles) {
         return 1;
      }
      else {
         std::cout << "ERROR: surface and volume files must be specified to run "
                "the mapper without a user interface." << std::endl;
      }
   }
   return 0;
}

/*
 * THE program's main.
 */
int
main(int argc, char* argv[])
{
   std::cout << "Set the environment variables MAP_FMRI_DEBUG for debugging information." << std::endl;
   DebugControl::setDebugOnWithEnvironmentVariable("MAP_FMRI_DEBUG");
   
   initializeStatic();
   
   //
   // Use native space for mapping
   //
   VolumeFile::setVolumeSpace(VolumeFile::VOLUME_SPACE_VOXEL_NATIVE);
   
   fmriMapper.loadUserPreferences();
   fmriMapper.setErrorMessageCallback(displayErrorMessage);

   readAtlasesFromEnvironment();
   
   const int processOptionsResult = processOptions(argc, argv);
   if (processOptionsResult > 0) {
      if (fmriMapper.execute()) {
         displayErrorMessage("Errors occurred while running the mapper.");
         exit(-1);
      }
      else {
         displaySuccessMessage();
         exit(0);
      }
   }
   else if (processOptionsResult < 0) {
      exit(-1);
   }
   
   haveGUI = true;
   
   QApplication app(argc, argv);
   
   //
   // Create and display the main window.
   //
   theMainWindow = new GuiMapFmriMainWindow();
   app.setMainWidget(theMainWindow);
   theMainWindow->show();
   app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
   return app.exec();
}

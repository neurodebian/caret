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
#include <QApplication>
#include <QDateTime>

#include "BrainModelVolumeToSurfaceMetricMapper.h"
#include "BrainSet.h"
#include "CommandLineUtilities.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "PreferencesFile.h"

static BrainModelVolumeToSurfaceMetricMapper::ALGORITHM algorithm =
          BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_ENCLOSING_VOXEL;

static float averageVoxelNeighbors = 1.0;

static float brainFishMaxDistance = 1.0;
static int   brainFishSplatFactor = 1;

static float gaussNeighbors = 1.0;
static float gaussSigmaNorm = 1.0;
static float gaussSigmaTang = 1.0;
static float gaussNormBelow = 1.0;
static float gaussNormAbove = 1.0;
static float gaussTang      = 1.0;

static float maximumVoxelNeighbors = 1.0;

static QString metricFileName;

static QString specFileName;
static QString topoFileName;
static QString coordFileName;

static std::vector<QString> volumeFileNames;

const QString algorithmAverageNodesName("AVERAGE_NODES");
const QString algorithmAverageVoxelName("AVERAGE_VOXEL");
const QString algorithmEnclosingVoxelName("ENCLOSING_VOXEL");
const QString algorithmGaussianName("GAUSSIAN");
const QString algorithmInterpolatedVoxelName("INTERPOLATED_VOXEL");
const QString algorithmMaximumVoxelName("MAXIMUM_VOXEL");
const QString algorithmMcwBrainFishName("MCW_BRAIN_FISH");
   
/**
 * Initialize the algorithm parameters
 */
static void
initializeAlgorithmParameters()
{
   BrainSet* bs = new BrainSet(true);
   PreferencesFile* pf = bs->getPreferencesFile();
   const PreferencesFile::FmriAlgorithm* algParam = pf->getFmriAlgorithmParameters();
   
   const QString algName = algParam->getAlgorithmName();
   if (algName == algorithmAverageNodesName) {
      algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_AVERAGE_NODES;
   }
   else if (algName == algorithmAverageVoxelName) {
      algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_AVERAGE_VOXEL;
   }
   else if (algName == algorithmEnclosingVoxelName) {
      algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_ENCLOSING_VOXEL;
   }
   else if (algName == algorithmGaussianName) {
      algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_GAUSSIAN;
   }
   else if (algName == algorithmInterpolatedVoxelName) {
      algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_INTERPOLATED_VOXEL;
   }
   else if (algName == algorithmMaximumVoxelName) {
      algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_MAXIMUM_VOXEL;
   }
   else if (algName == algorithmMcwBrainFishName) {
      algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_MCW_BRAINFISH;
   }
   
   algParam->getAverageVoxelParameters(averageVoxelNeighbors);
   algParam->getMaximumVoxelParameters(maximumVoxelNeighbors);
   algParam->getGaussianParameters(gaussNeighbors,
                             gaussSigmaTang,
                             gaussSigmaNorm,
                             gaussNormBelow,
                             gaussNormAbove,
                             gaussTang);
   algParam->getBrainFishParameters(brainFishMaxDistance, 
                              brainFishSplatFactor);
                              
   delete bs;
}

/**
 * Unexpected handler
 */
static void 
unexpectedHandler()
{
  std::cout << "WARNING: unhandled exception." << std::endl;
}

/**
 * New handler
 */
static void 
newHandler()
{
  std::cout << "WARNING: unhandled new.  Program may crash." << std::endl;
}

static void
printHelpInformation(const char* progNameIn)
{
   QString algName;
   switch (algorithm) {
      case BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_AVERAGE_NODES:
         algName = "AN";
         break;
      case BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_AVERAGE_VOXEL:
         algName = "AV";
         break;
      case BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_ENCLOSING_VOXEL:
         algName = "EV";
         break;
      case BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_GAUSSIAN:
         algName = "GS";
         break;
      case BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_INTERPOLATED_VOXEL:
         algName = "IV";
         break;
      case BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_MAXIMUM_VOXEL:
         algName = "MV";
         break;
      case BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_MCW_BRAINFISH:
         algName = "MB";
         break;
   }
   
   const QString programName(FileUtilities::basename(progNameIn));
   
   std::cout
      << "" << std::endl
      << "NAME" << std::endl
      << programName.toAscii().constData() << " - Map functional volume(s) to surface(s)" << std::endl
      << "" << std::endl
      << "DESCRIPTION" << std::endl
      << "   This program maps function volume(s) to nodes in one surface" << std::endl
      << "   The output is placed in a metric file." << std::endl
      << "" << std::endl
      << "OPTIONS" << std::endl
      << "" << std::endl
      << "   -a algorithm"<< std::endl
      << "      Specifies the algorithm for mapping voxels to nodes.  Valid values are:" << std::endl
      << "         AN  Use the Average Nodes algorithm" << std::endl
      << "         AV  Use the Average Voxel algorithm" << std::endl
      << "         EV  Use the Enclosing Voxel algorithm" << std::endl
      << "         GS  Use the Gaussian algorithm" << std::endl
      << "         IV  Use the Interpolated Voxel algorithm" << std::endl
      << "         MV  Use the Maximum Voxel algorithm" << std::endl
      << "         MB  Use the MCW Brainfish algorithm" << std::endl
      << "      Current Value: " << algName.toAscii().constData() << std::endl
      << "" << std::endl
      << "   -av NC" << std::endl
      << "      Specifies parameters used in the Average Voxel mapping algorithm." << std::endl
      << "        NC  neighbor cube size (mm)."<< std::endl
      << "      Current Value: " << averageVoxelNeighbors << std::endl
      << "" << std::endl
      << "   -bf MD SF" << std::endl
      << "      Specifies the MCW Brainfish algorithm parameters." << std::endl
      << "        MD value for max distance (float)." << std::endl
      << "        SF value for splat factor (integer)." << std::endl
      << "      Current Value: " << brainFishMaxDistance << " " 
                                 << brainFishSplatFactor << std::endl
      << "" << std::endl
      << "   -g NC SN ST NB NA TC" << std::endl
      << "      Specifies parameters used in the Gaussan mapping algorithm." << std::endl
      << "        NC  neighbor cube size (mm)."<< std::endl
      << "        SN  value for sigma norm." << std::endl
      << "        ST  value for sigma tang." << std::endl
      << "        NB  value for norm below cutoff (mm)." << std::endl
      << "        NA  value for norm above cutoff (mm)." << std::endl
      << "        TC  value for tang cutoff  (mm)." << std::endl
      << "      Current Value: " << gaussNeighbors << " "
                                 << gaussSigmaNorm << " "
                                 << gaussSigmaTang << " "
                                 << gaussNormBelow << " "
                                 << gaussNormAbove << " "
                                 << gaussTang << std::endl
      << "" << std::endl
      << "   -mv NC" << std::endl
      << "      Specifies parameters used in the Maximum Voxel mapping algorithm." << std::endl
      << "        NC  neighbor cube size (mm)."<< std::endl
      << "      Current Value: " << maximumVoxelNeighbors << std::endl
      << "" << std::endl
      << "   -m metric-file-name" << std::endl
      << "      Specified the name of the metric file in which the mapped functional" << std::endl
      << "      data will be placed.  If the metric file already exists new data" << std::endl
      << "      will be appended to it.  If the file does not exist, it will be" << std::endl
      << "      created." << std::endl
      << "" << std::endl
      << "   -s  spec-file-name  topo-file-name  coord-file-name" << std::endl
      << "      Specifies the spec file of the individual and the topology." << std::endl
      << "      and coordinate files for mapping the functional data." << std::endl
      << "" << std::endl
      << "   -v  volume-file-name" << std::endl
      << "      Specifies the name of a functional volume that is to be mapped" << std::endl
      << "      to the surface.  More than one functional volume name may be " << std::endl
      << "      specified, however, each must be preceded by \"-v\"." << std::endl
      << "" << std::endl
      << std::endl;
}

static void
processCommandLineOptions(int argc, char* argv[])
{
   for (int i = 1; i < argc; i++) {
      const QString arg(argv[i]);
      
      if (arg == "-a") {
         QString algName;
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                algName);
         if (algName == "AN") {
            algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_AVERAGE_NODES;
         }
         else if (algName == "AV") {
            algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_AVERAGE_VOXEL;
         }
         else if (algName == "EV") {
            algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_ENCLOSING_VOXEL;
         }
         else if (algName == "GS") {
            algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_GAUSSIAN;
         }
         else if (algName == "IV") {
            algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_INTERPOLATED_VOXEL;
         }
         else if (algName == "MV") {
            algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_MAXIMUM_VOXEL;
         }
         else if (algName == "MB") {
            algorithm = BrainModelVolumeToSurfaceMetricMapper::ALGORITHM_MCW_BRAINFISH;
         }
         else {
            std::cout << "ERROR: Invalid algorithm: \"" << algName.toAscii().constData() << "\"" << std::endl;
            exit(-1);
         }
      }
      else if (arg == "-av") {
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                averageVoxelNeighbors);
      }
      else if (arg == "bf") {
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                brainFishMaxDistance);
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                brainFishSplatFactor);
      }
      else if (arg == "-g") {
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                gaussNeighbors);
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                gaussSigmaNorm);
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                gaussSigmaTang);
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                gaussNormBelow);
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                gaussNormAbove);
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                gaussTang);
      }
      else if ((arg == "-h") ||
          (arg == "-help")) {
         printHelpInformation(argv[0]);
         exit(0);
      }
      else if (arg == "-mv") {
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                maximumVoxelNeighbors);
      }
      else if (arg == "-m") {
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                metricFileName);
      }
      else if (arg == "-s") {
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                specFileName);
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                topoFileName);
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                coordFileName);
      }
      else if (arg == "-v") {
         QString name;
         CommandLineUtilities::getNextParameter(arg,
                                                argc,
                                                argv,
                                                true,
                                                i,
                                                name);
         volumeFileNames.push_back(name);
      }
   }
}

static void
runCommandLineMapper()
{
   QString errorMessage;
   
   if (volumeFileNames.empty()) {
      errorMessage.append("ERROR: No volume files specified.\n");
   }
   if (specFileName.isEmpty()) {
      errorMessage.append("ERROR: No spec file name specified.\n");
   }
   if (topoFileName.isEmpty()) {
      errorMessage.append("ERROR: No topology file name specified.\n");
   }
   if (coordFileName.isEmpty()) {
      errorMessage.append("ERROR: No coordinate file name specified.\n");
   }
   if (metricFileName.isEmpty()) {
      errorMessage.append("ERROR: No metric file name specified.\n");
   }
   
   if (errorMessage.isEmpty() == false) {
      std::cout << errorMessage.toAscii().constData();
      exit(-1);
   }
   
   //
   // Create a spec file
   //
   SpecFile sf;
   
   //
   // Read spec file
   //
   try {
      sf.readFile(specFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR reading spec file: " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Set the closed topology and fiducial coord file
   //
   sf.setAllFileSelections(SpecFile::SPEC_FALSE);
   errorMessage = sf.setFilesForMetricMapping(topoFileName, 
                                              coordFileName, 
                                              metricFileName,
                                              sf.getStructure());
   if (errorMessage.isEmpty() == false) {
      std::cout << "ERROR setting files in spec file: " << errorMessage.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Create a brain set
   //
   BrainSet* bs = new BrainSet;
   bs->initializeDataFileStaticMembers();
   
   //
   // Read the spec file's data files
   //
   std::vector<QString> errorMessages;
   bs->readSpecFile(BrainSet::SPEC_FILE_READ_MODE_NORMAL,
                    sf, sf.getFileName(), errorMessages, NULL, NULL);
   if (errorMessages.empty()) {
      //
      // Find the surface
      //
      if (bs->getNumberOfBrainModels() <= 0) {
         QString msg("ERROR: No brain models found for ");
         msg.append(specFileName);
         std::cout << msg.toAscii().constData() << std::endl;
         exit(-1);
      }
      BrainModelSurface* bms = bs->getBrainModelSurface(0);
      if (bms == NULL) {
         QString msg("ERROR: Problems finding surface for ");
         msg.append(specFileName);
         std::cout << msg.toAscii().constData() << std::endl;
         exit(-1);
      }
      else {
         //
         // Map all of the volume files
         //
         for (int i = 0; i < static_cast<int>(volumeFileNames.size()); i++) {
            //
            // Read the volume file
            //
            VolumeFile vf;
            try {
               vf.readFile(volumeFileNames[i]);
            }
            catch (FileException& e) {
               std::cout << "ERROR: " << e.whatQString().toAscii().constData() << std::endl;
               exit(-1);
            }
            
            //
            // Create the mapper
            //
            BrainModelVolumeToSurfaceMetricMapper mapper(bs,
                                                         algorithm,
                                                         bms,
                                                         &vf,
                                                         bs->getMetricFile(),
                                                         -1,
                                          FileUtilities::basename(volumeFileNames[i]));
            
            //
            // Set the parameters
            //
            mapper.setAlgorithmAverageVoxelParameters(averageVoxelNeighbors);
            mapper.setAlgorithmMaximumVoxelParameters(maximumVoxelNeighbors);
            mapper.setAlgorithmGaussianParameters(gaussNeighbors,
                                                  gaussSigmaNorm,
                                                  gaussSigmaTang,
                                                  gaussNormBelow,
                                                  gaussNormAbove,
                                                  gaussTang);
            mapper.setAlgorithmMcwBrainFishParameters(brainFishMaxDistance,
                                                      brainFishSplatFactor);
            
            //
            // Run the mapper
            //
            try {
               mapper.execute();
            }
            catch (BrainModelAlgorithmException& e) {
               QString msg("ERROR while mapping: ");
               msg.append(FileUtilities::basename(volumeFileNames[i]));
               msg.append(e.whatQString());
               std::cout << msg.toAscii().constData() << std::endl;
               exit(-1);
            }
         }
         
         //
         // Save the metric file
         //
         try {
            bs->writeMetricFile(metricFileName);
         }
         catch (FileException& e) {
            std::cout << "ERROR: " << e.whatQString().toAscii().constData()  << std::endl;
            exit(-1);
         }
      }
   }
   else {
      QString message;
      for (int i = 0; i < static_cast<int>(errorMessages.size()); i++) {
         message.append(errorMessages[i]);
         message.append("\n");
      }
      std::cout << message.toAscii().constData() << std::endl;
   }
   
   delete bs;
   
   std::cout << "INFO: Mapping of data successful." << std::endl;
}

int
main(int argc, char* argv[])
{
   std::cout << std::endl;
   std::cout << "INFO: To see command line options, Run with \"-help\""
             << std::endl;
   std::cout << std::endl;

#ifdef Q_OS_MAC
   BrainSet::initializeDataFileStaticMembers();
#endif

   std::set_unexpected(unexpectedHandler);

   std::set_new_handler(newHandler);

   QApplication app(argc, argv, false);
   
   initializeAlgorithmParameters();

   processCommandLineOptions(argc, argv);
   
   if (getenv("CARET_TEST") != NULL) {
      DebugControl::setTestFlag(true);
   }
   if (getenv("CARET_DEBUG") != NULL) {
      DebugControl::setDebugOn(true);
   }

   runCommandLineMapper();
}


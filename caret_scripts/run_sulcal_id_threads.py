#!/usr/bin/python
#
# Run sulcal identification on buckner case 01-12 left and right
#

caretCommand = "C:/caret/bin/caret_command"
caretCommand = "/Users/john/caret5_development/caret5_cpp/caret_source/caret_command/caret_command"

import os
import re
import time
import subprocess
import sys
from threading import Thread

##-----------------------------------------------------------------------------
#
# Run a command
#
def runCommand(cmd) :
   print "\nExecuting: %s\n" % cmd
   result = os.system(cmd)
   if (result != 0) :
      print "ERROR COMMAND FAILED: "
      print "   ", cmd

##-----------------------------------------------------------------------------
#
# Add file to spec file
#
def addToSpecFile(specFileName, specFileTag, fileName) :
   cmdList = (
      caretCommand,
      "-spec-file-add",
      specFileName,
      specFileTag,
      fileName)
   runCommand(cmdList)
   
##-----------------------------------------------------------------------------
#
# Class that runs a command in a thread.
# If the command sent to the constructor is a list, each item in the list
# is executed as a command sequentially
#
class RunCommandInThread(Thread):
   #
   # Constructor
   # "commandIn" may be either a command or a list containing commands
   #
   def __init__ (self, commandIn):
      Thread.__init__(self)
      
      #
      # If input is a string, place it into a list
      # If input is a list, it's ok
      #
      if isinstance(commandIn, str):
         self.commandList = [commandIn]
      elif isinstance(commandIn, list):
         self.commandList = commandIn
      else:
         self.commandList = []
         
      #print "RunCommandInThread commandList:", self.commandList
      #print "RunCommandInThread commandIn:", commandIn
      #print "RunCommandInThread type(commandIn):", type(commandIn)
      
      
   def run(self):
      if len(self.commandList) == 0:
         print "ERROR COMMAND LIST EMPTY"
         return
         
      for cmd in self.commandList:
         p = subprocess.Popen(cmd, shell=True)
         result = p.wait()
         #result = os.waitpid(p.pid, 0)
         if (result != 0) :
            print "ERROR COMMAND FAILED: "
            print "   ", cmd

##-----------------------------------------------------------------------------
#
# Run a a list of commands in multiple threads
#
class RunCommandsInMultipleThreads(Thread):
   def __init__(self, commandList, numberOfThreads):
      Thread.__init__(self)
      self.commandList = commandList
      self.maximumNumberOfThreads = numberOfThreads
      
   def run(self):
      if len(self.commandList) == 0:
         print "ERROR: List of commands is empty"
         return
      
      numberOfCommandsToRun = len(self.commandList)
      
      i = 0
      while i < numberOfCommandsToRun: 
         #
         # Determine how many threads to create
         #
         numThreadsToCreate = self.maximumNumberOfThreads
         numCommandsLeft = numberOfCommandsToRun - i
         if (numCommandsLeft < numThreadsToCreate) :
            numThreadsToCreate = numCommandsLeft
            
         #
         # Get directories that are to be executed
         #
         commandsToRunList = []
         j = 0
         while j < numThreadsToCreate:
            commandsToRunList.append(self.commandList[i])
            i = i + 1
            j = j + 1
         
         #
         # Execute the directories
         #
         threadList = []
         for commandToRun in commandsToRunList :
            print "STARTING: ", commandToRun
            cmdThread = RunCommandInThread(commandToRun)
            threadList.append(cmdThread)
            cmdThread.start()
            
         for thread in threadList:
            thread.join()
      
##-----------------------------------------------------------------------------
#
# Build add file to spec file command
#
def buildAddToSpecFileCommand(directoryName, specFileName, specFileTag, fileName) :
   #if (os.path.exists(fileName)) :
      cmdList = (
         caretCommand,
         "-spec-file-add",
         "-CHDIR",
         directoryName,
         specFileName,
         specFileTag,
         fileName)
   
      commandText = " ".join(cmdList)
      return commandText


##-----------------------------------------------------------------------------
#
# Create an image of the first scene
#
def buildCreateSceneImageCommand(directoryName, sceneNumberInString) :
   #
   # Get files in directory
   #
   files = os.listdir(directoryName)

   #
   # Find the fiducial coord file to get case, hemisphere nodes
   #
   case = ""
   hem  = ""
   nodes = ""
#   for filename in files:
#      if (filename.find("Fiducial") >= 0) :
#         #print "Fiducial", filename
#         m = re.match(r"Human\.Buckner(Case\d\d)\.([L|R])\.Fiducial\.(\d+)\.coord",
#                      filename)
#         
#         case  = m.group(1)
#         hem   = m.group(2)
#         nodes = m.group(3)
#         
#         break
   #
   # Find the fiducial coord file to get case, hemisphere nodes
   #
   case = ""
   hem  = ""
   nodes = ""
   for filename in files:
      if (filename.find("Human.BucknerCase") >= 0) :
         if (filename.find(".spec") >= 0) :
            #print "SpecFile", filename
            m = re.match("Human.Buckner(Case\d\d)\.([L|R])\.spec",
                         filename)
            case  = m.group(1)
            hem   = m.group(2)
            break
   
   #print "case  ", case
   #print "hem   ", hem
   #print "nodes ", nodes

   #
   # Put together the data file names
   #
   sceneFileName = "Human.BUCKNER" + case + "." + hem + ".Landmarks.scene"
   specFileName = "Human.Buckner" + case + "." + hem + ".spec"
   imageFileName = "Landmarks." + case + "." + hem + ".jpg"
   
   #
   # Scene
   #
   idCmdList = (
      caretCommand,
      "-show-scene",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      specFileName,
      sceneFileName,
      sceneNumberInString,
      "-image-file",
         imageFileName,
         "3"
   )
   
   commandList = [" ".join(idCmdList)]


   #
   # Add text to image
   #
   idCmdList = (
      caretCommand,
      "-image-insert-text",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      imageFileName,
      imageFileName,
      "20",
      "20",
      "255",
      "0",
      "0",
      case)
   commandList.append(" ".join(idCmdList))
   
   return commandList
   
##-----------------------------------------------------------------------------
#
# Assemble images command.
#
def buildAssembleImagesCommand(compositeImageFileName, allDirectoryNames) :

   #
   # Command
   #
   idCmdList = (
      caretCommand,
      "-image-combine",
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      "1",
      compositeImageFileName)
   
   
   for directoryName in allDirectoryNames:
      #
      # Get files in directory
      #
      files = os.listdir(directoryName)

      #
      # Find the fiducial coord file to get case, hemisphere nodes
      #
      case = ""
      hem  = ""
      nodes = ""
      #
      # Find the fiducial coord file to get case, hemisphere nodes
      #
      case = ""
      hem  = ""
      nodes = ""
      for filename in files:
         if (filename.find("Human.BucknerCase") >= 0) :
            if (filename.find(".spec") >= 0) :
               #print "Fiducial", filename
               m = re.match("Human.Buckner(Case\d\d)\.([L|R])\.spec",
                            filename)
               case  = m.group(1)
               hem   = m.group(2)
               break
#      for filename in files:
#         if (filename.find("Fiducial") >= 0) :
#            #print "Fiducial", filename
#            m = re.match(r"Human\.Buckner(Case\d\d)\.([L|R])\.Fiducial\.(\d+)\.coord",
#                         filename)
#            
#            case  = m.group(1)
#            hem   = m.group(2)
#           nodes = m.group(3)
#            
#            break
      
      #
      # Put together the data file names
      #
      imageFileName = directoryName + "/Landmarks." + case + "." + hem + ".jpg"

      #
      # If image file exists
      #
      if os.path.exists(imageFileName) :
         #
         # Add to command
         # Semicolon near end of line is needed to make it a tuple
         #
         idCmdList = idCmdList + (imageFileName,) 
      
   #
   # Final command
   #   
   command = " ".join(idCmdList)
   
   return command
   
##-----------------------------------------------------------------------------
#
# Clean spec file commands
#
def buildCleanSpecFileCommand(directoryName) :
   #
   # Get files in directory
   #
   files = os.listdir(directoryName)

   #
   # Find the fiducial coord file to get case, hemisphere nodes
   #
   case = ""
   hem  = ""
   nodes = ""
   for filename in files:
      if (filename.find("Fiducial") >= 0) :
         #print "Fiducial", filename
         m = re.match(r"Human\.Buckner(Case\d\d)\.([L|R])\.Fiducial\.(\d+)\.coord",
                      filename)
         
         case  = m.group(1)
         hem   = m.group(2)
         nodes = m.group(3)
         
         break
   
   #print "case  ", case
   #print "hem   ", hem
   #print "nodes ", nodes

   #
   # Put together the data file names
   #
   specFileName = "Human.Buckner" + case + "." + hem + ".spec"
   
   #
   # Scene
   #
   idCmdList = (
      caretCommand,
      "-spec-file-clean",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      specFileName
   )
   
   commandList = [" ".join(idCmdList)]
   
   return commandList
   
##-----------------------------------------------------------------------------
#
# Build create medial wall scene command list and return it
#
def buildCreateMedialWallSceneCommand(directoryName) :
   #
   # Get files in directory
   #
   files = os.listdir(directoryName)

   #
   # Find the fiducial coord file to get case, hemisphere nodes
   #
   case = ""
   hem  = ""
   nodes = ""
   for filename in files:
      if (filename.find("Fiducial") >= 0) :
         #print "Fiducial", filename
         m = re.match(r"Human\.Buckner(Case\d\d)\.([L|R])\.Fiducial\.(\d+)\.coord",
                      filename)
         
         case  = m.group(1)
         hem   = m.group(2)
         nodes = m.group(3)
         
         break
   
   #print "case  ", case
   #print "hem   ", hem
   #print "nodes ", nodes

   #
   # Put together the data file names
   #
   sceneFileName = "Human.BUCKNER" + case + "." + hem + ".Landmarks.scene"
   specFileName = "Human.Buckner" + case + "." + hem + ".spec"
   
   #
   # Corpus Callosum Slice
   #
   idCmdList = (
      caretCommand,
      "-scene-create",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      specFileName,
      sceneFileName,
      sceneFileName,
      "Corpus_Callosum",
      "-window-volume-coord",
         "WINDOW_MAIN",
         "512",
         "512",
         "PARASAGITTAL",
         "0.0",
         "0.0",
         "0.0",
      "-volume-overlay",
         "PRIMARY",
         "SEGMENTATION",
      "-volume-overlay",
         "UNDERLAY",
         "ANATOMY",
      "-volume-overlay-segmentation",
         "CorpusCallosum.nii.gz",
         "1",
      "-show-borders",
      "-show-foci",
      "-window-surface-types",
         "WINDOW_2",
         "512",
         "512",
         "FIDUCIAL",
         "CLOSED",
         "MEDIAL",
      "-window-surface-types",
         "WINDOW_3",
         "512",
         "512",
         "INFLATED",
         "CLOSED",
         "MEDIAL",
      "-surface-overlay",
         "PRIMARY",
         "PAINT",
         "\"Sulcal Identification\"",
         "0",
      "-show-borders",
      "-show-foci"
   )
   
   commandList = [" ".join(idCmdList)]
   
   #
   # Fiducial Medial
   #
   idCmdList = (
      caretCommand,
      "-scene-create",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      specFileName,
      sceneFileName,
      sceneFileName,
      "Fiducial_Medial",
      "-window-surface-types",
         "WINDOW_MAIN",
         "512",
         "512",
         "FIDUCIAL",
         "CLOSED",
         "MEDIAL",
      "-show-borders",
      "-show-foci"
   )
   
   #commandList.append(" ".join(idCmdList))
   
   #
   # Update the spec file
   #
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "scene_file",
                                                sceneFileName))
   
   return commandList

##-----------------------------------------------------------------------------
#
# Build create border scene command list and return it
#
def buildEkrBorderSceneCommand(directoryName) :
   #
   # Get files in directory
   #
   files = os.listdir(directoryName)

   #
   # Find the fiducial coord file to get case, hemisphere nodes
   #
   case = ""
   hem  = ""
   nodes = ""
   for filename in files:
      if (filename.find("Human.BucknerCase") >= 0) :
         #print "Fiducial", filename
         m = re.match("Human.Buckner(Case\d\d)\.([L|R])\.spec",
                      filename)
         
         case  = m.group(1)
         hem   = m.group(2)
         
         break
   
   #print "case  ", case
   #print "hem   ", hem
   #print "nodes ", nodes

   #
   # Put together the data file names
   #
   sceneFileName = "Human.BUCKNER" + case + "." + hem + ".Landmarks.scene"
   specFileName = "Human.Buckner" + case + "." + hem + ".spec"
   
   #
   # Corpus Callosum Slice
   #
   idCmdList = (
      caretCommand,
      "-scene-create",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      specFileName,
      "\"\"",
      sceneFileName,
      "Landmarks",
      "-window-surface-types",
         "WINDOW_MAIN",
         "512",
         "512",
         "FIDUCIAL",
         "CLOSED",
         "MEDIAL",
      "-window-surface-types",
         "WINDOW_2",
         "512",
         "512",
         "VERY_INFLATED",
         "CLOSED",
         "LATERAL",
      "-window-surface-types",
         "WINDOW_3",
         "512",
         "512",
         "INFLATED",
         "CLOSED",
         "MEDIAL",
      "-surface-overlay",
         "UNDERLAY",
         "SURFACE_SHAPE",
         "\"Folding (Mean Curvature)\"",
         "0",
      "-show-borders"
   )
   
   commandList = [" ".join(idCmdList)]
   
   #
   # Fiducial Medial
   #
   idCmdList = (
      caretCommand,
      "-scene-create",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      specFileName,
      sceneFileName,
      sceneFileName,
      "Fiducial_Medial",
      "-window-surface-types",
         "WINDOW_MAIN",
         "512",
         "512",
         "FIDUCIAL",
         "CLOSED",
         "MEDIAL",
      "-show-borders",
      "-show-foci"
   )
   
   #commandList.append(" ".join(idCmdList))
   
   #
   # Update the spec file
   #
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "scene_file",
                                                sceneFileName))
   
   return commandList

##-----------------------------------------------------------------------------
#
# Build create border scene command list and return it
#
def buildCreateBorderSceneCommand(directoryName) :
   #
   # Get files in directory
   #
   files = os.listdir(directoryName)

   #
   # Find the fiducial coord file to get case, hemisphere nodes
   #
   case = ""
   hem  = ""
   nodes = ""
   for filename in files:
      if (filename.find("Fiducial") >= 0) :
         #print "Fiducial", filename
         m = re.match(r"Human\.Buckner(Case\d\d)\.([L|R])\.Fiducial\.(\d+)\.coord",
                      filename)
         
         case  = m.group(1)
         hem   = m.group(2)
         nodes = m.group(3)
         
         break
   
   #print "case  ", case
   #print "hem   ", hem
   #print "nodes ", nodes

   #
   # Put together the data file names
   #
   sceneFileName = "Human.BUCKNER" + case + "." + hem + ".Landmarks.scene"
   specFileName = "Human.Buckner" + case + "." + hem + ".spec"
   
   #
   # Corpus Callosum Slice
   #
   idCmdList = (
      caretCommand,
      "-scene-create",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      specFileName,
      "\"\"",
      sceneFileName,
      "Landmarks",
      "-window-surface-types",
         "WINDOW_MAIN",
         "512",
         "512",
         "FIDUCIAL",
         "CLOSED",
         "MEDIAL",
      "-window-surface-types",
         "WINDOW_2",
         "512",
         "512",
         "VERY_INFLATED",
         "CLOSED",
         "LATERAL",
      "-window-surface-types",
         "WINDOW_3",
         "512",
         "512",
         "INFLATED",
         "CLOSED",
         "MEDIAL",
      "-surface-overlay",
         "UNDERLAY",
         "SURFACE_SHAPE",
         "\"Folding (Mean Curvature)\"",
         "0",
      "-show-borders"
   )
   
   commandList = [" ".join(idCmdList)]
   
   #
   # Fiducial Medial
   #
   idCmdList = (
      caretCommand,
      "-scene-create",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      specFileName,
      sceneFileName,
      sceneFileName,
      "Fiducial_Medial",
      "-window-surface-types",
         "WINDOW_MAIN",
         "512",
         "512",
         "FIDUCIAL",
         "CLOSED",
         "MEDIAL",
      "-show-borders",
      "-show-foci"
   )
   
   #commandList.append(" ".join(idCmdList))
   
   #
   # Update the spec file
   #
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "scene_file",
                                                sceneFileName))
   
   return commandList

##-----------------------------------------------------------------------------
#
# Build a sulcal id command list and return it
#
def buildSulcalIdCommand(directoryName) :
   #
   # Get files in directory
   #
   files = os.listdir(directoryName)

   #
   # Find the fiducial coord file to get case, hemisphere nodes
   #
   case = ""
   hem  = ""
   nodes = ""
   for filename in files:
      if (filename.find("Fiducial") >= 0) :
         #print "Fiducial", filename
         m = re.match(r"Human\.Buckner(Case\d\d)\.([L|R])\.Fiducial\.(\d+)\.coord",
                      filename)
         
         case  = m.group(1)
         hem   = m.group(2)
         nodes = m.group(3)
         
         break
   
   #print "case  ", case
   #print "hem   ", hem
   #print "nodes ", nodes

   #
   # Put together the data file names
   #
   anatomyVolumeFileName = "Human.Buckner." + hem + "." + case + "_ANATOMY+orig.nii.gz"
   closedTopologyFileName = "Human.Buckner" + case + "." + hem + ".CLOSED." + nodes + ".topo"
   corpusCallosumVolumeFileName = "LANDMARK_BORDER_DEBUG_FILES/CorpusCallosum.nii.gz"
   ellipsoidCoordinateFileName = "Human.Buckner" + case + "." + hem + ".Ellipsoidal." + nodes + ".coord"
   fiducialCoordinateFileName = "Human.Buckner" + case + "." + hem + ".Fiducial." + nodes + ".coord"
   inflatedCoordinateFileName = "Human.Buckner" + case + "." + hem + ".Inflated." + nodes + ".coord"
   inputAreaColorFileName = "Human.BUCKNER" + case + "." + hem + ".Initial.areacolor"
   inputBorderColorFileName = "\"\""
   inputBorderProjectionFileName = "\"\""
   inputPaintFileName = "Human.Buckner" + case + "." + hem + ".Initial." + nodes + ".paint"
   inputPaintFileGeographyColumnName = "Geography"
   inputSurfaceShapeFileName = "Human.Buckner" + case + "." + hem + ".Initial." + nodes + ".surface_shape"
   inputSurfaceShapeFileDepthColumnName = "Depth"
   inputVocabularyFileName = "\"\""
   outputAreaColorFileName = "Human.BUCKNER" + case + "." + hem + ".SulcalIdentification.areacolor"
   outputBorderColorFileName = "Human.BUCKNER" + case + "." + hem + ".LandmarkColors.bordercolor"
   outputBorderProjectionFileName = "Human.BUCKNER" + case + "." + hem + ".Landmarks.borderproj"
   outputFociColorFileName = "LANDMARK_BORDER_DEBUG_FILES/Human.Buckner" + case + "." + hem + ".DebugFoci.focicolor"
   outputFociProjectionFileName = "LANDMARK_BORDER_DEBUG_FILES/Human.Buckner" + case + "." + hem + ".DebugFoci.fociproj"
   outputPaintFileName = "Human.BUCKNER" + case + "." + hem + ".SulcalIdentification.paint"
   outputVocabularyFileName = "Human.BUCKNER" + case + "." + hem + ".vocabulary"
   specFileName = "Human.Buckner" + case + "." + hem + ".spec"
   veryInflatedCoordFileName = "Human.Buckner" + case + "." + hem + ".VeryInflated." + nodes + ".coord"
   
   #
   # Stereotaxic space
   #
   stereotaxicSpace = "711-2C"
   
   #
   # Run sulcal identification
   #
   idCmdList = (
      caretCommand,
      "-surface-border-landmark-identification",
      "-CHDIR",
      directoryName,
      "-CHMOD",
      "UR,UW,GR,GW,AR,AW",
      stereotaxicSpace,
      anatomyVolumeFileName,
      fiducialCoordinateFileName,
      inflatedCoordinateFileName,
      veryInflatedCoordFileName,
      ellipsoidCoordinateFileName,
      closedTopologyFileName,
      inputPaintFileName,
      outputPaintFileName,
      inputPaintFileGeographyColumnName,
      inputSurfaceShapeFileName,
      inputSurfaceShapeFileDepthColumnName,
      inputAreaColorFileName,
      outputAreaColorFileName,
      inputVocabularyFileName,
      outputVocabularyFileName,
      inputBorderProjectionFileName,
      outputBorderProjectionFileName,
      inputBorderColorFileName,
      outputBorderColorFileName,
      #outputFociProjectionFileName,
      #outputFociColorFileName,
      "-no-flatten"
   )   
   
   commandList = [" ".join(idCmdList)]
   
   #
   # Update the spec file
   #
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "area_color_file",
                                                outputAreaColorFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "border_color_file",
                                                outputBorderColorFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "borderproj_file",
                                                outputBorderProjectionFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "foci_color_file",
                                                outputFociColorFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "fociproj_file",
                                                outputFociProjectionFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "paint_file",
                                                outputPaintFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "vocabulary_file",
                                                outputVocabularyFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "volume_segmentation_file",
                                                corpusCallosumVolumeFileName))
   
   return commandList
   
##-----------------------------------------------------------------------------
#
# Test
#
def buildTestCommand(directoryName) :
   commandList = []
   
   commandList.append(" ".join(("echo", directoryName)))
   
   return commandList
   
##-----------------------------------------------------------------------------
#
# Print help information
#
def printHelp() :
   print "Caret Landmark Testing"
   print ""
   print "Options"
   print "   -clean            Delete all foci/border proj/color files, jpg files"
   print "   -clean-specs      Remove non-existing files from spec files"
   print "   -landmarks        Generate landmarks"
   print "   -scene-create     Create scenes"
   print "   -scene-create-ekr Create scenes"
   print "   -scene-images     Create Images of Scenes"
   print "   -scene-images-mw  Create Images of Medial Wall Scenes"
   print "   -scene-composite  Create a composite of the images that were"
   print "                     created with \"-scene-images\""
   print " "
##-----------------------------------------------------------------------------
#
#  MAIN
#

#
# Flags for running different command
#
doCleanFlag = False
doCleanSpecsFlag = False
doEkrScenesFlag = False
doLandmarksFlag = False
doScenesFlag    = False
doSceneImagesFlag = False
doSceneMedialWallImagesFlag = False
doSceneImagesCompositeFlag = False
doTestFlag = False

#
# Process arguments
#
numArgs = len(sys.argv)
if (numArgs <= 1) :
   printHelp()
   os._exit(0)

for  i in range(1, numArgs) :
   arg = sys.argv[i]
   if arg == "-clean" :
      doCleanFlag = True
   elif arg == "-clean-specs" :
      doCleanSpecsFlag = True
   elif arg == "-landmarks" :
      doLandmarksFlag = True
   elif arg == "-scene-create" :
      doScenesFlag = True
   elif arg == "-scene-create-ekr" :
      doEkrScenesFlag = True
   elif arg == "-scene-images" :
      doSceneImagesFlag = True
   elif arg == "-scene-images-mw" :
      doSceneMedialWallImagesFlag = True
   elif arg == "-scene-composite" :
      doSceneImagesCompositeFlag = True;
   elif arg == "-test" :
      doTestFlag = True
   else :
      print "ERROR: Unrecognized parameter: ", arg
      os._exit(0)
      
#
# Run sulcal identification on the specified cases
#

subDirListEkr = [
   "Human_Buckner_Case01/BUCKNER_Case01.L",
   "Human_Buckner_Case02/BUCKNER_Case02.L",
   "Human_Buckner_Case03/BUCKNER_Case03.L",
   "Human_Buckner_Case01/BUCKNER_Case01.R",
   "Human_Buckner_Case02/BUCKNER_Case02.R",
   "Human_Buckner_Case03/BUCKNER_Case03.R"
]
#subDirListEkr = [ "Human_Buckner_Case01/BUCKNER_Case01.L" ]

subDirListLeft = [
   "Human_Buckner_Case01/BUCKNER_Case01.L",
   "Human_Buckner_Case02/BUCKNER_Case02.L",
   "Human_Buckner_Case03/BUCKNER_Case03.L",
   "Human_Buckner_Case04/BUCKNER_Case04.L",
   "Human_Buckner_Case05/BUCKNER_Case05.L",
   "Human_Buckner_Case06/BUCKNER_Case06.L",
   "Human_Buckner_Case07/BUCKNER_Case07.L",
   "Human_Buckner_Case08/BUCKNER_Case08.L",
   "Human_Buckner_Case09/BUCKNER_Case09.L",
   "Human_Buckner_Case10/BUCKNER_Case10.L",
   "Human_Buckner_Case11/BUCKNER_Case11.L",
   "Human_Buckner_Case12/BUCKNER_Case12.L",
   "Human_Buckner_Case13/BUCKNER_Case13.L",
   "Human_Buckner_Case14/BUCKNER_Case14.L",
   "Human_Buckner_Case15/BUCKNER_Case15.L",
   "Human_Buckner_Case16/BUCKNER_Case16.L",
   "Human_Buckner_Case17/BUCKNER_Case17.L",
   "Human_Buckner_Case18/BUCKNER_Case18.L",
   "Human_Buckner_Case19/BUCKNER_Case19.L",
   "Human_Buckner_Case20/BUCKNER_Case20.L",
   "Human_Buckner_Case21/BUCKNER_Case21.L",
   "Human_Buckner_Case22/BUCKNER_Case22.L",
   "Human_Buckner_Case23/BUCKNER_Case23.L",
   "Human_Buckner_Case24/BUCKNER_Case24.L"
]

subDirListRight = [
   "Human_Buckner_Case01/BUCKNER_Case01.R",
   "Human_Buckner_Case02/BUCKNER_Case02.R",
   "Human_Buckner_Case03/BUCKNER_Case03.R",
   "Human_Buckner_Case04/BUCKNER_Case04.R",
   "Human_Buckner_Case05/BUCKNER_Case05.R",
   "Human_Buckner_Case06/BUCKNER_Case06.R",
   "Human_Buckner_Case07/BUCKNER_Case07.R",
   "Human_Buckner_Case08/BUCKNER_Case08.R",
   "Human_Buckner_Case09/BUCKNER_Case09.R",
   "Human_Buckner_Case10/BUCKNER_Case10.R",
   "Human_Buckner_Case11/BUCKNER_Case11.R",
   "Human_Buckner_Case12/BUCKNER_Case12.R",
   "Human_Buckner_Case13/BUCKNER_Case13.R",
   "Human_Buckner_Case14/BUCKNER_Case14.R",
   "Human_Buckner_Case15/BUCKNER_Case15.R",
   "Human_Buckner_Case16/BUCKNER_Case16.R",
   "Human_Buckner_Case17/BUCKNER_Case17.R",
   "Human_Buckner_Case18/BUCKNER_Case18.R",
   "Human_Buckner_Case19/BUCKNER_Case19.R",
   "Human_Buckner_Case20/BUCKNER_Case20.R",
   "Human_Buckner_Case21/BUCKNER_Case21.R",
   "Human_Buckner_Case22/BUCKNER_Case22.R",
   "Human_Buckner_Case23/BUCKNER_Case23.R",
   "Human_Buckner_Case24/BUCKNER_Case24.R"
]

#
# All Cases
#
subDirListAll = subDirListLeft + subDirListRight

#
# ERIN - Put the case you would like to run here!!
#
subDirListSingle = ["Human_Buckner_Case01/BUCKNER_Case01.R"]

#
# Name of directory containing subjects
#
topDir = os.getcwd()  #"/backup/sulcal_id/sulcal_id_john";

#
# Choose directory list for execution
#
subDirList = subDirListEkr
#subDirList = subDirListAll
#subDirList = subDirListLeft
#subDirList = subDirListRight
#subDirList = subDirListSingle

#
# Maximum number of threads
#
maxThreads = 4
singleThread = 1

#
# Doing Landmarks ?
#
if doLandmarksFlag :
   #
   # Get all commands
   #
   allCommands = []
   for dirName in subDirList:
      subjectDirectory = topDir + "/" + dirName
      allCommands.append(buildSulcalIdCommand(subjectDirectory))

   #
   # Run the commands in threads
   #
   runThreads = RunCommandsInMultipleThreads(allCommands, maxThreads)
   runThreads.start()


#
# Doing EKR Scenes ?
#
if doEkrScenesFlag :
   #
   # Get all commands
   #
   allCommands = []
   for dirName in subDirListEkr:
      subjectDirectory = topDir + "/" + dirName
      allCommands.append(buildEkrBorderSceneCommand(subjectDirectory))

   #
   # Run the commands in ONE thread since multiple scenes
   #
   runThreads = RunCommandsInMultipleThreads(allCommands, singleThread)
   runThreads.start()

#
# Doing Scenes ?
#
if doScenesFlag :
   #
   # Get all commands
   #
   allCommands = []
   for dirName in subDirList:
      subjectDirectory = topDir + "/" + dirName
      allCommands.append(buildCreateBorderSceneCommand(subjectDirectory))
      allCommands.append(buildCreateMedialWallSceneCommand(subjectDirectory))

   #
   # Run the commands in ONE thread since multiple scenes
   #
   runThreads = RunCommandsInMultipleThreads(allCommands, singleThread)
   runThreads.start()

if doSceneImagesFlag :
   #
   # Get all commands
   #
   allCommands = []
   for dirName in subDirList:
      subjectDirectory = topDir + "/" + dirName
      allCommands.append(buildCreateSceneImageCommand(subjectDirectory, "1"))

   #
   # Run the commands in ONE thread since multiple scenes
   #
   runThreads = RunCommandsInMultipleThreads(allCommands, singleThread)
   runThreads.start()

if doSceneMedialWallImagesFlag :
   #
   # Get all commands
   #
   allCommands = []
   for dirName in subDirList:
      subjectDirectory = topDir + "/" + dirName
      allCommands.append(buildCreateSceneImageCommand(subjectDirectory, "2"))

   #
   # Run the commands in ONE thread since multiple scenes
   #
   runThreads = RunCommandsInMultipleThreads(allCommands, singleThread)
   runThreads.start()

if doSceneImagesCompositeFlag :   
   #
   # Create the composite image
   #
   allImagesCommand = buildAssembleImagesCommand("LandmarksAll.jpg", 
                                                 subDirList)
   runCommand(allImagesCommand)
   
if doTestFlag :
   allCommands = []
   for dirName in subDirList:
      subjectDirectory = topDir + "/" + dirName
      allCommands.append(buildTestCommand(subjectDirectory))
      
   runThreads = RunCommandsInMultipleThreads(allCommands, maxThreads)
   runThreads.start()
   
   
if doCleanSpecsFlag :
   allCommands = []
   for dirName in subDirList:
      subjectDirectory = topDir + "/" + dirName
      allCommands.append(buildCleanSpecFileCommand(subjectDirectory))
      
   runThreads = RunCommandsInMultipleThreads(allCommands, singleThread)
   runThreads.start()
   
   
if doCleanFlag :
   runCommand("rm `find . -name '*borderproj' -print`")
   runCommand("rm `find . -name '*bordercolor' -print`")
   runCommand("rm `find . -name '*fociproj' -print`")
   runCommand("rm `find . -name '*focicolor' -print`")
   runCommand("rm `find . -name '*.jpg' -print`")
   

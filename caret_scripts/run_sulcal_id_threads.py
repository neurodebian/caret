#!/usr/bin/python
#
# Run sulcal identification on buckner case 01-12 left and right
#

caretCommand = "C:/caret/bin/caret_command"
caretCommand = "/Users/john/caret5_osx/caret_source/caret_command/caret_command"

import os
import re
import time
import subprocess
import sys
from threading import Thread

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
   cmdList = (
      caretCommand,
      "-CHDIR",
      directoryName,
      "-spec-file-add",
      specFileName,
      specFileTag,
      fileName)
   
   commandText = " ".join(cmdList)
   return commandText

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
   outputFociColorFileName = "Human.BUCKNER" + case + "." + hem + ".Debug.focicolor"
   outputFociProjectionFileName = "Human.BUCKNER" + case + "." + hem + ".Debug.fociproj"
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
#      outputFociProjectionFileName,
#      outputFociColorFileName
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
#   commandList.append(buildAddToSpecFileCommand(directoryName,
#                                                specFileName,
#                                                "foci_color_file",
#                                                outputFociColorFileName))
#   commandList.append(buildAddToSpecFileCommand(directoryName,
#                                                specFileName,
#                                                "fociproj_file",
#                                                outputFociProjectionFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "paint_file",
                                                outputPaintFileName))
   commandList.append(buildAddToSpecFileCommand(directoryName,
                                                specFileName,
                                                "vocabulary_file",
                                                outputVocabularyFileName))
   
   return commandList
   
##-----------------------------------------------------------------------------
#
# Run sulcal identification on the specified cases
#

subDirListAll = [
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
   "Human_Buckner_Case12/BUCKNER_Case12.R"
]

subDirListSome = [
   "Human_Buckner_Case01/BUCKNER_Case01.L",
   "Human_Buckner_Case02/BUCKNER_Case02.L",
   "Human_Buckner_Case01/BUCKNER_Case01.R",
   "Human_Buckner_Case02/BUCKNER_Case02.R"
]

#
# ERIN - Put the case you would like to run here!!
#
subDirListSingle = ["TEST_CASE01/BUCKNER_Case01.L"]

#
# Name of directory containing subjects
#
topDir = "/backup/sulcal_id/sulcal_id_john";

#
# Choose directory list for execution
#
#subDirList = subDirListAll
#subDirList = subDirListSome
subDirList = subDirListSingle

#
# Number of subdirectories for processing
#
numSubDirs = len(subDirList)

#
# Maximum number of threads
#
maxThreads = 4

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
# Loop until all subdirectories are executed
#
##i = 0
##while i < numSubDirs: 
##   #
##   # Determine how many threads to create
##   #
##   numThreadsToCreate = maxThreads
##   numDirsLeft = numSubDirs - i
##   if (numDirsLeft < numThreadsToCreate) :
##      numThreadsToCreate = numDirsLeft
##      
##   #
##   # Get directories that are to be executed
##   #
##   subDirsToRunList = []
##   j = 0
##   while j < numThreadsToCreate:
##      subDirsToRunList.append(subDirList[i])
##      i = i + 1
##      j = j + 1
##   
##   #
##   # Execute the directories
##   #
##   threadList = []
##   for subDirName in subDirsToRunList :
##      subjectDirectory = topDir + "/" + subDirName
##      print "*******************************************************************"
##      print subjectDirectory
##      print "*******************************************************************"
##      sidThread = SulcalID(subjectDirectory)
##      threadList.append(sidThread)
##      sidThread.start()
##      
##   for thread in threadList:
##      thread.join()
      
   

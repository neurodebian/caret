#!/usr/bin/python
#
# Run sulcal identification on buckner case 01-12 left and right
#

import re
import os
import sys

caretCommand = "/Users/john/caret5_development/caret5_cpp/caret_source/caret_command/caret_command"

##-----------------------------------------------------------------------------
#
# Run a command
#
def runCommand(cmdList) :
   cmd = " ".join(cmdList)  # join cmdList into a string separated by blanks
   print "\nExecuting: %s\n" % cmd
   result = os.system(cmd)
   if (result != 0) :
      print "ERROR COMMAND FAILED: "
      print "   ", cmd
      ##os._exit(-1)


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
# Run sulcal identification
#
def runSulcalID(case, hem, nodes) :

   #
   # Some file names
   #
   areaColorFileName = "Human.BUCKNER_" + case + "." + hem + ".SulcalIdentification.areacolor"
   borderColorFileName = "Human.BUCKNER_" + case + "." + hem + ".LandmarkColors.bordercolor"
   borderProjectionFileName = "Human.BUCKNER_" + case + "." + hem + ".Landmarks.borderproj"
   fociColorFileName = "Human.BUCKNER_" + case + "." + hem + ".Debug.focicolor"
   fociProjectionFileName = "Human.BUCKNER_" + case + "." + hem + ".Debug.fociproj"
   paintFileName = "Human.BUCKNER_" + case + "." + hem + ".SulcalIdentification.paint"
   specFileName = "Human.Buckner" + case + "." + hem + ".spec"
   
   #
   # Run sulcal identification
   #
   cmdList = (
      caretCommand,
      "-surface-border-landmark-identification",
      "Human.Buckner." + hem + "." + case + "_ANATOMY+orig.nii.gz",
      "Human.Buckner" + case + "." + hem + ".Fiducial." + nodes + ".coord",
      "Human.Buckner" + case + "." + hem + ".Inflated." + nodes + ".coord",
      "Human.Buckner" + case + "." + hem + ".VeryInflated." + nodes + ".coord",
      "Human.Buckner" + case + "." + hem + ".Ellipsoidal." + nodes + ".coord",
      "Human.Buckner" + case + "." + hem + ".CLOSED." + nodes + ".topo",
      "Human.Buckner" + case + "." + hem + ".paint_file_80." + nodes + ".paint",
      paintFileName,
      "Geography",
      "Human.Buckner" + case + "." + hem + ".surface_shape_file_87." + nodes + ".surface_shape",
      "Depth",
      "Human.BUCKNER_" + case + "." + hem + ".SulcalID.areacolor",
      areaColorFileName,
      "\"\"",
      borderProjectionFileName,
      "\"\"",
      borderColorFileName,
      fociProjectionFileName,
      fociColorFileName
   )   
   runCommand(cmdList)

   #
   # Update spec file
   #
   addToSpecFile(specFileName,
                  "area_color_file",
                  areaColorFileName)
   addToSpecFile(specFileName,
                  "border_color_file",
                  borderColorFileName)
   addToSpecFile(specFileName,
                  "borderproj_file",
                  borderProjectionFileName)
   addToSpecFile(specFileName,
                  "foci_color_file",
                  fociColorFileName)
   addToSpecFile(specFileName,
                  "fociproj_file",
                  fociProjectionFileName)
   addToSpecFile(specFileName,
                  "paint_file",
                  paintFileName)

##-----------------------------------------------------------------------------
#
# Find Fiducial surface and run Sulcal Identification
#
def runSulcalIdentificationInDirectory() :
   #
   # Get files in directory
   #
   files = os.listdir(".")

   #
   # Find the fiducial coord file
   #
   for filename in files:
      if (filename.find("Fiducial") >= 0) :
         print "Fiducial", filename
         m = re.match(r"Human\.Buckner(Case\d\d)\.([L|R])\.Fiducial\.(\d+)\.coord",
                      filename)
         
         #print "m     " m
         print "case  ", m.group(1)
         print "hem   ", m.group(2)
         print "nodes ", m.group(3)

         runSulcalID(m.group(1), m.group(2), m.group(3))
         break
    

##-----------------------------------------------------------------------------
#
# Run sulcal identification on the specified cases
#

subDirListAll = (
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
)

subDirListSingle = (
   "Human_Buckner_Case01/BUCKNER_Case01.L",
   "Human_Buckner_Case01/BUCKNER_Case01.R",
   "Human_Buckner_Case02/BUCKNER_Case02.L",
   "Human_Buckner_Case02/BUCKNER_Case02.R",
)

#
# Choose directory list for execution
#
subDirList = subDirListAll
subDirList = subDirListSingle

for subDirName in subDirList :
   print "*******************************************************************"
   print "        ", subDirName
   print "*******************************************************************"
   os.chdir(subDirName)
   runSulcalIdentificationInDirectory()
   os.chdir("../..")
   
   
#m = re.match(r"Human\.Buckner(Case\d\d)\.([L|R])\.Fiducial\.(\d+)\.coord",
#             "Human.BucknerCase01.L.Fiducial.60417.coord")
             
#print "case  ", m.group(1)
#print "hem   ", m.group(2)
#print "nodes ", m.group(3)

#runSulcalID(m.group(1), m.group(2), m.group(3))



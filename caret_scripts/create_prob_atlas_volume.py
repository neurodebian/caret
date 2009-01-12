#!/usr/bin/python
#
# Create the probabilistic atlas volume
#

#
# Imports
#
import getpass
import os
import sys

caretCommand = "/Users/john/caret5_osx/caret_source/caret_command/caret_command"

##-----------------------------------------------------------------------------
#
# Run a command
#
def runCommand(cmdList) :
   cmd = " ".join(cmdList)  # join cmdList into a string separated by blanks
   print "\nExecuting: %s\n" % cmd
   result = os.system(cmd)
   if (result != 0) :
      print "COMMAND FAILED: "
      print "   ", cmd
      os._exit(-1)
      

##-----------------------------------------------------------------------------
#
# Data file names
#
leftCoordFileNameList = [
   "Human_Buck_Case01.L.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case02.L.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case03.L.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case04.L.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case05.L.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.clean.align.73730.coord",
   "Human_Buck_Case06.L.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.clean.align.73730.coord",
   "Human_Buck_Case07.L.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case08.L.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case09.L.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case10.L.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case11.L.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case12.L.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord"
]
leftPaintFileName = "Human.PALS_B12.IDsulci_B1-12_LEFT.clean3.73730.atlas.paint"
leftTopoFileName = "Human.sphere_6.LEFT_HEM.73730.topo"
leftOutputVolumeFileName = "PALS_B12.B1-12.LEFT.PROB-ATLAS_IDsulci.paint.clean.align+orig.nii.gz"

rightCoordFileNameList = [
   "Human_Buck_Case01.R.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case02.R.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case03.R.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case04.R.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.clean.align.73730.coord",
   "Human_Buck_Case05.R.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case06.R.F.RegToPALS_B12.LR.FIDUCIAL_711-2C.clean.align.73730.coord",
   "Human_Buck_Case07.R.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case08.R.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case09.R.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case10.R.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case11.R.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord",
   "Human_Buck_Case12.R.M.RegToPALS_B12.LR.FIDUCIAL_711-2C.align.73730.coord"
]
rightPaintFileName = "Human.PALS_B12.IDsulci_B1-12_RIGHT.clean4.73730.atlas.paint"
rightTopoFileName = "Human.sphere_6.RIGHT_HEM.73730.topo"
rightOutputVolumeFileName = "PALS_B12.B1-12.LEFT.PROB-ATLAS_IDsulci.paint.clean.align+orig.nii.gz"

coordFileNameList = rightCoordFileNameList
paintFileName = rightPaintFileName
topoFileName = rightTopoFileName
outputVolumeFileName = rightOutputVolumeFileName

##-----------------------------------------------------------------------------
#
# Create the paint volumes
#
paintVolumeFileNamesList = []
columnCounter = 1
for coordFileName in coordFileNameList :
   paintFileColumn = str(columnCounter)
   if (columnCounter < 10) :
      paintFileColumn = "0" + str(columnCounter)
   columnCounter = columnCounter + 1
   
   paintVolumeFileName = "Human_Buck_Case" \
                       + paintFileColumn \
                       + ".R111_SulcalID+orig.nii.gz"
   paintVolumeFileNamesList.append(paintVolumeFileName)
   
   cmdList = (caretCommand,
              "-volume-create-in-stereotaxic-space",
              "711-2C-111",
              paintVolumeFileName)
   runCommand(cmdList)
   
   cmdList = (caretCommand,
              "-surface-to-volume",
              coordFileName,
              topoFileName,
              paintFileName,
              paintFileColumn,
              paintVolumeFileName,
              "-inner -1.5 -outer 1.5 -step 0.5")
   runCommand(cmdList)
   
##-----------------------------------------------------------------------------
#
# Combine the paint volumes into a single, multi-brick volume file
#   
cmdList = (caretCommand,
           "-volume-file-combine",
           outputVolumeFileName,
           " ".join(paintVolumeFileNamesList),
           "-paint")
runCommand(cmdList)

   
##-----------------------------------------------------------------------------
#
# Create the functional volumes, one for each paint name
#
cmdList = (caretCommand,
           "-volume-prob-atlas-to-functional",
           outputVolumeFileName,
           "PALS_B12.RIGHT.PROBABILISTIC-",
           ".align+orig.nii.gz")
runCommand(cmdList)

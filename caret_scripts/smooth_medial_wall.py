#!/usr/bin/python

#
# Imports
#
import os
import sys

#
# Global Variables
#
progName = "/Users/john/caret5_osx/caret_source/caret_command/caret_command"
#progName = "caret_command"

areaColorFileName     = "Geography.areacolor"
separateBorderProjectionFileName = "MedialWall_Separate.borderproj"
mergedBorderProjectionFileName = "MedialWall_Merged.borderproj"
borderColorFileName   = "LANDMARK.bordercolor"
medialWallBorderName             = "LANDMARK.MEDIAL.WALL"
fiducialCoordFileName = "Human.colin.Cerebral.R.FIDUCIAL.TLRC.711-2B.71723.coord"
smoothedFiducialCoordFileName = "Human.colin.Cerebral.R.FIDUCIAL_SMOOTHED.TLRC.711-2B.71723.coord"
inflatedCoordFileName = "Human.colin.Cerebral.R.INFLATED.71723.coord"
paintFileName         = "MedialWall.paint"
paintName             = "MEDIAL.WALL"
roiFileName           = "medial_wall.roi"
surfaceShapeFileName  = "Curvature.surface_shape"
topologyFileName      = "Human.colin.Cerebral.R.CLOSED.71723.topo"

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
# Main
#

#
# Merge the two medial wall borders into a single border
#
cmdList = (progName,
           "-surface-border-merge",
           separateBorderProjectionFileName,
           mergedBorderProjectionFileName,
           medialWallBorderName,
           "LANDMARK.MedWall.DORSAL",
           "LANDMARK.MedWall.VENTRAL",
           "-delete-input-border-projections",
           "-close-border")
runCommand(cmdList)

#
# Resample the border
#
cmdList = (progName,
           "-surface-border-resample",
           fiducialCoordFileName,
           topologyFileName,
           mergedBorderProjectionFileName,
           mergedBorderProjectionFileName,
           str(2.0),
           "-all")
runCommand(cmdList)

#
# Create a region of interest that contains nodes within
# the medial wall border projection
#
cmdList = (progName,  
           "-surface-region-of-interest-selection", 
           inflatedCoordFileName, 
           topologyFileName,
           roiFileName,
           roiFileName,
           "-border-projection",
              mergedBorderProjectionFileName,
              medialWallBorderName,
              "M",
              "3D",
              0,
              "NORMAL")
runCommand(cmdList)

#
# Create the color for the medial wall paint
#
cmdList = (progName,
           "-color-file-add-color",
           areaColorFileName,
           areaColorFileName,
           paintName,
           str(255),
           str(0),
           str(0))
runCommand(cmdList)

#
# Create the color for the medial wall border
#
cmdList = (progName,
           "-color-file-add-color",
           borderColorFileName,
           borderColorFileName,
           medialWallBorderName,
           str(255),
           str(0),
           str(0))
runCommand(cmdList)

#
# Create a NEW paint file with one column named "Geography"
#
cmdList = (progName,  
           "-paint-file-create",
           paintFileName,
           str(1),
           "-coordinate-file",
              inflatedCoordFileName,
           "-set-column-name 1 Geography")
runCommand(cmdList)

#
# Assign nodes in ROI to paint
#
cmdList = (progName,  
           "-paint-assign-to-nodes",
           paintFileName,
           paintFileName,
           str(1),
           paintName,
           "-assign-from-roi-file",
              roiFileName)
runCommand(cmdList)

#
# Smooth the medial wall
#
cmdList = (progName,
           "-surface-smoothing",
           fiducialCoordFileName,
           smoothedFiducialCoordFileName,
           topologyFileName,
           str(1.0),
           str(50),
           str(-1),
           "-roi-file ",
              roiFileName)
runCommand(cmdList)

#
# Generate curvature
#
cmdList = (progName,
           "-surface-curvature",
           smoothedFiducialCoordFileName,
           topologyFileName,
           surfaceShapeFileName,
           surfaceShapeFileName,
           "-generate-mean-curvature",
           "-mean-column-name  \"Folding (Mean Curvature) MWS\"")
runCommand(cmdList)

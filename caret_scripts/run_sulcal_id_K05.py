#!/usr/bin/python

#
# Imports
#
import os
import sys

#
# Global Variables
#
progName = "/Users/john/caret5_osx/caret_source/caret_command_new/caret_command_new"
#progName = "caret_command_new"

#
# Post Central Sulcus parameters
#
postCentralSulcusOffset = 25.0
postCentralSulcusStdDevSquared = 100.0
postCentralSulcusSplit = 5.0

probabilisticSulcusVolumeListFileName = "probabilistic-depth-volume.csv"

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
##
## Generate Sulcal Paint Identification File
##
def createSulcalIdentificationPaintFile() :
   cmdList = (progName,
              "-system-file-delete",
              paintFileName)
   runCommand(cmdList)
   
   cmdList = (progName, 
              "-surface-sulcal-named-all-identification", 
              fiducialCoordFileName, 
              veryInflatedCoordFileName,
              closedTopoFileName, 
              geographyPaintFileName, 
              paintFileName,
              geographyPaintColumnName, 
              shapeFileName, 
              shapeFileDepthColumnName,
              probabilisticSulcusVolumeListFileName,
              str(postCentralSulcusOffset),
              str(postCentralSulcusStdDevSquared),
              str(postCentralSulcusSplit))
   runCommand(cmdList)

##-----------------------------------------------------------------------------
##
## Identify central sulcus
def identifyCentralSulcus() :
   #
   # Add Colors to Foci File
   #
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "CeS-medial",
              str(255),
              str(0),
              str(0),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "CeS-ventral",
              str(0),
              str(255),
              str(0),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "CeS-MedialExtreme",
              str(255),
              str(0),
              str(0),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "CeS-VentralExtreme",
              str(0),
              str(255),
              str(0),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              borderColorFileName,
              borderColorFileName,
              "LANDMARK.CentralSulcus",
              str(255),
              str(255),
              str(0));
   runCommand(cmdList)
              
   #
   # Delete any existing foci with these names
   #
   cesMedialFocusName = "CeS-medial"
   cesVentralFocusName = "CeS-ventral"
   cesVentralExtremeFocusName = "CeS-VentralExtreme"
   cesMedialExtremeFocusName = "CeS-MedialExtreme"
   cmdList = (progName,
              "-surface-foci-delete",
              fociProjectionFileName,
              fociProjectionFileName,
              cesMedialFocusName,
              cesVentralFocusName,
              cesVentralExtremeFocusName,
              cesMedialExtremeFocusName)
   runCommand(cmdList)
   
   #
   # Delete any existing border with these names
   #
   landmarkCentralSulcusBorderName = "LANDMARK.CentralSulcus"
   cmdList = (progName,
              "-surface-border-delete",
              borderProjectionFileName,
              borderProjectionFileName,
              landmarkCentralSulcusBorderName)
   runCommand(cmdList)
   
   #
   # Create an ROI that identifies the central sulcus nodes
   # with paint name SUL.CeS and Shape folding ranging -100 to -0.10
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiCeSFileName,
              roiCeSFileName,
              "-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,   # column name
                 "SUL.CeS",                       # paint name
                 "NORMAL",                         # normal selection
              "-shape",
                 shapeFileName,
                 shapeFileCurvatureColumnName,               # column number
                 "-100.0 -0.10",    # value range
                 "AND")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create foci at some limits of ROI
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              roiCeSFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-z-min " + cesVentralFocusName,
              "-x-most-medial " + cesMedialFocusName)
   runCommand(cmdList)

   #
   # Create an ROI that identifies the central sulcus nodes
   # with paint name SUL.CeS and Shape folding ranging -100 to -0.16
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiStringentCeSFileName,
              roiStringentCeSFileName,
              "-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,   # column name
                 "SUL.CeS",                       # paint name
                 "NORMAL",                        # normal selection
              "-shape",
                 shapeFileName,
                 shapeFileCurvatureColumnName,               # column number
                 "-100.0 -0.16",    # value range
                 "AND")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create the draw border comand to draw the LANDMARK.CentralSulcus border
   #
   borderSampling = 1.0
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              cesVentralFocusName,
              cesMedialFocusName,
              roiStringentCeSFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              landmarkCentralSulcusBorderName,
              str(borderSampling))
   runCommand(cmdList)
   
   #
   # Find extremum point for lateral side
   #
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              cesVentralFocusName,
              fociProjectionFileName,
              cesVentralExtremeFocusName,
              "Z-NEG",
              str(100000.0),
              str(3.0),
              str(100000.0))
   runCommand(cmdList)

   #
   # Find extremum point for medial side
   #
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              cesMedialFocusName,
              fociProjectionFileName,
              cesMedialExtremeFocusName,
              "X-MEDIAL",
              str(100000.0),
              str(5.0),
              str(100000.0))
   runCommand(cmdList)

   #
   # Trim border 19mm above ventral extreme
   #
   cmdList = (progName,
              "-surface-border-nibbler",
              inflatedCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              landmarkCentralSulcusBorderName,
              landmarkCentralSulcusBorderName,
              fociProjectionFileName,
              cesVentralExtremeFocusName,
              "-within-z-distance 19")
   runCommand(cmdList)

   #
   # Trim border 18mm lateral to midline
   #
   trimValue = -18
   if (rightHemFlag) :
      trimValue = 18
   cmdList = (progName,
              "-surface-border-nibbler",
              inflatedCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              landmarkCentralSulcusBorderName,
              landmarkCentralSulcusBorderName,
              fociProjectionFileName,
              cesMedialExtremeFocusName,
              "-within-x-distance ",
                 str(trimValue))
   runCommand(cmdList)
   
##-----------------------------------------------------------------------------
##
## Identify calcarine sulci
def identifyCalcarineSulcus() :
   #
   # Add Colors to Foci File
   #
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "CaS-posterior",
              str(0),
              str(150),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "CaS-anterior",
              str(0),
              str(150),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "CaS-PosteriorExtreme",
              str(0),
              str(255),
              str(0),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              borderColorFileName,
              borderColorFileName,
              "LANDMARK.CalcarineSulcus",
              str(246),
              str(114),
              str(0));
   runCommand(cmdList)

   #
   # Delete any existing foci with these names
   #
   casAnteriorFocusName = "CaS-anterior"
   casPosteriorFocusName = "CaS-posterior"
   casPosteriorExtremeFocusName = "CaS-PosteriorExtreme"
   cmdList = (progName,
              "-surface-foci-delete",
              fociProjectionFileName,
              fociProjectionFileName,
              casAnteriorFocusName,
              casPosteriorFocusName,
              casPosteriorExtremeFocusName)
   runCommand(cmdList)
   #
   # Delete any existing border with these names
   #
   landmarkCalcarineSulcusBorderName = "LANDMARK.CalcarineSulcus"
   cmdList = (progName,
              "-surface-border-delete",
              borderProjectionFileName,
              borderProjectionFileName,
              landmarkCalcarineSulcusBorderName)
   runCommand(cmdList)

   #
   # Create an ROI that identifies the central sulcus nodes
   # with paint name SUL.CaS and Shape folding ranging -100 to -0.10
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiCaSFileName,
              roiCaSFileName,
              "-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,   # column name
                 "SUL.CaS",                       # paint name
                 "NORMAL",                         # normal selection
              "-shape",
                 shapeFileName,
                 shapeFileCurvatureColumnName,               # column number
                 "-100.0 -0.10",    # value range
                 "AND")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create foci at some limits of ROI
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              roiCaSFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-y-max " + casAnteriorFocusName,
              "-y-min " + casPosteriorFocusName)
   runCommand(cmdList)

   #
   # Create an ROI that identifies the central sulcus nodes
   # with paint name SUL.CaS and Shape folding ranging -100 to -0.16
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiStringentCaSFileName,
              roiStringentCaSFileName,
              "-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,   # column name
                 "SUL.CaS",                       # paint name
                 "NORMAL",                        # normal selection
              "-shape",
                 shapeFileName,
                 shapeFileCurvatureColumnName,               # column number
                 "-100.0 -0.16",    # value range
                 "AND")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create the draw border comand to draw the LANDMARK.CalcarineSulcus border
   #
   borderSampling = 1.0
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              casPosteriorFocusName,
              casAnteriorFocusName,
              roiStringentCaSFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              landmarkCalcarineSulcusBorderName,
              str(borderSampling))
   runCommand(cmdList)
   
   #
   # Find extremum point for lateral side
   #
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              casPosteriorFocusName,
              fociProjectionFileName,
              casPosteriorExtremeFocusName,
              "Y-NEG",
              str(100000.0),
              str(100000.0),
              str(100000.0))
   runCommand(cmdList)

   #
   # Trim border 24mm from posterior extreme
   #
   cmdList = (progName,
              "-surface-border-nibbler",
              inflatedCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              landmarkCalcarineSulcusBorderName,
              landmarkCalcarineSulcusBorderName,
              fociProjectionFileName,
              casPosteriorExtremeFocusName,
              "-within-y-distance 24")
   runCommand(cmdList)

##-----------------------------------------------------------------------------
##
## Identify superior temporal gyrus
def identifySuperiorTemporalGyrus() :
   #
   # Add Colors to Foci File
   #
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "STS_Inflated_Ventral",
              str(0),
              str(150),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "STS_Inflated_Dorsal",
              str(0),
              str(150),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "TemporalPole",
              str(100),
              str(0),
              str(200),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "STG-posterior",
              str(0),
              str(150),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "STG-ventral",
              str(0),
              str(150),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "STG",
              str(100),
              str(255),
              str(150),
              "-point-size 1",
              "-symbol SPHERE");
   runCommand(cmdList)
   
   #
   # Delete any existing foci with these names
   #
   stsVentralFocusName = "STS_Inflated_Ventral"
   stsDorsalFocusName = "STS_Inflated_Dorsal"
   temporalPoleFocusName = "TemporalPole"
   stgPathFocusName = "STG-posterior"
   stgCesLimitFocusName = "STG-CES-limit"
   cmdList = (progName,
              "-surface-foci-delete",
              fociProjectionFileName,
              fociProjectionFileName,
              stsVentralFocusName,
              stsDorsalFocusName,
              temporalPoleFocusName,
              stgPathFocusName,
              stgCesLimitFocusName)
   runCommand(cmdList)
   #
   # Delete any existing border with these names
   #
   stgBorderName = "LANDMARK.SF_STSant"
   cmdList = (progName,
              "-surface-border-delete",
              borderProjectionFileName,
              borderProjectionFileName,
              stgBorderName)
   runCommand(cmdList)

   #
   # Add border colors
   #
   cmdList = (progName,
              "-color-file-add-color",
              borderColorFileName,
              borderColorFileName,
              "LANDMARK.SF_STSant",
              str(255),
              str(0),
              str(187),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   
   #
   # Create an ROI that identifies the superior temporal sulcus
   # with paint name SUL.STS
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiSTGFileName,
              roiSTGFileName,
              "-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,   # column name
                 "SUL.STS",                       # paint name
                 "NORMAL")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create foci at some limits of ROI
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              roiSTGFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-z-min " + stsVentralFocusName,
              "-z-max " + stsDorsalFocusName)
   runCommand(cmdList)

   #
   # Find extremum point for temporal pole
   #
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              stsVentralFocusName,
              fociProjectionFileName,
              temporalPoleFocusName,
              "Y-POS",
              str(100000.0),
              str(100000.0),
              str(100000.0))
   runCommand(cmdList)
   
   #
   # Create a transformation matrix rotated Y=-45
   #
   rotateString = "-rotate  0.0  45.0  0.0"
   if (rightHemFlag) :
      rotateString = "-rotate  0.0  -45.0  0.0"
   transformationMatrixFileName = "TempRotateYMinus45.matrix"
   transformationMatrixName     = "RotateYMinus45"
   cmdList = (progName, 
              "-transformation-matrix-create",
              transformationMatrixFileName,
              transformationMatrixFileName,
              transformationMatrixName,
              "-delete-all-matrices-from-file",
              "-matrix-comment \"rotate minus 45 degrees about Y-axis\"",
              rotateString)
   runCommand(cmdList)
   
   #
   # Create a temporary surface rotated Y-45
   #
   tempRotateY45InflatedCoordFileName = "temp.rotatedY45.coord"
   cmdList = (progName,
              "-surface-apply-transformation-matrix",
              inflatedCoordFileName,
              closedTopoFileName,
              tempRotateY45InflatedCoordFileName,
              "-matrix-file ",
                 transformationMatrixFileName,
                 transformationMatrixName)
              #"-matrix",
              #   "0.707107 0.000000 -0.707107 0.000000", 
              #   "0.000000 1.000000 0.000000 0.000000",
              #   "0.707107 0.000000 0.707107 0.000000",
              #   "0.000000 0.000000 0.000000 1.000000")
              #"-matrix-file STG.matrix Y45Down");
   runCommand(cmdList)
              
   #
   # Find extremum point starting at temporal pole
   # moving in a posterior/superior direction along the STG
   #
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              tempRotateY45InflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              temporalPoleFocusName,
              fociProjectionFileName,
              stgPathFocusName,
              "Z-POS",
              str(100000.0),
              str(100000.0),
              str(100000.0),
              "-create-roi-from-path",
                 roiSTGFileName)
              #"-start-offset  3.0  0.0  0.0")
   runCommand(cmdList)
   
   #
   # Limit the ROI so it does not go beyond temporal pole
   # or beyond the Y position of the ventral tip of the
   # central sulcus
   #
   cesVentralFocusName = "CeS-ventral"
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiSTGFileName,
              roiSTGFileName,
              "-limit-y-min-focus",
                 fociProjectionFileName,
                 cesVentralFocusName,
              "-limit-y-max-focus",
                 fociProjectionFileName,
                 temporalPoleFocusName,
              "-limit-z-min-focus",
                 fociProjectionFileName,
                 temporalPoleFocusName)
   runCommand(cmdList)
   
   #
   # Create limits at posterior and ventral ends of GYRAL.STG
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              roiSTGFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-y-min " + stgCesLimitFocusName)
   runCommand(cmdList)
   
   #
   # Create the draw border comand to draw the LANDMARK.SF_STSant border
   #
   borderSampling = 1.0
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              stgCesLimitFocusName,
              temporalPoleFocusName,
              roiSTGFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              stgBorderName,
              str(borderSampling))
   runCommand(cmdList)
   
   ######################################################
   # The following steps are not necessary but assign paint
   # for the STG
   #
   
   #
   # Dilate the STG ROI but only nodes identified as GYRAL
   # and limit the extent of the ROI so that it does not 
   # go posterior to ventral tip of central sulcus or 
   # beyond the temporal pole.
   #
   dilateIterations = 5
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiSTGFileName,
              roiSTGFileName,
              "-dilate-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,
                 "SUL.SF",  # SF may be too big
                 str(dilateIterations),
              "-dilate-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,
                 "GYRAL",
                 str(dilateIterations),
               "-limit-y-min-focus",
                  fociProjectionFileName,
                  cesVentralFocusName,
               "-limit-y-max-focus",
                  fociProjectionFileName,
                  temporalPoleFocusName,
               "-limit-z-min-focus",
                  fociProjectionFileName,
                  temporalPoleFocusName,
                  )
   runCommand(cmdList)
   
   #
   # Assign the STG paint
   #
   cmdList = (progName,
              "-paint-assign-to-nodes",
              paintFileName,
              paintFileGyralSTGName,
              sulcusIdPaintColumnName,
              "GYRAL.STG",
              "-assign-from-roi-file",
                 roiSTGFileName);
   runCommand(cmdList)

##-----------------------------------------------------------------------------
##
## Identify sylvian fissure
##
def identifySylvianFissure() :
   #
   # Add Colors to Foci File
   #
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "SF",
              str(50),
              str(255),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   ventralFrontalExtremeFocusName = "SF_VentralFrontalExtreme"
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "SF",
              str(0),
              str(255),
              str(0),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   ventralFrontalFocusName = "SF_VentralFrontal"
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "SF",
              str(0),
              str(0),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   
   #
   # Add border colors
   #
   cmdList = (progName,
              "-color-file-add-color",
              borderColorFileName,
              borderColorFileName,
              "LANDMARK.SF",
              str(0),
              str(255),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)

   #
   # Delete any existing foci with these names
   #
   sfAnteriorFocusName = "SF_Anterior"
   sfPosteriorFocusName = "SF_Posterior"
   sfDorsalFocusName = "SF_Dorsal"
   ventralFrontalFocusName = "SF_VentralFrontal"
   ventralFrontalExtremeFocusName = "SF_VentralFrontalExtreme"
   sfVentralFocusName = "SF_VentralNearSecondary_SF"
   sylvianIntersectionFocusName = "SF_Intersect_Superior_Inferior"
   ventralFrontalExtremeFocusName = "SF_VentralFrontalExtreme"
   cmdList = (progName,
              "-surface-foci-delete",
              fociProjectionFileName,
              fociProjectionFileName,
              sfAnteriorFocusName,
              sfPosteriorFocusName,
              sfDorsalFocusName,
              ventralFrontalFocusName,
              ventralFrontalExtremeFocusName,
              sfVentralFocusName,
              sylvianIntersectionFocusName,
              ventralFrontalExtremeFocusName)
   runCommand(cmdList)
   #
   # Delete any existing border with these names
   #
   sylvianFissureBorderName = "LANDMARK.SF"
   secondarySylvianFissureBorderName = "LANDMARK.SF-secondary"
   cmdList = (progName,
              "-surface-border-delete",
              borderProjectionFileName,
              borderProjectionFileName,
              sylvianFissureBorderName,
              secondarySylvianFissureBorderName)
   runCommand(cmdList)

   #
   # Generate curvature on the INFLATED surface
   #
   cmdList = (progName,
              "-surface-curvature",
              inflatedCoordFileName,
              closedTopoFileName,
              inflatedShapeCurvatureFileName,
              inflatedShapeCurvatureFileName,
              "-generate-mean-curvature",
              "-mean-column-name",
                 inflatedCurvatureColumnName);
   runCommand(cmdList);
              
   #
   # Create an ROI that identifies the sylvian fissure
   # with paint name SUL.SF
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiSFFileName,
              roiSFFileName,
              "-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,   # column name
                 "SUL.SF",          # paint name
                 "NORMAL")          # normal selection
   runCommand(cmdList)

   #
   # Erode the ROI since SF paint tends to be too big
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiSFFileName,
              roiSFErodedFileName,
              "-erode 5")             
   runCommand(cmdList)

   #
   # Create foci at some limits of ROI at eroded ROI
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              closedTopoFileName,
              roiSFErodedFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-y-max " + sfAnteriorFocusName,
              "-y-min " + sfPosteriorFocusName,
              "-z-max " + sfDorsalFocusName)
   runCommand(cmdList)

   #
   # Limit original ROI to apply curvature limits
   # 
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              closedTopoFileName,
              roiSFFileName,
              roiStringentSFFileName,
              "-shape",
                 inflatedShapeCurvatureFileName,
                 inflatedCurvatureColumnName,
                 str(-100.0),
                 str(-0.07),   # use -0.7 since inflated
                 "AND")
   runCommand(cmdList)

   #
   # Draw border from posterior to anterior of SF
   # USES GEODESIC METHOD
   #
   borderSampling = 1.0
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              sfPosteriorFocusName,
              sfAnteriorFocusName,
              roiStringentSFFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.SF1",
              str(borderSampling))
   runCommand(cmdList)
   
   #
   # USES CURVATURE METHOD
   #
   cmdList = (progName,
              "-surface-border-draw-metric",
              inflatedCoordFileName,
              closedTopoFileName,
              inflatedShapeCurvatureFileName,
              inflatedCurvatureColumnName,
              "NEGATIVE",
              fociProjectionFileName,
              sfPosteriorFocusName,
              sfAnteriorFocusName,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.SF1",
              str(borderSampling));
   ## DO NOT RUN  runCommand(cmdList)
   
   #
   # Create a focus near ventral limit of frontal pole,
   # on lateral bank of the olfactory sulcus on the fiducial surface
   #
   ventralFrontalX = -14
   if (rightHemFlag) :
      ventralFrontalX = 14
   cmdList = (progName,
              "-surface-foci-create",
              fiducialCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-focus",
                 ventralFrontalFocusName,
                 str(ventralFrontalX),
                 str(5),
                 str(-12))
   runCommand(cmdList);
                 
   #
   # Identify the ventral limit of frontal lobe, on the lateral bank of the 
   # olfactory sulcus on the fiducial surface
   #
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              fiducialCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              ventralFrontalFocusName,
              fociProjectionFileName,
              ventralFrontalExtremeFocusName,
              "Z-NEG",
              str(100000.0),
              str(100000.0),
              str(100000.0))
   runCommand(cmdList)

   #
   # Draw border from most anterior SF node to ventral tip of frontal lobe
   #
   borderSampling = 1.0
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              sfAnteriorFocusName,
              ventralFrontalExtremeFocusName,
              roiStringentSFFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.SF2",
              str(borderSampling))
   runCommand(cmdList)
   
   #
   # Merge the SF1 and SF2 borders into SF
   #
   cmdList = (progName,
              "-surface-border-merge",
              borderProjectionFileName,
              borderProjectionFileName,
              sylvianFissureBorderName,
              "LANDMARK.SF1",
              "LANDMARK.SF2",
              "-delete-input-border-projections")
   runCommand(cmdList)
   
   #
   # Create a focus posterior and dorsal to Temporal Pole, close to
   # secondary fundus of SF
   #
   temporalPoleFocusName = "TemporalPole"
   ventralSecondaryX = -7
   if (rightHemFlag) :
      ventralSecondaryX = 7
   cmdList = (progName,
              "-surface-foci-create",
              inflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-focus-offset",
                 sfVentralFocusName,
                 temporalPoleFocusName,
                 str(ventralSecondaryX),
                 str(-45),
                 str(40))
   runCommand(cmdList);
   
   #
   # Draw border along secondary fundus from anterior to posterior in SF
   #
   cmdList = (progName,
              "-surface-border-draw-metric",
              inflatedCoordFileName,
              closedTopoFileName,
              inflatedShapeCurvatureFileName,
              inflatedCurvatureColumnName,
              "NEGATIVE",
              fociProjectionFileName,
              sfVentralFocusName,
              sfPosteriorFocusName,
              borderProjectionFileName,
              borderProjectionFileName,
              secondarySylvianFissureBorderName,
              str(borderSampling))
   runCommand(cmdList)
   
   #
   # Find intersection of Sylvian fissue and secondary Sylvian fissure
   #
   borderIntersectionTolerance = 3.0
   cmdList = (progName,
              "-surface-border-intersection",
              inflatedCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              secondarySylvianFissureBorderName,
              sylvianFissureBorderName,
              sylvianIntersectionFocusName,
              str(borderIntersectionTolerance))
   runCommand(cmdList)
              
   #
   # Trim beyond 12mm posterior to intersection of superior and inferior circular sulci
   #
   cmdList = (progName,
              "-surface-border-nibbler",
              inflatedCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              sylvianFissureBorderName,
              sylvianFissureBorderName,
              fociProjectionFileName,
              sylvianIntersectionFocusName,
              "-less-than-y -12")
   runCommand(cmdList)

   #
   # Nibble anything within 15mm Z of ventral-frontal extreme
   #
   cmdList = (progName,
              "-surface-border-nibbler",
              inflatedCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              sylvianFissureBorderName,
              sylvianFissureBorderName,
              fociProjectionFileName,
              ventralFrontalExtremeFocusName,
              "-within-z-distance 15")
   runCommand(cmdList)

   
##-----------------------------------------------------------------------------
##
## Identify medial wall
##
def identifyMedialWall() :
   #
   # Add Colors to Foci File
   #
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "CC",
              str(0),
              str(0),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "Olf",
              str(0),
              str(0),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   cmdList = (progName,
              "-color-file-add-color",
              fociColorFileName,
              fociColorFileName,
              "MedialWall",
              str(0),
              str(0),
              str(255),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)
   
   #
   # Add border colors
   #
   cmdList = (progName,
              "-color-file-add-color",
              borderColorFileName,
              borderColorFileName,
              "LANDMARK.CorpusCallosum",
              str(255),
              str(255),
              str(0),
              "-point-size 3",
              "-symbol SPHERE");
   runCommand(cmdList)

   #
   # Names of foci created when creating medial wall landmark
   #
   ccPosteriorFocusName = "CC-post"
   ccAnteriorFocusName  = "CC-ant"
   ccCogFocusName = "CC-cog"
   genuLimitFocusName = "CC-genu-limit"
   spleniumLimitFocusName = "CC-splenium-limit"
   ccGenuBeginningFocusName = "CC-genu-beginning"
   olfSulcusPosteriorFocusName = "OlfSuclus-Posterior"
   olfSulcusMedialFocusName = "OlfSulcus-Medial"
   medialWallStartFocusName = "MedialWallStart"
   nearGenuBeginningFocusName = "CC-near-genu-beginning"
   nearSpleniumEndFocusName = "CC-near-splenium-end"
   
   #
   # Remove any existing foci that identify corpus callosum locations
   #
   cmdList = (progName,
              "-surface-foci-delete",
              fociProjectionFileName,
              fociProjectionFileName,
              ccPosteriorFocusName,
              ccAnteriorFocusName,
              ccCogFocusName,
              genuLimitFocusName,
              spleniumLimitFocusName,
              ccGenuBeginningFocusName,
              olfSulcusPosteriorFocusName,
              olfSulcusMedialFocusName,
              nearGenuBeginningFocusName,
              nearSpleniumEndFocusName)
   runCommand(cmdList)
   
   #
   # Remove any previous corpus callosum borders
   #
   corpusCallosumLandmarkBorderName = "LANDMARK.CorpusCallosum"
   corpusCallosumOlfactoryBorderName = "LANDMARK.CorpusCallosumOflactory"
   cmdList = (progName,
              "-surface-border-delete",
              borderProjectionFileName,
              borderProjectionFileName,
              corpusCallosumLandmarkBorderName,
              corpusCallosumOlfactoryBorderName)
   runCommand(cmdList)
   
   #
   # Create the corpus callosum slice
   #
   cmdList = (progName,
              "-volume-create-corpus-callosum-slice",
              anatomyVolumeFileName,
              "CorpusCallosumSlice+orig.nii.gz",
              "right")
   runCommand(cmdList)
   
   #
   # Smear along X axis
   #
   cmdList = (progName,
              "-volume-smear-axis",
              "CorpusCallosumSlice+orig.nii.gz",
              "CorpusCallosumSlice_Smear10+orig.nii.gz",
              "X",
              str(5),
              str(-1),
              str(1))
   runCommand(cmdList)
   
   #
   # Dilate
   #
   cmdList = (progName,
              "-volume-dilate",
              "CorpusCallosumSlice_Smear10+orig.nii.gz",
              "CorpusCallosumSlice_Smear10_Dilate5+orig.nii.gz",
              str(5))
   runCommand(cmdList)
              
   #
   # Smear along Y axis
   #
   cmdList = (progName,
              "-volume-smear-axis",
              "CorpusCallosumSlice_Smear10_Dilate5+orig.nii.gz",
              corpusCallosumVolumeFileName,
              "Y",
              str(2),
              str(-1),
              str(1))
   runCommand(cmdList)
   
   #
   # Map the non-zero voxels to a surface ROI file
   #
   cmdList = (progName,
              "-volume-map-to-surface-roi-file",
              corpusCallosumVolumeFileName,
              fiducialCoordFileName,
              closedTopoFileName,
              roiCorpusCallosumFileName)
   runCommand(cmdList)
   
      

   #
   # Place foci at anterior and posterior of corpus callosum - CHANGE FILE NAME
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              fiducialCoordFileName,
              closedTopoFileName,
              roiCorpusCallosumFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-y-min", 
                 ccPosteriorFocusName,
              "-y-max", 
                 ccAnteriorFocusName,
              "-cog", 
                 ccCogFocusName)
   runCommand(cmdList)
   
   #
   # Create a focus 20 mm posterior and 10 mm ventral to ccAnt 
   #
   cmdList = (progName,
              "-surface-foci-create",
              fiducialCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-focus-offset-xyz",
                 nearGenuBeginningFocusName,
                 ccAnteriorFocusName,  "0.0",
                 ccAnteriorFocusName, "-20.0",
                 ccAnteriorFocusName, "-10.0")
   runCommand(cmdList)
   
   #
   # Create a focus with 6 mm anterior and 5 mm ventral to ccPost
   #
   cmdList = (progName,
              "-surface-foci-create",
              fiducialCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-focus-offset-xyz",
                 nearSpleniumEndFocusName,
                 ccPosteriorFocusName,  "0.0",
                 ccPosteriorFocusName, "6.0",
                 ccPosteriorFocusName, "-5.0")

   runCommand(cmdList)





   #
   # Create border around ROI
   #
   cmdList = (progName,
              "-surface-border-draw-around-roi",
              fiducialCoordFileName,
              closedTopoFileName,
              roiCorpusCallosumFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              corpusCallosumLandmarkBorderName,
              "-start-near-focus",
                 fociProjectionFileName,
                 nearGenuBeginningFocusName)
   runCommand(cmdList)
   
   #
   # Resample the border
   #
   cmdList = (progName,
              "-surface-border-resample",
              fiducialCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              str(1.0),
              "-border-name",
                 corpusCallosumLandmarkBorderName)
   runCommand(cmdList)
            
   #
   # Remove links from border after near splenium
   #
   cmdList = (progName,
              "-surface-border-nibbler",
              fiducialCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              corpusCallosumLandmarkBorderName,
              corpusCallosumLandmarkBorderName,
              fociProjectionFileName,
              nearSpleniumEndFocusName,
              "-remove-after");
   runCommand(cmdList)
              
   #
   # Create a focus at start of LANDMARK.CorpusCallosum Border
   #
   cmdList = (progName,
              "-surface-border-link-to-focus",
              fiducialCoordFileName,
              closedTopoFileName,
              borderProjectionFileName,
              corpusCallosumLandmarkBorderName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-first-link",
                 ccGenuBeginningFocusName)
   runCommand(cmdList)
   
   #
   # Create an ROI containing Olfactory Sulcus
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              fiducialCoordFileName,
              closedTopoFileName,
              roiOlfactorySulcusFileName,
              roiOlfactorySulcusFileName,
              "-paint",
                 paintFileName,
                 sulcusIdPaintColumnName,
                 "SUL.OlfS",
                 "NORMAL")
   runCommand(cmdList)
   
   #
   # Find geographic limits of Olfactory Sulcus
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              fiducialCoordFileName,
              closedTopoFileName,
              roiOlfactorySulcusFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-y-min",
                 olfSulcusPosteriorFocusName,
              "-x-most-medial",
                 olfSulcusMedialFocusName)
   runCommand(cmdList)
   
   #
   # Create a new focus with X at Olfactory medial 
   # with Y at Olfactory posterior
   # with Z at Olfactory posterior
   #
   cmdList = (progName,
              "-surface-foci-create",
              fiducialCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-focus-offset-xyz",
                 medialWallStartFocusName,
                 olfSulcusMedialFocusName,  "0.0",
                 olfSulcusPosteriorFocusName, "-5.0",
                 olfSulcusPosteriorFocusName, "10.0")
   runCommand(cmdList)
   
   #
   # Create an ROI of all nodes
   #
   allNodesRoiFileName = "Olfactory-AllNodes.roi"
   cmdList = (progName,
              "-system-file-delete",
              allNodesRoiFileName)
   runCommand(cmdList)
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              fiducialCoordFileName,
              closedTopoFileName,
              allNodesRoiFileName,
              allNodesRoiFileName,
              "-all-nodes")
   runCommand(cmdList)
   
   #
   # Draw border connecting genu beginning to dorsal start
   # 
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              veryInflatedCoordFileName,
              closedTopoFileName,
              fociProjectionFileName,
              medialWallStartFocusName,
              ccGenuBeginningFocusName,
              allNodesRoiFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              corpusCallosumOlfactoryBorderName,
              str(1.0))
   runCommand(cmdList)
   
##-----------------------------------------------------------------------------
#
# Files created by this script
#
def createFileNames() :
   global rightHemFlag
   global borderColorFileName, borderProjectionFileName, corpusCallosumVolumeFileName
   global fociColorFileName, fociProjectionFileName
   global inflatedShapeCurvatureFileName, inflatedCurvatureColumnName
   global sulcusIdPaintColumnName
   global paintFileGyralSTGName, paintFileName
   global roiCaSFileName, roiStringentCaSFileName
   global roiCeSFileName, roiStringentCeSFileName
   global roiSTGFileName, roiStringentSTGFileName
   global roiSFFileName, roiStringentSFFileName, roiSFErodedFileName
   global roiCorpusCallosumFileName
   global roiOlfactorySulcusFileName
   
   rightHemFlag = False
   if (hemisphere == "R") :
      rightHemFlag = True

   filePrefix = species + "." + subject + "." + hemisphere + "."
   borderColorFileName = filePrefix + "Landmark.bordercolor"
   borderProjectionFileName = filePrefix + "Landmark.borderproj"
   corpusCallosumVolumeFileName = filePrefix + "CorpusCallosumSlice_Smear10+Dilate3_SmearPost+orig.nii.gz"
   fociColorFileName = filePrefix + "Landmark.focicolor"
   fociProjectionFileName = filePrefix + "Landmark.fociproj"
   inflatedShapeCurvatureFileName = filePrefix + "InflatedCurvature.shape"
   inflatedCurvatureColumnName = "\"Folding (Curvature) INFLATED\""
   sulcusIdPaintColumnName = "\"Sulcus ID\""
   paintFileGyralSTGName = filePrefix + "SulcalID_withSTG.paint"
   paintFileName = filePrefix + "SulcalID.paint"
   roiCaSFileName = filePrefix + "CaS.roi"
   roiStringentCaSFileName = filePrefix + "CaS_Stringent.roi"
   roiCeSFileName = filePrefix + "CeS.roi"
   roiStringentCeSFileName = filePrefix + "CeS_Stringent.roi"
   roiSTGFileName = filePrefix + "STG.roi"
   roiStringentSTGFileName = filePrefix + "STG_Stringent.roi"
   roiSFFileName = filePrefix + "SF.roi"
   roiStringentSFFileName = filePrefix + "SF_Stringent.roi"
   roiSFErodedFileName = filePrefix + "SF_Eroded.roi"
   roiCorpusCallosumFileName = filePrefix + "CorpusCallosum.roi"
   roiOlfactorySulcusFileName = filePrefix + "OlfactorySulcus.roi"

##-----------------------------------------------------------------------------
##
## Do the sulcal ID
##
def doSulcalID() :
   createFileNames()
   
   if (doMapProbabilisticVolumes) :
      createSulcalIdentificationPaintFile()

   if (doCentralSulcus) :
      identifyCentralSulcus()

   if (doCalcarineSulcus) :
      identifyCalcarineSulcus()
      
   if (doSuperiorTemporalGyrus) :
      identifySuperiorTemporalGyrus()

   if (doSylvianFissure) :
      identifySylvianFissure()

   if (doMedialWall) :
      identifyMedialWall()

##-----------------------------------------------------------------------------
##
## Print help information
##
def printHelp() :
   print "Options"
   print "-cas   Identify calcarine sulcus."
   print "-ces   Identify central sulcus."
   print "-mpv   Map probabilistic volumes to create Sulcal ID Paint File."
   print "-mw    Identify medial wall."
   print "-sf    Identify sylvian fissure."
   print "-stg   Identify Superior Temporal Gyrus."
   print ""
   print "-case04   Identify landmarks on Case 04"
   print "-case05   Identify landmarks on Case 05"

##-----------------------------------------------------------------------------
##
## "Main" code
##

#
# Get number of arguments
#
numArgs = len(sys.argv)
if (numArgs <= 1) :
   printHelp()
   os._exit(0)

#
# Flags for different operations
#
doCalcarineSulcus = False
doCentralSulcus = False
doMapProbabilisticVolumes = False
doMedialWall = False
doSuperiorTemporalGyrus = False
doSylvianFissure = False

#
# Flags for cases
#
doCase04 = False
doCase05 = False

#
# Process arguments
# 
for i in range (1, numArgs) :
   arg = sys.argv[i]
   if (arg == "-cas") :
      doCalcarineSulcus = True
   elif (arg == "-ces") :
      doCentralSulcus = True
   elif (arg == "-mpv") :
      doMapProbabilisticVolumes = True
   elif (arg == "-mw") :
      doMedialWall = True
   elif (arg == "-sf") :
      doSylvianFissure = True
   elif (arg == "-stg") :
      doSuperiorTemporalGyrus = True
   elif (arg == "-case04") :
      doCase04 = True
   elif (arg == "-case05") :
      doCase05 = True
   else:
      print "ERROR Invalid option: ", arg
      os._exit(-1)

#
# Case04
# 
if (doCase04) :
   anatomyVolumeFileName = "K04_mpr_on_711_2L_111_t88.4dfp.ifh"
   closedTopoFileName = "Human.K04.R.CLOSED.71612.topo"
   fiducialCoordFileName = "Human.K04.R.FIDUCIAL.71612.coord"
   inflatedCoordFileName = "Human.K04.R.Inflated.71612.coord"
   veryInflatedCoordFileName = "Human.K04.R.VeryInflated.71612.coord"
   geographyPaintFileName = "K04_mpr_on_711_2L_111_t88.R.full.segment_vent_corr4.geography.71612.paint"
   geographyPaintColumnName = "Geography"
   shapeFileName = "K04_mpr_on_711_2L_111_t88.R.full.segment_vent_corr4.71612.surface_shape"
   shapeFileDepthColumnName = "Depth"
   shapeFileCurvatureColumnName = "\"Folding (Mean Curvature)\""
   species = "Human"
   subject = "K04"
   hemisphere = "R"

   #
   # ID the sulci
   #
   doSulcalID()

#
# Case05
# 
if (doCase05) :
   anatomyVolumeFileName = "K05_mpr_on_711_2L_111_t88.4dfp.ifh"
   closedTopoFileName = "Human.K05.R.CLOSED.78049.topo"
   fiducialCoordFileName = "Human.K05.R.FIDUCIAL.78049.coord"
   inflatedCoordFileName = "Human.K05.R.Inflated.78049.coord"
   veryInflatedCoordFileName = "Human.K05.R.VeryInflated.78049.coord"
   geographyPaintFileName = "K05_mpr_on_711_2L_111_t88.R.full.segment_vent_corr5.geography.78049.paint"
   geographyPaintColumnName = "Geography"
   shapeFileName = "K05_mpr_on_711_2L_111_t88.R.full.segment_vent_corr5.78049.surface_shape"
   shapeFileDepthColumnName = "Depth"
   shapeFileCurvatureColumnName = "\"Folding (Mean Curvature)\""
   species = "Human"
   subject = "K05"
   hemisphere = "R"

   #
   # ID the sulci
   #
   doSulcalID()

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
# Hemisphere
#
rightHemFlag = 1

#
# Cluster ratio and output file name
#
clusterRatio = 0.5
outputCompositePaintFileName = "Human.Case13-18.R.TestSulcalID_Composite.73730.paint"

#
# Post Central Sulcus parameters
#
postCentralSulcusOffset = 25.0
postCentralSulcusStdDevSquared = 100.0
postCentralSulcusSplit = 5.0

#
# Names of input data files
#
borderColorFileName = "ForSphericalRegistration_Automated.bordercolor"
borderProjectionFilePrefix = "Case"
borderProjectionFileSuffix = ".LANDMARKS.73730.borderproj"

fiducialCoordFilePrefix = "Human.Buck_Case"
fiducialCoordFileSuffix = ".R.F.RegToPALS_B12.LR.FIDUCIAL.align.73730.coord"

fociColorFileName = "Landmark-related.focicolor"

fociProjectionFilePrefix = "LANDMARK-RELATED.Case"
fociProjectionFileSuffix = ".73730.fociproj"

inflatedCoordFilePrefix = "Human.Buck_Case"
inflatedCoordFileSuffix = ".R.F.RegToPALS_B12.LR.INFLATED.align.73730.coord"

veryInflatedCoordFilePrefix = "Human.Buck_Case"
veryInflatedCoordFileSuffix = ".R.F.RegToPALS_B12.LR.VERY_INFLATED.align.73730.coord"

inflatedShapeCurvatureFilePrefix = "Human.Buckner_INFLATED_Case"
shapeCurvatureFilePrefix = "Human.Buckner_Case"
shapeCurvatureFileSuffix = ".R.curvature.73730.surface_shape"

paintFilePrefix = "Human.Case"
paintFileSuffix = ".R.TestSulcalID.73730.paint"

veryInflatedCoordFile = "Human.PALS_B12.RIGHT_AVG_B1-12.VERY_INFLATED.clean.73730.coord"

outputPaintFiles = [
   "Human.Case13.R.TestSulcalID.73730.paint",
   "Human.Case14.R.TestSulcalID.73730.paint",
   "Human.Case15.R.TestSulcalID.73730.paint",
   "Human.Case16.R.TestSulcalID.73730.paint",
   "Human.Case17.R.TestSulcalID.73730.paint",
   "Human.Case18.R.TestSulcalID.73730.paint"
]

anatomyVolumeFilePrefix = "Human_Buckner_Case"
anatomyVolumeFileSuffix = "+orig.HEAD"

inputTopoFile = "Human.sphere_6.RIGHT_HEM.73730.topo"

inputPaintFile = "Human.PALS_B12.LR.B13-18_RIGHT_WOMEN.COMPOSITE.73730.paint"

inputShapeFile = "Human.PALS_B12.B13-18.RIGHT-DEPTH_INDIVIDUAL.73730.surface_shape"

probabilisticSulcusVolumeListFileName = "probabilistic-depth-volume.csv"

startCaseNumber = 13  # 13
endCaseNumber   = 19  # 19

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
## Delete border and foci projection files
##
def removeSulcalIdentficationBorderAndFociProjectionFiles() :
   for caseID in range(startCaseNumber, endCaseNumber) :
      caseNumberString = str(caseID)
      borderProjectionFileName = borderProjectionFilePrefix \
                               + caseNumberString \
                               + borderProjectionFileSuffix
      if (os.path.exists(borderProjectionFileName)) :
         os.remove(borderProjectionFileName) 

      fociProjectionFileName = fociProjectionFilePrefix \
                               + caseNumberString \
                               + fociProjectionFileSuffix
      if (os.path.exists(fociProjectionFileName)) :
         os.remove(fociProjectionFileName) 

   if (os.path.exists(borderColorFileName)) :
      os.remove(borderColorFileName) 
   if (os.path.exists(fociColorFileName)) :
      os.remove(fociColorFileName)

   
##-----------------------------------------------------------------------------
##
## Identify the Medial Wall
##
def identifyMedialWall(caseNumberString) :
   #
   # Global variables
   #
   
   #
   # Assemble file names
   #
   borderProjectionFileName = borderProjectionFilePrefix \
                            + caseNumberString \
                            + borderProjectionFileSuffix
   fiducialCoordFileName = fiducialCoordFilePrefix \
                           + caseNumberString \
                           + fiducialCoordFileSuffix
   fociProjectionFileName = fociProjectionFilePrefix \
                            + caseNumberString \
                            + fociProjectionFileSuffix
   inflatedCoordFileName = inflatedCoordFilePrefix \
                           + caseNumberString \
                           + inflatedCoordFileSuffix
   veryInflatedCoordFileName = veryInflatedCoordFilePrefix \
                           + caseNumberString \
                           + veryInflatedCoordFileSuffix
   paintFileName = paintFilePrefix \
                   + caseNumberString \
                   + paintFileSuffix
   paintColumnName = "\"Sulcus ID.Buck_Case" \
                     + caseNumberString \
                     + ".R\""
   roiFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "MedialWall" \
                 + ".roi"

   shapeCurvatureFileName = shapeCurvatureFilePrefix \
                          + caseNumberString \
                          + shapeCurvatureFileSuffix
                          
   anatomyVolumeFileName = anatomyVolumeFilePrefix \
                         + caseNumberString \
                         + anatomyVolumeFileSuffix
                  
   #
   # Names of foci created when creating medial wall landmark
   #
   ccPosteriorFocusName = "CC-post.Case" + caseNumberString
   ccAnteriorFocusName  = "CC-ant.Case" + caseNumberString
   ccCogFocusName = "CC-cog.Case" + caseNumberString
   genuLimitFocusName = "CC-genu-limit.Case" + caseNumberString
   spleniumLimitFocusName = "CC-splenium-limit.Case" + caseNumberString
   ccGenuBeginningFocusName = "CC-genu-beginning.Case" + caseNumberString
   olfSulcusPosteriorFocusName = "OlfSuclus-Posterior.Case" + caseNumberString
   olfSulcusMedialFocusName = "OlfSulcus-Medial.Case" + caseNumberString
   medialWallStartFocusName = "MedialWallStart.Case" + caseNumberString
   
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
              olfSulcusMedialFocusName)
   runCommand(cmdList)
   
   #
   # Remove any previous corpus callosum borders
   #
   corpusCallosumLandmarkBorderName = "LANDMARK.CorpusCallosum.Case" + caseNumberString
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
              "CorpusCallosumSlice+orig.nii",
              "right")
   runCommand(cmdList)
   
   #
   # Smear along X axis
   #
   cmdList = (progName,
              "-volume-smear-axis",
              "CorpusCallosumSlice+orig.nii",
              "CorpusCallosumSlice_Smear10+orig.nii",
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
              "CorpusCallosumSlice_Smear10+orig.nii",
              "CorpusCallosumSlice_Smear10_Dilate5+orig.nii",
              str(5))
   runCommand(cmdList)
              
   #
   # Smear along Y axis
   #
   corpusCallosumVolumeFileName = "CorpusCallosumSlice_Smear10+Dilate5_SmearPost_Case" \
                                  + caseNumberString \
                                  + "+orig.nii"
   cmdList = (progName,
              "-volume-smear-axis",
              "CorpusCallosumSlice_Smear10_Dilate5+orig.nii",
              corpusCallosumVolumeFileName,
              "Y",
              str(5),
              str(-1),
              str(1))
   runCommand(cmdList)
   
   #
   # Map the non-zero voxels to a surface ROI file
   #
   corpusCallosumFringeRoiFileName = "corpus_callosum_fringe.Case" + caseNumberString + ".roi"
   cmdList = (progName,
              "-volume-map-to-surface-roi-file",
              corpusCallosumVolumeFileName,
              fiducialCoordFileName,
              inputTopoFile,
              corpusCallosumFringeRoiFileName)
   runCommand(cmdList)
   
   #
   # Modify the ROI to exclude help exlude stuff in the cingulate sulcus
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              fiducialCoordFileName,
              inputTopoFile,
              corpusCallosumFringeRoiFileName,
              corpusCallosumFringeRoiFileName,
              "-erode 1",
              "-dilate 1")
   runCommand(cmdList)
                 
   #
   # Modify the ROI to that nodes have a curvature range (-100.0, -0.10)
   #
   corpusCallosumFringeCurveRoiFileName = "corpus_callosum_fringe_curve.Case" + caseNumberString + ".roi"
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              fiducialCoordFileName,
              inputTopoFile,
              corpusCallosumFringeRoiFileName,
              corpusCallosumFringeCurveRoiFileName,
              "-shape",
                 shapeCurvatureFileName,
                 str(1),
                 str(-100.0),
                 str(-0.10),
                 "AND")
   runCommand(cmdList)
   
   #
   # Place foci at anterior and posterior of corpus callosum
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              fiducialCoordFileName,
              inputTopoFile,
              corpusCallosumFringeCurveRoiFileName,
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
   # Deselect nodes above and posterior to CC-center leaving
   # just a ventral anterior group selected
   #
   corpusCallosumFringeCurveVentAntRoiFileName = "corpus_callosum_fringe_curve_vent_ant.Case" + caseNumberString + ".roi"
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              fiducialCoordFileName,
              inputTopoFile,
              corpusCallosumFringeCurveRoiFileName,
              corpusCallosumFringeCurveVentAntRoiFileName,
              "-limit-z-max-focus",
                 fociProjectionFileName,
                 ccCogFocusName,
              "-limit-y-min-focus",
                 fociProjectionFileName,
                 ccCogFocusName)
   runCommand(cmdList)
   
   #
   # Place a focus at "Genu" -Y-Max limit
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              fiducialCoordFileName,
              inputTopoFile,
              corpusCallosumFringeCurveVentAntRoiFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-y-max", genuLimitFocusName)
   runCommand(cmdList)
   
   #
   # Deselect nodes above and posterior to CC-center leaving
   # just a ventral anterior group selected
   #
   corpusCallosumFringeCurveVentPostRoiFileName = "corpus_callosum_fringe_curve_vent_post.Case" + caseNumberString + ".roi"
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              fiducialCoordFileName,
              inputTopoFile,
              corpusCallosumFringeCurveRoiFileName,
              corpusCallosumFringeCurveVentPostRoiFileName,
              "-limit-z-max-focus",
                 fociProjectionFileName,
                 ccCogFocusName,
              "-limit-y-max-focus",
                 fociProjectionFileName,
                 ccCogFocusName)
   runCommand(cmdList)
   
   #
   # Place a focus at "Splenium" -Y-Max limit
   #
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              fiducialCoordFileName,
              inputTopoFile,
              corpusCallosumFringeCurveVentPostRoiFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-z-min", spleniumLimitFocusName)
   runCommand(cmdList)
   
   
   #
   # Draw a callosal border
   #
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              fiducialCoordFileName,
              inputTopoFile,
              fociProjectionFileName,
              genuLimitFocusName,
              spleniumLimitFocusName,
              corpusCallosumFringeCurveRoiFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              corpusCallosumLandmarkBorderName,
              str(1.0))
   runCommand(cmdList)
   
   #
   # Create a focus at start of LANDMARK.CorpusCallosum Border
   #
   cmdList = (progName,
              "-surface-border-link-to-focus",
              fiducialCoordFileName,
              inputTopoFile,
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
   olfactorySulcusRoiFileName = "OlfactorySulcus.Case" + caseNumberString + ".roi"
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              fiducialCoordFileName,
              inputTopoFile,
              olfactorySulcusRoiFileName,
              olfactorySulcusRoiFileName,
              "-paint",
                 paintFileName,
                 str(1),  # use initial ID  paintColumnName,
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
              inputTopoFile,
              olfactorySulcusRoiFileName,
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
              inputTopoFile,
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
              "-surface-region-of-interest-selection",
              fiducialCoordFileName,
              inputTopoFile,
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
              inputTopoFile,
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
##
## Identify all medial walls
def identifyAllMedialWalls() :
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
   # Add paint colors
   #
   #cmdList = (progName,
   #           "-color-file-add-color",
   #           areaColorFileName,
   #           areaColorFileName,
   #           "GYRAL.STG",
  #            str(100),
  #            str(255),
  #            str(150),
  #            "-point-size 1",
  #            "-symbol SPHERE");
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
   # ID the central sulcus on the specified cases
   #
   for caseID in range(startCaseNumber, endCaseNumber) :
      identifyMedialWall(str(caseID))
      
##-----------------------------------------------------------------------------
##
## Identify the Sylvian fissure
##
def identifySylvianFissure(caseNumberString) :
   #
   # Global variables
   #
   
   #
   # Assemble file names
   #
   borderProjectionFileName = borderProjectionFilePrefix \
                            + caseNumberString \
                            + borderProjectionFileSuffix
   fiducialCoordFileName = fiducialCoordFilePrefix \
                           + caseNumberString \
                           + fiducialCoordFileSuffix
   fociProjectionFileName = fociProjectionFilePrefix \
                            + caseNumberString \
                            + fociProjectionFileSuffix
   inflatedCoordFileName = inflatedCoordFilePrefix \
                           + caseNumberString \
                           + inflatedCoordFileSuffix
   veryInflatedCoordFileName = veryInflatedCoordFilePrefix \
                           + caseNumberString \
                           + veryInflatedCoordFileSuffix
   paintFileName = paintFilePrefix \
                   + caseNumberString \
                   + paintFileSuffix
   paintColumnName = "\"Sulcus ID.Buck_Case" \
                     + caseNumberString \
                     + ".R\""
   roiFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "SF" \
                 + ".roi"
   roiErodedFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "SF_Eroded" \
                 + ".roi"
   roiStringentFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "SF_Stringent" \
                 + ".roi"

   inflatedShapeCurvatureFileName = inflatedShapeCurvatureFilePrefix \
                          + caseNumberString \
                          + shapeCurvatureFileSuffix
   inflatedCurvatureColumnName = "\"Folding (Mean Curvature) Inflated\""
                          
   shapeCurvatureFileName = shapeCurvatureFilePrefix \
                          + caseNumberString \
                          + shapeCurvatureFileSuffix

   #
   # Generate curvature on the INFLATED surface
   #
   cmdList = (progName,
              "-surface-curvature",
              inflatedCoordFileName,
              inputTopoFile,
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
              inputTopoFile,
              roiFileName,
              roiFileName,
              "-paint",
                 paintFileName,
                 paintColumnName,   # column name
                 "SUL.SF",          # paint name
                 "NORMAL")          # normal selection
   runCommand(cmdList)

   #
   # Erode the ROI since SF paint tends to be too big
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
              roiErodedFileName,
              "-erode 5")             
   runCommand(cmdList)

   #
   # Create foci at some limits of ROI at eroded ROI
   #
   sfAnteriorFocusName = "SF_Anterior.Case" + caseNumberString
   sfPosteriorFocusName = "SF_Posterior.Case" + caseNumberString
   sfDorsalFocusName = "SF_Dorsal.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
              roiErodedFileName,
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
              inputTopoFile,
              roiFileName,
              roiStringentFileName,
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
              inputTopoFile,
              fociProjectionFileName,
              sfPosteriorFocusName,
              sfAnteriorFocusName,
              roiStringentFileName,
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
              inputTopoFile,
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
   ventralFrontalFocusName = "SF_VentralFrontal.Case" + caseNumberString
   cmdList = (progName,
              "-surface-foci-create",
              fiducialCoordFileName,
              inputTopoFile,
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
   ventralFrontalExtremeFocusName = "SF_VentralFrontalExtreme.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              fiducialCoordFileName,
              inputTopoFile,
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
              inputTopoFile,
              fociProjectionFileName,
              sfAnteriorFocusName,
              ventralFrontalExtremeFocusName,
              roiStringentFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.SF2",
              str(borderSampling))
   runCommand(cmdList)
   
   #
   # Merge the SF1 and SF2 borders into SF
   #
   sylvianFissureBorderName = "LANDMARK.SF" + caseNumberString
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
   sfVentralFocusName = "SF_VentralNearSecondary_SF.Case" + caseNumberString
   temporalPoleFocusName = "TemporalPole.Case" + caseNumberString
   ventralSecondaryX = -7
   if (rightHemFlag) :
      ventralSecondaryX = 7
   cmdList = (progName,
              "-surface-foci-create",
              inflatedCoordFileName,
              inputTopoFile,
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
   secondarySylvianFissureBorderName = "LANDMARK.SF-secondary"
   cmdList = (progName,
              "-surface-border-draw-metric",
              inflatedCoordFileName,
              inputTopoFile,
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
   sylvianIntersectionFocusName = "SF_Intersect_Superior_Inferior" + caseNumberString
   borderIntersectionTolerance = 3.0
   cmdList = (progName,
              "-surface-border-intersection",
              inflatedCoordFileName,
              inputTopoFile,
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
   ##dorsalPostSylvianFissureBorderName = "LANDMARK.SF_DorsalPost" + caseNumberString
   cmdList = (progName,
              "-surface-border-nibbler",
              inflatedCoordFileName,
              inputTopoFile,
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
   #finishedSylvianBorderName = "LANDMARK.SylvianFissure.Case" + caseNumberString
   ventralFrontalExtremeFocusName = "SF_VentralFrontalExtreme.Case" + caseNumberString
   cmdList = (progName,
              "-surface-border-nibbler",
              inflatedCoordFileName,
              inputTopoFile,
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
## Identify all sylvian fissures
def identifyAllSylvianFissure() :
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
   ventralFrontalExtremeFocusName = "SF_VentralFrontalExtreme.Case"
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
   ventralFrontalFocusName = "SF_VentralFrontal.Case"
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
   # Add paint colors
   #
   #cmdList = (progName,
   #           "-color-file-add-color",
   #           areaColorFileName,
   #           areaColorFileName,
   #           "GYRAL.STG",
  #            str(100),
  #            str(255),
  #            str(150),
  #            "-point-size 1",
  #            "-symbol SPHERE");
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
   # ID the central sulcus on the specified cases
   #
   for caseID in range(startCaseNumber, endCaseNumber) :
      identifySylvianFissure(str(caseID))
      
##-----------------------------------------------------------------------------
##
## Identify the superior temporal gyrus
##
def identifySuperiorTemporalGyrus(caseNumberString) :
   #
   # Global variables
   #
   
   #
   # Assemble file names
   #
   borderProjectionFileName = borderProjectionFilePrefix \
                            + caseNumberString \
                            + borderProjectionFileSuffix
   fiducialCoordFileName = fiducialCoordFilePrefix \
                           + caseNumberString \
                           + fiducialCoordFileSuffix
   fociProjectionFileName = fociProjectionFilePrefix \
                            + caseNumberString \
                            + fociProjectionFileSuffix
   inflatedCoordFileName = inflatedCoordFilePrefix \
                           + caseNumberString \
                           + inflatedCoordFileSuffix
   tempRotateY45InflatedCoordFileName = "Temp_Rotated_Y45_Inflated_Case" \
                                        + caseNumberString \
                                        + ".coord"   
   veryInflatedCoordFileName = veryInflatedCoordFilePrefix \
                           + caseNumberString \
                           + veryInflatedCoordFileSuffix
   paintFileName = paintFilePrefix \
                   + caseNumberString \
                   + paintFileSuffix
   paintFileGyralSTGName = paintFilePrefix \
                   + caseNumberString \
                   + ".GyralSTG" \
                   + paintFileSuffix
   paintColumnName = "\"Sulcus ID.Buck_Case" \
                     + caseNumberString \
                     + ".R\""
   roiFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "STG" \
                 + ".roi"
   roiStringentFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "STG_Stringent" \
                 + ".roi"

   shapeCurvatureFileName = shapeCurvatureFilePrefix \
                          + caseNumberString \
                          + shapeCurvatureFileSuffix
               
   #
   # Create an ROI that identifies the superior temporal sulcus
   # with paint name SUL.STS
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
              roiFileName,
              "-paint",
                 paintFileName,
                 paintColumnName,   # column name
                 "SUL.STS",                       # paint name
                 "NORMAL")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create foci at some limits of ROI
   #
   stsVentralFocusName = "STS_Inflated_Ventral.Case" + caseNumberString
   stsDorsalFocusName = "STS_Inflated_Dorsal.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-z-min " + stsVentralFocusName,
              "-z-max " + stsDorsalFocusName)
   runCommand(cmdList)

   #
   # Find extremum point for temporal pole
   #
   temporalPoleFocusName = "TemporalPole.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
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
   cmdList = (progName,
              "-surface-apply-transformation-matrix",
              inflatedCoordFileName,
              inputTopoFile,
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
   stgPathFocusName = "STG-posterior.Case" + caseNumberString
   stgRoiFileName = "Case" + caseNumberString + ".STG.roi"
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              tempRotateY45InflatedCoordFileName,
              inputTopoFile,
              fociProjectionFileName,
              temporalPoleFocusName,
              fociProjectionFileName,
              stgPathFocusName,
              "Z-POS",
              str(100000.0),
              str(100000.0),
              str(100000.0),
              "-create-roi-from-path",
                 stgRoiFileName)
              #"-start-offset  3.0  0.0  0.0")
   runCommand(cmdList)
   
   #
   # Limit the ROI so it does not go beyond temporal pole
   # or beyond the Y position of the ventral tip of the
   # central sulcus
   #
   cesVentralFocusName = "CeS-ventral.Case" + caseNumberString
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              inputTopoFile,
              stgRoiFileName,
              stgRoiFileName,
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
   stgCesLimitFocusName = "STG-CES-limit.Case" + caseNumberString
   #stgPosteriorFocusName = "STG-posterior.Case" + caseNumberString
   #stgVentralFocusName   = "STG-ventral.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
              stgRoiFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-y-min " + stgCesLimitFocusName)
              #"-z-min " + stgVentralFocusName)
   runCommand(cmdList)
   
   #
   # Create the draw border comand to draw the LANDMARK.SF_STSant border
   #
   borderSampling = 1.0
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              inflatedCoordFileName,
              inputTopoFile,
              fociProjectionFileName,
              stgCesLimitFocusName,
              temporalPoleFocusName,
              stgRoiFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.SF_STSant",
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
   cesVentralFocusName = "CeS-ventral.Case" + caseNumberString
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              inputTopoFile,
              stgRoiFileName,
              stgRoiFileName,
              "-dilate-paint",
                 paintFileName,
                 paintColumnName,
                 "SUL.SF",  # SF may be too big
                 str(dilateIterations),
              "-dilate-paint",
                 paintFileName,
                 paintColumnName,
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
              paintColumnName,
              "GYRAL.STG",
              "-assign-from-roi-file",
                 stgRoiFileName);
   runCommand(cmdList)
   
   



   ###############################################
   #
   #  STOP STOP STOP
   #
   return
   




   
   #
   # Create an ROI that identifies the central sulcus nodes
   # with paint name GYRAL.STG and Shape folding ranging 0.10 to 100
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
              roiFileName,
              "-paint",
                 paintFileGyralSTGName,
                 paintColumnName,   # column name
                 "GYRAL.STG",                       # paint name
                 "NORMAL",                         # normal selection
              "-shape",
                 shapeCurvatureFileName,
                 "1",               # column number
                 "0.1  100.0",      # value range
                 "AND")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create limits at posterior and ventral ends of GYRAL.STG
   #
   stgPosteriorFocusName = "STG-posterior.Case" + caseNumberString
   stgVentralFocusName   = "STG-ventral.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
              fociProjectionFileName,
              fociProjectionFileName,
              "-y-min " + stgPosteriorFocusName,
              "-z-min " + stgVentralFocusName)
   runCommand(cmdList)

   #
   # Create an ROI that identifies the superior temporal gyrus nodes
   # with paint name GYRAL.STG and Shape folding ranging 0.16 to 100
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              inputTopoFile,
              roiStringentFileName,
              roiStringentFileName,
              "-paint",
                 paintFileGyralSTGName,
                 paintColumnName,   # column name
                 "GYRAL.STG",                       # paint name
                 "NORMAL",                         # normal selection
              "-shape",
                 shapeCurvatureFileName,
                 "1",               # column number
                 "0.16  100.0",      # value range
                 "AND")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create the draw border comand to draw the LANDMARK.SF_STSant border
   #
   borderSampling = 1.0
   cmdList = (progName,
              "-surface-border-draw-geodesic",
              inflatedCoordFileName,
              inputTopoFile,
              fociProjectionFileName,
              stgPosteriorFocusName,
              stgVentralFocusName,
              roiStringentFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.SF_STSant",
              str(borderSampling))
   runCommand(cmdList)
   
   

##-----------------------------------------------------------------------------
##
## Identify all superior temporal gyri
def identifyAllSuperiorTemporalGyri() :
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
   # Add paint colors
   #
   #cmdList = (progName,
   #           "-color-file-add-color",
   #           areaColorFileName,
   #           areaColorFileName,
   #           "GYRAL.STG",
  #            str(100),
  #            str(255),
  #            str(150),
  #            "-point-size 1",
  #            "-symbol SPHERE");
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
   # ID the central sulcus on the specified cases
   #
   for caseID in range(startCaseNumber, endCaseNumber) :
      identifySuperiorTemporalGyrus(str(caseID))
      
##-----------------------------------------------------------------------------
##
## Identify the calcarine sulcus
##
def identifyCalcarineSulcus(caseNumberString) :
   #
   # Global variables
   #
   
   #
   # Assemble file names
   #
   borderProjectionFileName = borderProjectionFilePrefix \
                            + caseNumberString \
                            + borderProjectionFileSuffix
   fiducialCoordFileName = fiducialCoordFilePrefix \
                           + caseNumberString \
                           + fiducialCoordFileSuffix
   fociProjectionFileName = fociProjectionFilePrefix \
                            + caseNumberString \
                            + fociProjectionFileSuffix
   inflatedCoordFileName = inflatedCoordFilePrefix \
                           + caseNumberString \
                           + inflatedCoordFileSuffix
   paintFileName = paintFilePrefix \
                   + caseNumberString \
                   + paintFileSuffix
   paintColumnName = "\"Sulcus ID.Buck_Case" \
                     + caseNumberString \
                     + ".R\""
   roiFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "CaS" \
                 + ".roi"
   roiStringentFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "CaS_Stringent" \
                 + ".roi"

   shapeCurvatureFileName = shapeCurvatureFilePrefix \
                          + caseNumberString \
                          + shapeCurvatureFileSuffix
               
   #
   # Create an ROI that identifies the central sulcus nodes
   # with paint name SUL.CaS and Shape folding ranging -100 to -0.10
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
              roiFileName,
              "-paint",
                 paintFileName,
                 paintColumnName,   # column name
                 "SUL.CaS",                       # paint name
                 "NORMAL",                         # normal selection
              "-shape",
                 shapeCurvatureFileName,
                 "1",               # column number
                 "-100.0 -0.10",    # value range
                 "AND")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create foci at some limits of ROI
   #
   casAnteriorFocusName = "CaS-anterior.Case" + caseNumberString
   casPosteriorFocusName = "CaS-posterior.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
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
              inputTopoFile,
              roiStringentFileName,
              roiStringentFileName,
              "-paint",
                 paintFileName,
                 paintColumnName,   # column name
                 "SUL.CaS",                       # paint name
                 "NORMAL",                        # normal selection
              "-shape",
                 shapeCurvatureFileName,
                 "1",               # column number
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
              inputTopoFile,
              fociProjectionFileName,
              casPosteriorFocusName,
              casAnteriorFocusName,
              roiStringentFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.CalcarineSulcus",
              str(borderSampling))
   runCommand(cmdList)
   
   #
   # Find extremum point for lateral side
   #
   casPosteriorExtremeFocusName = "CaS-PosteriorExtreme.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
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
              inputTopoFile,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.CalcarineSulcus",
              "LANDMARK.CalcarineSulcus",
              fociProjectionFileName,
              casPosteriorExtremeFocusName,
              "-within-y-distance 24")
   runCommand(cmdList)

##-----------------------------------------------------------------------------
##
## Identify all calcarine sulci
def identifyAllCalcarineSulci() :
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
   # ID the central sulcus on the specified cases
   #
   for caseID in range(startCaseNumber, endCaseNumber) :  
      identifyCalcarineSulcus(str(caseID))
      
##-----------------------------------------------------------------------------
##
## Identify the central sulcus
##
def identifyCentralSulcus(caseNumberString) :
   #
   # Global variables
   #
   
   #
   # Assemble file names
   #
   borderProjectionFileName = borderProjectionFilePrefix \
                            + caseNumberString \
                            + borderProjectionFileSuffix
   fiducialCoordFileName = fiducialCoordFilePrefix \
                           + caseNumberString \
                           + fiducialCoordFileSuffix
   fociProjectionFileName = fociProjectionFilePrefix \
                            + caseNumberString \
                            + fociProjectionFileSuffix
   inflatedCoordFileName = inflatedCoordFilePrefix \
                           + caseNumberString \
                           + inflatedCoordFileSuffix
   paintFileName = paintFilePrefix \
                   + caseNumberString \
                   + paintFileSuffix
   paintColumnName = "\"Sulcus ID.Buck_Case" \
                     + caseNumberString \
                     + ".R\""
   roiFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "CeS" \
                 + ".roi"
   roiStringentFileName = "Human.case" \
                 + caseNumberString \
                 + "." \
                 + "CeS_Stringent" \
                 + ".roi"

   shapeCurvatureFileName = shapeCurvatureFilePrefix \
                          + caseNumberString \
                          + shapeCurvatureFileSuffix
               
   #
   # Create an ROI that identifies the central sulcus nodes
   # with paint name SUL.CeS and Shape folding ranging -100 to -0.10
   #
   cmdList = (progName,
              "-surface-region-of-interest-selection",
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
              roiFileName,
              "-paint",
                 paintFileName,
                 paintColumnName,   # column name
                 "SUL.CeS",                       # paint name
                 "NORMAL",                         # normal selection
              "-shape",
                 shapeCurvatureFileName,
                 "1",               # column number
                 "-100.0 -0.10",    # value range
                 "AND")             # AND with previous selection
   runCommand(cmdList)

   #
   # Create foci at some limits of ROI
   #
   cesMedialFocusName = "CeS-medial.Case" + caseNumberString
   cesVentralFocusName = "CeS-ventral.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-foci-at-limits",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
              roiFileName,
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
              inputTopoFile,
              roiStringentFileName,
              roiStringentFileName,
              "-paint",
                 paintFileName,
                 paintColumnName,   # column name
                 "SUL.CeS",                       # paint name
                 "NORMAL",                        # normal selection
              "-shape",
                 shapeCurvatureFileName,
                 "1",               # column number
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
              inputTopoFile,
              fociProjectionFileName,
              cesVentralFocusName,
              cesMedialFocusName,
              roiStringentFileName,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.CentralSulcus",
              str(borderSampling))
   runCommand(cmdList)
   
   #
   # Find extremum point for lateral side
   #
   cesVentralExtremeFocusName = "CeS-VentralExtreme.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
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
   cesMedialExtremeFocusName = "CeS-MedialExtreme.Case" + caseNumberString
   cmdList = (progName,
              "-surface-place-focus-at-extremum",
              fiducialCoordFileName,
              inflatedCoordFileName,
              inputTopoFile,
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
              inputTopoFile,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.CentralSulcus",
              "LANDMARK.CentralSulcus",
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
              inputTopoFile,
              borderProjectionFileName,
              borderProjectionFileName,
              "LANDMARK.CentralSulcus",
              "LANDMARK.CentralSulcus",
              fociProjectionFileName,
              cesMedialExtremeFocusName,
              "-within-x-distance ",
                 str(trimValue))
   runCommand(cmdList)
   
##-----------------------------------------------------------------------------
##
## Identify all central sulci
def identifyAllCentralSulci() :
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
   # ID the central sulcus on the specified cases
   #
   for caseID in range(startCaseNumber, endCaseNumber) :
      identifyCentralSulcus(str(caseID))
      
##-----------------------------------------------------------------------------
##
## Combine paint files
##
def combinePaintFiles() :
   #
   # Global variables
   #
   global outputCompositePaintFileName
   global outputPaintFiles
   global progName

   #
   # Create the command
   #
   cmd = progName \
       + " -paint-composite " \
       + outputCompositePaintFileName \
       + " " \
       + " ".join(outputPaintFiles)
       
   print "\nExecuting: %s\n" % cmd
   result = os.system(cmd)
   if (result != 0) :
      print "COMMAND FAILED: "
      print "   ", cmd
      os._exit(-1)

##-----------------------------------------------------------------------------
##
## Run sulcal identification
##
def runSulcalIdentificationSome() :
   #
   # Global variables
   #
   global clusterRatio
   global fiducialCoordFiles
   global inputPaintFile
   global inputShapeFile
   global inputTopoFile
   global outputPaintFiles
   global progName

   #
   # loop through the coord files
   #
   paintShapeColumnNumber = 0
   for coordFile in fiducialCoordFiles :
      #
      # Create the command string
      #
      paintShapeColumnNumber = paintShapeColumnNumber + 1
      cmdList = (progName, 
                 "-surface-sulcal-named-identification", 
                 coordFile, 
                 inputTopoFile, 
                 inputPaintFile, 
                 str(paintShapeColumnNumber), 
                 inputShapeFile, 
                 str(paintShapeColumnNumber),
                 str(clusterRatio),
                 outputPaintFiles[paintShapeColumnNumber-1])
      

      cmd = " ".join(cmdList)
      print "\nExecuting: %s\n" % cmd
      result = os.system(cmd)
      if (result != 0) :
         print "COMMAND FAILED: "
         print "   ", cmd
         os._exit(-1)


##-----------------------------------------------------------------------------
##
## Run ALL sulcal identification
##
def runSulcalIdentificationAll() :
   #
   # Global variables
   #
   global clusterRatio
   global fiducialCoordFiles
   global inputPaintFile
   global inputShapeFile
   global inputTopoFile
   global outputPaintFiles
   global postCentralSulcusOffset
   global postCentralSulcusStdDevSquared
   global postCentralSulcusSplit
   global probabilisticSulcusVolumeListFileName
   global progName
   global veryInflatedCoordFile

   #
   # loop through the coord files
   #
   paintShapeColumnNumber = 0
   for coordFile in fiducialCoordFiles :
      #
      # Create the command string
      #
      paintShapeColumnNumber = paintShapeColumnNumber + 1
      cmdList = (progName, 
                 "-surface-sulcal-named-all-identification", 
                 coordFile, 
                 veryInflatedCoordFile,
                 inputTopoFile, 
                 inputPaintFile, 
                 str(paintShapeColumnNumber), 
                 inputShapeFile, 
                 str(paintShapeColumnNumber),
                 probabilisticSulcusVolumeListFileName,
                 str(postCentralSulcusOffset),
                 str(postCentralSulcusStdDevSquared),
                 str(postCentralSulcusSplit),
                 outputPaintFiles[paintShapeColumnNumber-1])
      

      cmd = " ".join(cmdList)
      print "\nExecuting: %s\n" % cmd
      result = os.system(cmd)
      if (result != 0) :
         print "COMMAND FAILED: "
         print "   ", cmd
         os._exit(-1)


##-----------------------------------------------------------------------------
##
## Print help information
##
def printHelp() :
   print "Options"
   #print "-cas     Identify calcarine sulcus landmark."
   #print "-ces     Identify central sulcus landmark."
   print "-cpf     Combine the output paint files into a composite file."
   print "-landmarks  Identify Calcarine, Central, Sylvian, Superior Temporal"
   print "-mw      Identify medial wall landmark."
   #print "-sf      Identify sylvian fissure landmark."
   #print "-stg     Identify superior temporal gyrus landmark."
   print "-run-all      Run the sulcal (Paint) identification of ALL sulci."
   print "-run-some     Run the sulcal (Paint)identification of CeSi and SF."

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
doCombinePaint = False
doMedialWall = False
doRemoveFiles = False
doRunSome = False
doRunAll  = False
doSuperiorTemporalGyrus = False
doSylvianFissure = False

#
# Process arguments
# 
for i in range (1, numArgs) :
   arg = sys.argv[i]
   if (arg == "-cas") :
      doCalcarineSulcus = True
   elif (arg == "-ces") :
      doCentralSulcus = True
   elif (arg == "-cpf") :
      doCombinePaint = True
   elif (arg == "-landmarks") :
      doCalcarineSulcus = True
      doCentralSulcus = True
      doSylvianFissure = True
      doSuperiorTemporalGyrus = True      
   elif (arg == "-mw") :
      doMedialWall = True
   elif (arg == "-run-all") :
      doRunAll = True
   elif (arg == "-run-some") :
      doRunSome = True
   elif (arg == "-sf") :
      doSylvianFissure = True
   elif (arg == "-stg") :
      doSuperiorTemporalGyrus = True
   else:
      print "ERROR Invalid option: ", arg
      os._exit(-1)

if (doRunAll) :
   runSulcalIdentificationAll()

if (doRunSome) :
   runSulcalIdentificationSome()

if (doCombinePaint) :
   combinePaintFiles()
   
if (doCentralSulcus or \
    doCalcarineSulcus or \
    doSylvianFissure or \
    doSuperiorTemporalGyrus) :
   removeSulcalIdentficationBorderAndFociProjectionFiles()
   
if (doCentralSulcus) :
   identifyAllCentralSulci()
   
if (doCalcarineSulcus) :
   identifyAllCalcarineSulci()

if (doSuperiorTemporalGyrus) :
   identifyAllSuperiorTemporalGyri()

if (doSylvianFissure) :
   identifyAllSylvianFissure()
  
if (doMedialWall) :
   identifyAllMedialWalls()

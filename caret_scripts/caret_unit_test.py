#!/usr/bin/python

#
# Imports
#
import os
import re
import sys
import time

#
# Globals
#
cleanupOutputFilesFlag = False
correctImageDirectory = "correct_images"
progName = "/Users/john/caret5_osx/caret_source/caret_command/caret_command"
#progName = "/usr/local/caret/bin/caret_command"
problemCount = 0
problemMessage = ""

##-----------------------------------------------------------------------------
##
## Delete any files matching the "glob regular expression"
##
def deleteFiles(regularExpression) :
   print "Removing files matching %s" % regularExpression

         
   for fileName in os.listdir(".") :
      if (os.path.isfile(fileName)) :
         if (re.compile(regularExpression).search(fileName)) :
            #print "file: %s" % fileName
            os.remove(fileName)

##-----------------------------------------------------------------------------
##
## Set the random seed
def setRandomSeed() :
   #
   # Global variables
   #
   global problemCount
   global problemMessage

   cmd = progName + " -statistic-set-random-seed 1001"
   result = os.system(cmd)
   if (result != 0) :
      problemCount   += 1
      problemMessage += ("Setting statistic random seed failed.\n")
      
   return result;

##-----------------------------------------------------------------------------
##
## Test statistics library
##
def testStatistics() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global problemCount
   global problemMessage
   global progName
   
   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   #
   # Test the statistical algrorithms
   #
   cmd = progName         \
       + " -statistic-unit-test false "
   print "cmd: %s" % (cmd)

   #
   # Run the command
   #
   result = os.system(cmd)
   if (result != 0) :
      problemCount += 1
      problemMessage += ("Statistics library testing failed.\n")

   return result
   
##-----------------------------------------------------------------------------
##
## Test graphics rendering of surfaces and volumes
##
def testRendering() :
   #
   # globals
   #
   global cleanupOutputFilesFlag
   global problemCount
   global problemMessage
   
   deleteFiles(".*\.jpg")

   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   specFileName = "rendering_unit_test_files.spec"
   sceneFileName = "Human.colin.R.RENDERING_TEST.scene"
   
   #renderSceneToImage($specFileName, $sceneFileName, "1", "render1.jpg");
   #renderSceneToImage($specFileName, $sceneFileName, "2", "render2.jpg");
   #renderSceneToImage($specFileName, $sceneFileName, "3", "render3.jpg");
   #renderSceneToImage($specFileName, $sceneFileName, "4", "render4.jpg");
   #renderSceneToImage($specFileName, $sceneFileName, "5", "render5.jpg");
   #renderSceneToImage($specFileName, $sceneFileName, "6", "render6.jpg");
   #renderSceneToImage($specFileName, $sceneFileName, "7", "render7.jpg");
   
   fileList = [ "render1.jpg",
                "render2.jpg",
                "render3.jpg",
                "render4.jpg",
                "render5.jpg",
                "render6.jpg",
                "render7.jpg" ]
                
   counter = 1
   for imageFile in fileList :
      if (renderSceneToImage(specFileName, sceneFileName, counter, imageFile) != 0) :
         problemCount += 1
         problemMessage += ("Rendering test %d failed\n" % counter)

      counter += 1

##-----------------------------------------------------------------------------
##
## Test scenes
##
def testScenes1() :
   #
   # globals
   #
   global cleanupOutputFilesFlag
   global problemCount
   global problemMessage
   
   deleteFiles(".*\.jpg")

   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   specFileName = "PALS_B12.RIGHT.STANDARD-SCENES.73730.spec"
   sceneFileName = "PALS_B12.RIGHT.STANDARD-for-Starting-ANALYSES.scene"
   
   fileList = [ "render1.jpg",
                "render2.jpg",
                "render3.jpg",
                "render4.jpg",
                "render5.jpg",
                "render6.jpg",
                "render7.jpg",
                "render8.jpg",
                "render9.jpg",
                "render10.jpg",
                "render11.jpg",
                "render12.jpg",
                "render13.jpg" ]
                
   counter = 1
   for imageFile in fileList :
      if (renderSceneToImage(specFileName, sceneFileName, counter, imageFile) != 0) :
         problemCount += 1
         problemMessage += ("Scenes 1 test %d failed\n" % counter)

      counter += 1

##-----------------------------------------------------------------------------
##
## Test scenes
##
def testScenes2() :
   #
   # globals
   #
   global cleanupOutputFilesFlag
   global problemCount
   global problemMessage
   
   deleteFiles(".*\.jpg")

   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   specFileName = "PALS_B12.RIGHT.DEMO.73730.spec"
   sceneFileName = "PALS_B12.RIGHT.DEMO.scene"
   
   fileList = [ "render1.jpg",
                "render2.jpg",
                "render3.jpg",
                "render4.jpg",
                "render5.jpg",
                "render6.jpg",
                "render7.jpg",
                "render8.jpg",
                "render9.jpg",
                "render10.jpg",
                "render11.jpg",
                "render12.jpg",
                "render13.jpg",
                "render14.jpg" ]
                
   counter = 1
   for imageFile in fileList :
      if (renderSceneToImage(specFileName, sceneFileName, counter, imageFile) != 0) :
         problemCount += 1
         problemMessage += ("Scenes 2 test %d failed\n" % counter)

      counter += 1

##-----------------------------------------------------------------------------
##
## Render a scene to an image file
##
def renderSceneToImage(specFileName, sceneFileName, sceneNumber, imageName) :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global progName
   
   #
   # Create the image
   #
   cmd = progName         \
       + " -show-scene "  \
       + specFileName     \
       + " "              \
       + sceneFileName    \
       + " "              \
       + str(sceneNumber) \
       + " -image-file "  \
       + imageName        \
       + " 1"
   print "cmd: %s" % (cmd)

   #
   # Run the command
   #
   result = os.system(cmd)

   #
   # Successful
   #
   if (result == 0) :
      #
      # Compare with previously created valid image
      #
      cmd = progName           \
          + " -image-compare " \
          + imageName          \
          + " "                \
          + correctImageDirectory + "/" + imageName
      result = os.system(cmd)
      
   return result

##-----------------------------------------------------------------------------
##
## Test metric mathematics
##
def testMetricMathematics() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Metric file names
   #
   inputMetricFileName = "input.metric"
   infixOutputMetricFileName = "infixOutput.metric"
   postfixOutputMetricFileName = "postfixOutput.metric"
   
   #
   # Remove output metric
   #
   deleteFiles(infixOutputMetricFileName)
   deleteFiles(postfixOutputMetricFileName)
   
   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
      
   #
   # Test postfix
   #
   cmd = "%s %s %s %s %s %s" % (progName,
                                "-metric-math-postfix",
                                inputMetricFileName,
                                postfixOutputMetricFileName,
                                "postfix1",
                                "\"@one@   @two@   *   @three@   @four@   *   -\"")
   print "Executing: %s" % cmd
   result = os.system(cmd)
   
   if (result == 0) :
      correctFile = "./results/" + postfixOutputMetricFileName
      tol = 1.0
      cmd = "%s -caret-data-file-compare %s %s %f" % (progName, postfixOutputMetricFileName, correctFile, tol)
      result = os.system(cmd)
   
   if (result != 0) :
      problemCount += 1
      problemMessage = problemMessage + "Metric math postfix failed.\n"
      
   #
   # Test infix 1 (normal)
   #
   cmd = "%s %s %s %s %s %s" % (progName,
                                "-metric-math",
                                inputMetricFileName,
                                infixOutputMetricFileName,
                                "math1",
                                "\"max2[@one@ * @two@, @three@ * @four@]\"")
   print "Executing: %s" % cmd
   result1 = os.system(cmd)
   
   #
   # Test infix 2 (normal)
   #
   cmd = "%s %s %s %s %s %s" % (progName,
                                "-metric-math",
                                infixOutputMetricFileName,
                                infixOutputMetricFileName,
                                "math2",
                                "\"@one@ - @two@ * @three@ + @four@\"")
   print "Executing: %s" % cmd
   result2 = os.system(cmd)
   
   #
   # Test infix 3 (normal)
   #
   cmd = "%s %s %s %s %s %s" % (progName,
                                "-metric-math",
                                infixOutputMetricFileName,
                                infixOutputMetricFileName,
                                "math3",
                                "\"sqrt[@three@] + log10[nodeavg]\"")
   print "Executing: %s" % cmd
   result3 = os.system(cmd)
   
   allResults = result1 \
                + result2 \
                + result3
                
   if (allResults == 0) :
      correctFile = "./results/" + infixOutputMetricFileName
      tol = 1.0
      cmd = "%s -caret-data-file-compare %s %s %f" % (progName, infixOutputMetricFileName, correctFile, tol)
      allResults = os.system(cmd)
   
   if (allResults != 0) :
      problemCount += 1
      problemMessage = problemMessage + "Metric math infix failed.\n"
      
##-----------------------------------------------------------------------------
##
## Test map volume to surface metric
##
def testMapVolumeToSurfaceMetric() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Remove image and CYCLE coord files
   deleteFiles(".*\.metric")
   
   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   option = "-volume-map-to-surface"
   topo = "Human.colin.Cerebral.R.CLOSED.71723.topo"
   fiducial = "Human.colin.Cerebral.R.FIDUCIAL.TLRC.711-2B.71723.coord"
   metric = "Human.colin.results.metric"
   volume = "CORBETTA_EtAl98.AttentionShift.PopAvg+orig.HEAD"
   cmd = "%s %s %s %s %s %s %s %s %s" % (progName, \
                                option,   \
                                fiducial, \
                                topo,     \
                                metric,   \
                                metric,   \
                                "METRIC_STRONGEST_VOXEL",     \
                                volume,   \
                                "-sv 3.0")

   print "Executing: %s" % cmd
   result = os.system(cmd)
   if (result == 0) :
      file = metric
      correctFile = "./results/" + file
      tol = 1.0
      cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
      result = os.system(cmd)
   
   if (result != 0) :
      problemCount += 1
      problemMessage = problemMessage + "Map Volume to Surface Metric test failed.\n"
   
##-----------------------------------------------------------------------------
##
## Test map volume to surface paint
##
def testMapVolumeToSurfacePaint() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Remove image and CYCLE coord files
   deleteFiles(".*\.paint")
   
   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   option = "-volume-map-to-surface"
   topo = "Human.colin.Cerebral.R.CLOSED.71723.topo"
   fiducial = "Human.colin.Cerebral.R.FIDUCIAL.TLRC.711-2B.71723.coord"
   paint = "Human.colin.results.paint"
   volume = "brodmann.nii.gz"
   cmd = "%s %s %s %s %s %s %s %s" % (progName, \
                                option,   \
                                fiducial, \
                                topo,     \
                                paint,   \
                                paint,   \
                                "PAINT_ENCLOSING_VOXEL",     \
                                volume)

   print "Executing: %s" % cmd
   result = os.system(cmd)
   if (result == 0) :
      file = paint
      correctFile = "./results/" + file
      tol = 1.0
      cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
      result = os.system(cmd)
   
   if (result != 0) :
      problemCount += 1
      problemMessage = problemMessage + "Map Volume to Surface Paint test failed.\n"
   
##-----------------------------------------------------------------------------
##
## Test flat multiresolution morphing
##
def testFlatMorphing() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Remove image and CYCLE coord files
   deleteFiles(".*\.jpg")
   deleteFiles(".*CYCLE.*\.coord")
   
   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   option = "-surface-flat-multi-morph"
   spec = "Human.1582.L.FULL.spec"
   topo = "Human.1582.L.Full.CUT.65950.topo"
   fiducial = "Human.1582.L.Full.FIDUCIAL.65950.coord"
   flat = "Human.1582.L.Full.InitialFlat.65950.coord"
   cmd = "%s %s %s %s %s %s" % (progName, option, spec, fiducial, flat, topo)

   print "Executing: %s" % cmd
   result = os.system(cmd)
   if (result == 0) :
      file = "Human.1582.L.Full.FLAT_CYCLE5_OVERLAP_SMOOTH.65950.coord"
      correctFile = "./results/" + file
      tol = 1.0
      cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
      result = os.system(cmd)
   
   if (result != 0) :
      problemCount += 1
      problemMessage = problemMessage + "Flat Multi-Resolution Morphing test failed.\n"
   
##-----------------------------------------------------------------------------
##
## Test spherical multiresolution morphing
##
def testSphericalMorphing() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Delete image and CYCLE coord files
   #
   deleteFiles(".*\.jpg")
   deleteFiles(".*CYCLE.*\.coord")

   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   option = "-surface-sphere-multi-morph"
   spec = "Human.1582.L.FULL.spec"
   topo = "Human.1582.L.Full.CLOSED.65950.topo"
   fiducial = "Human.1582.L.Full.FIDUCIAL.65950.coord"
   sphere = "Human.1582.L.Full.SPHERE.65950.coord"
   cmd = "%s %s %s %s %s %s" % (progName, option, spec, fiducial, sphere, topo)

   print "Executing: %s" % cmd
   result = os.system(cmd)
   if (result == 0) :
      file = "Human.1582.L.Full.SPHERE_CYCLE4.65950.coord"
      correctFile = "./results/" + file
      tol = 1.0
      cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
      result = os.system(cmd)
   
   if (result != 0) :
      problemCount += 1
      problemMessage = problemMessage + "Spherical Multi-Resolution Morphing test failed.\n"

##-----------------------------------------------------------------------------
##
## Test spherical registration
##
def testSphericalRegistration() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Delete image and deformed coord files
   #
   os.chdir("INDIVIDUAL.1582.L")
   deleteFiles("deformed.*")
   os.chdir("..")
   os.chdir("ATLAS_LEFT_HEM")
   deleteFiles("deformed.*")
   os.chdir("..")
   
   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   command = "-surface-register-sphere-spec-only"
   options = "N"
   defMapFile = "\"\""
   indivSpec = "INDIVIDUAL.1582.L/Human.1582.L.REGISTER-with-ATLAS.03-05.65950.spec"
   atlasSpec = "ATLAS_LEFT_HEM/Human.colin.L.REGISTER-to-INDIVIDUAL.03-05.71785.spec"
   cmd = "%s %s %s %s %s %s" % (progName, command, options, defMapFile, indivSpec, atlasSpec)
   
   print "Executing: cmd %s" % cmd
   result = os.system(cmd)
   if (result == 0) :
      os.chdir("ATLAS_LEFT_HEM")
      
      fileList = ("deformed_Human.1582.L.*.deform_map",
                  "deformed_Human.1582.L.Eye-movements_attention.71785.metric",
                  "deformed_Human.1582.L.FIDUCIAL.smMW.AC-orig.LPI.71785.coord",
                  "deformed_two.71785.foci",
                  "deformed_Human.1582.L.Full.FLAT.CartStd.71785.topo",
                  "deformed_Human.1582.L.Full.LANDMARKS.ForSphericalRegistration.71785.borderproj")
      for file in fileList :
         correctFile = "./results/" + file
         tol = 1.0
         cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
         result += os.system(cmd)
      
      os.chdir("..")
   
   if (result != 0) :
      problemCount += 1
      problemMessage = problemMessage + "Spherical registration test failed.\n"

##-----------------------------------------------------------------------------
##
## Test segmentation and generation of volume, surface, paint, and shape files
##
def testSegmentation() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Delete image and deformed coord files
   #
   deleteFiles(".*\.jpg")
   deleteFiles("Human\.case9\.R.*")
   deleteFiles("RadialPositionMap.*")
   deleteFiles("params_file.*")
   
   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   #              1111111111
   #     1234567890123456789
   op = "YYYYYYYYYNYYYYY"
   command = "-volume-segment"
   gray = 57
   white = 106
   pad = "NNNNNN"
   structure = "RIGHT"
   specName = "Human.TestSegmentation.R.spec"
   volName = "Human.AnatomyVolume.R+orig.nii"
   volType = "NIFTI";
   cmd  = "%s %s %s %s %s %d %d %s %s %s" % \
          (progName, command, volName, specName, op, gray, white, pad, structure, volType)
   
   print "Executing: %s" % cmd
   result = os.system(cmd)
   if (result == 0) :
      fileList = ("Human.case9.R.Segmentation.nii",
                  "Human.case9.R.Segmentation_vent.nii",
                  "RadialPositionMap+orig.nii",
                  "Human.case9.R.Segment_ErrorCorrected.nii",
                  "Human.case9.R.CerebralHull.nii",
                  "Human.case9.R.Fiducial.*.coord",
                  "Human.case9.R.Inflated.*.coord",
                  "Human.case9.R.paint_file_*.paint",
                  "Human.case9.R.surface_shape_file_*.surface_shape")
      
      for file in fileList :
         correctFile = "./results/" + file
         tol = 1.0;
         cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
         result += os.system(cmd);
   
   if (result != 0) :
      problemMessage += "Segmentation test failed.\n";
      problemCount += 1;


##-----------------------------------------------------------------------------
##
## Test one sample T-Test
##
def testOneSampleTTest() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Names of output files
   #
   fileNamePrefix      = "output-one-sample"
   outputTMap          = fileNamePrefix + "_TMap.surface_shape"
   outputShuffledTMap  = fileNamePrefix + "_ShuffledTMap.surface_shape"
   outputClustersPaint = fileNamePrefix + "_TMapClusters.paint"
   outputClustersMetric = fileNamePrefix + "_TMapClusters.metric"
   outputReport        = fileNamePrefix + "_TMap_Significant_Clusters.txt"
   
   #
   # Delete previous output files
   #
   deleteFiles(outputTMap)
   deleteFiles(outputShuffledTMap)
   deleteFiles(outputClustersPaint)
   deleteFiles(outputClustersMetric)
   deleteFiles(outputReport)

   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   #
   # arguments for command
   #   
   argList = ("-metric-statistics-one-sample-t-test",
              "Composite_CON_young_left.surface_shape",
              "Human.PALS_B12.LEFT_AVG_B1-12.FIDUCIAL.clean.73730.coord",
              "Human.sphere_6.LEFT_HEM_OPEN.73730.topo",
              "Human.PALS_B12.B1-12_LEFT_DISTORTION-vs-AVG-FIDUCIAL_ONLY.73730.surface_shape",
              "1",     # dist column number
              fileNamePrefix,
              "-3.0",  # negative threshold
              "3.0",   # positive threshold,
              "0.25",  # p-value,
              "0",     # variance smoothing iterations
              "0.0",   # variance smoothing strength
              "250",   # iterations
              "0.0",   # T-Test known mean
              "1")     # number of threads
              
   #
   # Assemble the command
   #
   cmd = progName
   for arg in argList :
      cmd = cmd + " " + arg
      
   #
   # Execute the command
   # 
   print "Executing: %s" % cmd
   result = os.system(cmd)
   print "One-sample-result-code: %d", result
   if (result == 0) :
      fileList = (outputTMap,
                  outputShuffledTMap,
                  outputClustersPaint)
      
      for file in fileList :
         print "comparing: %s" % (file)
         correctFile = "./results/" + file
         tol = 1.0;
         cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
         result += os.system(cmd)
   
   if (result != 0) :
      problemMessage += "One-Sample T-Test failed.\n"
      problemCount += 1

##-----------------------------------------------------------------------------
##
## Test two sample T-Test with unpooled variance
##
def testTwoSampleTTestUnpooled() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Names of output files
   #
   fileNamePrefix      = "output-two-sample-unpooled"
   outputTMap          = fileNamePrefix + "_TMap.surface_shape"
   outputShuffledTMap  = fileNamePrefix + "_ShuffledTMap.surface_shape"
   outputClustersPaint = fileNamePrefix + "_TMapClusters.paint"
   outputClustersMetric = fileNamePrefix + "_TMapClusters.metric"
   outputReport        = fileNamePrefix + "_TMap_Significant_Clusters.txt"
   
   #
   # Delete previous output files
   #
   deleteFiles(outputTMap)
   deleteFiles(outputShuffledTMap)
   deleteFiles(outputClustersPaint)
   deleteFiles(outputClustersMetric)
   deleteFiles(outputReport)

   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   #
   # arguments for command
   #   
   argList = ("-metric-statistics-two-sample-t-test",
              "NO_TRANSFORM",
              "UNPOOLED",
              "Composite_CON_young_left.surface_shape",
              "Composite_LFA_young_left.surface_shape",
              "Human.PALS_B12.LEFT_AVG_B1-12.FIDUCIAL.clean.73730.coord",
              "Human.sphere_6.LEFT_HEM_OPEN.73730.topo",
              "Human.PALS_B12.B1-12_LEFT_DISTORTION-vs-AVG-FIDUCIAL_ONLY.73730.surface_shape",
              "1",     # dist column number
              fileNamePrefix,
              "250",   # iterations
              "-3.0",  # negative threshold
              "3.0",   # positive threshold,
              "0.25",  # p-value,
              "25",    # variance smoothing iterations
              "0.5",   # variance smoothing strength
              "true",  # do Degrees-of-Freedom
              "true",  # do p-value
              "1")     # number of threads
              
   #
   # Assemble the command
   #
   cmd = progName
   for arg in argList :
      cmd = cmd + " " + arg
      
   #
   # Execute the command
   # 
   print "Executing: %s" % cmd
   result = os.system(cmd)
   if (result == 0) :
      fileList = (outputTMap,
                  outputShuffledTMap,
                  outputClustersPaint)
      
      for file in fileList :
         correctFile = "./results/" + file
         tol = 1.0;
         cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
         result += os.system(cmd)
   
   if (result != 0) :
      problemMessage += "Two-Sample T-Test Unpooled failed.\n"
      problemCount += 1

##-----------------------------------------------------------------------------
##
## Test two sample T-Test with pooled variance
##
def testTwoSampleTTestPooled() :
   #
   # Global variables
   #
   global cleanupOutputFilesFlag
   global correctImageDirectory
   global problemCount
   global problemMessage
   global progName

   #
   # Names of output files
   #
   fileNamePrefix      = "output-two-sample-pooled"
   outputTMap          = fileNamePrefix + "_TMap.surface_shape"
   outputShuffledTMap  = fileNamePrefix + "_ShuffledTMap.surface_shape"
   outputClustersPaint = fileNamePrefix + "_TMapClusters.paint"
   outputClustersMetric = fileNamePrefix + "_TMapClusters.metric"
   outputReport        = fileNamePrefix + "_TMap_Significant_Clusters.txt"
   
   #
   # Delete previous output files
   #
   deleteFiles(outputTMap)
   deleteFiles(outputShuffledTMap)
   deleteFiles(outputClustersPaint)
   deleteFiles(outputClustersMetric)
   deleteFiles(outputReport)

   #
   # If only cleaning up output files we are done
   #
   if (cleanupOutputFilesFlag) :
      return
   
   #
   # arguments for command
   #   
   argList = ("-metric-statistics-two-sample-t-test",
              "NO_TRANSFORM",
              "POOLED",
              "Composite_CON_young_left.surface_shape",
              "Composite_LFA_young_left.surface_shape",
              "Human.PALS_B12.LEFT_AVG_B1-12.FIDUCIAL.clean.73730.coord",
              "Human.sphere_6.LEFT_HEM_OPEN.73730.topo",
              "Human.PALS_B12.B1-12_LEFT_DISTORTION-vs-AVG-FIDUCIAL_ONLY.73730.surface_shape",
              "1",     # dist column number
              fileNamePrefix,
              "250",   # iterations
              "-3.0",  # negative threshold
              "3.0",   # positive threshold,
              "0.25",  # p-value,
              "25",    # variance smoothing iterations
              "0.5",   # variance smoothing strength
              "true",  # do Degrees-of-Freedom
              "true",  # do p-value
              "1")     # number of threads
              
   #
   # Assemble the command
   #
   cmd = progName
   for arg in argList :
      cmd = cmd + " " + arg
      
   #
   # Execute the command
   # 
   print "Executing: %s" % cmd
   result = os.system(cmd)
   if (result == 0) :
      fileList = (outputTMap,
                  outputShuffledTMap,
                  outputClustersPaint)
      
      for file in fileList :
         correctFile = "./results/" + file
         tol = 1.0;
         cmd = "%s -caret-data-file-compare %s %s %f" % (progName, file, correctFile, tol)
         result += os.system(cmd)
   
   if (result != 0) :
      problemMessage += "Two-Sample T-Test Pooled failed.\n"
      problemCount += 1

##-----------------------------------------------------------------------------
##
def printHelp() :
   print "Caret Unit Testing"
   print ""
   print "Options"
   print "   -all          Peform all tests"
   print "   -clean        Cleanup test files generated by this program"
   print "   -help         Print this help information"
   print "   -map-vol      Test map volumes to surface"
   print "   -metric-math  Test metric mathematics"
   print "   -morph        Test morphing"
   print "   -register     Test registration"
   print "   -render       Test rending"
   print "   -scenes       Test scenes"
   print "   -segment      Test segmentation"
   print "   -stat-lib     Test statistical library"
   print "   -surf-stat    Test surface statistics"
   print "   "
   print "More than one option may be specified."
   
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
# Flags to run the different tests
#
testMetricMathematicsFlag = False
testMapVolumeToSurfaceFlag = False
testMorphingFlag = False
testRegistrationFlag = False
testRenderingFlag = False
testSegmentationFlag = False
testScenesFlag = False
testStatsLibraryFlag = False
testSurfaceStatisticsFlag = False

doAllFlag = False

#
# Process arguments
#
for i in range (1, numArgs) :
   arg = sys.argv[i]
   if arg == "-all" :
      doAllFlag = True
   elif arg == "-clean" :
      doAllFlag = True
      cleanupOutputFilesFlag = True
   elif arg == "-help" :
      printHelp()
      os._exit(0)
   elif arg == "-map-vol" :
      testMapVolumeToSurfaceFlag = True
   elif arg == "-metric-math" :
      testMetricMathematicsFlag = True
   elif arg == "-morph" :
      testMorphingFlag = True
   elif arg == "-register" :
      testRegistrationFlag = True
   elif arg == "-render" :
      testRenderingFlag = True
   elif arg == "-scenes" :
      testScenesFlag = True
   elif arg == "-segment" :
      testSegmentationFlag = True
   elif arg == "-stat-lib" :
      testStatsLibraryFlag = True
   elif arg == "-surf-stat" :
      testSurfaceStatisticsFlag = True
   else:
      print "ERROR Invalid option: ", arg
      os._exit(-1)

if doAllFlag :
   testMapVolumeToSurfaceFlag = True
   testMetricMathematicsFlag = True
   testMorphingFlag = True
   testRegistrationFlag = True
   testRenderingFlag = True
   testScenesFlag = True
   testSegmentationFlag = True
   testStatsLibraryFlag = True
   testSurfaceStatisticsFlag = True

print "Unit testing started"

startTime = time.clock()

#
# Set the random seed so statistical results consistent
#
setRandomSeed()

#
# Test statistical algorithms
#
if testStatsLibraryFlag :
   testStatistics()

#
# Test Statistical One-Sample T-Test
#
if testSurfaceStatisticsFlag :
   os.chdir("statistics")
   testOneSampleTTest()
   os.chdir("..")

#
# Test Statistical Two-Sample T-Test with Unpooled Variance
#
if testSurfaceStatisticsFlag :
   os.chdir("statistics") 
   testTwoSampleTTestUnpooled()
   os.chdir("..")

#
# Test Statistical Two-Sample T-Test with Pooled Variance
#
if testSurfaceStatisticsFlag :
   os.chdir("statistics") 
   testTwoSampleTTestPooled()
   os.chdir("..")

#
# Test Map Volume to Surface
#
if testMapVolumeToSurfaceFlag :
   os.chdir("map_volume_to_surface_metric")
   testMapVolumeToSurfaceMetric()
   os.chdir("..")
   os.chdir("map_volume_to_surface_paint")
   testMapVolumeToSurfacePaint()
   os.chdir("..")

#
# Test Metric Math
#
if testMetricMathematicsFlag :   
   os.chdir("metric_math")
   testMetricMathematics()
   os.chdir("..")

#
# Test rendering code
#
if testRenderingFlag :
   os.chdir("rendering")
   testRendering()
   os.chdir("..")

#
# Test scene code
#
if testScenesFlag :
   os.chdir("scenes1")
   testScenes1()
   os.chdir("..")
   os.chdir("scenes2")
   testScenes2()
   os.chdir("..")

#
# Test flat multi-resolution morphing
#
if testMorphingFlag :
   os.chdir("flat_morphing")
   testFlatMorphing()
   os.chdir("..")

#
# Test spherical multi-resolution morphing
#
if testMorphingFlag :
   os.chdir("spherical_morphing")
   testSphericalMorphing()
   os.chdir("..")

#
# Test spherical registration
#
if testRegistrationFlag :
   os.chdir("spherical_registration")
   testSphericalRegistration()
   os.chdir("..")

#
# Test segmentation
#
if testSegmentationFlag :
   os.chdir("segmentation")
   testSegmentation()
   os.chdir("..")

print ""
print "There were %s errors.  **************************************\n" % problemCount
print "Unit Testing Completed."
print ""

if (problemCount > 0) :
   print "PROBLEMS: "
   print "%s" % problemMessage
   print ""

endTime = time.clock()
totalTime = endTime - startTime
print "Total time (in seconds): %f" % totalTime

os._exit(problemCount)


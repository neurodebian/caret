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

#include <cstdlib>
#include <iostream>
 
#include <qapplication.h>
#include <qdatetime.h>

#include "BrainModelSurfaceMetricClustering.h"
#include "BrainModelSurfaceMetricSmoothing.h"
#include "BrainSet.h"
#include "CaretVersion.h"
#include "DebugControl.h"
#include "FileUtilities.h"
#include "GaussianComputation.h"
#include "MetricFile.h"
#include "SpecFile.h"
#include "StringUtilities.h"

enum MODE { 
   MODE_NONE, 
   MODE_CLUSTER, 
   MODE_MATH_BINARY,
   MODE_MATH_UNARY,
   MODE_SMOOTH 
};
static MODE mode = MODE_NONE;

enum OUTPUT_FORMAT { OUTPUT_FORMAT_DEFAULT, OUTPUT_FORMAT_BINARY, OUTPUT_FORMAT_TEXT };
static OUTPUT_FORMAT outputFormat = OUTPUT_FORMAT_DEFAULT;

//
// Parameters for gaussian smoothing.
//
static float smoothGaussNormAbove = 0.0;
static float smoothGaussNormBelow = 0.0;
static float smoothGaussSigmaNorm = 0.0;
static float smoothGaussSigmaTang = 0.0;
static float smoothGaussTang      = 0.0;

/**
 * Print help information for clustering and smoothing.
 */
static void
printHelpClusterSmooth(const QString& programNameIn)
{
   const QString programName(FileUtilities::basename(programNameIn));
   
   std::cout
   << std::endl
   << "NAME" << std::endl
   << "   " << programName.toAscii().constData() 
            << "   v" << CaretVersion::getCaretVersionAsString().toAscii().constData() 
            << " (" << __DATE__ << ")" 
            << std::endl
   << "" << std::endl
   << "SYNOPSIS" << std::endl
   << "   " << programName.toAscii().constData() << " -cluster  -coord coord-name  -topo topo-name \\" << std::endl
   << "           -pos-min-thresh num  -pos-max-thresh num \\" << std::endl
   << "           -neg-min-thresh num  -neg-max-thresh num \\" << std::endl
   << "           [ -text  |  -binary ] \\" << std::endl
   << "           [ -output-file output-file-name ] \\" << std::endl
   << "           [ -anysize | -min-node num | -min-area area ] metric-file(s)" << std::endl
   << "" << std::endl
   << "   " << programName.toAscii().constData() << " -smooth  -coord coord-name  -topo topo-name \\" << std::endl
   << "           -alg algorithm  -iter num  -strength num  metric-file(s)" << std::endl
   << "" << std::endl
   << "DESCRIPTION" << std::endl
   << "   Cluster and smooth metric data." << std::endl
   << "" << std::endl
   << "   Note: The input files are overritten with clustering or smoothing applied." << std::endl
   << "   You may want to backup your files prior to running this program." << std::endl
   << "" << std::endl
   << "   REQUIRED PARAMETERS" << std::endl
   << "      " << std::endl
   << "      -coord coord-name  Specifies the coordinate file to be used." << std::endl
   << "      " << std::endl
   << "      -topo  topo-name   Specifies the topology file to be used." << std::endl
   << "      " << std::endl
   << "      Note: You must specify one of the following:" << std::endl
   << "" << std::endl
   << "         -cluster  " << std::endl
   << "            Perform clustering on the specified metric files." << std::endl
   << "" << std::endl
   << "         -smooth  " << std::endl
   << "            Perform smoothing on the specified metric files." << std::endl
   << "" << std::endl
   << "   REQUIRED PARAMETERS FOR CLUSTERING" << std::endl
   << "      " << std::endl
   << "      -neg-min-thresh num  values greater than \"num\" are excluded from clusters." << std::endl
   << "      " << std::endl
   << "      -neg-max-thresh num  values less than \"num\" are excluded from " << std::endl
   << "          clusters." << std::endl
   << "         Note that \"-neg-max-thresh\" is most negative" << std::endl
   << "      " << std::endl
   << "      -pos-min-thresh num  values less than \"num\" are excluded from clusters." << std::endl
   << "      " << std::endl
   << "      -pos-max-thresh num  values greater than \"num\" are excluded from " << std::endl
   << "          clusters." << std::endl
   << "      " << std::endl
   << "      One of the following must be specified" << std::endl
   << "         " << std::endl
   << "         -anysize  Specifies that clusters may be of any size." << std::endl
   << "         " << std::endl
   << "         -min-node num  Specifies that clusters must contain at least \"num\" " << std::endl
   << "            nodes." << std::endl
   << "         " << std::endl
   << "         -min-area area  Specifies that clusters must be of \"area\" surface area." << std::endl
   << "         " << std::endl
   << "   REQUIRED PARAMETERS FOR SMOOTHING" << std::endl
   << "      " << std::endl
   << "      -iter num  Specifies the number of smoothing iterations." << std::endl
   << "      " << std::endl
   << "      -strength num  Specifies the strength (ranges 0.0 to 1.0)." << std::endl
   << "      " << std::endl
   << "      -alg algorithm  Specifies the \"algorithm\" which is one of the following:" << std::endl
   << "           AN     Average Neighbors" << std::endl
   << "           GAUSS  Gaussian" << std::endl
   << "           WAN    Weighted Average Neighbors" << std::endl
   << "      " << std::endl
   << "   OPTIONAL PARAMETERS" << std::endl
   << "" << std::endl
   << "      -sphere-coord " << std::endl
   << "         Name of spherical coord file for gaussian smoothing."
   << "" << std::endl
   << "      -binary " << std::endl
   << "         Output metric file(s) will be in binary format." << std::endl
   << "" << std::endl
   << "      -text " << std::endl
   << "         Output metric file(s) will bin in text format." << std::endl
   << "" << std::endl
   << "      -gauss NB NA SN ST TC" << std::endl
   << "         Specifies parameters for the gaussian smoothing algorithm." << std::endl
   << "            NB - Norm Below Cutoff (curentValue = " << smoothGaussNormBelow<< ")" << std::endl
   << "            NA - Norm Above Cutoff (curentValue = " << smoothGaussNormAbove << ")" << std::endl
   << "            SN - Sigma Norm (curentValue = " << smoothGaussSigmaNorm << ")" << std::endl
   << "            ST - Sigma Tang (curentValue = " << smoothGaussSigmaTang << ")" << std::endl
   << "            TC - Tang Cutoff (curentValue = " << smoothGaussTang << ")" << std::endl
   << "" << std::endl
   << "      -output-file output-file-name"  << std::endl
   << "         Specifies the name of the output file.  If this parameter is specified, " << std::endl
   << "         one input file may be specified.  Without the parameter the output " << std::endl
   << "         files replace the input files. " << std::endl
   << "  For best results, run this program from the directory containing the files." << std::endl
   << "" << std::endl
   << "   EXAMPLES" << std::endl
   << "" << std::endl
   << "      Perform clustering on all metric files in the current directory.  Use" << std::endl
   << "      \"fiducial.coord\" for the coordinates, \"closed.topo\" for the topology," << std::endl
   << "      and find clusters whose surface area is at least 20 mm and whose values" << std::endl
   << "      are within the range 10.0 to 100.0 or within the range -50.0 to -20.0." << std::endl
   << "         " << programName.toAscii().constData() << "  -cluster -coord fiducial.coord -topo closed.topo \\" << std::endl
   << "             -pos-min-thresh 10.0  -pos-max-thresh 100.0  \\" << std::endl
   << "             -neg-min-thresh -20.0  -neg-max-thresh -50.0  \\" << std::endl
   << "             -min-area 20.0 *metric " << std::endl
   << "" << std::endl
   << "      Perform smoothing on all metric files in the current directory.  Use" << std::endl
   << "      \"fiducial.coord\" for the coordinates, \"closed.topo\" for the topology," << std::endl
   << "      and smooth with the Average Neighbors algorithm for 50 iterations with a" << std::endl
   << "      stength of 1.0." << std::endl
   << "         " << programName.toAscii().constData() << "  -smooth -coord fiducial.coord -topo closed.topo \\" << std::endl
   << "             -alg AN  -iter 50  -strength 1.0  *metric " << std::endl
   << "" << std::endl
   << "" << std::endl;
}

/**
 * Print help information for unary math.
 */
static void
printHelpMathUnary(const QString& programNameIn)
{
   const QString programName(FileUtilities::basename(programNameIn));
   
   std::cout
   << std::endl
   << "NAME" << std::endl
   << "   " << programName.toAscii().constData() 
            << "   v" << CaretVersion::getCaretVersionAsString().toAscii().constData() 
            << " (" << __DATE__ << ")" 
            << std::endl
   << "" << std::endl
   << "SYNOPSIS" << std::endl
   << "   " << programName.toAscii().constData() << " [options] -math1 OPERATION SCALARS   \\" << std::endl
   << "           INPUT-COL-NUM  OUTPUT-COL-NUM  INPUT-FILE  OUTPUT-FILE" << std::endl
   << "" << std::endl
   << "DESCRIPTION" << std::endl
   << "   Perform unary operations on a metric file column." << std::endl
   << "" << std::endl
   << "   REQUIRED PARAMETERS" << std::endl
   << "      " << std::endl
   << "      OPERATION - the operation that is to be performed." << std::endl
   << "         ABS" << std::endl
   << "            Each element in the column replaced with its absolute value." << std::endl
   << "            No scalars are need." << std::endl
   << "         " << std::endl
   << "         ADD  <scalar>" << std::endl
   << "            Each element in the column has the scalar added." << std::endl
   << "         " << std::endl
   << "         FIX-NAN" << std::endl
   << "            Each element in the column that is \"not a number\" is" << std::endl
   << "            replaced with zero.  No scalars are needed." << std::endl
   << "         " << std::endl
   << "         MAX  <scalar>" << std::endl
   << "            Each element in the column that exceeds the scalar is." << std::endl
   << "            replaced by the scalar." << std::endl
   << "         " << std::endl
   << "         MIN  <scalar>" << std::endl
   << "            Each element in the column that is less than the scalar " << std::endl
   << "            is replaced by the scalar." << std::endl
   << "         " << std::endl
   << "         MULTIPLY  <scalar>" << std::endl
   << "            Each element in the column is multiplied by the scalar." << std::endl
   << "         " << std::endl
   << "         NORMALIZE  <scalar-mean> <scalar-deviation>" << std::endl
   << "            The elements in the column are mapped to fit a normal distribution." << std::endl
   << "            The two scalars specify the mean and standard deviation of the " << std::endl
   << "            normal distribution." << std::endl
   << "         " << std::endl
   << "         RANGE-INC  <scalar-min-value> <scalar-max-value>" << std::endl
   << "            Each elemnent in the column that is within the range (inclusively)" << std::endl
   << "            of the two scalars is set to zero." << std::endl
   << "         " << std::endl
   << "         RANGE-EX  <scalar-min-value> <scalar-max-value>" << std::endl
   << "            Each elemnent in the column that is within the range (exclusively)" << std::endl
   << "            of the two scalars is set to zero." << std::endl
   << "         " << std::endl
   << "      SCALARS - scalars (numbers) used by the operation." << std::endl
   << "         The number of the scalars (numbers) specified depends upon the " << std::endl
   << "         operation.  See the OPERATION description." << std::endl
   << "      " << std::endl
   << "      INPUT-COL-NUM" << std::endl
   << "         The number of the input column.  The first column is 1 (one)." << std::endl
   << "      " << std::endl
   << "      OUTPUT-COL-NUM" << std::endl
   << "         The number of the output column.  The first column is 1 (one)." << std::endl
   << "         Use -1 (minus one) to create a new column.  The name of the" << std::endl
   << "         output column may be specified with the \"-out-col-name\"" << std::endl
   << "         option described below." << std::endl
   << "      " << std::endl
   << "      INPUT-FILE" << std::endl
   << "         The name of the input file." << std::endl
   << "      " << std::endl
   << "      OUTPUT-FILE" << std::endl
   << "         The name of the output file.  This may be the same name as" << std::endl
   << "         the input file.  Note that all columns from the input file" << std::endl
   << "         are written to the output file, even those on which no " << std::endl
   << "         operation was performed." << std::endl
   << "" << std::endl
   << "   OPTIONS" << std::endl
   << "      -out-col-name NAME" << std::endl
   << "         Specifies the name of the output column." << std::endl
   << "" << std::endl
   << "      -binary " << std::endl
   << "         Output file will be in binary format." << std::endl
   << "" << std::endl
   << "      -text " << std::endl
   << "         Output file will bin in text format." << std::endl
   << "" << std::endl
   << "  For best results, run this program from the directory containing the files." << std::endl
   << "" << std::endl
   << "   EXAMPLES" << std::endl
   << "" << std::endl
   << "      Add 5.7 to column 2 of the file." << std::endl
   << "         " << programName.toAscii().constData() << "  -math1 ADD 5.7 2 2 file.metric file.metric  " << std::endl
   << "" << std::endl
   << "      Fit the values in column 4 to a normal distribution with a mean of 1.0 and" << std::endl
   << "      a standard deviation of 2.0.  Create a new output file named" << std::endl
   << "      \"new-file.metric\" and a new column named \"norm data\".  In addition," << std::endl
   << "      the new metric file is written in binary format." << std::endl
   << "         " << programName.toAscii().constData() << " -out-col-name \"norm data\" -binary -math1 \\" << std::endl
   << "             NORMALIZE 1.0 2.0 4 -1 file.metric new-file.metric" << std::endl
   << "" << std::endl
   << "" << std::endl;
}

/**
 * Print help information for binary math.
 */
static void
printHelpMathBinary(const QString& programNameIn)
{
   const QString programName(FileUtilities::basename(programNameIn));
   
   std::cout
   << std::endl
   << "NAME" << std::endl
   << "   " << programName.toAscii().constData() 
            << "   v" << CaretVersion::getCaretVersionAsString().toAscii().constData() 
            << " (" << __DATE__ << ")" 
            << std::endl
   << "" << std::endl
   << "SYNOPSIS" << std::endl
   << "   " << programName.toAscii().constData() << " [options] -math2  OPERATION  INPUT-COL-NUM-A  INPUT-COL-B \\" << std::endl
   << "           OUTPUT-COL-NUM  INPUT-FILE  OUTPUT-FILE" << std::endl
   << "" << std::endl
   << "DESCRIPTION" << std::endl
   << "   Perform binary operations on a metric file column." << std::endl
   << "" << std::endl
   << "   REQUIRED PARAMETERS" << std::endl
   << "      " << std::endl
   << "      OPERATION - the operation that is to be performed." << std::endl
   << "         ADD" << std::endl
   << "            The output column is the sum of the two input columns." << std::endl
   << "         " << std::endl
   << "         DIVIDE" << std::endl
   << "            The output column is the result of INPUT-COL-NUM-A" << std::endl
   << "            divided by INPUT-COL-NUM-B." << std::endl
   << "         " << std::endl
   << "         MULTIPLY" << std::endl
   << "            The output column is the result of INPUT-COL-NUM-A" << std::endl
   << "            multiplied by INPUT-COL-NUM-B." << std::endl
   << "         " << std::endl
   << "         SUBTRACT" << std::endl
   << "            The output column is the result of INPUT-COL-NUM-B" << std::endl
   << "            subtracted from INPUT-COL-NUM-A." << std::endl
   << "         " << std::endl
   << "         AVERAGE" << std::endl
   << "            The output column is the avarege of INPUT-COL-NUM-A" << std::endl
   << "            and INPUT-COL-NUM-B." << std::endl
   << "      " << std::endl
   << "      INPUT-COL-NUM-A" << std::endl
   << "         The number of the first input column.  The first column is 1 (one)." << std::endl
   << "      " << std::endl
   << "      INPUT-COL-NUM-B" << std::endl
   << "         The number of the second input column.  The first column is 1 (one)." << std::endl
   << "      " << std::endl
   << "      OUTPUT-COL-NUM" << std::endl
   << "         The number of the output column.  The first column is 1 (one)." << std::endl
   << "         Use -1 (minus one) to create a new column.  The name of the" << std::endl
   << "         output column may be specified with the \"-out-col-name\"" << std::endl
   << "         option described below." << std::endl
   << "      " << std::endl
   << "      INPUT-FILE" << std::endl
   << "         The name of the input file." << std::endl
   << "      " << std::endl
   << "      OUTPUT-FILE" << std::endl
   << "         The name of the output file.  This may be the same name as" << std::endl
   << "         the input file.  Note that all columns from the input file" << std::endl
   << "         are written to the output file, even those on which no " << std::endl
   << "         operation was performed." << std::endl
   << "" << std::endl
   << "   OPTIONS" << std::endl
   << "      -out-col-name NAME" << std::endl
   << "         Specifies the name of the output column." << std::endl
   << "" << std::endl
   << "      -binary " << std::endl
   << "         Output file will be in binary format." << std::endl
   << "" << std::endl
   << "      -text " << std::endl
   << "         Output file will bin in text format." << std::endl
   << "" << std::endl
   << "  For best results, run this program from the directory containing the files." << std::endl
   << "" << std::endl
   << "   EXAMPLES" << std::endl
   << "" << std::endl
   << "      Add columns 1 and 2 and store the output in column 3." << std::endl
   << "         " << programName.toAscii().constData() << "  -math2 ADD 1 2 3 file.metric file.metric  " << std::endl
   << "" << std::endl
   << "      Store the average of columns 2 and 3 in a new column.  Create a new" << std::endl
   << "      output file named \"new-file.metric\" and a new column named" << std::endl
   << "      \"averages\".  In addition, the new metric file is written in" << std::endl
   << "      binary format." << std::endl
   << "         " << programName.toAscii().constData() << " -out-col-name \"averages\" -binary -math2 \\" << std::endl
   << "             AVERAGE 2 3 -1 file.metric new-file.metric" << std::endl
   << "" << std::endl
   << "" << std::endl;
}

/**
 * Get the next parameter as a string.
 */
void
getNextParameter(const QString& optionName, const std::vector<QString>& args, int& index, 
                 QString& s)
{                
   s = "";
   
   index++;
   if (index < static_cast<int>(args.size())) {
      s = args[index];
      return;
   }
   
   std::cout << "Missing parameter for \"" << optionName.toAscii().constData() << "\" option." << std::endl;
   exit(-1);
}

/**
 * Get the next parameter as an int.
 */
void
getNextParameter(const QString& optionName, const std::vector<QString>& args, int& index,
                 int& i){
   QString s;
   getNextParameter(optionName, args, index, s);
   i = StringUtilities::toInt(s);
}

/**
 * Get the next parameter as a float.
 */
void
getNextParameter(const QString& optionName, const std::vector<QString>& args, int& index,
                 float& f)
{
   QString s;
   getNextParameter(optionName, args, index, s);
   f = StringUtilities::toFloat(s);
}

static QString coordFileName;
static QString sphericalCoordFileName;
static QString topoFileName;

static float clusterNegMinThresh = 0.0;
static float clusterNegMaxThresh = 0.0;
static float clusterPosMinThresh = 0.0;
static float clusterPosMaxThresh = 0.0;
static int   clusterMinNodes = 0;
static float clusterMinArea  = 0.0;

static BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM clusterAlgorithm =
                BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_NONE;
static BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM smoothAlgorithm =
                              BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_NONE;
static int smoothIter = 0;
static float smoothStrength = 0.0;

/*
 * Do clustering and smoothing.
 */
static void 
clusterAndSmooth(const std::vector<QString>& args)
{
   QString outputFileName;
   
   int firstFileIndex = -1;
   const int argc = static_cast<int>(args.size());
   for (int i = 0; i < argc; i++) {
      const QString arg(args[i]);
      
      if (arg == "-binary") {
         // ignore
      }
      else if (arg == "-cluster") {
         // ignore
      }
      else if (arg == "-coord") {
         getNextParameter(args[i], args, i, coordFileName);
      }
      else if (arg == "-sphere-coord") {
         getNextParameter(args[i], args, i, sphericalCoordFileName);
      }
      else if (arg == "-topo") {
         getNextParameter(args[i], args, i, topoFileName);
      }
      else if (arg == "-neg-min-thresh") {
         getNextParameter(args[i], args, i, clusterNegMinThresh);
      }
      else if (arg == "-neg-max-thresh") {
         getNextParameter(args[i], args, i, clusterNegMaxThresh);
      }
      else if (arg == "-pos-min-thresh") {
         getNextParameter(args[i], args, i, clusterPosMinThresh);
      }
      else if (arg == "-pos-max-thresh") {
         getNextParameter(args[i], args, i, clusterPosMaxThresh);
      }
      else if (arg == "-anysize") {
         clusterAlgorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_ANY_SIZE;
      }
      else if (arg == "-min-node") {
         clusterAlgorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_NUMBER_OF_NODES;
         getNextParameter(args[i], args, i, clusterMinNodes);
      }
      else if (arg == "-min-area") {
         clusterAlgorithm = BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_MINIMUM_SURFACE_AREA;
         getNextParameter(args[i], args, i, clusterMinArea);
      }
      else if (arg == "-alg") {
         QString algName;
         getNextParameter(args[i], args, i, algName);
         if (algName == "AN") {
            smoothAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_AVERAGE_NEIGHBORS;
         }
         else if (algName == "WAN") {
            smoothAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_WEIGHTED_AVERAGE_NEIGHBORS;
         }
         else if (algName == "GAUSS") {
            smoothAlgorithm = BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_GAUSSIAN;
         }
         else {
            std::cout << "ERROR: Unknown smoothing algorithm: " << algName.toAscii().constData() << std::endl;
            exit(-1);
         }
      }
      else if (arg == "-gauss") {
         const int ii = i;
         getNextParameter(args[ii], args, i, smoothGaussNormBelow);
         getNextParameter(args[ii], args, i, smoothGaussNormAbove);
         getNextParameter(args[ii], args, i, smoothGaussSigmaNorm);
         getNextParameter(args[ii], args, i, smoothGaussSigmaTang);
         getNextParameter(args[ii], args, i, smoothGaussTang);
      }
      else if (arg == "-iter") {
         getNextParameter(args[i], args, i, smoothIter);
      }
      else if (arg == "-strength") {
         getNextParameter(args[i], args, i, smoothStrength);
      }
      else if (arg == "-output-file") {
         getNextParameter(args[i], args, i, outputFileName);
      }
      else if (arg == "-smooth") {
         // ignore
      }
      else if (arg == "-text") {
         // ignore
      }
      else if (arg[0] != '-') {
         firstFileIndex = i;
         break;
      }
      else {
         std::cout << "ERROR: Unrecognized parameter: " << arg.toAscii().constData() << std::endl;
         std::cout << std::endl;
         exit(-1);
      }
   }
   
   std::cout << std::endl;
   std::cout << "INFO: To see command line options, Run with \"-help\"" << std::endl;
   std::cout << std::endl;
   
   switch (mode) {
      case MODE_CLUSTER:
         if (clusterAlgorithm == BrainModelSurfaceMetricClustering::CLUSTER_ALGORITHM_NONE) {
            std::cout << "You must specify a clustering mode with \"\"" << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         break;
      case MODE_SMOOTH:
         if (smoothAlgorithm == BrainModelSurfaceMetricSmoothing::SMOOTH_ALGORITHM_NONE) {
            std::cout << "You must specify a smoothing mode with \"\"" << std::endl;
            std::cout << std::endl;
            exit(-1);
         }
         break;
      case MODE_MATH_BINARY:
      case MODE_MATH_UNARY:
      case MODE_NONE:
         std::cout << "ERROR: You must specify a mode parameter." << std::endl;
         std::cout << std::endl;
         exit(-1);
         break;
   }
   
   if (firstFileIndex < 0) {
      std::cout << "ERROR: No files specified." << std::endl;
      std::cout << std::endl;
      exit(-1);
   }
   
   //
   // Check for coord file
   //
   if (coordFileName.isEmpty()) {
      std::cout << "ERROR: No coord file specified." << std::endl;
      exit(-1);
   }
   if (QFile::exists(coordFileName) == false) {
      std::cout << "ERROR: coord file does not exist." << std::endl;
      exit(-1);
   }
   
   //
   // Check for topo file
   //
   if (topoFileName.isEmpty()) {
      std::cout << "ERROR: No topo file specified." << std::endl;
      exit(-1);
   }
   if (QFile::exists(topoFileName) == false) {
      std::cout << "ERROR: topo file does not exist." << std::endl;
      exit(-1);
   }
   
   //
   // Is an output file specified
   //
   if (outputFileName.isEmpty() == false) {
      const int numFiles = argc - firstFileIndex;
      if (numFiles > 1) {
         std::cout << "ERROR: when an output file name is specified, there may be only " << std::endl
                   << "one input file." << std::endl;
         exit(-1);
      }
   }
   
   //
   // Create a spec file
   //
   SpecFile sf;
   sf.addToSpecFile(SpecFile::fiducialCoordFileTag, coordFileName, "", false);
   sf.addToSpecFile(SpecFile::closedTopoFileTag, topoFileName, "", false);
   if (sphericalCoordFileName.isEmpty() == false) {
      sf.addToSpecFile(SpecFile::sphericalCoordFileTag, sphericalCoordFileName, "", false);
   }
   sf.setAllFileSelections(SpecFile::SPEC_TRUE);
   
   //
   // Create a brain set
   //
   QString errorMessages;
   BrainSet brainSet(true);
   brainSet.readSpecFile(sf, "temp.spec", errorMessages);
   if (errorMessages.isEmpty() == false) {
      std::cout << "ERROR: creating brain set.  Are coord and topo valid files?" << std::endl;
      std::cout << "   " << errorMessages.toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   //  Get the Coordinate and Topology Files
   //
   if (brainSet.getNumberOfBrainModels() <= 0) {
      std::cout << "ERROR: BrainSet contains no BrainModels." << std::endl;
      std::cout << std::endl;
   }
   BrainModelSurface* bms = brainSet.getBrainModelSurface(0);
   if (bms == NULL) {
      std::cout << "ERROR: BrainSet contains no BrainModelSurfaces." << std::endl;
      std::cout << std::endl;
   }
   const CoordinateFile* coordFile = bms->getCoordinateFile();
   const TopologyFile* topoFile = bms->getTopologyFile();
   if (topoFile == NULL) {
      std::cout << "ERROR: BrainModelSurface has no topology." << std::endl;
      std::cout << std::endl;
   }
   
   BrainModelSurface* sphericalSurface = NULL;
   if (sphericalCoordFileName.isEmpty() == false) {
      if (brainSet.getNumberOfBrainModels() > 1) {
         sphericalSurface = brainSet.getBrainModelSurface(1);
      }
   }
   
   //
   // Create a coordinate file containing normals needed by smoothing
   //
   CoordinateFile* normalsFile = NULL;
   switch (mode) {
      case MODE_CLUSTER:
      case MODE_SMOOTH:
         normalsFile = new CoordinateFile;
         break;
      case MODE_MATH_BINARY:
      case MODE_MATH_UNARY:
      case MODE_NONE:
         break;
   }
   const int numNormals = coordFile->getNumberOfCoordinates();
   normalsFile->setNumberOfCoordinates(numNormals);
   for (int j = 0; j < numNormals; j++) {
      normalsFile->setCoordinate(j, bms->getNormal(j));
   }
   
   //
   // Process the files
   //
   for (int i = firstFileIndex; i < argc; i++) {
      const QString filename(args[i]);
      QString errorMessage;
      
      //
      // Read the metric file
      //
      bool validMetricFile = false;
      MetricFile mf;
      try {
         mf.readFile(filename);
         validMetricFile = true;
      }
      catch (FileException& e) {
         std::cout << "ERROR reading " << filename.toAscii().constData()  
                   << " " << e.whatQString().toAscii().constData() << std::endl;
      }
      
      if (validMetricFile) {
         const int numNodes = mf.getNumberOfNodes();
         const int numColumns = mf.getNumberOfColumns();
         if (numNodes != bms->getNumberOfNodes()) {
            std::cout << "ERROR: " << filename.toAscii().constData() << " has a different number of nodes "
                      << " than " << coordFileName.toAscii().constData() << std::endl;
         }
         else {
            for (int j = 0; j < numColumns; j++) {
               switch (mode) {
                  case MODE_CLUSTER:
                     {
                        BrainModelSurfaceMetricClustering bmsmc(&brainSet,
                                                                bms,
                                                                &mf,
                                                                clusterAlgorithm,
                                                                j,
                                                                j,
                                                                mf.getColumnName(j),
                                                                clusterMinNodes,
                                                                clusterMinArea,
                                                                clusterNegMinThresh,
                                                                clusterNegMaxThresh,
                                                                clusterPosMinThresh,
                                                                clusterPosMaxThresh);
                        try {
                           bmsmc.execute();
                        }
                        catch (BrainModelAlgorithmException& e) {
                           std::cout << "ERROR: clustering file " << filename.toAscii().constData()
                                     << " column " << j << " " << e.whatQString().constData() << std::endl;
                        }
                     }
                     break;
                  case MODE_SMOOTH:
                     {
                        BrainModelSurfaceMetricSmoothing bmsms(
                                  &brainSet,
                                  bms,
                                  sphericalSurface,
                                  &mf,
                                  smoothAlgorithm,
                                  j,
                                  j,
                                  mf.getColumnName(j),
                                  smoothStrength,
                                  smoothIter,
                                  smoothGaussNormBelow,
                                  smoothGaussNormAbove,
                                  smoothGaussSigmaNorm,
                                  smoothGaussSigmaTang,
                                  smoothGaussTang);
                        try {
                           bmsms.execute();
                        }
                        catch (BrainModelAlgorithmException& e) {
                           std::cout << "ERROR: smoothing file " << filename.toAscii().constData()
                                     << " column " << j << " " << e.whatQString().toAscii().constData() << std::endl;
                        }
                     }
                     break;
                  case MODE_MATH_BINARY:
                  case MODE_MATH_UNARY:
                  case MODE_NONE:
                     break;
               }
            }
            
            //
            // Write the metric file
            //
            try {
               QString name(filename);
               if (outputFileName.isEmpty() == false) {
                  name = outputFileName;
               }
               switch (outputFormat) {
                  case OUTPUT_FORMAT_DEFAULT:
                     break;
                  case OUTPUT_FORMAT_BINARY:
                     mf.setFileWriteType(AbstractFile::FILE_FORMAT_BINARY);
                     break;
                  case OUTPUT_FORMAT_TEXT:
                     mf.setFileWriteType(AbstractFile::FILE_FORMAT_ASCII);
                     break;
               }
               mf.writeFile(name);
            }
            catch (FileException& e) {
               std::cout << "ERROR writing " << filename.toAscii().constData()  
                         << " " << e.whatQString().toAscii().constData() << std::endl;
            }
         }
      }
   }
   
   if (normalsFile != NULL) {
      delete normalsFile;
   }
}

/*
 * Binary math operations.
 */
static void
mathBinary(const std::vector<QString>& args)
{
   int inputColumnA = -1;
   int inputColumnB = -1;
   int outputColumn = -1;
   QString inputFileName;
   QString outputFileName;
   QString outputColumnName;
   
   enum OPERATION {
      OP_NONE,
      OP_ADD,
      OP_AVERAGE,
      OP_DIVIDE,
      OP_MULTIPLY,
      OP_SUBTRACT
   };
   OPERATION operation = OP_NONE;
   
   //
   // Check the parameters
   //
   const int numArgs = static_cast<int>(args.size());
   for (int i = 0; i < numArgs; i++) {
      const QString arg(args[i]);
      
      if (arg == "-math2") {
         const int ii = i;
         
         QString opString;
         getNextParameter(args[ii], args, i, opString);
         
         if (opString == "ADD") {
            operation = OP_ADD;
         }
         else if (opString == "AVERAGE") {
            operation = OP_AVERAGE;
         }
         else if (opString == "DIVIDE") {
            operation = OP_DIVIDE;
         }
         else if (opString == "MULTIPLY") {
            operation = OP_MULTIPLY;
         }
         else if (opString == "SUBTRACT") {
            operation = OP_SUBTRACT;
         }
         else {
            std::cout << "ERROR: Invalid Operation: " << opString.toAscii().constData() << std::endl;
         }
         getNextParameter(args[ii], args, i, inputColumnA);
         getNextParameter(args[ii], args, i, inputColumnB);
         getNextParameter(args[ii], args, i, outputColumn);
         getNextParameter(args[ii], args, i, inputFileName);
         getNextParameter(args[ii], args, i, outputFileName);
      }
      else if (arg == "-out-col-name") {
         getNextParameter(args[i], args, i, outputColumnName);
      }
   }
   
   if (operation == OP_NONE) {
      std::cout << "ERROR: No operation specified." << std::endl;
      exit(-1);
   }
   
   //
   // Read in the input file
   //
   MetricFile metricFile;
   try {
      metricFile.readFile(inputFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: Unable to read " << inputFileName.toAscii().constData() << " : " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Check input column
   //
   if ((inputColumnA <= 0) || (inputColumnA > metricFile.getNumberOfColumns())) {
      std::cout << "ERROR: input metric column A number is invalid." << std::endl;
      exit(-1);
   }
   if ((inputColumnB <= 0) || (inputColumnB > metricFile.getNumberOfColumns())) {
      std::cout << "ERROR: input metric column B number is invalid." << std::endl;
      exit(-1);
   }
   //
   // columns actually index starting at zero
   //
   inputColumnA--;
   inputColumnB--;
   
   //
   // Check output column
   //
   if (outputColumn > -1) {
      if ((outputColumn <= 0) || (outputColumn > metricFile.getNumberOfColumns())) {
         std::cout << "ERROR: output metric column number is invalid." << std::endl;
         exit(-1);
      }
      
      //
      // columns actually index starting at zero
      //
      outputColumn--;
   }   
   
   //
   // Set output column name if necessary
   //
   if (outputColumnName.isEmpty()) {
      QString s(metricFile.getColumnName(inputColumnA));
      switch (operation) {
         case OP_NONE:
            break;
         case OP_ADD:
            s.append(" added to ");
            break;
         case OP_AVERAGE:
            s.append(" averaged with ");
            break;
         case OP_DIVIDE:
            s.append(" divided by ");
            break;
         case OP_MULTIPLY:
            s.append(" multiplied by  ");
            break;
         case OP_SUBTRACT:
            s.append(" subtract ");
            break;
      }
      s.append(metricFile.getColumnName(inputColumnB));
      outputColumnName = s;
   }
   
   //
   // Perform the operation
   //
   switch (operation) {
      case OP_NONE:
         break;
      case OP_ADD:
         metricFile.performBinaryOperation(MetricFile::BINARY_OPERATION_ADD,
                                          inputColumnA,
                                          inputColumnB,
                                          outputColumn,
                                          outputColumnName);
         break;
      case OP_AVERAGE:
         metricFile.performBinaryOperation(MetricFile::BINARY_OPERATION_AVERAGE,
                                          inputColumnA,
                                          inputColumnB,
                                          outputColumn,
                                          outputColumnName);
         break;
      case OP_DIVIDE:
         metricFile.performBinaryOperation(MetricFile::BINARY_OPERATION_DIVIDE,
                                          inputColumnA,
                                          inputColumnB,
                                          outputColumn,
                                          outputColumnName);
         break;
      case OP_MULTIPLY:
         metricFile.performBinaryOperation(MetricFile::BINARY_OPERATION_MULTIPLY,
                                          inputColumnA,
                                          inputColumnB,
                                          outputColumn,
                                          outputColumnName);
         break;
      case OP_SUBTRACT:
         metricFile.performBinaryOperation(MetricFile::BINARY_OPERATION_SUBTRACT,
                                          inputColumnA,
                                          inputColumnB,
                                          outputColumn,
                                          outputColumnName);
         break;
   }
   
   //
   // Write the output file
   //
   try {
      metricFile.writeFile(outputFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: Unable to write " << outputFileName.toAscii().constData() << " : " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
}

/*
 * Unary math operations.
 */
static void
mathUnary(const std::vector<QString>& args)
{
   int inputColumn = -1;
   int outputColumn = -1;
   QString inputFileName;
   QString outputFileName;
   float scalar1 = 0.0;
   float scalar2 = 0.0;
   QString outputColumnName;
   
   enum OPERATION {
      OP_NONE,
      OP_ABS,
      OP_ADD,
      OP_FIX_NAN,
      OP_MAX,
      OP_MIN,
      OP_MULTIPLY,
      OP_NORMALIZE,
      OP_RANGE_INC,
      OP_RANGE_EX
   };
   OPERATION operation = OP_NONE;
   
   //
   // Check the parameters
   //
   const int numArgs = static_cast<int>(args.size());
   for (int i = 0; i < numArgs; i++) {
      const QString arg(args[i]);
      
      if (arg == "-math1") {
         const int ii = i;
         
         QString opString;
         getNextParameter(args[ii], args, i, opString);
         
         if (opString == "ABS") {
            operation = OP_ABS;
         }
         else if (opString == "ADD") {
            getNextParameter(args[ii], args, i, scalar1);
            operation = OP_ADD;
         }
         else if (opString == "FIX-NAN") {
            operation = OP_FIX_NAN;
         }
         else if (opString == "MAX") {
            getNextParameter(args[ii], args, i, scalar1);
            operation = OP_MAX;
         }
         else if (opString == "MIN") {
            getNextParameter(args[ii], args, i, scalar1);
            operation = OP_MIN;
         }
         else if (opString == "MULTIPLY") {
            getNextParameter(args[ii], args, i, scalar1);
            operation = OP_MULTIPLY;
         }
         else if (opString == "NORMALIZE") {
            getNextParameter(args[ii], args, i, scalar1);
            getNextParameter(args[ii], args, i, scalar2);
            operation = OP_NORMALIZE;
         }
         else if (opString == "RANGE-INC") {
            getNextParameter(args[ii], args, i, scalar1);
            getNextParameter(args[ii], args, i, scalar2);
            operation = OP_RANGE_INC;
         }
         else if (opString == "RANGE-EX") {
            getNextParameter(args[ii], args, i, scalar1);
            getNextParameter(args[ii], args, i, scalar2);
            operation = OP_RANGE_EX;
         }
         getNextParameter(args[ii], args, i, inputColumn);
         getNextParameter(args[ii], args, i, outputColumn);
         getNextParameter(args[ii], args, i, inputFileName);
         getNextParameter(args[ii], args, i, outputFileName);
      }
      else if (arg == "-out-col-name") {
         getNextParameter(args[i], args, i, outputColumnName);
      }
   }
   
   if (operation == OP_NONE) {
      std::cout << "ERROR: No operation specified." << std::endl;
      exit(-1);
   }
   
   //
   // Read in the input file
   //
   MetricFile metricFile;
   try {
      metricFile.readFile(inputFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: Unable to read " << inputFileName.toAscii().constData() << " : " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
   
   //
   // Check input column
   //
   if ((inputColumn <= 0) || (inputColumn > metricFile.getNumberOfColumns())) {
      std::cout << "ERROR: input metric column number is invalid." << std::endl;
      exit(-1);
   }
   //
   // columns actually index starting at zero
   //
   inputColumn--;
   
   //
   // Check output column
   //
   if (outputColumn > -1) {
      if ((outputColumn <= 0) || (outputColumn > metricFile.getNumberOfColumns())) {
         std::cout << "ERROR: output metric column number is invalid." << std::endl;
         exit(-1);
      }
      
      //
      // columns actually index starting at zero
      //
      outputColumn--;
   }   
   
   //
   // Set output column name if necessary
   //
   if (outputColumnName.isEmpty()) {
      outputColumnName = metricFile.getColumnName(inputColumn);
   }
   
   //
   // Perform the operation
   //
   switch (operation) {
      case OP_NONE:
         break;
      case OP_ABS:
         metricFile.performUnaryOperation(MetricFile::UNARY_OPERATION_ABS_VALUE,
                                          inputColumn,
                                          outputColumn,
                                          outputColumnName,
                                          scalar1);
         break;
      case OP_ADD:
         metricFile.performUnaryOperation(MetricFile::UNARY_OPERATION_ADD,
                                          inputColumn,
                                          outputColumn,
                                          outputColumnName,
                                          scalar1);
         break;
      case OP_FIX_NAN:
         metricFile.performUnaryOperation(MetricFile::UNARY_OPERATION_FIX_NOT_A_NUMBER,
                                          inputColumn,
                                          outputColumn,
                                          outputColumnName,
                                          scalar1);
         break;
      case OP_MAX:
         metricFile.performUnaryOperation(MetricFile::UNARY_OPERATION_CEILING,
                                          inputColumn,
                                          outputColumn,
                                          outputColumnName,
                                          scalar1);
         break;
      case OP_MIN:
         metricFile.performUnaryOperation(MetricFile::UNARY_OPERATION_FLOOR,
                                          inputColumn,
                                          outputColumn,
                                          outputColumnName,
                                          scalar1);
         break;
      case OP_MULTIPLY:
         metricFile.performUnaryOperation(MetricFile::UNARY_OPERATION_MULTIPLY,
                                          inputColumn,
                                          outputColumn,
                                          outputColumnName,
                                          scalar1);
         break;
      case OP_NORMALIZE:
         metricFile.remapColumnToNormalDistribution(inputColumn,
                                                     outputColumn,
                                                     outputColumnName,
                                                     scalar1,
                                                     scalar2);
         break;
      case OP_RANGE_INC:
         metricFile.setRangeOfValuesToZero(inputColumn,
                                            outputColumn,
                                            outputColumnName,
                                            scalar1,
                                            scalar2,
                                            true);
         break;
      case OP_RANGE_EX:
         metricFile.setRangeOfValuesToZero(inputColumn,
                                            outputColumn,
                                            outputColumnName,
                                            scalar1,
                                            scalar2,
                                            false);
         break;
   }
   
   //
   // Write the output file
   //
   try {
      metricFile.writeFile(outputFileName);
   }
   catch (FileException& e) {
      std::cout << "ERROR: Unable to write " << outputFileName.toAscii().constData() << " : " << e.whatQString().toAscii().constData() << std::endl;
      exit(-1);
   }
}

/*
 * Show help information.
 */
static void
printHelp(const QString& programName) 
{
   std::cout << std::endl;
   std::cout << programName.toAscii().constData() << "   v" << CaretVersion::getCaretVersionAsString().toAscii().constData() 
             << " (" << __DATE__ << ")" 
             << std::endl;
   std::cout << "   This program performs operations on metric files." << std::endl;
   std::cout << std::endl;
   std::cout << "   For help with clustering and smoothing run the command:" << std::endl;
   std::cout << "      " << programName.toAscii().constData() << " -help-cs" << std::endl;
   std::cout << std::endl;
   std::cout << "   For help with binary (two column) math run the command:" << std::endl;
   std::cout << "      " << programName.toAscii().constData() << " -help-bm" << std::endl;
   std::cout << std::endl;
   std::cout << "   For help with unary (one column) math run the command:" << std::endl;
   std::cout << "      " << programName.toAscii().constData() << " -help-um" << std::endl;
   std::cout << std::endl;
}

/*
 * The main function.
 */ 
int
main(int argc, char* argv[])
{
   const QString programName(FileUtilities::basename(argv[0]));
   
   //
   // Initialize gaussian smoothing parameters here so values printed in help
   //
   GaussianComputation::getDefaultParameters(smoothGaussNormBelow,
                                             smoothGaussNormAbove,
                                             smoothGaussSigmaNorm,
                                             smoothGaussSigmaTang,
                                             smoothGaussTang);
                                             
   if (argc == 1) {
      printHelp(argv[0]);
      exit(0);
   }
   
   QApplication app(argc, argv, false);

   //
   // static initializes needed on OSX
   //
#ifdef Q_OS_MAC
   BrainSet::initializeDataFileStaticMembers();
#endif

   //
   // Initialize debugging off
   //
   DebugControl::setDebugOn(false);

   //
   // Process any environment variables
   //
   DebugControl::setDebugOnWithEnvironmentVariable("CARET_DEBUG");

   //
   // Look for the mode, output format and help
   //
   std::vector<QString> args;
   for (int i = 1; i < argc; i++) {
      const QString arg(argv[i]);
      
      if (arg == "-cluster") {
         mode = MODE_CLUSTER;
      }
      else if (arg == "-math1") {
         mode = MODE_MATH_UNARY;
         args.push_back(arg);
      }
      else if (arg == "-math2") {
         mode = MODE_MATH_BINARY;
         args.push_back(arg);
      }
      else if (arg == "-smooth") {
         mode = MODE_SMOOTH;
      }
      else if (arg == "-binary") {
         outputFormat = OUTPUT_FORMAT_BINARY;
      }
      else if (arg == "-text") {
         outputFormat = OUTPUT_FORMAT_TEXT;
      }
      else if ((arg == "-h") || (arg == "-help")) {
         printHelp(programName);
         exit(0);
      }
      else if (arg == "-help-cs") {
         printHelpClusterSmooth(programName);
         exit(0);
      }
      else if (arg == "-help-bm") {
         printHelpMathBinary(programName);
         exit(0);
      }
      else if (arg == "-help-um") {
         printHelpMathUnary(programName);
         exit(0);
      }
      else {
         args.push_back(arg);
      }
   }
   
   switch (mode) {
      case MODE_CLUSTER:
         clusterAndSmooth(args);
         break;
      case MODE_MATH_UNARY:
         mathUnary(args);
         break;
      case MODE_MATH_BINARY:
         mathBinary(args);
         break;
      case MODE_SMOOTH:
         clusterAndSmooth(args);
         break;
      case MODE_NONE:
         std::cout << "ERROR: You must specify a mode parameter." << std::endl;
         std::cout << std::endl;
         exit(-1);
         break;
   }
   
   return 0;
}


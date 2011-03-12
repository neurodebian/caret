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

#include <iostream>
#include <QTime>

#include "BrainModelSurfaceMetricCorrelationMatrix.h"
#include "BrainSet.h"
#include "CommandMetricCorrelationMatrix.h"
#include "FileFilters.h"
#include "GiftiDataArrayFile.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricCorrelationMatrix::CommandMetricCorrelationMatrix()
   : CommandBase("-metric-correlation-matrix",
                 "METRIC CORRELATION MATRIX")
{
}

/**
 * destructor.
 */
CommandMetricCorrelationMatrix::~CommandMetricCorrelationMatrix()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricCorrelationMatrix::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Output Metric File Name", FileFilters::getMetricFileFilter());
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandMetricCorrelationMatrix::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-name>\n"
       + indent9 + "<output-metric-file-name>\n"
       + indent9 + "[-apply-fisher-z-transform]\n"
       + indent9 + "[-parallel]\n"
       + indent9 + "\n"
       + indent9 + "Compute a correlation matrix using the input metric file.\n"
       + indent9 + "Each row (node) in the metric file is correlated with all\n"
       + indent9 + "other rows in the metric file.  Each column in the metric\n"
       + indent9 + "file represents an \"observation\" at each node.  \n"
       + indent9 + "\n"
       + indent9 + "The number of rows and columns are the same in the output\n"
       + indent9 + "metric file and are equal to the number of rows from the \n"
       + indent9 + "input metric file.  The values in the output metric file \n"
       + indent9 + "form a symmetric matrix with each element containing \n"
       + indent9 + "a correlation coefficient for the data of two nodes where\n"
       + indent9 + "the node numbers are the row and column indices.\n"
       + indent9 + "\n"
       + indent9 + "An example input metric file is one in which each row\n"
       + indent9 + "represents nodes and each column represents timepoints. \n"
       + indent9 + "The output metric file contains correlation coefficients\n"
       + indent9 + "that measure the similarity of the timepoints from all\n"
       + indent9 + "nodes to all nodes.\n"
       + indent9 + "\n"
       + indent9 + "If \"-apply-fisher-z-transform\" options is specified a\n"
       + indent9 + "is applied to the correlation values.\n"
       + indent9 + "\n"
       + indent9 + "If the \"-parallel\" option is specified, the algorithm\n"
       + indent9 + "will run its operations with multiple threads to reduce\n"
       + indent9 + "execution time.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricCorrelationMatrix::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the input and output file names
   //
   const QString inputMetricFileName = 
      parameters->getNextParameterAsString("Input Metric File Name");
   const QString outputMetricFileName = 
      parameters->getNextParameterAsString("Output Metric File Name");
   bool applyFisherZTransformFlag = false;
   bool parallelFlag = false;
   bool giftiFlag = true;
   bool incrementalFlag = true;
   
   //
   // Process optional parameters
   //
   while (parameters->getParametersAvailable()) {
      //
      // parameter name
      //
      const QString paramName =
         parameters->getNextParameterAsString("Option");
      if (paramName == "-apply-fisher-z-transform") {
         applyFisherZTransformFlag = true;
      }
      else if (paramName == "-parallel") {
         parallelFlag = true;
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }      

   //
   // Read the metric file
   //
   MetricFile mf;
   if (incrementalFlag == false) {
      QTime readTimer;
      readTimer.start();
      mf.readFile(inputMetricFileName);
      std::cout << "Time to read file "
                << (readTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }
   
   QTime algTimer;
   algTimer.start();
   BrainModelSurfaceMetricCorrelationMatrix* alg = NULL;
   if (incrementalFlag) {
      alg = new BrainModelSurfaceMetricCorrelationMatrix(inputMetricFileName,
                                                         outputMetricFileName,
                                                         applyFisherZTransformFlag,
                                                         giftiFlag,
                                                         parallelFlag);
   }
   else {
      BrainSet brainSet;
      alg = new BrainModelSurfaceMetricCorrelationMatrix(&brainSet,
                                                   &mf,
                                                   applyFisherZTransformFlag,
                                                   giftiFlag,
                                                   parallelFlag);
   }
   alg->execute();
   std::cout << "Time to run algorithm "
             << (algTimer.elapsed() * 0.001)
             << " seconds."
             << std::endl;
             
   
   //
   // Write metric file
   //
   if (incrementalFlag == false) {
      QTime writeTimer;
      writeTimer.start();
      if (giftiFlag) {
         GiftiDataArrayFile* outputGiftiFile = alg->getOutputGiftiFile();
         outputGiftiFile->writeFile(outputMetricFileName);
         delete outputGiftiFile;
      }
      else {
         MetricFile* outputMetricFile = alg->getOutputMetricFile();
         outputMetricFile->writeFile(outputMetricFileName);
         delete outputMetricFile;
      }
      std::cout << "Time to write file "
                << (writeTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   }
}

      


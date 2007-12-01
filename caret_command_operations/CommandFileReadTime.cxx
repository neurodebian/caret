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

#include <QFileInfo>
#include <QTime>

#include "BrainSet.h"
#include "CommandFileReadTime.h"
#include "CoordinateFile.h"
#include "BorderProjectionFile.h"
#include "FileFilters.h"
#include "FociProjectionFile.h"
#include "GiftiDataArrayFile.h"
#include "MetricFile.h"
#include "PreferencesFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "SpecFile.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
CommandFileReadTime::CommandFileReadTime()
   : CommandBase("-file-read-time",
                 "FILE READ TIME")
{
   iterations = 3;
   numberOfThreads = 1;
}

/**
 * destructor.
 */
CommandFileReadTime::~CommandFileReadTime()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandFileReadTime::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   
   QStringList filters;
   filters << FileFilters::getCoordinateGenericFileFilter();
   filters << FileFilters::getBorderProjectionFileFilter();
   filters << FileFilters::getFociProjectionFileFilter();
   filters << FileFilters::getMetricFileFilter();
   filters << FileFilters::getSpecFileFilter();
   filters << FileFilters::getSurfaceShapeFileFilter();
   filters << FileFilters::getTopologyGenericFileFilter();
   
   paramsOut.addFile("File Name", filters);
   paramsOut.addVariableListOfParameters("File Timing");
}

/**
 * get full help information.
 */
QString 
CommandFileReadTime::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<filename>\n"
       + indent9 + "[-iter iterations]\n"
       + indent9 + "[-threads number-of-threads] \n"
       + indent9 + "\n"
       + indent9 + "Time the reading of a caret data file.  Not all caret files\n"
       + indent9 + "are supported for this command.\n"
       + indent9 + "\n"
       + indent9 + "The file is read \"iteration\" times and the average of \n"
       + indent9 + "those times is printed.  The default number of iterations\n"
       + indent9 + "is " + QString::number(iterations) + ". \n"
       + indent9 + "\n"
       + indent9 + "File types supported at this time are\n"
       + indent9 + "   Coordinate \n"
       + indent9 + "   Border Projection \n"
       + indent9 + "   Foci Projection \n"
       + indent9 + "   Metric \n"
       + indent9 + "   Spec File \n"
       + indent9 + "   Surface Shape File \n"
       + indent9 + "   Topology File \n"
       + indent9 + "\n"
       + indent9 + "The time for a Spec File is the time to read all of the\n"
       + indent9 + "spec file's data files.\n"
       + indent9 + "\n"
       + indent9 + "The number of threads parameter applied only to the \n"
       + indent9 + "reading of Spec Files.  The default number of threads \n"
       + indent9 + "is " + QString::number(numberOfThreads) + ". \n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * read the file for timing purposes.
 */
void 
CommandFileReadTime::readFileForTiming(AbstractFile* dataFile,
                                       const QString& fileName,
                                       float& timeToReadOut,
                                       float& fileSizeInMBOut)
{
   float timeTotal = 0.0;
   for (int i = 0; i < iterations; i++) {
      dataFile->readFile(fileName);
      timeTotal += dataFile->getTimeToReadFileInSeconds();
   }
   
   timeToReadOut = timeTotal / static_cast<float>(iterations);
   
   const float oneMegabyte = 1048576.0;
   QFileInfo fi(dataFile->getFileName());
   fileSizeInMBOut = fi.size() / oneMegabyte;
}

/**
 * execute the command.
 */
void 
CommandFileReadTime::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   const QString fileName = parameters->getNextParameterAsString("File Name");
   while (parameters->getParametersAvailable()) {
      const QString paramName = parameters->getNextParameterAsString("Param Name");
      if (paramName == "-iter") {
         iterations = parameters->getNextParameterAsInt("Iterations");
      }
      else if (paramName == "-threads") {
         numberOfThreads = parameters->getNextParameterAsInt("Number of Threads");
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }
   
   //
   // Create a file based upon the file name extension
   //
   float timeInSeconds = 0.0;
   float fileSizeInMB = 0.0;
   if (fileName.endsWith(SpecFile::getCoordinateFileExtension())) {
      CoordinateFile cf;
      readFileForTiming(&cf, fileName, timeInSeconds, fileSizeInMB);
   }
   else if (fileName.endsWith(SpecFile::getBorderProjectionFileExtension())) {
      BorderProjectionFile bpf;
      readFileForTiming(&bpf, fileName, timeInSeconds, fileSizeInMB);
   }
   else if (fileName.endsWith(SpecFile::getFociProjectionFileExtension())) {
      FociProjectionFile fpf;
      readFileForTiming(&fpf, fileName, timeInSeconds, fileSizeInMB);
   }
   else if (fileName.endsWith(SpecFile::getGiftiFileExtension())) {
      GiftiDataArrayFile gifti;
      readFileForTiming(&gifti, fileName, timeInSeconds, fileSizeInMB);
   }
   else if (fileName.endsWith(SpecFile::getMetricFileExtension())) {
      MetricFile mf;
      readFileForTiming(&mf, fileName, timeInSeconds, fileSizeInMB);
   }
   else if (fileName.endsWith(SpecFile::getSpecFileExtension())) {
      for (int i = 0; i < iterations; i++) {
         SpecFile sf;
         sf.readFile(fileName);
         sf.setAllFileSelections(SpecFile::SPEC_TRUE);
         BrainSet bs;
         PreferencesFile* pf = bs.getPreferencesFile();
         pf->setNumberOfFileReadingThreads(numberOfThreads);
         QString errorMessage;
         QTime timer;
         timer.start();
         bs.readSpecFile(sf,
                         fileName,
                         errorMessage);
         if (errorMessage.isEmpty() == false) {
            throw CommandException("Spec File Read error, timing test invalid\n"
                                   + errorMessage);
         }
         timeInSeconds += (static_cast<float>(timer.elapsed()) / 1000.0);
      }
      
      timeInSeconds /= static_cast<float>(iterations);
   }
   else if (fileName.endsWith(SpecFile::getSurfaceShapeFileExtension())) {
      SurfaceShapeFile ssf;
      readFileForTiming(&ssf, fileName, timeInSeconds, fileSizeInMB);
   }
   else if (fileName.endsWith(SpecFile::getTopoFileExtension())) {
      TopologyFile tf;
      readFileForTiming(&tf, fileName, timeInSeconds, fileSizeInMB);
   }
   else  {
      throw CommandException("Unsupported file type for timing.");
   }
   
   std::cout << "Average time to read file was "
             << timeInSeconds
             << " seconds." << std::endl;
   if (fileSizeInMB > 0) {
      std::cout << "File Size (MB): "
                << fileSizeInMB
                << std::endl;
   }
}

      


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

#include "BrainModelCiftiCorrelationMatrix.h"
#include "BrainSet.h"
#include "CommandCiftiCorrelationMatrix.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include <DebugControl.h>

/**
 * constructor.
 */
CommandCiftiCorrelationMatrix::CommandCiftiCorrelationMatrix()
   : CommandBase("-cifti-correlation-matrix",
                 "CIFTI CORRELATION MATRIX")
{
}

/**
 * destructor.
 */
CommandCiftiCorrelationMatrix::~CommandCiftiCorrelationMatrix()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandCiftiCorrelationMatrix::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Cifti File Name", FileFilters::getMetricFileFilter());
   paramsOut.addFile("Output Cifti File Name", FileFilters::getMetricFileFilter());
   paramsOut.addVariableListOfParameters("Options");
}

/**
 * get full help information.
 */
QString 
CommandCiftiCorrelationMatrix::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-cifti-dense-timeseries>\n"
       + indent9 + "<output-cifti-dense-connectivity>\n"
       + indent9 + "[-output-gifti-external-binary filename]\n"//specified in .gii, .dat is created automatically
       + indent9 + "[-apply-fisher-z-transform]\n"
       + indent9 + "[-parallel]\n"
       + indent9 + "\n"
       + indent9 + "Compute a correlation matrix using the input cifti file.\n"
       + indent9 + "Each row (node) in the cifti file is correlated with all\n"
       + indent9 + "other rows in the cifti file.  Each column in the cifti\n"
       + indent9 + "file represents an \"observation\" at each node.  \n"
       + indent9 + "\n"
       + indent9 + "The number of rows and columns are the same in the output\n"
       + indent9 + "cifti file and are equal to the number of rows from the \n"
       + indent9 + "input cifti file.  The values in the output cifti file \n"
       + indent9 + "form a symmetric matrix with each element containing \n"
       + indent9 + "a correlation coefficient for the data of two nodes where\n"
       + indent9 + "the node numbers are the row and column indices.\n"
       + indent9 + "\n"
       + indent9 + "An example input cifti file is one in which each row\n"
       + indent9 + "represents nodes and each column represents timepoints. \n"
       + indent9 + "The output cifti file contains correlation coefficients\n"
       + indent9 + "that measure the similarity of the timepoints from all\n"
       + indent9 + "nodes to all nodes.\n"
       + indent9 + "\n"
       + indent9 + "If \"-output-gifti-external-binary\" option is specified a\n"
       + indent9 + "a gifti file header and external binary are written to disk.\n"
       + indent9 + "The command expects a gifti file to be specified here, and\n"
       + indent9 + "an externa binary *.dat file is created with the same prefix.\n"
       + indent9 + "\n"
       + indent9 + "If \"-apply-fisher-z-transform\" option is specified a\n"
       + indent9 + "a fischer transform is applied to the correlation values.\n"
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
CommandCiftiCorrelationMatrix::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the input and output file names
   //
   const QString inputCiftiFileName = 
      parameters->getNextParameterAsString("Input Cifti File Name");
   const QString outputCiftiFileName = 
      parameters->getNextParameterAsString("Output Cifti File Name");
   bool applyFisherZTransformFlag = false;
   bool parallelFlag = false;
   QString outputGiftiFileName;
      
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
      else if (paramName == "-output-gifti-external-binary") {
         outputGiftiFileName = parameters->getNextParameterAsString("Output Gifti File Name");
      }
      else {
         throw CommandException("Unrecognized parameter: " + paramName);
      }
   }      

   //
   // Read the cifti file
   //
   CiftiFile cf;
 
   QTime readTimer;
   readTimer.start();
   cf.openFile(inputCiftiFileName);
   if(DebugControl::getDebugOn()) 
      std::cout << "Time to read file "
                << (readTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;

   
   QTime algTimer;
   algTimer.start();
   BrainModelCiftiCorrelationMatrix* alg = NULL;

   BrainSet brainSet;
   alg = new BrainModelCiftiCorrelationMatrix(&brainSet,
                                                &cf,
                                                applyFisherZTransformFlag,
                                                parallelFlag);

   alg->execute();
   if(DebugControl::getDebugOn()) 
      std::cout << "Time to run algorithm "
                << (algTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
             
   
   //
   // Write cifti file
   //
   
   QTime writeTimer;
   writeTimer.start();
   
   CiftiFile* outputCiftiFile = alg->getOutputCiftiFile();
   outputCiftiFile->writeFile(outputCiftiFileName);
   //delete outputCiftiFile;
         delete alg;

   if(outputGiftiFileName.length() > 0)
   {
      QString binaryFileName;
      binaryFileName = outputGiftiFileName;
      QRegExp rx;
      rx.setPatternSyntax(QRegExp::Wildcard);
      rx.setPattern("*.gii");
      if(rx.exactMatch(outputGiftiFileName))
      {
         binaryFileName.truncate(binaryFileName.length()-4);
         binaryFileName.append(".dat");
      }
      std::vector <int> dimensions;
      float *data;
      CiftiMatrix *matrix = outputCiftiFile->getCiftiMatrix();
      matrix->getMatrixData(data, dimensions);
      writeGiftiHeader(outputGiftiFileName,binaryFileName,dimensions);    
      writeGiftiExternalBinary(binaryFileName,matrix);
   }
   
   if(DebugControl::getDebugOn()) 
      std::cout << "Time to write file "
                << (writeTimer.elapsed() * 0.001)
                << " seconds."
                << std::endl;
   
}

//hack
/*
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE GIFTI SYSTEM "http://www.nitrc.org/frs/download.php/115/gifti.dtd">
<GIFTI Version="1.0"  NumberOfDataArrays="1">
   <MetaData/>
   <LabelTable/>
   <DataArray  ArrayIndexingOrder="ColumnMajorOrder"
               DataType="NIFTI_TYPE_FLOAT32"
               Dim0="48085"
               Dim1="48085"
               Dimensionality="2"
               Encoding="ExternalFileBinary"
               Endian="LittleEndian"
               ExternalFileName="DenseConnectome.dat"
               ExternalFileOffset="0"
               Intent="NIFTI_INTENT_NONE">
      <MetaData>
      </MetaData>
      <Data></Data>
   </DataArray>
</GIFTI>
*/
void 
CommandCiftiCorrelationMatrix::writeGiftiHeader(QString &fileName, QString &binaryFileName, std::vector < int > &dimensions)
{
   QFile file;
   file.setFileName(fileName);
   file.open(QIODevice::WriteOnly);
   QString indent12 = "            ";
   QString header = 
   (QString::fromAscii("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
    + QString::fromAscii("<!DOCTYPE GIFTI SYSTEM \"http://www.nitrc.org/frs/download.php/115/gifti.dtd\">\n")
    + QString::fromAscii("<GIFTI Version=\"1.0\"  NumberOfDataArrays=\"1\">\n")
    + indent3 + "<MetaData/>\n"
    + indent3 + "<LabelTable/>\n"
    + indent3 + "<DataArray  ArrayIndexingOrder=\"ColumnMajorOrder\"\n"
    + indent12 + "DataType=\"NIFTI_TYPE_FLOAT32\"\n"
    + indent12 + "Dim0=\"" + QString::number(dimensions[0]) + "\"\n"
    + indent12 + "Dim1=\"" + QString::number(dimensions[1]) + "\"\n"
    + indent12 + "Dimensionality=\"2\"\n"
    + indent12 + "Encoding=\"ExternalFileBinary\"\n"
    + indent12 + "Endian=\"LittleEndian\"\n"
    + indent12 + "ExternalFileName=\""+ binaryFileName + "\"\n"
    + indent12 + "ExternalFileOffset=\"0\"\n"
    + indent12 + "Intent=\"NIFTI_INTENT_NONE\">\n"
    + indent6 + "<MetaData>\n"
    + indent6 + "</MetaData>\n"
    + indent6 + "<Data></Data>\n"
    + indent3 + "</DataArray>\n"
    + QString::fromAscii("</GIFTI>\n")
    );
   
   QByteArray temp;
   temp.append(header);
   file.write(temp);
   file.close();
}

void CommandCiftiCorrelationMatrix::writeGiftiExternalBinary(QString& binaryFileName, CiftiMatrix *matrix)
{
   QFile file;
   file.setFileName(binaryFileName);
   file.open(QIODevice::WriteOnly);
   
   matrix->writeMatrix(file);
   file.close();
   
}


                         
                         
      


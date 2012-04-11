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

#include "Basename.h"
#include "CommandCreateCiftiDenseTimeseries.h"
#include "FileFilters.h"
#include "FileUtilities.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "CiftiFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "CiftiXMLWriter.h"
#include <matrix.h>
#include "nifti2.h"
#include <DebugControl.h>

/**
 * constructor.
 */
CommandCreateCiftiDenseTimeseries::CommandCreateCiftiDenseTimeseries()
   : CommandBase("-create-cifti-dense-timeseries",
                 "CREATE CIFTI DENSE TIMESERIES")
{
}

/**
 * destructor.
 */
CommandCreateCiftiDenseTimeseries::~CommandCreateCiftiDenseTimeseries()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandCreateCiftiDenseTimeseries::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
    paramsOut.clear();
    //paramsOut.addFile("Metric File Name", 
    //                  FileFilters::getMetricFileFilter());
}

/**
 * get full help information.
 */
QString 
CommandCreateCiftiDenseTimeseries::getHelpInformation() const
{
    QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "[-time-step value] (default of 1.0 seconds)\n"
       + "\n"
       + indent9 + "[-input-volume filename]\n"
       + "\n"
       + indent9 + "[-cifti-structure-name structurename]\n"
       + indent9 + "[[-input-surface-roi filename]]\n"
       + indent9 + "[-input-timeseries filename]\n"
       + indent9 + "\n"
       + indent9 + "[-cifti-structure-name structurename]\n"
       + indent9 + "[-input-volumetric-roi filename]\n"
       + indent9 + "...\n"
       + indent9 + "\n"
       + indent9 + "[-cifti-structure-name structurename]\n"
       + indent9 + "[[-input-surface-roi filename]]\n"
       + indent9 + "[-input-timeseries filename]\n"
       + indent9 + "\n"
       + indent9 + "[-cifti-structure-name structurename]\n"
       + indent9 + "[-input-volumetric-roi filename]\n"
       + indent9 + "...\n"
       + indent9 + "\n"
       + indent9 + "<-output-cifti-file filename>\n"
       + indent9 + "[-output-header filename]\n"
       + indent9 + "[-output-matrix filename]\n"
       + indent9 + "[-output-gifti-external-binary-header]\n"
       + indent9 + "\n"
       + indent9 + "Combine surface and volume timeseries into a single CIFTI dense timeseries\n"
       + indent9 + "file.  All inputs are optional but there must be at least one input.  If\n"
       + indent9 + "any volume structures are specified, a volume timeseries must precede the\n"
       + indent9 + "inputs.  For each column, a name and metric timeseries are required.  A\n"
       + indent9 + "ROI is optional.  For each volume, a volume ROI and name are required.\n"
       + indent9 + "CIFTI files can contain both surface and volume structures, or only one\n"
       + indent9 + "or the other.  All structures must be given a name.\n"
       + indent9 + "Common names include:\n"
       + indent9 + "CIFTI_STRUCTURE_ACCUMBENS_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_ACCUMBENS_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_AMYGDALA_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_AMYGDALA_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_BRAIN_STEM\n"
       + indent9 + "CIFTI_STRUCTURE_CAUDATE_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_CAUDATE_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_CEREBELLUM\n"
       + indent9 + "CIFTI_STRUCTURE_CEREBELLUM_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_CEREBELLUM_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_CORTEX_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_CORTEX_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_HIPPOCAMPUS_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_HIPPOCAMPUS_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_OTHER\n"
       + indent9 + "CIFTI_STRUCTURE_PALLIDUM_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_PALLIDUM_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_PUTAMEN_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_PUTAMEN_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_SUBCORTICAL_WHITE_MATTER_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_SUBCORTICAL_WHITE_MATTER_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_THALAMUS_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_THALMUS_RIGHT\n"
       + indent9 + "CIFTI_STRUCTURE_DIENCEPHALON_VENTRAL_LEFT\n"
       + indent9 + "CIFTI_STRUCTURE_DIENCEPHALON_VENTRAL_RIGHT\n"
       + indent9 + "\n"
       + indent9 + "The only names that must be the same as above, if used are\n"
       + indent9 + "CIFTI_STRUCTURE_CORTEX_LEFT and CIFTI_STRUCTURE_CORTEXT_RIGHT\n"
       + indent9 + "so that viewers know the structure is a left or right surface.\n"
       + indent9 + "The CIFTI file output will be ordered in the order of the inputs.\n"
       + indent9 + "It is possible to also output a text CIFTI header and GIFTI\n"
       + indent9 + "external binary file for compatibility with Matlab.  A CIFTI file\n"
       + indent9 + "output must be specified.\n"
       + indent9 + "\n"
       + indent9 + "\n");
      
    return helpInfo;
}

typedef enum {
   VOLUME,
   SURFACE
} structType;


   
typedef struct { 
   QString structureName; 
   union {
      VolumeFile * roiV;
      MetricFile * roiM;
   };
   structType type;
   MetricFile *valuesFile;
   std::vector < std::vector < unsigned long long > > ijk;//ijk values inside the ROI
} ciftiStructType;

typedef struct {
   QString structureName;
   QString roiFileName;
   QString valFileName;
   structType type;
} ciftiStructParamsType;
//forward declarations for local functions

//void getMaskedVolumeValues(VolumeFile *vol, VolumeFile *roi, int timeSlice, std::vector< float > &values, std::vector < std::vector < unsigned long long > > &ijk) throw (FileException);
//void getMaskedSurfaceValues(MetricFile *valuesFile, MetricFile *roiFile, int timeSlice, std::vector < float > &maskedValues, std::vector < unsigned long long > & indices) throw (FileException);
void createCiftiFile(QString &inputVolume, std::vector < ciftiStructParamsType > &ciftiStructures, Nifti2Header &header, CiftiXML &xml, CiftiMatrix &matrix, float timeStep) throw (FileException);
void openCiftiStructures(QString &volumeFileName, VolumeFile *&vol, std::vector < ciftiStructParamsType > &ciftiStructureParams, std::vector < ciftiStructType > &ciftiStructures) throw (FileException);

void getVolumeValuesOnly(VolumeFile *&vol, int timeSlice, std::vector < std::vector < unsigned long long > > &ijk, std::vector< float > &values) throw (FileException)
{
   vol->readFile(vol->getFileName(),timeSlice);
   values.resize(ijk.size());
   int size = ijk.size();   

   for(int i = 0;i < size;i++)
   {
	  values[i]=vol->getVoxel(ijk.at(i)[0],ijk.at(i)[1],ijk.at(i)[2]);      
   }
}

void getVolumeValues(VolumeFile *&vol, VolumeFile *roi, int timeSlice, std::vector< float > &values, std::vector < std::vector < unsigned long long > > &ijk) throw (FileException)
{
   int dim[3], roiDim[3];
   vol->readFile(vol->getFileName(),timeSlice);
   vol->getDimensions(dim);
   roi->getDimensions(roiDim);
   if((dim[0] != roiDim[0]) || (dim[1] != roiDim[1]) || (dim[2] != roiDim[2])) throw FileException("ROI Dimensions do not match the volume dimensions");
   for(int i = 0;i < dim[0];i++)
   {
      for(int j = 0;j < dim[1];j++)
      {
         for(int k = 0;k < dim[2];k++)
         {
            if(roi->getVoxel(i,j,k) > 0.0f)
            {
               values.push_back(vol->getVoxel(i,j,k));
               ijk.push_back(std::vector <unsigned long long> (3));
               ijk[values.size()-1][0]= i;
               ijk[values.size()-1][1]= j;
               ijk[values.size()-1][2]= k;
            }
         }
      }
   }
}

void getSurfaceValues(MetricFile* valuesFile, MetricFile* roiFile, int timeSlice, std::vector < float > &maskedValues, std::vector < unsigned long long > & indices) throw (FileException)
{
   std::vector < float > values;
   std::vector < float > roi;
   if(roiFile == NULL) 
   {
      valuesFile->getColumnForAllNodes(timeSlice,maskedValues);
      return;
   }
   
   valuesFile->getColumnForAllNodes(timeSlice,values);
   roiFile->getColumnForAllNodes(0,roi);
   if(values.size() != roi.size()) throw FileException("ROI node count does not match the number of surface nodes.");
   int count = values.size();
   for(long long  i = 0;i < count;i++)
   {
      if(roi[i] > 0.0f)
      {
         maskedValues.push_back(values[i]);
         indices.push_back(i);
      }
   }
}

void createCiftiFile(QString &inputVolume, std::vector < ciftiStructParamsType > &ciftiStructureParams, Nifti2Header &header, CiftiXML &xml, CiftiMatrix &matrix,float timeStep) throw (FileException)
{
   VolumeFile * vol = NULL;
   std::vector < ciftiStructType > ciftiStructures;
   openCiftiStructures(inputVolume, vol, ciftiStructureParams, ciftiStructures);
   //first we need to create a valid Nifti Header
   
   int timeCount = -1; //timeCount should match for each set of metric data that we load, if not, then trigger an error
   int nodeCount = 0;
   int dim[3] = { 0, 0, 0 };//dimensions of volume file
   if(vol)
   {
      timeCount = vol->getNumberOfSubVolumes();
      if(timeCount < 1)
      {
         throw FileException("Volume File doesn't contain enough rows");
      }      
      vol->getDimensions(dim);      
   }
   
   for(unsigned int i = 0;i<ciftiStructures.size();i++)
   {
      if(ciftiStructures.at(i).type == SURFACE)
      {
         MetricFile * valuesFile = ciftiStructures.at(i).valuesFile;
         int time = valuesFile->getNumberOfColumns();
         if(timeCount != -1 && time != timeCount)
         {
            CiftiFileException("Input timeseries do not have the same number of time points.");
         }
         else if(timeCount == -1)
         {              
            timeCount = time;
         }
         
         std::vector < float > values;
         std::vector < unsigned long long > indices;
         getSurfaceValues(valuesFile, ciftiStructures.at(i).roiM, 0, values, indices);
         
         nodeCount += values.size();
      }
      else if(ciftiStructures.at(i).type == VOLUME)
      {
         std::vector < float > values;
         std::vector < std::vector < unsigned long long > > ijk;
         getVolumeValues(vol,ciftiStructures.at(i).roiV,0,values,ijk);
         nodeCount += values.size();         
      }
   }
   header.initTimeSeriesHeaderStruct();
   nifti_2_header head;
   header.getHeaderStruct(head);
   head.dim[5] = nodeCount;
   head.dim[6] = timeCount;
   header.setHeaderStuct(head);
   
   //now create the Cifti XML
   CiftiRootElement root;
   root.m_version = "1.0";
   root.m_numberOfMatrices = 1;

   root.m_matrices.push_back(CiftiMatrixElement());
   CiftiMatrixElement *me = &(root.m_matrices.at(0));

   if(vol)
   {
	  me->m_volume.push_back(CiftiVolumeElement());
      CiftiVolumeElement *volume =&(me->m_volume.at(0));
      volume->m_volumeDimensions[0] = dim[0];
      volume->m_volumeDimensions[1] = dim[1];
      volume->m_volumeDimensions[2] = dim[2];
      
      VolumeFile::ORIENTATION orientation[3];
      TransformationMatrixVoxelIndicesIJKtoXYZElement voxIndTrans;
      voxIndTrans.m_dataSpace = NIFTI_XFORM_UNKNOWN;
      vol->getOrientation(orientation);
      if ((orientation[0] == VolumeFile::ORIENTATION_LEFT_TO_RIGHT) && 
         (orientation[1] == VolumeFile::ORIENTATION_POSTERIOR_TO_ANTERIOR) &&
         (orientation[2] == VolumeFile::ORIENTATION_INFERIOR_TO_SUPERIOR)) {
         voxIndTrans.m_transformedSpace = NIFTI_XFORM_TALAIRACH;
      }
      else {
         voxIndTrans.m_transformedSpace = NIFTI_XFORM_SCANNER_ANAT;         
      }
      
      float spacing[3];
      float origin[3];
      vol->getSpacing(spacing);
      vol->getOrigin(origin);
      
      voxIndTrans.m_transform[0] = spacing[0];
      voxIndTrans.m_transform[1] = 0.0;
      voxIndTrans.m_transform[2] = 0.0;
      voxIndTrans.m_transform[3] = origin[0];
      voxIndTrans.m_transform[4] = 0.0;
      voxIndTrans.m_transform[5] = spacing[1];
      voxIndTrans.m_transform[6] = 0.0;
      voxIndTrans.m_transform[7] = origin[1];
      voxIndTrans.m_transform[8] = 0.0;
      voxIndTrans.m_transform[9] = 0.0;
      voxIndTrans.m_transform[10] = spacing[2];
      voxIndTrans.m_transform[11] = origin[2];
      voxIndTrans.m_transform[12] = 0.0;
      voxIndTrans.m_transform[13] = 0.0;
      voxIndTrans.m_transform[14] = 0.0;
      voxIndTrans.m_transform[15] = 1.0;
      voxIndTrans.m_unitsXYZ = NIFTI_UNITS_MM;
      
      volume->m_transformationMatrixVoxelIndicesIJKtoXYZ.push_back( voxIndTrans);
      
   }
   
   // create matrix indices map for spacing
   root.m_matrices.at(0).m_matrixIndicesMap.push_back(CiftiMatrixIndicesMapElement());
   CiftiMatrixIndicesMapElement * mm = &(root.m_matrices.at(0).m_matrixIndicesMap.at(0));
   mm->m_indicesMapToDataType = CIFTI_INDEX_TYPE_BRAIN_MODELS;
   mm->m_appliesToMatrixDimension.push_back(0);
   //create brain models that belong to matrix indices map
   int totalIndexCount = 0;
   for(unsigned int i = 0;i < ciftiStructures.size();i++)
   {
      ciftiStructType * item = &(ciftiStructures[i]);
      mm->m_brainModels.push_back(CiftiBrainModelElement());
      CiftiBrainModelElement * bm = &(mm->m_brainModels[i]);
      bm->m_brainStructure = item->structureName;
      bm->m_indexOffset = totalIndexCount;
      if(item->type == SURFACE) 
      {
         bm->m_modelType = CIFTI_MODEL_TYPE_SURFACE;
         if(item->roiM)
            bm->m_surfaceNumberOfNodes = item->roiM->getNumberOfNodes();
         else
            bm->m_surfaceNumberOfNodes = item->valuesFile->getNumberOfNodes();
         std::vector < float > values;
         std::vector < unsigned long long > indices;
         if(item->roiM)
         {
            getSurfaceValues(item->valuesFile, item->roiM, 0, values, indices);
            bm->m_indexCount = indices.size();
            bm->m_nodeIndices = indices;         
         }
         else // look at spec 
         {
            bm->m_indexCount = bm->m_surfaceNumberOfNodes;         
         }
      }
      else
      {
         bm->m_modelType = CIFTI_MODEL_TYPE_VOXELS;
         
         std::vector < float > values;
         std::vector < std::vector < unsigned long long > > ijk;
         std::vector < unsigned long long > indices;
         getVolumeValues(vol,item->roiV, 0, values, ijk);
         item->ijk = ijk;
         indices.resize(values.size() * 3);
         for(unsigned int j = 0;j<values.size();j++) 
         {
            indices[j*3]=ijk[j][0];
            indices[j*3+1]=ijk[j][1];
            indices[j*3+2]=ijk[j][2];
         }
         bm->m_voxelIndicesIJK = indices;
         bm->m_indexCount = ijk.size();
      }
      totalIndexCount += bm->m_indexCount;      
   }
   //create matrix indices map for time
   root.m_matrices.at(0).m_matrixIndicesMap.push_back(CiftiMatrixIndicesMapElement());
   mm = &(root.m_matrices.at(0).m_matrixIndicesMap[1]);   
   mm->m_indicesMapToDataType = CIFTI_INDEX_TYPE_TIME_POINTS;
   mm->m_appliesToMatrixDimension.push_back(1);
   mm->m_timeStep = timeStep;
   mm->m_timeStepUnits = NIFTI_UNITS_SEC;
   
   //finished creating cifti xml, set root element
   xml.setXMLRoot(root);
   
   // now create cifti matrices
   float *matrixVals = new float[nodeCount*timeCount];
   
   
   for(int i = 0;i<timeCount;i++)
   {
      int offset = 0;
      if (DebugControl::getDebugOn()) std::cout << "Adding time slice number: " << i << std::endl;
      for(unsigned int s = 0;s<ciftiStructures.size();s++)
      {
         std::vector < float > tempval;
         std::vector < unsigned long long > indices;     
         ciftiStructType *item = &(ciftiStructures.at(s));
         if(item->type == SURFACE) 
            getSurfaceValues(item->valuesFile,item->roiM, i, tempval,indices );
         else 
            getVolumeValuesOnly(vol, i, item->ijk,tempval);
         
         /* we have to increment by points since we don't have an easy way to select a given voxel from multiple sub-volumes at once */
         for(unsigned int j = 0;j<tempval.size();j++)
         {
            matrixVals[j*timeCount+i+offset*timeCount] = tempval[j];          
         }
         offset+=tempval.size();
         //tempval.clear();
      }
   }
   matrix.setCopyData(false);
   std::vector < int > cifti_dimensions(2,0);
   cifti_dimensions[0] = nodeCount;//M
   cifti_dimensions[1] = timeCount;//N

   matrix.setMatrixData(matrixVals,cifti_dimensions);
}

void openCiftiStructures(QString &volumeFileName, VolumeFile * &vol, std::vector < ciftiStructParamsType > &ciftiStructureParams, std::vector < ciftiStructType > &ciftiStructures) throw (FileException)
{
   if(volumeFileName.length())
   {
      vol = new VolumeFile;
      vol->readFile(volumeFileName);
   }
   for(unsigned int i=0;i< ciftiStructureParams.size();i++)
   {
	  ciftiStructures.push_back(ciftiStructType());
      ciftiStructures.at(i).structureName = ciftiStructureParams.at(i).structureName;
      ciftiStructures.at(i).type = ciftiStructureParams.at(i).type;
      if(ciftiStructures.at(i).type == SURFACE)
      {
         if(ciftiStructureParams.at(i).roiFileName.length())
         {
            ciftiStructures.at(i).roiM = new MetricFile();
            ciftiStructures.at(i).roiM->readFile(ciftiStructureParams.at(i).roiFileName);
         }
         if(ciftiStructureParams.at(i).valFileName.length())
         {
            ciftiStructures.at(i).valuesFile = new MetricFile();
            ciftiStructures.at(i).valuesFile->readFile(ciftiStructureParams.at(i).valFileName);
         }
         else
               throw FileException ("Each Surface Brain Structure Requires a corresponding Metric File.");
      }
      else if(ciftiStructures.at(i).type == VOLUME)
      {
         ciftiStructures.at(i).roiV = new VolumeFile();
         ciftiStructures.at(i).roiV->readFile(ciftiStructureParams.at(i).roiFileName);
      }
   }
}

/**
 * execute the command.
 */
#include <iostream>
void 
CommandCreateCiftiDenseTimeseries::executeCommand() throw (CommandException,
                                     FileException,
                                     ProgramParametersException
                                     )
{
    //
    // Get the name of the metric file
    //
    QString inputVolume;	
    
    std::vector < ciftiStructParamsType > ciftiStructureParams; // { structure-name, roi-name, input-time-series } -or- { structure-name, input-volumetric-roi }
    QString outputCiftiFile;
    QString outputHeaderFile;
    QString outputMatrixFile;
    QString outputGiftiHeaderFile;
    int structLast=-1;//last Element in Cifti Structures vector
    float timeStep = 1.0;
    
    
    while (parameters->getParametersAvailable()) {      
      const QString paramValue = parameters->getNextParameterAsString("Create Cifti Dense Time Series Parameter");
      if (paramValue == "-input-volume") { 
         inputVolume = parameters->getNextParameterAsString("Input Volume");         
      }
      else if(paramValue == "-cifti-structure-name")
      {
         structLast++;
         ciftiStructureParams.push_back(ciftiStructParamsType());
         ciftiStructureParams.at(structLast).structureName = parameters->getNextParameterAsString("Cifti Structure Name");
      }
      else if(paramValue == "-input-surface-roi")
      {
         ciftiStructureParams.at(structLast).roiFileName = parameters->getNextParameterAsString("Input Surface ROI");
      }
      else if(paramValue == "-input-timeseries")
      {
         ciftiStructureParams.at(structLast).valFileName = parameters->getNextParameterAsString("Input Timeseries");
         ciftiStructureParams.at(structLast).type = SURFACE;
      }
      else if(paramValue == "-input-volumetric-roi")//for volumetric structures
      {
         ciftiStructureParams.at(structLast).roiFileName = parameters->getNextParameterAsString("Input Volumetric ROI");
         ciftiStructureParams.at(structLast).type = VOLUME;
      }
      else if(paramValue == "-output-cifti-file")
      {
         outputCiftiFile = parameters->getNextParameterAsString("Output Cifti Dense Timeseries File");
      }
      else if(paramValue == "-output-header")
      {
         outputHeaderFile = parameters->getNextParameterAsString("Output Cifti XML Header");
      }
      else if(paramValue == "-output-matrix")
      {
         outputMatrixFile = parameters->getNextParameterAsString("Output Gifti External Binary Matrix");
      }
      else if(paramValue == "-time-step")
      {
         timeStep = parameters->getNextParameterAsFloat("Time Step");
      }
      else if(paramValue == "-output-gifti-external-binary-header")
      {
         outputGiftiHeaderFile = parameters->getNextParameterAsString("Output Gifti External Binary Header");         
      }      
      else {
         throw CommandException("Unrecognized parameter: " + paramValue);
      }
   }
   
   Nifti2Header header;
   CiftiXML xml;
   CiftiMatrix matrix;
   try {
      createCiftiFile(inputVolume, ciftiStructureParams, header, xml, matrix,timeStep);
   }
   catch (FileException e)
   {
	   std::cout << "An exception occured." << e.whatQString().toAscii().data() << std::endl;
   }
   CiftiFile outFile;
   outFile.setHeader(header);
   outFile.setCiftiXML(xml);
   outFile.setCiftiMatrix(matrix);
   outFile.writeFile(outputCiftiFile);
   //write cifti xml
   if(outputHeaderFile.length())
   {
      QByteArray text;
      xml.writeXML(text);
      QFile file;
      file.setFileName(outputHeaderFile);
      file.open(QIODevice::WriteOnly);
      file.write(text);
      file.close();
   }
   if(outputGiftiHeaderFile.length())
   {
      std::vector<int> dimensions(2,0);
      
      matrix.getDimensions(dimensions);
      QString text;
      text.append( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      text.append("<!DOCTYPE GIFTI SYSTEM \"http://www.nitrc.org/frs/download.php/115/gifti.dtd\">\n");
      text.append("<GIFTI Version=\"1.0\"  NumberOfDataArrays=\"1\">\n");
      text.append("   <MetaData/>\n");
      text.append("   <LabelTable/>\n");
      text.append("   <DataArray  ArrayIndexingOrder=\"RowMajorOrder\"\n");
      text.append("            DataType=\"NIFTI_TYPE_FLOAT32\"\n");
      text.append(QString::fromAscii("            Dim0=\"")+QString::number(dimensions[0])+"\"\n");
      text.append(QString::fromAscii("            Dim1=\"")+QString::number(dimensions[1])+"\"\n");
      text.append("            Dimensionality=\"2\"\n");
      text.append("            Encoding=\"ExternalFileBinary\"\n");
      text.append("            Endian=\"LittleEndian\"\n");
// no basename on Mac, use caret_common/Basename      text.append(QString::fromAscii("             ExternalFileName=\"")+ basename(outputMatrixFile.toAscii())+ "\"\n");
      text.append(QString::fromAscii("             ExternalFileName=\"")+ Basename((char*)qPrintable(outputMatrixFile))+ "\"\n");
      text.append("            ExternalFileOffset=\"0\"\n");
      text.append("            Intent=\"NIFTI_INTENT_NONE\">\n");
      text.append("   <MetaData>\n");
      text.append("   </MetaData>\n");
      text.append("   <Data></Data>\n");
      text.append("   </DataArray>\n");
      text.append("</GIFTI>\n");
      QFile file;
      file.setFileName(outputGiftiHeaderFile);
      file.open(QIODevice::WriteOnly);
      file.write(text.toAscii());
      file.close();
   }
   if(outputMatrixFile.length())
   {
      QFile file;
      file.setFileName(outputMatrixFile);
      file.open(QIODevice::WriteOnly);
      matrix.writeMatrix(file);
   }
   
}

      


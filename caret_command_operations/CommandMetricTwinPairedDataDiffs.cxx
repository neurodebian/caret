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

#include "CommandMetricTwinPairedDataDiffs.h"
#include "FileFilters.h"
#include "MetricFile.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"

/**
 * constructor.
 */
CommandMetricTwinPairedDataDiffs::CommandMetricTwinPairedDataDiffs()
   : CommandBase("-metric-twin-paired-data-diffs",
                 "METRIC TWIN PAIRED DATA DIFFERENCES")
{
}

/**
 * destructor.
 */
CommandMetricTwinPairedDataDiffs::~CommandMetricTwinPairedDataDiffs()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandMetricTwinPairedDataDiffs::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addFile("Input Metric File Name A", FileFilters::getMetricShapeFileFilter());
   paramsOut.addFile("InputMetric File Name B", FileFilters::getMetricShapeFileFilter());
   paramsOut.addString("Output Directory", "Twins");
}

/**
 * get full help information.
 */
QString 
CommandMetricTwinPairedDataDiffs::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<input-metric-file-group-A>\n"
       + indent9 + "<input-metric-file-group-B>\n"
       + indent9 + "<directory-for-diffs>\n"
       + indent9 + "\n"
       + indent9 + "Compare the two data files, assuming a paired relationship in the\n"
       + indent9 + "same column numbers in the two data files, e.g., the first pair of twins\n"
       + indent9 + "are both the first column in data file A and data file B, respectively\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandMetricTwinPairedDataDiffs::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
   //
   // Get the parameters
   //
   
   const QString inputAName = 
      parameters->getNextParameterAsString("Group A Input Name");
   const QString inputBName = 
      parameters->getNextParameterAsString("Group B Input Name");
   QString outputDir = 
      parameters->getNextParameterAsString("Diffs Output Directory");
   checkForExcessiveParameters();
   
   char mycharbuff[50];
   MetricFile a, b, outtwin, outnontwin;
   a.readFile(inputAName);
   b.readFile(inputBName);
   //
   // Check assumption that both files have the same number of subjects
   // Reasonable assumption for twin/nontwin comparison, makes little sense to abandon it, though it is
   //  possible with a little rewriting
   // WARNING: if this assumption is abandoned, rewrite the loop comparing all subjects within the same
   //          files to two loops, one loop for each file.
   //
   if (a.getNumberOfColumns() != b.getNumberOfColumns()) throw FileException("Files must have the same number of subjects.");
   std::vector<float> adata, bdata;
   //
   // Allocate array neccesary to write to files
   //
   float* diff;
   GiftiDataArray* diffarray;
   int nodes;
   //
   // Loop through all columns in composite file A
   //
   for(int i = 0; i < a.getNumberOfColumns(); ++i)
   {
      a.getColumnForAllNodes(i, adata);
      nodes = adata.size();
      std::vector<int> nodesvect;
      nodesvect.clear();
      nodesvect.push_back(nodes);
      //
	   // Loop through all columns in composite file B
      for(int j = 0; j < b.getNumberOfColumns(); ++j) 
      {
	       //
	       // Allocate memory for a new column in the output files
	       //
          diffarray = new GiftiDataArray(NULL, "Shape");
          diffarray->setDataType(GiftiDataArray::DATA_TYPE_FLOAT32);
          diffarray->setDimensions(nodesvect);
          diff = diffarray->getDataPointerFloat();
          b.getColumnForAllNodes(j, bdata);
            //
	    // Check assumption that both files have the same number of nodes
	    // Note: because of how this is tested through each loop, it makes absolutely sure that
	    //       All columns have the same number of nodes, in both files.
	    //       This is partially for peace of mind, as the file format does not allow
	    //        different size columns
	    //
	    if (nodes != static_cast<int>(bdata.size())) throw FileException("Files must have the same number of nodes!");
	    //
	    // Compare all nodes between the two columns
	    // Both files must have the same number of nodes, in the same order
	    // Data must be registered to have the same # node mean the same place on the brain
	    //
	    for (int k = 0; k < nodes; ++k)
            {
               diff[k] = adata[k]-bdata[k];
            }
	    //
	    // Generate the column label, workaround for changing an int to a string
	    //
	    QString label;
            //
	    // Separate twin from nontwin data
	    //
	    if (i == j)
            {
               outtwin.addDataArray(diffarray);
	       sprintf(mycharbuff, "%d", i + 1);
	       label = "twins #";
	       label = label + (QString)mycharbuff;
               outtwin.setColumnName(i, label);
            } else {
               outnontwin.addDataArray(diffarray);
	       sprintf(mycharbuff, "%d", i + 1);
               label = "subjects ";
	       label = label + (QString)mycharbuff;
               label = label + "A and ";
	       sprintf(mycharbuff, "%d", j + 1);
               label = label + (QString)mycharbuff;
               label = label + "B";
               outnontwin.setColumnName(outnontwin.getNumberOfColumns() - 1, label);
            }
         }
      }
      //
      // Compare the subjects in each file against the others in the same file
      // Both composite files must have the same number of subjects, again with
      // the same number of nodes
      //
      // Allocate 2 more data columns to analyse both sets at the same time
      // If the assumption that the composite files have the same number of subjects
      //   is abandoned, rewrite to analyse each file separately
      //
      std::vector<float> cdata, ddata;
      //
      // Loop through all the subjects
      //
      for(int i = 0; i < a.getNumberOfColumns(); ++i)
      {
         a.getColumnForAllNodes(i, adata);
         b.getColumnForAllNodes(i, bdata);
         nodes = adata.size();
         std::vector<int> nodesvect;
         nodesvect.clear();
         nodesvect.push_back(nodes);
	 //
	 // Loop through all subjects below the current subject
	 //
         for(int j = i + 1; j < a.getNumberOfColumns(); ++j)
	 {
            diffarray = new GiftiDataArray(NULL, "Shape");
            diffarray->setDataType(GiftiDataArray::DATA_TYPE_FLOAT32);
            diffarray->setDimensions(nodesvect);
            diff = diffarray->getDataPointerFloat();
            a.getColumnForAllNodes(j, cdata);
            b.getColumnForAllNodes(j, ddata);
            //
	    // Compare the two subjects at those indexes in file A
	    //
	    for (int k = 0; k < nodes; ++k)
            {
               diff[k] = adata[k]-cdata[k];
            }
	    //
	    // Record data, label column, reallocate array
	    //
            outnontwin.addDataArray(diffarray);
	    sprintf(mycharbuff, "%d", i + 1);
            QString label = "subjects ";
	    label = label + (QString)mycharbuff;
            label = label + "A and ";
	    sprintf(mycharbuff, "%d", j + 1);
            label = label + (QString)mycharbuff;
            label = label + "A";
            outnontwin.setColumnName(outnontwin.getNumberOfColumns() - 1, label);
            diffarray = new GiftiDataArray(NULL, "Shape");
            diffarray->setDataType(GiftiDataArray::DATA_TYPE_FLOAT32);
            diffarray->setDimensions(nodesvect);
            diff = diffarray->getDataPointerFloat();
            //
	    // Compare the two subjects at those indexes in file B
	    // WARNING: the indexes are generated due to the number of subjects in file A ONLY
	    //          comparing two files with the second having fewer columns will cause a
	    //          null pointer error below at ddata[k], where if the second file is
	    //          bigger, not all comparisons in it will be made.
	    //
	    for (int k = 0; k < nodes; ++k)
            {
               diff[k] = bdata[k]-ddata[k];
            }
	    //
	    // Record data, label column
	    //
            outnontwin.addDataArray(diffarray);
	    sprintf(mycharbuff, "%d", i + 1);
            label = "subjects ";
	    label = label + (QString)mycharbuff;
            label = label + "B and ";
	    sprintf(mycharbuff, "%d", j + 1);
            label = label + (QString)mycharbuff;
            label = label + "B";
            outnontwin.setColumnName(outnontwin.getNumberOfColumns() - 1, label);
         }
      }
      //
      // Write output difference files
      // Note: writing outtwin can be moved to before the comparisons within files
      //       move above if you want more of an idea on progress
      //       when the cross comparison is finished, about 2/3 of the processing is done
      //
      outtwin.writeFile(outputDir + "/twin_diff.surface_shape");
      outnontwin.writeFile(outputDir + "/non_twin_diff.surface_shape");
}

      


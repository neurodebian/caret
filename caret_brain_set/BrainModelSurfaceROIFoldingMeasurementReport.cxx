
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

#include <cmath>

#include "BrainModelSurface.h"
#include "BrainModelSurfaceCurvature.h"
#include "BrainModelSurfaceROIFoldingMeasurementReport.h"
#include "BrainModelSurfaceROINodeSelection.h"
#include "StringUtilities.h"
#include "SurfaceShapeFile.h"
#include "TopologyFile.h"

/**
 * constructor.
 */
BrainModelSurfaceROIFoldingMeasurementReport::BrainModelSurfaceROIFoldingMeasurementReport(
                                              BrainSet* bs,
                                              const BrainModelSurface* surfaceIn,
                                              const BrainModelSurfaceROINodeSelection* roiIn,
                                              const QString& headerTextIn,
                                              const bool semicolonSeparateReportFlagIn,
                                              const BrainModelSurface* hullSurfaceIn,
                                              const QString& metricFoldingMeasurementsFileNameIn)
   : BrainModelSurfaceROIOperation(bs,
                                   surfaceIn,
                                   roiIn),
     hullSurface(hullSurfaceIn),
     semicolonSeparateReportFlag(semicolonSeparateReportFlagIn),
     metricFoldingMeasurementsFileName(metricFoldingMeasurementsFileNameIn)
{
   setHeaderText(headerTextIn);
}

/**
 * destructor.
 */
BrainModelSurfaceROIFoldingMeasurementReport::~BrainModelSurfaceROIFoldingMeasurementReport()
{
}

/**
 * execute the operation.
 */
void 
BrainModelSurfaceROIFoldingMeasurementReport::executeOperation() throw (BrainModelAlgorithmException)
{
   //
   // Compute the numerous curvatures measurements for each node
   //
   std::vector<NodeCurvatureMeasure> cm;
   computeNodeCurvatureMeasurements(cm);
   
   //
   // Start creating the report
   //
   QString separator("");
   if (semicolonSeparateReportFlag) {
      separator = ";";
   }
   float roiArea = 0.0;
   createReportHeader(roiArea);
  
   //
   // Start creating the surface's folding measurements
   //
   FoldingMeasurements surfaceFoldingMeasurements;
   
   //
   // Loop through the triangles
   //
   TopologyFile* tf = bms->getTopologyFile();
   const int numTriangles = tf->getNumberOfTiles();
   for (int i = 0; i < numTriangles; i++) {
      //
      // Is the triangle in the ROI ?
      //
      if (tileInROI[i]) {
         //
         // Get the nodes in the triangle
         //
         int n[3];
         tf->getTile(i, n);
         
         //
         // Compute and add tiles measurements to the surface's folding measurements
         //
         surfaceFoldingMeasurements.addTileFolding(operationSurfaceROI,
                                                   tileArea[i],
                                                   n,
                                                   cm);
       }
   }
   
   //
   // Finish the surface measurements
   //
   surfaceFoldingMeasurements.finalizeMeasurements(roiArea);

   const int textLen = 45;
   const int numLen  = 14;
   
   //
   // Create the remainder of the report
   //
   reportText.append(QString("Intrinsic Curvature Index (ICI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.ICI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Negative Intrinsic Curvature Index (NICI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.NICI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Gaussian L2 Norm (GLN)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.GLN, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Absolute Intrinsic Curvature Index (AICI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.AICI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Mean Curvature Index (MCI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.MCI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Negative Mean Curvature Index (NMCI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.NMCI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Mean L2 Form (MLN)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.MLN, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Absolute Mean Curvature Index (AMCI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.AMCI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Folding Index (FI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.FI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Curvedness Index (CI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.CI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Shape Index (SI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.SI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Area Fraction of ICI (FICI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.FICI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Area Fraction Negative ICI (FICI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.FNICI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Area Fraction of MCI (FMCI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.FMCI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("Area Fraction of Neg MCI (FNMCI)").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.FNMCI, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("SH2SH").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.SH2SH, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

   reportText.append(QString("SK2SK").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements.SK2SK, 'f', 5).rightJustified(
                                                                   numLen, ' ')
                     + "\n");

/*   
   reportText.append(QString("").leftJustified(textLen, ' ')
                     + QString::number(surfaceFoldingMeasurements., 'f', 5).leftJustified(
                                                                   numLen, ' ')
                     + "\n");
   int longestColumnNameLength = 10;
   for (int j = 0; j < numColumns; j++) {
      longestColumnNameLength = std::max(longestColumnNameLength,
               static_cast<int>(surfaceShapeFile->getColumnName(j).length()));
   }
   longestColumnNameLength += 5;
   
   QString s = StringUtilities::leftJustify("Name", longestColumnNameLength)
               + separator
               + StringUtilities::rightJustify("IFI", 12)
               + "\n";
   reportText.append(s);
   
   for (int j = 0; j < numColumns; j++) {
      const float integratedFloatingIndex = areaTimesFoldingSum[j] / roiArea;
      
      QString s = StringUtilities::leftJustify(surfaceShapeFile->getColumnName(j), longestColumnNameLength)
                  + separator
                  + StringUtilities::rightJustify(QString::number(integratedFloatingIndex, 'f', 6), 12)
                  + "\n";
      reportText.append(s);
   }
*/

   computeMetricFoldingMeasurementsFile(cm, operationSurfaceROI);
}

/*  
 * compute the node curvature measurements.
 */
void 
BrainModelSurfaceROIFoldingMeasurementReport::computeNodeCurvatureMeasurements(std::vector<NodeCurvatureMeasure>& cm) 
                                         throw (BrainModelAlgorithmException)
{
   //
   // Get principal curvatures
   //
   SurfaceShapeFile curvatureShapeFile;
   BrainModelSurfaceCurvature curvatures(brainSet,
                                         bms,
                                         &curvatureShapeFile,
                                         -1,
                                         -1,
                                         SurfaceShapeFile::meanCurvatureColumnName,
                                         SurfaceShapeFile::gaussianCurvatureColumnName,
                                         true);
   curvatures.execute();
   const int k1ShapeColumn = curvatures.getKMaxColumnNumber();
   const int k2ShapeColumn = curvatures.getKMinColumnNumber();

   if (k1ShapeColumn < 0) {
      throw new BrainModelAlgorithmException("K1 Curvature failed.");
   }
   if (k2ShapeColumn < 0) {
      throw new BrainModelAlgorithmException("K2 Curvature failed.");
   }
   
   //
   // Set the curvature measurements for each node
   //
   const int numNodes = curvatureShapeFile.getNumberOfNodes();
   cm.resize(numNodes);
   for (int i = 0; i < numNodes; i++) {
      cm[i].setMeasurements(curvatureShapeFile.getValue(i, k1ShapeColumn),
                            curvatureShapeFile.getValue(i, k2ShapeColumn));
   }
}

void
BrainModelSurfaceROIFoldingMeasurementReport::computeMetricFoldingMeasurementsFile(
                                            const std::vector<NodeCurvatureMeasure>& cm,
                                            const BrainModelSurfaceROINodeSelection* roi)
                                               throw (BrainModelAlgorithmException)
{
   if (metricFoldingMeasurementsFileName.isEmpty()) {
      return;
   }

   int numNodes = this->bms->getNumberOfNodes();
   
   int numColumns = 0;
   const int COLUMN_K1 = numColumns++;
   const int COLUMN_K2 = numColumns++;
   const int COLUMN_MEAN = numColumns++;
   const int COLUMN_GAUSS = numColumns++;
   const int COLUMN_ICI = numColumns++;
   const int COLUMN_NICI = numColumns++;
   const int COLUMN_GLN = numColumns++;
   const int COLUMN_AICI = numColumns++;
   const int COLUMN_MCI = numColumns++;
   const int COLUMN_NMCI = numColumns++;
   const int COLUMN_MLN = numColumns++;
   const int COLUMN_AMCI = numColumns++;
   const int COLUMN_FI = numColumns++;
   const int COLUMN_CI = numColumns++;
   const int COLUMN_SI = numColumns++;
   const int COLUMN_FICI = numColumns++;
   const int COLUMN_FNICI = numColumns++;
   const int COLUMN_FMCI = numColumns++;
   const int COLUMN_FNMCI = numColumns++;
   const int COLUMN_SH2SH = numColumns++;
   const int COLUMN_SK2SK = numColumns++;
   const int COLUMN_SURFACE_AREA = numColumns++;

   MetricFile mf;
   mf.setNumberOfNodesAndColumns(numNodes, numColumns);
   mf.setColumnName(COLUMN_K1, "K1");
   mf.setColumnComment(COLUMN_K1, "");
   mf.setColumnName(COLUMN_K2, "K2");
   mf.setColumnComment(COLUMN_K2, "");
   mf.setColumnName(COLUMN_MEAN, "Mean");
   mf.setColumnComment(COLUMN_MEAN, "");
   mf.setColumnName(COLUMN_GAUSS, "GAUSS");
   mf.setColumnComment(COLUMN_GAUSS, "");
   mf.setColumnName(COLUMN_ICI, "ICI (Intrinsic Curvature Index)");
   mf.setColumnComment(COLUMN_ICI, "");
   mf.setColumnName(COLUMN_NICI, "NICI (Negative Intrinsic Curvature Index)");
   mf.setColumnComment(COLUMN_NICI, "");
   mf.setColumnName(COLUMN_GLN, "GLN (Gaussian L2 Norm)");
   mf.setColumnComment(COLUMN_GLN, "");
   mf.setColumnName(COLUMN_AICI, "AICI (Absolute Intrinsic Curvature Index)");
   mf.setColumnComment(COLUMN_AICI, "");
   mf.setColumnName(COLUMN_MCI, "MCI (Mean Curvature Index)");
   mf.setColumnComment(COLUMN_MCI, "");
   mf.setColumnName(COLUMN_NMCI, "NMCI (Negative Mean Curvature Index)");
   mf.setColumnComment(COLUMN_NMCI, "");
   mf.setColumnName(COLUMN_MLN, "MLN (Mean L2 Form)");
   mf.setColumnComment(COLUMN_MLN, "");
   mf.setColumnName(COLUMN_AMCI, "AMCI (Absolute Mean Curvature Index)");
   mf.setColumnComment(COLUMN_AMCI, "");
   mf.setColumnName(COLUMN_FI, "FI (Folding Index)");
   mf.setColumnComment(COLUMN_FI, "");
   mf.setColumnName(COLUMN_CI, "CI (Curvedness Index)");
   mf.setColumnComment(COLUMN_CI, "");
   mf.setColumnName(COLUMN_SI, "SI (Shape Index)");
   mf.setColumnComment(COLUMN_SI, "");
   mf.setColumnName(COLUMN_FICI, "FICI (Area Fraction of ICI)");
   mf.setColumnComment(COLUMN_FICI, "");
   mf.setColumnName(COLUMN_FNICI, "FNICI (Area Fraction of Negative ICI)");
   mf.setColumnComment(COLUMN_FNICI, "");
   mf.setColumnName(COLUMN_FMCI, "FMCI (Area Fraction of MCI)");
   mf.setColumnComment(COLUMN_FMCI, "");
   mf.setColumnName(COLUMN_FNMCI, "FNMCI (Area Fraction of Negative MCI)");
   mf.setColumnComment(COLUMN_FNMCI, "");
   mf.setColumnName(COLUMN_SH2SH, "SH2SH");
   mf.setColumnComment(COLUMN_SH2SH, "");
   mf.setColumnName(COLUMN_SK2SK, "SK2SK");
   mf.setColumnComment(COLUMN_SK2SK, "");
   mf.setColumnName(COLUMN_SURFACE_AREA, "Surface Area");
   mf.setColumnComment(COLUMN_SURFACE_AREA, "");

   std::vector<float> nodeAreas;
   this->bms->getAreaOfAllNodes(nodeAreas);

   for (int i = 0; i < numNodes; i++) {
      if (roi->getNodeSelected(i)) {
         //
         // Add in partial amounts
         //
         const NodeCurvatureMeasure ncm = cm[i];
         float ici   = ncm.Kplus;
         float nici  = ncm.Kminus;
         float gln   = (ncm.K * ncm.K);
         float aici  = std::fabs(ncm.K);
         float mci   = ncm.Hplus;
         float nmci  = ncm.Hminus;
         float mln   = (ncm.H * ncm.H);
         float amci  = std::fabs(ncm.H);
         float fi    = ncm.fi;
         float ci    = ncm.ci;
         float si    = std::fabs(ncm.si);
         float fici  = ncm.KplusAreaMeasure;
         float fnici = ncm.KminusAreaMeasure;
         float fmci  = ncm.HplusAreaMeasure;
         float fnmci = ncm.HminusAreaMeasure;
         float sh2sh = 0.0;
         if (amci != 0.0) {
            sh2sh = mln / amci;
         }
         float sk2sk = 0.0;
         if (aici != 0.0) {
            sk2sk = gln / aici;
         }

         mf.setValue(i, COLUMN_K1, ncm.k1);
         mf.setValue(i, COLUMN_K2, ncm.k2);
         mf.setValue(i, COLUMN_MEAN, ncm.H);
         mf.setValue(i, COLUMN_GAUSS, ncm.K);
         mf.setValue(i, COLUMN_ICI, ici);
         mf.setValue(i, COLUMN_NICI, nici);
         mf.setValue(i, COLUMN_GLN, gln);
         mf.setValue(i, COLUMN_AICI, aici);
         mf.setValue(i, COLUMN_MCI, mci);
         mf.setValue(i, COLUMN_NMCI, nmci);
         mf.setValue(i, COLUMN_MLN, mln);
         mf.setValue(i, COLUMN_AMCI, amci);
         mf.setValue(i, COLUMN_FI, fi);
         mf.setValue(i, COLUMN_CI, ci);
         mf.setValue(i, COLUMN_SI, si);
         mf.setValue(i, COLUMN_FICI, fici);
         mf.setValue(i, COLUMN_FNICI,fnici);
         mf.setValue(i, COLUMN_FMCI, fmci);
         mf.setValue(i, COLUMN_FNMCI, fnmci);
         mf.setValue(i, COLUMN_SH2SH, sh2sh);
         mf.setValue(i, COLUMN_SK2SK, sk2sk);
         mf.setValue(i, COLUMN_SURFACE_AREA, nodeAreas[i]);
      }
   }

   for (int i = 0; i < numColumns; i++) {
      float minValue, maxValue;
      mf.getDataColumnMinMax(i, minValue, maxValue);
      //ssf.setColumnColorMappingMinMax(i, minValue, maxValue);
   }
   
   try {
      mf.writeFile(metricFoldingMeasurementsFileName);
   }
   catch (FileException& e) {
      throw BrainModelAlgorithmException("Writing Metric Folding Indices File: "
                                         + e.whatQString());
   }
}

//
//==============================================================================
//==============================================================================
//==============================================================================
//

/**
 * constructor.
 */
BrainModelSurfaceROIFoldingMeasurementReport::NodeCurvatureMeasure::NodeCurvatureMeasure()
{
}

/**
 * destructor.
 */
BrainModelSurfaceROIFoldingMeasurementReport::NodeCurvatureMeasure::~NodeCurvatureMeasure()
{
}

/**
 * Set the measurements.
 */
void
BrainModelSurfaceROIFoldingMeasurementReport::NodeCurvatureMeasure::setMeasurements(
                                                    const float k1In, const float k2In)
{
   k1 = k1In;
   k2 = k2In;
   H = (k1 + k2) / 2.0;
   K = (k1 * k2);
   Hplus =  ((H > 0.0) ? H : 0.0);
   Hminus = ((H < 0.0) ? H : 0.0);
   Kplus =  ((K > 0.0) ? K : 0.0);
   Kminus = ((K < 0.0) ? K : 0.0);
   HplusAreaMeasure =  ((H > 0.0) ? 1.0 : 0.0);
   HminusAreaMeasure = ((H < 0.0) ? 1.0 : 0.0);
   KplusAreaMeasure =  ((K > 0.0) ? 1.0 : 0.0);
   KminusAreaMeasure = ((K < 0.0) ? 1.0 : 0.0);
   ci = std::sqrt((k1*k1 + k2*k2) / 2.0);
   si = 0.0;
   const float denom = k2 - k1;
   //if (denom != 0.0) {
      si = (2.0 / M_PI) * std::atan2((k2 + k1), denom);
   //}
   fi = std::fabs(k1) * (std::fabs(k1) - std::fabs(k2));
}

//
//==============================================================================
//==============================================================================
//==============================================================================
//

/**
 * constructor.
 */
BrainModelSurfaceROIFoldingMeasurementReport::FoldingMeasurements::FoldingMeasurements()
{
   ICI = 0.0;
   NICI = 0.0;
   GLN = 0.0;
   AICI = 0.0;
   MCI = 0.0;
   NMCI = 0.0;
   MLN = 0.0;
   AMCI = 0.0;
   FI = 0.0;
   CI = 0.0;
   SI = 0.0;
   FICI = 0.0;
   FNICI = 0.0;
   FMCI = 0.0;
   FNMCI = 0.0;
   SH2SH = 0.0;
   SK2SK = 0.0;
   roiTotalArea = 0.0;
}

/**
 * destructor.
 */
BrainModelSurfaceROIFoldingMeasurementReport::FoldingMeasurements::~FoldingMeasurements()
{
}

/**
 * add folding measurements for a tile.
 */
void 
BrainModelSurfaceROIFoldingMeasurementReport::FoldingMeasurements::addTileFolding(
                             const BrainModelSurfaceROINodeSelection* roi,
                             const float tileFullArea,
                             const int tileNodes[3],
                             const std::vector<NodeCurvatureMeasure>& nodeCurvatures)
{
   const float oneThird = 1.0 / 3.0;
   
   float ici   = 0.0;
   float nici  = 0.0;
   float gln   = 0.0;
   float aici  = 0.0;
   float mci   = 0.0;
   float nmci  = 0.0;
   float mln   = 0.0;
   float amci  = 0.0;
   float fi    = 0.0;
   float ci    = 0.0;
   float si    = 0.0;
   float fici  = 0.0;
   float fnici = 0.0;
   float fmci  = 0.0;
   float fnmci = 0.0;
   
   float oneThirdTileArea = tileFullArea * oneThird;
   float tileAreaUsed = 0.0;
   
   //
   // Loop through nodes of tile
   //
   bool nodeUsedFlag = false;
   for (int n = 0; n < 3; n++) {
      const int nodeNumber = tileNodes[n];
      if (roi->getNodeSelected(nodeNumber)) {
         //
         // Add in partial amounts
         //
         const NodeCurvatureMeasure ncm = nodeCurvatures[nodeNumber];
         ici   += ncm.Kplus * oneThird;
         nici  += ncm.Kminus * oneThird;
         gln   += (ncm.K * ncm.K)  * oneThird; 
         aici  += std::fabs(ncm.K) * oneThird; 
         mci   += ncm.Hplus * oneThird; 
         nmci  += ncm.Hminus * oneThird; 
         mln   += (ncm.H * ncm.H) * oneThird; 
         amci  += std::fabs(ncm.H) * oneThird; 
         fi    += ncm.fi * oneThird; 
         ci    += ncm.ci * oneThird; 
         si    += std::fabs(ncm.si) * oneThird; 
         fici  += ncm.KplusAreaMeasure * oneThird; 
         fnici += ncm.KminusAreaMeasure * oneThird; 
         fmci  += ncm.HplusAreaMeasure * oneThird; 
         fnmci += ncm.HminusAreaMeasure * oneThird; 

         //
         // Each node uses approximately one third of the tile
         //
         tileAreaUsed += oneThirdTileArea;   
         
         nodeUsedFlag = true;
      }
   }
   
   //
   // Was the tile in the ROI
   //
   if (nodeUsedFlag) {
      ICI   +=   ici * tileAreaUsed;
      NICI  +=  nici * tileAreaUsed;
      GLN   +=   gln * tileAreaUsed;
      AICI  +=  aici * tileAreaUsed;
      MCI   +=   mci * tileAreaUsed;
      NMCI  +=  nmci * tileAreaUsed;
      MLN   +=   mln * tileAreaUsed;
      AMCI  +=  amci * tileAreaUsed;
      FI    +=    fi * tileAreaUsed;
      CI    +=    ci * tileAreaUsed;
      SI    +=    si * tileAreaUsed;
      FICI  +=  fici * tileAreaUsed;
      FNICI += fnici * tileAreaUsed;
      FMCI  +=  fmci * tileAreaUsed;
      FNMCI += fnmci * tileAreaUsed;
      
      roiTotalArea += tileAreaUsed;
   }
}

/**
 * finalize the measurements.
 */
void 
BrainModelSurfaceROIFoldingMeasurementReport::FoldingMeasurements::finalizeMeasurements(
                                                    const float roiSurfaceArea)
{
   ICI   /= roiSurfaceArea;
   NICI  /= roiSurfaceArea;
   GLN   /= roiSurfaceArea;
   AICI  /= roiSurfaceArea;
   MCI   /= roiSurfaceArea;
   NMCI  /= roiSurfaceArea;
   MLN   /= roiSurfaceArea;
   AMCI  /= roiSurfaceArea;
   FI    /= roiSurfaceArea;
   CI    /= roiSurfaceArea;
   SI    /= roiSurfaceArea;
   FICI  /= roiSurfaceArea;
   FNICI /= roiSurfaceArea;
   FMCI  /= roiSurfaceArea;
   FNMCI /= roiSurfaceArea;
   if (AMCI != 0.0) {
      SH2SH = MLN / AMCI;
   }
   if (AICI != 0.0) {
      SK2SK = GLN/AICI;
   }
   
   //std::cout << "ROI area: " << roiTotalArea << std::endl;
}
            

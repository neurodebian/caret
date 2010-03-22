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

/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: BrainModelVolumeBiasCorrection.cxx,v $
Language:  C++
Date:      $Date: 2009/04/09 15:50:54 $
Version:   $Revision: 1.1.1.1 $

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

//
// This code adapted from the ITK Application "BiasCorrector.cxx".
//

#include <QString>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "BrainModelVolumeBiasCorrection.h"
#include "DebugControl.h"
#include "StatisticDataGroup.h"
#include "StatisticMeanAndDeviation.h"
#include "VolumeFile.h"

#ifdef HAVE_ITK
#include "itkCompositeValleyFunction.h"
#include "itkMRIBiasFieldCorrectionFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkZeroCrossingBasedEdgeDetectionImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryCrossStructuringElement.h" 
#include "itkBinaryDilateImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkIndent.h"
#include "itkHistogram.h" 
#include "vnl/vnl_math.h"

typedef unsigned char MaskPixelType;
typedef itk::Image<MaskPixelType, 3> MaskType;
typedef MaskType::Pointer MaskPointer;

typedef itk::MRIBiasFieldCorrectionFilter<VolumeITKImage::ImageTypeFloat3, 
                                          VolumeITKImage::ImageTypeFloat3, MaskType> Corrector ;
#endif // HAVE_ITK

/**
 * constructor.
 */
BrainModelVolumeBiasCorrection::BrainModelVolumeBiasCorrection(VolumeFile* vf,
                                                               const float grayWhiteValuesIn[4],
                                                               const float lowerUpperThresholdsIn[2],
                                                               const int   axisIterationsIn[3])
   : BrainModelAlgorithm(NULL)
{
   volumeFile = vf;
   grayWhiteValues[0] = grayWhiteValuesIn[0];
   grayWhiteValues[1] = grayWhiteValuesIn[1];
   grayWhiteValues[2] = grayWhiteValuesIn[2];
   grayWhiteValues[3] = grayWhiteValuesIn[3];
   
   lowerUpperThresholds[0] = lowerUpperThresholdsIn[0];
   lowerUpperThresholds[1] = lowerUpperThresholdsIn[1];
   
   axisIterations[0] = axisIterationsIn[0];
   axisIterations[1] = axisIterationsIn[1];
   axisIterations[2] = axisIterationsIn[2];
}
                               
/**
 * destructor.
 */
BrainModelVolumeBiasCorrection::~BrainModelVolumeBiasCorrection()
{
}

#ifdef HAVE_ITK
void 
BrainModelVolumeBiasCorrection::remove_edges(VolumeITKImage::ImagePointer input, 
                                             VolumeITKImage::ImagePointer& NoEdgeImage)
{
  const int Dimension = 3;
  typedef itk::ZeroCrossingBasedEdgeDetectionImageFilter<VolumeITKImage::ImageTypeFloat3, 
                                          VolumeITKImage::ImageTypeFloat3> zeroCrossFilterType;
  typedef itk::CastImageFilter< VolumeITKImage::ImageTypeFloat3,  MaskType> castFilterType;
  typedef itk::MaskImageFilter< VolumeITKImage::ImageTypeFloat3,MaskType,
                                VolumeITKImage::ImageTypeFloat3 > maskFilterType;
  //typedef itk::BinaryBallStructuringElement<MaskPixelType,Dimension> StructuringElementType;
  typedef itk::BinaryCrossStructuringElement<MaskPixelType,Dimension> StructuringElementType;
  typedef itk::BinaryDilateImageFilter<MaskType, MaskType, StructuringElementType> dilateFilterType;
  typedef itk::BinaryThresholdImageFilter< MaskType,  MaskType> BinaryThresholdType;

  double variance[3];
  variance[0] = 2.5;
  variance[1] = 2.5;
  variance[2] = 2.5;
  
  //std::cout << " edge detection" << std::endl;
  zeroCrossFilterType::Pointer edgeFilter = zeroCrossFilterType::New();
  edgeFilter->SetInput(input);
  edgeFilter->SetVariance(variance);
  edgeFilter->Update();

  castFilterType::Pointer convFilter = castFilterType::New();
  convFilter->SetInput(edgeFilter->GetOutput());
  convFilter->Update();
 
  StructuringElementType structuringElement;
  int ballSize = 1;
  structuringElement.SetRadius( ballSize ); 
  structuringElement.CreateStructuringElement( );
  
  //   //std::cout << "dilate  edge image" << std::endl;
  dilateFilterType::Pointer dilateFilter = dilateFilterType::New();  
  dilateFilter->SetInput(convFilter->GetOutput());
  dilateFilter->SetDilateValue (1);
  dilateFilter->SetKernel( structuringElement );
  dilateFilter->Update();

  const int MaskValue = 1;
  const int NoMaskValue = 0;

  //std::cout << "invert edge image" << std::endl;
  // Not Operation on Image -> select places where there is NO Edge
  BinaryThresholdType::Pointer NotFilter = BinaryThresholdType::New();
  //NotFilter->SetInput(convFilter->GetOutput());
  NotFilter->SetInput(dilateFilter->GetOutput());
  NotFilter->SetOutsideValue(MaskValue);
  NotFilter->SetInsideValue(NoMaskValue);
  NotFilter->SetUpperThreshold(255);
  NotFilter->SetLowerThreshold(1);
  NotFilter->Update();


  //std::cout << "mask with inverted edge image" << std::endl;
  /** masking of the input image with the inverted&dilated edge image */
  maskFilterType::Pointer maskFilter = maskFilterType::New();
  maskFilter->SetInput1(input);
  maskFilter->SetInput2(NotFilter->GetOutput());
  maskFilter->Update();
  
  NoEdgeImage = maskFilter->GetOutput();
}
#endif // HAVE_ITK

#ifdef HAVE_ITK
void
BrainModelVolumeBiasCorrection::biasCorrection(VolumeITKImage::ImagePointer& input,
                                               VolumeITKImage::ImagePointer& output,
                                               const int sliceDirection,
                                               const bool useLog)
{
  // load images
  MaskPointer inputMask ;
  MaskPointer outputMask ;

  typedef itk::BinaryThresholdImageFilter< VolumeITKImage::ImageTypeFloat3,  MaskType> threshFilterType;
  static const int TRESH_VAL = 1;
  static const int BG_VAL = 0;

  int degree = 3;
  std::vector<double> InclassMeans ;
  std::vector<double> InclassSigmas ;
  int volumeMaximumIteration = axisIterations[sliceDirection];  // jwh 200; 
  int interSliceMaximumIteration = 200; // jwh 200; 
  double initialRadius = 1.01;
  double growth = 1.05;
  double shrink = pow(growth, -0.25);

  bool usingSlabIdentification = false;
  bool useIntersliceCorrection = false;

      
   //
   // Bias corrector filter
   //
   Corrector::Pointer filter = Corrector::New() ;
   filter->SetInput(input) ;

     //
     // Create Output Mask
     //
     threshFilterType::Pointer threshFilter1 = threshFilterType::New();
     threshFilter1->SetInput(input);
     threshFilter1->SetUpperThreshold(lowerUpperThresholds[1]); // 32000);
     threshFilter1->SetLowerThreshold(lowerUpperThresholds[0]); // percVal + 1);
     threshFilter1->SetOutsideValue( BG_VAL );
     threshFilter1->SetInsideValue( TRESH_VAL );
     threshFilter1->Update();        
     outputMask = threshFilter1->GetOutput() ;
      
   //
   // Create input mask
   //
   VolumeITKImage::ImagePointer NoEdgeImage ;
   remove_edges(input, NoEdgeImage);
   threshFilterType::Pointer threshFilter = threshFilterType::New();
   threshFilter->SetInput(NoEdgeImage);
   threshFilter->SetUpperThreshold(lowerUpperThresholds[1]); // 32000 );
   threshFilter->SetLowerThreshold(lowerUpperThresholds[0]); // percVal + 1 );
   threshFilter->SetOutsideValue( BG_VAL );
   threshFilter->SetInsideValue( TRESH_VAL );
   threshFilter->Update();
   inputMask = threshFilter->GetOutput() ;
      
   //
   // Set intputs and run the filter
   //
   if (DebugControl::getDebugOn()) {
      filter->DebugOn() ;
   }
   filter->SetOutputMask(outputMask) ;
   filter->SetInputMask(inputMask) ;
   filter->IsBiasFieldMultiplicative(useLog) ;
   filter->SetTissueClassStatistics(classMeans, classSigmas) ;
   filter->SetOptimizerGrowthFactor(growth) ;
   filter->SetOptimizerShrinkFactor(shrink) ;
   filter->SetVolumeCorrectionMaximumIteration(volumeMaximumIteration) ;
   filter->SetInterSliceCorrectionMaximumIteration(interSliceMaximumIteration) ;
   filter->SetOptimizerInitialRadius(initialRadius) ;
   filter->SetBiasFieldDegree(degree) ;
   filter->SetUsingSlabIdentification(usingSlabIdentification) ;
   filter->SetUsingInterSliceIntensityCorrection(useIntersliceCorrection) ;
   filter->SetSlicingDirection(sliceDirection) ;
   filter->SetUsingBiasFieldCorrection(true) ;
   filter->SetGeneratingOutput(true) ;
   filter->Update() ;

   //
   // Get output of filter
   //
   output = filter->GetOutput();

   if (DebugControl::getDebugOn()) {
      std::cout << " coefficients :" ;
      Corrector::BiasFieldType::CoefficientArrayType coefficients = 
          filter->GetEstimatedBiasFieldCoefficients() ;

      Corrector::BiasFieldType::CoefficientArrayType::iterator iter =
          coefficients.begin() ;

      while (iter != coefficients.end()) {
         std::cout << *iter << " " ;
         iter++ ;
      } 
      std::cout << std::endl ;
   }
}
#endif // HAVE_ITK

#ifdef HAVE_ITK
/**
 * Do energy function
 */
void
BrainModelVolumeBiasCorrection::energyFunctions() const
{
   if (DebugControl::getDebugOn()) {
      typedef itk::CompositeValleyFunction EnergyFunction ;
      int interval = 100;
     
      EnergyFunction energy(classMeans, classSigmas) ;
     
      double higher = energy.GetUpperBound() ;
      double lower = energy.GetLowerBound() ;
      long noOfSamples = energy.GetNumberOfSamples() ; 
      double TableInc = (double) ((higher - lower) / 
                                 (noOfSamples - 1));

      std::ofstream fout("energy.txt");
      
      double d = lower;
      std::cout << "intensity\tenergy" << std::endl ;
      int i = 0 ;
      while(i < noOfSamples) {
          std::cout << d << "\t" << energy(d) << std::endl ;
          fout  << d << "," << energy(d) << std::endl ;
          i += interval ;
          d += TableInc * interval ;
      }
      
      fout.close();
   }
}
#endif // HAVE_ITK

#ifdef HAVE_ITK
/**
 * determine means and variance.
 */
void 
BrainModelVolumeBiasCorrection::meansVariance(const bool useLog) 
{
   std::vector<float> grayVoxels, whiteVoxels;
   const int numVoxels = volumeFile->getTotalNumberOfVoxels();
   for (int m = 0; m < numVoxels; m++) {
      const float v = volumeFile->getVoxelWithFlatIndex(m);
      if ((v >= grayWhiteValues[0]) && (v <= grayWhiteValues[1])) {
         grayVoxels.push_back(v);
      }
      if ((v >= grayWhiteValues[2]) && (v <= grayWhiteValues[3])) {
         whiteVoxels.push_back(v);
      }
   }
     
   //
   // Mean and deviation of gray
   //
   StatisticDataGroup sdgGray(&grayVoxels, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticMeanAndDeviation smdGray;
   smdGray.addDataGroup(&sdgGray);
   try {
      smdGray.execute();
   }
   catch (StatisticException&) {
   }
   const float grayMean = smdGray.getMean();
   const float grayVar  = smdGray.getStandardDeviation();

   //
   // Mean and deviation of white
   //
   StatisticDataGroup sdgWhite(&whiteVoxels, StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticMeanAndDeviation smdWhite;
   smdWhite.addDataGroup(&sdgWhite);
   try {
      smdWhite.execute();
   }
   catch (StatisticException&) {
   }
   const float whiteMean = smdWhite.getMean();
   const float whiteVar  = smdWhite.getStandardDeviation();

   if (DebugControl::getDebugOn()) {
      std::cout << "Gray (mean, var): " << grayMean << ", " << grayVar << std::endl;
      std::cout << "White (mean, var): " << whiteMean << ", " << whiteVar << std::endl;
   }
   
   classMeans.SetSize(2);
   classSigmas.SetSize(2);
   classMeans[0] = grayMean;
   classMeans[1] = whiteMean;
   classSigmas[0] = grayVar;
   classSigmas[1] = whiteVar;
   
   if (useLog) { 
      for (unsigned int i = 0 ; i < classMeans.size() ; i++) {
         classSigmas[i] = log(1.0 + classSigmas[i] / (classMeans[i] + 1.0)) ;
         classMeans[i] = log(classMeans[i] + 1.0) ;
      }
   }

}
#endif // HAVE_ITK

/**
 * execute the algorithm.
 */
void 
BrainModelVolumeBiasCorrection::execute() throw (BrainModelAlgorithmException)
{
#ifdef HAVE_ITK
   if (volumeFile == NULL) {
      throw BrainModelAlgorithmException("Input volume is invalid (NULL).");
   }
   
   try {
      const bool useLog = false;
      meansVariance(useLog);
      
      energyFunctions();
      
      VolumeITKImage image;
      volumeFile->convertToITKImage(image);
      
      if (axisIterations[2] > 0) {
         VolumeITKImage imageOut;
         biasCorrection(image.image, imageOut.image, 2, useLog);
         image.image = imageOut.image;
      }
      
      if (axisIterations[0] > 0) {
         VolumeITKImage imageOut;
         biasCorrection(image.image, imageOut.image, 0, useLog);
         image.image = imageOut.image;
      }
      
      if (axisIterations[1] > 0) {
         VolumeITKImage imageOut;
         biasCorrection(image.image, imageOut.image, 1, useLog);
         image.image = imageOut.image;
      }
      
      volumeFile->convertFromITKImage(image);
      volumeFile->stretchVoxelValues();
      
      std::ostringstream str;
      str << "\n"
          << "Bias Correction:\n"
          << "   Iterations (X, Y, Z): "
          << axisIterations[0] << ", " << axisIterations[1] << ", " << axisIterations[2] << "\n"
          << "   Thresholds (Low, High): "
          << lowerUpperThresholds[0] << ", " << lowerUpperThresholds[1] << "\n"
          << "   Gray Range: "
          << grayWhiteValues[0] << ", " << grayWhiteValues[1] << "\n"
          << "   White Range: "
          << grayWhiteValues[2] << ", " << grayWhiteValues[3] << "\n"
          << "\n";
      volumeFile->appendToFileComment(str.str().c_str());
   }
   catch (itk::ExceptionObject& e) {
      throw BrainModelAlgorithmException(e.what());
   }
#else  // HAVE_ITK
   throw FileException("Bias Correction unavailable.  ITK library not available at compile time.");
#endif // HAVE_ITK
}      


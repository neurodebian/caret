

#ifndef __BRAIN_MODEL_VOLUME_SUREFIT_ERROR_CORRECTION_H__
#define __BRAIN_MODEL_VOLUME_SUREFIT_ERROR_CORRECTION_H__

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

#include <vector>

#include "BrainModelAlgorithm.h"
#include "DebugControl.h"
#include "VolumeFile.h"

class BrainModelSurface;
class MetricFile;

/// class for correcting topological errors in segmentation volumes
class BrainModelVolumeSureFitErrorCorrection : public BrainModelAlgorithm {
   public:
      // Constructor
      BrainModelVolumeSureFitErrorCorrection(BrainSet* bs,
                                             VolumeFile* segmentationVolumeIn,
                                             VolumeFile* radialPositionMapVolumeIn,
                                             const VolumeFile::FILE_READ_WRITE_TYPE typeOfVolumeFilesToWriteIn,
                                             const int acIJKIn[3],
                                             const bool leftHemFlagIn,
                                             const bool saveIntermediateFilesIn);
      
      // Destructor
      ~BrainModelVolumeSureFitErrorCorrection();
      
      // get the output volume
      VolumeFile* getOutputVolume() { return outputVolume; }
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
   
   protected:
      /// class for storing error information
      class ErrorStats {
         public:
            ErrorStats () {
               StartingHandles=0;
               TestObjectNum=0;
               UncorrectedObjects=0;
               CurrentHandles=0;
               MaskHandlesBefore=0;
               MaskCavitiesBefore=0;
               RemainingObjectNum=0;
               CurrentTestNum=1;
               CumulativeTestNum=0;
               ObjectsPatched=0;
               CavitiesPatched=0;
               DisconnectFlag=0;
            }

            ~ErrorStats ()  {
            }      

            int getStartingHandles() const { return StartingHandles; }
            int getTestObjectNum() const { return TestObjectNum; }
            int getUncorrectedObjects() const { return UncorrectedObjects; }
            int getCurrentHandles() const { return CurrentHandles; }
            int getMaskHandlesBefore() const { return MaskHandlesBefore; }
            int getMaskCavitiesBefore() const { return MaskCavitiesBefore; }
            int getRemainingObjectNum() const { return RemainingObjectNum; }
            int getCurrentTestNum() const { return CurrentTestNum; }
            int getCumulativeTestNum() const { return CumulativeTestNum; }
            int getObjectsPatched() const { return ObjectsPatched; }
            int getCavitiesPatched() const { return CavitiesPatched; }
            int getDisconnectFlag() const { return DisconnectFlag; }

            void setStartingHandles(int value) {
               this->StartingHandles=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "StartingHandles = " << this->StartingHandles << std::endl;
               }
            }

            void setTestObjectNum(int value)  {
               this->TestObjectNum=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "TestObjectNum = " << this->TestObjectNum << std::endl;
               }
            }

            void setUncorrectedObjects(int value) {
               this->UncorrectedObjects=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "UncorrectedObjects = " << this->UncorrectedObjects << std::endl;
               }
            }

            void setCurrentHandles(int value) {
               this->CurrentHandles=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "CurrentHandles = " << this->CurrentHandles << std::endl;
               }
            }

            void setMaskHandlesBefore(int value) {
               this->MaskHandlesBefore=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "MaskHandlesBefore = " << this->MaskHandlesBefore << std::endl;
               }
            }

            void setMaskCavitiesBefore(int value) {
               this->MaskCavitiesBefore=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "MaskCavitiesBefore = " << this->MaskCavitiesBefore << std::endl;
               }
            }

            void setRemainingObjectNum(int value) {
               this->RemainingObjectNum=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "RemainingObjectNum = " << this->RemainingObjectNum << std::endl;
               }
            }

            void setCurrentTestNum(int value) {
               this->CurrentTestNum=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "CurrentTestNum = " << this->CurrentTestNum << std::endl;
               }
            }

            void setCumulativeTestNum(int value) {
               this->CumulativeTestNum=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "CumulativeTestNum = " << this->CumulativeTestNum << std::endl;
               }
            }

            void setObjectsPatched(int value) {
               this->ObjectsPatched=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "ObjectsPatched = " << this->ObjectsPatched << std::endl;
               }
            }

            void setCavitiesPatched(int value) {
               this->CavitiesPatched=value;
               if (DebugControl::getDebugOn()) {
                  std::cout << "CavitiesPatched = " << this->CavitiesPatched << std::endl;
               }
            }

            void setDisconnectFlag(int value) {
               this->DisconnectFlag=value;
               if (DebugControl::getDebugOn()) {
                  if ( this->DisconnectFlag > 0 ) {
                     std::cout << "Handle patched" << std::endl;
                  }
               }
            }

            void print () {
               std::cout << "StartingHandles = " << this->StartingHandles << std::endl;
               std::cout << "TestObjectNum = " << this->TestObjectNum << std::endl;
               std::cout << "UncorrectedObjects = " << this->UncorrectedObjects << std::endl;
               std::cout << "CurrentHandles = " << this->CurrentHandles << std::endl;
               std::cout << "MaskHandlesBefore = " << this->MaskHandlesBefore << std::endl;
               std::cout << "MaskCavitiesBefore = " << this->MaskCavitiesBefore << std::endl;
               std::cout << "RemainingObjectNum = " << this->RemainingObjectNum << std::endl;
               std::cout << "CurrentTestNum = " << this->CurrentTestNum << std::endl;
               std::cout << "CumulativeTestNum = " << this->CumulativeTestNum << std::endl;
               std::cout << "ObjectsPatched = " << this->ObjectsPatched << std::endl;
               std::cout << "CavitiesPatched = " << this->CavitiesPatched << std::endl;
            }
         
         protected:
            int StartingHandles;
            int TestObjectNum;
            int UncorrectedObjects;
            int CurrentHandles;
            int MaskHandlesBefore;
            int MaskCavitiesBefore;
            int RemainingObjectNum;
            int CurrentTestNum;
            int CumulativeTestNum;
            int ObjectsPatched;
            int CavitiesPatched;
            int DisconnectFlag;
      };
      
      // Find the nearest node that is not a crossover and has a compressed 
      void crossoverProjection(const BrainModelSurface* bms,
                    const MetricFile* crossoversMetric,
                    const int crossoverColumn,
                    const MetricFile* compressedMetric,
                    const int compressedMetricColumn,
                    MetricFile* outputMetric,
                    const int outputMetricColumn,
                    const float thresholdValue) throw (BrainModelAlgorithmException);
      
      // Generate a surface from the specified volume.
      void generateSurfaceAndMeasurements(const VolumeFile* vf) throw (BrainModelAlgorithmException);

      // Correct errors in segmetation.
      int correctErrors() throw (BrainModelAlgorithmException);
                                                      
      // 
      void uncorrectedObject() throw (BrainModelAlgorithmException);

      // patch an object
      ErrorStats patchObjectBatch(VolumeFile& rodata, 
                                  VolumeFile& sbpdata, 
                                  const int CurrentHandles, 
                                  const int TestObjectNum) throw (BrainModelAlgorithmException);
                 
      // perform a cycle of patching
      void patchCycle(VolumeFile& ctodata, 
                      VolumeFile& errorsdata, 
                      VolumeFile& sbpdata, 
                      int ctoextent[6], 
                      int TestStats[3], 
                      ErrorStats es) throw (BrainModelAlgorithmException);
            
      // Patch an "endo" handle.
      void patchEndoHandle(VolumeFile& sbpdata, 
                           int ctoextent[6], 
                           int TestStats[3], 
                           ErrorStats es, 
                           const int endo_count) throw (BrainModelAlgorithmException);
                     
      // patch an "Exo" handle.
      void patchExoHandle(VolumeFile& sbpdata, 
                          int ctoextent[6], 
                          int TestStats[3], 
                          ErrorStats es, 
                          const int exoCount) throw (BrainModelAlgorithmException);
                     
      // Patch invaginations
      void patchInvagination(VolumeFile& sbpdata, 
                             int TestStats[3], 
                             ErrorStats es) throw (BrainModelAlgorithmException);

      // read an intermediate volume
      void readIntermediateVolume(VolumeFile* vf,
                      const QString& nameIn) throw (BrainModelAlgorithmException);
                               
      // read an intermediate volume
      void readIntermediateVolume(VolumeFile& vf,
                      const QString& nameIn) throw (BrainModelAlgorithmException);
      
      // write an intermediate volume
      void writeIntermediateVolume(VolumeFile* vf,
                       const QString& nameIn) throw (BrainModelAlgorithmException);
                               
      // write an intermediate volume
      void writeIntermediateVolume(VolumeFile& vf,
                       const QString& nameIn) throw (BrainModelAlgorithmException);
                               
      // convert metric surface data into a volume
      VolumeFile* convertMetricToVolume(const BrainModelSurface* bms,
                                        const MetricFile* mf,
                                        const int metricColumn,
                                        const float scaling,
                                        const float voxelBoxSize);
                            
      // the input segmentation volume
      VolumeFile* segmentationVolume;
      
      // the input radial position map volume
      VolumeFile* radialPositionMapVolume;
      
      // x dimensions of volume being segmented
      int xDim;

      // y dimensions of volume being segmented
      int yDim;

      // z dimensions of volume being segmented
      int zDim;

      // Anterior Commissure voxel indices
      int acIJK[3];

      // hemisphere
      int leftHemFlag;
      
      // intermediate files subdirectory
      QString intermediateFilesSubDirectory;
      
      // save (do not delete) the intermediate files
      bool saveIntermediateFiles;
      
      // intermediate files generated by this algorithm
      std::vector<QString> intermediateFiles;
      
      // the output volume
      VolumeFile* outputVolume;
      
      /// type of volume files to write
      VolumeFile::FILE_READ_WRITE_TYPE typeOfVolumeFilesToWrite;
};

#endif //  __BRAIN_MODEL_VOLUME_SUREFIT_ERROR_CORRECTION_H__

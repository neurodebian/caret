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



#ifndef __FOCI_FILE_TO_PALS_PROJECTOR_H__
#define __FOCI_FILE_TO_PALS_PROJECTOR_H__

#include <vector>

#include "BrainModelAlgorithm.h"
#include "Structure.h"

class CellProjection;
class FociProjectionFile;
class BrainModelSurfacePointProjector;
class BrainSet;
class MapFmriAtlasSpecFileInfo;
class StudyMetaDataFile;

/// This class is used to project a FocusFile to a BrainModelSurface and store the 
/// results in a cell projection file.
class FociFileToPalsProjector : public BrainModelAlgorithm {
   public:
      /// Constructor
      FociFileToPalsProjector(BrainSet* brainSetIn,
                              FociProjectionFile* fociProjectionFileIn,
                              const StudyMetaDataFile* studyMetaDataFileIn,
                              const int firstFocusIndexIn,
                              const int lastFocusIndexIn,
                              const float projectOntoSurfaceAboveDistanceIn,
                              const bool projectOntoSurfaceFlagIn,
                              const bool projectToCerebellumFlagIn,
                              const float cerebralCutoffIn,
                              const float cerebellumCutoffIn);
                          
      /// Destructor
      ~FociFileToPalsProjector();
      
      /// project the foci
      void execute() throw (BrainModelAlgorithmException);
        
      /// default cutoff for distance to cerebral cortex
      static float getDefaultCerebralCutoffDistance() { return 2.0; }
      
      /// default cutoff for distance to cerebellum
      static float getDefaultCerebellumCutoffDistance() { return 4.0; }
      
      /// set the index of the first focus to project
      void setFirstFocusIndex(const int firstFocusIndexIn,
                              const int lastFocusIndexIn);
      
   protected:
      //
      // class for storing the point projectors
      //
      class PointProjector {
         public:
            /// constructor
            PointProjector(const QString& spaceNameIn,
                           const Structure::STRUCTURE_TYPE structureTypeIn,
                           BrainModelSurfacePointProjector* pointProjectorIn,
                           BrainSet* bsIn,
                           BrainModelSurface* bmsIn);
            
            /// destructor 
            ~PointProjector();
            
            /// get the space name
            QString getSpaceName() const { return spaceName; }
            
            /// get the structure
            Structure::STRUCTURE_TYPE getStructureType() const { return structureType; }

            /// equality operator
            bool operator==(const PointProjector& pp) const;
            
            /// brain set
            BrainSet* bs;
            
            /// surface used for projection
            BrainModelSurface* bms;
            
            /// original name of space
            QString originalSpaceName;
            
            /// name of space
            QString spaceName;
            
            /// structure
            Structure::STRUCTURE_TYPE structureType;
            
            /// the point projector
            BrainModelSurfacePointProjector* pointProjector;
            
      };
      
      // project a single cell 
      void projectFocus(CellProjection& cp,
                        BrainModelSurface* bms,
                        BrainModelSurfacePointProjector* pointProjector,
                        const BrainModelSurface* searchSurface);

      // convert space names to identical spaces
      static void spaceNameConvert(QString& spaceName);
      
      // load the needed point projectors
      void loadNeededPointProjectors(const std::vector<PointProjector>& projectorsNeeded) throw (BrainModelAlgorithmException);

      // find the surface for the specified space and structure
      BrainModelSurface* findSearchSurface(const QString& spaceName,
                                           const Structure::STRUCTURE_TYPE structure);
                                           
      // get distance of focus from a surface
      float getDistanceToSurface(const CellProjection* cp,
                                 const PointProjector* pp) const;
                                 
      // get point projector for space and structure
      PointProjector* getPointProjector(const QString& spaceName,
                                        const Structure::STRUCTURE_TYPE structure);
                                              
      // spaces currently loaded
      std::vector<PointProjector*> pointProjectors;
      
      // the file that is to be projected
      FociProjectionFile* fociProjectionFile;
      
      // the study metadata file
      const StudyMetaDataFile* studyMetaDataFile;
      
      // the first focus that is to be projected
      int firstFocusIndex;
      
      // the last focus that is to be projected (if -1 do all)
      int lastFocusIndex;
      
      // project onto surface distance
      const float projectOntoSurfaceAboveDistance;
      
      // project onto surface flag
      const bool projectOntoSurfaceFlag;
      
      /// project to cerebellum flag
      bool projectToCerebellumFlag;
                                    
      /// cerebral cortext cutoff
      float cerebralCutoff;
      
      /// cerebellum cutoff
      float cerebellumCutoff;
      
      /// atlases available for mapping foci
      std::vector<MapFmriAtlasSpecFileInfo> availableAtlases;

      /// atlases need to be loaded flag
      bool atlasesDirectoryLoadedFlag;

};
/*
bool 
operator==(const FociFileToPalsProjector::PointProjector& pp1,
           const FociFileToPalsProjector::PointProjector& pp2)
{
   const bool val = ((pp1.getSpaceName() == pp2.getSpaceName()) &&
                     (pp1.getStructureType() == pp2.getStructureType()));
   return val;
}
*/
#endif // __FOCI_FILE_TO_PALS_PROJECTOR_H__

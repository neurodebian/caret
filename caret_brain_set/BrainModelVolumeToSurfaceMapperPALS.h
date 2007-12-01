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
#include "BrainModelVolumeToSurfaceMapperAlgorithmParameters.h"
#include "Structure.h"

class GiftiNodeDataFile;
class VolumeFile;

/// class for mapping a volume to the PALS atlas
class BrainModelVolumeToSurfaceMapperPALS : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelVolumeToSurfaceMapperPALS(BrainSet* bsIn,
                     VolumeFile* volumeFileIn,
                     const QString& stereotaxicSpaceNameIn,
                     const Structure& structureIn,
                     const BrainModelVolumeToSurfaceMapperAlgorithmParameters& mappingParameters,
                     GiftiNodeDataFile* dataFileIn);
      
      // destructor
      ~BrainModelVolumeToSurfaceMapperPALS();
      
      // get the names of the supported stereotaxic spaces
      static void getSupportedStereotaxicSpaceName(std::vector<QString>& namesOut);
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
   protected:
      // map to average fiducial surface
      void mapAverageFiducial(const QString& topologyFileName,
                              const QString& avgFidCoordFileName,
                              const QString& structureName,
                              GiftiNodeDataFile* nodeDataFile) throw (BrainModelAlgorithmException);
                              
      // map all indiv cases
      void mapIndividualCases(const QString& topologyFileName,
                              const std::vector<QString>& indivCoordFileNames,
                              const QString& structureAbbreviation,
                              GiftiNodeDataFile* nodeDataFile) throw (BrainModelAlgorithmException);
                              
      /// the volume that is to be mapped
      VolumeFile* volumeFile;
      
      /// the structure that is being mapped
      Structure structure;
      
      /// the stereotaxic space that is being mapped
      QString stereotaxicSpaceName;
      
      /// the mapping algorithm parameters
      BrainModelVolumeToSurfaceMapperAlgorithmParameters mappingParameters;
      
      /// the output data file
      GiftiNodeDataFile* dataFile;
};


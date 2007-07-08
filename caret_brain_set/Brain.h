
#ifndef __BRAIN_H__
#define __BRAIN_H__

#include <string>
#include <vector>

#include "SceneFile.h"

class AreaColorFile;
class BorderColorFile;
class BrainStructure;
class CellColorFile;
class CellFile;
class CocomacConnectivityFile;
class ContourCellColorFile;
class DisplaySettingsArealEstimation;
class DisplaySettingsBorders;
class DisplaySettingsCells;
class DisplaySettingsContours;
class DisplaySettingsCuts;
class DisplaySettingsCoCoMac;
class DisplaySettingsDeformationField;
class DisplaySettingsFoci;
class DisplaySettingsGeodesicDistance;
class DisplaySettingsImages;
class DisplaySettingsMetric;
class DisplaySettingsModels;
class DisplaySettingsSurface;
class DisplaySettingsPaint;
class DisplaySettingsProbabilisticAtlas;
class DisplaySettingsScene;
class DisplaySettingsRgbPaint;
class DisplaySettingsSurfaceShape;
class DisplaySettingsSurfaceVectors;
class DisplaySettingsTopography;
class DisplaySettingsVolume;
class DisplaySettingsWustlRegion;
class FociColorFile;
class FociFile;
class PaletteFile;
class ParamsFile;
class TransformationMatrixFile;
class WustlRegionFile;

/// this class holds items independent of all BrainStructure's
class Brain {
   public:
      // constructor
      Brain();
      
      // destructor
      ~Brain();
      
      // add a brain structure
      void addBrainStructure(BrainStructure* bs, const bool deleteBrainStructureFlag = false);
      
      // reset the files
      void resetDataFiles(const bool keepSceneData);
      
      // Reset the display settings.
      void resetDisplaySettings(const bool keepSceneData);

      // Setup scene for display settings
      void displaySettingsShowScene(const SceneFile::Scene* ss,
                                    std::string& errorMessage);
      
      // save scene for display settings
      void displaySettingsSaveScene(SceneFile::Scene& scene,
                                    const bool onlyIfSelectedFlag);
      
      // Update all display settings.
      void updateAllDisplaySettings();

   protected:
      /// the brain structures associated with this brain
      std::vector<BrainStructure*> brainStructures;
      
      /// if true the corresponding brain structure should be deleted in "this" destructor
      std::vector<bool> brainStructureDeleteFlag;
      
      /// area color file
      AreaColorFile* areaColorFile;
      
      /// Border color file
      BorderColorFile* borderColorFile;
      
      /// Cell color file
      CellColorFile* cellColorFile;
      
      /// cocomac file
      CocomacConnectivityFile* cocomacFile;
      
      /// contour cell color file
      ContourCellColorFile* contourCellColorFile;
      
      /// foci color file
      FociColorFile* fociColorFile;
      
      /// Palette File
      PaletteFile* paletteFile;
      
      /// Params File
      ParamsFile* paramsFile;
      
      /// scene file
      SceneFile* sceneFile;
      
      /// transformation matrix file
      TransformationMatrixFile* transformationMatrixFile;
      
      /// volume cell file
      CellFile* volumeCellFile;
      
      /// volume foci file
      FociFile* volumeFociFile;
      
      /// wustl region file
      WustlRegionFile* wustlRegionFile;
      
      /// Areal Estimation display settings
      DisplaySettingsArealEstimation* displaySettingsArealEstimation;
      
      /// Border display settings
      DisplaySettingsBorders* displaySettingsBorders;
      
      /// Cell display settings
      DisplaySettingsCells* displaySettingsCells;
      
      /// CoCoMac display settings
      DisplaySettingsCoCoMac* displaySettingsCoCoMac;
      
      /// Contour display settings
      DisplaySettingsContours* displaySettingsContours;
      
      /// Cuts display settings
      DisplaySettingsCuts* displaySettingsCuts;
      
      /// Foci display settings
      DisplaySettingsFoci* displaySettingsFoci;
      
      /// node display settings
      DisplaySettingsSurface* displaySettingsSurface;
      
      /// deformation field display settings
      DisplaySettingsDeformationField* displaySettingsDeformationField;
      
      /// images display settings
      DisplaySettingsImages* displaySettingsImages;
      
      /// Metric display settings
      DisplaySettingsMetric* displaySettingsMetric;
      
      /// Models display settings
      DisplaySettingsModels* displaySettingsModels;
      
      /// Paint settings
      DisplaySettingsPaint* displaySettingsPaint;
      
      /// Probabilistic Atlas settings for surface
      DisplaySettingsProbabilisticAtlas* displaySettingsProbabilisticAtlasSurface;
      
      /// RGB Paint display settings
      DisplaySettingsRgbPaint* displaySettingsRgbPaint;
      
      /// Scene display settings
      DisplaySettingsScene* displaySettingsScene;
      
      /// Surface shape display settings
      DisplaySettingsSurfaceShape* displaySettingsSurfaceShape;
      
      /// Surface vector display settings
      DisplaySettingsSurfaceVectors* displaySettingsSurfaceVectors;
      
      /// Topography display settings
      DisplaySettingsTopography* displaySettingsTopography;
      
      /// Volume display settings
      DisplaySettingsVolume* displaySettingsVolume;
      
      /// Wustl Region Display Settings
      DisplaySettingsWustlRegion* displaySettingsWustlRegion;
      
      /// Geodesic distance file Display Settings
      DisplaySettingsGeodesicDistance* displaySettingsGeodesicDistance;
      
      /// Probabilistic Atlas settings for volume
      DisplaySettingsProbabilisticAtlas* displaySettingsProbabilisticAtlasVolume;
      
   friend class BrainStructure;
};

#endif // __BRAIN_H__


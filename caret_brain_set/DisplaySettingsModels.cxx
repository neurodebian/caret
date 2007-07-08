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

#include "BrainSet.h"
#include "DisplaySettingsModels.h"
#include "FileUtilities.h"
#include "TransformationMatrixFile.h"
#include "VtkModelFile.h"

/**
 * Constructor.
 */
DisplaySettingsModels::DisplaySettingsModels(BrainSet* bsIn)
   : DisplaySettings(bsIn)
{
   opacity = 1.0;
   lineWidth  = 2.0;
   vertexSize = 2.0;
   lightLines = false;
   lightVertices = true;
   lightPolygons = true;
}

/**
 * Destructor.
 */
DisplaySettingsModels::~DisplaySettingsModels()
{
}

/**
 * reinitialize all display settings.
 */
void 
DisplaySettingsModels::reset()
{
}

/**
 * update any selections due to changes with loaded data files.
 */
void 
DisplaySettingsModels::update()
{
}

/**
 * apply a scene (set display settings).
 */
void 
DisplaySettingsModels::showScene(const SceneFile::Scene& scene, QString& errorMessage) 
{
   TransformationMatrixFile* tmf = brainSet->getTransformationMatrixFile();

   const int numClasses = scene.getNumberOfSceneClasses();
   for (int nc = 0; nc < numClasses; nc++) {
      const SceneFile::SceneClass* sc = scene.getSceneClass(nc);
      if (sc->getName() == "DisplaySettingsModels") {
         const int num = sc->getNumberOfSceneInfo();
         for (int i = 0; i < num; i++) {
            const SceneFile::SceneInfo* si = sc->getSceneInfo(i);
            const QString infoName = si->getName();      
            
            if ((infoName == "model-display-status") ||
                (infoName == "model-xform")) {
               const int num = brainSet->getNumberOfVtkModelFiles();
               const QString name = si->getModelName();
               bool modelFound = false;
               for (int i = 0; i < num; i++) {
                  VtkModelFile* vmf = brainSet->getVtkModelFile(i);
                  if (name == FileUtilities::basename(vmf->getFileName())) {
                     if (infoName == "model-display-status") {
                        vmf->setDisplayFlag(si->getValueAsBool());
                     }
                     else if (name == "model-xform") {
                        TransformationMatrix* tm = tmf->getTransformationMatrixWithName(si->getValueAsString());
                        if (tm != NULL) {
                           vmf->setAssociatedTransformationMatrix(tm);
                        }
                        else {
                           QString msg("Unable to find transformation matrix \"");
                           msg.append(si->getValueAsString());
                           msg.append("\" for use by VTK model \"");
                           msg.append(name);
                           msg.append("\"\n");
                           errorMessage.append(msg);
                        }
                     }
                     modelFound = true;
                  }
               }
               if (modelFound == false) {
                  QString msg("Vtk Model \"");
                  msg.append(name);
                  msg.append("\" not found.\n");
                  errorMessage.append(msg);
               }
            }
            else if (infoName == "model-opacity") {
               si->getValue(opacity);
            }
            else if (infoName == "model-lineWidth") {
               si->getValue(lineWidth);
            }
            else if (infoName == "model-vertexSize") {
               si->getValue(vertexSize);
            }
            else if (infoName == "model-lightVertices") {
               si->getValue(lightVertices);
            }
            else if (infoName == "model-lightLines") {
               si->getValue(lightLines);
            }
            else if (infoName == "model-lightPolygons") {
               si->getValue(lightPolygons);
            }
         }
      }
   }
}

/**
 * create a scene (read display settings).
 */
void 
DisplaySettingsModels::saveScene(SceneFile::Scene& scene, const bool onlyIfSelected)
{
   const int num = brainSet->getNumberOfVtkModelFiles();

   if (onlyIfSelected) {
      bool haveModelsOn = false;
      for (int i = 0; i < num; i++) {
         const VtkModelFile* vmf = brainSet->getVtkModelFile(i);
         if (vmf->getDisplayFlag()) {
            haveModelsOn = true;
         }
      }
      if (haveModelsOn == false) {
         return;
      }
   }

   TransformationMatrixFile* tmf = brainSet->getTransformationMatrixFile();
   
   SceneFile::SceneClass sc("DisplaySettingsModels");
   
   for (int i = 0; i < num; i++) {
      const VtkModelFile* vmf = brainSet->getVtkModelFile(i);
      sc.addSceneInfo(SceneFile::SceneInfo("model-display-status",
                                           FileUtilities::basename(vmf->getFileName()),
                                           vmf->getDisplayFlag()));
      const TransformationMatrix* tm = vmf->getAssociatedTransformationMatrix();
      if (tmf->getMatrixIndex(tm) >= 0) {
         sc.addSceneInfo(SceneFile::SceneInfo("model-xform",
                                              FileUtilities::basename(vmf->getFileName()),
                                              tm->getMatrixName()));
      }
   }
   sc.addSceneInfo(SceneFile::SceneInfo("model-opacity",
                                        opacity));
   sc.addSceneInfo(SceneFile::SceneInfo("model-lineWidth",
                                        lineWidth));
   sc.addSceneInfo(SceneFile::SceneInfo("model-vertexSize",
                                        vertexSize));
   sc.addSceneInfo(SceneFile::SceneInfo("model-lightVertices",
                                        lightVertices));
   sc.addSceneInfo(SceneFile::SceneInfo("model-lightLines",
                                        lightLines));
   sc.addSceneInfo(SceneFile::SceneInfo("model-lightPolygons",
                                        lightPolygons));
   
   scene.addSceneClass(sc);
}
                       

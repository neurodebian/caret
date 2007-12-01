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


#ifndef __SCENE_FILE_H__
#define __SCENE_FILE_H__

class QStringList;

#include "AbstractFile.h"
#include "FileException.h"

/// File for storing, reading, and writing a Caret coordinate file
class SceneFile : public AbstractFile {
   public:
      /// scene information
      class SceneInfo {
         public:
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                      const QString& modelNameIn,
                      const QString& valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                      const QString& modelNameIn,
                      const float valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                      const QString& modelNameIn,
                      const int valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                      const QString& modelNameIn,
                      const bool valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                      const float valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                      const int valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                      const bool valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                      const QString& valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                               const QStringList& valueIn);
                      
            /// Constructor
            explicit SceneInfo(const QString& nameIn,
                               const char* valueIn);
                      
            /// Destructor
            ~SceneInfo();
            
            /// get the name
            QString getName() const { return name; }
            
            /// get the model name
            QString getModelName() const { return modelName; }
            
            /// get the value as a string
            void getValue(QString& val) const { val = getValueAsString(); }
            
            /// get the value as a float
            void getValue(float& val) const { val = getValueAsFloat(); }
            
            /// get the value as an int
            void getValue(int& val) const { val = getValueAsInt(); }
            
            /// get the value as a bool
            void getValue(bool& val) const { val = getValueAsBool(); }
            
            /// get the value as a string
            QString getValueAsString() const;
            
            /// get the value as a float
            float getValueAsFloat() const;
            
            /// get the value as an int
            int getValueAsInt() const;
            
            /// get the value as a bool
            bool getValueAsBool() const;
            
            /// get default surfaces special name
            static QString getDefaultSurfacesName() { return "___DEFAULT___"; }
                        
            /// set the name
            void setName(const QString& n) { name = n; }
            
            /// set the model name
            void setModelName(const QString& n) { modelName = n; }
            
            /// set the value
            void setValue(const QString& v) { value = v; }
            
         protected:
            /// the name
            QString name;
            
            /// the model name
            QString modelName;
            
            /// the value
            QString value;
         
      };
      
      /// scene class
      class SceneClass {
         public:
            /// Constructor
            SceneClass(const QString& nameIn);
            
            /// Destructor
            ~SceneClass();
            
            /// get the name of the scene class
            QString getName() const { return name; }
            
            /// set the name
            void setName(const QString& n) { name = n; }
            
            /// add a scene info
            void addSceneInfo(const SceneInfo& si);
            
            /// clear the scene class
            void clear();
            
            /// get the number of scene info's
            int getNumberOfSceneInfo() const { return info.size(); }
            
            /// get a scene info (const method)
            const SceneInfo* getSceneInfo(const int indx) const;
            
            /// get a scene info
            SceneInfo* getSceneInfo(const int indx);

         protected:
            /// name of scene class
            QString name;
            
            /// the scene info
            std::vector<SceneInfo> info;
      };
      
      /// scene 
      class Scene {
         public:
            /// Constructor
            Scene(const QString& nameIn);
            
            /// Destructor
            ~Scene();
            
            /// get the name of the scene
            QString getName() const { return name; }
            
            /// set the name of a scene
            void setName(const QString& s) { name = s; }
            
            /// add a scene data for a class
            void addSceneClass(const SceneClass& sc);
            
            /// clear the scene info
            void clear();
            
            /// get the number of scene classes
            int getNumberOfSceneClasses() const { return classes.size(); }
            
            /// get a scene class (const method)
            const SceneClass* getSceneClass(const int indx) const;
            
            /// get a scene class
            SceneClass* getSceneClass(const int indx);

            /// get a class with the specified name
            SceneClass* getSceneClassWithName(const QString& name);
            
            /// get a class with the specified name (const method)
            const SceneClass* getSceneClassWithName(const QString& name) const;
            
            /// remove a class with the specified name
            void removeClassWithName(const QString& name);
      
         protected:
            /// name of scene
            QString name;
            
            /// the scene classes
            std::vector<SceneClass> classes;
      };
      
      /// constructor
      SceneFile();
      
      /// copy constructor
      SceneFile(const SceneFile& cf);
      
      /// destructor
      ~SceneFile();
      
      /// add a scene
      void addScene(const Scene& ss);

      /// insert a scene
      void insertScene(const int insertAfterIndex, const Scene& ss);
      
      /// replace a scene
      void replaceScene(const int sceneIndex, 
                        const Scene& si) throw (FileException);
            
      /// delete a scene
      void deleteScene(const int indx);
      
      /// get the number of scenes
      int getNumberOfScenes() const { return scenes.size(); }
      
      /// get a scene
      Scene* getScene(const int indx);
      
      /// get a scene 
      const Scene* getScene(const int indx) const;
      
      /// get a scene from the scene name (NULL if not found)
      Scene* getSceneFromName(const QString& sceneName);
      
      /// get a scene from the scene name (NULL if not found)
      const Scene* getSceneFromName(const QString& sceneName) const;
      
      /// append a scene file to this one
      void append(SceneFile& sf) throw (FileException);
      
      /// clear the coordinate file
      void clear();
      
      /// returns true if the file is isEmpty (contains no data)
      bool empty() const { return scenes.empty(); }
   
      /// remove paths from all of the scene spec file's data file names
      void removePathsFromAllSpecFileDataFileNames();
      
      /// add paths to all of the scene spec file's data file names
      void addPathToAllSpecFileDataFileNames(const QString& path);
      
      /// remove paths from all of the scene' spec file data names for a file on disk
      static void removePathsFromAllSpecFileDataFileNames(const QString& sceneFileName)
                                                        throw (FileException);
      
      /// add paths to all of the scene spec file's data file names for a file on disk
      static void addPathToAllSpecFileDataFileNames(const QString& sceneFileName,
                                                    const QString& path)
                                                        throw (FileException);
      
   protected:
      /// read coordinate file data
      void readFileData(QFile& file, QTextStream& stream, QDataStream& binStream,
                                  QDomElement& /* rootElement */) throw (FileException);
      
      /// write coordinate file data
      void writeFileData(QTextStream& stream, QDataStream& binStream,
                                 QDomDocument& /* xmlDoc */,
                                  QDomElement& /* rootElement */) throw (FileException);      
      //
      // NOTE: If any variables are added be sure to update the copy constructor.
      //
      /// the scenes
      std::vector<Scene> scenes;
      
      /// scene tag name
      static const QString sceneTagName;
      
      /// scene name tag name
      static const QString sceneNameTagName;
      
      /// scene class tag name
      static const QString sceneClassTagName;
      
      /// scene name attribute name
      static const QString sceneNameAttributeName;
      
      /// name for model attribute
      static const QString modelAttributeName;
      
      /// name for value attribute name
      static const QString valueAttributeName;
};

#ifdef __SCENE_FILE_MAIN__
const QString SceneFile::sceneTagName = "Scene";
const QString SceneFile::sceneNameTagName = "SceneName";
const QString SceneFile::sceneClassTagName = "Class";
const QString SceneFile::sceneNameAttributeName = "name";
const QString SceneFile::modelAttributeName = "model";
const QString SceneFile::valueAttributeName   = "value";
#endif // __SCENE_FILE_MAIN__

#endif // SCENE_FILE


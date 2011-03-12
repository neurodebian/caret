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

#include <QGlobalStatic>
#ifdef Q_OS_WIN32
#define NOMINMAX
#endif

#include <iostream>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomText>

#include "DebugControl.h"
#include "FileUtilities.h"
#define __SCENE_FILE_MAIN__
#include "SceneFile.h"
#undef __SCENE_FILE_MAIN__
#include "SpecFile.h"
#include "StringUtilities.h"

/** 
 * The constructor.
 */
SceneFile::SceneFile()
   : AbstractFile("Scene File", 
                  SpecFile::getSceneFileExtension(),
                  true, 
                  FILE_FORMAT_XML, 
                  FILE_IO_NONE, 
                  FILE_IO_NONE, 
                  FILE_IO_READ_AND_WRITE,
                  FILE_IO_NONE)
{
   clear();
}

/** 
 * The copy constructor.
 */
SceneFile::SceneFile(const SceneFile& cf)
   : AbstractFile(cf)
{
}

/** 
 * The destructor.
 */
SceneFile::~SceneFile()
{
   clear();
}

/**
 * append a scene file to this one.
 */
void 
SceneFile::append(SceneFile& sf) throw (FileException)
{
   scenes.insert(scenes.end(), sf.scenes.begin(), sf.scenes.end());
}
      
/**
 * Clear the coordinate file.
 */
void
SceneFile::clear()
{
   clearAbstractFile();
   scenes.clear();
}

/**
 * add a scene.
 */
void 
SceneFile::addScene(const Scene& s)
{
   scenes.push_back(s);
   setModified();
}

/**
 * insert a scene.
 */
void 
SceneFile::insertScene(const int insertAfterIndex, const Scene& ss)
{
   if ((insertAfterIndex >= 0) && (insertAfterIndex < (getNumberOfScenes() - 1))) {
      scenes.insert(scenes.begin() + insertAfterIndex + 1, ss);
   }
   else {
      addScene(ss);
   }
}
      
/**
 * replace a scene.
 */
void 
SceneFile::replaceScene(const int sceneIndex,
                        const Scene& s)  throw (FileException)
{
   if ((sceneIndex >= 0) && (sceneIndex < getNumberOfScenes())) {
      scenes[sceneIndex] = s;
      setModified();
   }
}            

/**
 * delete a scene.
 */
void 
SceneFile::deleteScene(const int indx)
{
   if (indx < getNumberOfScenes()) {
      scenes.erase(scenes.begin() + indx);
   }
   setModified();
}
      
/**
 * get a scene set.
 */
SceneFile::Scene* 
SceneFile::getScene(const int indx)
{
   if (indx < getNumberOfScenes()) {
      return &scenes[indx];
   }
   return NULL;
}

/**
 * get a scene set.
 */
const SceneFile::Scene* 
SceneFile::getScene(const int indx) const
{
   if (indx < getNumberOfScenes()) {
      return &scenes[indx];
   }
   return NULL;
}

/**
 * get a scene index from the scene name (-1 if not found).
 */
int 
SceneFile::getSceneIndexFromName(const QString& sceneName) const
{
  for (int i = 0; i < getNumberOfScenes(); i++) {
     if (scenes[i].getName() == sceneName) {
        return i;
     }
  }
  return -1;
}      

/**
 * get a scene from the scene name (NULL if not found).
 */
SceneFile::Scene* 
SceneFile::getSceneFromName(const QString& sceneName)
{
  for (int i = 0; i < getNumberOfScenes(); i++) {
     if (scenes[i].getName() == sceneName) {
        return &scenes[i];
     }
  }
  return NULL;
}

/**
 * get a scene from the scene name (NULL if not found).
 */
const SceneFile::Scene* 
SceneFile::getSceneFromName(const QString& sceneName) const
{
  for (int i = 0; i < getNumberOfScenes(); i++) {
     if (scenes[i].getName() == sceneName) {
        return &scenes[i];
     }
  }
  return NULL;
}

/**
 * Read the scene file data.  May throw FileException.
 */
void
SceneFile::readFileData(QFile& /*file*/, QTextStream& /*stream*/, 
                             QDataStream& /*binStream*/,
                             QDomElement& rootElement) throw (FileException)
{
   //
   // Should reading data be skipped ?
   //
   if (getReadMetaDataOnlyFlag()) {
      return;
   }

   switch (getFileReadType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Reading in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Reading in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            QDomNode node = rootElement.firstChild();
            while (node.isNull() == false) {
               QDomElement elem = node.toElement();
               if (elem.isNull() == false) { 
                  if (DebugControl::getDebugOn()) {      
                     std::cout << "Tag Name: " << elem.tagName().toAscii().constData() << std::endl;
                  }
                  
                  //
                  // Is this a "Scene" element
                  //
                  if (elem.tagName() == sceneTagName) {
                     const QString sceneName = elem.attribute(sceneNameAttributeName, "");
                     
                     //
                     // Create a new scene
                     //
                     Scene scene(sceneName);
                     
                     //
                     // First child 
                     //
                     QDomNode classNode = elem.firstChild();
                     
                     //
                     // Loop the the classes
                     //
                     while (classNode.isNull() == false) {
                        QDomElement classElem = classNode.toElement();
                        
                        //
                        // Is this the scene name
                        //
                        if (classElem.tagName() == sceneNameTagName) {
                           QString s = getXmlElementFirstChildAsString(classElem);
                           if (s.isEmpty() == false) {
                              scene.setName(s);
                           }
                        }
                        //
                        // Is this a "Scene" element
                        //
                        else if (classElem.tagName() == sceneClassTagName) {
                           const QString sceneClassName = classElem.attribute(sceneNameAttributeName, "");
                           
                           //
                           // Make sure the scene has a name
                           //
                           if (sceneClassName.isEmpty() == false) {
                              SceneClass sceneClass(sceneClassName);
                              
                              //
                              // Look at the scene's children
                              //
                              QDomNode sceneClassNode = classElem.firstChild();
                              while (sceneClassNode.isNull() == false) {
                                 QDomElement sceneElem = sceneClassNode.toElement();
                                 const QString sceneDataType = sceneElem.tagName();
                                 if (DebugControl::getDebugOn()) {      
                                    std::cout << "Scene Data Type: " << sceneDataType.toAscii().constData() << std::endl;
                                 }
                                 
                                 //
                                 // Get the associated model and the value
                                 //
                                 QString modelName;
                                 int overlayNumber = -1;
                                 QString valueName;
                                 QDomNodeList nodeList = sceneClassNode.childNodes();
                                 for (int j = 0; j < static_cast<int>(nodeList.count()); j++) {
                                    QDomNode childNode = nodeList.item(j);
                                    const QDomElement childElem = childNode.toElement();
                                    const QString val = getXmlElementFirstChildAsString(childElem);
                                    if (DebugControl::getDebugOn()) {      
                                       std::cout << "Child: " << childElem.tagName().toAscii().constData() 
                                                 << " " << val.toAscii().constData() << std::endl;
                                    }
                                    if (childElem.tagName() == modelAttributeName) {
                                       modelName = val;
                                    }
                                    else if (childElem.tagName() == overlayAttributeName) {
                                       bool ok = false;
                                       overlayNumber = val.toInt(&ok);
                                       if (ok == false) {
                                          overlayNumber = -1;
                                       }
                                    }
                                    else if (childElem.tagName() == valueAttributeName) {
                                       valueName = val;
                                    }
                                 }
               
                                 //
                                 // Make sure scene data type and value are valid
                                 //
                                 if ((sceneDataType.isEmpty() == false) &&
                                     (valueName.isEmpty() == false)) {
                                    //
                                    // Create the scene info
                                    //
                                    SceneInfo si(sceneDataType, modelName, overlayNumber, valueName);
                                    sceneClass.addSceneInfo(si);
                                 }
                                 
                                 //
                                 // Next sibling
                                 //
                                 sceneClassNode = sceneClassNode.nextSibling();
                              } // while (sceneClassNode.isNull() ....
                              
                              //
                              // Add the scene set
                              //
                              if (sceneClass.getNumberOfSceneInfo() > 0) {
                                 scene.addSceneClass(sceneClass);
                              }
                           }
                           else {
                              std::cout << "ERROR: Scene name is isEmpty reading scene file." << std::endl;
                           } // else
                        } // if (classElem.tagName()...
                        
                        //
                        // Get next scene class
                        //
                        classNode = classNode.nextSibling();
                     } // while (classNode.isNull()...
                     
                     //
                     //
                     //
                     if (scene.getNumberOfSceneClasses() > 0) {
                        addScene(scene);
                     }
                  } // if (elem.tagName()...
               } // if (elem.isNull()...
               node = node.nextSibling();
            } /// while (node.isNull
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Reading XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Reading in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
   
   setModified();
}

/**
 * Read the scene file data.  May throw FileException.
 */
void
SceneFile::writeFileData(QTextStream& /*stream*/, QDataStream& /*binStream*/,
                              QDomDocument& xmlDoc,
                              QDomElement& rootElement) throw (FileException)
{
   switch (getFileWriteType()) {
      case FILE_FORMAT_ASCII:
         throw FileException(filename, "Writing in Ascii format not supported.");
         break;
      case FILE_FORMAT_BINARY:
         throw FileException(filename, "Writing in Binary format not supported.");
         break;
      case FILE_FORMAT_XML:
         {
            const int numScenes = getNumberOfScenes();
            for (int j = 0; j < numScenes; j++) {
               const Scene* scene = getScene(j);
               QString sceneName = scene->getName();
               if (sceneName.isEmpty()) {
                  sceneName = "NoName";
               }
               
               QString attSceneName = sceneName;
               const int len = static_cast<int>(attSceneName.length());
               for (int i = 0; i < len; i++) {
                  bool validCharFlag = false;
                  QCharRef c = attSceneName[i];
                  if (((c >= 'A') && (c <= 'Z')) ||
                      ((c >= 'a') && (c <= 'z'))) {
                     validCharFlag = true;
                  }
                  if (i > 0) {
                     if (((c >= '0') && (c <= '9')) ||
                          (c == '-') ||
                          (c == '_') ||
                          (c == ' ')) {
                        validCharFlag = true;
                     }
                  }
                  if (validCharFlag == false) {
                     if (i == 0) {
                        attSceneName[i] = 'A';
                     }
                     else {
                        attSceneName[i] = '_';
                     }
                  }
               }
               QDomElement sceneElement = xmlDoc.createElement(sceneTagName);
               sceneElement.setAttribute(sceneNameAttributeName, attSceneName);
               
               //
               // This scene name overrides the attribute contained in the scene tag
               //  
               QDomElement sceneNameElement = xmlDoc.createElement(sceneNameTagName);
               QDomCDATASection sceneNameValue = xmlDoc.createCDATASection(sceneName);
               sceneNameElement.appendChild(sceneNameValue);
               sceneElement.appendChild(sceneNameElement);
               
               const int numClasses = scene->getNumberOfSceneClasses();
               for (int i = 0; i < numClasses; i++) {
                  const SceneClass* sc = scene->getSceneClass(i);
                  const QString sceneClassName = sc->getName();
                  if (sceneClassName.isEmpty() == false) {
                     QDomElement sceneClassElement = xmlDoc.createElement(sceneClassTagName);
                     sceneClassElement.setAttribute(sceneNameAttributeName, sceneClassName);
                     
                     const int numSceneInfo = sc->getNumberOfSceneInfo();
                     for (int j = 0; j < numSceneInfo; j++) {
                        const SceneInfo* si = sc->getSceneInfo(j);
                        const QString sceneName = si->getName();
                        if (sceneName.isEmpty() == false) {
                           
                           QDomElement infoElement = xmlDoc.createElement(sceneName);
                           
                           QDomElement modelElem = xmlDoc.createElement(modelAttributeName);
                           QDomCDATASection modelValue = xmlDoc.createCDATASection(si->getModelName());
                           modelElem.appendChild(modelValue);
                           infoElement.appendChild(modelElem);
                           
                           QDomElement overlayElem = xmlDoc.createElement(overlayAttributeName);
                           QDomCDATASection overlayValue = xmlDoc.createCDATASection(QString::number(si->getOverlayNumber()));
                           overlayElem.appendChild(overlayValue);
                           infoElement.appendChild(overlayElem);
                           
                           QDomElement valueElem = xmlDoc.createElement(valueAttributeName);
                           QDomCDATASection valValue = xmlDoc.createCDATASection(si->getValueAsString());
                           valueElem.appendChild(valValue);
                           infoElement.appendChild(valueElem);
                           
                           sceneClassElement.appendChild(infoElement);
                        }
                        else {
                           std::cout << "ERROR: scene info has no name.  It has not been written." << std::endl;
                        }
                     }
                     sceneElement.appendChild(sceneClassElement);
                  }
                  else {
                     std::cout << "ERROR: scene class has no name.  It has not been written." << std::endl;
                  }
               }

               rootElement.appendChild(sceneElement);
            }
         }
         break;
      case FILE_FORMAT_XML_BASE64:
         throw FileException(filename, "XML Base64 not supported.");
         break;
      case FILE_FORMAT_XML_GZIP_BASE64:
         throw FileException(filename, "XML GZip Base64 not supported.");
         break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
      case FILE_FORMAT_OTHER:
         throw FileException(filename, "Writing in Other format not supported.");
         break;
      case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
         throw FileException(filename, "Comma Separated Value File Format not supported.");
         break;
   }
}

/**
 * remove paths from all of the scene spec file's data file names.
 */
void 
SceneFile::removePathsFromAllSpecFileDataFileNames()
{
   const int numScenes = getNumberOfScenes();
   for (int i = 0; i < numScenes; i++) {
      Scene* s = getScene(i);
      if (s->getSceneClassWithName("SpecFile") != NULL) {
         //
         // Get the spec file
         //
         SpecFile sf;
         QString msg;
         sf.showScene(*s, msg);
         
         //
         // Remove spec file from scene
         //
         s->removeClassWithName("SpecFile");
         
         //
         // Remove paths from spec file
         //
         sf.removePathsFromAllFiles();
         
         //
         // Add spec file back to scene
         //
         sf.saveScene(*s, false);
      }
   }
}

/**
 * transfer a scene class from one scene to other scene(s).
 */
void 
SceneFile::transferSceneClass(const int fromSceneNumber,
                              const std::vector<int>& toSceneNumbers,
                              const QString& nameOfClassToTransfer) throw (FileException)
{
   //
   // Get the from scene
   //
   const int numScenes = getNumberOfScenes();
   if ((fromSceneNumber < 0) ||
       (fromSceneNumber >= numScenes)) {
      throw FileException("\"From\" scene number is invalid.");
   }
   const Scene* fromScene = getScene(fromSceneNumber);
   if (fromScene == NULL) {
      throw FileException("\"From\" scene not found.");
   }
   
   //
   // Find the class to transer
   //
   const SceneClass* fromBMI = fromScene->getSceneClassWithName(nameOfClassToTransfer);
   if (fromBMI == NULL) {
      throw FileException("Scene class "
                          + nameOfClassToTransfer
                          + " not found in \"from\" scene.");
   }
   
   //
   // Transfer the class
   //
   const int numScenesToChange = static_cast<int>(toSceneNumbers.size());
   for (int i = 0; i < numScenesToChange; i++) {
      const int sceneNumber = toSceneNumbers[i];
      if ((sceneNumber >= 0) &&
          (sceneNumber < numScenes)) {
         Scene* toScene = getScene(sceneNumber);
         if (toScene != NULL) {
            SceneClass* toBMI = toScene->getSceneClassWithName(nameOfClassToTransfer);
            if (toBMI != NULL) {
               *toBMI = *fromBMI;
               setModified();
            }
         }
      }
   }
}                                                              

/**
 * add paths to all of the scene spec file's data file names.
 */
void 
SceneFile::addPathToAllSpecFileDataFileNames(const QString& path)
{
   const int numScenes = getNumberOfScenes();
   for (int i = 0; i < numScenes; i++) {
      Scene* s = getScene(i);
      if (s->getSceneClassWithName("SpecFile") != NULL) {
         //
         // Get the spec file
         //
         SpecFile sf;
         QString msg;
         sf.showScene(*s, msg);
         
         //
         // Remove spec file from scene
         //
         s->removeClassWithName("SpecFile");
         
         //
         // Remove paths from spec file
         //
         sf.prependPathsToAllFiles(path, true);
         
         //
         // Add spec file back to scene
         //
         sf.saveScene(*s, false);
      }
   }
}

/**
 * remove paths from all of the scene' spec file data names for a file on disk.
 */
void 
SceneFile::removePathsFromAllSpecFileDataFileNames(const QString& sceneFileName)
                                                        throw (FileException)
{
   SceneFile sf;
   sf.readFile(sceneFileName);
   sf.removePathsFromAllSpecFileDataFileNames();
   sf.writeFile(sceneFileName);
}

/**
 * add paths to all of the scene spec file's data file names for a file on disk.
 */
void 
SceneFile::addPathToAllSpecFileDataFileNames(const QString& sceneFileName,
                                             const QString& path)
                                                        throw (FileException)
{
   SceneFile sf;
   sf.readFile(sceneFileName);
   sf.addPathToAllSpecFileDataFileNames(path);
   sf.writeFile(sceneFileName);
}      

/*--------------------------------------------------------------------------------------*/

/**
 * Constructor.
 */
SceneFile::Scene::Scene(const QString& nameIn)
{
   clear();
   name = nameIn;
}

/**
 * Destructor.
 */
SceneFile::Scene::~Scene()
{
   clear();
}

/**
 * add a scene data for a class.
 */
void 
SceneFile::Scene::addSceneClass(const SceneClass& sc)
{
   classes.push_back(sc);
}

/**
 * clear the scene info.
 */
void 
SceneFile::Scene::clear()
{
   name = "";
   classes.clear();
}

/**
 * get a scene class (const method).
 */
const SceneFile::SceneClass* 
SceneFile::Scene::getSceneClass(const int indx) const
{
   if ((indx >= 0) && (indx < getNumberOfSceneClasses())) {
      return &classes[indx];
   }
   return NULL;
}

/**
 * get a scene class.
 */
SceneFile::SceneClass*
SceneFile::Scene::getSceneClass(const int indx)
{
   if ((indx >= 0) && (indx < getNumberOfSceneClasses())) {
      return &classes[indx];
   }
   return NULL;
}

/**
 * get a class with the specified name.
 */
SceneFile::SceneClass* 
SceneFile::Scene::getSceneClassWithName(const QString& name)
{
   for (int i = 0; i < getNumberOfSceneClasses(); i++) {
      SceneClass* sc = getSceneClass(i);
      if (sc->getName() == name) {
         return sc;
      }
   }
   return NULL;
}

/**
 * get a class with the specified name (const method).
 */
const SceneFile::SceneClass* 
SceneFile::Scene::getSceneClassWithName(const QString& name) const
{
   for (int i = 0; i < getNumberOfSceneClasses(); i++) {
      const SceneClass* sc = getSceneClass(i);
      if (sc->getName() == name) {
         return sc;
      }
   }
   return NULL;
}

/**
 * remove a class with the specified name.
 */
void 
SceneFile::Scene::removeClassWithName(const QString& name)
{
   for (int i = 0; i < getNumberOfSceneClasses(); i++) {
      SceneClass* sc = getSceneClass(i);
      if (sc->getName() == name) {
         classes.erase(classes.begin() + i);
         return ;
      }
   }
}

/*--------------------------------------------------------------------------------------*/

/**
 * Constructor.
 */
SceneFile::SceneClass::SceneClass(const QString& nameIn) 
{ 
   name = nameIn; 
}

/**
 * Destructor.
 */
SceneFile::SceneClass::~SceneClass()
{
}

/**
 * clear the scene class.
 */
void 
SceneFile::SceneClass::clear()
{
   name = "";
   info.clear();
}
            
/**
 * add a scene info.
 */
void 
SceneFile::SceneClass::addSceneInfo(const SceneInfo& si)
{
   info.push_back(si);
}

/**
 * get a scene info (const method).
 */
const SceneFile::SceneInfo* 
SceneFile::SceneClass::getSceneInfo(const int indx) const
{
   if (indx < getNumberOfSceneInfo()) {
      return &info[indx];
   }
   return NULL;
}

/**
 * get a scene info.
 */
SceneFile::SceneInfo* 
SceneFile::SceneClass::getSceneInfo(const int indx)
{
   if (indx < getNumberOfSceneInfo()) {
      return &info[indx];
   }
   return NULL;
}

/*--------------------------------------------------------------------------------------*/

/**
 * Constructor for Node Attributes with Multiple Overlays.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const QString& modelNameIn,
                                const int overlayNumberIn,
                                const QString& valueIn)
{
   initialize(nameIn, modelNameIn, overlayNumberIn, valueIn);
}

/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const QString& modelNameIn,
                                const QString& valueIn)
{
   initialize(nameIn, modelNameIn, -1, valueIn);
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const QString& modelNameIn,
                                const float valueIn)
{
   initialize(nameIn, modelNameIn, -1, QString::number(valueIn, 'f', 6));
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const QString& modelNameIn,
                                const int valueIn)
{
   initialize(nameIn, modelNameIn, -1, QString::number(valueIn));
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const QString& modelNameIn,
                                const bool valueIn)
{
   initialize(nameIn, modelNameIn, -1, StringUtilities::fromBool(valueIn));
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const QString& valueIn)
{
   initialize(nameIn, "", -1, valueIn);
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const QStringList& valueIn)
{
   initialize(nameIn, "", -1, valueIn.join(" "));
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const char* valueIn)
{
   initialize(nameIn, "", -1, valueIn);
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const float valueIn)
{
   initialize(nameIn, "", -1, QString::number(valueIn, 'f', 6));
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const int valueIn)
{
   initialize(nameIn, "", -1, QString::number(valueIn));
}
    
/**
 * Constructor.
 */
SceneFile::SceneInfo::SceneInfo(const QString& nameIn,
                                const bool valueIn)
{
   initialize(nameIn, "", -1, StringUtilities::fromBool(valueIn));
}
    
/**
 * Destructor.
 */
SceneFile::SceneInfo::~SceneInfo()
{
}
            
/**
 * initialize the scene info.
 */
void 
SceneFile::SceneInfo::initialize(const QString& nameIn,
                                 const QString& modelNameIn,
                                 const int overlayNumberIn,
                                 const QString& valueIn)
{
   name = nameIn;
   modelName = modelNameIn;
   overlayNumber = overlayNumberIn;
   value = valueIn;
}

/**
 * get the value as a string.
 */
QString 
SceneFile::SceneInfo::getValueAsString() const
{
   return value;
}

/**
 * get the value as a float.
 */
float 
SceneFile::SceneInfo::getValueAsFloat() const
{
   return StringUtilities::toFloat(value);
}

/**
 * get the value as an int.
 */
int 
SceneFile::SceneInfo::getValueAsInt() const
{
   return StringUtilities::toInt(value);
}

/** 
 * get the value as a bool
 */
bool 
SceneFile::SceneInfo::getValueAsBool() const
{
   return StringUtilities::toBool(value);
}


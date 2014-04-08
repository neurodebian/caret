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

#include <iostream>

#include <QDateTime>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QTextStream>
#include <DateAndTime.h>

#include "AbstractFile.h"
#include "ColorFile.h"
#include "DebugControl.h"
#include "GiftiLabelTable.h"
#include "NiftiCaretExtension.h"
#include "XmlGenericWriter.h"
#include "XmlGenericWriterAttributes.h"
#include "VolumeFile.h"

static const QString TAG_CARET_EXTENSION = "CaretExtension";
static const QString TAG_COMMENT = "Comment";
static const QString TAG_DATE  = "Date";
static const QString TAG_GUI_LABEL = "GuiLabel";
static const QString TAG_INDEX = "Index";
static const QString TAG_VOLUME_INFORMATION = "VolumeInformation";
static const QString TAG_VOLUME_TYPE = "VolumeType";

/**
 * Constructor.
 */
NiftiCaretExtension::NiftiCaretExtension(std::vector<VolumeFile*>& volumesIn,
                                         const ColorFile* labelColorsIn,
                                          const int writeForCaretVersionIn)
   : volumes(volumesIn),
     labelColors(labelColorsIn),
     writeForCaretVersion(writeForCaretVersionIn)
{

}

/**
 * Apply the extension to the volumes.
 */
void
NiftiCaretExtension::readAndApplyExtensionToVolumes(QString& extensionXML,
                                                    QString& fileCommentOut) throw (FileException)
{
   int numVolumes = static_cast<int>(this->volumes.size());

   //
   // Place the CARET extension into a DOM XML Parser
   //
   QDomDocument doc("nifti_caret_extension");
   QString errorMessage;
   int errorLine, errorColumn;
   if (doc.setContent(extensionXML, &errorMessage, &errorLine, &errorColumn) == false) {
      QString msg("Error parsing Caret extension at line ");
      msg += QString::number(errorLine);
      msg += ", column ";
      msg += QString::number(errorColumn);
      throw FileException(msg);
   }

   //
   // Get the root element
   //
   QDomElement rootElement = doc.documentElement();
   if (rootElement.isNull()) {
      throw FileException("Caret extension root element is NULL");
   }
   if (DebugControl::getDebugOn()) {
      std::cout << "Caret extension root element: " << rootElement.tagName().toAscii().constData() << std::endl;
   }
   if (rootElement.tagName() != TAG_CARET_EXTENSION) {
      throw FileException("Caret extension root element is not " +
                          TAG_CARET_EXTENSION);
   }

   //
   // Loop through the children of the root element
   //
   QDomNode node = rootElement.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      QString nodeName = elem.tagName();
      if (DebugControl::getDebugOn()) {
         std::cout << "CARET extension element: " << nodeName.toAscii().constData() << std::endl;
      }

      if (nodeName == TAG_COMMENT) {
         fileCommentOut = AbstractFile::getXmlElementFirstChildAsString(elem);
      }
      else if (nodeName == TAG_DATE) {

      }
      else if (nodeName == TAG_VOLUME_INFORMATION) {
         QString indexString = elem.attribute("Index");
         if (indexString.length() > 0) {
            int indx = indexString.toInt(NULL);
            //std::cout << "Index=" << indx << std::endl;

            if ((indx >= 0) && (indx < numVolumes)) {
               VolumeFile* vf = this->volumes[indx];
               
               QDomNode volInfoChild = node.firstChild();
               while (volInfoChild.isNull() == false) {
                  QDomElement volInfoChildElement = volInfoChild.toElement();
                  QString volInfoChildName = volInfoChildElement.tagName();
                  //std::cout << "Child of "
                  //          << TAG_VOLUME_INFORMATION.toAscii().constData()
                  //          << " is "
                  //          << volInfoChildName.toAscii().constData()
                  //          << std::endl;

                  if (volInfoChildName == TAG_COMMENT) {
                     vf->setFileComment(AbstractFile::getXmlElementFirstChildAsString(volInfoChildElement));
                  }
                  else if (volInfoChildName == TAG_GUI_LABEL) {
                     vf->setDescriptiveLabel(AbstractFile::getXmlElementFirstChildAsString(volInfoChildElement));

                  }
                  else if (volInfoChildName == "LabelTable") {
                     QDomNode labelChild = volInfoChild.firstChild();
                     while (labelChild.isNull() == false) {
                        QDomElement labelElement = labelChild.toElement();
                        QString labelIndexString = labelElement.attribute("Index");
                        QString labelName = AbstractFile::getXmlElementFirstChildAsString(labelElement);
                        if (labelIndexString.isEmpty() == false) {
                           int labelIndex = labelIndexString.toInt(NULL);
                           //std::cout << "Label "
                           //          << labelIndex
                           //          << ": "
                           //          << labelName.toAscii().constData()
                           //          << std::endl;

                           int numRegionNames = vf->getNumberOfRegionNames();
                           while (numRegionNames < labelIndex) {
                              vf->addRegionName("");
                           }
                           vf->addRegionName(labelName);
                        }
                        labelChild = labelChild.nextSibling();
                     }
                  }
                  else if (volInfoChildName == "StudyMetaDataLinkSet") {
                     StudyMetaDataLinkSet smdls;
                     smdls.readXML(volInfoChild);
                     vf->setStudyMetaDataLinkSet(smdls);
                  }
                  else if (volInfoChildName == TAG_VOLUME_TYPE) {
                     QString typeName = AbstractFile::getXmlElementFirstChildAsString(volInfoChildElement);
                     if (typeName == "Anatomy") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_ANATOMY);
                     }
                     else if (typeName == "Functional") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_FUNCTIONAL);
                     }
                     else if (typeName == "Label") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_PAINT);
                     }
                     else if (typeName == "Paint") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_PAINT);
                     }
                     else if (typeName == "Probabilistic Atlas") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_PROB_ATLAS);
                     }
                     else if (typeName == "RGB") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_RGB);
                     }
                     else if (typeName == "ROI") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_ROI);
                     }
                     else if (typeName == "Segmentation") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_SEGMENTATION);
                     }
                     else if (typeName == "Vector") {
                        vf->setVolumeType(VolumeFile::VOLUME_TYPE_VECTOR);
                     }
                     else {
                        std::cout << "Unrecognized volume type "
                                  << typeName.toAscii().constData()
                                  << " in Caret Extension"
                                  << std::endl;
                     }

                  }
                  volInfoChild = volInfoChild.nextSibling();
               }
            }
            //else {
            //   throw FileException("Invalid volume index="
            //                       + QString::number(indx)
            //                       + " in Caret Extension.");
            //}
         }
         else throw FileException("Missing attribute Index in node"
                                  + TAG_VOLUME_INFORMATION);

      }
      else {
         std::cout << "Unrecognized child of "
                   << TAG_CARET_EXTENSION.toAscii().constData()
                   << " is "
                   << nodeName.toAscii().constData()
                   << std::endl;
      }

      //
      // Move to next node
      //
      node = node.nextSibling();
   }
}

/**
 * Get the extension in a string.
 */
QString
NiftiCaretExtension::getExtensionAsString()
{
   int numVolumes = this->volumes.size();

   //
   // Is this a paint or prob atlas volume
   //
   QString labelTableString;
   if (numVolumes > 0) {
      VolumeFile* firstVolume = this->volumes[0];
      if ((firstVolume->getVolumeType() == VolumeFile::VOLUME_TYPE_PAINT) ||
          (firstVolume->getVolumeType() == VolumeFile::VOLUME_TYPE_PROB_ATLAS)) {
         std::vector<QString> regNames;
         for (int i = 0; i < firstVolume->getNumberOfRegionNames(); i++) {
            //
            // Cannot have '~' character in the name
            //
            QString name(firstVolume->getRegionNameFromIndex(i));
            name = name.replace(QChar('~'), QChar('_'));
            regNames.push_back(name);
         }

         //
         // Label table
         //
         GiftiLabelTable labelTable;
         int numNames = static_cast<int>(regNames.size());
         for (int i = 0; i < numNames; i++) {
            QString name = regNames[i];
            labelTable.addLabel(name);
         }
         if (labelColors != NULL) {
            labelTable.assignColors(*labelColors);
         }
         QTextStream labelStream(&labelTableString, QIODevice::WriteOnly);
         labelTable.writeAsXML(labelStream, 0);
      }
   }

   QString dataString;
   QTextStream xmlDataStream(&dataString, QIODevice::WriteOnly);
   
   XmlGenericWriter xmlWriter(xmlDataStream);

   xmlWriter.writeStartDocument();
   
   xmlWriter.writeStartElement(TAG_CARET_EXTENSION);
   
   xmlWriter.writeElementCData(TAG_DATE, QDateTime::currentDateTime().toString(Qt::ISODate));
   
   for (int i = 0; i < numVolumes; i++) {
      const VolumeFile* volume = volumes[i];
      XmlGenericWriterAttributes volumeAttributes;
      volumeAttributes.addAttribute(TAG_INDEX, i);
      xmlWriter.writeStartElement(TAG_VOLUME_INFORMATION, volumeAttributes);
      
      xmlWriter.writeElementCData(TAG_COMMENT, volume->getFileComment());
      xmlWriter.writeElementCData(TAG_GUI_LABEL, volume->getDescriptiveLabel());
      xmlWriter.writeCharacters(labelTableString);
      volume->getStudyMetaDataLinkSet().writeXML(xmlWriter);
      QString volumeTypeName = volume->getVolumeTypeDescription();
      if (volumeTypeName == "Paint") {
         volumeTypeName = "Label";
      }
      xmlWriter.writeElementCData(TAG_VOLUME_TYPE, volumeTypeName);


      xmlWriter.writeEndElement();
   }
   
   xmlWriter.writeEndElement();
   
   xmlWriter.writeEndDocument();

   return dataString;
}


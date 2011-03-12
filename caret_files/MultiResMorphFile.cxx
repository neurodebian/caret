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

#include <QDataStream>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

#define __MULTI_RES_MORPH_FILE_MAIN__
#include "MultiResMorphFile.h"
#undef __MULTI_RES_MORPH_FILE_MAIN__

#include "SpecFile.h"
#include "StringUtilities.h"


MultiResMorphFile::MultiResMorphFile()
: AbstractFile("Multi-Resolution Morphing Parameters File",
               SpecFile::getMultiResMorphFileExtension(),
               true,
               AbstractFile::FILE_FORMAT_XML,
               AbstractFile::FILE_IO_NONE,
               AbstractFile::FILE_IO_NONE,
               AbstractFile::FILE_IO_READ_AND_WRITE,
               AbstractFile::FILE_IO_NONE,
               AbstractFile::FILE_IO_NONE,
               AbstractFile::FILE_IO_NONE,
               AbstractFile::FILE_IO_NONE)
{
    this->initializeParametersSpherical();
}

/**
 *
 * Clears current file data in memory.  Deriving classes must override this method and
 * call AbstractFile::clearAbstractFile() from its clear method.
 */
void
MultiResMorphFile::clear()
{
    AbstractFile::clearAbstractFile();
}

/**
 *  returns true if the file is isEmpty (contains no data)
 */
bool
MultiResMorphFile::empty() const
{
   return false;
}

/**
 * Initialize parameters to their default values for spherical morphing.
 */
void
MultiResMorphFile::initializeParametersSpherical()
{
    this->deleteTemporaryFilesFlag = true;
    this->smoothOutCrossoversFlag = true;;
    this->smoothOutFlatSurfaceOverlapFlag = true;
    this->pointSphericalTrianglesOutwardFlag = false;
    this->alignToCentralSulcusLandmarkFlag = true;
    this->centralSulcusLandmarkName = "LANDMARK.CentralSulcus";

    this->setNumberOfCycles(4);
    this->setNumberOfLevels(6);

    /*
     * Default the parameters for all cycles.
     */
    for (int i = 0; i < MAXIMUM_NUMBER_OF_CYCLES; i++) {
        MultiResolutionMorphingCycle* cycle = this->getCycle(i);
        cycle->setIteration(0, 0);
        cycle->setIteration(1, 25);
        cycle->setIteration(2, 10);
        cycle->setIteration(3, 10);
        cycle->setIteration(4, 5);
        cycle->setIteration(5, 2);
        cycle->setIteration(6, 1);
        cycle->setLinearForce(0.5);
        cycle->setAngularForce(0.3);
        cycle->setStepSize(0.5);
        cycle->setSmoothingStrength(1.0);
        cycle->setSmoothingIterations(10);
        cycle->setSmoothingIterationsEdges(10);
    }

    /*
     * Cycle 4 (index 3)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(3);
        cycle->setIteration(0, 2);
        cycle->setIteration(1, 5);
        cycle->setIteration(2, 10);
        cycle->setIteration(3, 20);
        cycle->setIteration(4, 5);
        cycle->setIteration(5, 0);
        cycle->setIteration(6, 0);
    }

    /*
     * Cycle 3 (index 2)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(2);
        cycle->setIteration(0, 2);
        cycle->setIteration(1, 5);
        cycle->setIteration(2, 10);
        cycle->setIteration(3, 10);
        cycle->setIteration(4, 10);
        cycle->setIteration(5, 0);
        cycle->setIteration(6, 0);
    }

    /*
     * Cycle 2 (index 1)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(1);
        cycle->setIteration(0, 2);
        cycle->setIteration(1, 3);
        cycle->setIteration(2, 15);
        cycle->setIteration(3, 20);
        cycle->setIteration(4, 15);
        cycle->setIteration(5, 0);
        cycle->setIteration(6, 0);
    }

    /*
     * Cycle 1 (index 0)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(0);
        cycle->setIteration(0, 2);
        cycle->setIteration(1, 3);
        cycle->setIteration(2, 5);
        cycle->setIteration(3, 20);
        cycle->setIteration(4, 20);
        cycle->setIteration(5, 15);
        cycle->setIteration(6, 0);
    }
}

/**
 * Initialize parameters to their default values for flat morphing.
 */
void
MultiResMorphFile::initializeParametersFlat()
{
    this->deleteTemporaryFilesFlag = true;
    this->smoothOutCrossoversFlag = true;;
    this->smoothOutFlatSurfaceOverlapFlag = true;
    this->pointSphericalTrianglesOutwardFlag = false;
    this->alignToCentralSulcusLandmarkFlag = true;
    this->centralSulcusLandmarkName = "LANDMARK.CentralSulcus";

    this->setNumberOfCycles(5);
    this->setNumberOfLevels(7);

    /*
     * Default the parameters for all cycles.
     */
    for (int i = 0; i < MAXIMUM_NUMBER_OF_CYCLES; i++) {
        MultiResolutionMorphingCycle* cycle = this->getCycle(i);
        for (int i = 0; i < MultiResolutionMorphingCycle::MAXIMUM_NUMBER_OF_LEVELS; i++) {
            cycle->setIteration(i, 0);
        }
        cycle->setIteration(0, 0);
        cycle->setIteration(1, 10);
        cycle->setIteration(2, 20);
        cycle->setIteration(3, 40);
        cycle->setIteration(4, 60);
        cycle->setIteration(5, 80);
        cycle->setIteration(6, 100);
        cycle->setLinearForce(0.5);
        cycle->setAngularForce(0.5);
        cycle->setStepSize(0.5);
        cycle->setSmoothingStrength(1.0);
        cycle->setSmoothingIterations(50);
        cycle->setSmoothingIterationsEdges(10);
    }

    /*
     * Cycle 5 (index 4)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(4);
        cycle->setIteration(0, 0);
        cycle->setIteration(1, 20);
        cycle->setIteration(2, 20);
        cycle->setIteration(3, 40);
        cycle->setIteration(4, 60);
        cycle->setIteration(5, 50);
        cycle->setIteration(6, 30);
        cycle->setLinearForce(0.4);
    }

    /*
     * Cycle 4 (index 3)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(3);
        cycle->setIteration(0, 0);
        cycle->setIteration(1, 20);
        cycle->setIteration(2, 20);
        cycle->setIteration(3, 40);
        cycle->setIteration(4, 60);
        cycle->setIteration(5, 50);
        cycle->setIteration(6, 30);
    }

    /*
     * Cycle 3 (index 2)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(2);
        cycle->setIteration(0, 0);
        cycle->setIteration(1, 10);
        cycle->setIteration(2, 20);
        cycle->setIteration(3, 40);
        cycle->setIteration(4, 60);
        cycle->setIteration(5, 60);
        cycle->setIteration(6, 50);
    }

    /*
     * Cycle 2 (index 1)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(1);
        cycle->setIteration(0, 0);
        cycle->setIteration(1, 10);
        cycle->setIteration(2, 20);
        cycle->setIteration(3, 40);
        cycle->setIteration(4, 60);
        cycle->setIteration(5, 60);
        cycle->setIteration(6, 50);
    }

    /*
     * Cycle 1 (index 0)
     */
    {
        MultiResolutionMorphingCycle* cycle = this->getCycle(0);
        cycle->setIteration(0, 0);
        cycle->setIteration(1, 10);
        cycle->setIteration(2, 20);
        cycle->setIteration(3, 40);
        cycle->setIteration(4, 60);
        cycle->setIteration(5, 70);
        cycle->setIteration(6, 60);
    }
}

/**
 * Read the contents of the file (header has already been read).
 */
void
MultiResMorphFile::readFileData(QFile& file,
						  QTextStream& stream,
						  QDataStream& binStream,
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
              int cycleCounter = 0;
              QDomNode node = rootElement.firstChild();
              while (node.isNull() == false) {
                 QDomElement elem = node.toElement();
                 if (elem.isNull() == false) {
                    //
                    // Is this a "cycle" element
                    //
                    if (elem.tagName() == MultiResMorphFile::XML_TAG_NUMBER_OF_CYCLES) {
                        this->numberOfCycles = AbstractFile::getXmlElementFirstChildAsInt(elem);
                    }
                    else if (elem.tagName() == MultiResMorphFile::XML_TAG_NUMBER_OF_LEVELS) {
                        this->numberOfLevels = AbstractFile::getXmlElementFirstChildAsInt(elem);
                    }
                    else if (elem.tagName() == MultiResMorphFile::XML_TAG_DELETE_TEMPORARY_FILES) {
                        this->deleteTemporaryFilesFlag = (AbstractFile::getXmlElementFirstChildAsInt(elem) != 0);
                    }
                    else if (elem.tagName() == MultiResMorphFile::XML_TAG_SMOOTH_OUT_CROSSOVERS) {
                        this->smoothOutCrossoversFlag = (AbstractFile::getXmlElementFirstChildAsInt(elem) != 0);
                    }
                    else if (elem.tagName() == MultiResMorphFile::XML_TAG_SMOOTH_OUT_FLAT_OVERLAP) {
                        this->smoothOutFlatSurfaceOverlapFlag = (AbstractFile::getXmlElementFirstChildAsInt(elem) != 0);
                    }
                    else if (elem.tagName() == MultiResMorphFile::XML_TAG_POINT_SPHERICAL_TRIANGLES) {
                        this->pointSphericalTrianglesOutwardFlag = (AbstractFile::getXmlElementFirstChildAsInt(elem) != 0);
                    }
                    else if (elem.tagName() == MultiResMorphFile::XML_TAG_ALIGN_TO_CES) {
                        this->alignToCentralSulcusLandmarkFlag = (AbstractFile::getXmlElementFirstChildAsInt(elem) != 0);
                    }
                    else if (elem.tagName() == MultiResMorphFile::XML_TAG_CES_LANDMARK_NAME) {
                        this->centralSulcusLandmarkName = AbstractFile::getXmlElementFirstChildAsString(elem);
                    }
                    else if (elem.tagName() == MultiResolutionMorphingCycle::XML_TAG_MORPH_CYCLE) {
                        MultiResolutionMorphingCycle* cycle = this->getCycle(cycleCounter);
                        cycle->readXML(node);
                        cycleCounter++;
                    }
                    else if ((elem.tagName() == xmlHeaderOldTagName) ||
                             (elem.tagName() == xmlHeaderTagName)) {
                       // ignore, read by AbstractFile::readFile()
                    }
                    else {
                       std::cout << "WARNING: unrecognized Multi-Res Morph File element: "
                        << elem.tagName().toAscii().constData()
                        << std::endl;
                    }
                 }
                 node = node.nextSibling();
              }
          }
          break;
       case FILE_FORMAT_XML_BASE64:
          throw FileException(filename, "Writing XML Base64 not supported.");
          break;
       case FILE_FORMAT_XML_GZIP_BASE64:
          throw FileException(filename, "Writing XML GZip Base64 not supported.");
          break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
       case FILE_FORMAT_OTHER:
          throw FileException(filename, "Writing in Other format not supported.");
          break;
       case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
           throw FileException(filename, "Writing in CSV format not supported.");
          break;
    }
}

/**
 *  Write the file's data (header has already been written).
 */
void
MultiResMorphFile::writeFileData(QTextStream& stream,
				   QDataStream& binStream,
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
             //
             // Write the number of cycles and other parameters
             //
             AbstractFile::addXmlTextElement(xmlDoc, rootElement,
                     XML_TAG_NUMBER_OF_CYCLES, this->numberOfCycles);
             AbstractFile::addXmlTextElement(xmlDoc, rootElement,
                     XML_TAG_NUMBER_OF_LEVELS, this->numberOfLevels);
             AbstractFile::addXmlTextElement(xmlDoc, rootElement,
                     XML_TAG_DELETE_TEMPORARY_FILES, this->deleteTemporaryFilesFlag);
             AbstractFile::addXmlTextElement(xmlDoc, rootElement,
                     XML_TAG_SMOOTH_OUT_CROSSOVERS, this->smoothOutCrossoversFlag);
             AbstractFile::addXmlTextElement(xmlDoc, rootElement,
                     XML_TAG_SMOOTH_OUT_FLAT_OVERLAP, this->smoothOutFlatSurfaceOverlapFlag);
             AbstractFile::addXmlTextElement(xmlDoc, rootElement,
                     XML_TAG_POINT_SPHERICAL_TRIANGLES, this->pointSphericalTrianglesOutwardFlag);
             AbstractFile::addXmlTextElement(xmlDoc, rootElement,
                     XML_TAG_ALIGN_TO_CES, this->alignToCentralSulcusLandmarkFlag);
             AbstractFile::addXmlTextElement(xmlDoc, rootElement,
                     XML_TAG_CES_LANDMARK_NAME, this->centralSulcusLandmarkName);

             //
             // Write the cycles
             //
             for (int i = 0; i < this->numberOfCycles; i++) {
                cycles[i].writeXML(xmlDoc, rootElement, i);
             }
          }
          break;
       case FILE_FORMAT_XML_BASE64:
          throw FileException(filename, "Writing XML Base64 not supported.");
          break;
       case FILE_FORMAT_XML_GZIP_BASE64:
          throw FileException(filename, "Writing XML GZip Base64 not supported.");
          break;
      case FILE_FORMAT_XML_EXTERNAL_BINARY:
         throw FileException(filename, "Writing XML External Binary not supported.");
         break;      
       case FILE_FORMAT_OTHER:
          throw FileException(filename, "Writing in Other format not supported.");
          break;
       case FILE_FORMAT_COMMA_SEPARATED_VALUE_FILE:
           throw FileException(filename, "Writing in CSV format not supported.");
          break;
    }
}

/**
 * called to read from an XML structure.
 */
void
MultiResolutionMorphingCycle::readXML(QDomNode& nodeIn) throw (FileException)
{
   if (nodeIn.isNull()) {
      return;
   }
   QDomElement elem = nodeIn.toElement();
   if (elem.isNull()) {
      return;
   }
   if (elem.tagName() != MultiResolutionMorphingCycle::XML_TAG_MORPH_CYCLE) {
      QString msg("Incorrect element type passed to MultiResMorphFileCycle::readXML(): \"");
      msg.append(elem.tagName());
      msg.append("\"");
      throw FileException("", msg);
   }

   QDomNode node = nodeIn.firstChild();
   while (node.isNull() == false) {
      QDomElement elem = node.toElement();
      if (elem.isNull() == false) {
         if (elem.tagName() == MultiResolutionMorphingCycle::XML_TAG_ITERATIONS) {
            QString iterString = AbstractFile::getXmlElementFirstChildAsString(elem);
            std::vector<int> intValues;
            StringUtilities::token(iterString, " ", intValues);
            int iterMax = std::min(static_cast<int>(MultiResolutionMorphingCycle::MAXIMUM_NUMBER_OF_LEVELS),
                                   static_cast<int>(intValues.size()));
            for (int i = 0; i < iterMax; i++) {
                this->iterations[i] = intValues[i];
            }
         }
         else if (elem.tagName() == MultiResolutionMorphingCycle::XML_TAG_ANGULAR_FORCE) {
            this->angularForce = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == MultiResolutionMorphingCycle::XML_TAG_LINEAR_FORCE) {
            this->linearForce = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == MultiResolutionMorphingCycle::XML_TAG_STEP_SIZE) {
            this->stepSize = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == MultiResolutionMorphingCycle::XML_TAG_SMOOTHING_STRENGTH) {
            this->smoothingStrength = AbstractFile::getXmlElementFirstChildAsFloat(elem);
         }
         else if (elem.tagName() == MultiResolutionMorphingCycle::XML_TAG_SMOOTHING_ITERATIONS) {
            this->smoothingIterations = AbstractFile::getXmlElementFirstChildAsInt(elem);
         }
         else if (elem.tagName() == MultiResolutionMorphingCycle::XML_TAG_SMOOTHING_ITERATIONS_EDGES) {
            this->smoothingIterationsEdges = AbstractFile::getXmlElementFirstChildAsInt(elem);
         }
         else {
            std::cout << "WARNING: unrecognized MultiResolutionMorphingCycleQ element: "
                      << elem.tagName().toAscii().constData()
                      << std::endl;
         }
      }
      node = node.nextSibling();
   }
}

/**
 * called to write to an XML structure.
 */
void
MultiResolutionMorphingCycle::writeXML(QDomDocument& xmlDoc,
                         QDomElement&  parentElement,
                         const int cycleIndex)
{
   //
   // Create the element for this class instance's data
   //
   QDomElement cycleElement = xmlDoc.createElement(XML_TAG_MORPH_CYCLE);

   //
   // cycle data
   //
   AbstractFile::addXmlTextElement(xmlDoc, cycleElement,
           XML_TAG_ITERATIONS, this->iterations, MAXIMUM_NUMBER_OF_LEVELS);

   AbstractFile::addXmlTextElement(xmlDoc, cycleElement,
           XML_TAG_ANGULAR_FORCE, this->angularForce);

   AbstractFile::addXmlTextElement(xmlDoc, cycleElement,
           XML_TAG_LINEAR_FORCE, this->linearForce);

   AbstractFile::addXmlTextElement(xmlDoc, cycleElement,
           XML_TAG_STEP_SIZE, this->stepSize);

   AbstractFile::addXmlTextElement(xmlDoc, cycleElement,
           XML_TAG_SMOOTHING_STRENGTH, smoothingStrength);

   AbstractFile::addXmlTextElement(xmlDoc, cycleElement,
           XML_TAG_SMOOTHING_ITERATIONS, smoothingIterations);

   AbstractFile::addXmlTextElement(xmlDoc, cycleElement,
           XML_TAG_SMOOTHING_ITERATIONS_EDGES, smoothingIterationsEdges);

   //
   // Add class instance's data to the parent
   //
   parentElement.appendChild(cycleElement);
}


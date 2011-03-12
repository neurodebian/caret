

#ifndef __MULTI_RES_MORPH_FILE_H__
#define __MULTI_RES_MORPH_FILE_H__

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

#include "AbstractFile.h"

/**
 * Multi-Resolution Morphing cycle.
 */
class MultiResolutionMorphingCycle {
public:
    enum { MAXIMUM_NUMBER_OF_LEVELS = 7 };

    /**
     * Constructor.
     */
    MultiResolutionMorphingCycle() {
        this->smoothingStrength = 1.0;
        this->smoothingIterations = 10;
        this->smoothingIterationsEdges = 10;
        this->linearForce = 0.5;
        this->angularForce = 0.3;
        this->stepSize = 0.5;
    }

    /**
     * Destructor.
     */
    virtual ~MultiResolutionMorphingCycle() { }

    /**
     * Get the number of iterations at a level.
     * @param level  Level for which an iteration is sought.
     *    (0 == Fine, 2, 3, ..., N-1 == Course).
     * @return Number of iterations at the level.
     */
    int getIteration(const int level) { return this->iterations[level]; }

    /**
     * Set the number of iterations at a level.
     * @param level The level (0 == Fine, 2, 3, ..., N-1 == Course)
     * @param iterations Number of iterations for level.
     */
    void setIteration(const int level,
                      int iterations) {
        this->iterations[level] = iterations;
    }

    /**
     * Get all iterations for the levels.
     * @param  iterationsOut  Output array into which iterations are loaded.
     */
    void getIterationsAll(int iterationsOut[]) const {

        for (int i = 0; i < MAXIMUM_NUMBER_OF_LEVELS; i++) {
            iterationsOut[i] = this->iterations[i];
        }
    }

    /**
     * Set all iterations at all levels.
     * @param  iterationsIn  Output array from which iterations are obtained.
     */
    void setIterationsAll(const int iterationsIn[]) {
        for (int i = 0; i < MAXIMUM_NUMBER_OF_LEVELS; i++) {
            this->iterations[i] = iterationsIn[i];
        }
    }

    /**
     * Get the smoothing strength.
     * @return Strength value.
     */
    float getSmoothingStrength() const { return this->smoothingStrength; }

    /**
     * Get the iterations.
     * @return Number of iterations of smoothing.
     */
    int getSmoothingIterations() const { return this->smoothingIterations; }

    /**
     * Get the edge iterations.
     * @return Edge iterations.
     */
    int getSmoothingIterationEdges() const { return this->smoothingIterationsEdges; }

    /**
     * Set the strength.
     * @param smoothingStrength - New strength value.
     */
    void setSmoothingStrength(const float smoothingStrength) {
        this->smoothingStrength = smoothingStrength;
    }

    /**
     * Set the iterations.
     * @param smoothingIterations New iterations value.
     */
    void setSmoothingIterations(const int smoothingIterations) {
        this->smoothingIterations = smoothingIterations;
    }

    /**
     * Set the edge iterations.
     * @param smoothingIterationsEdges New edge iterations value.
     */
    void setSmoothingIterationsEdges(const int smoothingIterationsEdges) {
        this->smoothingIterationsEdges = smoothingIterationsEdges;
    }

    /**
     * Get the linear force.
     * @return Linear force value.
     */
    float getLinearForce() const {
        return this->linearForce;
    }

    /**
     * Set the linear force.
     * @param linearForce new linear force value.
     */
    void setLinearForce(float linearForce) {
        this->linearForce = linearForce;
    }

    /**
     * Get the angular force.
     * @return Angular force value.
     */
    float getAngularForce() const {
        return this->angularForce;
    }

    /**
     * Set the angular force.
     * @param angularForce new angular force value.
     */
    void setAngularForce(float angularForce) {
        this->angularForce = angularForce;
    }

    /**
     * Get the step size.
     * @return step size value.
     */
    float getStepSize() const {
        return this->stepSize;
    }

    /**
     * Set the step size.
     * @param stepSize new step size value.
     */
    void setStepSize(float stepSize) {
        this->stepSize = stepSize;
    }

    /**
     * Read from XML.
     */
    void readXML(QDomNode& nodeIn) throw (FileException);

    /**
     * Write as XML.
     */
    void writeXML(QDomDocument& xmlDoc,
                  QDomElement&  parentElement,
                  const int cycleIndex);

    static const QString XML_TAG_MORPH_CYCLE;

private:
    int iterations[MAXIMUM_NUMBER_OF_LEVELS];

    float smoothingStrength;
    int smoothingIterations;
    int smoothingIterationsEdges;

    float linearForce;
    float angularForce;
    float stepSize;

    static const QString XML_TAG_ITERATIONS;
    static const QString XML_TAG_LINEAR_FORCE;
    static const QString XML_TAG_ANGULAR_FORCE;
    static const QString XML_TAG_STEP_SIZE;
    static const QString XML_TAG_SMOOTHING_STRENGTH;
    static const QString XML_TAG_SMOOTHING_ITERATIONS;
    static const QString XML_TAG_SMOOTHING_ITERATIONS_EDGES;

};

#ifdef __MULTI_RES_MORPH_FILE_MAIN__
    const QString MultiResolutionMorphingCycle::XML_TAG_ITERATIONS = "iterations";
    const QString MultiResolutionMorphingCycle::XML_TAG_LINEAR_FORCE = "linearForce";
    const QString MultiResolutionMorphingCycle::XML_TAG_ANGULAR_FORCE = "angularForce";
    const QString MultiResolutionMorphingCycle::XML_TAG_STEP_SIZE = "stepSize";
    const QString MultiResolutionMorphingCycle::XML_TAG_SMOOTHING_STRENGTH = "smoothingStrength";
    const QString MultiResolutionMorphingCycle::XML_TAG_SMOOTHING_ITERATIONS = "smoothingIterations";
    const QString MultiResolutionMorphingCycle::XML_TAG_SMOOTHING_ITERATIONS_EDGES = "smoothingIterationsEdges";
    const QString MultiResolutionMorphingCycle::XML_TAG_MORPH_CYCLE = "cycle";
#endif // __MULTI_RES_MORPH_FILE_MAIN__

class MultiResMorphFile : public AbstractFile {
public:

    enum { MAXIMUM_NUMBER_OF_CYCLES = 10 };

    // constructor
    MultiResMorphFile();

    // constructor
    ~MultiResMorphFile() { }

    /**
     * Get the number of cycles.
     * @return The number of cycles.
     */
    int getNumberOfCycles() const {
        return this->numberOfCycles;
    }

    /**
     * Set the number of cycles.
     * @param numberOfCycles New number of cycles.
     */
    void setNumberOfCycles(const int numberOfCycles) {
        this->numberOfCycles = numberOfCycles;
        this->numberOfCycles = std::min(this->numberOfCycles, static_cast<int>(MAXIMUM_NUMBER_OF_CYCLES));
        this->numberOfCycles = std::max(this->numberOfCycles, 1);
    }

    /**
     * Get the number of levels.
     * @return The number of levels.
     */
    int getNumberOfLevels() const {
        return this->numberOfLevels;
    }

    /**
     * Set the number of levels.
     * @param numberOfLevels New number of levels.
     */
    void setNumberOfLevels(const int numberOfLevels) {
        this->numberOfLevels = numberOfLevels;
        this->numberOfLevels = std::min(this->numberOfLevels, static_cast<int>(MultiResolutionMorphingCycle::MAXIMUM_NUMBER_OF_LEVELS));
        this->numberOfLevels = std::max(this->numberOfLevels, 1);
    }

    /**
     * Get pointer to cycle for updating.
     */
    MultiResolutionMorphingCycle* getCycle(int cycleIndex) {
        return &this->cycles[cycleIndex];
    }

    /**
     * Get pointer to cycle for reading.
     */
    const MultiResolutionMorphingCycle* getCycle(int cycleIndex) const {
        return &this->cycles[cycleIndex];
    }

    /**
     * Should temporary files be deleted.
     * @return true if temporary files should be deleted, else false.
     */
    bool isDeleteTemporaryFiles() const {
        return this->deleteTemporaryFilesFlag;
    }

    /**
     * Set temporary files.
     * @param deleteTemporaryFilesFlag true to delete files, else false.
     */
    void setDeleteTemporaryFiles(const bool deleteTemporaryFilesFlag) {
        this->deleteTemporaryFilesFlag = deleteTemporaryFilesFlag;
    }

    /**
     * Should crossovers be smoothed to eliminate them.
     * @return true if crossovers should be smoothed, else false.
     */
    bool isSmoothOutCrossovers() const {
        return this->smoothOutCrossoversFlag;
    }

    /**
     * Set crossovers should be smoothed to eliminate them on surface.
     * @param  smoothOutCrossovers  true to smooth, else false.
     */
    void setSmoothOutCrossovers(const bool smoothOutCrossoversFlag) {
        this->smoothOutCrossoversFlag = smoothOutCrossoversFlag;
    }

    /**
     * Should flat surface overlap be smoothed to eliminate it.
     * @return true if surface overlap should be smoothed, else false.
     */
    bool isSmoothOutFlatSurfaceOverlap() const {
        return this->smoothOutFlatSurfaceOverlapFlag;
    }

    /**
     * Set surface overlap should be smoothed to eliminate it.
     * @param  smoothOutSurfaceOverlapFlag  true to smooth, else false.
     */
    void setSmoothOutFlatSurfaceOverlap(const bool smoothOutFlatSurfaceOverlapFlag) {
        this->smoothOutFlatSurfaceOverlapFlag = smoothOutFlatSurfaceOverlapFlag;
    }

    /**
     * Should the surface be aligned using the Central Sulcus Landmark?
     * @return true if align, else false.
     */
    bool isAlignToCentralSulcalsLandmark() const {
        return this->alignToCentralSulcusLandmarkFlag;
    }

    /**
     * Set the surface be aligned using the Central Sulcus Landmark.
     * @@param alignToCentralSulcusLandmarkFlag True if align, else false.
     */
    void setAlignToCentralSulcusLandmark(const bool alignToCentralSulcusLandmarkFlag) {
        this->alignToCentralSulcusLandmarkFlag = alignToCentralSulcusLandmarkFlag;
    }

    /**
     * Get the name of the Central Sulcus Landmark.
     * @return Name of Central Sulcus Landmark.
     */
    QString getCentralSulcusLandmarkName() {
        return this->centralSulcusLandmarkName;
    }

    /**
     * Set the name of the Central Sulcus Landmark.
     * @param centralSulcusLandmarkName Name of Central Sulcus Landmark.
     */
    void setCentralSulcusLandmarkName(const QString& centralSulcusLandmarkName) {
        this->centralSulcusLandmarkName = centralSulcusLandmarkName;
    }

    /**
     * Should spherical surface triangles be oriented so normal point out of the sphere?
     * @return true if so, else false.
     */
    bool isPointSphericalTrianglesOutward() const {
        return this->pointSphericalTrianglesOutwardFlag;
    }

    /**
     * Set spherical surface triangles should be oriented so normal points out of the sphere.
     * @param pointSphericalTrianglesOutwardFlag true if so, else false.
     */
    void setPointSphericalTrianglesOutward(const bool pointSphericalTrianglesOutwardFlag) {
        this->pointSphericalTrianglesOutwardFlag = pointSphericalTrianglesOutwardFlag;
    }

    /**
     *
     * Clears current file data in memory.  Deriving classes must override this method and
     * call AbstractFile::clearAbstractFile() from its clear method.
     */
    void clear();

    /**
     *  returns true if the file is isEmpty (contains no data)
     */
    bool empty() const;

    /**
     * Initialize parameters to their default values for flat morphing.
     */
    void initializeParametersFlat();

    /**
     * Initialize parameters to their default values for spherical morphing.
     */
    void initializeParametersSpherical();

    /**
     * Read the contents of the file (header has already been read).
     */
    void readFileData(QFile& file,
                      QTextStream& stream,
                      QDataStream& binStream,
                      QDomElement& rootElement) throw (FileException);

    /**
     *  Write the file's data (header has already been written).
     */
    void writeFileData(QTextStream& stream,
                       QDataStream& binStream,
                       QDomDocument& xmlDoc,
                       QDomElement& rootElement) throw (FileException);


private:
    MultiResolutionMorphingCycle cycles[MAXIMUM_NUMBER_OF_CYCLES];

    int numberOfCycles;
    int numberOfLevels;

    bool deleteTemporaryFilesFlag;
    bool smoothOutCrossoversFlag;
    bool smoothOutFlatSurfaceOverlapFlag;
    bool pointSphericalTrianglesOutwardFlag;
    bool alignToCentralSulcusLandmarkFlag;
    QString centralSulcusLandmarkName;

    static const QString XML_TAG_NUMBER_OF_CYCLES;
    static const QString XML_TAG_NUMBER_OF_LEVELS;
    static const QString XML_TAG_DELETE_TEMPORARY_FILES;
    static const QString XML_TAG_SMOOTH_OUT_CROSSOVERS;
    static const QString XML_TAG_POINT_SPHERICAL_TRIANGLES;
    static const QString XML_TAG_ALIGN_TO_CES;
    static const QString XML_TAG_CES_LANDMARK_NAME;
    static const QString XML_TAG_SMOOTH_OUT_FLAT_OVERLAP;

};

#ifdef __MULTI_RES_MORPH_FILE_MAIN__
    const QString MultiResMorphFile::XML_TAG_NUMBER_OF_CYCLES = "numberOfCycles";
    const QString MultiResMorphFile::XML_TAG_NUMBER_OF_LEVELS = "numberOfLevels";
    const QString MultiResMorphFile::XML_TAG_DELETE_TEMPORARY_FILES = "deleteTemporaryFilesFlag";
    const QString MultiResMorphFile::XML_TAG_SMOOTH_OUT_CROSSOVERS = "smoothOutCrossoversFlag";
    const QString MultiResMorphFile::XML_TAG_POINT_SPHERICAL_TRIANGLES = "pointSphericalTrianglesOutwardFlag";
    const QString MultiResMorphFile::XML_TAG_ALIGN_TO_CES = "alignToCentralSulcusLandmarkFlag";
    const QString MultiResMorphFile::XML_TAG_CES_LANDMARK_NAME = "centralSulcusLandmarkName";
    const QString MultiResMorphFile::XML_TAG_SMOOTH_OUT_FLAT_OVERLAP = "smoothOutFlatSurfaceOverlapFlag";
#endif



#endif // __MULTI_RES_MORPH_FILE_H__


#ifndef __BRAIN_MODEL_SURFACE_CELL_ATTRIBUTE_ASSIGNMENT_H__
#define __BRAIN_MODEL_SURFACE_CELL_ATTRIBUTE_ASSIGNMENT_H__

#include <vector>

#include "BrainModelAlgorithm.h"

class BrainModelSurface;
class BrainModelSurfacePointLocator;
class CellProjectionFile;
class PaintFile;

/// class for assigning cell attributes
class BrainModelSurfaceCellAttributeAssignment : public BrainModelAlgorithm {
   public:
      /// attribute that is to be assigned
      enum ASSIGN_ATTRIBUTE {
         /// assign area attribute
         ASSIGN_ATTRIBUTE_AREA,
         /// assign geography attribute
         ASSIGN_ATTRIBUTE_GEOGRAPHY,
         /// assign region of interest attribute
         ASSIGN_ATTRIBUTE_REGION_OF_INTEREST
      };
      
      /// assignment method
      enum ASSIGNMENT_METHOD {
         /// assign append
         ASSIGNMENT_METHOD_APPEND, 
         /// assign clear
         ASSIGNMENT_METHOD_CLEAR, 
         /// assign replace
         ASSIGNMENT_METHOD_REPLACE 
      };
      
      // constructor
      BrainModelSurfaceCellAttributeAssignment(BrainSet* brainSetIn,
                             const BrainModelSurface* leftSurfaceIn,
                             const BrainModelSurface* rightSurfaceIn,
                             const BrainModelSurface* cerebellumSurfaceIn,
                             CellProjectionFile* cellProjectionFileIn,
                             const PaintFile* paintFileIn,
                             const std::vector<bool>& paintColumnsSelectedIn,
                             const float maximumDistanceFromSurfaceIn,
                             const ASSIGN_ATTRIBUTE assignAttributeIn,
                             const ASSIGNMENT_METHOD assignmentMethodIn,
                             const QString attributeIDIn,
                             const bool optionIgnoreUnknownValuesFlagIn);

      // destructor
      ~BrainModelSurfaceCellAttributeAssignment();     
      
      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);
      
      // get attribute names and values
      static void getAttributeNamesAndValues(std::vector<QString>& namesOut,
                                    std::vector<ASSIGN_ATTRIBUTE>& valuesOut);
                                       
      // get assignment names and values
      static void getAssignmentNamesAndValues(std::vector<QString>& namesOut,
                                    std::vector<ASSIGNMENT_METHOD>& valuesOut);
                                    
   protected:
      /// left surface
      const BrainModelSurface* leftSurface;

      /// right surface
      const BrainModelSurface* rightSurface;

      /// cerebellum surface
      const BrainModelSurface* cerebellumSurface;

      /// cell projection file
      CellProjectionFile* cellProjectionFile;

      /// paint file
      const PaintFile* paintFile;

      /// paint columns selected for assignment
      const std::vector<bool>& paintColumnsSelected;

      /// maximum distance from surface
      const float maximumDistanceFromSurface;

      /// attribute being assigned
      const ASSIGN_ATTRIBUTE assignAttribute;

      /// method of assignment
      const ASSIGNMENT_METHOD assignmentMethod;
      
      /// attribute id
      QString attributeID;
      
      /// ignore ??? values option
      const bool optionIgnoreUnknownValuesFlag;
      
      /// left surface point locator    
      BrainModelSurfacePointLocator* leftPointLocator;
      
      /// right surface point locator
      BrainModelSurfacePointLocator* rightPointLocator;
      
      /// cerebellum surface point locator
      BrainModelSurfacePointLocator* cerebellumPointLocator;

};

#endif // __BRAIN_MODEL_SURFACE_CELL_ATTRIBUTE_ASSIGNMENT_H__



#ifndef __GUI_NODE_FILE_TYPE_H__
#define __GUI_NODE_FILE_TYPE_H__

/// node file types
enum GUI_NODE_FILE_TYPE {
   /// none
   GUI_NODE_FILE_TYPE_NONE,
   /// areal estimation
   GUI_NODE_FILE_TYPE_AREAL_ESTIMATION,
   /// deformation field
   GUI_NODE_FILE_TYPE_DEFORMATION_FIELD,
   /// geodesic distance
   GUI_NODE_FILE_TYPE_GEODESIC_DISTANCE,
   /// lat lon
   GUI_NODE_FILE_TYPE_LAT_LON,
   /// metric
   GUI_NODE_FILE_TYPE_METRIC,   // gifti
   /// paint
   GUI_NODE_FILE_TYPE_PAINT,    // gifti
   /// probabilistic atlas
   GUI_NODE_PROBABILISTIC_ATLAS,
   /// rgb paint
   GUI_NODE_FILE_TYPE_RGB_PAINT,
   /// section
   GUI_NODE_FILE_TYPE_SECTION,
   /// surface shape
   GUI_NODE_FILE_TYPE_SURFACE_SHAPE,
   /// topography file
   GUI_NODE_FILE_TYPE_TOPOGRAPHY
};
      
#endif // __GUI_NODE_FILE_TYPE_H__

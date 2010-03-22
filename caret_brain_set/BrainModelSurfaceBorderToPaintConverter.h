#ifndef __BRAIN_MODEL_SURFACE_BORDER_TO_PAINT_CONVERTER_H__
#define	__BRAIN_MODEL_SURFACE_BORDER_TO_PAINT_CONVERTER_H__

#include <QString>

#include <BrainModelAlgorithm.h>

class BrainModelSurface;
class BorderProjectionFile;
class PaintFile;

class BrainModelSurfaceBorderToPaintConverter : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceBorderToPaintConverter(
                                   BrainSet* bs,
                                   BrainModelSurface* surfaceIn,
                                   BorderProjectionFile* borderProjectionFileIn,
                                   PaintFile* paintFileIn,
                                   int paintColumnNumberIn,
                                   QString paintColumnNameIn);

      // destructor
      ~BrainModelSurfaceBorderToPaintConverter();

      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);

   protected:
      /// the surface
      BrainModelSurface* surface;

      /// the border projection file
      BorderProjectionFile* borderProjectionFile;

      /// the paint file
      PaintFile* paintFile;

      /// the paint column number
      int paintColumnNumber;

      /// the name of the paint column
      QString paintColumnName;

};

#endif	/* __BRAIN_MODEL_SURFACE_BORDER_TO_PAINT_CONVERTER_H__ */


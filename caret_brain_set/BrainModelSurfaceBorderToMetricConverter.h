#ifndef __BRAIN_MODEL_SURFACE_BORDER_TO_METRIC_CONVERTER_H__
#define	__BRAIN_MODEL_SURFACE_BORDER_TO_METRIC_CONVERTER_H__

#include <QString>

#include <BrainModelAlgorithm.h>

class BrainModelSurface;
class BorderProjectionFile;
class MetricFile;

class BrainModelSurfaceBorderToMetricConverter : public BrainModelAlgorithm {
   public:
      // constructor
      BrainModelSurfaceBorderToMetricConverter(
                                   BrainSet* bs,
                                   BrainModelSurface* surfaceIn,
                                   BorderProjectionFile* borderProjectionFileIn,
                                   MetricFile* metricFileIn,
                                   int metricColumnNumberIn,
                                   QString metricColumnNameIn);

      // destructor
      ~BrainModelSurfaceBorderToMetricConverter();

      // execute the algorithm
      void execute() throw (BrainModelAlgorithmException);

   protected:
      /// the surface
      BrainModelSurface* surface;

      /// the border projection file
      BorderProjectionFile* borderProjectionFile;

      /// the metric file
      MetricFile* metricFile;

      /// the metric column number
      int metricColumnNumber;

      /// the name of the metric column
      QString metricColumnName;
   
};

#endif	/* __BRAIN_MODEL_SURFACE_BORDER_TO_METRIC_CONVERTER_H__ */


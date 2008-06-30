
#ifndef __STATISTIC_NUMERICAL_RECIPES_H__
#define __STATISTIC_NUMERICAL_RECIPES_H__

#include "StatisticException.h"

/// code from numerical recipes book
class StatisticNumericalRecipes {
   public:
   
      static float SIGN(const float a, const float b);
      static float FMAX(const float a, const float b);
      static int IMIN(const int a, const int b);
      static float SQR(const float a);
      static float *vector(long nl, long nh) throw (StatisticException);
      static void free_vector(float *v, long nl, long nh);
      static float pythag(float a, float b);
      static float **matrix(long nrl, long nrh, long ncl, long nch) throw (StatisticException);
      static void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);
      static void svdcmp(float **a, int m, int n, float w[], float **v) throw (StatisticException);

   protected:
};
#endif // __STATISTIC_NUMERICAL_RECIPES_H__

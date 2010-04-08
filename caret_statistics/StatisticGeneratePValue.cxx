
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

#include <cmath>
#include <iostream>

#include "StatisticDataGroup.h"
#include "StatisticDcdflib.h"
#include "StatisticGeneratePValue.h"

/**
 * constructor.
 * @param 1st data group must be the statistic for T-Distribution
 * @param 2nd data group must be the degrees of freedom for T-Distribution

 * @param 1st data group must be the statistic for F-Distribution
 * @param 2nd data group must be the numerator degrees of freedom for F-Distribution
 * @param 2nd data group must be the deonominator degrees of freedom for F-Distribution
 */
StatisticGeneratePValue::StatisticGeneratePValue(const INPUT_STATISTIC inputStatisticTypeIn)
   : StatisticAlgorithm("Generate P-Value")
{
   inputStatisticType = inputStatisticTypeIn;
   outputDataGroupContainingPValues = NULL;
}

/**
 * destructor.
 */
StatisticGeneratePValue::~StatisticGeneratePValue()
{
   if (outputDataGroupContainingPValues != NULL) {
      delete outputDataGroupContainingPValues;
      outputDataGroupContainingPValues = NULL;
   }
}

/**
 * generate P-Value for One-Tailed T-Test.
 */
float 
StatisticGeneratePValue::getOneTailTTestPValue(const float degreesOfFreedom,
                                               const float T)
{
   StatisticDataGroup tSDG(&T,
                           1,
                           StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup dofSDG(&degreesOfFreedom,
                             1,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
                                 
   StatisticGeneratePValue genP(INPUT_STATISTIC_T_ONE_TALE);
   genP.addDataGroup(&tSDG);
   genP.addDataGroup(&dofSDG);
   
   float pValue = -100000.0;
   
   try {
      genP.execute();
      const StatisticDataGroup* sdg = genP.getOutputDataGroupContainingPValues();
      if (sdg->getNumberOfData() > 0) {
         pValue = sdg->getData(0);
      }
   }
   catch (StatisticException&) {
   }
   
   return pValue;
}
                                         
/**
 * generate P-Value for Two-Tailed T-Test.
 */
float 
StatisticGeneratePValue::getTwoTailTTestPValue(const float degreesOfFreedom,
                                               const float T)
{
   StatisticDataGroup tSDG(&T,
                           1,
                           StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup dofSDG(&degreesOfFreedom,
                             1,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
                                 
   StatisticGeneratePValue genP(INPUT_STATISTIC_T_TWO_TALE);
   genP.addDataGroup(&tSDG);
   genP.addDataGroup(&dofSDG);
   
   float pValue = -100000.0;
   
   try {
      genP.execute();
      const StatisticDataGroup* sdg = genP.getOutputDataGroupContainingPValues();
      if (sdg->getNumberOfData() > 0) {
         pValue = sdg->getData(0);
      }
   }
   catch (StatisticException&) {
   }
   
   return pValue;
}
                                         
/**
 * generate P-Value for an F-Statistic.
 */
float 
StatisticGeneratePValue::getFStatisticPValue(const float numeratorDegreesOfFreedom,
                                             const float denominatorDegreesOfFreedom,
                                             const float F)
{
   StatisticDataGroup fSDG(&F,
                           1,
                           StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup numSDG(&numeratorDegreesOfFreedom,
                             1,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
   StatisticDataGroup denSDG(&denominatorDegreesOfFreedom,
                             1,
                             StatisticDataGroup::DATA_STORAGE_MODE_POINT);
                                 
   StatisticGeneratePValue genP(INPUT_STATISTIC_F);
   genP.addDataGroup(&fSDG);
   genP.addDataGroup(&numSDG);
   genP.addDataGroup(&denSDG);
   
   float pValue = -100000.0;
   
   try {
      genP.execute();
      const StatisticDataGroup* sdg = genP.getOutputDataGroupContainingPValues();
      if (sdg->getNumberOfData() > 0) {
         pValue = sdg->getData(0);
      }
   }
   catch (StatisticException&) {
   }
   
   return pValue;
}

/**
 * generate the P-Values.
 */
void 
StatisticGeneratePValue::execute() throw (StatisticException)
{
   
   StatisticDataGroup* statisticDataGroup = NULL;
   StatisticDataGroup* degreesOfFreedomOneDataGroup = NULL;
   StatisticDataGroup* degreesOfFreedomTwoDataGroup = NULL;
   
   switch (inputStatisticType) {
      case INPUT_STATISTIC_F:
         if (getNumberOfDataGroups() != 3) {
            throw StatisticException("Number of data groups for StatisticGeneratePValue must be 3.\n"
                                     "1st group contains the statistics\n"
                                     "2nd group contains the numerator degrees-of-freedom\n"
                                     "3rd group contains the denominator degrees-of-freedom");
         }
         statisticDataGroup = getDataGroup(0);
         degreesOfFreedomOneDataGroup = getDataGroup(1);
         degreesOfFreedomTwoDataGroup = getDataGroup(2);
         break;
      case INPUT_STATISTIC_T_ONE_TALE:
      case INPUT_STATISTIC_T_TWO_TALE:
         if (getNumberOfDataGroups() != 2) {
            throw StatisticException("Number of data groups for StatisticGeneratePValue must be 2.\n"
                                     "1st group contains the statistics\n"
                                     "2nd group contains the degrees-of-freedom");
         }
         statisticDataGroup = getDataGroup(0);
         degreesOfFreedomOneDataGroup = getDataGroup(1);
         break;
   }
   
   const int numValues = statisticDataGroup->getNumberOfData();
   if (numValues <= 0) {
      throw StatisticException("Data group passed to StatisticGeneratePValue is empty.");
   }
   if (numValues != degreesOfFreedomOneDataGroup->getNumberOfData()) {
      throw StatisticException("Degrees of Freedom Data Group one must have same number of values as the Statistic Data Group.");
   }
   if (degreesOfFreedomTwoDataGroup != NULL) {
      if (numValues != degreesOfFreedomTwoDataGroup->getNumberOfData()) {
         throw StatisticException("Degrees of Freedom Data Group two must have same number of values as the Statistic Data Group.");
      }
   }
   
   float* outputPValues = new float[numValues];
   
   for (int i = 0; i < numValues; i++) {
      double statistic = statisticDataGroup->getData(i);
      const double dof1 = degreesOfFreedomOneDataGroup->getData(i);
      double dof2 = 0.0;
      if (degreesOfFreedomTwoDataGroup != NULL) {
         dof2 = degreesOfFreedomTwoDataGroup->getData(i);
      }
      double pValue = 0.0;
      
      switch (inputStatisticType) {
         case INPUT_STATISTIC_F:
            {
               //
               // Use symmetry
               //
               if (statistic < 0.0) {
                  statistic = -statistic;
               }

               //if( statistic <= 0.0 || dof1 < 1.0 ) {
               //   pValue = 1.0;
               //}
               //else {
                  //
                  // Use dcdflib routine to calculate P
                  // Note they report close to one (ie P = 0.95) and Q = 1 - P
                  // so just use Q.
                  //
                  int which = 1;
                  double p = 0.0;
                  double q = 0.0;
                  double f = statistic;
                  double dfn = dof1;
                  double dfd = dof2;
                  int status = 0;
                  double bound = 0;
                  
                  cdff(&which,
                       &p,
                       &q,
                       &f,
                       &dfn,
                       &dfd,
                       &status,
                       &bound);
                       
                  if (status != 0) {
                     //pValue = p;
                     std::cout << "WARNING: F-Statistic to P-Value function (cdft) failed, code="
                               << status << "." << std::endl;
                     std::cout << "   F: " << f << ", "
                               << "DOF-N: " << dfn << ", "
                               << "DOF-D: " << dfd << std::endl;
                  }
                  pValue = q;
               //}
            }
            break;
         case INPUT_STATISTIC_T_ONE_TALE:
            {
               //
               // Use symmetry
               //
               if (statistic < 0.0) {
                  statistic = -statistic;
               }

               if( statistic <= 0.0 || dof1 < 1.0 ) {
                  pValue = 1.0;
               }
               else {
                  //
                  // Use dcdflib routine to calculate P
                  // Note they report close to one (ie P = 0.95) and Q = 1 - P
                  // The result is also a one tail test so we really one Q * 2
                  //
                  int which = 1;
                  double p = 0.0;
                  double q = 0.0;
                  double t = statistic;
                  double df = dof1;
                  int status = 0;
                  double bound = 0;
                  
                  cdft(&which,
                       &p,
                       &q,
                       &t,
                       &df,
                       &status,
                       &bound);
                       
                  if (status != 0) {
                     //pValue = p;
                     std::cout << "WARNING: T-Statistic to P-Value function (cdft) failed, code="
                               << status << "." << std::endl;
                  }
                  pValue = q;
               }
            }
            break;
         case INPUT_STATISTIC_T_TWO_TALE:
            {
               //
               // Use symmetry
               //
               if (statistic < 0.0) {
                  statistic = -statistic;
               }

               if( statistic <= 0.0 || dof1 < 1.0 ) {
                  pValue = 1.0;
               }
               else {
                  //
                  // Use dcdflib routine to calculate P
                  // Note they report close to one (ie P = 0.95) and Q = 1 - P
                  // The result is also a one tail test so we really one Q * 2
                  //
                  int which = 1;
                  double p = 0.0;
                  double q = 0.0;
                  double t = statistic;
                  double df = dof1;
                  int status = 0;
                  double bound = 0;
                  
                  cdft(&which,
                       &p,
                       &q,
                       &t,
                       &df,
                       &status,
                       &bound);
                       
                  if (status != 0) {
                     //pValue = p;
                     std::cout << "WARNING: T-Statistic to P-Value function (cdft) failed, code="
                               << status << "." << std::endl;
                  }
                  pValue = q * 2.0;
               }
            }
            break;
      }
      outputPValues[i] = pValue;
   }
   
   outputDataGroupContainingPValues = new StatisticDataGroup(outputPValues,
                                                             numValues,
                                          StatisticDataGroup::DATA_STORAGE_MODE_TAKE_OWNERSHIP);
}

/***********************************************************************/
/****      Taken from AFNI's mri_stats.c                            ****/
/****   Provide a ln(gamma(x)) function for stupid math libraries.  ****/
/****   This routine is not very efficient!  Don't use elsewhere.   ****/
/****   (cf. Abramowitz and Stegun, Eq. 6.1.36.)                    ****/
/***********************************************************************/

/** 
 *  Taken from AFNI's mri_stats.c
 *  log of gamma, for argument between 1 and 2
 */
double 
StatisticGeneratePValue::gamma_12( double y )
{
   double x , g ;
   x = y - 1.0 ;
   g = ((((((( 0.035868343 * x - 0.193527818 ) * x
                               + 0.482199394 ) * x
                               - 0.756704078 ) * x
                               + 0.918206857 ) * x
                               - 0.897056937 ) * x
                               + 0.988205891 ) * x
                               - 0.577191652 ) * x + 1.0 ;
   return std::log(g) ;
}

/** 
  * Taken from AFNI's mri_stats.c
  * asymptotic expansion of ln(gamma(x)) for large positive x 
  */
double 
StatisticGeneratePValue::gamma_asympt(double x)
{
   const double LNSQRT2PI = 0.918938533204672;  /* ln(sqrt(2*PI)) */
   double sum ;

   sum = (x-0.5)*std::log(x) - x + LNSQRT2PI + 1.0/(12.0*x) - 1./(360.0*x*x*x) ;
   return sum ;
}


/**
  * Taken from AFNI's mri_stats.c
  * log of gamma, argument positive (not very efficient!)
  */
double 
StatisticGeneratePValue::gamma( double x )
{
   double w , g ;

   if( x <= 0.0 ){
      fprintf(stderr,"Internal gamma: argument %g <= 0\a\n",x) ;
      return 0.0 ;
   }

   if( x <  1.0 ) return gamma_12( x+1.0 ) - log(x) ;
   if( x <= 2.0 ) return gamma_12( x ) ;
   if( x >= 6.0 ) return gamma_asympt(x) ;

   g = 0 ; w = x ;
   while( w > 2.0 ){
      w -= 1.0 ; g += std::log(w) ;
   }
   return ( gamma_12(w) + g ) ;
}
/**
 *    Taken from AFNI's mri_stats.c
 * compute log of complete beta function, using the
 * Unix math library's log gamma function.  If this is
 * not available, see the end of this file.
*/
double 
StatisticGeneratePValue::lnbeta( double p , double q )
{
   return (gamma(p) + gamma(q) - gamma(p+q)) ;
}

/**
  * Taken from AFNI's mri_stats.c
  * TRANSLATED FROM THE ORIGINAL FORTRAN:
  *algorithm as 63  appl. statist. (1973), vol.22, no.3
  *
  *   computes incomplete beta function ratio for arguments
  *   x between zero and one, p and q positive.
  *   log of complete beta function, beta, is assumed to be known
  */
double 
StatisticGeneratePValue::incbeta( double x , double p , double q , double beta )
{
   const double ZERO = 0.0;
   const double ONE  = 1.0;
   const double ACU  = 1.0e-15;
   
   double betain , psq , cx , xx,pp,qq , term,ai , temp , rx ;
   int indx , ns ;

   if( p <= ZERO || q <= ZERO ) return -1.0 ;  /* error! */

   if( x <= ZERO ) return ZERO ;
   if( x >= ONE  ) return ONE ;

   /**  change tail if necessary and determine s **/

   psq = p+q ;
   cx  = ONE-x ;
   if(  p < psq*x ){
      xx   = cx ;
      cx   = x ;
      pp   = q ;
      qq   = p ;
      indx = 1 ;
   } else {
      xx   = x ;
      pp   = p ;
      qq   = q ;
      indx = 0 ;
   }

   term   = ONE ;
   ai     = ONE ;
   betain = ONE ;
   ns     = static_cast<int>(qq + cx*psq) ;

   /** use soper's reduction formulae **/

      rx = xx/cx ;

lab3:
      temp = qq-ai ;
      if(ns == 0) rx = xx ;

lab4:
      term   = term*temp*rx/(pp+ai) ;
      betain = betain+term ;
      temp   = std::fabs(term) ;
      if(temp <= ACU && temp <= ACU*betain) goto lab5 ;

      ai = ai+ONE ;
      ns = ns-1 ;
      if(ns >= 0) goto lab3 ;
      temp = psq ;
      psq  = psq+ONE ;
      goto lab4 ;

lab5:
      betain = betain*std::exp(pp*std::log(xx)+(qq-ONE)*std::log(cx)-beta)/pp ;
      if(indx) betain=ONE-betain ;

   return betain ;
}



#ifndef __FALSE_DISCOVERY_RATE_H__
#define __FALSE_DISCOVERY_RATE_H__

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

#include "StatisticAlgorithm.h"

/// given an array of P-Values, determine the false discovery rate
/// "Thresholding of Statistical Maps in Functional Neuroimaging Using the False
/// Discovery Rate" by Christopher R. Genovese, Nicole A. Lazar, and Thomas Nichols
/// in NeuroImage 15, 870-878 (2002).
class StatisticFalseDiscoveryRate : public StatisticAlgorithm {
   public:
      /// determination of "C" constant
      enum C_CONSTANT {
         /// use "C" equal to 1 
         C_CONSTANT_1,
         /// use "C" equal to summation of 1/i for i = [1, numValues]
         C_CONSTANT_SUMMATION
      };
      
      // constructor 
      StatisticFalseDiscoveryRate(const float qIn,
                                  const C_CONSTANT cConstantIn);
                         
      // destructor
      ~StatisticFalseDiscoveryRate();
      
      // execute the algorthm
      void execute() throw (StatisticException);
      
      // get the "p-cutoff" that was found when execute() ran
      float getPCutoff() const { return pCutoff; }
      
   protected:
      /// the user entered q-value
      float q;
      
      /// the "C" constant
      C_CONSTANT cConstant;
      
      /// p-cutoff found when executed
      float pCutoff;
};

#endif // __FALSE_DISCOVERY_RATE_H__

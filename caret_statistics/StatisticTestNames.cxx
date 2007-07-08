
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

#include "StatisticTestNames.h"

/**
 * get the names of the tests.
 */
void 
StatisticTestNames::getTestNames(std::vector<std::string>& testNames)
{
   testNames.clear();
   
   testNames.push_back("Correlation statistic");
   testNames.push_back("T-statistic");
   testNames.push_back("F-statistic");
   testNames.push_back("Z-score");
   testNames.push_back("Chi-squared distribution");
   testNames.push_back("Beta distribution");
   testNames.push_back("Binomial distribution");
   testNames.push_back("Gamma distribution");
   testNames.push_back("Poisson distribution");
   testNames.push_back("Normal distribution");
   testNames.push_back("F-statistic noncentral");
   testNames.push_back("Chi-squared noncentral");
   testNames.push_back("Logistic distribution");
   testNames.push_back("Laplace distribution");
   testNames.push_back("Uniform distribition");
   testNames.push_back("T-statistic noncentral");
   testNames.push_back("Weibull distribution");
   testNames.push_back("Chi distribution");
   testNames.push_back("Inverse Gaussian distribution");
   testNames.push_back("Extreme Value distribution");
   testNames.push_back("P-value");
   testNames.push_back("Log P-value");
   testNames.push_back("Log10 P-value");
   testNames.push_back("Estimate");
}

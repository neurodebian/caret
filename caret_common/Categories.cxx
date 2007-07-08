
#include "Categories.h"

/**
 * get a list of valid categories.
 */
void 
Categories::getAllCategories(std::vector<QString>& categories)
{
   categories.clear();
   categories.push_back("ATLAS");
   categories.push_back("INDIVIDUAL");
   categories.push_back("TUTORIAL");
}
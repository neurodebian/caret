/* 
 * File:   dateandtime.h
 * Author: john
 *
 * Created on June 23, 2009, 11:49 AM
 */

#ifndef  __DATE_AND_TIME_H__
#define	__DATE_AND_TIME_H__

#include <QString>

class DateAndTime {
   public:
      // get the date and time in a string
      static QString getDateAndTimeAsString();

      // get the date and time in a string for naming
      static QString getDateAndTimeForNaming();

      // get the date in a string for naming
      static QString getDateForNaming();

   private:

};

#endif	/* __DATE_AND_TIME_H__ */


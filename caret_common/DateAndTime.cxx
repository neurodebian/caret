/* 
 * File:   dateandtime.cxx
 * Author: john
 * 
 * Created on June 23, 2009, 11:49 AM
 */

#include "DateAndTime.h"

#include <QChar>
#include <QDateTime>

/**
 * Get the current date and time in a string.  This methods returns
 * a format that should not crash like the locale date format does if
 * written to a text file.
 *
 * Returned date is in form:
 *   23 Jun 2009 11:57:13
 */
QString 
DateAndTime::getDateAndTimeAsString()
{
   QString s = QDateTime::currentDateTime().toString(Qt::DefaultLocaleShortDate);
   return s;
}

/**
 * Get the current date and time in a string.  This methods returns
 * a format that should not crash like the locale date format does if
 * written to a text file.
 *
 * Returned date is in form:
 *   23_Jun_2009_11_57_13
 */
QString
DateAndTime::getDateAndTimeForNaming()
{
   QString s = QDateTime::currentDateTime().toString(Qt::DefaultLocaleShortDate);

   for (int i = 0; i < s.length(); i++) {
      if (s.at(i).isLetterOrNumber() == false) {
         s[i] = '_';
      }
   }
   
   return s;
}

/**
 * get the date in a string for naming.
 */
QString
DateAndTime::getDateForNaming()
{
   QString s = QDate::currentDate().toString(Qt::DefaultLocaleShortDate);

   for (int i = 0; i < s.length(); i++) {
      if (s.at(i).isLetterOrNumber() == false) {
         s[i] = '_';
      }
   }

   return s;
}


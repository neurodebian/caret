/* 
 * File:   XmlGenericWriterAttributes.h
 * Author: john
 *
 * Created on November 6, 2009, 10:48 AM
 */

#ifndef __XML_GENERIC_WRITER_ATTRIBUTES_H__
#define	__XML_GENERIC_WRITER_ATTRIBUTES_H__

#include <QString>
#include <QVector>

/**
 * Maintains attribute for XML Generic Writer.
 */
class XmlGenericWriterAttributes {

public:
   /**
    * Constructor.
    */
   XmlGenericWriterAttributes() {
      this->clear();
   }

   /**
    * Clear the attributes.
    */
   void clear() {
      this->names.clear();
      this->values.clear();
   }

   /**
    * Add an attribute.
    *
    * @param name - Name of attribute.
    * @param value - Value of attribute.
    */
   void addAttribute(QString name, QString value) {
      names.append(name);
      values.append(value);
   }

   /**
    * Add an attribute.
    *
    * @param name - Name of attribute.
    * @param value - Value of attribute.
    */
   void addAttribute(QString name, char value) {
      names.append(name);
      values.append(QString::number(value));
   }

   /**
    * Add an attribute.
    *
    * @param name - Name of attribute.
    * @param value - Value of attribute.
    */
   void addAttribute(QString name, short value) {
      names.append(name);
      values.append(QString::number(value));
   }

   /**
    * Add an attribute.
    *
    * @param name - Name of attribute.
    * @param value - Value of attribute.
    */
   void addAttribute(QString name, int value) {
      names.append(name);
      values.append(QString::number(value));
   }

   /**
    * Add an attribute.
    *
    * @param name - Name of attribute.
    * @param value - Value of attribute.
    */
   void addAttribute(QString name, long value) {
      names.append(name);
      values.append(QString::number(value));
   }

   /**
    * Add an attribute.
    *
    * @param name - Name of attribute.
    * @param value - Value of attribute.
    */
   void addAttribute(QString name, float value) {
      names.append(name);
      values.append(QString::number(value, 'f'));
   }

   /**
    * Add an attribute.
    *
    * @param name - Name of attribute.
    * @param value - Value of attribute.
    */
   void addAttribute(QString name, double value) {
      names.append(name);
      values.append(QString::number(value, 'f'));
   }

   /**
    * Get the number of attributes.
    *
    * @return Number of attributes.
    */
   int getNumberOfAttributes() {
      return this->names.size();
   }

   /**
    * Get the name of an attribute.
    *
    * @param index - index of attribute.
    *
    * @return Name of attribute at index.
    */
   QString getAttributeName(int index) {
      return this->names.at(index);
   }

   /**
    * Get the value of an attribute.
    *
    * @param index - index of attribute.
    *
    * @return Value of attribute at index.
    */
   QString getAttributeValue(int index) {
      return this->values.at(index);
   }

private:
   /** attribute names. */
   QVector<QString> names;

   /** attribute values. */
   QVector<QString> values;
};


#endif	/* __XML_GENERIC_WRITER_ATTRIBUTES_H__ */


/* 
 * File:   XmlGenericWriter.h
 * Author: john
 *
 * Created on November 6, 2009, 10:22 AM
 */

#ifndef __XML_GENERIC_WRITER_H__
#define	__XML_GENERIC_WRITER_H__

#include <QStack>
#include <QString>

#include "FileException.h"
#include "XmlGenericWriterAttributes.h"

class QTextStream;
class XmlGenericWriterAttributes;

/**
 * Writes XML to a writer with indentation (pretty printing).
 * Similar to StAX.
 */
class XmlGenericWriter {

public:
   /**
    * Constructor.
    *
    * @param writer - Writer to which XML is written.
    */
   XmlGenericWriter(QTextStream& writerIn);

   /**
    * Write the XML Start Document.
    *
    * @param xmlVersion - Version number, eg: "1.0".
    * @throws IOException if an I/O error occurs.
    */
   void writeStartDocument(QString xmlVersion) throw (FileException);

   /**
    * Writes a start tag to the output.
    *
    * @param localName - local name of tag to write.
    * @throws IOException if an I/O error occurs.
    */
   //void writeStartDocument(const char* localName) throw(FileException) {
   //   QString ln = localName;
   //   this->writeStartDocument(ln);
   //}

   /**
    * Write the XML Start document.
    *
    * @throws IOException if an I/O error occurs.
    */
   void writeStartDocument() throw (FileException);

   /**
    * Write a DTD section.
    *
    * @param rootTag - the root tag of the XML document.
    * @param dtdURL - URL of DTD.
    * @throws IOException if an I/O error occurs.
    */
   void writeDTD(QString rootTag, QString dtdURL) throw(FileException);
   /**
    * Closes any start tags and writes corresponding end tags.
    * @throws IOException if an I/O error occurs.
    */
   void writeEndDocument() throw(FileException);

   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeElementCharacters(const QString localName, const float f);

   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeElementCharacters(const QString localName, const float* values, const int num);
   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeElementCharacters(const QString localName, const int value);

   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeElementCharacters(const QString localName, const int* values, const int num);

   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   //void writeElementCharacters(const char* localName, const char* text) {
   //   this->writeElementCharacters(localName, text);
   //}

   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeElementCharacters(QString localName, QString text)
                                                    throw(FileException);

   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   //void writeElementCharacters(const char* localName, QString text)
   //                                                 throw(FileException) {
   //   QString ln = localName;
   //   this->writeElementCharacters(ln, text);
   //}
   
   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   //void writeElementCData(const char* localName, const char* text) {
   //   this->writeElementCData(localName, text);
   //}

   /**
    * Write an element on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   //void writeElementCData(const char* localName, QString text) {
   //   this->writeElementCData(localName, text);
   //}

   /**
    * Write a CData section on one line.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeElementCData(QString localName, QString text)
                                                    throw(FileException);

   /**
    * Write a CData section on one line with attributes.
    *
    * @param localName - local name of tag to write.
    * @param attributes - attribute for element.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeElementCData(QString localName,
                                 XmlGenericWriterAttributes& attributes,
                                 QString text)
                                                    throw(FileException);

   /**
    * Write an element with no spacing between start and end tags.
    *
    * @param localName - local name of tag to write.
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeElementNoSpace(QString localName, QString text)
                                                    throw(FileException);
   /**
    * Writes a start tag to the output.
    *
    * @param localName - local name of tag to write.
    * @throws IOException if an I/O error occurs.
    */
   //void writeStartElement(const char* localName) throw(FileException) {
   //   QString ln = localName;
   //   this->writeStartElement(ln);
   //}
   /**
    * Writes a start tag to the output.
    *
    * @param localName - local name of tag to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeStartElement(QString localName) throw(FileException);

   /**
    * Writes a start tag to the output.
    *
    * @param localName - local name of tag to write.
    * @param attributes - attributes for start tag
    * @throws IOException if an I/O error occurs.
    */
   void writeStartElement(QString localName,
                     XmlGenericWriterAttributes& attributes) throw(FileException);

   /**
    * Writes an end tag to the output.
    *
    * @throws IOException if an I/O error occurs
    */
   void writeEndElement() throw(FileException);

   /**
    * Writes a CData section.
    *
    * @param data - data to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeCData(QString data) throw(FileException);

   /**
    * Writes text to the output.
    *
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeCharacters(QString text) throw(FileException);

   /**
    * Writes text with indentation to the output.
    *
    * @param text - text to write.
    * @throws IOException if an I/O error occurs.
    */
   void writeCharactersWithIndent(QString text) throw(FileException);

   void setNumberOfDecimalPlaces(int decimals);

private:
   /**
    * Write indentation spaces.
    *
    * @throws IOException if an I/O error occurs.
    */
   void writeIndentation() throw(FileException);

   /** The writer to which XML is written */
   QTextStream& writer;

   /** The indentation amount for new element tags. */
   int indentationSpaces;

   /** The element stack used for closing elements. */
   QStack<QString> elementStack;

   int numberOfDecimalPlaces;
};


#endif	/* __XML_GENERIC_WRITER_H__ */


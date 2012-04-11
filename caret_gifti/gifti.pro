DEFINES += CARET_QT4
QT += network opengl xml

TARGET   = Gifti
!vs:TEMPLATE = lib
vs:TEMPLATE=vclib
CONFIG  += staticlib
INCLUDEPATH += .
dll {
	CONFIG -= staticlib
	CONFIG += plugin
}



# Update include paths
#

INCLUDEPATH += ../caret_common


DEPENDPATH += ../caret_common


# Input
HEADERS += AbstractFile.h \
      CommaSeparatedValueFile.h \
      GiftiCommon.h \
      GiftiDataArray.h \
      GiftiDataArrayFile.h \
      GiftiDataArrayFileSaxReader.h \
      GiftiDataArrayFileStreamReader.h \
      GiftiLabelTable.h \
      GiftiMatrix.h \
      GiftiMetaData.h \
      GiftiNodeDataFile.h \
     XhtmlTableExtractorFile.h \
      XmlGenericWriter.h \
      XmlGenericWriterAttributes.h \
    GiftiDataArrayReadListener.h \
    nifti1.h

SOURCES += AbstractFile.cxx \
      GiftiCommon.cxx \
      GiftiDataArray.cxx \
      GiftiDataArrayFile.cxx \
      GiftiDataArrayFileSaxReader.cxx \
      GiftiDataArrayFileStreamReader.cxx \
      GiftiLabelTable.cxx \
      GiftiMatrix.cxx \
      GiftiMetaData.cxx \
      GiftiNodeDataFile.cxx \
      XhtmlTableExtractorFile.cxx \
      XmlGenericWriter.cxx

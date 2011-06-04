TARGET       = CaretCifti
CONFIG  += staticlib 
INCLUDEPATH += .
!vs:TEMPLATE = lib
win32:vs:TEMPLATE=vclib

include(../caret_qmake_include.pro)
dll {
	CONFIG -= staticlib
	CONFIG += plugin
}
# Input

SOURCES = CiftiFile.cxx \
          CiftiFileException.cxx \
          CiftiMatrix.cxx \
          CiftiXMLReader.cxx \
          CiftiXMLWriter.cxx \
          CiftiXMLElements.cxx \
          CiftiXML.cxx \
          Nifti2Header.cxx

HEADERS = CiftiFile.h \
          CiftiFileException.h \
          CiftiMatrix.h \
          CiftiXMLReader.h \
          CiftiXMLWriter.h \
          CiftiXMLElements.h \
          CiftiXML.h \
          nifti1.h \
          nifti2.h \
          Nifti2Header.h
          







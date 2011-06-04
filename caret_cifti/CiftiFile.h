/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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

#ifndef __CIFTI_FILE
#define __CIFTI_FILE
#include <QtCore>
#include "nifti2.h"
#include "iostream"
#include "CiftiFileException.h"
#include "Nifti2Header.h"
#include "CiftiXML.h"
#include "CiftiMatrix.h"


class CiftiFile {
public:
   CiftiFile(CACHE_LEVEL clevel = IN_MEMORY) throw (CiftiFileException);
   CiftiFile(const QString &fileName,CACHE_LEVEL clevel = IN_MEMORY) throw (CiftiFileException);
   virtual void openFile(const QString &fileName) throw (CiftiFileException);
   virtual void openFile(const QString &fileName, CACHE_LEVEL clevel) throw (CiftiFileException);
   virtual void writeFile(const QString &fileName) const throw (CiftiFileException);
   virtual void setHeader(const Nifti2Header &header) throw (CiftiFileException);
   virtual Nifti2Header * getHeader() throw (CiftiFileException);
   virtual void getHeader(Nifti2Header &header) throw (CiftiFileException);
   virtual void setCiftiXML(CiftiXML &ciftixml) throw (CiftiFileException);
   virtual CiftiXML * getCiftiXML() throw (CiftiFileException);   
   virtual void getCiftiXML(CiftiXML &xml) throw (CiftiFileException);
   virtual void setCiftiMatrix(CiftiMatrix &matrix) throw (CiftiFileException);
   virtual CiftiMatrix * getCiftiMatrix() throw (CiftiFileException);
   virtual ~CiftiFile();
protected:
   virtual void init();
   virtual void readHeader() throw (CiftiFileException);   
   virtual void readCiftiMatrix() throw (CiftiFileException);
   bool m_copyMatrix;//determines whether matrix is copied, when retrieved or set.  Avoiding a copy allows for memory usage to be halved, but has the caveats of allowing
                      //other classes to access what should be private data
                      //in the event that this flag is true, the internal pointer to the cifti matrix is set to null, and the file is read again upon access, so don't use this as a hack to meddle
                      //with internal data.  Doing will mean that each copy of the matrix that is retrieved will still be unique
   
   CACHE_LEVEL m_clevel;
   QFile m_inputFile;
   Nifti2Header *m_nifti2Header;
   CiftiMatrix *m_matrix;
   CiftiXML *m_xml;   
   bool m_swapNeeded;
};   

#endif //__CIFTI_FILE
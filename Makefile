SHELL	= /bin/sh

PROGS	= caret \
	  caret_command \
	  caret_edit 

LIBS	= \
	  caret_statistics \
	  caret_common \
	  caret_files \
	  caret_brain_set \
	  caret_command_operations \
	  caret_uniformize \
	  caret_widgets \
	  caret_vtk4_classes

DIRS	= $(LIBS) $(PROGS)

all:
	@echo ""
	@echo ""
	@echo "Use \"make qmake-static\" to create the makefiles with static libraries."
	@echo "Use \"make qmake-dynamic\" to create the makefiles with dynamic libraries."
	@echo "Use \"make build\" to build the libraries and executables."
	@echo "Use \"make clean\" to remove executables, libraries, and objects."
	@echo ""
	@echo "Use \"make doc\" to build the Doxygen generated documentation."
	@echo ""

doc:
	doxygen Doxyfile

build:
	@for i in ${DIRS} ; do \
	   echo "making " $$i ; \
	   cd $$i ; \
	   make ; \
	   cd .. ; \
	done

build8:
	@for i in ${DIRS} ; do \
	   echo "making " $$i ; \
	   cd $$i ; \
	   make -j 8; \
	   cd .. ; \
	done

build4:
	@for i in ${DIRS} ; do \
	   echo "making " $$i ; \
	   cd $$i ; \
	   make -j 4; \
	   cd .. ; \
	done

build2:
	@for i in ${DIRS} ; do \
	   echo "making " $$i ; \
	   cd $$i ; \
	   make -j 2; \
	   cd .. ; \
	done

clean:
	@for i in ${DIRS} ; do \
	   echo "cleaning " $$i ; \
	   cd $$i ; \
	   make clean ; \
	   cd .. ; \
	done

qmake-static:
	@for i in ${LIBS} ; do \
	   echo "creating makefile for static libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake ; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake ; \
	   cd .. ; \
	done

qmake-dynamic:
	@for i in ${LIBS} ; do \
	   echo "creating makefile for dynamic libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG+=dll"; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake ; \
	   cd .. ; \
	done
	
rebuild:
	make qmake-static
	make build
	
rebuild8:
	make qmake-static
	make build8
	
rebuild4:
	make qmake-static
	make build4
	
rebuild2:
	make qmake-static
	make build2
	
check_env:
	@cnt=""
	@if [ "${VTK_INC_DIR}" == "" ]; then \
          echo "Environment variable VTK_INC_DIR not set" ; \
	  cnt="1" ; \
	fi ;  
	@if [ "${VTK_LIB_DIR}" == "" ]; then \
          echo "Environment variable VTK_LIB_DIR not set" ; \
	  cnt="1" ; \
	fi ;  
	@if [ "${NETCDF_INC_DIR}" == "" ]; then \
          echo "Environment variable NETCDF_INC_DIR not set" ; \
	  cnt="1" ; \
	fi ;  
	@if [ "${NETCDF_LIB_DIR}" == "" ]; then \
          echo "Environment variable NETCDF_LIB_DIR not set" ; \
	  cnt="1" ; \
	fi ;  

#	@if [ "$cnt" != "" ]; then \
#	   echo "Required environment variables are not set" ; \
#	fi ; 

test2:
	@for i in ${DIRS} ; do \
	   echo "testing..." ; \
	   cd $$i ; \
	   pwd ; \
	   cd .. ; \
	done

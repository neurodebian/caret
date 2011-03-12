SHELL	= /bin/sh

PROGS_CMD = caret_command

PROGS_GUI = \
	  caret \
	  caret_edit 

PROGS	= $(PROGS_CMD) \
	  $(PROGS_GUI)

UBUNTULIBS	= \
	  caret_statistics \
	  caret_common \
	  caret_files \
	  caret_brain_set \
	  caret_command_operations \
	  caret_uniformize \
	  caret_widgets

LIBS = $(UBUNTULIBS) caret_vtk4_classes

DIRS	= $(LIBS) $(PROGS)
DIRS_NO_GUI = $(LIBS) $(PROGS_CMD)
UBUNTUDIRS = $(UBUNTULIBS) $(PROGS)

all:
	@echo ""
	@echo ""
	@echo "Use \"make qmake-debug\"\n     to create the makefiles with static libraries for debug."
	@echo "Use \"make qmake-debug-dynamic\"\n     to create the makefiles with dynamic libraries for debug."
	@echo "Use \"make qmake-profile\"\n     to create the makefiles with static libraries for profiling/debugging."
	@echo "Use \"make qmake-profile-dynamic\" \n     to create the makefiles with dynamic libraries for profiling/debugging."
	@echo "Use \"make qmake-release\"\n     to create the makefiles with static libraries for release."
	@echo "Use \"make qmake-release-dynamic\"\n     to create the makefiles with dynamic libraries for release."
	@echo "Use \"make build\"\n     to build the libraries and executables."
	@echo "Use \"make clean\"\n     to remove executables, libraries, and objects."
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

build16:
	@for i in ${DIRS} ; do \
	   echo "making " $$i ; \
	   cd $$i ; \
	   make -j 16; \
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



build-no-gui:
	@for i in ${DIRS_NO_GUI} ; do \
	   echo "making " $$i ; \
	   cd $$i ; \
	   make ; \
	   cd .. ; \
	done

build4-no-gui:
	@for i in ${DIRS_NO_GUI} ; do \
	   echo "making " $$i ; \
	   cd $$i ; \
	   make -j 4; \
	   cd .. ; \
	done


clean:
	@for i in ${DIRS} ; do \
	   echo "cleaning " $$i ; \
	   cd $$i ; \
	   make clean ; \
	   cd .. ; \
	done

distclean:
	@for i in ${DIRS} ; do \
	   echo "cleaning " $$i ; \
	   cd $$i ; \
	   make distclean ; \
	   cd .. ; \
	done

qmake-debug:
	@for i in ${LIBS} ; do \
	   echo "creating debug makefile for static libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG += debug"; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating debug makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG += debug"; \
	   cd .. ; \
	done

qmake-debug-dynamic:
	@for i in ${LIBS} ; do \
	   echo "creating makefile for static libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG += debug dll"; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG += debug dll"; \
	   cd .. ; \
	done

qmake-release:
	@for i in ${LIBS} ; do \
	   echo "creating makefile for static libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG -= debug" "CONFIG += release"; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG -= debug" "CONFIG += release"; \
	   cd .. ; \
	done

qmake-release-dynamic:
	@for i in ${LIBS} ; do \
	   echo "creating makefile for dynamic libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG -= debug" "CONFIG += release dll" "CONFIG -= staticlib"; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG -= debug" "CONFIG += release dll"; \
	   cd .. ; \
	done

qmake-profile:
	@for i in ${LIBS} ; do \
	   echo "creating makefile for dynamic libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG += debug profile"; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG += debug profile"; \
	   cd .. ; \
	done
        
qmake-profile-dynamic:
	@for i in ${LIBS} ; do \
	   echo "creating makefile for dynamic libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG += debug profile dll" "CONFIG -= staticlib"; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG += debug profile dll"; \
	   cd .. ; \
	done

ubuntu:
	@for i in ${UBUNTULIBS} ; do \
	   echo "creating makefile for dynamic libs with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG -= debug" "CONFIG += release ubuntu dll"; \
	   cd .. ; \
	done
	@for i in ${PROGS} ; do \
	   echo "creating makefile with qmake for " $$i ; \
	   cd $$i ; \
	   qmake "CONFIG -= debug" "CONFIG += release ubuntu dll"; \
	   cd .. ; \
	done
	
rebuild:
	make qmake-debug
	make build
	
rebuild16:
	make qmake-debug
	make build16

rebuild8:
	make qmake-debug
	make build8
	
rebuild4:
	make qmake-debug
	make build4
	
rebuild2:
	make qmake-debug
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

test:
	echo "Testing" ; \
	cd testing ; \
	./run_test.sh ; \
	cd .. ;

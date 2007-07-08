SHELL=/bin/sh
HOST_TYPE_NAME=`uname -s`



all:
	@case $(HOST_TYPE_NAME) in \
	   IRIX|IRIX64) \
	      echo "SGI SYSTEM !!!" ; \
	      make -f Makefile.sgi ;; \
	   Linux) \
	      echo "Linux SYSTEM !!!" ; \
	      make -f Makefile.linux ;; \
	   FreeBSD) \
	      echo "FreeBSD SYSTEM !!!" ; \
	      make -f Makefile.FreeBSD ;; \
	   SunOS) \
	      echo "Sun SYSTEM !!!" ; \
	      make -f Makefile.sun ;; \
	   Darwin) \
	      echo "Mac OSX SYSTEM !!!" ; \
	      make -f Makefile.osx ;; \
	   *) \
	      echo "ERROR: Unknown System " $(HOST_TYPE_NAME) ; \
	      echo "   You will need to create a new Makefile" ; \
	esac

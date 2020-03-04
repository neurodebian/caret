# caret
Debian packaging of good old caret


# Building on Ubuntu

## pre-patch

The debianization requires that all modifications to the upstream source be put in patches. If changes are made to files
*outside of the debian/ folder*, then the *patch* should be added to git, but the patched file should remain unmodified. 
The *dquilt patch -a* command applies the patch and restores the modifications. 

Changes made to files *inside the debian/ folder and subfolders* should be committed directly - do not patch files in debian/!

I've done away with this nonsense and hassle with the patched branches - see below. These notes are for posterity....

1. Build deps
1. git clone
1. dquilt patch -a
1. debian/rules build | tee build.log 2>&1
1. dpkg-buildpackage -b -us -uc -nc -rfakeroot
 * -b : binary package, please
 * -us : unsigned package
 * -uc : unsigned changes file
 * -nc : no clean - saves a LOT of time in building. Without this, build will clean first.
 * -rfakeroot : needed to build deb package right
 
 
## post-patch branches
 
These branches (*splash-patched* and beyond) have the patches applied and changes committed. For one thing this makes development on windows a lot easier. For another thing, why not? Upstream caret development is basically dead, so preserving these patches in the event some upstream change makes its way down here to caret is, well, a waste IMHO.


## Building windows packages

1. Qt 4.8 - have to build this to make it all work. I found a site that patched a version of the source. 
   * git clone https://github.com/scharsig/Qt.git
   * run MSVC "vcvars32.bat" to set environment for building 32 bit stuff (though I think the platform arg below takes care of that)
   * cd qt-4.8.7-vs2017
   * configure.exe -release -opensource -static -platform win32-msvc2017
   * nmake
1. VTK
1. QWT
   * download qwt 5.2.3 source (compatible with qt4)
   * set PATH to use qmake from Qt4.8 (32 bit)
   * nmake install puts all in c:\Qwt-5.2.3 (configurable in qwtConfig.pri)
1. netCDF - Download binaries for 32-bit netCDF-3 (legacy) [here](https://www.unidata.ucar.edu/software/netcdf/docs/winbin.html "netCDF windows binaries")
1. HDF5 - must register at [HDF site](https://www.hdfgroup.org/) before downloading installer.
1. zlib - build from [source](https://www.zlib.net/)
   * Must build INSTALL project as administrator
1. nifti
   * Changes in cmake config for main and "znz"
      * ZLIB_LIBRARY_DEBUG="C:/Program Files (x86)/zlib/lib/zlibstatic.lib" 
      * ZLIB_LIBRARY_RELEASE="C:/Program Files (x86)/zlib/lib/zlibstatic.lib" 
      * BUILD_TESTING="0" 
      * ZLIB_INCLUDE_DIR "C:/Program Files (x86)/zlib/include" 
      * FIND_PACKAGE(ZLIB REQUIRED) (this line was present)
      * include_directories(${ZLIB_INCLUDE_DIRS})
      * set(LIBS ${LIBS} ${ZLIB_LIBRARIES})
   * Must build "INSTALL" project as Administrator (start vs as Admin)
1. libminc
   * git clone https://github.com/BIC-MNI/libminc.git 
   * run cmake-gui, fix these values:
      * LIBMINC_MINC1_SUPPORT - check this box
      * LIBMINC_USE_SYSTEM_NIFTI - check this box (requires setting the 4 vals below)
      * NIFTI_INCLUDE_DIR (location of "nifticdf.h")
      * NIFTI_LIBRARY (location of "nifticdf.lib")
      * ZNZ_INCLUDE_DIR (location of "znzlib.h")
      * ZNZ_LIBRARY (location of "znz.lib")
   * Must build INSTALL as admin

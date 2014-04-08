#!/bin/sh

#
# This script copies the mac icon into
# into the Mac App's Resources directory.
#

exeName=$1
iconName=$2

for buildType in Debug/ Release/ RelWithDebInfo/ MinRelSize/ ""
do
   echo "BUILD TYPE ${buildType}"
   appName=${buildType}${exeName}.app/Contents/MacOS/${exeName}
   echo "App ${appName}"

   if [ -f ${appName} ] ; then
      echo "App ${appName} EXISTS!"

      if [ ! -d ${buildType}${exeName}.app/Contents/Resources ] ; then
         echo "Creating resources directory"
         mkdir ${buildType}${exeName}.app/Contents/Resources
      fi

      cp -v $iconName ${buildType}${exeName}.app/Contents/Resources/
   fi
done



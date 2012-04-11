#!/bin/bash
# Repackage Caret upstream sources. Determine version, strip unnecessary
# files, zip->tar.gz
#
# Usage:
#   repack.sh <mcaret_source.zip>
#

set -e

ORIGSRC=$1
if [ -z "$ORIGSRC" ]; then
	echo "No upstream sources given."
	exit 1
fi

CURDIR=$(pwd)
WDIR=$(mktemp -d)
SUBDIR=caret5_source

# put upstream sources into working dir
ORIGSRC_PATH=$(readlink -f ${ORIGSRC})
cd $WDIR
unzip -q $ORIGSRC_PATH

UPSTREAM_VERSION_STRING=$(cat $WDIR/$SUBDIR/caret_common/CaretVersion.h | grep getCaretVersionAsString | cut -d '"' -f 2,2)
UPSTREAM_VERSION=$(python -c "import re; a=str($UPSTREAM_VERSION_STRING); print re.sub(r'([0-9])', r'\1.', a.replace('.',''))[:-1]")
ORIG_VERSION="$UPSTREAM_VERSION~dfsg.1"

echo "Determined version: $UPSTREAM_VERSION"
echo "Debian orig version: $ORIG_VERSION"

# remove non-free pieces
rm -rf $SUBDIR/caret_vtk4_classes
# remove rubbish
rm -f $SUBDIR/caret_brain_set/stE0IX1E
rm -f $SUBDIR/caret_statistics/sttxIrBF

mv $SUBDIR caret-$ORIG_VERSION.orig
tar czf caret_$ORIG_VERSION.orig.tar.gz caret-$ORIG_VERSION.orig
mv caret_$ORIG_VERSION.orig.tar.gz $CURDIR

# clean working dir
rm -rf $WDIR

echo "Tarball is at: $CURDIR/caret_$ORIG_VERSION.orig.tar.gz"


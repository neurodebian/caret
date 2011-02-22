#!/bin/sh
#
# Test rending of surface shape data
#

#
# Use black background
#
caret_command -preferences-file-settings  \
         -background-color  0  0  0 \

#
# Show the scene
#
caret_command -show-scene \
    caret_file_check.spec \
   Human.crap.R.test.scene \
   1 \
   -image-file image1-test.jpg   1 

#
# Compare to the correct image
#
caret_command -image-compare \
   image1-test.jpg \
   image1-correct.jpg \
   -tol 3

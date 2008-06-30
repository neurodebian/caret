#!/usr/bin/python
#
# Create an RGB paint file with random colors
#
import os
import random
import sys

#
# Name of RGB Paint file and number of nodes in the file
#
rgbPaintFileName = "random.RGB_paint"
numberOfNodes    = 71723

#
# Create an RGB paint file with random colors
#
file = open(rgbPaintFileName, 'w')
file.write("tag-version " + str(2) + "\n")
file.write("tag-number-of-nodes " + str(numberOfNodes) + "\n")
file.write("tag-number-of-columns  " + str(1) + "\n")
file.write("tag-BEGIN-DATA\n");
for i in range(numberOfNodes): 
   node  = str(i)
   red   = str(random.random() * 255.0)
   green = str(random.random() * 255.0)
   blue  = str(random.random() * 255.0)
   line = node + " " + red + " " + green + " " + blue + "\n"
   file.write(line)

file.close()


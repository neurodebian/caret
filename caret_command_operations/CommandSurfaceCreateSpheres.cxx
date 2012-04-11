/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#include "CommandSurfaceCreateSpheres.h"
#include "FileFilters.h"
#include "ProgramParameters.h"
#include "ScriptBuilderParameters.h"
#include "BrainModelSurfacePolyhedronNew.h"
#include "BrainSet.h"

#include <cmath>

using namespace std;

/**
 * constructor.
 */
CommandSurfaceCreateSpheres::CommandSurfaceCreateSpheres()
   : CommandBase("-surface-create-spheres",
                 "CREATE LEFT AND RIGHT SPHERES")
{
}

/**
 * destructor.
 */
CommandSurfaceCreateSpheres::~CommandSurfaceCreateSpheres()
{
}

/**
 * get the script builder parameters.
 */
void 
CommandSurfaceCreateSpheres::getScriptBuilderParameters(ScriptBuilderParameters& paramsOut) const
{
   paramsOut.clear();
   paramsOut.addInt("approximate node count", 30000, 4);
}

/**
 * get full help information.
 */
QString 
CommandSurfaceCreateSpheres::getHelpInformation() const
{
   QString helpInfo =
      (indent3 + getShortDescription() + "\n"
       + indent6 + parameters->getProgramNameWithoutPath() + " " + getOperationSwitch() + "  \n"
       + indent9 + "<desired node count>\n"
       + indent9 + "<right coord output>\n"
       + indent9 + "<right topo output>\n"
       + indent9 + "<left coord output>\n"
       + indent9 + "<left topo output>\n"
       + indent9 + "\n"
       + indent9 + "Create matched left and right spherical surfaces, with node count\n"
       + indent9 + "as close to <desired node count> as possible by subdividing the\n"
       + indent9 + "faces of an icosahedron.  The surface is generated in a way that\n"
       + indent9 + "minimizes the variability of node surface areas.\n"
       + indent9 + "\n");
      
   return helpInfo;
}

/**
 * execute the command.
 */
void 
CommandSurfaceCreateSpheres::executeCommand() throw (BrainModelAlgorithmException,
                                     CommandException,
                                     FileException,
                                     ProgramParametersException,
                                     StatisticException)
{
    int numNodesDesired = parameters->getNextParameterAsInt("desired node count");
    QString rightCoordOutName = parameters->getNextParameterAsString("right coord out");
    QString rightTopoOutName = parameters->getNextParameterAsString("right topo out");
    QString leftCoordOutName = parameters->getNextParameterAsString("left coord out");
    QString leftTopoOutName = parameters->getNextParameterAsString("left topo out");
    if (numNodesDesired < 4)
    {
         throw CommandException("Invalid number of nodes: "
                             + QString::number(numNodesDesired));
    }
    int divisions = -1, lastnodes = -1000, mynodes, mytris, numNodes;
    for (int i = 0; i < 10000; ++i)//10363 divisions overflows an int32 in number of triangles, tighten this sanity check?
    {
        BrainModelSurfacePolyhedronNew::getNumberOfNodesAndTrianglesFromDivisions(i, mynodes, mytris);
        if (abs(numNodesDesired - lastnodes) < abs(numNodesDesired - mynodes))
        {
            divisions = i - 1;
            numNodes = lastnodes;
            std::cout << "Closest divided icosahedron has " << lastnodes << " nodes." << std::endl;
            break;
        }
        lastnodes = mynodes;
    }
    if (divisions == -1)
    {
        throw CommandException("Number of nodes too large: "
                             + QString::number(numNodesDesired));
    }
    BrainSet mySet;
    BrainModelSurfacePolyhedronNew myPolyNew(&mySet, divisions);
    myPolyNew.execute();
    BrainModelSurface* rightSurf = mySet.getBrainModelSurface(0);
    if (rightSurf->getNumberOfNodes() != numNodes)
    {
        throw CommandException("program error, number of nodes does not match expected value");
    }
    CoordinateFile leftCoord = *(rightSurf->getCoordinateFile());
    TopologyFile leftTopo = *(rightSurf->getTopologyFile());
    float* tempCoords = new float[numNodes * 3];
    leftCoord.getAllCoordinates(tempCoords);
    for (int i = 0; i < numNodes * 3; i += 3)
    {
        tempCoords[i] = -tempCoords[i];//flip the x coordinate
    }
    leftCoord.setAllCoordinates(tempCoords);
    delete[] tempCoords;
    leftTopo.flipTileOrientation();
    rightSurf->getCoordinateFile()->removeHeaderTag("topo_file");//blow away the hidden topo association, if it exists, which it shouldn't (it doesn't have a file name)
    leftCoord.removeHeaderTag("topo_file");
    rightSurf->getCoordinateFile()->writeFile(rightCoordOutName);
    rightSurf->getTopologyFile()->writeFile(rightTopoOutName);
    leftCoord.writeFile(leftCoordOutName);
    leftTopo.writeFile(leftTopoOutName);
}

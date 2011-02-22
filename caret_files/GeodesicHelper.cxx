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

#include "CoordinateFile.h"
#include "GeodesicHelper.h"
#include "TopologyFile.h"
#include "TopologyHelper.h"
#include <iostream>
#include <QMutexLocker>

GeodesicHelper::GeodesicHelper(const CoordinateFile* coordsIn, const TopologyFile* topoFileIn)
{
   marked = NULL;
   if (coordsIn->getNumberOfNodes() != topoFileIn->getNumberOfNodes())
   {
      numNodes = 0;
      return;
   }//sanity checks passed
   bool found;
   const TopologyHelper* topoHelpIn = topoFileIn->getTopologyHelper(false, true, false);
   int i, j, k, numChanged, myneigh, myneigh2, coordbase, neigh2base, neighbase, sidebase;
   float ep[3], ns[3], d[3], ne[3], tempvec[3], tempvec2[3], fullmag, partmag, dmag, tempf, tempf2;
   numNodes = coordsIn->getNumberOfNodes();
   //allocate
   float* coords = new float[3 * numNodes];
   output = new float[numNodes];
   numNeighbors = new int[numNodes];
   marked = new int[numNodes];
   changed = new int[numNodes];
   parent = new int[numNodes];
   nodeNeighbors = new int*[numNodes];
   distances = new float*[numNodes];
   coordsIn->getAllCoordinates(coords);//get coords
   for (i = 0; i < numNodes; ++i)
   {//get neighbors
      const int* neighbors = topoHelpIn->getNodeNeighbors(i, numNeighbors[i]);
      nodeNeighbors[i] = new int[numNeighbors[i]];
      distances[i] = new float[numNeighbors[i]];
      coordbase = i * 3;
      for (j = 0; j < numNeighbors[i]; ++j)
      {
         nodeNeighbors[i][j] = neighbors[j];
         coordDiff(coords + coordbase, coords + neighbors[j] * 3, tempvec);
         distances[i][j] = std::sqrt(tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2]);//precompute for speed in calls
      }//so few floating point operations, this should turn out symmetric
      marked[i] = 0;//initialize
   }
   std::vector<int> tempneigh2;
   std::vector<float> tempdist2;
   nodeNeighbors2 = new int*[numNodes];
   numNeighbors2 = new int[numNodes];
   distances2 = new float*[numNodes];
   for (i = 0; i < numNodes; ++i)
   {//find shared neighbors, for smoothed dijkstra
      tempneigh2.clear();
      tempdist2.clear();
      numChanged = 0;//only tracks non-neighbors
      marked[i] = 1;
      coordbase = i * 3;
      for (j = 0; j < numNeighbors[i]; ++j)
      {
         marked[nodeNeighbors[i][j]] = 1;//use marked as scratch space
      }
      for (j = 0; j < numNeighbors[i]; ++j)
      {
         myneigh = nodeNeighbors[i][j];
         neighbase = myneigh * 3;
         for (k = 0; k < numNeighbors[myneigh]; ++k)
         {
            myneigh2 = nodeNeighbors[myneigh][k];
            if (marked[myneigh2] != 1)
            {//don't need bitwise, sets are mutually exclusive
               if (marked[myneigh2] > 1)
               {
                  if (myneigh2 < i)//find the symmetric value
                  {
                     found = false;
                     for (int m = 0; m < numNeighbors2[myneigh2]; ++m)
                     {
                        if (nodeNeighbors2[myneigh2][m] == i)
                        {
                           tempf = distances2[myneigh2][m];
                           found = true;
                           break;
                        }
                     }
                     if (!found)
                     {//was rejected by the previous (reverse) calculation, due to the tetrahedron being obtuse, so reject it now too
                        continue;
                     }
                  } else {
                     neigh2base = myneigh2 * 3;//uses a bunch of vector math/geometry to find the path across the two faces of the tetrahedron
                     sidebase = parent[myneigh2] * 3;//the crossing point of the path with the shared edge will be at the smallest separation between the euclidean path and the shared edge
                     coordDiff(coords + coordbase, coords + neigh2base, ep);//ep = vector from this neighbor to this node's parent
                     coordDiff(coords + sidebase, coords + neighbase, ns);//ns = vector from this node to shared neighbor of parent and this neighbor
                     coordDiff(coords + neigh2base, coords + neighbase, ne);//ne = vector from this node to this neighbor
                     crossProd(ep, ns, d);//d is a vector perpendicular to both: it is the direction of shortest distance between them
                     fullmag = normalize(d);//now a unit vector in the correct direction, fullmag is used later
                     dmag = dotProd(d, ne);//dmag is distance between the lines
                     d[0] *= dmag; d[1] *= dmag; d[2] *= dmag;//d is the vector between the lines, from ns to ep
                     tempvec[0] = ne[0] - d[0];
                     tempvec[1] = ne[1] - d[1];
                     tempvec[2] = ne[2] - d[2];
                     crossProd(tempvec, ns, tempvec2);//cannot use a vector as both input and output to crossprod
                     partmag = std::sqrt(tempvec2[0] * tempvec2[0] + tempvec2[1] * tempvec2[1] + tempvec2[2] * tempvec2[2]);
                     tempvec2[0] = coords[neigh2base] + ep[0] * partmag / fullmag - d[0];//this formula calculates the point on the opposite edge of the tetrahedron
                     tempvec2[1] = coords[neigh2base + 1] + ep[1] * partmag / fullmag - d[1];//that the path must pass through for shortest path
                     tempvec2[2] = coords[neigh2base + 2] + ep[2] * partmag / fullmag - d[2];//temporarily assumes tetrahedron is fully acute
                     coordDiff(tempvec2, coords + neighbase, tempvec);//so, let's check the range and make sure
                     tempf2 = normalize(ns);//tempf2 is the magnitude before normalization
                     tempf = dotProd(ns, tempvec);
                     if (tempf < tempf2 * 0.5f)
                     {
                        parent[myneigh2] = myneigh;//adjust parent to be the current 1hop neighbor if shortest path on faces is closer to it, just in case
                     }
                     if (tempf < 0)
                     {//intersection went past n away from s, discard, regular dijkstra will find this distance
                        continue;
                     } else if (tempf > tempf2) {//went past s away from n, ditto
                        continue;
                     }//tempvec2 should now be a point on the shared edge of the triangles, which the shortest path along those triangles passes through
                     coordDiff(coords + coordbase, tempvec2, tempvec);//the rest is straight lines, so euclidean and we're done
                     tempf = std::sqrt(tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2]);
                     coordDiff(tempvec2, coords + myneigh2 * 3, tempvec);
                     tempf += std::sqrt(tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2]);
                     coordDiff(coords + coordbase, coords + neigh2base, tempvec);
                     tempf2 = std::sqrt(tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2]);//straight line dist for comparison
                     if (tempf2 > tempf)
                     {//catch floating point inconsistencies
                        tempf = tempf2;
                     }
                  }
                  if (marked[myneigh2] == 2)
                  {
                     tempneigh2.push_back(myneigh2);
                     tempdist2.push_back(tempf);
                     marked[myneigh2] = tempneigh2.size() + 2;//encode position in tempneigh2 in the marked array, subtract 3 to retrieve
                  } else {//more than 2 ways to reach a 2 step neighbor...hopefully this won't be needed, because it doesn't try all combinations
                     if (tempf < tempdist2[marked[myneigh2] - 3])
                     {
                        tempdist2[marked[myneigh2] - 3] = tempf;
                     }
                  }
               } else {
                  marked[myneigh2] = 2;
                  parent[myneigh2] = myneigh;//repurpose parent array to store the other node that can reach the second hop neighbor
                  changed[numChanged++] = myneigh2;
               }
            }
         }
      }
      numNeighbors2[i] = tempneigh2.size();
      nodeNeighbors2[i] = new int[numNeighbors2[i]];
      distances2[i] = new float[numNeighbors2[i]];
      for (j = 0; j < numNeighbors2[i]; ++j)
      {
         nodeNeighbors2[i][j] = tempneigh2[j];
         distances2[i][j] = tempdist2[j];
      }
      for (j = 0; j < numNeighbors[i]; ++j)
      {
         marked[nodeNeighbors[i][j]] = 0;
      }
      for (j = 0; j < numChanged; ++j)
      {
         marked[changed[j]] = 0;
      }
      marked[i] = 0;
   }
}

void GeodesicHelper::getNodesToGeoDist(const int node, const float maxdist, std::vector<int>& nodesOut, std::vector<float>& distsOut, const bool smoothflag)
{//public methods sanity check, private methods process
   nodesOut.clear();
   distsOut.clear();
   if (node >= numNodes || maxdist < 0.0f || node < 0) return;
   QMutexLocker locked(&inUse);//let sanity checks go multithreaded, as if it mattered
   dijkstra(node, maxdist, nodesOut, distsOut, smoothflag);
}

void GeodesicHelper::getNodesToGeoDist(const int node, const float maxdist, std::vector<int>& nodesOut, std::vector<float>& distsOut, std::vector<int>& parentsOut, const bool smoothflag)
{//public methods sanity check, private methods process
   nodesOut.clear();
   distsOut.clear();
   if (node >= numNodes || maxdist < 0.0f || node < 0) return;
   QMutexLocker locked(&inUse);//we need the parents array to stay put, so don't scope this
   dijkstra(node, maxdist, nodesOut, distsOut, smoothflag);
   int mysize = (int)nodesOut.size();
   parentsOut.resize(mysize);
   for (int i = 0; i < mysize; ++i)
   {
      parentsOut[i] = parent[nodesOut[i]];
   }
}

void GeodesicHelper::dijkstra(const int root, const float maxdist, std::vector<int>& nodes, std::vector<float>& dists, bool smooth)
{
   int i, j, whichnode, whichneigh, numNeigh, numChanged = 0;
   int* neighbors;
   float tempf;
   output[root] = 0.0f;
   marked[root] |= 4;
   parent[root] = root;//idiom for end of path
   changed[numChanged++] = root;
   myheap active;
   active.push(root, 0.0f);
   //we keep values greater than maxdist off the stack, so anything pulled from the stack which is unmarked belongs in the list
   while (!active.isEmpty())
   {
      whichnode = active.pop();
      if (!(marked[whichnode] & 1))
      {
         nodes.push_back(whichnode);
         dists.push_back(output[whichnode]);
         marked[whichnode] |= 1;//anything pulled from stack will already be marked as having a valid value (flag 4)
         neighbors = nodeNeighbors[whichnode];
         numNeigh = numNeighbors[whichnode];
         for (j = 0; j < numNeigh; ++j)
         {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if marked
               tempf = output[whichnode] + distances[whichnode][j];//isn't precomputation wonderful
               if (tempf <= maxdist)
               {//keep it off the heap if it is too far
                  if (!(marked[whichneigh] & 4))
                  {
                     parent[whichneigh] = whichnode;
                     marked[whichneigh] |= 4;
                     changed[numChanged++] = whichneigh;
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  } else if (tempf < output[whichneigh]) {
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  }
               }
            }
         }
         if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
         {
            neighbors = nodeNeighbors2[whichnode];
            numNeigh = numNeighbors2[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
               whichneigh = neighbors[j];
               if (!(marked[whichneigh] & 1))
               {//skip floating point math if marked
                  tempf = output[whichnode] + distances2[whichnode][j];//isn't precomputation wonderful
                  if (tempf <= maxdist)
                  {//keep it off the heap if it is too far
                     if (!(marked[whichneigh] & 4))
                     {
                        parent[whichneigh] = whichnode;
                        marked[whichneigh] |= 4;
                        changed[numChanged++] = whichneigh;
                        output[whichneigh] = tempf;
                        active.push(whichneigh, tempf);
                     } else if (tempf < output[whichneigh]) {
                        output[whichneigh] = tempf;
                        active.push(whichneigh, tempf);
                     }
                  }
               }
            }
         }
      }
   }
   for (i = 0; i < numChanged; ++i)
   {
      marked[changed[i]] = 0;//minimize reinitialization of arrays
   }
}

void GeodesicHelper::dijkstra(const int root, bool smooth)
{//straightforward dijkstra, no cutoffs, full surface
   int i, j, whichnode, whichneigh, numNeigh;
   int* neighbors;
   float tempf;
   output[root] = 0.0f;
   parent[root] = root;//idiom for end of path
   myheap active;
   active.push(root, 0.0f);
   while (!active.isEmpty())
   {
      whichnode = active.pop();
      if (!(marked[whichnode] & 1))
      {
         marked[whichnode] |= 1;
         neighbors = nodeNeighbors[whichnode];
         numNeigh = numNeighbors[whichnode];
         for (j = 0; j < numNeigh; ++j)
         {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if marked
               tempf = output[whichnode] + distances[whichnode][j];
               if (!(marked[whichneigh] & 4))
               {
                  parent[whichneigh] = whichnode;
                  marked[whichneigh] |= 4;
                  output[whichneigh] = tempf;
                  active.push(whichneigh, tempf);
               } else if (tempf < output[whichneigh]) {
                  output[whichneigh] = tempf;
                  active.push(whichneigh, tempf);
               }
            }
         }
         if (smooth)
         {
            neighbors = nodeNeighbors2[whichnode];
            numNeigh = numNeighbors2[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
               whichneigh = neighbors[j];
               if (!(marked[whichneigh] & 1))
               {//skip floating point math if marked
                  tempf = output[whichnode] + distances2[whichnode][j];
                  if (!(marked[whichneigh] & 4))
                  {
                     parent[whichneigh] = whichnode;
                     marked[whichneigh] |= 4;
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  } else if (tempf < output[whichneigh]) {
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  }
               }
            }
         }
      }
   }
   for (i = 0; i < numNodes; ++i)
   {
      marked[i] = 0;
   }
}

float** GeodesicHelper::getGeoAllToAll(const bool smooth)
{
   long long bytes = (((long long)numNodes) * numNodes * (sizeof(float) + sizeof(int)) + numNodes * (sizeof(float*) + sizeof(int*))) * 100;//fixed point
   short index = 0;
   static const char *labels[9] = {" Bytes", " Kilobytes", " Megabytes", " Gigabytes", " Terabytes", " Petabytes", " Exabytes", " Zettabytes", " Yottabytes"};
   while (index < 8 && bytes > 80000)
   {//add 2 zeroes, thats 800.00
      ++index;
      bytes = bytes >> 10;
   }
   QMutexLocker locked(&inUse);//don't sit there with memory allocated but locked out of computation, lock early - also before status messages
   std::cout << "attempting to allocate " << bytes / 100 << "." << bytes % 100 << labels[index] << "...";
   std::cout.flush();
   int i = -1, j;
   bool fail = false;
   float** ret = NULL;
   int** parents = NULL;
   try
   {
      ret = new float*[numNodes];
      if (ret != NULL)
      {
         for (i = 0; i < numNodes; ++i)
         {
            ret[i] = new float[numNodes];
            if (ret[i] == NULL)
            {
               fail = true;
               break;//have to break so it doesn't increment i
            }
         }
      }
   } catch (std::bad_alloc e) {//should catch if new likes to throw exceptions instead of returning null
      fail = true;
   }
   if (ret == NULL)
   {
      std::cout << "failed" << std::endl;
      return NULL;
   }
   if (fail)
   {
      std::cout << "failed" << std::endl;
      for (j = 0; j < i; ++j) delete[] ret[j];
      if (i > -1) delete[] ret;
      return NULL;
   }
   i = -1;
   try
   {
      parents = new int*[numNodes];
      if (parents != NULL)
      {
         for (i = 0; i < numNodes; ++i)
         {
            parents[i] = new int[numNodes];
            if (parents[i] == NULL)
            {
               fail = true;
               break;//have to break so it doesn't increment i
            }
         }
      }
   } catch (std::bad_alloc e) {//should catch if new likes to throw exceptions instead of returning null
      fail = true;
   }
   if (parents == NULL)
   {
      std::cout << "failed" << std::endl;
      for (i = 0; i < numNodes; ++i) delete[] ret[i];
      delete[] ret;
      return NULL;
   }
   if (fail)
   {
      std::cout << "failed" << std::endl;
      for (j = 0; j < i; ++j) delete[] parents[j];
      if (i > -1) delete[] parents;
      for (i = 0; i < numNodes; ++i) delete[] ret[i];
      delete[] ret;
      return NULL;
   }
   std::cout << "success" << std::endl;
   alltoall(ret, parents, smooth);
   for (i = 0; i < numNodes; ++i) delete[] parents[i];
   delete[] parents;
   return ret;
}

void GeodesicHelper::alltoall(float** out, int** parents, bool smooth)
{//propagates info about shortest paths not containing root to other roots, hopefully making the problem tractable
   int root, i, j, whichnode, whichneigh, numNeigh, remain, myparent, myparent2, myparent3, prevdots = 0, dots;
   int* neighbors;
   float tempf, tempf2;
   myheap active;
   for (i = 0; i < numNodes; ++i)
   {
      for (j = 0; j < numNodes; ++j)
      {
         out[i][j] = -1.0f;
      }
   }
   remain = numNodes;
   std::cout << "|0%      calculating geodesic distances      100%|" << std::endl;
   //            ..................................................
   for (root = 0; root < numNodes; ++root)
   {
      //std::cout << root << std::endl;
      dots = (50 * root) / numNodes;//simple progress indicator
      while (prevdots < dots)
      {
         std::cout << '.';
         std::cout.flush();
         ++prevdots;
      }
      if (root != 0)
      {
         remain = 0;
         for (i = 0; i < numNodes; ++i)//find known values
         {
            if (out[root][i] > 0.0f)
            {
               marked[i] = 2;//mark that we already have a value, skip calculation, but not yet added to active list
            } else {
               marked[i] = 0;
               ++remain;//count how many more we need to compute so we can stop early
            }
         }
         active.clear();
      }//marking done, dijkstra time
      out[root][root] = 0.0f;
      parents[root][root] = root;//idiom for end of path
      active.push(root, 0.0f);
      while (remain && !active.isEmpty())
      {
         whichnode = active.pop();
         if (!(marked[whichnode] & 1))
         {
            if (!(marked[whichnode] & 2)) --remain;
            marked[whichnode] |= 1;
            neighbors = nodeNeighbors[whichnode];
            numNeigh = numNeighbors[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
               whichneigh = neighbors[j];
               if (marked[whichneigh] & 2)
               {//already has a value and parent for this root
                  if (!(marked[whichneigh] & 8))
                  {//not yet in active list
                     active.push(whichneigh, out[root][whichneigh]);
                     marked[whichneigh] |= 8;
                  }
               } else {
                  if (!(marked[whichneigh] & 1))
                  {//skip floating point math if marked
                     tempf = out[root][whichnode] + distances[whichnode][j];
                     if (!(marked[whichneigh] & 4))
                     {
                        out[root][whichneigh] = tempf;
                        parents[root][whichneigh] = whichnode;
                        marked[whichneigh] |= 4;
                        active.push(whichneigh, tempf);
                     } else if (tempf < out[root][whichneigh]) {
                        out[root][whichneigh] = tempf;
                        parents[root][whichneigh] = whichnode;
                        active.push(whichneigh, tempf);
                     }
                  }
               }
            }
            if (smooth)
            {
               neighbors = nodeNeighbors2[whichnode];
               numNeigh = numNeighbors2[whichnode];
               for (j = 0; j < numNeigh; ++j)
               {
                  whichneigh = neighbors[j];
                  if (marked[whichneigh] & 2)
                  {//already has a value and parent for this root
                     if (!(marked[whichneigh] & 8))
                     {//not yet in active list
                        active.push(whichneigh, out[root][whichneigh]);
                        marked[whichneigh] |= 8;
                     }
                  } else {
                     if (!(marked[whichneigh] & 1))
                     {//skip floating point math if marked
                        tempf = out[root][whichnode] + distances2[whichnode][j];
                        if (!(marked[whichneigh] & 4))
                        {
                           out[root][whichneigh] = tempf;
                           parents[root][whichneigh] = whichnode;
                           marked[whichneigh] |= 4;
                           active.push(whichneigh, tempf);
                        } else if (tempf < out[root][whichneigh]) {
                           out[root][whichneigh] = tempf;
                           parents[root][whichneigh] = whichnode;
                           active.push(whichneigh, tempf);
                        }
                     }
                  }
               }
            }
         }
      }//dijkstra done...lotsa brackets...now to propagate the information gained to other roots
      for (i = root + 1; i < numNodes; ++i)
      {//any node smaller than root already has all distances calculated for entire surface
         if (!(marked[i] & 2))
         {//if endpoint already had distance to root precomputed, all available info from this node has been propagated previously
            myparent2 = i;
            myparent = parents[root][i];
            myparent3 = myparent;
            tempf = out[root][i];
            while (myparent != root)
            {
               tempf2 = tempf - out[root][myparent];
               if (myparent > root)
               {//try to be swap-friendly by not setting values in finished columns
                  out[myparent][i] = tempf2;//use midpoint as root, parent of endpoint is myparent3
                  parents[myparent][i] = myparent3;
               }
               out[i][myparent] = tempf2;//use endpoint as root, parent of midpoint is myparent2
               parents[i][myparent] = myparent2;
               myparent2 = myparent;//step along path
               myparent = parents[root][myparent];
            }
            out[i][root] = out[root][i];//finally, fill the transpose
            parents[i][root] = myparent2;
         }
      }//propagation of best paths to other roots complete, dijkstra again
   }
   while (prevdots < 50)
   {
      std::cout << '.';
      ++prevdots;
   }
   std::cout << std::endl;
   for (i = 0; i < numNodes; ++i)
   {
      marked[i] = 0;
   }
}

void GeodesicHelper::getGeoFromNode(const int node, float* valuesOut, const bool smoothflag)
{
   if (node < 0 || node >= numNodes || !valuesOut)
   {
      return;
   }
   QMutexLocker locked(&inUse);//don't screw with member variables while in use
   float* temp = output;//swap out the output pointer to avoid allocation
   output = valuesOut;
   dijkstra(node, smoothflag);
   output = temp;//restore the pointer to the original memory
}

void GeodesicHelper::getGeoFromNode(const int node, float* valuesOut, std::vector<int>& parentsOut, const bool smoothflag)
{
   if (node < 0 || node >= numNodes || !valuesOut)
   {
      return;
   }
   QMutexLocker locked(&inUse);//again, do not scope this, we need the parent array to stay put
   float* temp = output;//swap out the output pointer to avoid allocation
   output = valuesOut;
   dijkstra(node, smoothflag);
   output = temp;//restore the pointer to the original memory
   parentsOut.resize(numNodes);
   for (int i = 0; i < numNodes; ++i)
   {
      parentsOut[i] = parent[i];
   }
}

void GeodesicHelper::dijkstra(const int root, const std::vector<int>& interested, bool smooth)
{
   int i, j, whichnode, whichneigh, numNeigh, numChanged = 0, remain = 0;
   int* neighbors;
   float tempf;
   j = interested.size();
   for (i = 0; i < j; ++i)
   {
      whichnode = interested[i];
      if (!marked[whichnode])
      {
         ++remain;
         marked[whichnode] = 2;//interested, not expanded, no valid value
         changed[numChanged++] = whichnode;
      }
   }
   output[root] = 0.0f;
   if (!marked[root])
   {
      changed[numChanged++] = root;
   }
   marked[root] |= 4;
   parent[root] = root;//idiom for end of path
   myheap active;
   active.push(root, 0.0f);
   while (remain && !active.isEmpty())
   {
      whichnode = active.pop();
      if (!(marked[whichnode] & 1))
      {
         if (marked[whichnode] & 2)
         {
            --remain;
         }
         marked[whichnode] |= 1;//anything pulled from stack will already be marked as having a valid value (flag 4), so already in changed list
         neighbors = nodeNeighbors[whichnode];
         numNeigh = numNeighbors[whichnode];
         for (j = 0; j < numNeigh; ++j)
         {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if marked
               tempf = output[whichnode] + distances[whichnode][j];//isn't precomputation wonderful
               if (!(marked[whichneigh] & 4))
               {
                  parent[whichneigh] = whichnode;
                  if (!marked[whichneigh])
                  {
                     changed[numChanged++] = whichneigh;
                  }
                  marked[whichneigh] |= 4;
                  output[whichneigh] = tempf;
                  active.push(whichneigh, tempf);
               } else if (tempf < output[whichneigh]) {
                  output[whichneigh] = tempf;
                  active.push(whichneigh, tempf);
               }
            }
         }
         if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
         {
            neighbors = nodeNeighbors2[whichnode];
            numNeigh = numNeighbors2[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
               whichneigh = neighbors[j];
               if (!(marked[whichneigh] & 1))
               {//skip floating point math if marked
                  tempf = output[whichnode] + distances2[whichnode][j];//isn't precomputation wonderful
                  if (!(marked[whichneigh] & 4))
                  {
                     parent[whichneigh] = whichnode;
                     if (!marked[whichneigh])
                     {
                        changed[numChanged++] = whichneigh;
                     }
                     marked[whichneigh] |= 4;
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  } else if (tempf < output[whichneigh]) {
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  }
               }
            }
         }
      }
   }
   for (i = 0; i < numChanged; ++i)
   {
      marked[changed[i]] = 0;//minimize reinitialization of arrays
   }
}

void GeodesicHelper::getGeoToTheseNodes(const int root, const std::vector<int>& ofInterest, std::vector<float>& distsOut, bool smoothflag)
{
   if (root < 0 || root >= numNodes)
   {
      distsOut.clear();//empty array is error condition
      return;
   }
   int i, mysize = ofInterest.size(), node;
   for (i = 0; i < mysize; ++i)
   {//needs to do a linear scan of this array later anyway, so lets sanity check it
      node = ofInterest[i];
      if (node < 0 || node >= numNodes)
      {
         distsOut.clear();//empty array is error condition
         return;
      }
   }
   QMutexLocker locked(&inUse);//let sanity checks fail without locking
   dijkstra(root, ofInterest, smoothflag);
   distsOut.resize(mysize);
   for (i = 0; i < mysize; ++i)
   {
      distsOut[i] = output[ofInterest[i]];
   }
}

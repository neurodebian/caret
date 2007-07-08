#include        <stdio.h>
#include        <math.h>

#include        "surface.h"

#define         DEG2RAD         0.0174533
#define         RAD2DEG         1.0/DEG2RAD

#define		X	0
#define		Y	1
#define		Z 	2

void            find_surface_extent ();
void            translate_to_center_of_mass ();
void            compute_center_of_mass ();
void            reindex_points ();
int             find_index ();
float 		distance  ();
void 		normal_check ();
void    	find_section_extent ();
void            copy_point (int snum1, int idx1, int snum2, int idx2);

int     int_is_in_list (int val, int num, int *list, int *pos)
{
        int     i;

        for (i = 0; i < num; i++){
           if (list [i] == val){
	      *pos = i;
              return (True);
           }
        }
        return (False);
}
 
int     find_index (int	nbr, int snum)
{
        int     k, kmax;

	kmax = surface [snum].num_points;
        k = 0;
        while (k < kmax){
           if (nbr == surface [snum].points [k].Index)
              return (k);
           else
              k++;
        }
        return (-1);
}


void	reindex_points (snum)
{
	int	i, j, k;
    	int	Index, num_neigh;
	int	point_to_delete, l, kmax;

	kmax = surface [snum].num_points;

  	/* Reindex neighbors */
  	printf ("Reindexing %d neighbors from file...\n", kmax);
  	for (k = 0; k < kmax; k++){
    	   num_neigh = 0;
    	   if ((k % 1000) == 0)
              printf ("\t\treindex point %d\n", k);
    	   for (j = 0; j < surface [snum].points [k].num_neighs; j++){
       	      Index = find_index (surface [snum].points [k].neighs [j], snum);
       	      if ((Index >= 0) && (Index != k))
                 surface [snum].points [k].neighs [num_neigh++] = Index;
    	   }
           surface [snum].points [k].num_neighs = num_neigh;
           if (num_neigh > MAX_NEIGHS){
              printf ("ERROR: Exceeded max # of neighbors=%d for point %d (%d)\n",
			MAX_NEIGHS, kmax, num_neigh);
              exit (-1);
           }
  	}
  	k = 0;
  	while (k < kmax){
     	   if (surface [snum].points [k].num_neighs == 0)
              k++;
           else if (surface [snum].points [k].num_neighs < 2){
              printf ("POINT %d (%d,%d) has %d neighbors -> DELETING\n", 
			k, surface [snum].points [k].section, surface [snum].points [k].point,
			surface [snum].points [k].num_neighs);
	      surface [snum].points [k].num_neighs = 0;
              point_to_delete = k; 
              for (j = 0; j < kmax; j++){
                 for (i = 0; i < surface [snum].points [j].num_neighs; i++){
  	            if (surface [snum].points [j].neighs [i] == point_to_delete){
	               for (l = i; l < surface [snum].points [j].num_neighs; l++)
		          surface [snum].points [j].neighs [l] = 
				surface [snum].points [j].neighs [l+1];
		       surface [snum].points [j].num_neighs--; 
	            } 
	         }
              }
	      k = 0;
           }
           else
              k++;
   	}
	surface [snum].num_points = kmax;
	printf ("%d points after reindexing...\n", surface [snum].num_points); 
}

void	find_surface_extent (int snum)
{
	int	i;
	float	minx, maxx;
	float	miny, maxy;
	float	minz, maxz;

	minx = maxx = surface [snum].points [0].pos [0];	
	miny = maxy = surface [snum].points [0].pos [1];	
	minz = maxz = surface [snum].points [0].pos [2];	
	for (i = 0; i < surface [snum].num_points; i++){
	   if (surface [snum].points [i].pos [0] < minx)
	      minx = surface [snum].points [i].pos [0];
	   if (surface [snum].points [i].pos [1] < miny)
	      miny = surface [snum].points [i].pos [1];
	   if (surface [snum].points [i].pos [2] < minz)
	      minz = surface [snum].points [i].pos [2];
	   if (surface [snum].points [i].pos [0] > maxx)
	      maxx = surface [snum].points [i].pos [0];
	   if (surface [snum].points [i].pos [1] > maxy)
	      maxy = surface [snum].points [i].pos [1];
	   if (surface [snum].points [i].pos [2] > maxz)
	      maxz = surface [snum].points [i].pos [2];
	}
	surface [snum].minx = minx;
	surface [snum].miny = miny;
	surface [snum].minz = minz;
	surface [snum].maxx = maxx;
	surface [snum].maxy = maxy;
	surface [snum].maxz = maxz;
	printf ("Surface extent: X %.2f..%.2f, Y %.2f..%.2f, Z %.2f..%.2f\n",
		surface [snum].minx, 
		surface [snum].maxx, 
		surface [snum].miny, 
		surface [snum].maxy, 
		surface [snum].minz,
		surface [snum].maxz); 
}

void    translate_to_center_of_mass (int snum)
{
        int     i;
        float   cx, cy, cz;

        printf ("Translate to center of mass...\n");
        compute_center_of_mass (snum, &cx, &cy, &cz);
        for (i = 0; i < surface [snum].num_points; i++){
           surface [snum].points [i].pos [0] -= cx;
           surface [snum].points [i].pos [1] -= cy;
           surface [snum].points [i].pos [2] -= cz;
        }
        for (i = 0; i < surface [snum].NumCells; i++){
           surface [snum].cells [i].pos [0] -= cx;
           surface [snum].cells [i].pos [1] -= cy;
           surface [snum].cells [i].pos [2] -= cz;
        }
}

void    compute_center_of_mass (int snum, float *CX, float *CY, float *CZ)
{
        float   cx, cy, cz;
        int     i, cnt = 0;

        cx = cy = cz = 0.0;
        for (i = 0; i < surface [snum].num_points; i++){
               cx += surface [snum].points [i].pos [0];
               cy += surface [snum].points [i].pos [1];
               cz += surface [snum].points [i].pos [2];
               cnt++;
        }
        *CX = cx/cnt;
        *CY = cy/cnt;
        *CZ = cz/cnt;
}

int	DeleteLink (int snum, int n1, int n2)
{
	int	i, j, n, found = False;
        int	temp_neighs [MAX_NEIGHS];

	/*printf ("\tDelete %d %d\n", n1, n2);*/
	i = 0;
	for (j = 0; j < surface [snum].points [n1].num_neighs; j++){
           n = surface [snum].points [n1].neighs [j];
	   if (n2 == n){ /* found pair to delete, mark it! */
	      /*printf ("Found pair to delete...\n");*/
	      found = True;
	   }
	   else
              surface [snum].points [n1].neighs [i++] = surface [snum].points [n1].neighs [j];
	}
	if (found == True){
	   surface [snum].points [n1].num_neighs = surface [snum].points [n1].num_neighs-1; 
	   return (True);
	}
	else{ 
	   printf ("\n");
	   return (False);
	}
}

int	InsertLink (int snum, int n1, int n2)
{
	int	i, j, n;
	int	t1;

	/*printf ("Insert %d %d\n", n1, n2);*/
	if ((surface [snum].points [n1].num_neighs+1) > MAX_NEIGHS){
	   fprintf (stderr, "ERROR: Can't add link, exceeded MAX_NEIGHS %d\n", MAX_NEIGHS);
	   return (False);
	}
	t1 = surface [snum].points [n1].num_neighs; 
	surface [snum].points [n1].neighs [t1] = n2; 
	surface [snum].points [n1].num_neighs++; 
	return (True);
}

void	DeleteAllLinks (int idx)
{
	int    	n1, n2, t1, t2 [MAX_NEIGHS];
	int	np;

	n1 = idx; 
	t1 = surface [0].points [n1].num_neighs;
        for (np = 0; np < t1; np++)
           t2 [np] = surface [0].points [n1].neighs [np];
        for (np = 0; np < t1; np++){
	   n2 = t2 [np];
           DeleteLink (0, n1, n2);
           DeleteLink (0, n2, n1);
        }
        /*point_hits [n1] = False;*/
}

void	ResetSurface (snum)
{
        surface [snum].num_points = 0;
        surface [snum].NumBorders = 0;
        surface [snum].NumCells = 0;
}

int	isBorder (int snum, int idx)
{
	int	i, j;

        for (i = 0; i < surface [snum].NumBorders; i++){
           for (j = 0; j < surface [snum].borders [i].num_links; j++){
	      if (surface [snum].borders [i].links [j] == idx)
	         return (True);
           }
        }
	return (False);
}

int     find_point (int snum, int section, int point)
{
        int     i;

        for (i = 0; i < surface [snum].num_points; i++){
           if (surface [snum].points [i].section == section){
              if (surface [snum].points [i].point == point)
                 return (i);
           }
        }
}

int     find_point_fast (int snum, int section, int point, int prev)
{
        int     i;

        for (i = prev; i < surface [snum].num_points; i++){
           if (surface [snum].points [i].section == section){
              if (surface [snum].points [i].point == point)
                 return (i);
           }
        }
        for (i = 0; i < prev; i++){
           if (surface [snum].points [i].section == section){
              if (surface [snum].points [i].point == point)
                 return (i);
           }
        }
        printf ("\tWARNING: Point (%d,%d) not found in surface\n",
                section, point);
        return (-1);
}

float distance  (v1, v2)

register        float   *v1, *v2;
{
  register float x, y, z;

  x = v1 [0] - v2 [0];
  y = v1 [1] - v2 [1];
  z = v1 [2] - v2 [2];
  return (sqrt(x*x + y*y + z*z));
}

int     find_closest_point (int snum, float *point, float *dist)
{
        int     j,
                closest_point;
        float   min_distance,
                length;

        closest_point = 0;
        min_distance = distance (point, surface [snum].points [0].pos);
        for (j = 1; j < surface [snum].num_points; j++){
           if (surface [snum].points [j].num_neighs > 0){
              length = distance (point, surface [snum].points [j].pos);
              if (length < min_distance){
                 min_distance = length;
                 closest_point = j;
              }
           }
        }
        *dist = min_distance;
        return (closest_point);
}

void filter_colors (int snum)
{
	int	i, j, k, n;
	float	accum [3];

	printf ("Low-pass filter colors...\n");
	for (i = 0; i < surface [snum].num_points; i++){
	   if ((i % 5000) == 0)
	      printf ("\t%4d of %4d\n", i, surface [snum].num_points);
	   for (k = 0; k < 3; k++)
	      accum [k] = surface [snum].points [i].colors [k];
	   for (j = 0; j < surface [snum].points [i].num_neighs; j++){
	      n = surface [snum].points [i].neighs [j];
	      for (k = 0; k < 3; k++)
	         accum [k] += surface [snum].points [n].colors [k];
	   }
	   for (k = 0; k < 3; k++)
	      surface [snum].points [i].colors [k] = 
			accum [k]/(float)(surface [snum].points [i].num_neighs+1); 
	}
}

float  magnitude (vector)

register        float  *vector;
{
        return (sqrt(vector [X]*vector [X] + vector [Y]*vector [Y] + vector [Z]*vector [Z]));
}


void	subtract_vectors (v1, v2, diff)

register        float   *v1,
                        *v2,
                        *diff;
{
        int     i;

        for (i = 0; i < 3; i++)
           diff [i] = v1 [i] - v2 [i];
}

void	unit_normal (float *o, float *n, float * m, float *normal)
{
	int     i;
        float   d_o [3],
        	d_m [3],
                nmag;

        /* Obtain the unit normal from (o - n) x (m - n) */
        subtract_vectors (o, n, d_o);
        subtract_vectors (m, n, d_m);

        normal [X] = d_o [Y]*d_m [Z] - d_m [Y]*d_o [Z];
        normal [Y] = d_m [X]*d_o [Z] - d_o [X]*d_m [Z];
        normal [Z] = d_o [X]*d_m [Y] - d_m [X]*d_o [Y];
        nmag = magnitude (normal);
        if (nmag > 0.0) {
           for (i = 0; i < 3; i++)
              normal [i] /= nmag;
        }
        else {
           for (i = 0; i < 3; i++)
              normal [i] = 0.0;
        }
}

void	compute_normals (int snum)
{
	int	v1, v2, v3;
	int	i, j;
	float	normal [3], nmag;

	for (i = 0; i < surface [snum].num_points; i++){
	   for (j = 0; j < 3; j++)
	      surface [snum].points [i].normal [j] = 0.0;
	   surface [snum].points [i].num_tris = 0;
	}
	for (i = 0; i < surface [snum].NumPolygons; i++){
	   v1 = surface [snum].tiles [i].triangle [0]; 
	   v2 = surface [snum].tiles [i].triangle [1]; 
	   v3 = surface [snum].tiles [i].triangle [2]; 
	   unit_normal (surface [snum].points [v1].pos, 
			surface [snum].points [v2].pos, 
			surface [snum].points [v3].pos, normal);
	   for (j = 0; j < 3; j++){
	      surface [snum].points [v1].normal [j] += normal [j];
	      surface [snum].points [v1].num_tris++; 
	      surface [snum].points [v2].normal [j] += normal [j];
	      surface [snum].points [v2].num_tris++; 
	      surface [snum].points [v3].normal [j] += normal [j];
	      surface [snum].points [v3].num_tris++; 
	   }
	   /*if ((i % 1) == 0)
	      printf ("Normal for triangle %d: %d %d %d is %.2f %.2f %.2f\n",
			i, v1, v2, v3, normal [0], normal [1], normal [2]);*/
	}
	for (i = 0; i < surface [snum].num_points; i++){
	   for (j = 0; j < 3; j++)
	      surface [snum].points [i].normal [j] /= surface [snum].points [v3].num_tris; 
           nmag = magnitude (surface [snum].points [i].normal); 
	   for (j = 0; j < 3; j++)
	      surface [snum].points [i].normal [j] /= nmag; 
	}
}

normal_constistency_check (int snum)
{
	int	i;
	for (i = 0; i < surface [snum].NumPolygons; i++)
	   surface [snum].tiles [i].visited = False;
	normal_check (snum, 0);
}

int DoTrianglesShareEdge (int snum, int Tidx1, int Tidx2, int *common)
{
	int	i;
	int	v1, v2;
	int	found, flag, pos; 
	float	val;

	i = 0;
	found = False;
	while (found == False){	
	   flag = int_is_in_list (surface [snum].tiles [Tidx1].triangle [i],
		3, surface [snum].tiles [Tidx2].triangle, &pos); 
	   if (flag == True){
	      v1 = surface [snum].tiles [Tidx1].triangle [i];
	      found = True;
	   }
	   else
	      i++;
	   if (i == 3)
	      return (False);
	}
	i = 0;
	found = False;
	while (found == False){	
	   if (surface [snum].tiles [Tidx1].triangle [i] == v1)
	      i++;
	   flag = int_is_in_list (surface [snum].tiles [Tidx1].triangle [i], 
		3, surface [snum].tiles [Tidx2].triangle, &pos); 
	   if (flag == True){
	      v2 = surface [snum].tiles [Tidx1].triangle [i];
	      found = True;
	   }
	   else
	      i++;
	   if (i >= 3)
	      return (False);
	}
	common [0] = v1;
	common [1] = v2;
	/*printf ("Triangles share edge %d %d\n", v1, v2);
	printf ("\t%d: %d %d %d\n", 
		Tidx1, surface [snum].tiles [Tidx1].triangle [0],
		surface [snum].tiles [Tidx1].triangle [1],
		surface [snum].tiles [Tidx1].triangle [2]);
	printf ("\t%d: %d %d %d\n", 
		Tidx2, surface [snum].tiles [Tidx2].triangle [0],
		surface [snum].tiles [Tidx2].triangle [1],
		surface [snum].tiles [Tidx2].triangle [2]);*/
	return (True);
}

int	findTriangleNeighbors (int snum, int Tidx, int *Tneighs)
{
	int	i, flag, num = 0;
	int	commonVertices [2];

	for (i = 0; i < surface [snum].NumPolygons; i++){
	   if (i != Tidx){
	      flag = DoTrianglesShareEdge (snum, Tidx, i, commonVertices);
	      if (flag == True)
	         Tneighs [num++] = i;
	   }
	}
	if (num > 3){
	   printf ("ERROR: What?\n");
	   printf ("\tTriangle %d has %d neighboring triangles...\n", Tidx, num);
	   for (i = 0; i < num; i++)
	      printf ("\t\t%d %d\n", i, Tneighs [i]);
	   /*exit (-1);*/
	}
	return (num);
}

void insertVertex (int snum, int Tidx1, int Tidx2, int *commonVertices, int *tri)
{
	int	notshared;
	int	i, flag;
	int	p1, p2;

	for (i = 0; i < 3; i++){
	   flag = int_is_in_list (surface [snum].tiles [Tidx2].triangle [i], 
		2, commonVertices, &p1);
	   if (flag == False)
	      notshared = surface [snum].tiles [Tidx2].triangle [i];
	}
	flag = int_is_in_list (commonVertices [0], 
		3, surface [snum].tiles [Tidx1].triangle, &p1); 
	flag = int_is_in_list (commonVertices [1], 
		3, surface [snum].tiles [Tidx1].triangle, &p2); 
	/*printf ("New polygon: %4d %4d, new = %4d\n", 
		commonVertices [0], commonVertices [1], notshared);
	printf ("           : %4d %4d\n", p1, p2);*/

	if ((p1 == 0) && (p2 == 1)){
	   tri [0] = commonVertices [0];
	   tri [1] = notshared; 
	   tri [2] = commonVertices [1];
	}else if ((p1 == 0) && (p2 == 2)){
	   tri [0] = commonVertices [1]; 
	   tri [1] = notshared; 
	   tri [2] = commonVertices [0];
	}else if ((p1 == 1) && (p2 == 2)){
	   tri [0] = commonVertices [0]; 
	   tri [1] = notshared; 
	   tri [2] = commonVertices [1];
	}
	else{
	   printf ("What to do!\n");
	   exit (-1); 
	}
}

void normal_check (int snum, int Tidx) 
{
	int	Tneighs [10]; /* triangles that share common edge (can't be more than 3) */
	int	newTri [3];
	int	numTneighs;
	int	i, j; 
	int	commonVertices [2], notshared;

	if (surface [snum].tiles [Tidx].visited == True)
	   return;
	if ((Tidx % 500) == 0)
	   printf ("Checking triangle %d\n", Tidx);
	surface [snum].tiles [Tidx].visited = True;
	numTneighs = findTriangleNeighbors (snum, Tidx, Tneighs);
	for (i = 0; i < numTneighs; i++){
	   DoTrianglesShareEdge (snum, Tidx, Tneighs [i], commonVertices);
	   insertVertex (snum, Tidx, Tneighs [i], commonVertices, newTri);
	   for (j = 0; j < 3; j++)
	      surface [snum].tiles [Tneighs [i]].triangle [j] = newTri [j];
	   normal_check (snum, Tneighs [i]); 
	}
}
#define sgn(a)          ((a) < 0.0 ? -1.0 : 1.0)        /* NB 0.0 -> 1.0 */

float compute_angle (float *v1, float *v2, float *v3, float nx, float ny, float nz)
{
  	float x1, x2, y1, y2, z1, z2, dx, dy, dz, s, c, phi;

  	x1 = v2 [0] - v1 [0];
  	y1 = v2 [1] - v1 [1];
  	z1 = v2 [2] - v1 [2];
  	x2 = v3 [0] - v1 [0];
  	y2 = v3 [1] - v1 [1];
  	z2 = v3 [2] - v1 [2];

  	dx = y1*z2 - y2*z1;
  	dy = x2*z1 - x1*z2;
  	dz = x1*y2 - x2*y1;
  	s = sgn((dx*nx) + (dy*ny) + (dz*nz)) * sqrt((dx*dx) + (dy*dy) + (dz*dz));

  	c = x1*x2 + y1*y2 + z1*z2;
  	phi = atan2(s,c);
  	return (phi);
}

float   compute_triangle_area (float *p1, float *p2, float *p3, float *tnormal)
{
        float   area, w, h;

        w = distance (p2, p1);
        h = distance (p2, p3);
        if ((w <= 0.0) || (h <= 0.0))
           area = 0.0;
        else
           area = 0.5*w*h*sin(compute_angle (p2, p3, p1, tnormal [0], tnormal [1], tnormal [2]));
        return (area);
}

void	find_section_extent (int snum)
{
	int	min, max;
	int	i;

	min = max = surface [snum].points [0].section;
        for (i = 0; i < surface [snum].num_points; i++){
	   /*printf ("%d %d\n", i, surface [snum].points [i].section);*/
           if (surface [snum].points [i].section > max)
              max = surface [snum].points [i].section;
           if (surface [snum].points [i].section < min)
              min = surface [snum].points [i].section;
        }
        printf ("SURFACE: Sections %d..%d\n", min, max);
        surface [snum].lo_section = min;
        surface [snum].hi_section = max;
}

int     find_point_using_pos (int snum, float *pos)
{
        int     i;
        float   d1, d2, d3;

        for (i = 0; i < surface [snum].num_points; i++){
           d1 = fabs (pos [0] - surface [snum].points [i].pos [0]);
           if (d1 < 0.01){
              d2 = fabs (pos [1] - surface [snum].points [i].pos [1]);
              if (d2 < 0.01){
                 d3 = fabs (pos [2] - surface [snum].points [i].pos [2]);
                 if (d3 < 0.1)
                    return (i);
              }
           }
        }
        printf ("ERROR: Can't find point %f %f %f...\n",
		pos [0], pos [1], pos [2]); 
        return (-1);
}

void	smoothing (int snum, float init_t_value, int iter)
{
	float		F_VALUE; 
	int		nsmooth, i, j, neigh;
	float		xa, ya, za;	
	float   	smoothp [MAX_POINTS][3];

	printf ("\tSMOOTH surface %d for %d iters, %f param\n", 
		snum, iter, init_t_value);
	for (j = 0; j < surface [snum].num_points; j++)
	   for (i = 0; i < 3; i++)
	      smoothp [j][i] = surface [snum].points [j].pos [i];
	for (nsmooth = 0; nsmooth < iter; nsmooth++){
	   /*if ((nsmooth % 10) == 0)
	      printf ("\t%d of %d\n", nsmooth, iter);*/
	   for (j = 0; j < surface [snum].num_points; j++){
		 F_VALUE = 1.0 - init_t_value;
	         xa = ya = za = 0.0;
                 for (i = 0; i < surface [snum].points [j].num_neighs; i++){
		    neigh = surface [snum].points [j].neighs [i];
		    xa += surface [snum].points [neigh].pos [0];
		    ya += surface [snum].points [neigh].pos [1];
		    za += surface [snum].points [neigh].pos [2];
                 }
	         xa = xa/(float) surface [snum].points [j].num_neighs; 
	         ya = ya/(float) surface [snum].points [j].num_neighs; 
	         za = za/(float) surface [snum].points [j].num_neighs; 

	         if (surface [snum].points [j].num_neighs > 0){
	            smoothp [j][0] = 
			(1.0 - F_VALUE)*surface [snum].points [j].pos [0] + F_VALUE*xa;
	            smoothp [j][1] = 
			(1.0 - F_VALUE)*surface [snum].points [j].pos [1] + F_VALUE*ya;
	            smoothp [j][2] = 
			(1.0 - F_VALUE)*surface [snum].points [j].pos [2] + F_VALUE*za;
	         }
	   }
	}
	for (j = 0; j < surface [snum].num_points; j++)
	   for (i = 0; i < 3; i++)
	      surface [snum].points [j].pos [i] = smoothp [j][i];
}

void	cross_product (d_o, d_m, product)

float   *product, *d_o, *d_m;
{
        int     i;
        float   nmag;

        product [X] = d_o [Y]*d_m [Z] - d_m [Y]*d_o [Z];
        product [Y] = d_m [X]*d_o [Z] - d_o [X]*d_m [Z];
        product [Z] = d_o [X]*d_m [Y] - d_m [X]*d_o [Y];
        nmag = magnitude (product);
        if (nmag > 0.0) {
           for (i = 0; i < 3; i++)
              product [i] /= nmag;
        }
        else {
           for (i = 0; i < 3; i++)
              product [i] = 0.0;
        }
}

float   dot_product (v, w)

float   *v, *w;
{
        float   result;

        result = v [0]*w [0] + v [1]*w [1] + v [2]*w [2];
        return (result);
}

void	check_triangles (int snum)
{
	int	i, v1, v2, v3;
	float	angle1, angle2, angle3, normal [3], 
		temp1, temp2, temp3;
	int	s1, s2, s3;
	float	LOWER = 2.0;

	printf ("\nChecking triangles for surface %d\n", snum);
	for (i = 0; i < surface [snum].NumPolygons; i++){
	   if ((i % 10000) == 0)
	      printf ("\ttriangle %5d of %5d\n", i, surface [snum].NumPolygons);
	   v1 = surface [snum].tiles [i].triangle [0]; 
	   v2 = surface [snum].tiles [i].triangle [1]; 
	   v3 = surface [snum].tiles [i].triangle [2]; 
	   unit_normal (surface [snum].points [v1].pos, 
			surface [snum].points [v2].pos, 
			surface [snum].points [v3].pos, normal);
	   angle1 = compute_angle (
		surface [snum].points [v1].pos,
		surface [snum].points [v2].pos,
		surface [snum].points [v3].pos,
		normal [0], normal [1], normal [2]);
	   temp1 = fabsf (angle1)*RAD2DEG;
	   angle2 = compute_angle (
		surface [snum].points [v2].pos,
		surface [snum].points [v3].pos,
		surface [snum].points [v1].pos,
		normal [0], normal [1], normal [2]);
	   temp2 = fabsf (angle2)*RAD2DEG;
	   angle3 = compute_angle (
		surface [snum].points [v3].pos,
		surface [snum].points [v1].pos,
		surface [snum].points [v2].pos,
		normal [0], normal [1], normal [2]);
	   temp3 = fabsf (angle2)*RAD2DEG;
	   if ((temp1 < LOWER) || (temp2 < LOWER) || (temp3 < LOWER)){
	      s1 = surface [snum].points [v1].section;
	      s2 = surface [snum].points [v2].section;
	      s3 = surface [snum].points [v3].section;
	      if ((s1 == s2) && (s2 == s3))
	         printf ("Triangle on one section\n");
	      else{
	         printf ("WARNING: Angle for triangle %d too SMALL\n", i); 
	         printf ("\tVertices: %d (%d,%d) %d (%d,%d) %d (%d,%d)\n", 
			v1, surface [snum].points [v1].section, surface [snum].points [v1].point, 
			v2, surface [snum].points [v2].section, surface [snum].points [v2].point, 
			v3, surface [snum].points [v3].section, surface [snum].points [v3].point);
	         printf ("\tAngles  : %f,%f,%f)\n", temp1, temp2, temp3);
	      }
	   }
	   /*printf ("Triangle %4d: %.2f %.2f %.2f\n", i, 
		angle1*RAD2DEG, angle2*RAD2DEG, angle3*RAD2DEG);*/
	}
}

void	compute_distortion (int snum)
{
	int	i;
	int	v1, v2, v3;
	float	temp, avg = 0.0;

	printf ("Compute distortion for surface %d\n", snum);
        for (i = 0; i < surface [snum].NumPolygons; i++){
	   temp = surface [snum].tiles [i].area2D/surface [snum].tiles [i].area3D;
	   if ((i % 1000) == 0)
	      printf ("%d %f %f %f %f\n", i, surface [snum].tiles [i].area2D, 
		surface [snum].tiles [i].area3D, temp, avg);
	   avg += fabsf ((temp-1.0)*100.0);
	   surface [snum].tiles [i].distortion = log (temp)/log (2.0); 
	}
	avg /= surface [snum].NumPolygons;
	printf ("\tAverage polygon distortion is %f\n", avg);

        for (i = 0; i < surface [snum].num_points; i++)
           surface [snum].points [i].distortion = 0.0;
        for (i = 0; i < surface [snum].NumPolygons; i++){
	   v1 = surface [snum].tiles [i].triangle [0]; 
	   v2 = surface [snum].tiles [i].triangle [1]; 
	   v3 = surface [snum].tiles [i].triangle [2]; 
	
           surface [snum].points [v1].distortion += surface [snum].tiles [i].distortion; 
           surface [snum].points [v2].distortion += surface [snum].tiles [i].distortion; 
           surface [snum].points [v3].distortion += surface [snum].tiles [i].distortion; 
        }
        for (i = 0; i < surface [snum].num_points; i++){
           surface [snum].points [i].distortion /= surface [snum].points [i].num_neighs;
	   /*if ((i % 1000) == 0)
	      printf ("\t\t%d %f\n", i, surface [snum].points [i].distortion);*/
	}
}

/* Copy point idx1 -> idx2 */
void            copy_point (int snum1, int idx1, int snum2, int idx2)
{
	int	i;

	for (i = 0; i < 3; i++){
	   surface [snum2].points [idx2].pos [i] = 
		surface [snum1].points [idx1].pos [i];
	   surface [snum2].points [idx2].pos3D [i] = 
		surface [snum1].points [idx1].pos3D [i];
	   surface [snum2].points [idx2].normal [i] = 
		surface [snum1].points [idx1].normal [i];
	   surface [snum2].points [idx2].colors [i] = 
		surface [snum1].points [idx1].colors [i];
	}
	surface [snum2].points [idx2].num_neighs = 
		surface [snum1].points [idx1].num_neighs;
	surface [snum2].points [idx2].num_tris =
		surface [snum1].points [idx1].num_tris;
	for (i = 0; i < surface [snum1].points [idx1].num_neighs; i++){
	   surface [snum2].points [idx2].neighs [i] =
		surface [snum1].points [idx1].neighs [i];
	}
	surface [snum2].points [idx2].section =
		surface [snum1].points [idx1].section;
	surface [snum2].points [idx2].point =
		surface [snum1].points [idx1].point;
	surface [snum2].points [idx2].class =
		surface [snum1].points [idx1].class;
	surface [snum2].points [idx2].Index =
		surface [snum1].points [idx1].Index;
	strcpy (surface [snum2].points [idx2].region,
		surface [snum1].points [idx1].region);
	surface [snum2].points [idx2].display_flag = True;
	surface [snum2].points [idx2].curvature =
		surface [snum1].points [idx1].curvature;
	surface [snum2].points [idx2].projcells =
		surface [snum1].points [idx1].projcells;
	surface [snum2].points [idx2].distortion =
		surface [snum1].points [idx1].distortion;

}

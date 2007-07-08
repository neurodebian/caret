#include	<stdio.h>
#include	<math.h>

double 	dist (double,double,double,double);

void	resample_line (double *xorig, double *yorig, int totpoints, 
		double *xout, double *yout, double ddd, int *numpoint, int flag)
{
	int 	i, j, k, l;
	double 	totdist, curd, dj, dj1;
	double 	x1, y1, frac;

	if (flag == 1){
	   totdist = 0.0;
	   for (i = 1; i < totpoints; i++)
	      totdist += dist (xorig [i], yorig [i], xorig [i-1], yorig [i-1]);
	   ddd = totdist/(*numpoint-1);
	   printf ("\tTotal distance of segment is %.2f, avg distance %.2f, numpoint %d\n", 
		totdist, ddd, *numpoint);
	   printf ("\tCOMPUTE %d points...\n", *numpoint);
	}
	else if (flag == 0){
	   totdist = 0.0;
	   for (i = 1; i < totpoints; i++)
	      totdist += dist (xorig [i], yorig [i], xorig [i-1], yorig [i-1]);
	   *numpoint = totdist/ddd + 1;
	   printf ("Total distance of segment is %.2f, avg distance %.2f, numpoint %d\n", 
		totdist, ddd, *numpoint);
	   printf ("\tCOMPUTE points that are %.2f away...\n", ddd); 
	}
	/* Now find the next point dist away from the previous point */
	xout [0] = xorig [0];
	yout [0] = yorig [0];

	j = 0; /* index of the points processed */
        for (i = 0; i < *numpoint-1; i++){
	   x1 = xout [i];
	   y1 = yout [i];

	   dj1 = 0;
	   dj = dist (x1, y1, xorig [j], yorig [j]);
	   /*printf ("Find next point after %.2f %.2f that is %.2f away...\n",
			x1, y1, ddd); */
	   k = j;
	   /* find the distance to the jth point */
	   while (dj < ddd){
	      /*printf ("\t\t\tloop: j %d, dj %.2f, dj1 %.2f\n", j, dj, dj1);*/
	      dj1 = dj;
	      dj = dj+dist (xorig [j], yorig [j], xorig [j+1], yorig [j+1]);
	      j++;
	   }
	   /* The new point lies in the jth segment */
	   frac = (ddd - dj1)/(dj-dj1);
	   /*printf ("\t\tj is %d, dj1 is %.2f, dj is %.2f, ddd %.2f, frac %.2f\n", 
			j, dj1, dj, ddd, frac);*/
	
	   if (k != j) {
	      x1 = xorig [j-1];
	      y1 = yorig [j-1];
	   }
	   /*printf ("\t\tCompute location betwen %.2f %.2f and %.2f %.2f\n",
			x1, y1, xorig [j], yorig [j]);*/
	   xout [i+1] = (1-frac)*x1 + frac*xorig[j];
	   yout [i+1] = (1-frac)*y1 + frac*yorig[j];
	   /*printf ("\t%d %d %d, %.2f %.2f ->  %.2f %.2f, dist %.2f\n", i, k, j, 
			xout [i], yout [i],
			xout [i+1], yout [i+1],
			dist (xout [i], yout [i], xout [i+1], yout [i+1]));*/
	}
	/**numpoint = i-1;*/

	/*printf ("%d points...\n", *numpoint);
	for (i = 1; i < *numpoint; i++){
	   printf ("\t\t%d: %.2f %.2f, dist %.2f\n",i, xout [i],yout [i], totdist);
	   totdist = dist (xout [i], yout [i], xout [i-1], yout [i-1]);
	}*/
}

double dist (double x1,double y1,double x2,double y2)
{
	double 	d;
	double	xdiff, ydiff;	

	xdiff = x2-x1;
	ydiff = y2-y1;	

	/*d = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));*/
	d = sqrt ((xdiff*xdiff)+(ydiff*ydiff));
	/*printf ("\tDIST: %.2f %.2f, answer %f\n", xdiff, ydiff, d);*/
	return(d);
}

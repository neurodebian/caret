#include        <stdio.h>

#include       	"surface.h" 

#define		NEW
#define		SKIP	1

extern 	int     num_surfaces; 

void 		read_surface_file ();
void 		write_all_surface_file ();
void 		write_coord_file ();
void 		write_3Dcoord_file ();
void 		write_sort_file ();
void 		write_cnt_file ();
void 		read_coord_file ();
void 		read_3Dcoord_file ();
void 		read_sort_file ();
void 		read_aux_file ();
void 		read_border_file ();
void 		read_ascii_border_file ();
void 		read_polygon_file ();
void		read_off_file ();
void		convert_off_file ();
void		convert_off_file_new ();
void 		match_points_to_triangles ();
void		read_curvature_file ();
void		read_projcells_file ();
void		copy_point ();
void		read_anat_boundaries_file ();
void		extract_sections (int snum, int num_sections, float *distinct_z);
int		determine_num_contours (int snum, int sect, int *npoints);

int		read_2Darea_file ();
int		read_3Darea_file ();

extern	void	reindex_points ();
extern	void	compute_normals ();
extern	int     find_index ();
extern	void	find_surface_extent ();
extern	void 	translate_to_center_of_mass ();
extern	void	compute_center_of_mass ();
extern 	int     find_point ();
extern 	int	find_closest_point ();
extern	int	find_section_extent ();
extern	void    check_triangles ();

#define MAX_PT_TRIS 100
struct{
        int     num_tris;
        int     tris [MAX_PT_TRIS];
}pt_tris [MAX_POINTS];

int     determine_section_num (int snum, float val, int nsections, float *distinct_z)
{
        int     i;

        for (i = 0; i < nsections; i++){
           if (distinct_z [i] == val)
              return (i);
        }
        printf ("ERROR: Can't find point with z value of %f\n", val);
        exit (-1);
}

void	InitStuff (int snum)
{
	int	i;

	for (i = 0; i < MAX_BORDERS; i++)
	   surface [snum].borders [i].num_links = 0;
}

void 	read_surface_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	int	i1, i2, i3, i4;
	float	f1, f2, f3;
	char	line [100], s1 [5];
	int	debug = False;

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_surface_file) could not open file %s\n", filename);
     	   exit (-1);
  	}
  	fgets (line, MAXLINE, fp);
  	sscanf (line, "%d", &surface [snum].num_points);
	InitStuff (snum);
	printf ("%d points to read from raw %s...\n", surface [snum].num_points, filename);
	if (surface [snum].num_points > MAX_POINTS){
	   printf ("ERROR: %d Exceeded max # of points %d\n",
		surface [snum].num_points, MAX_POINTS);
	   exit (-1);
	}
	for (i = 0; i < surface [snum].num_points; i++){
  	   fgets (line, MAXLINE, fp);
  	   sscanf (line, "%d %f %f %f %d %d %d %s", 
			&i1, &f1, &f2, &f3, &i2, &i3, &i4, s1);
  	   /*printf ("%d %f %f %f %d %d %d %s\n", 
			i1, f1, f2, f3, i2, i3, i4, s1);*/
	   /*surface [snum].points [i].Index = i1;*/
	   surface [snum].points [i].pos [0] = f1;
	   surface [snum].points [i].pos [1] = f2;
	   surface [snum].points [i].pos [2] = f3;
  	   /*printf ("\t%d %f %f %f\n", 
		i,	
		surface [snum].points [i].pos [0],
		surface [snum].points [i].pos [1],
		surface [snum].points [i].pos [2]);*/
	   /*surface [snum].points [i].num_neighs = i2;*/
	   if (surface [snum].points [i].num_neighs > MAX_NEIGHS){
	      printf ("ERROR: %d Exceeded max # of neighbors %d > %d\n",
		i, surface [snum].points [i].num_neighs, MAX_NEIGHS);
	      exit (-1);
	   }
	   /*surface [snum].points [i].section = i3;
	   surface [snum].points [i].point = i4;
	   strcpy (surface [snum].points [i].region, s1);*/
	   if (debug)
  	      printf ("%d %f %f %f %d %d %d %s\n", 
			surface [snum].points [i].Index,
			surface [snum].points [i].pos [0],
			surface [snum].points [i].pos [1],
			surface [snum].points [i].pos [2],
			surface [snum].points [i].num_neighs,
			surface [snum].points [i].section,
			surface [snum].points [i].point,
			surface [snum].points [i].region);
	   for (j = 0; j < i2; j++){ 
  	      fgets (line, MAXLINE, fp);
	      sscanf (line, "%d %d", &i1, &i3);
	      /*printf ("\t\t%d %d\n", i1, i3);*/
	      /*surface [snum].points [i].neighs [j] = i3;*/
	      if (debug)
	         printf ("\t%d %d\n", j, surface [snum].points [i].neighs [j]); 
	   }
	}
	fclose (fp);

	printf ("Read %d points from file\n", i);
	/*reindex_points (snum);
	translate_to_center_of_mass (snum);
	find_surface_extent (snum);*/
}

void 	write_all_3Dsurface_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	char	newfile [100];

	sprintf (newfile, "%s.3Dall", filename);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_surface_file) could not open file %s\n", newfile);
     	   exit (-1);
  	}
  	fprintf (fp, "%d\n", surface [snum].num_points);
	printf ("%d points to write to %s...\n", surface [snum].num_points, newfile);
	for (i = 0; i < surface [snum].num_points; i++){
  	      fprintf (fp, "%d %f %f %f %d %d %d %s\n", 
	    		i,	
	   		surface [snum].points [i].pos3D [0],
	   		surface [snum].points [i].pos3D [1],
	   		surface [snum].points [i].pos3D [2], 
	   		surface [snum].points [i].num_neighs, 
	   		surface [snum].points [i].section, 
	   		surface [snum].points [i].point, 
	   		surface [snum].points [i].region);
	      for (j = 0; j < surface [snum].points [i].num_neighs; j++)
	         fprintf (fp, "%d %d\n", j, surface [snum].points [i].neighs [j]); 
	}
	fclose (fp);
}

void 	write_all_surface_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	char	newfile [100];

	sprintf (newfile, "%s.all", filename);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_surface_file) could not open file %s\n", newfile);
     	   exit (-1);
  	}
  	fprintf (fp, "%d\n", surface [snum].num_points);
	printf ("%d points to write to %s...\n", surface [snum].num_points, newfile);
	for (i = 0; i < surface [snum].num_points; i++){
  	      fprintf (fp, "%d %f %f %f %d %d %d %s\n", 
	    		i,	
	   		surface [snum].points [i].pos [0],
	   		surface [snum].points [i].pos [1],
	   		surface [snum].points [i].pos [2], 
	   		surface [snum].points [i].num_neighs, 
	   		surface [snum].points [i].section, 
	   		surface [snum].points [i].point, 
	   		surface [snum].points [i].region);
	      for (j = 0; j < surface [snum].points [i].num_neighs; j++)
	         fprintf (fp, "%d %d\n", j, surface [snum].points [i].neighs [j]); 
	}
	fclose (fp);
}

void 	extract_surface_file (char *filename, int snum, char *area)
{
	FILE	*fp;
	int	i, j, s1, temp;

  	if ((fp = fopen (filename, "w")) == NULL){;
     	   printf ("ERROR: (write_surface_file) could not open file %s\n", filename);
     	   exit (-1);
  	}
	temp = 0;
	for (i = 0; i < surface [snum].num_points; i++){
	   s1 = strcmp (surface [snum].points [i].region, area);
	   if (s1 == 0)
	      temp++;
	}

  	fprintf (fp, "%d\n", temp); 
	printf ("%d points to write to %s for area %s...\n", temp, filename, area);
	for (i = 0; i < surface [snum].num_points; i++){
	   s1 = strcmp (surface [snum].points [i].region, area);
	   if (s1 == 0){
  	      fprintf (fp, "%d %f %f %f %d %d %d %s\n", 
	    		surface [snum].points [i].Index,	
	   		surface [snum].points [i].pos [0],
	   		surface [snum].points [i].pos [1],
	   		surface [snum].points [i].pos [2], 
	   		surface [snum].points [i].num_neighs, 
	   		surface [snum].points [i].section, 
	   		surface [snum].points [i].point, 
	   		surface [snum].points [i].region);
	      for (j = 0; j < surface [snum].points [i].num_neighs; j++)
	         fprintf (fp, "%d %d\n", j, surface [snum].points [i].neighs [j]); 
	   }
	}
	fclose (fp);
}

void 	write_3Dcoord_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	char	newfile [100];

	sprintf (newfile, "%s.3Dcoord", filename);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_3Dcoord_file) could not open file %s\n", newfile);
     	   exit (-1);
  	}
  	fprintf (fp, "%d\n", surface [snum].num_points);
	printf ("%d points to write to %s for snum %d...\n", 
			surface [snum].num_points, newfile, snum);
	for (i = 0; i < surface [snum].num_points; i++){
  	   fprintf (fp, "%d %f %f %f\n", 
	    	i, 
		surface [snum].points [i].pos3D [0],
	   	surface [snum].points [i].pos3D [1],
	   	surface [snum].points [i].pos3D [2]);
	}
	fclose (fp);
}

void 	write_coord_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	char	newfile [100];

	sprintf (newfile, "%s.coord", filename);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_coord_file) could not open file %s\n", newfile);
     	   exit (-1);
  	}
  	fprintf (fp, "%d\n", surface [snum].num_points);
	printf ("%d points to write to %s for snum %d...\n", 
			surface [snum].num_points, newfile, snum);
	for (i = 0; i < surface [snum].num_points; i++){
  	   fprintf (fp, "%d %f %f %f\n", 
	    	i, surface [snum].points [i].pos [0],
	   	surface [snum].points [i].pos [1],
	   	surface [snum].points [i].pos [2]);
	   if (i < 10)
	      printf ("%d %.2f %.2f\n",
		i, surface [snum].points [i].pos [0],
                surface [snum].points [i].pos [1]);
	}
	fclose (fp);
}

void 	write_cnt_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j, idx;
	char	newfile [100];
	int	cnt, current_contour, num_contour_points [10], num_contours;

	sprintf (newfile, "%s.cnt", filename);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_cnt_file) could not open file %s\n", newfile);
     	   exit (-1);
  	}

  	fprintf (fp, "S %d\n", surface [snum].NumSections);
	printf ("%d points to write to %s...\n", surface [snum].num_points, newfile);
	for (i = 0; i < surface [snum].NumSections; i++){
	   num_contours = determine_num_contours (snum, i, num_contour_points);
	   printf ("\tSection %d has %d contours\n", i, num_contours);
	   j = 0;
	   current_contour = 0;
	   idx = surface [snum].sections [i].points [j];
  	   fprintf (fp, "v %d z %f\n", 
	 	num_contour_points [current_contour],	
		surface [snum].points [idx].pos [2]);
	   fprintf (fp, "{\n");
	   while (j < surface [snum].sections [i].num_points){
	      idx = surface [snum].sections [i].points [j];
	      if (surface [snum].points [idx].contour != current_contour){
	         fprintf (fp, "}\n");
	         current_contour++;
  	         fprintf (fp, "v %d z %f\n", 
	 		num_contour_points [current_contour],	
			surface [snum].points [idx].pos [2]);
	         fprintf (fp, "{\n");
	      }
  	      fprintf (fp, "%f %f\n",
			surface [snum].points [idx].pos [0],
			surface [snum].points [idx].pos [1]);
	      j++;
	   }
	   fprintf (fp, "}\n");
	}
	fclose (fp);
}

void 	write_sort_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	char	newfile [100];

	sprintf (newfile, "%s.sort", filename);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_sort_file) could not open file %s\n", newfile);
     	   exit (-1);
  	}

  	fprintf (fp, "%d\n", surface [snum].num_points);
	printf ("%d points to write to %s...\n", surface [snum].num_points, newfile);
	for (i = 0; i < surface [snum].num_points; i++){
	   if (strlen (surface [snum].points [i].region) == 0)
	      strcpy (surface [snum].points [i].region, "???");
#ifdef NEW
  	   fprintf (fp, "%d %d %d %d %d %d %s\n", 
	    	i, surface [snum].points [i].num_neighs, 
	   	surface [snum].points [i].section, 
	   	surface [snum].points [i].contour, 
	   	surface [snum].points [i].point, 
	   	surface [snum].points [i].class, 
	   	surface [snum].points [i].region);
#else
  	   fprintf (fp, "%d %d %d %d %d %s\n", 
	    	i, surface [snum].points [i].num_neighs, 
	   	surface [snum].points [i].section, 
	   	surface [snum].points [i].point, 
	   	surface [snum].points [i].class, 
	   	surface [snum].points [i].region);
#endif
  	   /*printf ("%d %d %d %d %s\n", 
	    	i, surface [snum].points [i].num_neighs, 
	   	surface [snum].points [i].section, 
	   	surface [snum].points [i].point, 
	   	surface [snum].points [i].region);*/
	   for (j = 0; j < surface [snum].points [i].num_neighs; j++)
	      fprintf (fp, "%d %d\n", j, surface [snum].points [i].neighs [j]); 
	}
	fclose (fp);
}

void 	read_coord_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	float	f1, f2, f3;
	int	i1, i2, i3, i4, i5, i6;
	char	s1 [5];
	int	cnt;

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_coord_file) could not open file %s\n", filename);
     	   exit (-1);
  	}
  	fscanf (fp, "%d", &surface [snum].num_points);
	printf ("%d points to read from coord %s to snum %d...\n", 
		surface [snum].num_points, filename, snum);
	if (surface [snum].num_points > MAX_POINTS){
	   printf ("ERROR: %d Exceeded max # of points %d\n",
		surface [snum].num_points, MAX_POINTS);
	   exit (-1);
	}
	InitStuff (snum);
	cnt = 0;
	for (i = 0; i < surface [snum].num_points; i++){
	   if ((i % 5000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points);
#ifdef NEW
  	   fscanf (fp, "%d %f %f %f",  &i1, &f1, &f2, &f3);
  	   /*printf ("%d %f %f %f\n",  i1, f1, f2, f3);*/
#else
  	   fscanf (fp, "%d %f %f %f %d %d %d %d %s %d %d",  
		&i1, &f1, &f2, &f3, &i1, &i2, &i3, &i4, s1, &i5, &i6);
#endif
	   if ((i % SKIP) == 0){
	      double	TransformX2Image (double);
	      double	TransformY2Image (double);

	      surface [snum].points [cnt].pos [0] = f1;
	      surface [snum].points [cnt].pos [1] = f2;
	      surface [snum].points [cnt].pos [2] = f3;

	      /*surface [snum].points [cnt].pos [0] = TransformX2Image (f1);
	      surface [snum].points [cnt].pos [1] = TransformY2Image (f2);*/

	      surface [snum].points [cnt].display_flag = True;
	      cnt++; 
	   }
	}
	surface [snum].num_points = cnt;
	printf ("%d points for surface...\n", cnt);
	fclose (fp);
}

void 	read_sort_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	int	i1, i2, i3, i4, i5, i6;
	char	s1 [10];

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_sort_file) could not open file %s\n", filename);
     	   exit (-1);
  	}

  	fscanf (fp, "%d", &surface [snum].num_points);
	printf ("%d points to read from sort %s...\n", surface [snum].num_points, filename);
	if (surface [snum].num_points > MAX_POINTS){
	   printf ("ERROR: %d Exceeded max # of points %d\n",
		surface [snum].num_points, MAX_POINTS);
	   exit (-1);
	}
	for (i = 0; i < surface [snum].num_points; i++){
	   if ((i % 5000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points);
#ifdef NEW 
  	   /*fscanf (fp, "%d %d %d %d %d %d %s", &i1, &i2, &i3, &i6, &i4, &i5, s1);*/
  	   /*printf ("%d %d %d %d %d %d %s\n", i1, i2, i3, i6, i4, i5, s1);*/
  	   fscanf (fp, "%d %d %d %d %d %s", &i1, &i2, &i3, &i4, &i5, s1);
	   /*surface [snum].points [i].contour = i6;*/
#else
  	   fscanf (fp, "%d %d %d %d %d %s", &i1, &i2, &i3, &i4, &i5, s1);
#endif
	   surface [snum].points [i].num_neighs = i2;
	   if (surface [snum].points [i].num_neighs > MAX_NEIGHS){
	      printf ("ERROR: %d Exceeded max # of neighbors %d > %d\n",
		i, surface [snum].points [i].num_neighs, MAX_NEIGHS);
	      exit (-1);
	   }
	   surface [snum].points [i].Index = i1; 
	   surface [snum].points [i].section = i3;
	   surface [snum].points [i].point = i4; 
	   surface [snum].points [i].class = i5; 
	   strcpy (surface [snum].points [i].region, s1);
	   for (j = 0; j < surface [snum].points [i].num_neighs; j++){
	      fscanf (fp, "%d %d", &i1, &i2); 
	      surface [snum].points [i].neighs [j] = i2;
	   }
	}
	fclose (fp);
}

void 	read_sort_file_old (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	int	i1, i2, i3, i4, i5, i6;
	char	s1 [10];

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_sort_file) could not open file %s\n", filename);
     	   exit (-1);
  	}

  	fscanf (fp, "%d", &surface [snum].num_points);
	printf ("%d points to read from sort %s...\n", surface [snum].num_points, filename);
	if (surface [snum].num_points > MAX_POINTS){
	   printf ("ERROR: %d Exceeded max # of points %d\n",
		surface [snum].num_points, MAX_POINTS);
	   exit (-1);
	}
	for (i = 0; i < surface [snum].num_points; i++){
	   if ((i % 5000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points);
#ifdef NEW 
  	   fscanf (fp, "%d %d %d %d %d %d %s", &i1, &i2, &i3, &i6, &i4, &i5, s1);
  	   printf ("%d %d %d %d %d %d %s\n", i1, i2, i3, i6, i4, i5, i6, s1);
	   surface [snum].points [i].contour = i6;
#else
  	   fscanf (fp, "%d %d %d %d %d %s", &i1, &i2, &i3, &i4, &i5, s1);
#endif
	   surface [snum].points [i].num_neighs = i2;
	   if (surface [snum].points [i].num_neighs > MAX_NEIGHS){
	      printf ("ERROR: %d Exceeded max # of neighbors %d > %d\n",
		i, surface [snum].points [i].num_neighs, MAX_NEIGHS);
	      exit (-1);
	   }
	   surface [snum].points [i].Index = i1; 
	   surface [snum].points [i].section = i3;
	   surface [snum].points [i].point = i4; 
	   surface [snum].points [i].class = i5; 
	   /*if (i5 != 0)
	      printf ("EDGE %d %d %d %d %s\n", i, 
	   	surface [snum].points [i].num_neighs, 
	   	surface [snum].points [i].section, 
	   	surface [snum].points [i].point, 
	   	surface [snum].points [i].region);*/
	   strcpy (surface [snum].points [i].region, s1);
	   /*printf ("%d %d %d %d %s\n", i, 
	   	surface [snum].points [i].num_neighs, 
	   	surface [snum].points [i].section, 
	   	surface [snum].points [i].point, 
	   	surface [snum].points [i].region);*/
			
	   for (j = 0; j < surface [snum].points [i].num_neighs; j++){
	      fscanf (fp, "%d %d", &i1, &i2); 
	      /*printf ("\t%d %d\n", i1, i2); */
	      surface [snum].points [i].neighs [j] = i2;
	   }
	}
	fclose (fp);
}

void    ReadParamFile (int snum, char *file)
{
	int	i, j;
        FILE    *fd_params;
        char    line [MAXLINE], temp_string [MAXLINE];
	char	aux_file [100];
	char	curvature_file [100], 
		projcells_file [100], 
		area2D_file [100], area3D_file [100];
	char    param_string [][80] = {
                "coord_file",
                "sort_file",
                "border_file",
                "cell_file",
                "polygon_file",
                "3Dcoord_file",
                "off_file",
                "aux_file",
                "curvature_file",
		"2Darea_file",
		"3Darea_file",
		"projcells_file"
	};
        float        distinct_z [100];
        int          num_sections;

        printf ("Open parameter file %s\n", file);
        if ((fd_params = fopen (file, "r")) == NULL){
           printf ("ERROR: Can't read params file %s\n", file);
           exit (-1);
        }
        while (fgets (line, MAXLINE, fd_params) != NULL){
           if (line [0] != '#'){
              if (strncmp (line, param_string [0], strlen (param_string [0])) == 0){
                 sscanf (line, "%s %s\n", temp_string, surface [snum].coord_file);
                 printf ("Coord file %s\n", surface [snum].coord_file);
              }
              else if (strncmp (line, param_string [1], strlen (param_string [1])) == 0){
                 sscanf (line, "%s %s\n", temp_string, surface [snum].sort_file);
                 printf ("Sort file %s\n", surface [snum].sort_file);
              }
              else if (strncmp (line, param_string [2], strlen (param_string [2])) == 0){
                 sscanf (line, "%s %s\n", temp_string, surface [snum].border_file);
                 printf ("Border file %s\n", surface [snum].border_file);
              }
              else if (strncmp (line, param_string [3], strlen (param_string [3])) == 0){
                 sscanf (line, "%s %s\n", temp_string, surface [snum].cell_file);
                 printf ("Cell file %s\n", surface [snum].cell_file);
              }
              else if (strncmp (line, param_string [4], strlen (param_string [4])) == 0){
                 sscanf (line, "%s %s\n", temp_string, surface [snum].polygon_file);
                 printf ("Polygon file %s\n", surface [snum].polygon_file);
              }
              else if (strncmp (line, param_string [5], strlen (param_string [5])) == 0){
                 sscanf (line, "%s %s\n", temp_string, surface [snum].coord3D_file);
                 printf ("3D coordinate file %s\n", surface [snum].coord3D_file);
              }
              else if (strncmp (line, param_string [6], strlen (param_string [6])) == 0){
                 sscanf (line, "%s %s\n", temp_string, surface [snum].off_file);
                 printf ("OFF file %s\n", surface [snum].off_file);
              }
              else if (strncmp (line, param_string [7], strlen (param_string [7])) == 0){
                 sscanf (line, "%s %s\n", temp_string, aux_file);
                 printf ("AUX file %s\n", aux_file);
              }
              else if (strncmp (line, param_string [8], strlen (param_string [8])) == 0){
                 sscanf (line, "%s %s\n", temp_string, curvature_file);
                 printf ("Curvature file %s\n", curvature_file);
              }
              else if (strncmp (line, param_string [9], strlen (param_string [9])) == 0){
                 sscanf (line, "%s %s\n", temp_string, area2D_file);
                 printf ("2D Area file %s\n", area2D_file);
              }
              else if (strncmp (line, param_string [10], strlen (param_string [10])) == 0){
                 sscanf (line, "%s %s\n", temp_string, area3D_file);
                 printf ("3D Area file %s\n", area3D_file);
              }
              else if (strncmp (line, param_string [11], strlen (param_string [11])) == 0){
                 sscanf (line, "%s %s\n", temp_string, projcells_file);
                 printf ("Cell Projection file %s\n", projcells_file);
              }
           }
        }
        if (strlen (surface [snum].off_file) > 0){
           read_off_file (surface [snum].off_file, snum);
           read_cell_file (surface [snum].cell_file, snum);
	   read_border_file (surface [snum].border_file, snum);
	   match_points_to_triangles (snum);
           convert_off_file_new (snum);
           /*convert_off_file (snum);*/
           num_sections = determine_num_sections (snum, distinct_z);
           printf ("%d: Number of sections %d, points %d\n",  
			snum, num_sections, surface [snum].num_points);
           for (i = 0; i < surface [snum].num_points; i++){
              surface [snum].points [i].section =
                        determine_section_num (snum,
                                surface [snum].points [i].pos [2],
                                num_sections, distinct_z);
	      /*printf ("\t%d %d\n", i, surface [snum].points [i].section);*/
	   }
	   /*write_coord_file (surface [snum].off_file, snum);
	   write_sort_file (surface [snum].off_file, snum);*/
        }
	else{
	   int 	flag1, flag2;

           read_coord_file (surface [snum].coord_file, snum);
           read_curvature_file (curvature_file, snum);
	   read_projcells_file (projcells_file, snum);
	   /*read_surface_file (surface [snum].coord_file, snum);*/
           read_3Dcoord_file (surface [snum].coord3D_file, snum);
           read_sort_file (surface [snum].sort_file, snum);
           read_aux_file (aux_file, snum);
	   read_border_file (surface [snum].border_file, snum);
	   /*read_anat_boundaries_file (surface [snum].border_file, snum);*/
	   /*read_ascii_border_file (surface [snum].border_file, snum);*/
           read_cell_file (surface [snum].cell_file, snum);
           read_polygon_file (surface [snum].polygon_file, snum);
           flag1 = read_2Darea_file (area2D_file, snum);
           flag2 = read_3Darea_file (area3D_file, snum);
	   if ((flag1 == True) && (flag2 == True)){
	      compute_distortion (snum);
	   }
           num_sections = determine_num_sections (snum, distinct_z);
           printf ("%d: Number of sections %d, points %d\n",  
			snum, num_sections, surface [snum].num_points);
	   extract_sections (snum, num_sections, distinct_z); 
	}
	find_section_extent (snum);
	/*translate_to_center_of_mass (snum);*/
	find_surface_extent (snum);

	surface [snum].is_view = True;	
	surface [snum].StateChanged = True;
	/*surface [snum].Object = glGenLists (1)*/
	surface [snum].Object = snum+1;
	for (i = 0; i < 4; i++)
	   for (j = 0; j < 4; j++)
              surface [snum].matrix [i][j] = (i == j) ? 1.0 : 0.0;
 
	printf ("Surface %d has object %d\n", snum, surface [snum].Object); 
	num_surfaces++;
}

void	write_border_file (char *file, int snum)
{
	FILE	*fp;
	int	i, j, n;
	char	newfile [100];

	sprintf (newfile, "%s.borders", file);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_border_file) could not open file %s\n", newfile);
     	   return; 
  	}
  	fprintf (fp, "%d\n", surface [snum].NumBorders); 
	printf ("%d borders to write to %s...\n", surface [snum].NumBorders, newfile); 
	for (i = 0; i < surface [snum].NumBorders; i++){ 
	   fprintf (fp, "%d %d\n", i, surface [snum].borders [i].num_links);
	   for (j = 0; j < surface [snum].borders [i].num_links; j++){
  	      n = surface [snum].borders [i].links [j],
  	      fprintf (fp, "%d %d %d %d\n", j, 
			surface [snum].borders [i].links [j],
			surface [snum].points [n].section,
			surface [snum].points [n].point);
  	      /*fprintf (fp, "%d %d %d %d %f %f %f\n", j, 
			surface [snum].borders [i].links [j],
			surface [snum].points [n].section,
			surface [snum].points [n].point,
			surface [snum].points [n].pos [0],
			surface [snum].points [n].pos [1],
			surface [snum].points [n].pos [2]);*/
	   }
	}
	fclose (fp);
}

void	read_ascii_border_file (char *file, int snum)
{
	FILE	*fp;
	int	i, j;
	int	t1, t2, t3, t4;
	int	t5, t6;
	char	s1 [5];

  	if ((fp = fopen (file, "r")) == NULL){;
     	   printf ("ERROR: (read_ascii_border_file) could not open file %s\n", file);
     	   return; 
  	}
  	fscanf (fp, "%d", &surface [snum].NumBorders); 
	if (surface [snum].NumBorders > MAX_BORDERS){
	   printf ("ERROR: %d Exceeded max # of borders %d\n",
		surface [snum].NumBorders, MAX_BORDERS);
	   exit (-1);
	}
	printf ("%d borders to read from %s...\n", surface [snum].NumBorders, file); 
	for (i = 0; i < surface [snum].NumBorders; i++){ 
	   fscanf (fp, "%d %s %d", &t1, s1, &t2);
	   surface [snum].borders [i].num_links = t2;
	   if (surface [snum].borders [i].num_links > MAX_BORDER_LENGTH){
	      printf ("ERROR: %d Exceeded max # of borders %d\n", t2, MAX_BORDER_LENGTH);
	      exit (-1);
	   }
	   t5 = 0;
	   for (j = 0; j < surface [snum].borders [i].num_links; j++){
  	      fscanf (fp, "%d %d %d", &t1, &t3, &t4);

	      /*if ((t3+1) == 85) t3 = 0;*/
	      t2 = find_point (snum, t3, t4);
	
 	      if (t2 > 0)
	         surface [snum].borders [i].links [t5++] = t2;
	   }
	   surface [snum].borders [i].num_links = t5;
	}
	fclose (fp);
}

void	read_border_file (char *file, int snum)
{
	FILE	*fp;
	int	i, j;
	int	t1, t2, t3, t4;
	int	t5, t6;
	float	f [3], dist;

  	if ((fp = fopen (file, "r")) == NULL){;
     	   printf ("ERROR: (read_border_file) could not open file %s\n", file);
     	   return; 
  	}
  	fscanf (fp, "%d", &surface [snum].NumBorders); 
	if (surface [snum].NumBorders > MAX_BORDERS){
	   printf ("ERROR: %d Exceeded max # of borders %d\n",
		surface [snum].NumBorders, MAX_BORDERS);
	   exit (-1);
	}
	printf ("%d borders to read from %s...\n", surface [snum].NumBorders, file); 
	for (i = 0; i < surface [snum].NumBorders; i++){ 
	   fscanf (fp, "%d %d", &t1, &t2);
	   surface [snum].borders [i].num_links = t2;
	   if (surface [snum].borders [i].num_links > MAX_BORDER_LENGTH){
	      printf ("ERROR: %d Exceeded max # of borders %d\n", t2, MAX_BORDER_LENGTH);
	      exit (-1);
	   }
	   t5 = 0;
	   for (j = 0; j < surface [snum].borders [i].num_links; j++){
  	      fscanf (fp, "%d %d %d %d", &t1, &t2, &t3, &t4);

  	      /*fscanf (fp, "%d %d %d %d %f %f %f", 
			&t1, &t2, &t3, &t4, &f [0], &f [1], &f [2]);*/
	      /*t2 = find_point_using_pos (snum, f);*/
	      /*t2 = find_closest_point (snum, f, &dist);*/

	      /*if ((t3+1) == 85) t3 = 0;
	      t2 = find_point (snum, t3+1, t4);
	
 	      if (t2 > 0)*/
	         surface [snum].borders [i].links [t5++] = t2;
	   }
	   surface [snum].borders [i].num_links = t5;
	}
	fclose (fp);
}

read_cell_file (char *file, int snum)
{
	int	i, j;
	char	line [MAXLINE];
	int	t1, t2, t3;
	char	s1 [5], s2 [5];
	float	f1, f2, f3;
	FILE	*fp;
	int	num, num_cells;

  	if ((fp = fopen (file, "r")) == NULL){;
     	   printf ("ERROR: (read_cell_file) could not open file %s\n", file);
     	   return; 
  	}
  	fscanf (fp, "%d", &surface [snum].NumCells); 
	if (surface [snum].NumCells > MAX_CELLS){
	   printf ("ERROR: %d Exceeded max # of cells %d\n",
		surface [snum].NumCells, MAX_CELLS);
	   exit (-1);
	}
	printf ("%d cells to read from %s...\n", surface [snum].NumCells, file); 
	num_cells = 0;
	for (i = 0; i < surface [snum].NumCells; i++){ 
  	   fgets (line, MAXLINE, fp);
  	   num = sscanf (line, "%d %d %d %s %s %f %f %f", 
		&t1, &t2, &t3, s1, s2, &f1, &f2, &f3);
	   if (num != 8){
	      /* We don't want to use this info */
	      printf ("WARNING: Ignore line %d, %d entries\n", i, num); 
	   }
	   else{
	      strcpy (surface [snum].cells [num_cells].area1, s1);
	      strcpy (surface [snum].cells [num_cells].area2, s2);
	      surface [snum].cells [num_cells].display_flag = True;
	      surface [snum].cells [num_cells].section = t2;
	      surface [snum].cells [num_cells].point = t3;
	      surface [snum].cells [num_cells].pos [0] = f1;
	      surface [snum].cells [num_cells].pos [1] = f2;
	      surface [snum].cells [num_cells].pos [2] = f3;
	      if (((strcmp (s1, "E")) != 0) &&
		   (strcmp (s2, "PRP")) != 0)
	         num_cells++;
	   }
	}
	fclose (fp);
	surface [snum].NumCells = num_cells;
	printf ("\n%d CELLS used for Areal Borders...\n", surface [snum].NumCells); 

/*#define	MAP_3D_TO_2D*/
#ifdef MAP_3D_TO_2D
	/* Now find the points on the surface based on position */
	/* SPECIAL CASE FOR FINDING 79-O BORDERS! 1.12.95 */
	{
	int	i, j, t1;

	printf ("79-O: Mapping 3D areal borders onto surface...\n");
	for (i = 0; i < surface [snum].NumCells; i++){
	   if ((i % 50) == 0)
	      printf ("\tCell %d (%d %d) %s %s\n",
			i, surface [snum].cells [i].section,
			surface [snum].cells [i].point,
			surface [snum].cells [i].area1,
			surface [snum].cells [i].area2);
	   t1 = find_point (snum, 
		surface [snum].cells [i].section,
		surface [snum].cells [i].point);
	   for (j = 0; j < 3; j++)
	      surface [snum].cells [i].pos [j] = surface [snum].points [t1].pos [j]; 
	}
	/*write_cell_file ("dve79.cells", 0);*/
	}
#endif
/*#define CREATE_AREAL_NEW*/
#ifdef CREATE_AREAL_NEW 
	/* Now find the points on the surface based on position */
	/* SPECIAL CASE FOR FINDING 79-O BORDERS! 1.12.95 */
	{
	int	i, j;
	float	dist;
	int	closest;

	printf ("79-O: Creating areal.new file...\n"); 
	for (i = 0; i < surface [snum].NumCells; i++){
	   if ((i % 10) == 0)
	      printf ("\tCell %d (%d %d) %s %s\n",
			i, surface [snum].cells [i].section,
			surface [snum].cells [i].point,
			surface [snum].cells [i].area1,
			surface [snum].cells [i].area2);
	   closest = find_closest_point (snum, surface [snum].cells [i].pos, &dist); 
   	   /*printf ("\tOLD: %.2f %.2f %.2f\n",
	      	surface [snum].cells [i].pos [0],
	      	surface [snum].cells [i].pos [1],
	      	surface [snum].cells [i].pos [2]);
   	   printf ("\tCLS: %4d %.2f %.2f %.2f\n",
	      	closest, surface [snum].points [closest].pos [0],
	      	surface [snum].points [closest].pos [1],
	      	surface [snum].points [closest].pos [2]);*/
	   surface [snum].cells [i].section = surface [snum].points [closest].section;
	   surface [snum].cells [i].point = surface [snum].points [closest].point;
	   for (j = 0; j < 3; j++)
	      surface [snum].cells [i].pos [j] = surface [snum].points [closest].pos [j]; 
	}
	write_cell_file ("areal.new", 0);
	}
#endif
}

write_cell_file (char *file, int snum)
{
	int	i;
	FILE	*fp;
	char	newfile [100];

	sprintf (newfile, "%s.cells", file);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_cell_file) could not open file %s\n", newfile);
     	   return; 
  	}
  	fprintf (fp, "%d\n", surface [snum].NumCells); 
	printf ("%d cells to write to %s...\n", surface [snum].NumCells, newfile); 
	for (i = 0; i < surface [snum].NumCells; i++){
  	   fprintf (fp, "%d %d %d %s %s %f %f %f\n", 
		i, 
	      	surface [snum].cells [i].section, 
	      	surface [snum].cells [i].point, 
	      	surface [snum].cells [i].area1,
	      	surface [snum].cells [i].area2, 
	      	surface [snum].cells [i].pos [0],
	      	surface [snum].cells [i].pos [1], 
	      	surface [snum].cells [i].pos [2]); 
	}
	fclose (fp);
}

void	write_polygon_file ();
void	read_polygon_file (char *filename, int snum)
{
	int	i, i1, i2, i3;
	FILE	*fp;

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_polygon_file) could not open file %s\n", filename);
     	   return; 
  	}
  	fscanf (fp, "%d", &surface [snum].NumPolygons);
	printf ("%d polygons to read from polygon file %s...\n", surface [snum].NumPolygons, filename);
	if (surface [snum].NumPolygons > MAX_POLYS){
	   printf ("ERROR: %d Exceeded max # of polygons %d\n",
		surface [snum].NumPolygons, MAX_POLYS);
	   exit (-1); 
	}
	for (i = 0; i < surface [snum].NumPolygons; i++){
	   if ((i % 10000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].NumPolygons);
  	   fscanf (fp, "%d %d %d",  &i1, &i2, &i3);
	   surface [snum].tiles [i].triangle [0] = i1;
	   surface [snum].tiles [i].triangle [1] = i2;
	   surface [snum].tiles [i].triangle [2] = i3;
	}
	fclose (fp);

	/*normal_constistency_check (snum);
	write_polygon_file ("newpoly", snum);*/

	compute_normals (snum);
	/*check_triangles (snum);*/
}

void	write_polygon_file (char *filename, int snum)
{
	int	i, i1, i2, i3;
	FILE	*fp;
	char	newfile [100];

	sprintf (newfile, "%s.polygons", filename);
  	if ((fp = fopen (newfile, "w")) == NULL){;
     	   printf ("ERROR: (write_polygon_file) could not open file %s\n", newfile);
     	   return; 
  	}
  	fprintf (fp, "%d\n", surface [snum].NumPolygons);
	printf ("%d polygons to write to polygon file %s...\n", 
		surface [snum].NumPolygons, newfile);
	for (i = 0; i < surface [snum].NumPolygons; i++){
  	   fprintf (fp, "%d %d %d\n",  
	   	surface [snum].tiles [i].triangle [0],
	   	surface [snum].tiles [i].triangle [1],
	   	surface [snum].tiles [i].triangle [2]); 
	}
	fclose (fp);
}

void 	read_3Dcoord_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	float	f1, f2, f3;
	int	i1, i2, i3, i4, i5, i6;
	char	s1 [5];

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_3Dcoord_file) could not open file %s\n", filename);
     	   return; 
  	}
  	fscanf (fp, "%d", &i1);
	if (i1 != surface [snum].num_points)
	   printf ("WARNING: Number of points in 3D %d file does not correspond %d\n",
		i1, surface [snum].num_points); 
	printf ("%d points to read from 3D coord %s...\n", i1, filename); 
	if (i1 > MAX_POINTS){
	   printf ("ERROR: %d Exceeded max # of points %d\n", i1, MAX_POINTS);
	   return; 
	}
	for (i = 0; i < surface [snum].num_points; i++){
	   if ((i % 5000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points);
  	   /*fscanf (fp, "%d %f %f %f",  &i1, &f1, &f2, &f3);*/
  	   fscanf (fp, "%d %f %f %f %d %d %d %d %s %d %d",  
		&i1, &f1, &f2, &f3, &i1, &i2, &i3, &i4, s1, &i5, &i6);
	   surface [snum].points [i].pos3D [0] = f1;
	   surface [snum].points [i].pos3D [1] = f2;
	   surface [snum].points [i].pos3D [2] = f3;
	}
	fclose (fp);
}

void read_off_file (char *file, int snum)
{
	int	i;
	float	tf1, tf2, tf3;
	FILE	*fp;
	char	line [100];
	int	temp;

	printf ("Reading file %s...\n", file);
        if ((fp = fopen (file, "r")) == NULL){;
           printf ("ERROR: (read_file) could not open file %s\n", file);
           exit (-1);
        }
    	fgets (line, MAXLINE, fp);
    	fgets (line, MAXLINE, fp);
    	fgets (line, MAXLINE, fp);
    	fgets (line, MAXLINE, fp);
    	fgets (line, MAXLINE, fp);
    	fgets (line, MAXLINE, fp);
    	sscanf (line, "%d %d %d", 
		&surface [snum].num_points, &surface [snum].NumPolygons, &temp); 
	printf ("%d vertices, %d triangles\n", 
		surface [snum].num_points, surface [snum].NumPolygons);
	if (surface [snum].num_points > MAX_POINTS){
	   printf ("ERROR: Exceeded MAX_POINTS %d points\n", MAX_POINTS);
	   exit (-1);
	}
	if (surface [snum].NumPolygons > MAX_POLYS){
	   printf ("ERROR: Exceeded max # of triangles %d\n", MAX_POLYS);
	   exit (-1);
	}
	for (i = 0; i < surface [snum].num_points; i++){
	   if ((i % 5000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points);
    	   fgets (line, MAXLINE, fp);
    	   sscanf (line, "%f %f %f %f %f %f",
	        &surface [snum].points [i].pos [0], 
	        &surface [snum].points [i].pos [1], 
	        &surface [snum].points [i].pos [2], 
	        &surface [snum].points [i].normal [0], 
	        &surface [snum].points [i].normal [1], 
	        &surface [snum].points [i].normal [2]);
	}
	for (i = 0; i < surface [snum].NumPolygons; i++){
	   if ((i % 10000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].NumPolygons);
    	   fgets (line, MAXLINE, fp);
    	   sscanf (line, "%d %d %d %d",
		&temp, 
		&surface [snum].tiles [i].triangle [0],
		&surface [snum].tiles [i].triangle [1],
		&surface [snum].tiles [i].triangle [2]);
	}
}

void	convert_off_file_new (int snum)
{
        int     i, j, k;
        int     temp_neighbors [MAX_NEIGHS];
        int     num, p0, p1, p2;

        printf ("Converting OFF file...\n");
        for (i = 0; i < surface [snum].num_points; i++){
           if ((i % 5000) == 0) 
	      printf ("\tpoint %5d of %5d\n", i, surface [snum].num_points);
           surface [snum].points [i].num_neighs = 0;
           num = 0;
           for (k = 0; k < pt_tris [i].num_tris; k++){
	      j = pt_tris [i].tris [k];
	      p0 = surface [snum].tiles [j].triangle [0];
	      p1 = surface [snum].tiles [j].triangle [1];
	      p2 = surface [snum].tiles [j].triangle [2];
	      /*printf ("\t%d: %d %d %d\n", i, p0, p1, p2);*/
              if (p0 == i){
                 if ((is_neighbor (p1, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p1;
                 }
                 if ((is_neighbor (p2, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p2;
                 }
              }
              if (p1 == i){
                 if ((is_neighbor (p0, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p0;
                 }
                 if ((is_neighbor (p2, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p2;
                 }
              }
              if (p2 == i){
                 if ((is_neighbor (p0, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p0;
                 }
                 if ((is_neighbor (p1, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p1;
		 }
	      }
	   }
           surface [snum].points [i].num_neighs = num;
	   /*printf ("Point %d has %d neighs\n", i, num);*/
           for (j = 0; j < surface [snum].points [i].num_neighs; j++){
              surface [snum].points [i].neighs [j] = temp_neighbors [j];
	      /*printf ("\t%d %d\n", j, temp_neighbors [j]);*/
	   }
	}
}

void	convert_off_file (int snum)
{
        int     i, j;
        int     temp_neighbors [MAX_NEIGHS];
        int     num, p0, p1, p2;

        printf ("Converting OFF file...\n");
        for (i = 0; i < surface [snum].num_points; i++){
           if ((i % 500) == 0) 
	      printf ("\tpoint %5d of %5d\n", i, surface [snum].num_points);
           surface [snum].points [i].num_neighs = 0;
           num = 0;
           for (j = 0; j < surface [snum].NumPolygons; j++){
	      p0 = surface [snum].tiles [j].triangle [0];
	      p1 = surface [snum].tiles [j].triangle [1];
	      p2 = surface [snum].tiles [j].triangle [2];
	      /*printf ("\t%d: %d %d %d\n", i, p0, p1, p2);*/
              if (p0 == i){
                 if ((is_neighbor (p1, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p1;
                 }
                 if ((is_neighbor (p2, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p2;
                 }
              }
              if (p1 == i){
                 if ((is_neighbor (p0, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p0;
                 }
                 if ((is_neighbor (p2, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p2;
                 }
              }
              if (p2 == i){
                 if ((is_neighbor (p0, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p0;
                 }
                 if ((is_neighbor (p1, num, temp_neighbors)) == False){
                    temp_neighbors [num++] = p1;
		 }
	      }
	   }
           surface [snum].points [i].num_neighs = num;
	   /*printf ("Point %d has %d neighs\n", i, num);*/
           for (j = 0; j < surface [snum].points [i].num_neighs; j++){
              surface [snum].points [i].neighs [j] = temp_neighbors [j];
	      /*printf ("\t%d %d\n", j, temp_neighbors [j]);*/
	   }
	}
}

int     is_neighbor (idx, num, list)

int     idx, num;
int     *list;
{
        int     i;

        for (i = 0; i < num; i++){
           if (list [i] == idx)
              return (True);
        }
        return (False);
}

int     is_in_list (idx, num, list)

int     idx, num, *list;
{
        int     i;

        for (i = 0; i < num; i++){
           if (list [i] == idx){
              return (True);
           }
        }
        return (False);
}

int     float_is_in_list (float val, int num, float *list)
{
        int     i;

        for (i = 0; i < num; i++){
           if (list [i] == val)
              return (True);
        }
        return (False);
}

int     determine_num_sections (int snum, float *distinct_z)
{
        int     num_z = 0;
        int     i, pos;

        distinct_z [0] = surface [snum].points [0].pos [2];
        /*printf ("%d %f\n", num_z, distinct_z [num_z]);*/
        num_z++;
        for (i = 0; i < surface [snum].num_points; i++){
           if (float_is_in_list (surface [snum].points [i].pos [2], 
			num_z, distinct_z) == False){
              distinct_z [num_z] = surface [snum].points [i].pos [2];
              /*printf ("%d %f\n", num_z, distinct_z [num_z]);*/
              num_z++;
           }
        }
	/*printf ("%d sections\n", num_z);*/
        return (num_z);
}

void match_points_to_triangles (int snum)
{
	int 	i, j;
	int	v1, v2, v3;
	int	n1, n2, n3;
	
	for (i = 0; i < surface [snum].num_points; i++)
	   pt_tris [i].num_tris = 0;
	printf ("Assigning triangles to points...\n");
	for (i = 0; i < surface [snum].NumPolygons; i++){
	   if ((i % 5000) == 0)
	      printf ("\t%5d of %5d\n", i, surface [snum].NumPolygons);
	   v1 = surface [snum].tiles [i].triangle [0];
	   v2 = surface [snum].tiles [i].triangle [1];
	   v3 = surface [snum].tiles [i].triangle [2];
	   n1 = pt_tris [v1].num_tris;
	   n2 = pt_tris [v2].num_tris;
	   n3 = pt_tris [v3].num_tris;

	   if ((is_in_list (i, n1, pt_tris [v1].tris)) == False)
	      pt_tris [v1].tris [n1] = i;
	   if ((is_in_list (i, n2, pt_tris [v2].tris)) == False)
	      pt_tris [v2].tris [n2] = i;
	   if ((is_in_list (i, n3, pt_tris [v3].tris)) == False)
	      pt_tris [v3].tris [n3] = i;

	   pt_tris [v1].num_tris++;
	   if (pt_tris [v1].num_tris > MAX_PT_TRIS){
	      printf ("ERROR: Point %d exceeds max # of tris %d\n", v1, MAX_PT_TRIS);
	      for (j = 0; j < pt_tris [v1].num_tris; j++)
	         printf ("\t%d %d\n", j, pt_tris [v1].tris [j]);
	      exit (-1);
	   }
	   pt_tris [v2].num_tris++;
	   if (pt_tris [v2].num_tris > MAX_PT_TRIS){
	      printf ("ERROR: Point %d exceeds max # of tris %d\n", v2, MAX_PT_TRIS);
	      for (j = 0; j < pt_tris [v2].num_tris; j++)
	         printf ("\t%d %d\n", j, pt_tris [v2].tris [j]);
	      exit (-1);
	   }
	   pt_tris [v3].num_tris++;
	   if (pt_tris [v3].num_tris > MAX_PT_TRIS){
	      printf ("ERROR: Point %d exceeds max # of tris %d\n", v3, MAX_PT_TRIS);
	      for (j = 0; j < pt_tris [v3].num_tris; j++)
	         printf ("\t%d %d\n", j, pt_tris [v3].tris [j]);
	      exit (-1);
	   }
	}
}

void 	read_aux_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, j;
	int	i1, i2, i3, i4, i5, i6, i7, i8, i9, prev;
	char	s1 [5];

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_aux_file) could not open file %s\n", filename);
     	   return; 
  	}

  	fscanf (fp, "%d", &i6);
	printf ("%d points to read from sort %s...\n", i6, filename);
	/*for (i = 0; i < i6; i++)
	   strcpy (surface [snum].points [i].region, "HEM");*/
	prev = i7 = 0;
	for (i = 0; i < i6; i++){
	   if ((i % 1000) == 0) 
	      printf ("\t%5d of %5d\n", i, i6); 
  	   fscanf (fp, "%d %d %d %d %d %s", &i1, &i2, &i3, &i4, &i5, s1);

	   prev = i7; 
           i7 = find_point (snum, i3, i4, prev);

	   /*printf ("New %d %d (%d,%d) gets %s\n", i, i7, i3, i4, s1);*/
	   strcpy (surface [snum].points [i7].region, s1);
	   for (j = 0; j < i2; j++){
	      fscanf (fp, "%d %d", &i8, &i9); 
	   }
	}
	fclose (fp);
}

void 	read_projcells_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, i1;
	float	f1;

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_projcells_file) could not open file %s\n", filename);
     	   return; 
  	}

	for (i = 0; i < surface [snum].num_points; i++){
	   if ((i % 10000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points); 
  	   fscanf (fp, "%d %f", &i1, &f1);
	   surface [snum].points [i].projcells = f1;
	}
	fclose (fp);
}

void 	read_curvature_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, i1;
	float	f1;

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_curvature_file) could not open file %s\n", filename);
     	   return; 
  	}

	for (i = 0; i < surface [snum].num_points; i++){
	   if ((i % 10000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points); 
  	   fscanf (fp, "%d %f", &i1, &f1);
	   surface [snum].points [i].curvature = f1;
  	   /*printf ("%d %f", i, surface [snum].points [i].curvature); */
	}
	fclose (fp);
}

int	read_2Darea_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, i1;
	float	f1;

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_2Darea_file) could not open file %s\n", filename);
     	   return (False); 
  	}
	printf ("Read 2D areas from %s\n", filename);
	for (i = 0; i < surface [snum].NumPolygons; i++){
	   if ((i % 10000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points); 
  	   fscanf (fp, "%d %f", &i1, &f1);
	   surface [snum].tiles [i].area2D = f1;
	}
	fclose (fp);
	return (True);
}

int	read_3Darea_file (char *filename, int snum)
{
	FILE	*fp;
	int	i, i1;
	float	f1;

  	if ((fp = fopen (filename, "r")) == NULL){;
     	   printf ("ERROR: (read_3Darea_file) could not open file %s\n", filename);
	   return (False); 
  	}
	printf ("Read 3D areas from %s\n", filename);
	for (i = 0; i < surface [snum].NumPolygons; i++){
	   if ((i % 10000) == 0) 
	      printf ("\t%5d of %5d\n", i, surface [snum].num_points); 
  	   fscanf (fp, "%d %f", &i1, &f1);
	   surface [snum].tiles [i].area3D = f1;
	}
	fclose (fp);
	return (True); 
}

void	read_anat_boundaries_file (char *file, int snum)
{
        char    line [100];
        FILE    *fp;
        int     t1, t2, t3;
        int     i, j, pt;
        char    name [MAXLINE];
	int	cur;

        printf ("Attempting to read ANAT boundaries file: %s\n", file);
        if ((fp = fopen (file, "r")) == NULL){;
           printf ("ERROR: (read_anat_boundaries_file) could not open file %s\n", file);
           return;
        }
        cur = surface [snum].NumBorders = 0;
        while (fgets (line, MAXLINE, fp) != NULL){
           sscanf (line, "%d %s %d", &t1, name, &t2);
	   printf ("Border %d %s %d\n", t1, name, t2);
	   surface [snum].borders [cur].num_links = t2;
           for (i = 0; i < surface [snum].borders [cur].num_links; i++){
              fgets (line, MAXLINE, fp);
              sscanf (line, "%d %d %d", &t1, &t2, &t3);
	      pt = find_point (snum, t2-1, t3);
	      printf ("\t%d %d %d -> %d\n", t1, t2-1, t3, pt);
              /*pt = find_point (t2-1, t3);*/
              if (pt < 0){
                 printf ("WARNING: Can't find point %d,%d in structure\n", t2, t3);
                 exit (-1);
              }
	      surface [snum].borders [cur].links [i] = pt;
           }
	   cur++;
           if (cur > MAX_BORDERS){
              printf ("ERROR: Exceeded MAX_BORDERS %d\n", MAX_BORDERS);
              exit (-1);
           }
        }
        surface [snum].NumBorders = cur;
        printf ("\n%d boundaries read...\n", surface [snum].NumBorders);
}

void	extract_sections (int snum, int num_sections, float *distinct_z)
{
	int	i, sec, num;

	surface [snum].NumSections = num_sections;
	if (surface [snum].NumSections > MAX_SECTIONS){
	   printf ("ERROR: Exceeded MAX # of sections %d (%d)\n", 
		MAX_SECTIONS, surface [snum].NumSections);
	   exit (-1);
	}
	for (i = 0; i < surface [snum].num_points; i++){
	   sec = surface [snum].points [i].section;
	   num = surface [snum].sections [sec].num_points;
	   surface [snum].sections [sec].points [num] = i;
	   surface [snum].sections [sec].num_points++;
	}
	for (i = 0; i < surface [snum].NumSections; i++)
	   printf ("Section %4d: %4d points\n", i, surface [snum].sections [i].num_points);
}

int	determine_num_contours (int snum, int sect, int *npoints)
{
	int	i;
	int	idx, prev = -1, num_contours = 0;

		
	idx = surface [snum].sections [sect].points [0];
	prev = surface [snum].points [idx].contour;
	npoints [num_contours] = 0;
	for (i = 0; i < surface [snum].sections [sect].num_points; i++){
	   idx = surface [snum].sections [sect].points [i];
	   if (surface [snum].points [prev].contour != surface [snum].points [idx].contour){
	      prev = idx; 
	      num_contours++;
	      npoints [num_contours] = 0;
	   }
	   npoints [num_contours]++;
	}
	num_contours++;
	return (num_contours);
}

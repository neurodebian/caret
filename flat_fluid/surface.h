
#include	"surface_defs.h"

typedef	struct{
	float 	pos3D [3];	
	float 	pos [3];	
	float 	normal [3];	
	short	num_neighs;
	short	num_tris;
	int	neighs [MAX_NEIGHS];
	int	section, point, contour, class, Index;
	char	region [5];
	GLbyte	colors [3];
	float	curvature,
		projcells,
		distortion;
	GLuint	display_flag;
}Point2;

typedef struct{
	int	num_links;
	int	links [MAX_BORDER_LENGTH];
}Border;

typedef struct{
	int	section, point;
	char	area1 [5], area2 [5];
	float	pos [3];
	GLbyte	colors [3];
	short	display_flag;
}Cell;

typedef struct{
	int	triangle [3];	
	int	visited;	
	float	area3D,
		area2D,
		distortion;
}Poly;

typedef struct{
	int	num_points;
	int	points [MAX_SECTION_POINTS]; 
	int	num_contours;
}Section;

struct Surface{
	int	StateChanged;
	GLuint	Object;
	float	matrix [4][4];

	char	coord_file [MAXLINE];
	char	coord3D_file [MAXLINE];
	char	sort_file [MAXLINE];
	char	border_file [MAXLINE];
	char	cell_file [MAXLINE];
	char	polygon_file [MAXLINE];
	char	off_file [MAXLINE];

	int	is_view;

	int	lo_section, hi_section;

	float	minx, maxx;
	float	miny, maxy;
	float	minz, maxz;

	int	num_points;
	Point2	points [MAX_POINTS];

	int	NumBorders;
	Border	borders [MAX_BORDERS];

	int	NumCells;
	Cell 	cells [MAX_CELLS];

	int	NumPolygons;
	Poly	tiles [MAX_POLYS];

	int	NumSections;
	Section	sections [MAX_SECTIONS];
}surface [MAX_SURFACES];

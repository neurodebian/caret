typedef unsigned char GLbyte;
typedef unsigned int  GLuint;

#define         MAX_NEIGHS      20
#define         MAX_POINTS      60000
#define		MAX_SECTIONS	200
#define		MAX_SECTION_POINTS	5000
#define         MAX_SURFACES    2
#define         MAXLINE         200

#define		MAX_BORDER_LENGTH	100	
#define		MAX_BORDERS	 	100

#define		MAX_CELLS		1	

#define		MAX_POLYS		MAX_POINTS*2	

#define         True    1
#define         False   0

#define         VIEWING_MODE    	0
#define         INSERT_MODE     	1
#define         DELETE_ALL_MODE 	2
#define         DELETE_LINK_MODE 	3
#define		BORDER_ADD_MODE		4
#define		BORDER_DELETE_MODE	5
#define		BORDER_FILL_MODE	6
#define		ID_MODE			7
#define		BORDER_DELETE_LINK_MODE	8

#define 	MAXSELECT 	100
#define 	PI      	3.141593

#define         DISP_POINT      1
#define         DISP_LINKS      2
#define         DISP_FILLED	4
#define         DISP_SPHERE     8
#define		DISP_BORDERS	16
#define		DISP_CELLS	32
#define		DISP_AREAS	64
#define		DISP_CURVATURE	128	
#define		DISP_DISTORTION 256	
#define		DISP_PROJECT_CELLS 512

#define		CELL_POINT	0
#define		CELL_SPHERE	1


#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "StatisticNumericalRecipes.h"

static const int NR_END = 1;

float 
StatisticNumericalRecipes::SIGN(const float a, const float b) 
{ 
   return ((b) >= 0.0 ? std::fabs(a) : -std::fabs(a)); 
}

float 
StatisticNumericalRecipes::FMAX(const float a, const float b)
{ 
   return std::max(a, b);
   float maxarg1,maxarg2;
   return (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
           (maxarg1) : (maxarg2));
}

int 
StatisticNumericalRecipes::IMIN(const int a, const int b)
{
   return std::min(a, b);
   int iminarg1,iminarg2;
   return (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
           (iminarg1) : (iminarg2));
}

float 
StatisticNumericalRecipes::SQR(const float a)
{
   float sqrarg;
   return ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg);
}

float*
StatisticNumericalRecipes::vector(long nl, long nh) throw (StatisticException)
/* allocate a float vector with subscript range v[nl..nh] */
{
        float *v;

        v=(float *)std::malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));
        if (!v) throw StatisticException("allocation failure in vector()");
        return v-nl+NR_END;
}

void 
StatisticNumericalRecipes::free_vector(float *v, long nl, long /*nh*/)
/* free a float vector allocated with vector() */
{
        std::free((char*) (v+nl-NR_END));
}

float 
StatisticNumericalRecipes::pythag(float a, float b)
{
	float absa,absb;
	absa=std::fabs(a);
	absb=std::fabs(b);
	if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}

float **
StatisticNumericalRecipes::matrix(long nrl, long nrh, long ncl, long nch) throw (StatisticException)
/* allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] */
{
        long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
        float **m;

        /* allocate pointers to rows */
        m=(float **) std::malloc((size_t)((nrow+NR_END)*sizeof(float*)));
        if (!m) throw StatisticException("allocation failure 1 in matrix()");
        m += NR_END;
        m -= nrl;

        /* allocate rows and set pointers to them */
        m[nrl]=(float *) std::malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
        if (!m[nrl]) throw StatisticException("allocation failure 2 in matrix()");
        m[nrl] += NR_END;
        m[nrl] -= ncl;

        for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

        /* return pointer to array of pointers to rows */
        return m;
}

void 
StatisticNumericalRecipes::free_matrix(float **m, long nrl, long /*nrh*/, long ncl, long /*nch*/)
/* free a float matrix allocated by matrix() */
{
        std::free((char*) (m[nrl]+ncl-NR_END));
        std::free((char*) (m+nrl-NR_END));
}

void 
StatisticNumericalRecipes::svdcmp(float **a, int m, int n, float w[], float **v) throw (StatisticException)
{
	int flag,i,its,j,jj,k,l,nm;
	float anorm,c,f,g,h,s,scale,x,y,z,*rv1;

	rv1=vector(1,n);
	g=scale=anorm=0.0;
	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i]=scale*g;
		g=s=scale=0.0;
		if (i <= m) {
			for (k=i;k<=m;k++) scale += std::fabs(a[k][i]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}
				f=a[i][i];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i][i]=f-g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
					f=s/h;
					for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
				}
				for (k=i;k<=m;k++) a[k][i] *= scale;
			}
		}
		w[i]=scale *g;
		g=s=scale=0.0;
		if (i <= m && i != n) {
			for (k=l;k<=n;k++) scale += std::fabs(a[i][k]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i][l]=f-g;
				for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
				for (j=l;j<=m;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
					for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
				}
				for (k=l;k<=n;k++) a[i][k] *= scale;
			}
		}
		anorm=FMAX(anorm,(std::fabs(w[i])+std::fabs(rv1[i])));
	}
	for (i=n;i>=1;i--) {
		if (i < n) {
			if (g) {
				for (j=l;j<=n;j++)
					v[j][i]=(a[i][j]/a[i][l])/g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
					for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
				}
			}
			for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
		}
		v[i][i]=1.0;
		g=rv1[i];
		l=i;
	}
	for (i=IMIN(m,n);i>=1;i--) {
		l=i+1;
		g=w[i];
		for (j=l;j<=n;j++) a[i][j]=0.0;
		if (g) {
			g=1.0/g;
			for (j=l;j<=n;j++) {
				for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
				f=(s/a[i][i])*g;
				for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
			}
			for (j=i;j<=m;j++) a[j][i] *= g;
		} else for (j=i;j<=m;j++) a[j][i]=0.0;
		++a[i][i];
	}
	for (k=n;k>=1;k--) {
		for (its=1;its<=30;its++) {
			flag=1;
			for (l=k;l>=1;l--) {
				nm=l-1;
				if ((float)(std::fabs(rv1[l])+anorm) == anorm) {
					flag=0;
					break;
				}
				if ((float)(std::fabs(w[nm])+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if ((float)(std::fabs(f)+anorm) == anorm) break;
					g=w[i];
					h=pythag(f,g);
					w[i]=h;
					h=1.0/h;
					c=g*h;
					s = -f*h;
					for (j=1;j<=m;j++) {
						y=a[j][nm];
						z=a[j][i];
						a[j][nm]=y*c+z*s;
						a[j][i]=z*c-y*s;
					}
				}
			}
			z=w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j=1;j<=n;j++) v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == 30) throw StatisticException("no convergence in 30 svdcmp iterations");
			x=w[l];
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=pythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i];
				y=w[i];
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1[j]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=1;jj<=n;jj++) {
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}
				z=pythag(f,h);
				w[j]=z;
				if (z) {
					z=1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=1;jj<=m;jj++) {
					y=a[jj][j];
					z=a[jj][i];
					a[jj][j]=y*c+z*s;
					a[jj][i]=z*c-y*s;
				}
			}
			rv1[l]=0.0;
			rv1[k]=f;
			w[k]=x;
		}
	}
	free_vector(rv1,1,n);
}

/*
int main(int argc, char* argv[]) {
   float** a = matrix(1, 2, 1, 2);
   float*  w = vector(1, 4);
   float** v = matrix(1, 2, 1, 2);
   int i, j;
   
   a[1][1] = 3.0;
   a[1][2] = -13.0;
   a[2][1] = -2.0;
   a[2][2] = 9.0;
   
   svdcmp(a, 2, 2, w, v);

   std::printf("A\n");
   for (i = 1; i <= 2; i++) {
      for (j = 1; j <= 2; j++) {
         printf("   %d, %d: %f\n", i, j, a[i][j]);
      }
   }
   
   std::printf("V\n");
   for (i = 1; i <= 2; i++) {
      for (j = 1; j <= 2; j++) {
         std::printf("   %d, %d: %f\n", i, j, v[i][j]);
      }
   }
   
   std::printf("W\n");
   for (i = 1; i <= 2; i++) {
      std::printf("   %d: %f\n", i, w[i]);
   }
}
*/

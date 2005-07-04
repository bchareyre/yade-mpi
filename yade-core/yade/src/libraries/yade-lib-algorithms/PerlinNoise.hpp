////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PERLINNOISE_HPP__
#define __PERLINNOISE_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade-lib-wm3-math/Vector2.hpp>
#include <yade-lib-wm3-math/Vector3.hpp>
#include <yade-lib-wm3-math/Vector4.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class PerlinNoise
{
	public :  static double coherentNoise(double p, int octaves, double persistence, double noiseOrigin=0.0);
	
	public  : static double noise(double x)
	{
		int X = (int)floor(x) & 255;                  			// FIND UNIT SEGMENT THAT
		x -= floor(x);                                			// FIND RELATIVE X
		double u = fade(x);                                		// COMPUTE FADE CURVES FOR X
		int A = p[X  ];							// HASH COORDINATES OF THE 2 SEGMENT EXTREMITIES
		return lerp(u, grad(p[A], x),					// AND ADD BLENDED RESULTS 
                               grad(p[A+1], x-1));				// FROM 2 EXTREMITIES OF SEGMENT
	}

	public :  static double coherentNoise(const Vector2d& p, int octaves, double persistence, const Vector2d& noiseOrigin=Vector2d::ZERO);
	
	public  : static double noise(double x, double y)
	{
		int X = (int)floor(x) & 255;                  			// FIND UNIT SQUARE THAT
		int Y = (int)floor(y) & 255;                  			// CONTAINS POINT.
		x -= floor(x);                                			// FIND RELATIVE X,Y
		y -= floor(y);                                			// OF POINT IN SQUARE.
		double u = fade(x);                                		// COMPUTE FADE CURVES
		double v = fade(y);                                		// FOR EACH OF X,Y.
		int A = p[X  ]+Y;						// HASH COORDINATES OF
		int B = p[X+1]+Y;						// THE 4 SQUARE CORNERS,
		return lerp(v, lerp(u, grad(p[A], x  , y   ),			// AND ADD BLENDED
                                       grad(p[B], x-1, y   )),			// RESULTS FROM 4
                               lerp(u, grad(p[A+1], x  , y-1 ),			// CORNERS OF SQUARE
                               	       grad(p[B+1], x-1, y-1 )));
	}

	public :  static double coherentNoise(const Vector3d& p, int octaves, double persistence, const Vector3d& noiseOrigin=Vector3d::ZERO);
	
	public  : static double noise(double x, double y, double z)
	{
		int X = (int)floor(x) & 255;                  			// FIND UNIT CUBE THAT
		int Y = (int)floor(y) & 255;                  			// CONTAINS POINT.
		int Z = (int)floor(z) & 255;
		x -= floor(x);                                			// FIND RELATIVE X,Y,Z
		y -= floor(y);                                			// OF POINT IN CUBE.
		z -= floor(z);
		double u = fade(x);                                		// COMPUTE FADE CURVES
		double v = fade(y);                                		// FOR EACH OF X,Y,Z.
		double w = fade(z);
		int A = p[X  ]+Y; int AA = p[A]+Z; int AB = p[A+1]+Z;      	// HASH COORDINATES OF
		int B = p[X+1]+Y; int BA = p[B]+Z; int BB = p[B+1]+Z;      	// THE 8 CUBE CORNERS,
		return lerp(w,
				lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
                                                grad(p[BA  ], x-1, y  , z   )), // BLENDED
                               		lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
                                       		grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
                       		lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),	// CORNERS
                                	        grad(p[BA+1], x-1, y  , z-1 )),	// OF CUBE
                               		lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                       		grad(p[BB+1], x-1, y-1, z-1 ))));
	}

	public :  static double coherentNoise(const Vector4d& p , int octaves, double persistence, const Vector4d& noiseOrigin=Vector4d::ZERO);
	
	public  : static double noise(double x, double y, double z, double w)
	{
      		int X = (int)floor(x) & 255;				// FIND UNIT HYPERCUBE
		int Y = (int)floor(y) & 255;                  		// THAT CONTAINS POINT.
          	int Z = (int)floor(z) & 255;
          	int W = (int)floor(w) & 255;
      		x -= floor(x);                              		// FIND RELATIVE X,Y,Z,W
      		y -= floor(y);                              		// OF POINT IN CUBE.
      		z -= floor(z);
      		w -= floor(w);
      		double a = fade(x);                                	// COMPUTE FADE CURVES
      		double b = fade(y);                                	// FOR EACH OF X,Y,Z,W.
      		double c = fade(z);
      		double d = fade(w);
      		int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z;      	// HASH COORDINATES OF
          	int B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;      	// THE 16 CORNERS OF
		int AAA = p[AA]+W, AAB = p[AA+1]+W;                	// THE HYPERCUBE.
		int ABA = p[AB]+W, ABB = p[AB+1]+W;
		int BAA = p[BA]+W, BAB = p[BA+1]+W;
		int BBA = p[BB]+W, BBB = p[BB+1]+W;
      		return lerp(d,                                     	// INTERPOLATE DOWN.
          			lerp(c,lerp(b,lerp(a,grad(p[AAA  ], x  , y  , z  , w), 
                               			     grad(p[BAA  ], x-1, y  , z  , w)),
                        		      lerp(a,grad(p[ABA  ], x  , y-1, z  , w),
                               			     grad(p[BBA  ], x-1, y-1, z  , w))),
                 		       lerp(b,lerp(a,grad(p[AAB  ], x  , y  , z-1, w),
                               			     grad(p[BAB  ], x-1, y  , z-1, w)),
                        		      lerp(a,grad(p[ABB  ], x  , y-1, z-1, w),
                               			     grad(p[BBB  ], x-1, y-1, z-1, w)))),
          			       lerp(c,lerp(b,lerp(a,grad(p[AAA+1], x  , y  , z  , w-1),
                               			     grad(p[BAA+1], x-1, y  , z  , w-1)),
                        		      lerp(a,grad(p[ABA+1], x  , y-1, z  , w-1),
                               			     grad(p[BBA+1], x-1, y-1, z  , w-1))),
                 		       lerp(b,lerp(a,grad(p[AAB+1], x  , y  , z-1, w-1),
                               			     grad(p[BAB+1], x-1, y  , z-1, w-1)),
                        		      lerp(a,grad(p[ABB+1], x  , y-1, z-1, w-1),
                               			     grad(p[BBB+1], x-1, y-1, z-1, w-1)))));
	}
   
	private : static double fade(double t)
	{
     		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	
   	private : static double lerp(double t, double a, double b)
	{
     		return a + t * (b - a);
	}
   	
	private : static double grad(int hash, double x)
	{
		int h = hash & 3;			// CONVERT LO 2 BITS OF HASH CODE
		return ((h&1) == 0 ? x : -x);		// INTO 2 GRADIENT DIRECTIONS.
	}
	
   	private : static double grad(int hash, double x, double y)
	{
		int h = hash & 7;			// CONVERT LO 3 BITS OF HASH CODE
		double u = h<4 ? x : y;			// INTO 8 GRADIENT DIRECTIONS.
		return ((h&1) == 0 ? u : -u);
	}
	
   	private : static double grad(int hash, double x, double y, double z)
	{
		int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
		double u = h<8 ? x : y;                 // INTO 12 GRADIENT DIRECTIONS.
		double v = h<4 ? y : h==12||h==14 ? x : z;
		return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
	}

	private : static double grad(int hash, double x, double y, double z, double w)
	{
      		int h = hash & 31; 			// CONVERT LO 5 BITS OF HASH TO 32 GRAD DIRECTIONS.
      		double a=y,b=z,c=w;            		// X,Y,Z
      		switch (h >> 3)				// OR, DEPENDING ON HIGH ORDER 2 BITS:
		{          
      			case 1: a=w;b=x;c=y;break;     // W,X,Y
      			case 2: a=z;b=w;c=x;break;     // Z,W,X
      			case 3: a=y;b=z;c=w;break;     // Y,Z,W
      		}
      		return ((h&4)==0 ? -a:a) + ((h&2)==0 ? -b:b) + ((h&1)==0 ? -c:c);
	}
 
      
  	// permutation
  	private : static int p[512];
};


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __PERLINNOISE_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

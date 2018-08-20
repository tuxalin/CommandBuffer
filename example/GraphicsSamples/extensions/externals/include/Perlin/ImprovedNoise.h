/*
    Ken Perlin's "Improved Noise"
    http://mrl.nyu.edu/~perlin/noise/
*/

#include "NV/NvMath.h"
using namespace nv;

static int permutation[] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static float g[][3] = {
    1,1,0,
    -1,1,0,
    1,-1,0,
    -1,-1,0,  
    1,0,1,
    -1,0,1,
    1,0,-1,
    -1,0,-1,  
    0,1,1,
    0,-1,1,
    0,1,-1,
    0,-1,-1,
    1,1,0,
    0,-1,1,
    -1,1,0,
    0,-1,-1,
};

class ImprovedNoise {
public:
    ImprovedNoise() {
        p = new int[512];
        for (int i=0; i < 256 ; i++) p[256+i] = p[i] = permutation[i];
    }

    ~ImprovedNoise() {
        delete [] p;
    }

   inline float noise(float x, float y, float z) {
      int X = (int)floor(x) & 255,                  // FIND UNIT CUBE THAT
          Y = (int)floor(y) & 255,                  // CONTAINS POINT.
          Z = (int)floor(z) & 255;
      x -= floor(x);                                // FIND RELATIVE X,Y,Z
      y -= floor(y);                                // OF POINT IN CUBE.
      z -= floor(z);
      float u = fade(x),                                // COMPUTE FADE CURVES
             v = fade(y),                                // FOR EACH OF X,Y,Z.
             w = fade(z);
      int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      // HASH COORDINATES OF
          B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;      // THE 8 CUBE CORNERS,

      return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
                                     grad(p[BA  ], x-1, y  , z   )), // BLENDED
                             lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
                                     grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
                                     grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
   }

    float noise(vec3f p)
    {
        return noise(p.x, p.y, p.z);
    }

   // vector noise
    vec3f noise3f(vec3f p)
    {
        return vec3f(noise(p),
                        noise(p + vec3f(32, 78, 7)),
                        noise(p + vec3f(123, 11, 96))
                        );
    }

    // fractal sum
    float fBm(vec3f p, int octaves = 4, float lacunarity = 2.0, float gain = 0.5)
    {
	    float freq = 1.0, amp = 0.5;
	    float sum = 0.0;	
	    for(int i=0; i<octaves; i++) {
		    sum += noise(p*freq)*amp;
		    freq *= lacunarity;
		    amp *= gain;
	    }
	    return sum;
    }

    vec3f fBm3f(vec3f p, int octaves = 4, float lacunarity = 2.0, float gain = 0.5)
    {
	    float freq = 1.0, amp = 0.5;
	    vec3f sum = 0.0;	
	    for(int i=0; i<octaves; i++) {
		    sum += noise3f(p*freq)*amp;
		    freq *= lacunarity;
		    amp *= gain;
	    }
	    return sum;
    }

    float turbulence(vec3f p, int octaves = 4, float lacunarity = 2.0, float gain = 0.5)
    {
	    float freq = 1.0, amp = 1.0;
	    float sum = 0.0;
        for(int i=0; i<octaves; i++) {
		    sum += abs(noise(p*freq))*amp;
		    freq *= lacunarity;
		    amp *= gain;
	    }
	    return sum;
    }

   float noise(float x, float y, float z, float w) {
        int X = (int)floor(x) & 255,                  // FIND UNIT HYPERCUBE
            Y = (int)floor(y) & 255,                  // THAT CONTAINS POINT.
            Z = (int)floor(z) & 255,
            W = (int)floor(w) & 255;
        x -= floor(x);                                // FIND RELATIVE X,Y,Z,W
        y -= floor(y);                                // OF POINT IN CUBE.
        z -= floor(z);
        w -= floor(w);
        float a = fade(x),                                // COMPUTE FADE CURVES
               b = fade(y),                                // FOR EACH OF X,Y,Z,W.
               c = fade(z),
               d = fade(w);
        int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,      // HASH COORDINATES OF
            B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z,      // THE 16 CORNERS OF
            AAA = p[AA]+W, AAB = p[AA+1]+W,                // THE HYPERCUBE.
            ABA = p[AB]+W, ABB = p[AB+1]+W,
            BAA = p[BA]+W, BAB = p[BA+1]+W,
            BBA = p[BB]+W, BBB = p[BB+1]+W;

        return lerp(d,                                     // INTERPOLATE DOWN.
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

//private:
   inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
   inline float lerp(float t, float a, float b) { return a + t * (b - a); }
   inline float dot(float a, float b, float c, float x, float y, float z) { return a*x+b*y+c*z; }

#if 0
   inline float grad(int hash, float x, float y, float z) {
      int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
      float u = h<8 ? x : y;                 // INTO 12 GRADIENT DIRECTIONS.
      float v = h<4 ? y : h==12||h==14 ? x : z;
      return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
   }
#else
   // use look-up table
   inline float grad(int hash, float x, float y, float z) {
        int h = hash & 15;
        return dot(g[h][0], g[h][1], g[h][2], x, y, z);
   }
#endif

/*
   void dump_perm_2d(char *filename)
   {
     FILE *fp = fopen(filename, "wb");
     int Z = 0;
     for(int Y=0; Y<256; Y++) {
        for(int X=0; X<256; X++) {
          int  A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,
               B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;
          char c[4];
          c[0] = AA;
          c[1] = BA;
          c[2] = AB;
          c[3] = BB;
          fwrite(c, sizeof(char), 4, fp);
        }
     }
     fclose(fp);
   }
*/

   float grad(int hash, float x, float y, float z, float w) {
      int h = hash & 31; // CONVERT LO 5 BITS OF HASH TO 32 GRAD DIRECTIONS.
      float a=y,b=z,c=w;            // X,Y,Z
      switch (h >> 3) {          // OR, DEPENDING ON HIGH ORDER 2 BITS:
      case 1: a=w;b=x;c=y;break;     // W,X,Y
      case 2: a=z;b=w;c=x;break;     // Z,W,X
      case 3: a=y;b=z;c=w;break;     // Y,Z,W
      }
      return ((h&4)==0 ? -a:a) + ((h&2)==0 ? -b:b) + ((h&1)==0 ? -c:c);
   }

   int *p;
};

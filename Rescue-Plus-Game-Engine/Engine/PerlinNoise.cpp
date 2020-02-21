#include "PerlinNoise.h"
// Translated from Ken Perlin's java code: https://mrl.nyu.edu/~perlin/noise/

int PerlinNoise::p[512] = {};

static double Fade(double t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}
static double Lerp(double t, double a, double b)
{
    return a + t * (b - a);
}
static double Grad(int hash, double x, double y, double z)
{
    int h = hash & 15; // CONVERT LO 4 BITS OF HASH CODE
    double u = h < 8 ? x : y; // INTO 12 GRADIANT DIRECTIONS
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

void PerlinNoise::Seed(int seed)
{
    //Fill array with 0-256
    std::iota(std::begin(p), std::end(p), 0);

    //Create a random engine
    std::default_random_engine rand(seed);

    //Shuffle it
    std::shuffle(std::begin(p), std::end(p), rand);

    //Copy first half into second half
    for (int i = 0; i < 256; i++)
        p[256 + i] = p[i];
}

double PerlinNoise::Noise(double x, double y, double z)
{
    int X = (int)floor(x) & 255; // FIND UNIT CUBE THAT
    int Y = (int)floor(y) & 255; // CONTAINS POINT.
    int Z = (int)floor(z) & 255;

    x -= floor(x); // FIND RELATIVE X,Y,Z
    y -= floor(y); // OF POINT IN CUBE.
    z -= floor(z);

    double u = Fade(x); // COMPUTE FADE CURVES
    double v = Fade(y); // FOR EACH OF X,Y,Z.
    double w = Fade(z);

    int A = p[X] + Y; // HASH COORDINATES OF
    int AA = p[A] + Z; // THE 8 CUBE CORNERS,
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z), // AND ADD
        Grad(p[BA], x - 1, y, z)), // BLENDED
        Lerp(u, Grad(p[AB], x, y - 1, z), // RESULTS
            Grad(p[BB], x - 1, y - 1, z))), // FROM 8
        Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1), // CORNERS
            Grad(p[BA + 1], x - 1, y, z - 1)), // OF CUBE
            Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
                Grad(p[BB + 1], x - 1, y - 1, z - 1))));
}
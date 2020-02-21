#pragma once
#include <cmath>
#include <algorithm>
#include <random>
#include <numeric>

// --------------------------------------------------------
// Generate perlin noise after seeding. ALWAYS SEED FIRST
// Translated from Ken Perlin's java code: https://mrl.nyu.edu/~perlin/noise/
// --------------------------------------------------------
class PerlinNoise {
private:
    static int p[512];

public:
    // --------------------------------------------------------
    // Seed the perlin noise generator
    // --------------------------------------------------------
    static void Seed(int seed);

    // --------------------------------------------------------
    // Generate perlin noise.
    // ALWAYS SEED AT LEAST ONCE PER PROGRAM!
    // --------------------------------------------------------
    static double Noise(double x, double y, double z);

};
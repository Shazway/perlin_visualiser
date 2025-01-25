#include "NoiseGenerator.hpp"

NoiseGenerator::NoiseGenerator(unsigned int seed)
{
	// Initialize permutation table
	std::mt19937 generator(seed);
	std::vector<int> p(256);
	for (int i = 0; i < 256; i++) p[i] = i;
	std::shuffle(p.begin(), p.end(), generator);
	permutation.resize(512);
	for (int i = 0; i < 512; i++) permutation[i] = p[i % 256];
}

void NoiseGenerator::setNoiseData(const NoiseData &data)
{
	_data = data;
}

// Layered perlin noise samples by octaves number
double NoiseGenerator::noise(double x, double y) const
{
	double total = 0.0;
	double amplitude = _data.amplitude;
	double frequency = _data.frequency;
	double maxValue = 0.0; // Used for normalizing

	for (size_t i = 0; i < _data.octaves; i++) {
		total += singleNoise(x * frequency, y * frequency) * amplitude;
		maxValue += amplitude;

		amplitude *= _data.persistence;
		frequency *= _data.lacunarity;
	}
	return total / maxValue; // Normalize
}

double NoiseGenerator::singleNoise(double x, double y) const
{
	int X = (int)std::floor(x) & 255;
	int Y = (int)std::floor(y) & 255;

	x -= std::floor(x);
	y -= std::floor(y);

	double u = fade(x);
	double v = fade(y);

	int A = permutation[X] + Y;
	int B = permutation[X + 1] + Y;

	return lerp(
		lerp(grad(permutation[A], x, y), grad(permutation[B], x - 1, y), u),
		lerp(grad(permutation[A + 1], x, y - 1), grad(permutation[B + 1], x - 1, y - 1), u),
		v
	);
}

vec2 NoiseGenerator::getBorderWarping(double x, double z)
{
	NoiseData data = {
		4,
		2.0,
		0.5,
		0.1,
		1.0
	};
	_data = data;
	double noiseX = noise(x, z);
	double noiseY = noise(z, x);
	vec2 offset;
	offset.x = noiseX * 16.0;
	offset.y = noiseY * 16.0;
	return offset;
}

double NoiseGenerator::fade(double t) const
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double NoiseGenerator::lerp(double a, double b, double t) const
{
	return a + t * (b - a);
}

double NoiseGenerator::grad(int hash, double x, double y) const
{
	int h = hash & 3; // Only 4 gradients
	double u = h < 2 ? x : y;
	double v = h < 2 ? y : x;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

#include <Noise.h>

Vector3f Noise::getColor(const Vector3f &pos)
{
	// Interpolate between two colors by weighted average
	float weight = frequency * pos[0] + amplitude * PerlinNoise::octaveNoise(pos, octaves);
	// Clamp sin output from [-1, 1] to [0, 1]
	weight = 0.5 * sin(weight) + 0.5;
	return weight * color[0] + (1 - weight) * color[1];
}

Noise::Noise(int _octaves, const Vector3f &color1,
			 const Vector3f &color2, float freq, float amp) : octaves(_octaves), frequency(freq), amplitude(amp)
{
	color[0] = color1;
	color[1] = color2;
	init = true;
}

Noise::Noise(const Noise &n) : octaves(n.octaves), frequency(n.frequency),
							   amplitude(n.amplitude), init(n.init)
{
	color[0] = n.color[0];
	color[1] = n.color[1];
}

bool Noise::valid()
{
	return init;
}

Noise::Noise() : octaves(0), init(false)
{
}

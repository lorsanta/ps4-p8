#include "audio.h"
#include <math.h>

#define M_PI 3.14159265358979323

unsigned int audio_get_wavelength(float frequency, int sample_rate) {
	return sample_rate / frequency;
}
unsigned int audio_get_points(float seconds, int sample_rate) {
	return sample_rate * seconds;
}

float audio_sin_wave(float phase) {
	return sinf(phase * 2 * M_PI);
}
float audio_triangle_wave(float phase) {
	return abs(phase - 0.5) * -4 + 1;
}
float audio_tilted_wave(float phase) {
	if (phase < 0.75) {
		return (phase / 0.75) * 2 - 1;
	}
	else {
		return 4 - (phase / 0.25) * 2;
	}
}
float audio_sawtooth_wave(float phase) {
	return phase * 2 - 1;
}
float audio_square_wave(float phase) {
	if (phase < 0.5) {
		return 0;
	}
	else {
		return 1;
	}
}
float audio_pulse_wave(float phase) {
	if (phase < 0.75) {
		return 0;
	}
	else {
		return 1;
	}
}
float audio_organ_wave(float phase);
float audio_noise_wave(float phase) {
	return ((float)rand() / RAND_MAX * 2) - 1;
}
float audio_phaser_wave(float phase);

void audio_generate_wave(float (*wave_fn)(float), unsigned int wavelength, std::vector<float>& dest) {
	audio_generate_wave(wave_fn, wavelength, dest, 0, dest.size());
}
void audio_generate_wave(float (*wave_fn)(float), unsigned int wavelength, std::vector<float>& dest, unsigned int from, unsigned int to) {
	unsigned int len = to - from;
	// Generate the first wave
	for (unsigned int i = 0; i < wavelength && i < len; i++) {
		dest[from + i] = wave_fn((float)i / wavelength);
	}
	// Copy it until we reach the end
	for (unsigned int i = wavelength; i < len; i++) {
		dest[from + i] = dest[from + i % wavelength];
	}
}

#define SMOOTHNESS 100
void audio_smooth(std::vector<float>& dest, unsigned int position) {
	float start = dest[position - SMOOTHNESS / 2];
	float end = dest[position + SMOOTHNESS / 2];
	for (int i = 0; i < SMOOTHNESS; i++) {
		dest[position + i - SMOOTHNESS / 2] = start + (end - start) * i / SMOOTHNESS;
	}
}

void audio_amplify(std::vector<float>& src, std::vector<int16_t>& dest, unsigned int volume) {
	audio_amplify(src, dest, volume, 0);
}
void audio_amplify(std::vector<float>& src, std::vector<int16_t>& dest, unsigned int volume, unsigned int from) {
	int max = dest.size() - from;
	if (max > src.size()) {
		max = src.size();
	}
	for (int i = 0; i < max; i++) {
		dest[i+from] = volume * src[i];
	}
}

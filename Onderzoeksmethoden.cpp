// Onderzoeksmethoden.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

namespace sound {
	const double PI = 3.14159265359;
	const double TWOPI = PI * 2;
	const unsigned int SAMPLE_RATE = 44100;
	const short SHORT_MAX_VAL = 32767;

	short SineWave(double time, double freq, double amp = 1) {
		double tpc = SAMPLE_RATE / freq; // ticks per cycle
		double cycles = time / tpc;
		double rad = TWOPI * cycles;
		short amplitude = SHORT_MAX_VAL * amp;
		short result = amplitude * sin(rad);
		return result;
	}

	short SquareWave(double time, double freq, double amp = 1) {
		int tpc = SAMPLE_RATE / freq; // ticks per cycle
		double cycles = time / tpc;
		double cyclepart = (int)time % tpc;
		short amplitude = SHORT_MAX_VAL * amp;
		if (cyclepart < tpc / 2) return amplitude;
		return 0;
	}
}

void Play(vector<sf::SoundBuffer> buffers) {
	sf::Sound sound;
	for (sf::SoundBuffer buffer : buffers) {
		sound.setBuffer(buffer);
		sound.play();
		Sleep(buffer.getDuration().asMilliseconds());
	}
}

void Play(sf::SoundBuffer buffer) {
	sf::Sound sound;
	sound.setBuffer(buffer);
	sound.play();
	Sleep(buffer.getDuration().asMilliseconds());
}

vector<sf::SoundBuffer> Drieklank(vector<double> freqs, double duration, double volume) { // duration in seconds
	vector<sf::SoundBuffer> buffers;
	sf::SoundBuffer buffer;
	vector<sf::Int16> samples;
	const unsigned int SAMPLE_COUNT = duration * sound::SAMPLE_RATE;

	for (double freq : freqs) {
		for (unsigned int i = 0; i < SAMPLE_COUNT; i++) {
			samples.push_back(sound::SineWave(i, freq, volume));
		}
		buffer.loadFromSamples(&samples[0], samples.size(), 1, sound::SAMPLE_RATE);
		buffers.push_back(sf::SoundBuffer(buffer));
		samples.clear();
	}

	return buffers;
}

vector<sf::SoundBuffer> Drieklank(vector<double> freqs, vector<double> durations, double volume) { // duration in seconds
	vector<sf::SoundBuffer> buffers;
	sf::SoundBuffer buffer;
	vector<sf::Int16> samples;

	for (unsigned int i = 0; i < freqs.size(); i++) {
		const unsigned int SAMPLE_COUNT = durations[i] * sound::SAMPLE_RATE;
		for (unsigned int i = 0; i < SAMPLE_COUNT; i++) {
			samples.push_back(sound::SineWave(i, freqs[i], volume));
		}
		buffer.loadFromSamples(&samples[0], samples.size(), 1, sound::SAMPLE_RATE);
		buffers.push_back(sf::SoundBuffer(buffer));
		samples.clear();
	}

	return buffers;
}

sf::SoundBuffer Akkoord(vector<double> freqs, double duration, double volume) { // duration in seconds
	sf::SoundBuffer buffer;
	vector<sf::Int16> samples;
	const unsigned int SAMPLE_COUNT = duration * sound::SAMPLE_RATE;

	for (unsigned int i = 0; i < SAMPLE_COUNT; i++) {
		short sample = 0;
		for (double freq : freqs) {
			sample += sound::SineWave(i, freq, volume / freqs.size()); // divide by number of freqencies or amplitude may overflow
		}
		samples.push_back(sample);
	}
	buffer.loadFromSamples(&samples[0], samples.size(), 1, sound::SAMPLE_RATE);
	return buffer;
}

sf::SoundBuffer Akkoord(vector<double> freqs, double duration, vector<double> volumes) { // duration in seconds
	sf::SoundBuffer buffer;
	vector<sf::Int16> samples;
	const unsigned int SAMPLE_COUNT = duration * sound::SAMPLE_RATE;

	for (unsigned int i = 0; i < SAMPLE_COUNT; i++) {
		short sample = 0;
		for (unsigned int i = 0; i < freqs.size(); i++) {
			sample += sound::SineWave(i, freqs[i], volumes[i] / freqs.size()); // divide by number of freqencies or amplitude may overflow
		}
		samples.push_back(sample);
	}
	buffer.loadFromSamples(&samples[0], samples.size(), 1, sound::SAMPLE_RATE);
	return buffer;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(100, 100), "SFML works!");
	vector<sf::SoundBuffer> sounds = Drieklank({ 440, 554.3, 659.2, 880 }, 0.25, 0.5);
	sounds.push_back(Akkoord({ 440, 554.3, 659.2, 880 }, 1, 0.5));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.display();
		Play(Akkoord({ 220, 416.7 }, 1, 0.5)); // aa
		Play(Akkoord({ 220, 391.1 }, 1, 0.5)); // oo
		Play(Akkoord({ 220, 2200 }, 1, 0.5)); // ie
		/*Play(sounds);
		Play(Akkoord({ 440, 523.2, 659.2 }, 1, 1)); // A-mineur
		Play(Akkoord({ 349.2, 440, 523.2 }, 1, 1)); // F-majeur
		Play(Akkoord({ 293.6, 349.2, 440 }, 1, 1)); // D-mineur
		Play(Akkoord({ 329.6, 391.9, 493.8 }, 1, 1)); // E-majeur
		Play(Akkoord({ 845, 850, 855, 1605, 1610 }, 1, 0.5)); // aa
		Play(Akkoord({ 355, 360, 365, 635, 640 }, 1, 0.5)); // oo
		Play(Akkoord({ 235, 240, 245, 2395, 2400 }, 1, 0.5)); // ie
		Play(Akkoord({ 230, 235, 240, 2095, 2100 }, 1, 0.5)); // uu
		Play(Akkoord({ 605, 610, 615, 1895, 1900 }, 1, 0.5)); // eh
		Play(Akkoord({ 245, 250, 255, 590, 595 }, 1, 0.5)); // oe
		Play(Akkoord({ 385, 390, 395, 2295, 2300 }, 1, 0.5)); // ee
		Play(Akkoord({ 745, 750, 755, 935, 940 }, 1, 0.5)); // ah
		Play(Akkoord({ 495, 500, 505, 695, 700 }, 1, 0.5)); // oh
		Play(Akkoord({ 365, 370, 375, 1895, 1900 }, 1, 0.5)); // eu*/
		Play(Akkoord({ 0 }, 0.3, 0)); // '
	}

	return 0;
}


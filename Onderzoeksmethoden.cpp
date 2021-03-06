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
		for (unsigned int j = 0; j < freqs.size(); j++) {
			sample += sound::SineWave(i, freqs[j], volumes[j] / freqs.size()); // divide by number of freqencies or amplitude may overflow
		}
		samples.push_back(sample);
	}
	buffer.loadFromSamples(&samples[0], samples.size(), 1, sound::SAMPLE_RATE);
	return buffer;
}

sf::SoundBuffer NaturalTone(double grondtoon, double duration, vector<double> amplitudes) { // duration in seconds
	sf::SoundBuffer buffer;
	vector<sf::Int16> samples;
	const unsigned int SAMPLE_COUNT = duration * sound::SAMPLE_RATE;
	double amp_sum = 0;
	for (double amplitude : amplitudes) amp_sum += amplitude;

	for (unsigned int i = 0; i < SAMPLE_COUNT; i++) {
		short sample = 0;
		for (unsigned int j = 0; j < amplitudes.size(); j++) sample += sound::SineWave(i, grondtoon*(j+1), amplitudes[j] / amp_sum); 
		// divide amplitude per overtone by total or sample may overflow
		samples.push_back(sample);
	}
	buffer.loadFromSamples(&samples[0], samples.size(), 1, sound::SAMPLE_RATE);
	return buffer;
}

sf::SoundBuffer ImitatedTone(const double grondtoon, const int n_overtones, vector<double> imitated_freqs, double duration, double volume = 1) {
	vector<vector<double>> imitations;
	const double BASE_AMP = volume * 0.1;
	for (double imitated : imitated_freqs) {
		vector<double> imitation(n_overtones, 0);
		if (imitated <= grondtoon) { imitation[0] = volume; } // imitated tone is lower or equal to grondtoon
		else if (imitated > grondtoon*n_overtones) { imitation[n_overtones-1] = volume; } // imitated tone is higher than highest overtone
		else { // imitated tone is in range <grondtoon, highest overtone]
			double prev_freq = grondtoon, cur_freq = grondtoon*2;
			int i = 1;
			while (cur_freq < imitated) {
				i++;
				prev_freq = grondtoon * i;
				cur_freq = grondtoon * (i+1);
			}
			double scale_lower = volume - volume*((imitated - prev_freq) / (cur_freq - prev_freq)); // garanteed (cur_freq - prev_freq) > 0
			imitation[i-1] = scale_lower;
			imitation[i] = volume - scale_lower;
		}
		for (int i = 0; i < n_overtones; i++) imitation[i] += BASE_AMP; // add base amplitude to all overtones
		imitations.push_back(imitation);
	}
	vector<double> summed_imitation(n_overtones);
	for (int i = 0; i < n_overtones; i++) {
		double sum = 0;
		for (vector<double> imitation : imitations) sum += imitation[i];
		summed_imitation[i] = sum;
	}
	return NaturalTone(grondtoon, duration, summed_imitation);
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
		//Play(Akkoord({ 850, 1610 }, 1, 0.5)); // aa
		Play(ImitatedTone(220, 12, { 850, 1610 }, 1, 0.5)); // aa
		//Play(Akkoord({ 360, 640 }, 1, 0.5)); // oo
		Play(ImitatedTone(220, 12, { 360, 640 }, 1, 0.5)); // oo
		//Play(Akkoord({ 240, 2400 }, 1, 0.5)); // ie
		Play(ImitatedTone(220, 12, { 240, 2400 }, 1, 0.5)); // ie
		//Play(Akkoord({ 235, 2100 }, 1, 0.5)); // uu
		Play(ImitatedTone(220, 12, { 235, 2100 }, 1, 0.5)); // uu
		//Play(Akkoord({ 610, 1900 }, 1, 0.5)); // eh
		Play(ImitatedTone(220, 12, { 610, 1900 }, 1, 0.5)); // eh
		//Play(Akkoord({ 250, 595 }, 1, 0.5)); // oe
		Play(ImitatedTone(220, 12, { 250, 595 }, 1, 0.5)); // oe
		//Play(Akkoord({ 390, 2300 }, 1, 0.5)); // ee
		Play(ImitatedTone(220, 12, { 390, 2300 }, 1, 0.5)); // ee
		//Play(Akkoord({ 750, 940 }, 1, 0.5)); // ah
		Play(ImitatedTone(220, 12, { 750, 940 }, 1, 0.5)); // ah
		//Play(Akkoord({ 500, 700 }, 1, 0.5)); // oh
		Play(ImitatedTone(220, 12, { 500, 700 }, 1, 0.5)); // oh
		//Play(Akkoord({ 370, 1900 }, 1, 0.5)); // eu
		Play(ImitatedTone(220, 12, { 370, 1900 }, 1, 0.5)); // eu
		//Play(NaturalTone(220, 1, {1}));
		//Play(NaturalTone(220, 1, {1,1.0/2,1.0/3,1.0/4,1.0/5,1.0/6,1.0/7,1.0/8,1.0/9,1.0/10}));
		//Play(NaturalTone(220, 1, {5,1.0/2,1.0/3,1.0/4,1.0/5,1.0/6,1.0/7,1.0/8,1.0/9,0.5}));
		//Play(NaturalTone(220, 1, { 1.0 / 3,1.0 / 2,1,1.0/2,1.0/3,1.0/4,1.0/5,1.0/2,1.0/7,1.0/8}));
		/*Play(sounds);
		Play(Akkoord({ 440, 523.2, 659.2 }, 1, 1)); // A-mineur
		Play(Akkoord({ 349.2, 440, 523.2 }, 1, 1)); // F-majeur
		Play(Akkoord({ 293.6, 349.2, 440 }, 1, 1)); // D-mineur
		Play(Akkoord({ 329.6, 391.9, 493.8 }, 1, 1)); // E-majeur*/
		Play(Akkoord({ 0 }, 0.3, 0)); // '
	}

	return 0;
}


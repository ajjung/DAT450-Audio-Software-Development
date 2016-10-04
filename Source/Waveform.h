#ifndef SYNTH_H_INCLUDED
#define SYNTH_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <math.h>

const int MAX_POLYPHONY = 32;

struct JOSSound : public SynthesiserSound
{
	JOSSound() {}

	bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
	bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

class Waveform
{
private:
	float newSampleRate;

public:
	void setSampleRate(double samplerate)
	{
		newSampleRate = samplerate;
	}

	float sineWave(double freq, float n)
	{
		float sine = sin(freq) * 2 * 3.14 * (n / newSampleRate);
		return sine;
	}

	float squareWave(double freq, float n)
	{
		float square;
		return square;
	}

	float triangleWave(double freq, float n)
	{
		float triangle;
		return triangle;
	}

	float sawToothWave(double freq, float n)
	{
		float sawTooth;
		return sawTooth;
	}


	int switchF(int caseNum, float n, double freq)
	{
		switch (caseNum)
		{
		case 1:
			sineWave(freq, n);
			break;
		case 2:
			squareWave(freq, n);
			break;
		case 3:
			triangleWave(freq, n);
			break;
		case 4:
			sawToothWave(freq, n);
		}
		return caseNum;
	}
};
//=============================================================================================
class WaveformSynth : public AudioSource
{
private:
		AudioDeviceManager& inputDeviceManager;
		AudioDeviceManager& outputDeviceManager;
		MidiKeyboardState& keyboardState;
		Waveform synth;
		MidiMessageCollector midiCollector;
		AudioSourcePlayer audioSourcePlayer;
		float newSampleRate;

public:
	WaveformSynth(MidiKeyboardState& _state, AudioDeviceManager& _in, AudioDeviceManager& _out)
		: keyboardState(_state), inputDeviceManager(_in), outputDeviceManager(_out)
	{
		inputDeviceManager.addMidiInputCallback(String::empty, &midiCollector);
		inputDeviceManager.setMidiInputEnabled(MidiInput::getDevices()[0], true);

		// AudioDeviceManager& outputDeviceManager 
		//   -> SynthPlayer(:AudioIODeviceCallback) audioSourcePlayer
		outputDeviceManager.addAudioCallback(&audioSourcePlayer);

		// ... -> PiSynthPlayer(:AudioSource)
		audioSourcePlayer.setSource(this);

		for (int i = 0; i < MAX_POLYPHONY; ++i)
		{
			//synth.addVoice(new JOSVoice());
		}
		//synth.clearSounds();
		//synth.addSound(new JOSSound());
	}

	~WaveformSynth()
	{
		audioSourcePlayer.setSource(nullptr);
		inputDeviceManager.removeMidiInputCallback(String::empty, &midiCollector);

		outputDeviceManager.removeAudioCallback(&audioSourcePlayer);
		outputDeviceManager.closeAudioDevice();
	}

	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override 
	{
		midiCollector.reset(sampleRate);
		synth.setSampleRate(sampleRate);
	}

	void releaseResources() override
	{ }
	
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override {
		bufferToFill.clearActiveBufferRegion();

		MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

		// pass these messages to the keyboard state so that it can update the component
		// to show on-screen which keys are being pressed on the physical midi keyboard.	
		const int startSample = 0;
		const bool injectIndirectEvents = true;  // add midi messages generated by clicking on the on-screen keyboard.
		keyboardState.processNextMidiBuffer(incomingMidi, startSample, bufferToFill.numSamples, injectIndirectEvents);

		//synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, startSample, bufferToFill.numSamples);
	}
};


#endif
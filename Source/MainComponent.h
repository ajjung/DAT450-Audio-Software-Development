/*
  ==============================================================================

    MainComponent.h
    Created: 6 Oct 2016 9:12:25pm
    Author:  Anthony

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED
#include "SineWave.h"
#include "SawtoothWave.h"
#include "TriangleWave.h"
#include "SquareWave.h"

class MainComponent : public Component,
	private AudioIODeviceCallback,  // [1]
	private MidiInputCallback,       // [2]
	private ComboBox::Listener,
	private MidiKeyboardStateListener
{
public:
	//==============================================================================
	MainComponent()
		: audioSetupComp(audioDeviceManager, 0, 0, 0, 256, true, true, true, false),
		keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard)
	{
		setSize(1220, 600);

		audioDeviceManager.initialise(0, 2, nullptr, true, String(), nullptr);
		audioDeviceManager.addMidiInputCallback(String(), this); // [6]
		audioDeviceManager.addAudioCallback(this);

		addAndMakeVisible(audioSetupComp);
		addAndMakeVisible(waveformList);
		addAndMakeVisible(waveformListLabel);
		addAndMakeVisible(keyboardComponent);

		waveformList.addListener(this);
		keyboardState.addListener(this);

		for (int i = 0; i < 15; ++i)
		{
			synth.addVoice(new SineWave);
		}

		waveformListLabel.setText("Wavefrom:", dontSendNotification);
		waveformListLabel.attachToComponent(&waveformList, true);
		waveformList.addItem("Sine Wave", 1);
		waveformList.addItem("Square Wave", 2);
		waveformList.addItem("Triangle Wave", 3);
		waveformList.addItem("Sawtooth Wave", 4);
		waveformList.setSelectedId(1);

		synth.enableLegacyMode(24);
		synth.setVoiceStealingEnabled(false);
		visualiserInstrument.enableLegacyMode(24);
	}

	~MainComponent()
	{
		audioDeviceManager.removeMidiInputCallback(String(), this);
		waveformList.removeListener(this);
	}

	//==============================================================================
	void paint(Graphics& g) override
	{
		g.fillAll(Colours::grey);
	}

	void resized() override
	{
		Rectangle<int> r(getLocalBounds());
		audioSetupComp.setBounds(r);
		waveformList.setBounds(10, 350, 200, 20);
		keyboardComponent.setBounds(r.removeFromBottom(75));
	}

	//==============================================================================
	void audioDeviceIOCallback(const float** /*inputChannelData*/, int /*numInputChannels*/,
		float** outputChannelData, int numOutputChannels,
		int numSamples) override
	{
		// make buffer
		AudioBuffer<float> buffer(outputChannelData, numOutputChannels, numSamples);

		// clear it to silence
		buffer.clear();

		MidiBuffer incomingMidi;

		// get the MIDI messages for this audio block
		midiCollector.removeNextBlockOfMessages(incomingMidi, numSamples);

		// synthesise the block
		synth.renderNextBlock(buffer, incomingMidi, 0, numSamples);
	}

	void audioDeviceAboutToStart(AudioIODevice* device) override
	{
		const double sampleRate = device->getCurrentSampleRate();
		midiCollector.reset(sampleRate);
		synth.setCurrentPlaybackSampleRate(sampleRate);
	}

	void audioDeviceStopped() override
	{
	}

	void comboBoxChanged(ComboBox *box) override
	{
		if (box == &waveformList)
		{
			if (waveformList.getSelectedId() == 1)
			{
				synth.clearVoices();
				for (int i = 0; i < 15; ++i)
				{
					synth.addVoice(new SineWave);
				}
			}

			if (waveformList.getSelectedId() == 2)
			{
				synth.clearVoices();
				for (int i = 0; i < 15; ++i)
				{
					synth.addVoice(new SquareWave);
				}
			}

			if (waveformList.getSelectedId() == 3)
			{
				synth.clearVoices();
				for (int i = 0; i < 15; ++i)
				{
					synth.addVoice(new TriangleWave);
				}
			}

			if (waveformList.getSelectedId() == 4)
			{
				synth.clearVoices();
				for (int i = 0; i < 15; ++i)
				{
					synth.addVoice(new SawtoothWave);
				}
			}
		}
	}

	void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
	{
	}

	void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
	{
	}

private:
	//==============================================================================
	void handleIncomingMidiMessage(MidiInput* /*source*/,
		const MidiMessage& message) override
	{
		visualiserInstrument.processNextMidiEvent(message);
		midiCollector.addMessageToQueue(message);
		const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
		keyboardState.processNextMidiEvent(message);
	}

	//==============================================================================
	AudioDeviceManager audioDeviceManager;         // [3]
	AudioDeviceSelectorComponent audioSetupComp;   // [4]

	ComboBox waveformList;
	Label waveformListLabel;

	MidiKeyboardState keyboardState;
	MidiKeyboardComponent keyboardComponent;
	bool isAddingFromMidiInput;

	MPEInstrument visualiserInstrument;
	MPESynthesiser synth;
	MidiMessageCollector midiCollector;            // [5]

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

#endif  // MAINCOMPONENT_H_INCLUDED

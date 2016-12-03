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
	private MidiKeyboardStateListener,
	private Slider::Listener
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
		addAndMakeVisible(filterList);
		addAndMakeVisible(filterLabel);
		addAndMakeVisible(keyboardComponent);
		addAndMakeVisible(knob1);
		addAndMakeVisible(frequency);

		waveformList.addListener(this);
		filterList.addListener(this);
		keyboardState.addListener(this);
		knob1.addListener(this);

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
		
		filterLabel.setText("Filter:", dontSendNotification);
		filterLabel.attachToComponent(&filterList, true);
		filterList.addItem("None", 1);
		filterList.addItem("High Pass", 2);
		filterList.addItem("Low Pass", 3);
		filterList.addItem("Band Pass", 4);
		filterList.addItem("Notch", 5);
		filterList.setSelectedId(1);

		frequency.setText("Frequency", dontSendNotification);
		frequency.attachToComponent(&knob1, true);
		knob1.setRange(10.0, 10000.0);
		knob1.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
		knob1.setTextValueSuffix(" hz");
		knob1.setSliderStyle(Slider::Rotary);
		knob1.setColour(Slider::rotarySliderFillColourId, Colours::grey);
		knob1.setColour(Slider::rotarySliderOutlineColourId, Colours::white);

		synth.enableLegacyMode(24);
		synth.setVoiceStealingEnabled(false);
		visualiserInstrument.enableLegacyMode(24);

		m_cutOff = knob1.getValue();
	}

	~MainComponent()
	{
		audioDeviceManager.removeMidiInputCallback(String(), this);
		waveformList.removeListener(this);
		filterList.removeListener(this);
		knob1.removeListener(this);
	}

	//==============================================================================
	void paint(Graphics& g) override
	{
		g.fillAll(Colours::orange);
	}

	void resized() override
	{
		Rectangle<int> r(getLocalBounds());
		audioSetupComp.setBounds(r);
		waveformList.setBounds(100, 350, 200, 20);
		filterList.setBounds(1000, 350, 200, 20);
		knob1.setBounds(1000, 400, 80, 80);
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

		float* channelDataL = buffer.getWritePointer(0);
		float* channelDataR = buffer.getWritePointer(1);
		
		if (filterList.getSelectedId() == 2){
			filterL.processSamples(channelDataL, numSamples);
			filterR.processSamples(channelDataR, numSamples);
		}

		if (filterList.getSelectedId() == 3){
			filterL.processSamples(channelDataL, numSamples);
			filterR.processSamples(channelDataR, numSamples);
		}
	}

	void audioDeviceAboutToStart(AudioIODevice* device) override
	{
		const double sampleRate = device->getCurrentSampleRate();
		midiCollector.reset(sampleRate);
		synth.setCurrentPlaybackSampleRate(sampleRate);

		m_cutOff = knob1.getValue();

		if (filterList.getSelectedId() == 2)
			coefficients.makeHighPass(sampleRate, m_cutOff);
		if (filterList.getSelectedId() == 3)
			coefficients.makeLowPass(sampleRate, m_cutOff);
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

	void sliderValueChanged(Slider *slider)override
	{
		if (slider == &knob1)
		{
			m_cutOff = knob1.getValue();
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

	ComboBox filterList;
	Label filterLabel;
	
	Slider knob1;
	Label frequency;

	MidiKeyboardState keyboardState;
	MidiKeyboardComponent keyboardComponent;
	bool isAddingFromMidiInput;

	MPEInstrument visualiserInstrument;
	MPESynthesiser synth;
	MidiMessageCollector midiCollector;            // [5]

	IIRCoefficients coefficients;
	IIRFilter filterL;
	IIRFilter filterR;

	float m_cutOff;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

#endif  // MAINCOMPONENT_H_INCLUDED

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
        addAndMakeVisible(knob2);
        addAndMakeVisible(knob3);
        addAndMakeVisible(knob4);
        addAndMakeVisible(knob5);
        addAndMakeVisible(knob6);
        addAndMakeVisible(knob7);
        

		waveformList.addListener(this);
		filterList.addListener(this);
		keyboardState.addListener(this);
		knob1.addListener(this);
        knob2.addListener(this);
        knob3.addListener(this);
        knob4.addListener(this);
        knob5.addListener(this);
        knob6.addListener(this);
        knob7.addListener(this);

		for (int i = 0; i < 15; ++i)
		{
			synth.addVoice(new SineWave);
		}

		waveformListLabel.setText("Waveform:", dontSendNotification);
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
		filterList.addItem("Peak", 4);
		//filterList.addItem("Notch", 5);
		filterList.setSelectedId(1);
        
        /*Filter*/
        //Fc
		frequency.setText("Fc", dontSendNotification);
		frequency.attachToComponent(&knob1, true);
		knob1.setRange(20.0, 20000.0);
		knob1.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
		knob1.setTextValueSuffix(" hz");
		knob1.setSliderStyle(Slider::Rotary);
		knob1.setColour(Slider::rotarySliderFillColourId, Colours::grey);
		knob1.setColour(Slider::rotarySliderOutlineColourId, Colours::white);
        
        //Q
        Q.setText("Q", dontSendNotification);
        Q.attachToComponent(&knob6, true);
        knob6.setRange(0.01, 4.0);
        knob6.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        knob6.setTextValueSuffix(" Q");
        knob6.setSliderStyle(Slider::Rotary);
        knob6.setColour(Slider::rotarySliderFillColourId, Colours::grey);
        knob6.setColour(Slider::rotarySliderOutlineColourId, Colours::white);
        
        //gain
        resonance.setText("Resonance", dontSendNotification);
        resonance.attachToComponent(&knob7, true);
        knob7.setRange(0.01, 1.0);
        knob7.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        knob7.setTextValueSuffix(" res");
        knob7.setSliderStyle(Slider::Rotary);
        knob7.setColour(Slider::rotarySliderFillColourId, Colours::grey);
        knob7.setColour(Slider::rotarySliderOutlineColourId, Colours::white);
        
        /*ADSR*/
        //Attack
        attack.setText("Attack", dontSendNotification);
        attack.attachToComponent(&knob2, true);
        knob2.setRange(0.0001, 1.0);
        knob2.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        knob2.setTextValueSuffix(" sec");
        knob2.setSliderStyle(Slider::Rotary);
        knob2.setColour(Slider::rotarySliderFillColourId, Colours::grey);
        knob2.setColour(Slider::rotarySliderOutlineColourId, Colours::white);
        
        //Decay
        decay.setText("Decay", dontSendNotification);
        decay.attachToComponent(&knob3, true);
        knob3.setRange(0.0001, 1.0);
        knob3.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        knob3.setTextValueSuffix(" sec");
        knob3.setSliderStyle(Slider::Rotary);
        knob3.setColour(Slider::rotarySliderFillColourId, Colours::grey);
        knob3.setColour(Slider::rotarySliderOutlineColourId, Colours::white);
        
        //Sustain
        sustain.setText("Sustain", dontSendNotification);
        sustain.attachToComponent(&knob4, true);
        knob4.setRange(0.0001, 1.0);
        knob4.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        knob4.setTextValueSuffix(" sec");
        knob4.setSliderStyle(Slider::Rotary);
        knob4.setColour(Slider::rotarySliderFillColourId, Colours::grey);
        knob4.setColour(Slider::rotarySliderOutlineColourId, Colours::white);
        
        //Release
        release.setText("Release", dontSendNotification);
        release.attachToComponent(&knob5, true);
        knob5.setRange(0.0001, 1.0);
        knob5.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 15);
        knob5.setTextValueSuffix(" sec");
        knob5.setSliderStyle(Slider::Rotary);
        knob5.setColour(Slider::rotarySliderFillColourId, Colours::grey);
        knob5.setColour(Slider::rotarySliderOutlineColourId, Colours::white);

		synth.enableLegacyMode(24);
		synth.setVoiceStealingEnabled(false);
		visualiserInstrument.enableLegacyMode(24);

		m_cutOff = knob1.getValue();
        m_q = knob6.getValue();
        m_resonance = knob7.getValue();
        m_attack = knob2.getValue();
        m_decay = knob3.getValue();
        m_sustain = knob4.getValue();
        m_release = knob5.getValue();
        
	}

	~MainComponent()
	{
		audioDeviceManager.removeMidiInputCallback(String(), this);
		waveformList.removeListener(this);
		filterList.removeListener(this);
		knob1.removeListener(this);
        knob2.removeListener(this);
        knob3.removeListener(this);
        knob4.removeListener(this);
        knob5.removeListener(this);
        knob6.removeListener(this);
        knob7.removeListener(this);
        
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
		knob1.setBounds(1130, 400, 80, 80);
        knob6.setBounds(1030, 400, 80, 80);
        knob7.setBounds(930, 400, 80, 80);
        knob2.setBounds(60, 400, 80, 80);
        knob3.setBounds(180, 400, 80, 80);
        knob4.setBounds(300, 400, 80, 80);
        knob5.setBounds(430, 400, 80, 80);
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
        // ADSR

        
		// get the MIDI messages for this audio block
		midiCollector.removeNextBlockOfMessages(incomingMidi, numSamples);

		// synthesise the block
		synth.renderNextBlock(buffer, incomingMidi, 0, numSamples);

		float* channelDataL = buffer.getWritePointer(0);
		float* channelDataR = buffer.getWritePointer(1);
		
        filterL.processSamples(channelDataL, numSamples);
        filterR.processSamples(channelDataR, numSamples);
	}
    

	void audioDeviceAboutToStart(AudioIODevice* device) override
	{
		const double sampleRate = device->getCurrentSampleRate();
        m_sampleRate = sampleRate;
		midiCollector.reset(sampleRate);
		synth.setCurrentPlaybackSampleRate(sampleRate);

		m_cutOff = knob1.getValue();
        m_attack = knob2.getValue();
        m_decay = knob3.getValue();
        m_sustain = knob4.getValue();
        m_release = knob5.getValue();
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
        
        if (box == &filterList)
        {
            if (filterList.getSelectedId() == 2)
                coefficients.makeHighPass(m_sampleRate, 1000);
                
            if (filterList.getSelectedId() == 3)
                coefficients.makeLowPass(m_sampleRate, 1000);
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
	
    //Filter
    IIRCoefficients coefficients;
    IIRFilter filterL;
    IIRFilter filterR;
    
	Slider knob1; //Fc
    Slider knob6; //Q
    Slider knob7; //Resonance/Gain
	Label frequency;
    Label Q;
    Label resonance; //aka, gain
    float m_cutOff;
    float m_q;
    float m_resonance;

    //ADSR
    Slider knob2; //attack
    Slider knob3; //decay
    Slider knob4; //sustain
    Slider knob5; //release
    
    Label attack;
    Label decay;
    Label sustain;
    Label release;
    float m_attack;
    float m_decay;
    float m_sustain;
    float m_release;
    
	MidiKeyboardState keyboardState;
	MidiKeyboardComponent keyboardComponent;
	bool isAddingFromMidiInput;

	MPEInstrument visualiserInstrument;
	MPESynthesiser synth;
	MidiMessageCollector midiCollector;            // [5]


    
    double m_sampleRate; //Fs

    
    // initialize settings


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

#endif  // MAINCOMPONENT_H_INCLUDED

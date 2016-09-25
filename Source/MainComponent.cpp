/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public AudioAppComponent,
	private ComboBox::Listener,
	private Button::Listener,
	private MidiInputCallback
{
public:
	MainContentComponent()
		:lastInputIndex(0),
		isAddingFromMidiInput(false)
	{
		addAndMakeVisible(sineButton);
		sineButton.setButtonText("Sine Wave");
		sineButton.addListener(this);

		addAndMakeVisible(squareButton);
		squareButton.setButtonText("Square Wave");
		squareButton.addListener(this);

		addAndMakeVisible(triangleButton);
		triangleButton.setButtonText("Triangle Wave");
		triangleButton.addListener(this);

		addAndMakeVisible(sawtoothButton);
		sawtoothButton.setButtonText("Sawtooth Wave");
		sawtoothButton.addListener(this);

		addAndMakeVisible(midiInputListLabel);
		midiInputListLabel.setText("MIDI Input:", dontSendNotification);
		midiInputListLabel.attachToComponent(&midiInputList, true);

		addAndMakeVisible(midiInputList);
		midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
		const StringArray midiInputs(MidiInput::getDevices());
		midiInputList.addItemList(midiInputs, 1);
		midiInputList.addListener(this);

		// find the first enabled device and use that by default
		for (int i = 0; i < midiInputs.size(); ++i)
		{
			if (deviceManager.isMidiInputEnabled(midiInputs[i]))
			{
				setMidiInput(i);
				break;
			}
		}

		// if no enabled devices were found just use the first one in the list
		if (midiInputList.getSelectedId() == 0)
			setMidiInput(0);

		setAudioChannels(2, 2);
		setSize(600, 400);
	}

	~MainContentComponent()
	{
		deviceManager.removeMidiInputCallback(MidiInput::getDevices()[midiInputList.getSelectedItemIndex()], this);
		midiInputList.removeListener(this);
		shutdownAudio();
	}

	void prepareToPlay(int samplesPerBlockExpected, double newSampleRate) override
	{
		sampleRate = newSampleRate;
		expectedSamplesPerBlock = samplesPerBlockExpected;
	}

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
		{
			float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);

			for (int i = 0; i < bufferToFill.numSamples; ++i)
			{

			}
			bufferToFill.clearActiveBufferRegion();
		}
	}

	void releaseResources() override
	{
		// This will be called when the audio device stops, or when it is being
		// restarted due to a setting change.
		// For more details, see the help for AudioProcessor::releaseResources()
	}

	void paint(Graphics& g) override
	{
		g.fillAll(Colours::grey);
	}

	void resized() override
	{

	}

private:
	/** Starts listening to a MIDI input device, enabling it if necessary. */
	
	void setMidiInput(int index)
	{
		const StringArray list(MidiInput::getDevices());

		deviceManager.removeMidiInputCallback(list[lastInputIndex], this);

		const String newInput(list[index]);

		if (!deviceManager.isMidiInputEnabled(newInput))
			deviceManager.setMidiInputEnabled(newInput, true);

		deviceManager.addMidiInputCallback(newInput, this);
		midiInputList.setSelectedId(index + 1, dontSendNotification);

		lastInputIndex = index;
	}

	void comboBoxChanged(ComboBox* box) override
	{
		if (box == &midiInputList)
			setMidiInput(midiInputList.getSelectedItemIndex());
	}

	void buttonClicked(Button* button) override
	{
		
	}

	// These methods handle callbacks from the midi device + on-screen keyboard..
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override
	{
		const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
	}

	//==============================================================================
	AudioDeviceManager deviceManager;           // [1]
	ComboBox midiInputList;                     // [2]
	Label midiInputListLabel;
	TextButton sineButton;
	TextButton squareButton;
	TextButton triangleButton;
	TextButton sawtoothButton;
	int lastInputIndex;                         // [3]
	bool isAddingFromMidiInput;                 // [4]
	double sampleRate;
	int expectedSamplesPerBlock;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent);
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
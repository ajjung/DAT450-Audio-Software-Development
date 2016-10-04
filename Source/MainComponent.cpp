/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Waveform.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class MainContentComponent : public Component,
	private ComboBox::Listener,
	private MidiInputCallback
{
public:
	MainContentComponent()
		:lastInputIndex(0),
		isAddingFromMidiInput(false)
	{
       // double currentSampleRate, currentAngle, angleDelta;
        
		addAndMakeVisible(midiInputListLabel);
		midiInputListLabel.setText("MIDI Input:", dontSendNotification);
		midiInputListLabel.attachToComponent(&midiInputList, true);

		addAndMakeVisible(waveformListLabel);
		waveformListLabel.setText("Waveform:", dontSendNotification);
		waveformListLabel.attachToComponent(&waveformList, true);

		addAndMakeVisible(waveformList);
		waveformList.addItem("Sine Wave", 1);
		waveformList.addItem("Square Wave", 2);
		waveformList.addItem("Triangle Wave", 3);
		waveformList.addItem("Sawtooth Wave", 4);
		waveformList.setSelectedId(1);

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

		setSize(600, 400);
	}

	~MainContentComponent()
	{
		deviceManager.removeMidiInputCallback(MidiInput::getDevices()[midiInputList.getSelectedItemIndex()], this);
		midiInputList.removeListener(this);
	}


	void paint(Graphics& g) override
	{
		g.fillAll(Colours::grey);
	}

	void resized() override
	{
		midiInputList.setBounds(200, 10, 200, 20);
		waveformList.setBounds(10, 40, 200, 20);
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

		if (box == &waveformList)
		{
            if(waveformList.getSelectedId() == 1){
                
            }
		}

	}

	// These methods handle callbacks from the midi device + on-screen keyboard..
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override
	{
		const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
        keyboardState.processNextMidiEvent(message);
	}

	//==============================================================================
	AudioDeviceManager deviceManager;           // [1]
	ComboBox midiInputList;                     // [2]
	Label midiInputListLabel;
	ComboBox waveformList;
	Label waveformListLabel;    
    
    MidiMessage midi;
    MidiKeyboardState keyboardState;

    
	int lastInputIndex;                         // [3]
	bool isAddingFromMidiInput;                 // [4]
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent);
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
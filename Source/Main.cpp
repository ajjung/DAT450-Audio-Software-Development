/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

struct MPETestClasses
{
#include "SquareWave.h"
#include "SineWave.h"
#include "TriangleWave.h"
#include "SawtoothWave.h"
#include "MainComponent.h"
};

//==============================================================================
class MIDISynthesizerApplication  : public JUCEApplication
{
public:
    //==============================================================================
    MIDISynthesizerApplication() {}

	const String getApplicationName() override       { return ProjectInfo::projectName; }
	const String getApplicationVersion() override    { return ProjectInfo::versionString; }
	bool moreThanOneInstanceAllowed() override       { return true; }

	//==============================================================================
	void initialise(const String&) override
	{
		mainWindow = new MainWindow(getApplicationName());
	}

	void shutdown() override
	{
		mainWindow = nullptr;
	}

	void systemRequestedQuit() override
	{
		quit();
	}

	//==============================================================================
	class MainWindow : public DocumentWindow
	{
	public:
		MainWindow(String name)
			: DocumentWindow(name, Colours::lightgrey, DocumentWindow::allButtons)
		{
			setUsingNativeTitleBar(true);
			setContentOwned(new MPETestClasses::MainComponent(), true);

			setResizable(true, false);
			centreWithSize(getWidth(), getHeight());
			setVisible(true);
		}

		void closeButtonPressed() override
		{
			JUCEApplication::getInstance()->systemRequestedQuit();
		}
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
	};

private:
	//==============================================================================
	ScopedPointer<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (MIDISynthesizerApplication)

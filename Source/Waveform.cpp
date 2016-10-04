#include "Waveform.h"

<<<<<<< HEAD
void WaveformF::setSampleRate(double samplerate)
{
	newSampleRate = samplerate;
}

float WaveformF::sineWave(double freq, float n)
{
	float sine = sin(freq) * 2 * M_PI * (n / newSampleRate);
    return sine;
}

float WaveformF::squareWave(double freq, float n)
{
    float square;
    return square;
}

float WaveformF::triangleWave(double freq, float n)
{
    float triangle;
    return triangle;
}

float WaveformF::sawToothWave(double freq, float n)
{
    float sawTooth;
    return sawTooth;
}

void WaveformF::switchF(int caseNum, float n, double freq)
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
        default:
            break;
    }
}

void prepareToPlay(int samplesPerBlockExpected, double SampleRate) override
{
	newSampleRate = SampleRate;
}

void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
{
	for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
	{
		float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
		for (int i = 0; i < bufferToFill.numSamples; ++i)
		{
			if (chan == 0)
			{

			}

			else if (chan == 1)
			{

			}
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

=======
Waveform::Waveform()
{

}

void Waveform::setSampleRate(double samplerate)
{
	newSampleRate = samplerate;
}
>>>>>>> origin/master

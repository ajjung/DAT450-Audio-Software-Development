/*
  ==============================================================================

    SquareWave.h
    Created: 6 Oct 2016 5:46:36pm
    Author:  Anthony Jung

  ==============================================================================
*/

#ifndef SQUAREWAVE_H_INCLUDED
#define SQUAREWAVE_H_INCLUDED
class SquareWave : public MPESynthesiserVoice
{
public:
    //==============================================================================
    SquareWave()
    : phase (0.0), phaseDelta (0.0), tailOff (0.0)
    {
    }
    
    //==============================================================================
    void noteStarted() override
    {
        jassert (currentlyPlayingNote.isValid());
        jassert (currentlyPlayingNote.keyState == MPENote::keyDown
                 || currentlyPlayingNote.keyState == MPENote::keyDownAndSustained);
        
        // get data from the current MPENote
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
        
        phase = 0.0;
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        
        tailOff = 0.0;
    }
    
    void noteStopped (bool allowTailOff) override
    {
        jassert (currentlyPlayingNote.keyState == MPENote::off);
        
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
            
            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                // stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
            clearCurrentNote();
            phaseDelta = 0.0;
        }
    }
    
    void notePressureChanged() override
    {
        level.setValue (currentlyPlayingNote.pressure.asUnsignedFloat());
    }
    
    void notePitchbendChanged() override
    {
        frequency.setValue (currentlyPlayingNote.getFrequencyInHertz());
    }
    
    void noteTimbreChanged() override
    {
        timbre.setValue (currentlyPlayingNote.timbre.asUnsignedFloat());
    }
    
    void noteKeyStateChanged() override
    {
    }
    
    void setCurrentSampleRate (double newRate) override
    {
        if (currentSampleRate != newRate)
        {
            noteStopped (false);
            currentSampleRate = newRate;
            
            level.reset (currentSampleRate, smoothingLengthInSeconds);
            timbre.reset (currentSampleRate, smoothingLengthInSeconds);
            frequency.reset (currentSampleRate, smoothingLengthInSeconds);
        }
    }
    
    //==============================================================================
    void renderNextBlock (AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override
    {
		if (phaseDelta != 0.0)
		{
			if (tailOff > 0.0)
			{
				for (int i = 0; i < numSamples; ++i)
				{
					const float currentSample = (float)(1 - 2 * (i * 2 / numSamples)) * (float)tailOff;

					for (int j = outputBuffer.getNumChannels(); --j >= 0;)
						outputBuffer.addSample(j, startSample, currentSample);

					++startSample;

					tailOff *= 0.99;

					if (tailOff <= 0.005)
					{
						clearCurrentNote();

						phaseDelta = 0.0;
						break;
					}
				}
			}
			else
			{
				for (int i = 0; i < numSamples; ++i)
				{
					const float currentSample = (float)(1 - 2 * (i * 2 / numSamples));

					for (int j = outputBuffer.getNumChannels(); --j >= 0;)
						outputBuffer.addSample(j, startSample, currentSample);

					++startSample;
				}
			}
		}
    }
    
private:
    //==============================================================================
    float getNextSample() noexcept
    {
        const double levelDb = (level.getNextValue() - 1.0) * maxLevelDb;
        const double amplitude = std::pow (10.0f, 0.05f * levelDb) * maxLevel;
        
        // timbre is used to blend between a sine and a square.
        const double f1 = std::cos (phase);
        const double f2 = std::copysign (1.0, f1);
        const double a2 = timbre.getNextValue();
        const double a1 = 1.0 - a2;
        
        const float nextSample = float (amplitude * ((a2 * f2)));
        
        const double cyclesPerSample = frequency.getNextValue() / currentSampleRate;
        phaseDelta = 2.0 * double_Pi * cyclesPerSample;
        phase = std::fmod (phase + phaseDelta, 2.0 * double_Pi);
        
        return nextSample;
    }
    
    //==============================================================================
    LinearSmoothedValue<double> level, timbre, frequency;
    double phase, phaseDelta, tailOff;
    
    // some useful constants
    const double maxLevel = 1.0f;
    const double maxLevelDb = 31.0f;
    const double smoothingLengthInSeconds = 0.01;
};




#endif  // SQUAREWAVE_H_INCLUDED

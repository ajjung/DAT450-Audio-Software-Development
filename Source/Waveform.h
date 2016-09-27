class Waveform
{
	Waveform();
	void setSampleRate(double samplerate);
	void sineWave();
	void squareWave();
	void triangleWave();
	void sawtoothWave();

private:
	double newSampleRate;
};
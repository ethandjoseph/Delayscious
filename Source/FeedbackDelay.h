#pragma once
#include <JuceHeader.h>

class FeedbackDelay
{
	
public:
	FeedbackDelay() {};

	~FeedbackDelay() {};

    void reset();

    void prepare(juce::dsp::ProcessSpec spec);

    void setParameters(float delayTime, float feedback, float dry, float wet);

    void fillBuffer(juce::AudioBuffer<float>& buffer, int channel);

    void readFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel);

    void feedbackBuffer(juce::AudioBuffer<float>& buffer, int channel);

	void updateBufferPositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer);

    void process(juce::AudioBuffer<float>& buffer);

private:
    float sampleRate = 44100.0;
    float bufferSize = 512;
    float numChannels = 2;
    juce::AudioBuffer<float> delayBuffer;
    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> wetBuffer;
    float delayBufferSize = 512;
    float delayTimeLeft = 0.;
    float delayTimeRight = 0.;
    float fbLeft = 0.;
    float fbRight = 0.;
    float dry = 100.0;
    float wet = 50.0;
    int writePosition{ 0 };
};
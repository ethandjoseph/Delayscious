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

    void feedbackBuffer(juce::AudioBuffer<float>& buffer, int channel);

    void readFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel);

	void updateBufferPositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer);

    void process(juce::AudioBuffer<float>& buffer);

private:
    float sampleRate;
    float bufferSize;
    float numChannels;
    juce::AudioBuffer<float> delayBuffer;
    juce::AudioBuffer<float> dryBuffer;
    juce::AudioBuffer<float> wetBuffer;
    float delayBufferSize;
    float delayTimeLeft;
    float delayTimeRight;
    float fbLeft;
    float fbRight;
    float dry = 100.0;
    float wet = 50.0;
	float percent;
    int writePosition{ 0 };
};
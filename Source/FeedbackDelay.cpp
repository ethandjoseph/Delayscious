#pragma once
#include "FeedbackDelay.h"
#include <JuceHeader.h>

void FeedbackDelay::reset()
{
    delayBuffer.clear();
}

void FeedbackDelay::prepare(juce::dsp::ProcessSpec spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);
	jassert(spec.maximumBlockSize > 0);
    sampleRate = spec.sampleRate;
	numChannels = spec.numChannels;
	bufferSize = spec.maximumBlockSize;

	dryBuffer.setSize(numChannels, bufferSize);
	wetBuffer.setSize(numChannels, bufferSize);

	delayBufferSize = sampleRate * 2.0;
    delayBuffer.setSize(numChannels, (int)delayBufferSize);

    reset();
    /*for (int ch = 0; ch < numChannels; ++ch)
    {
        delayInMillis[ch].reset(sampleRate, 0.05f);
        feedback[ch].reset(sampleRate, 0.05f);
    }*/
}

void FeedbackDelay::setParameters(float newDelayTime, float newFeedback, float newDry, float newWet)
{
	delayTimeLeft = newDelayTime;
	delayTimeRight = newDelayTime;
	fbLeft = newFeedback;
	fbRight = newFeedback;
	dry = newDry;
	wet = newWet;
}

void FeedbackDelay::fillBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
	float total = dry + wet;
	if (total == 0)
		percent = 0;
    else
        percent = wet / total;
    buffer.applyGain(0, bufferSize, 1.0f - percent);

    // Check to see if main buffer copies to delay buffer without needing to wrap...
    if (delayBufferSize >= bufferSize + writePosition)
    {
        // copy main buffer contents to delay buffer
        delayBuffer.copyFrom(channel, writePosition, buffer.getWritePointer(channel), bufferSize);
    }
    // if no
    else
    {
        // Determine how much space is left at the end of the delay buffer
        auto numSamplesToEnd = delayBufferSize - writePosition;

        // Copy that amount of contents to the end...
        delayBuffer.copyFrom(channel, writePosition, buffer.getWritePointer(channel), numSamplesToEnd);

        // Calculate how much contents is remaining to copy
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;

        // Copy remaining amount to beginning of delay buffer
        delayBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd), numSamplesAtStart);
    }
}

void FeedbackDelay::feedbackBuffer(juce::AudioBuffer<float>& buffer, int channel)
{

    auto fb = channel == 0 ? fbLeft : fbRight;

    // Check to see if main buffer copies to delay buffer without needing to wrap...
    if (delayBufferSize >= bufferSize + writePosition)
    {
        // copy main buffer contents to delay buffer
        delayBuffer.addFromWithRamp(channel, writePosition, buffer.getWritePointer(channel), bufferSize, fb, fb);
    }
    // if no
    else
    {
        // Determine how much space is left at the end of the delay buffer
        auto numSamplesToEnd = delayBufferSize - writePosition;

        // Copy that amount of contents to the end...
        delayBuffer.addFromWithRamp(channel, writePosition, buffer.getWritePointer(channel), numSamplesToEnd, fb, fb);

        // Calculate how much contents is remaining to copy
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;

        // Copy remaining amount to beginning of delay buffer
        delayBuffer.addFromWithRamp(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd), numSamplesAtStart, fb, fb);
    }
}

void FeedbackDelay::readFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel)
{
    auto g = wet/100.0f;
    auto dryGain = dry/100.0f;

    auto delayTime = channel == 0 ? delayTimeLeft : delayTimeRight;

    // delayMs
    auto readPosition = std::round(writePosition - (sampleRate * delayTime / 1000.0f));

    if (readPosition < 0)
        readPosition += delayBufferSize;

    buffer.applyGainRamp(0, bufferSize, dryGain, dryGain);

    if (readPosition + bufferSize < delayBufferSize)
    {
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), bufferSize, g, g);
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - readPosition;
        buffer.addFromWithRamp(channel, 0, delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, g, g);

        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
        buffer.addFromWithRamp(channel, numSamplesToEnd, delayBuffer.getReadPointer(channel, 0), numSamplesAtStart, g, g);
    }
}

void FeedbackDelay::updateBufferPositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer)
{
    auto delayBufferSize = delayBuffer.getNumSamples();

    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}

void FeedbackDelay::process(juce::AudioBuffer<float>& buffer)
{
	auto totalNumInputChannels = buffer.getNumChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        fillBuffer(buffer, channel);
        readFromBuffer(buffer, delayBuffer, channel);
        feedbackBuffer(buffer, channel);
    }

    updateBufferPositions(buffer, delayBuffer);
}
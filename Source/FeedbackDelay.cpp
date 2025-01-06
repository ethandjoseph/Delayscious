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
}

void FeedbackDelay::setParameters(float newDelayTime, float newFeedback, float newDry, float newWet)
{
	delayTimeLeft = newDelayTime;
	delayTimeRight = newDelayTime;
	fbLeft = newFeedback;
	fbRight = newFeedback;
	//dry = juce::Decibels::decibelsToGain(newDry);
	//wet = juce::Decibels::decibelsToGain(newWet);
	dry = newDry/100.0f;
	wet = newWet/100.0f;
}

void FeedbackDelay::fillBuffer(juce::AudioBuffer<float>& buffer, int channel)
{
    // Check to see if main buffer copies to delay buffer without needing to wrap...
    if (delayBufferSize >= bufferSize + writePosition)
    {
        // copy main buffer contents to delay buffer
        delayBuffer.copyFrom(channel, writePosition, buffer.getReadPointer(channel), bufferSize);
    }
    // if no
    else
    {
        // Determine how much space is left at the end of the delay buffer
        auto numSamplesToEnd = delayBufferSize - writePosition;

        // Copy that amount of contents to the end...
        delayBuffer.copyFrom(channel, writePosition, buffer.getReadPointer(channel), numSamplesToEnd);

        // Calculate how much contents is remaining to copy
        auto numSamplesAtStart = bufferSize - numSamplesToEnd;

        // Copy remaining amount to beginning of delay buffer
        delayBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd), numSamplesAtStart);
    }
}

void FeedbackDelay::readFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, int channel)
{
    auto delayTime = channel == 0 ? delayTimeLeft : delayTimeRight;

    // delayMs
    auto readPosition = std::round(writePosition - (sampleRate * delayTime / 1000.0f));

    if (readPosition < 0)
        readPosition += delayBufferSize;

    if (readPosition + bufferSize < delayBufferSize)
    {
		buffer.addFrom(channel, 0, delayBuffer, channel, readPosition, bufferSize);
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - readPosition;
		buffer.addFrom(channel, 0, delayBuffer, channel, readPosition, numSamplesToEnd);

        auto numSamplesAtStart = bufferSize - numSamplesToEnd;
		buffer.addFrom(channel, numSamplesToEnd, delayBuffer, channel, 0, numSamplesAtStart);
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

void FeedbackDelay::updateBufferPositions(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer)
{
    auto delayBufferSize = delayBuffer.getNumSamples();

    writePosition += bufferSize;
    writePosition %= delayBufferSize;
}

void FeedbackDelay::process(juce::AudioBuffer<float>& buffer)
{
	dryBuffer = buffer;
    buffer.clear();
	wetBuffer.clear();

    auto totalNumInputChannels = wetBuffer.getNumChannels();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        fillBuffer(dryBuffer, channel);
        readFromBuffer(wetBuffer, delayBuffer, channel);
        feedbackBuffer(wetBuffer, channel);
    }

    updateBufferPositions(wetBuffer, delayBuffer);


	dryBuffer.applyGain(0, bufferSize, dry);
	wetBuffer.applyGain(0, bufferSize, wet);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.addFrom(channel, 0, dryBuffer, channel, 0, buffer.getNumSamples());
        buffer.addFrom(channel, 0, wetBuffer, channel, 0, buffer.getNumSamples());
    }
}
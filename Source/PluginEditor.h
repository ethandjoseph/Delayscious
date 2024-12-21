#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class DelayciousAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DelayciousAudioProcessorEditor (DelayciousAudioProcessor&);
    ~DelayciousAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    DelayciousAudioProcessor& audioProcessor;
	juce::Slider delayTimeSlider;
	juce::Slider feedbackSlider;
	juce::Slider drySlider;
	juce::Slider wetSlider;
	juce::Label delayTimeLabel;
	juce::Label feedbackLabel;
	juce::Label dryLabel;
	juce::Label wetLabel;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayciousAudioProcessorEditor)
};
#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayciousAudioProcessorEditor::DelayciousAudioProcessorEditor (DelayciousAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	drySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	wetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

	delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	drySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	wetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

	delayTimeSlider.setTextValueSuffix(" ms");
	feedbackSlider.setTextValueSuffix(" %");
	drySlider.setTextValueSuffix(" dB");
	wetSlider.setTextValueSuffix(" dB");

	delayTimeLabel.setText("Delay Time", juce::dontSendNotification);
	feedbackLabel.setText("Feedback", juce::dontSendNotification);
	dryLabel.setText("Dry", juce::dontSendNotification);
	wetLabel.setText("Wet", juce::dontSendNotification);

	delayTimeLabel.attachToComponent(&delayTimeSlider, false);
	feedbackLabel.attachToComponent(&feedbackSlider, false);
	dryLabel.attachToComponent(&drySlider, false);
	wetLabel.attachToComponent(&wetSlider, false);

	addAndMakeVisible(delayTimeSlider);
	addAndMakeVisible(feedbackSlider);
	addAndMakeVisible(drySlider);
	addAndMakeVisible(wetSlider);

	addAndMakeVisible(delayTimeLabel);
	addAndMakeVisible(feedbackLabel);
	addAndMakeVisible(dryLabel);
	addAndMakeVisible(wetLabel);

	delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY_TIME", delayTimeSlider);
	feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FEEDBACK", feedbackSlider);
	dryAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRY", drySlider);
	wetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WET", wetSlider);

    setSize (600, 220);
}

DelayciousAudioProcessorEditor::~DelayciousAudioProcessorEditor()
{
}

void DelayciousAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkred);
}

void DelayciousAudioProcessorEditor::resized()
{
	auto area = getLocalBounds().reduced(32);
	auto sliderWidth = area.getWidth() / 4;
	delayTimeSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(8));
	feedbackSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(8));
	drySlider.setBounds(area.removeFromLeft(sliderWidth).reduced(8));
	wetSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(8));
}

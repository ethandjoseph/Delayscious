#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayciousAudioProcessorEditor::DelayciousAudioProcessorEditor (DelayciousAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	feedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	drySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	wetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

	delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
	feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
	drySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
	wetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);

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

    setSize (400, 200);
}

DelayciousAudioProcessorEditor::~DelayciousAudioProcessorEditor()
{
}

void DelayciousAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void DelayciousAudioProcessorEditor::resized()
{
	auto area = getLocalBounds().reduced(20);
	auto sliderWidth = area.getWidth() / 4;
	delayTimeSlider.setBounds(area.removeFromLeft(sliderWidth));
	feedbackSlider.setBounds(area.removeFromLeft(sliderWidth));
	drySlider.setBounds(area.removeFromLeft(sliderWidth));
	wetSlider.setBounds(area.removeFromLeft(sliderWidth));
}

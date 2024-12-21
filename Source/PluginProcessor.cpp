#include "PluginProcessor.h"
#include "PluginEditor.h"

DelayciousAudioProcessor::DelayciousAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
	apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

DelayciousAudioProcessor::~DelayciousAudioProcessor()
{
}

const juce::String DelayciousAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayciousAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayciousAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayciousAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayciousAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayciousAudioProcessor::getNumPrograms()
{
    return 1;
}

int DelayciousAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayciousAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayciousAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayciousAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void DelayciousAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
	delay.prepare(spec);

    float delayTime = *apvts.getRawParameterValue("DELAY_TIME");
    float feedback = *apvts.getRawParameterValue("FEEDBACK");
    float dry = *apvts.getRawParameterValue("DRY");
    float wet = *apvts.getRawParameterValue("WET");

    delay.setParameters(delayTime, feedback, dry, wet);
}

void DelayciousAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayciousAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// Process Block ==============================================================================
void DelayciousAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	float delayTime = *apvts.getRawParameterValue("DELAY_TIME");
	float feedback = *apvts.getRawParameterValue("FEEDBACK");
	float dry = *apvts.getRawParameterValue("DRY");
	float wet = *apvts.getRawParameterValue("WET");

	delay.setParameters(delayTime, feedback, dry, wet);
	delay.process(buffer);
}
//==============================================================================

bool DelayciousAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DelayciousAudioProcessor::createEditor()
{
    return new DelayciousAudioProcessorEditor (*this);
}

void DelayciousAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void DelayciousAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

juce::AudioProcessorValueTreeState::ParameterLayout DelayciousAudioProcessor::createParameterLayout()
{
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
	params.push_back(std::make_unique<juce::AudioParameterFloat>("DELAY_TIME", "Delay Time", 0.0f, 2000.0f, 0.5f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("FEEDBACK", "Feedback", 0.0f, 1.0f, 0.0f));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("DRY", "Dry", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f, 0.8f, false), 100.0f));
    //params.push_back(std::make_unique<juce::AudioParameterFloat>("WET", "Wet", juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f, 0.8f, false), 100.0f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>("DRY", "Dry", 0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("WET", "Wet", 0.0f, 1.0f, 0.5f));
	return { params.begin(), params.end() };
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayciousAudioProcessor();
}

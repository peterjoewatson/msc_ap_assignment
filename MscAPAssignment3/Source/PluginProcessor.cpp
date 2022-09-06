/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.
    Adapted by author: B191392

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
APAssignment3AudioProcessor::APAssignment3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
#endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                          ),
#endif
      myParams (*this),
      myNormalDelay (&myParams),
      myPingPongDelay (&myParams),
      myReverb (&myParams)
{
    for (int i = 0; i < voiceCount; i++)
    {
        mySynth.addVoice (new MySynthVoice (&myParams));
    }
    mySynth.addSound (new MySynthSound());
}

APAssignment3AudioProcessor::~APAssignment3AudioProcessor()
{
}

//==============================================================================
const juce::String APAssignment3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool APAssignment3AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool APAssignment3AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool APAssignment3AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double APAssignment3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int APAssignment3AudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int APAssignment3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void APAssignment3AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String APAssignment3AudioProcessor::getProgramName (int index)
{
    return {};
}

void APAssignment3AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void APAssignment3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    mySynth.setCurrentPlaybackSampleRate (sampleRate);
    myNormalDelay.prepareToPlay (sampleRate);
    myPingPongDelay.prepareToPlay (sampleRate);
    myReverb.prepareToPlay (sampleRate);
}

void APAssignment3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool APAssignment3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void APAssignment3AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
    mySynth.renderNextBlock (buffer, midiMessages, 0, numSamples);
    if (int (*myParams.delayType) == 0)
        myNormalDelay.apply (buffer, numSamples, numChannels);
    else
        myPingPongDelay.apply (buffer, numSamples, numChannels);
    myReverb.apply (buffer, numSamples);
}

//==============================================================================
bool APAssignment3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* APAssignment3AudioProcessor::createEditor()
{
    //    return new APAssignment3AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void APAssignment3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = myParams.apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void APAssignment3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (myParams.apvts.state.getType()))
        {
            myParams.apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new APAssignment3AudioProcessor();
}

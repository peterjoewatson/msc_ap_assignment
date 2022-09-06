/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.
    Adapted by author: B191392

  ==============================================================================
*/

#pragma once

#include "MyDelay.h"
#include "MyParameters.h"
#include "MyReverb.h"
#include "MySynth.h"
#include <JuceHeader.h>

//==============================================================================
/**
*/
class APAssignment3AudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    APAssignment3AudioProcessor();
    ~APAssignment3AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

private:
    MyParameters myParams;

    juce::Synthesiser mySynth;
    int voiceCount = 16;

    MyDelay myNormalDelay;
    MyPingPongDelay myPingPongDelay;
    MyReverb myReverb;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APAssignment3AudioProcessor)
};

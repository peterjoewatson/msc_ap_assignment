/*
  ==============================================================================

    MySynthesiser.h
    Created: Apr/May 2022
    Author: B191392
    Adapted from original author: Tom Mudd

    This implements the synth voice in MySynthVoice. It does not do much itself
    other than chain together the various sub components where most of the work
    is delegated to. On each iteration in the renderNextBlock the various
    dependencies are updated and then applied. From the perspective of this class
    this simply gets the source signal from the 3 elements (osc1, osc2 and noiseGen),
    sums them together and then passes them through the filter and amp modules.
    An LFO sample is gotten at the start of each iteration and passed to each
    sub-component along with a boolean of whether it applies to that compenent
    at the time. How the LFO is applied is left to the other classes.
 
  ==============================================================================
*/

#pragma once

#include "MyAmp.h"
#include "MyFilter.h"
#include "MyLfo.h"
#include "MyNoiseGenerator.h"
#include "MyOscillator.h"
#include "MyParameters.h"

// ===========================
// ===========================
// SOUND
class MySynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel (int) override { return true; }
};

// =================================
// =================================
// Synthesiser Voice - your synth code goes in here

/*!
 @class MySynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple MySynthVoice objects will be created by the Synthesiser so that it can be played polyphicially
 
 @namespace none
 @updated 2019-06-18
 */
class MySynthVoice : public juce::SynthesiserVoice
{
public:
    /**
     Constructor for MySynthVoice
     
     Note that the oscillator params need to be passed in specifically since an instance of  MyOscillator does not know which oscillator it actually is.
     
     @param _params A pointer to the user editable parameters.
     */
    MySynthVoice (MyParameters* _params) :
    osc1 (_params->osc1Type, _params->osc1Gain, _params->osc1Octave, _params->osc1Cents, _params->osc1Push),
    osc2 (_params->osc2Type, _params->osc2Gain, _params->osc2Octave, _params->osc2Cents, _params->osc2Push),
    noiseGen (_params),
    lfo (_params),
    filter (_params),
    amp (_params)
    {
        // empty
    }

    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts
            
     This calls start note on any dependencies that also require it.

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        ending = false;

        float frequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);

        osc1.startNote (frequency);
        osc2.startNote (frequency);
        noiseGen.startNote();

        filter.startNote();
        amp.startNote (velocity);
    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        noiseGen.stopNote();
        filter.stopNote();
        amp.stopNote();

        if (allowTailOff)
        {
            ending = true;
        }
        else
        {
            clearCurrentNote();
            playing = false;
        }
    }

    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here
     
     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (playing) // check to see if this voice should be playing
        {
            float sampleRate = getSampleRate();

            noiseGen.updateParams (sampleRate);
            amp.updateParams (sampleRate);

            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                // Get the LFO sample for this iteration to pass to the various subsystems.
                lfo.updateParams (sampleRate);
                float lfoSample = lfo.getNextSample();

                // Ensure the oscillators are up to date with the LFO or user parameters.
                osc1.updateParams (sampleRate, lfo.appliesToOsc1Frequency(), lfo.appliesToOsc1Cents(), lfoSample);
                osc2.updateParams (sampleRate, lfo.appliesToOsc2Frequency(), lfo.appliesToOsc2Cents(), lfoSample);

                // Create the source signal by summing the oscillators and the noise
                float sourceSample = osc1.getNextSample() + osc2.getNextSample() + noiseGen.getNextSample();
                
                // Apply the filter to the source signal
                float filteredSample = filter.apply (sampleRate, sourceSample, lfo.appliesToFilterFrequency(), lfo.appliesToFilterQ(), lfoSample);

                // Apply the amp envelope, distortion and output volume
                float ampedSample = amp.apply (filteredSample, lfo.appliesToAmpVolume(), lfo.appliesToAmpDistortion(), lfoSample);

                // for each channel, write the sample to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                    outputBuffer.addSample (chan, sampleIndex, ampedSample);

                // Clear the note once the amp envelope is finished.
                if (ending && amp.isClosed())
                {
                    clearCurrentNote();
                    playing = false;
                    ending = false;
                }
            }
        }
    }

    //--------------------------------------------------------------------------
    void pitchWheelMoved (int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved (int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of MySynthSound
     */
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<MySynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;
    bool ending = false;

    MyOscillator osc1;
    MyOscillator osc2;
    MyNoiseGenerator noiseGen;
    MyLfo lfo;

    MyFilter filter;
    MyAmp amp;
};

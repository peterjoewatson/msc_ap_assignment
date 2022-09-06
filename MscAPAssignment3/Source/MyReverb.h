/*
  ==============================================================================

    MyReverb.h
    Created: Apr/May 2022
    Author: B191392

    This is a simple wrapper around the Juce Reverb implementation. Parameters
    are provided in the UI and mapped directly to the Juce reverb parameters. The
    following parameters are provided for the reverb (directly reflecting the Juce
    parameters):
 
    * reverbOn: Whether to apply or bypass the reverb
    * reverbRoomSize: The size of the simulated room
    * reverbDamping: How much damping of higher frequencies will occur
    * reverbWetLevel: How much of the reverb signal is in the output
    * reverbDryLevel: How much of the original signal is in the output
    * reverbWidth: A factor controlling the stereo spread of the reverb

  ==============================================================================
*/

#pragma once

#include "MyParameters.h"
#include <JuceHeader.h>

class MyReverb
{
public:
    /**
     Constructor for MyReverb
     
     @param _params A pointer to the user editable parameters.
     */
    MyReverb (MyParameters* _params) : params (_params)
    {
        // empty
    }

    /**
     Must by called before use to set the sample rate and ensure the reverb is initially in a reset state
     
     @param sampleRate The current sampleRate
     */
    void prepareToPlay (double sampleRate)
    {
        reverb.setSampleRate (sampleRate);
        reset();
    }

    /**
     Applies the reveb to the given buffer if the reverb is turned on, otherwise simply returns without any processing of the buffer.
     
     @param buffer The buffer to apply the reverb to
     @param numSamples The number of samples in the buffer
     */
    void apply (juce::AudioBuffer<float>& buffer, int numSamples)
    {
        if (! params->reverbOn->get())
        {
            if (! isReset)
                reset();
            return;
        }

        // Set the flag that this is no longer in a clear reset state.
        isReset = false;

        updateParams();
        if (buffer.getNumChannels() < 2)
            reverb.processMono (buffer.getWritePointer (0), numSamples);
        else
            reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), numSamples);
    }

private:
    MyParameters* params;

    juce::Reverb reverb;
    juce::Reverb::Parameters reverbParams;

    // Helper flag to avoid resetting every time the filter is off.
    bool isReset = false;

    /**
     Simply maps the user params to the reverb parameters object and applies it to the reverb object.
     */
    void updateParams()
    {
        reverbParams.roomSize = *params->reverbRoomSize;
        reverbParams.damping = *params->reverbDamping;
        reverbParams.wetLevel = *params->reverbWetLevel;
        reverbParams.dryLevel = *params->reverbDryLevel;
        reverbParams.width = *params->reverbWidth;
        reverb.setParameters (reverbParams);
    }

    /**
     Resets the reverb and sets the flag to know this was done.
     */
    void reset()
    {
        reverb.reset();
        isReset = true;
    }
};

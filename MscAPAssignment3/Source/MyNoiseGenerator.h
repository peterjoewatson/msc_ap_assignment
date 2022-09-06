/*
  ==============================================================================

    NoiseGenerator.h
    Created: Apr/May 2022
    Author: B191392

    This implements a simple noise generator class that has the following user
    parameters:
 
    * noiseOn: Whether this should produce something or return 0
    * noiseGain: How loud the noise signal should be
    * noiseFilter: A simple low pass filter that colours the noise
    * noiseDuration: How long the noise should be
 
  ==============================================================================
*/

#pragma once

#include "MyParameters.h"
#include <JuceHeader.h>

class MyNoiseGenerator
{
public:
    MyNoiseGenerator (MyParameters* _myParams) : params (_myParams)
    {
        // We fix these values since they are present mostly just to avoid clicks
        noiseEnvParams.attack = 0.01f;
        // Note that the decay is set by user parameters in updateParams
        noiseEnvParams.sustain = 0.0f;
        noiseEnvParams.release = 0.01f;
    }

    void startNote()
    {
        noiseFilter.reset();
        noiseEnv.reset();
        noiseEnv.noteOn();
    }

    void stopNote()
    {
        noiseEnv.noteOff();
    }

    float getNextSample()
    {
        if (! params->noiseOn->get())
            return 0.0f;

        // Ensure a value between -1 and 1
        float noiseSample = (random.nextFloat() * 2) - 1;
        float filteredSample = noiseFilter.processSingleSampleRaw (noiseSample);
        float envelopedSample = noiseEnv.getNextSample() * filteredSample;
        return *params->noiseGain * envelopedSample;
    }

    void updateParams (float sampleRate)
    {
        float noiseFilterFreq = (*params->noiseFilter * 5000.0f) + 20;
        noiseFilter.setCoefficients (juce::IIRCoefficients::makeLowPass (sampleRate, noiseFilterFreq));

        // When the duration is set to it's maximum this becomes effectively infinite
        float noiseDurationVal = *params->noiseDuration;
        if (noiseDurationVal == 100.0f)
            noiseEnvParams.decay = 10000.0f;
        else
            noiseEnvParams.decay = noiseDurationVal;
        noiseEnv.setParameters (noiseEnvParams);
    }

private:
    MyParameters* params;

    juce::Random random;
    juce::IIRFilter noiseFilter;
    juce::ADSR noiseEnv;
    juce::ADSR::Parameters noiseEnvParams;
};

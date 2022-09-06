/*
  ==============================================================================

    MyAmp.h
    Created: Apr/May 2022
    Author: B191392
 
    This implements a amplifier section for the synth. It is responsible for the
    ADSR volume control, the velocity control, the distortion and the master
    volume. The available parameters are:

    * ampEnvAttack: Specifies the time for the envelope to ramp up to full
    * ampEnvDecay: Specifies the time for the envelope to ramp down to the sustain value
    * ampEnvSustain: Specifies the level the envelope will remain at after decay and before release
    * ampEnvRelease: Specifies the time for the envelope to ramp down fulls after the note stops
    * ampDistOn: Whether to apply the distortion
    * ampDistGain: How much gain to apply in the distortion
    * ampVolume: Final master volume.
 
    In addition to these user parameters, the velocity provided to the startNote
    function is used to also modify gain of the sample before applying the other
    modifications. This gives a more expressive playing sound and works particularly
    well with the distortion.
 
    The distortion is implemented similarly to the Push Square oscillator where
    the tanh function is used to clip the input. The gain factor increasing will
    cause more clipping and thus a harsher distortion.

    The volume and the distortion can also both be controlled by the LFO where
    the parameter is simply multiplied by the LFO sample, however, there is some
    protection to ensure that the value does not go above one.
 
  ==============================================================================
*/

#pragma once

#include <cmath>
#include <JuceHeader.h>
#include "MyParameters.h"

class MyAmp
{
public:
    MyAmp (MyParameters* _params) : params (_params)
    {
        // empty
    }

    void startNote (float velocity)
    {
        ampEnv.reset();
        ampEnv.noteOn();

        velocityGain = velocity;
    }

    void stopNote()
    {
        ampEnv.noteOff();
    }

    bool isClosed()
    {
        return envVal < 0.000001f;
    }

    float apply (float sample, bool applyLfoToAmpVolume, bool applyLfoToAmpDist, float lfoSample)
    {
        envVal = ampEnv.getNextSample();
        float envSample = velocityGain * (envVal * sample);

        float distSample;
        if (params->ampDistOn->get())
            distSample = tanh (getAmpDist (applyLfoToAmpDist, lfoSample) * envSample);
        else
            distSample = envSample;

        return getAmpVolume (applyLfoToAmpVolume, lfoSample) * distSample;
    }

    float getAmpDist (float applyLfo, float lfoSample)
    {
        float ampDistGain = *params->ampDistGain;

        if (applyLfo)
        {
            return std::max (0.0f, ampDistGain + (lfoSample * ampDistGain));
        }
        else
        {
            return ampDistGain;
        }
    }

    float getAmpVolume (bool applyLfo, float lfoSample)
    {
        float ampVolume = *params->ampVolume;

        if (applyLfo)
        {
            float lfoVolume = ampVolume + (lfoSample * ampVolume);
            return std::max (0.0f, std::min (1.0f, lfoVolume));
        }
        else
        {
            return ampVolume;
        }
    }

    void updateParams (float sampleRate)
    {
        ampEnv.setSampleRate (sampleRate);
        ampEnvParams.attack = *params->ampEnvAttack;
        ampEnvParams.decay = *params->ampEnvDecay;
        ampEnvParams.sustain = *params->ampEnvSustain;
        ampEnvParams.release = *params->ampEnvRelease;
        ampEnv.setParameters (ampEnvParams);
    }

private:
    MyParameters* params;

    juce::ADSR ampEnv;
    juce::ADSR::Parameters ampEnvParams;

    float velocityGain;

    float envVal = 0;
};

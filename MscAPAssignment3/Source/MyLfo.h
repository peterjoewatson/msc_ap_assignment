/*
  ==============================================================================

    MyLfo.h
    Created: Apr/May 2022
    Author: B191392
 
    This implements an LFO with 5 wave shapes: Sine, Triangle, Square, Rising
    Sawtooth, Falling Sawtooth. There user controllable parameters are:
 
    * lfoOn: Determines whether the LFO is applied to anything
    * lfoType: Specifies the type of waveform
    * lfoAppliesTo: Specifies what parameter is modified by the LFO
    * lfoFrequency: Specifies the how fast the LFO oscillates
    * lfoDepth: Specifies the depth or strength of the LFO

    The depth of the LFO provides a strength factor. At 1 the outputted waves
    oscillate between -1 and 1. With a depth of 0.25 it would only oscillate
    between -0.25 and 0.25.
 
    This also provides a set of appliesTo* functions that can be used to check
    if the LFO should apply to certain parameters. These are provided so that
    the knowledge of which index refers to which choice remains encapsulated
    within this. 

  ==============================================================================
*/

#pragma once

#include "MyParameters.h"
#include <cmath>

class MyLfo
{
public:
    MyLfo (MyParameters* _params) : params (_params)
    {
        // empty
    }

    void startNote (float _frequency)
    {
    }

    void updateParams (float sampleRate)
    {
        updatePhaseDelta (*params->lfoFrequency, sampleRate);
    }

    float getNextSample()
    {
        float sample;
        switch (int (*params->lfoType))
        {
            case 0:
                sample = getNextSampleSine();
                break;
            case 1:
                sample = getNextSampleTriangle();
                break;
            case 2:
                sample = getNextSampleSquare();
                break;
            case 3:
                sample = getNextSampleSaw();
                break;
            case 4:
                sample = getNextSampleInvertedSaw();
                break;
            default:
                sample = getNextSampleTriangle();
        }

        return *params->lfoDepth * sample;
    }

    bool appliesToOsc1Frequency() { return appliesTo (0, 4); }

    bool appliesToOsc1Cents() { return appliesTo (1, 5); }

    bool appliesToOsc2Frequency() { return appliesTo (2, 4); }

    bool appliesToOsc2Cents() { return appliesTo (3, 5); }

    bool appliesToFilterFrequency() { return appliesTo (6); }

    bool appliesToFilterQ() { return appliesTo (7); }

    bool appliesToAmpVolume() { return appliesTo (8); }

    bool appliesToAmpDistortion() { return appliesTo (9); }

private:
    MyParameters* params;

    float phaseDelta;
    float phase = 0;

    float pi2 = 2 * M_PI;

    bool appliesTo (int index) { return params->lfoOn->get() && int (*params->lfoAppliesTo) == index; }

    bool appliesTo (int index1, int index2)
    {
        int lfoAppliesTo = int (*params->lfoAppliesTo);
        return params->lfoOn->get() && (lfoAppliesTo == index1 || lfoAppliesTo == index2);
    }

    float getNextSampleSine()
    {
        return sin (pi2 * getNextPhase());
    }

    float getNextSampleTriangle()
    {
        return (fabs (getNextPhase() - 0.5) * 4) - 1;
    }

    float getNextSampleSquare()
    {
        if (getNextPhase() < 0.5)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

    float getNextSampleSaw()
    {
        return (getNextPhase() * 2) - 1;
    }

    float getNextSampleInvertedSaw()
    {
        return -getNextSampleSaw();
    }

    float getNextPhase()
    {
        phase += phaseDelta;

        if (phase > 1)
        {
            phase -= 1;
        }

        return phase;
    }

    void updatePhaseDelta (float frequency, float sampleRate)
    {
        phaseDelta = frequency / sampleRate;
    }
};

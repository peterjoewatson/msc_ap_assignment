/*
  ==============================================================================

    MyDelay.h
    Created: Apr/May 2022
    Author: B191392
    
    This file contains 2 different delay effect implementations. The first,
    MyPingPongDelay is a stereo effect where the echoes created by the delay
    alternate being in the left or right channel with how far left or right
    determined by a depth parameter. The second is a delay that maintains the
    left/right stereo field (which turned out to be identical in the assignment
    but it would still handle stereo differneces). Apart from the depth parameter,
    the other paramaters are shared:
 
    * delayOn: Whether the delay effect is to be applied or bypassed
    * delayType: Which of the two delay types to use
    * delayTime: The time between the original and delayed signal
    * delayWetLevel: How much of the delayed signal is in the output
    * delayDryLevel: How much of the original signal is in the output
    * delayFeedback: How much of the delayed sample is fed back into the buffer to give more repeated echoes
    * delayDepth: How far left and right the Ping Pong delay travels
    
  ==============================================================================
*/

#pragma once

#include "MyParameters.h"
#include <JuceHeader.h>
#include <cmath>

class MyPingPongDelay
{
public:
    MyPingPongDelay (MyParameters* _params) : params (_params)
    {
    }

    ~MyPingPongDelay()
    {
        delete leftDelayBuffer;
        delete rightDelayBuffer;
    }

    void prepareToPlay (double _sampleRate)
    {
        sampleRate = _sampleRate;
        resetBuffers();
        smoothDelayInSamples.reset (_sampleRate, 0.1f);
        smoothDelayInSamples.setCurrentAndTargetValue (0.5f * sampleRate);

        smoothFrequency.reset (_sampleRate, 0.1f);
        smoothDelayInSamples.setCurrentAndTargetValue (0.5f * sampleRate);
    }

    void apply (juce::AudioBuffer<float>& buffer, int numSamples, int numChannels)
    {
        if (! params->delayOn->get())
        {
            if (! emptyBuffers)
            {
                clearBuffers();
            }
            return;
        }

        emptyBuffers = false;

        float* leftChannel = buffer.getWritePointer (0);
        float* rightChannel = nullptr;

        bool rightChannelAvailable = numChannels > 1;

        if (rightChannelAvailable)
        {
            rightChannel = buffer.getWritePointer (1);
        }

        for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++)
        {
            updateParams();
            float exactDelayInSamples = smoothDelayInSamples.getNextValue();

            float originalLeftSample = leftChannel[sampleIndex];
            float originalRightSample;
            float originalSample;
            if (rightChannelAvailable)
            {
                originalRightSample = rightChannel[sampleIndex];
                originalSample = (originalLeftSample + originalRightSample) / 2.0f;
            }
            else
            {
                originalSample = leftChannel[sampleIndex];
            }

            float delayedLeftSample = getInterpolatedDelayedSample (leftDelayBuffer, exactDelayInSamples);
            float delayedRightSample = getInterpolatedDelayedSample (rightDelayBuffer, 2 * exactDelayInSamples);

            leftDelayBuffer[currentIndex] = originalSample + (*params->delayFeedback * delayedLeftSample);
            rightDelayBuffer[currentIndex] = originalSample + ((*params->delayFeedback / 2.0f) * delayedRightSample);

            float sameChannelGain = *params->delayDepth;
            float otherChannelGain = 1 - sameChannelGain;

            float delayWetLevel = *params->delayWetLevel;
            float leveledDelayedLeftSample = delayWetLevel * delayedLeftSample;
            float leveledDelayedRightSample = delayWetLevel * delayedRightSample;
            leftChannel[sampleIndex] = (*params->delayDryLevel * originalLeftSample)
                                       + (sameChannelGain * leveledDelayedLeftSample)
                                       + (otherChannelGain * leveledDelayedRightSample);

            if (rightChannelAvailable)
            {
                rightChannel[sampleIndex] = (*params->delayDryLevel * originalRightSample)
                                            + (sameChannelGain * leveledDelayedRightSample)
                                            + (otherChannelGain * leveledDelayedLeftSample);
            }
            incrementCurrentIndex();
        }
    }

private:
    MyParameters* params;

    float* leftDelayBuffer = nullptr;
    float* rightDelayBuffer = nullptr;

    float sampleRate;

    juce::SmoothedValue<float> smoothDelayInSamples;
    juce::SmoothedValue<float> smoothFrequency;

    int bufferSize;
    int currentIndex;
    bool emptyBuffers;

    float getInterpolatedDelayedSample (float* buffer, float exactDelayInSamples)
    {
        int delayInSamplesInt = std::floor (exactDelayInSamples);

        float delayInSamplesDecimal = exactDelayInSamples - delayInSamplesInt;

        int rightIndex = ((currentIndex - delayInSamplesInt) + bufferSize) % bufferSize;
        int leftIndex = (rightIndex - 1 + bufferSize) % bufferSize;

        float delayedSample = ((1 - delayInSamplesDecimal) * buffer[leftIndex]) + (delayInSamplesDecimal * buffer[rightIndex]);

        return delayedSample;
    }

    void updateParams()
    {
        float delayTime = *params->delayTime;
        smoothDelayInSamples.setTargetValue (delayTime * sampleRate);
        smoothFrequency.setTargetValue (1.0f / (2 * delayTime));
    }

    void incrementCurrentIndex()
    {
        currentIndex = (currentIndex + 1) % bufferSize;
    }

    void resetBuffers()
    {
        bufferSize = std::ceil (4 * sampleRate) + 1;

        if (leftDelayBuffer != nullptr)
        {
            delete leftDelayBuffer;
        }

        if (rightDelayBuffer != nullptr)
        {
            delete rightDelayBuffer;
        }

        leftDelayBuffer = new float[bufferSize];
        rightDelayBuffer = new float[bufferSize];
        clearBuffers();
    }

    void clearBuffers()
    {
        for (int i = 0; i < bufferSize; i++)
        {
            leftDelayBuffer[i] = 0.0f;
            rightDelayBuffer[i] = 0.0f;
        }
        currentIndex = 0;
        emptyBuffers = true;
    }
};

class MyDelay
{
public:
    MyDelay (MyParameters* _params) : params (_params)
    {
        // empty
    }

    ~MyDelay()
    {
        delete leftBuffer;
        delete rightBuffer;
    }

    void prepareToPlay (double _sampleRate)
    {
        sampleRate = _sampleRate;
        resetBuffers();
        smoothDelaySamples.reset (_sampleRate, 0.1f);
        smoothDelaySamples.setCurrentAndTargetValue (0.5f * sampleRate);
    }

    void apply (juce::AudioBuffer<float>& buffer, int numSamples, int numChannels)
    {
        if (! params->delayOn->get())
        {
            if (! emptyBuffers)
            {
                clearBuffers();
            }
            return;
        }

        emptyBuffers = false;

        float* leftChannel = buffer.getWritePointer (0);
        float* rightChannel = nullptr;

        bool rightChannelAvailable = numChannels > 1;

        if (rightChannelAvailable)
        {
            rightChannel = buffer.getWritePointer (1);
        }

        for (int i = 0; i < numSamples; i++)
        {
            updateParams();
            float delaySamples = smoothDelaySamples.getNextValue();

            applyDelay (leftChannel, i, leftBuffer, delaySamples);
            if (rightChannelAvailable)
            {
                applyDelay (rightChannel, i, rightBuffer, delaySamples);
            }
            incrementCurrentIndex();
        }
    }

private:
    MyParameters* params;

    float* leftBuffer = nullptr;
    float* rightBuffer = nullptr;

    float sampleRate;

    juce::SmoothedValue<float> smoothDelaySamples;

    int bufferSize;
    int currentIndex;
    bool emptyBuffers;

    void applyDelay (float* channel, int sampleIndex, float* buffer, float delaySamples)
    {
        float originalSample = channel[sampleIndex];

        int sampleDelay = std::floor (delaySamples);

        float decimal = delaySamples - sampleDelay;

        int rightIndex = ((currentIndex - sampleDelay) + bufferSize) % bufferSize;
        int leftIndex = (rightIndex - 1 + bufferSize) % bufferSize;

        float delayedSample = ((1 - decimal) * buffer[leftIndex]) + (decimal * buffer[rightIndex]);

        float newSample = channel[sampleIndex] = (*params->delayDryLevel * originalSample) + (*params->delayWetLevel * delayedSample);
        buffer[currentIndex] = originalSample + (*params->delayFeedback * delayedSample);
        channel[sampleIndex] = newSample;
    }

    void updateParams()
    {
        smoothDelaySamples.setTargetValue (*params->delayTime * sampleRate);
    }

    void incrementCurrentIndex()
    {
        currentIndex = (currentIndex + 1) % bufferSize;
    }

    void resetBuffers()
    {
        bufferSize = std::ceil (2 * sampleRate) + 1;

        if (leftBuffer != nullptr)
        {
            delete leftBuffer;
        }

        if (rightBuffer != nullptr)
        {
            delete rightBuffer;
        }

        leftBuffer = new float[bufferSize];
        rightBuffer = new float[bufferSize];
        clearBuffers();
    }

    void clearBuffers()
    {
        for (int i = 0; i < bufferSize; i++)
        {
            leftBuffer[i] = 0.0f;
            rightBuffer[i] = 0.0f;
        }
        currentIndex = 0;
        emptyBuffers = true;
    }
};


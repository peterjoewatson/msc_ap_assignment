/*
  ==============================================================================

    MyParameters.h
    Created: Apr/May 2022
    Author: B191392

    This is a central class that encapsulates all user editable parameters for
    the synth. An instance of this is created when the plugin starts and a
    pointer to it is passed to any classes that need it.
 
    This esentially wraps a Juce Audio Processor Value Tree State object and
    creates the params within it before pulling the references back out of it.
 
    There are also some helper classes provided that make the main code a bit
    cleaner.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// Specifying the std namepsace to help reduce the length of some longer lines.
using namespace std;

class MyParameters
{
public:
    
    /**
     Simple helper for making a float parameter.
     
     @param paramId The ID for this parameter
     @param paramName The user displayable name for the parameter
     @param minVal The lowest possible value for the parameter
     @param maxVal The highest possible value for the parameter
     @param defaultVal The default value for the parameter
     */
    static auto makeFloat (string paramId, string paramName, float minVal, float maxVal, float defaultVal)
    {
        return make_unique<juce::AudioParameterFloat> (paramId, paramName, minVal, maxVal, defaultVal);
    }

    /**
     Simple helper for making an int parameter.
     
     @param paramId The ID for this parameter
     @param paramName The user displayable name for the parameter
     @param minVal The lowest possible value for the parameter
     @param maxVal The highest possible value for the parameter
     @param defaultVal The default value for the parameter
     */
    static auto makeInt (string paramId, string paramName, int minVal, int maxVal, int defaultVal)
    {
        return make_unique<juce::AudioParameterInt> (paramId, paramName, minVal, maxVal, defaultVal);
    }

    /**
     Helper for making a skewed float parameter that skews the slider so more space is given to higher or lower values accordingly.
     
     @param paramId The ID for this parameter
     @param paramName The user displayable name for the parameter
     @param minVal The lowest possible value for the parameter
     @param maxVal The highest possible value for the parameter
     @param skewFactor Determins how the slider will be skewed. Lower skew factor gives more priority to low values
     @param defaultVal The default value for the parameter
     */
    static auto makeSkewedFloat (string paramId, string paramName, float minVal, float maxVal, float skewFactor, float defaultVal)
    {
        auto normalisableRange = juce::NormalisableRange<float> (minVal, maxVal, 0.000001f, skewFactor);
        return make_unique<juce::AudioParameterFloat> (paramId, paramName, normalisableRange, defaultVal);
    }

    /**
     Simple helper for making a boolean parameter.
     
     @param paramId The ID for this parameter
     @param paramName The user displayable name for the parameter
     @param defaultVal The default value for the parameter
     */
    static auto makeBool (string paramId, string paramName, bool defaultVal)
    {
        return make_unique<juce::AudioParameterBool> (paramId, paramName, defaultVal);
    }

    /**
     Simple helper for making a parameter with a list of specific choices.
     
     @param paramId The ID for this parameter
     @param paramName The user displayable name for the parameter
     @param _choices A list of strings for the available choices
     @param defaultVal The index of the default choice for the parameter
     */
    static auto makeChoice (string paramId, string paramName, const initializer_list<const char*>& _choices, int defaultChoice)
    {
        auto choices = juce::StringArray (_choices);
        return make_unique<juce::AudioParameterChoice> (paramId, paramName, choices, defaultChoice);
    }
    
    juce::AudioProcessorValueTreeState apvts;

    // Oscillator 1 Parameters
    juce::AudioParameterChoice* osc1Type;
    atomic<float>* osc1Gain;
    atomic<float>* osc1Octave;
    atomic<float>* osc1Cents;
    atomic<float>* osc1Push;

    // Oscillator 2 Parameters
    juce::AudioParameterChoice* osc2Type;
    atomic<float>* osc2Gain;
    atomic<float>* osc2Octave;
    atomic<float>* osc2Cents;
    atomic<float>* osc2Push;

    // Noise Generator Parameters
    juce::AudioParameterBool* noiseOn;
    atomic<float>* noiseGain;
    atomic<float>* noiseFilter;
    atomic<float>* noiseDuration;

    // LFO Parameters
    juce::AudioParameterBool* lfoOn;
    juce::AudioParameterChoice* lfoType;
    juce::AudioParameterChoice* lfoAppliesTo;
    atomic<float>* lfoFrequency;
    atomic<float>* lfoDepth;

    // Filter Parameters
    juce::AudioParameterBool* filterOn;
    juce::AudioParameterChoice* filterType;
    juce::AudioParameterChoice* filterAppliesTo;
    atomic<float>* filterFreq;
    atomic<float>* filterQ;
    atomic<float>* filterAttack;
    atomic<float>* filterDecay;
    atomic<float>* filterSustain;
    atomic<float>* filterRelease;

    // Amplitude Envelope and Distortion Parameters
    atomic<float>* ampEnvAttack;
    atomic<float>* ampEnvDecay;
    atomic<float>* ampEnvSustain;
    atomic<float>* ampEnvRelease;
    juce::AudioParameterBool* ampDistOn;
    atomic<float>* ampDistGain;
    atomic<float>* ampVolume;

    // Delay Parameters
    juce::AudioParameterBool* delayOn;
    juce::AudioParameterChoice* delayType;
    atomic<float>* delayTime;
    atomic<float>* delayWetLevel;
    atomic<float>* delayDryLevel;
    atomic<float>* delayFeedback;
    atomic<float>* delayDepth;

    // Reverb Parameters
    juce::AudioParameterBool* reverbOn;
    atomic<float>* reverbRoomSize;
    atomic<float>* reverbDamping;
    atomic<float>* reverbWetLevel;
    atomic<float>* reverbDryLevel;
    atomic<float>* reverbWidth;

    MyParameters (juce::AudioProcessor& audioProcessor)
        : apvts (audioProcessor,
                 nullptr,
                 "MyParameters",
                 {
                     // Oscillator 1 Parameters
                     makeChoice ("osc1_type", "Osc 1: Type", { "Sine", "Triangle", "Square", "Sawtooth", "Push Square", "Better Sawtooth" }, 0),
                     makeFloat ("osc1_gain", "Osc 1: Gain", 0.0f, 1.0f, 0.5f),
                     makeInt ("osc1_octave", "Osc 1: Octave", -2, 2, 0),
                     makeInt ("osc1_cents", "Osc 1: Cents", -100, 100, 0),
                     makeSkewedFloat ("osc1_push", "Osc 1: Push", 1.0f, 100.0f, 0.33f, 1.0f),

                     // Oscillator 2 Parameters
                     makeChoice ("osc2_type", "Osc 2: Type", { "Sine", "Triangle", "Square", "Sawtooth", "Push Square", "Better Sawtooth" }, 0),
                     makeFloat ("osc2_gain", "Osc 2: Gain", 0.0f, 1.0f, 0.5f),
                     makeInt ("osc2_octave", "Osc 2: Octave", -2, 2, 0),
                     makeInt ("osc2_cents", "Osc 2: Cents", -100, 100, 0),
                     makeSkewedFloat ("osc2_push", "Osc 2: Push", 1.0f, 100.0f, 0.33f, 1.0f),

                     // Noise Generator Parameters
                     makeBool ("noise_on", "Noise: On", false),
                     makeFloat ("noise_gain", "Noise: Gain", 0.0f, 1.0f, 0.0f),
                     makeFloat ("noise_filter", "Noise: Filter", 0.0f, 1.0f, 1.0f),
                     makeSkewedFloat ("noise_duration", "Noise: Duration", 0.0f, 100.0f, 0.25f, 1.0f),

                     // LFO Parameters
                     makeBool ("lfo_on", "LFO: On", false),
                     makeChoice ("lfo_type", "LFO: Type", { "Sine", "Triangle", "Square", "Sawtooth", "Inverted Sawtooth" }, 0),
                     makeChoice ("lfo_applies_to", "LFO: Applies To", { "Osc 1 Frequency", "Osc 1 Cents", "Osc 2 Frequency", "Osc 2 Cents", "Osc 1&2 Frequency", "Osc 1&2 Cents", "Filter Frequency", "Filter Q", "Amp Volume", "Amp Distortion" }, 0),
                     makeSkewedFloat ("lfo_frequency", "LFO: Frequency", 0.1f, 20.0f, 0.33f, 1.0f),
                     makeFloat ("lfo_depth", "LFO: Depth", 0.0f, 1.0f, 0.5f),

                     // Filter Parameters
                     makeBool ("filter_on", "Filter: On", true),
                     makeChoice ("filter_type", "Filter: Type", { "Low pass", "High pass" }, 0),
                     makeChoice ("filter_applies_to", "Filter: Applies To", { "Frequency", "Q" }, 0),
                     makeSkewedFloat ("filter_freq", "Filter: Frequency", 20.0f, 20000.0f, 0.25f, 20000.0f),
                     makeSkewedFloat ("filter_q", "Filter: Q", 1.0f, 100.0f, 0.33f, 1.0f),
                     makeFloat ("filter_attack", "Filter: Attack", 0.0f, 1.0f, 0.1f),
                     makeFloat ("filter_decay", "Filter: Decay", 0.0f, 1.0f, 0.33f),
                     makeFloat ("filter_sustain", "Filter: Sustain", 0.0f, 1.0f, 0.5f),
                     makeFloat ("filter_release", "Filter: Release", 0.0f, 1.0f, 0.1f),

                     // Amp Envelope and Distortion Parameters
                     makeFloat ("amp_env_attack", "Amp: Envelope Attack", 0.001f, 1.0f, 0.1f),
                     makeFloat ("amp_env_decay", "Amp: Envelope Decay", 0.0f, 1.0f, 0.33f),
                     makeFloat ("amp_env_sustain", "Amp: Envelope Sustain", 0.0f, 1.0f, 0.5f),
                     makeFloat ("amp_env_release", "Amp: Envelope Release", 0.0f, 1.0f, 0.1f),
                     makeBool ("amp_dist_on", "Amp: Distortion On", false),
                     makeSkewedFloat ("amp_dist_gain", "Amp: Distortion Gain", 1.0f, 100.0f, 0.4f, 1.0f),
                     makeSkewedFloat ("amp_volume", "Amp: Volume", 0.0f, 1.0f, 0.25f, 0.1f),

                     // Delay Parameters
                     makeBool ("delay_on", "Delay: On", false),
                     makeChoice ("delay_type", "Delay: Type", { "Normal", "Ping Pong" }, 1),
                     makeFloat ("delay_delay_time", "Delay: Delay Time (s)", 0.0f, 2.0f, 0.5f),
                     makeFloat ("delay_wet_level", "Delay: Wet Level", 0.0f, 1.0f, 0.0f),
                     makeFloat ("delay_dry_level", "Delay: Dry Level", 0.0f, 1.0f, 0.4f),
                     makeFloat ("delay_feedback", "Delay: Feedback", 0.0f, 1.0f, 0.0f),
                     makeFloat ("delay_depth", "Delay: Depth", 0.5f, 1.0f, 1.0f),

                     // Reverb Parameters
                     makeBool ("reverb_on", "Reverb: On", false),
                     makeFloat ("reverb_room_size", "Reverb: Room Size", 0.0f, 1.0f, 0.5f),
                     makeFloat ("reverb_damping", "Reverb: Damping", 0.0f, 1.0f, 0.5f),
                     makeFloat ("reverb_wet_level", "Reverb: Wet Level", 0.0f, 1.0f, 0.33f),
                     makeFloat ("reverb_dry_level", "Reverb: Dry Level", 0.0f, 1.0f, 0.4f),
                     makeFloat ("reverb_width", "Reverb: Width", 0.0f, 1.0f, 1.0f) }),

          // Oscillator 1 Parameters
          osc1Type (getChoice ("osc1_type")),
          osc1Gain (getFloat ("osc1_gain")),
          osc1Octave (getFloat ("osc1_octave")),
          osc1Cents (getFloat ("osc1_cents")),
          osc1Push (getFloat ("osc1_push")),

          // Oscillator 2 Parameters
          osc2Type (getChoice ("osc2_type")),
          osc2Gain (getFloat ("osc2_gain")),
          osc2Octave (getFloat ("osc2_octave")),
          osc2Cents (getFloat ("osc2_cents")),
          osc2Push (getFloat ("osc2_push")),

          // Noise Generator Parameters
          noiseOn (getBool ("noise_on")),
          noiseGain (getFloat ("noise_gain")),
          noiseFilter (getFloat ("noise_filter")),
          noiseDuration (getFloat ("noise_duration")),

          // LFO Parameters
          lfoOn (getBool ("lfo_on")),
          lfoType (getChoice ("lfo_type")),
          lfoAppliesTo (getChoice ("lfo_applies_to")),
          lfoFrequency (getFloat ("lfo_frequency")),
          lfoDepth (getFloat ("lfo_depth")),

          // Filter Parameters
          filterOn (getBool ("filter_on")),
          filterType (getChoice ("filter_type")),
          filterAppliesTo (getChoice ("filter_applies_to")),
          filterFreq (getFloat ("filter_freq")),
          filterQ (getFloat ("filter_q")),
          filterAttack (getFloat ("filter_attack")),
          filterDecay (getFloat ("filter_decay")),
          filterSustain (getFloat ("filter_sustain")),
          filterRelease (getFloat ("filter_release")),

          // Amp Envelope and Distortion Parameters
          ampEnvAttack (getFloat ("amp_env_attack")),
          ampEnvDecay (getFloat ("amp_env_decay")),
          ampEnvSustain (getFloat ("amp_env_sustain")),
          ampEnvRelease (getFloat ("amp_env_release")),
          ampDistOn (getBool ("amp_dist_on")),
          ampDistGain (getFloat ("amp_dist_gain")),
          ampVolume (getFloat ("amp_volume")),

          // Delay Parameters
          delayOn (getBool ("delay_on")),
          delayType (getChoice ("delay_type")),
          delayTime (getFloat ("delay_delay_time")),
          delayWetLevel (getFloat ("delay_wet_level")),
          delayDryLevel (getFloat ("delay_dry_level")),
          delayFeedback (getFloat ("delay_feedback")),
          delayDepth (getFloat ("delay_depth")),

          // Reverb Parameters
          reverbOn (getBool ("reverb_on")),
          reverbRoomSize (getFloat ("reverb_room_size")),
          reverbDamping (getFloat ("reverb_damping")),
          reverbWetLevel (getFloat ("reverb_wet_level")),
          reverbDryLevel (getFloat ("reverb_dry_level")),
          reverbWidth (getFloat ("reverb_width"))
    {
        // empty
    }
    
private:
    /**
     Helper to get a float parameter back from the Audio Processor Value Tree State.
     
     @param paramId The ID for the parameter
     */
    atomic<float>* getFloat (string paramId)
    {
        return apvts.getRawParameterValue (paramId);
    }

    /**
     Helper to get a choice parameter back from the Audio Processor Value Tree State.
     
     @param paramId The ID for the parameter
     */
    juce::AudioParameterChoice* getChoice (string paramId)
    {
        return dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramId));
    }

    /**
     Helper to get a boolean parameter back from the Audio Processor Value Tree State.
     
     @param paramId The ID for the parameter
     */
    juce::AudioParameterBool* getBool (string paramId)
    {
        return dynamic_cast<juce::AudioParameterBool*> (apvts.getParameter (paramId));
    }
};

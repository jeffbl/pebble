/*
The MIT License (MIT)

Copyright (c) 2014 Jeffrey R. Blum

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <pebble.h>

/*
 * the total length of each PWM pulse (entire on/off cycle) in milliseconds DO NOT CHANGE THIS
 */
#define MIN_PWM_PULSE_PERIOD           10

/*
 * increasing extends the pulse period, making it more "choppy" but allowing longer more complicated patterns
 */
#define PWM_PULSE_PERIOD_MULTIPLIER    1
#define PWM_PULSE_PERIOD               MIN_PWM_PULSE_PERIOD*PWM_PULSE_PERIOD_MULTIPLIER

/*
 * max number of vibration pulses * 2 AFTER it is broken into PWM.
 * always uses this number of bytes (inefficient...)
 * (e.g. if you have a long pattern, it may not play completely
 * in order to avoid going past end of this fixed-size buffer.
 * Currently set to allow at least 5 seconds of total vibration)
 *
 * NOTE: If the total vibration time exceeds the pebble 10 second max
 *       it will just truncate silently
 */
#define MAX_PATTERN_SIZE 512*2


/*
 * Pebble VibePatternPWM structure, essentially like a normal VibePattern
 * but the durations is not declared const, so that helper functions are simplified.
 *
 * Instead of alternating on and off,
 * the "durations" field of the struct instead contains duration/intensity alternating pairs.
 * Intensity ranges from 0-10, with 10 full intensity, and 0 no vibration
 * For example, durations [100, 5, 200, 7, 300, 0, 150, 2] would output:
 * 	100ms pulse at intensity 5 (half)
 * 	200ms pulse at intensity 7 (70% of full intensity)
 * 	300ms pause
 * 	150ms pulse at intensity 2 (very low)
 *
 * NOTE: durations are floored (rounded down) to 10ms increments.
 */
typedef struct {
  uint32_t *durations;
  uint32_t num_segments;
} VibePatternPWM;

uint64_t ms_since_epoch();
bool vibes_isVibing();

/*
 *  Enqueues a VibePatternPWM, breaking the durations down into
 *  individual PWM pulses and sending them through Pebble's
 *  normal vibes_enqueue_custom_pattern function
 *
 *  Return value == true :
 *    maximum pattern size was reached or exceeded
 *    (pattern may have been truncated...)
 *
 *  Note this simply calls two functions one after the other:
 *    vibes_prepare_custom_pwm_pattern
 *    vibes_play_current_custom_pwm_pattern
 */
bool vibes_enqueue_custom_pwm_pattern(VibePatternPWM *pwmPat);


/*
 *  Prepare a VibePatternPWM for playback, but don't actually play it
 *  Return value is the same as for vibes_enqueue_custom_pwm_pattern
 */
bool vibes_prepare_custom_pwm_pattern(VibePatternPWM *pwmPat);

/*
 *  Replay the last prepared or played pattern
 *  Useful since it will not re-parse the pattern to make it into
 *  a native call, making it start faster
 */
void vibes_play_current_custom_pwm_pattern();

/*
 * Helper function that takes a VibePatternPWM and appends a duration/force tuple.
 */
VibePatternPWM * vibesPatternPWM_addpulse(VibePatternPWM *pat, uint32_t duration, uint32_t force);

/*
 * Returns an int with number of milliseconds the entire pattern will take, including 0 intensity (pause) pulses
 */
int vibesPatternPWM_get_duration(VibePatternPWM *pat);

/*
 * Clear the pattern so it can be built again using addpulse
 */
VibePatternPWM * vibesPatternPWM_clear(VibePatternPWM *pat);

/*
 * Fills passed buffer with string containing vibe pattern
 * Useful for debugging
 * If bufSize is not large enough to contain everything, it terminates the string with "!"
 */
char * pwmPat_asStr(VibePatternPWM *pat, char *buf, uint32_t bufSize);

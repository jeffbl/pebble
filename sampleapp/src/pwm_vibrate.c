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

#include "pwm_vibrate.h"

#define PWM_PULSE_PERIOD MIN_PWM_PULSE_PERIOD * PWM_PULSE_PERIOD_MULTIPLIER //the actual pulse period in ms

uint32_t pat_pulses[MAX_PATTERN_SIZE]; //todo:  dynamically calculate max to be more space efficient

/*
 * Takes a VibePattern with the duration in the pwm format and adds on new pulses for the passed duration and intensity
 */
static VibePattern * pwm_vibrate_add_pulse(VibePattern *pat, int duration, int intensity) {
	intensity *= PWM_PULSE_PERIOD_MULTIPLIER;		// adjust by multiplier if it is not using the minimum intensity
	int pulses = (duration/PWM_PULSE_PERIOD);		// one pulse == one on/off cycle
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "pwmVibrate intensity %d duration %d pulses %d", intensity, duration, pulses);
	
	if(pat->num_segments > MAX_PATTERN_SIZE-2) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "pwmVibrate pattern is full - ignoring adding more pulses");
		return pat;
	}

	if(intensity == 0) { // pause - we don't have to PWM that!
		pat_pulses[pat->num_segments++] = 0;
		pat_pulses[pat->num_segments++] = duration;
	}
	else if (intensity >= 10) { //intensity is at *or over* maximum, don't need to PWM that either
		pat_pulses[pat->num_segments++] = duration;
		pat_pulses[pat->num_segments++] = 0;
	}
	else{ //have to actually use PWM to fulfil the request
		for(int i=0; i<pulses && pat->num_segments<MAX_PATTERN_SIZE-2; i++) {
			pat_pulses[pat->num_segments++] = intensity;
			pat_pulses[pat->num_segments++] = 10-intensity;
		}
	}

	return pat;
}

/*
 * Sends PWM pattern to standard Pebble SDK vibes_enqueue_custom_pattern
 */
void vibes_enqueue_custom_pwm_pattern(VibePatternPWM *pwmPat) {
	VibePattern pat;
	
	pat.num_segments=0; //clear the pattern to start adding segments
	for(unsigned int i=0; i<pwmPat->num_segments; i+=2) {
		pwm_vibrate_add_pulse(&pat, pwmPat->durations[i], pwmPat->durations[i+1]);
	}
	pat.durations=pat_pulses;
	vibes_enqueue_custom_pattern(pat);
}

/*
 * Helper function to add another pulse to a VibePatternPWM structure
 */
VibePatternPWM * vibesPatternPWM_addpulse(VibePatternPWM *pat, uint32_t duration, uint32_t force) {
	pat->durations[pat->num_segments++] = duration;
	pat->durations[pat->num_segments++] = force;
	return pat;
}
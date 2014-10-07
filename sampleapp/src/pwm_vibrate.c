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

#define PWM_PULSE_PERIOD       MIN_PWM_PULSE_PERIOD * PWM_PULSE_PERIOD_MULTIPLIER //the actual pulse period in ms

/*
 * Allocate one global buffer for the long pwm pattern to be handed to native API.
 * This is ugly.
 * TODO:  dynamically calculate max to be more space efficient
 */
static uint32_t pat_pulses[MAX_PATTERN_SIZE];

/*
 * Takes a VibePattern with the duration in the pwm format and appends on new pulses for the passed duration and intensity,
 * converting it from the PWM vibe format to a long series of little pulses that can be sent to the native vibes_enqueue_custom_pattern
 * Internal function - not exposed to API users
 */
static bool append_pulse(VibePattern *pat, int duration, int intensity) {
   intensity *= PWM_PULSE_PERIOD_MULTIPLIER;      // adjust by multiplier if it is not using the minimum intensity
   int pulses = (duration/PWM_PULSE_PERIOD);      // one pulse == one on/off cycle
   //APP_LOG(APP_LOG_LEVEL_DEBUG, "pwmVibrate intensity %d duration %d pulses %d", intensity, duration, pulses);
   
   //only add pulse if the duration is non-zero and there is still room in the pattern
   if(duration != 0 && pat->num_segments <= MAX_PATTERN_SIZE-2 ) {
      if(intensity == 0) { // pause - we don't have to PWM that!
         pat_pulses[pat->num_segments++] = 0;
         pat_pulses[pat->num_segments++] = duration;
      }
      else if (intensity >= 10) { //intensity is at *or over* maximum, don't need to PWM that either
         pat_pulses[pat->num_segments++] = duration;
         pat_pulses[pat->num_segments++] = 0;
      }
      else{ //have to actually use PWM to fulfil the request
         int offtime=10-intensity;
         for(int i=0; i<pulses && pat->num_segments <= MAX_PATTERN_SIZE-2; i++) {
            pat_pulses[pat->num_segments++] = intensity;
            pat_pulses[pat->num_segments++] = offtime;
         }
      }
   }
   
   //return whether there is still room in the pattern
   if(pat->num_segments == MAX_PATTERN_SIZE) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "pwmVibrate pattern is full - ignoring adding more pulses");
      return true; //ALL FULL!
   }
   else {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "pwmVibrate pattern has room: %lu", MAX_PATTERN_SIZE-pat->num_segments);
      return false; //there is still room for more
   }
}

/*
 * Sends PWM pattern to standard Pebble SDK vibes_enqueue_custom_pattern
 */
bool vibes_enqueue_custom_pwm_pattern(VibePatternPWM *pwmPat) {
   bool isFull=false;

   VibePattern pat;
   pat.num_segments=0; //clear the pattern to start adding segments
   pat.durations=pat_pulses; //hack since this is not writable in a VibePattern
   
   for(unsigned int i=0; i<pwmPat->num_segments; i+=2) {
      isFull = append_pulse(&pat, pwmPat->durations[i], pwmPat->durations[i+1]);
      if(isFull) break; //no sense trying to add more if it is full...
   }
   
   vibes_enqueue_custom_pattern(pat);
   
   return isFull;
}

/*
 * Helper function to add another pulse to a VibePatternPWM structure
 */
VibePatternPWM * vibesPatternPWM_addpulse(VibePatternPWM *pat, uint32_t duration, uint32_t force) {
   pat->durations[pat->num_segments++] = duration;
   pat->durations[pat->num_segments++] = force;
   return pat;
}

/*
 *  Fills passed buffer with string containing vibe pattern
 */
char * pwmPat_asStr(VibePatternPWM *pat, char *buf, uint32_t bufSize) {
   strcpy(buf, "");
   for(unsigned int i=0; i<pat->num_segments; i++) {         
      char numStr[16];
      snprintf(numStr, 16, " %d", (unsigned int)pat->durations[i]);
      if(i%2==1) {
         strcat(numStr, " |");
      }

      uint32_t slen = strlen(buf);
      if(slen + strlen(numStr) < bufSize) {
         strcat(buf, numStr);
      }
      else if(slen < bufSize-2) {
         strcat(buf, "!"); //add exclamation point to indicate truncated text...
         break;
      }
      else { //not enough room to even add the exclamation point, so truncate previous entry (not ideal)
         buf[bufSize-2] = 0;
         strcat(buf, "!");
      }
   }
   return buf;
}

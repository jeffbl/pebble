pebble
======

Pebble smartwatch utility code

pwm_vibrate:
Lets app control intensity of pebble vibrator in 10 increments

NOTE: I do not have any Pebble devices except the original Pebble, so this code has not been tested on any later watches.

WARNING: Long patterns will reset the Pebble (started around version 2.7, still existed as of 3.8.2). This is a known bug they are fixing (hopefully soon).
Once fixed, the max duration will be returned to 10 seconds in the library.
Note that the Pebble may still reset even with short patterns (I have not tested enough to put a bound on it).
See http://forums.getpebble.com/discussion/comment/122761

revision history
----------------
### 12APR2016  v1.3
* Added call to get the total duration of a PWM pattern in milliseconds, including any zero intensity pulses (pauses). Due to a Pebble firmware bug with did_vibrate (https://forums.getpebble.com/discussion/31867/bug-report-did-vibrate-gets-stuck-to-true-until-watch-reboot), this is useful for writing code to ignore accelerometer samples if the motor is vibrating.

### 15OCT2014  v1.2
* Further reduced maximum vibe pattern duration to around 3 seconds due to watch reset in native pebble vibe API with longer durations
* New vibes_prepare_custom_pwm_pattern function that creates the PWM pattern, but does not play it.
This is useful, for example, to get better control over the delay between triggering the vibration and when the motor actually starts.
The pattern can be prepared in advance, then triggered more quickly when it actually needs to be played.
* New vibes_play_current_custom_pwm_pattern function that plays the most recently prepared pattern, or repeats the previous vibration

### 06OCT2014  v1.1
* Reduced maximum size of PWM structure since long patterns were causing native pebble vibes_enqueue_custom_pattern() to reset the watch
* Added return value to indicate whether requested pwm vibration pattern was truncated due to running out of space
* Improved debug logging
* Improved sample app demonstrating features - ramps up or down, better initial instruction screen
* Added function to library that creates formatted string of the pwmPattern for debugging 
* Updated sample app config file to work with Pebble SDK 2.6


### 21MAY2014  v1.0
* First real public release with MIT licence, decent sample app

### 07MAR2014
* Initial commit 

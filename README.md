pebble
======

Pebble smartwatch utility code

pwm_vibrate:
Lets app control intensity of pebble vibrator in 10 increments


revision history
----------------

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
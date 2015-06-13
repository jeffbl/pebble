#include <pebble.h>
#include "pwm_vibrate.h"

static Window *window;
static TextLayer *text_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
   text_layer_set_text(text_layer, "Wobble up");
  
   uint32_t segments[40];
   VibePatternPWM pwmPat = {
      .durations = segments,
      .num_segments = 0, //pulseCount*2,
   };
	
   // demonstrate use of vibesPatternPWM_addpulse
   vibesPatternPWM_addpulse(&pwmPat, 300, 1);
   vibesPatternPWM_addpulse(&pwmPat, 300, 6);
   vibesPatternPWM_addpulse(&pwmPat, 300, 2);
   vibesPatternPWM_addpulse(&pwmPat, 300, 7);
   vibesPatternPWM_addpulse(&pwmPat, 300, 3);
   vibesPatternPWM_addpulse(&pwmPat, 300, 8);
   vibesPatternPWM_addpulse(&pwmPat, 300, 4);
   vibesPatternPWM_addpulse(&pwmPat, 300, 9);
   vibesPatternPWM_addpulse(&pwmPat, 300, 5);
   vibesPatternPWM_addpulse(&pwmPat, 300, 10);

   vibes_enqueue_custom_pwm_pattern(&pwmPat);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Ramp up");

   // demonstrate a straightforward vibration ramp
   static uint32_t segments[] = { 300, 1, 300, 2, 300, 3, 300, 4, 300, 5, 300, 6, 300, 7, 300, 8, 300, 9, 300, 10 };
   //static uint32_t segments[] = { 100, 2, 1000, 0, 100, 10, 1000, 0, 100, 2, 1000, 0, 100, 10, 1000, 0, 100, 2 };
   VibePatternPWM pwmPat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
   };
   
   //Could just call vibes_enqueue_custom_pwm_pattern,
   //but will instead do it in two steps - prepare and play:
   vibes_prepare_custom_pwm_pattern(&pwmPat); //Note this will not cause it to actually vibrate!
   vibes_play_current_custom_pwm_pattern(); //Now it actually plays the pattern that was "prepared"
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
   text_layer_set_text(text_layer, "Ramp down");
	
   // demonstrate a straightforward vibration ramp
   static uint32_t segments[] = { 300, 10, 300, 9, 300, 8, 300, 7, 300, 6, 300, 5, 300, 4, 300, 3, 300, 2, 300*10, 1 }; //note very long final pulse
   VibePatternPWM pwmPat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
   };
   
   bool isFull = vibes_enqueue_custom_pwm_pattern(&pwmPat);
   if(isFull) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Note: Due to the very long final pulse, it will not render the full duration");
   }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 40 }, .size = { bounds.size.w, 80 } });
  text_layer_set_text(text_layer, "Up: ramp up\nSelect: wobble\nDown: ramp down");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

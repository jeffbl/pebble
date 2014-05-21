#include <pebble.h>
#include "pwm_vibrate.h"

static Window *window;
static TextLayer *text_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
   text_layer_set_text(text_layer, "Ramp");
  
   // demonstrate a very straightforward vibration ramp
   static uint32_t segments[] = { 500, 1, 500, 2, 500, 3, 500, 4, 500, 5, 500, 6, 500, 7, 500, 8, 500, 9, 500, 10 };
   VibePatternPWM pwmPat = {
      .durations = segments,
      .num_segments = ARRAY_LENGTH(segments),
   };
   vibes_enqueue_custom_pwm_pattern(&pwmPat);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
   text_layer_set_text(text_layer, "Wobble");
	
   uint32_t segments[40];
   VibePatternPWM pwmPat = {
      .durations = segments,
      .num_segments = 0, //pulseCount*2,
   };
	
   // demonstrate use of vibesPatternPWM_addpulse
   vibesPatternPWM_addpulse(&pwmPat, 300, 1);
   vibesPatternPWM_addpulse(&pwmPat, 300, 5);
   vibesPatternPWM_addpulse(&pwmPat, 300, 2);
   vibesPatternPWM_addpulse(&pwmPat, 300, 6);
   vibesPatternPWM_addpulse(&pwmPat, 300, 3);
   vibesPatternPWM_addpulse(&pwmPat, 300, 7);
   vibesPatternPWM_addpulse(&pwmPat, 300, 4);
   vibesPatternPWM_addpulse(&pwmPat, 300, 8);
   vibesPatternPWM_addpulse(&pwmPat, 300, 5);
   vibesPatternPWM_addpulse(&pwmPat, 300, 9);
   vibesPatternPWM_addpulse(&pwmPat, 300, 6);
   vibesPatternPWM_addpulse(&pwmPat, 300, 10);

	vibes_enqueue_custom_pwm_pattern(&pwmPat);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
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

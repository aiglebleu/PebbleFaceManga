#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_hello_layer;
static TextLayer *s_sig_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;

static BitmapLayer *s_white_background;
static BitmapLayer *s_background_layer2;
static GBitmap *s_background_bitmap2;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char s_buffer2[30];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  strftime(s_buffer2, sizeof(s_buffer2), "%A %d %B", tick_time);
  text_layer_set_text(s_date_layer, s_buffer2);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  /* Get information about the Window */
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  /* Create GBitmap from manga image */
  s_white_background = bitmap_layer_create(
    GRect(0, bounds.size.h-110, bounds.size.w, bounds.size.h));
  bitmap_layer_set_background_color(s_white_background, GColorWhite);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_white_background));
  
  s_background_bitmap2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MANGA); //create image
  s_background_layer2 = bitmap_layer_create( 
    GRect(0, bounds.size.h-140, bounds.size.w, bounds.size.h)); //create layer
  bitmap_layer_set_bitmap(s_background_layer2, s_background_bitmap2); //put image in layer
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer2)); //put layer on screen

  /* Create the TextLayer with specific bounds */
  // Title saying "hello"
  s_hello_layer = text_layer_create(
      GRect(0, 5, bounds.size.w, 20)); 
  text_layer_set_background_color(s_hello_layer, GColorClear);
  text_layer_set_text_color(s_hello_layer, GColorWhite);
  text_layer_set_text(s_hello_layer, "~Welcome back~");
  text_layer_set_font(s_hello_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_hello_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_hello_layer));

  // Signature from rukyou
  s_sig_layer = text_layer_create(
      GRect(10, 25, bounds.size.w - 20, 20));
  text_layer_set_background_color(s_sig_layer, GColorClear);
  text_layer_set_text_color(s_sig_layer, GColorWhite);
  text_layer_set_text(s_sig_layer, "-Rukyou-Chan");
  text_layer_set_font(s_sig_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_sig_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_sig_layer));

  // Text giving time (current hour)
  s_time_layer = text_layer_create(
      GRect(0, bounds.size.h - 65, bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Text giving date (current day and month)
  s_date_layer = text_layer_create(
      GRect(0, bounds.size.h-20, bounds.size.w, 20));
  text_layer_set_background_color(s_date_layer, GColorWhite);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "Date");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_sig_layer);
  text_layer_destroy(s_hello_layer);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap2);
  
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer2);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  window_set_background_color(s_main_window, GColorBlack);


  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
#include <pebble.h>

static Window *s_window;
static TextLayer *s_hour_layer, *s_time_layer;
static GFont s_date_font, s_date_font2, s_date_font3;
static char s_num_buffer[8];
static BitmapLayer *s_background_layer;
static GBitmap *s_background_map;



static void update_time() {
  // Structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Buffer Day
  static char s_day_buffer[6];
  strftime(s_day_buffer, sizeof(s_day_buffer), "%d", tick_time);
  static char s_time_buffer[8];
  strftime(s_time_buffer, sizeof(s_time_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

           
  // Show the hour
  text_layer_set_text(s_hour_layer, s_day_buffer);
  // Show the time
  text_layer_set_text(s_time_layer, s_time_buffer);
  
}


static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
  
  update_time();
}

static void update_timer_callback(void *data) {
  layer_set_hidden((Layer *)s_time_layer, true);
  layer_set_hidden((Layer *)s_hour_layer, false);
  layer_set_hidden((Layer *)s_background_layer, false);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  
  layer_set_hidden((Layer *)s_time_layer, false);
  layer_set_hidden((Layer *)s_hour_layer, true);
  layer_set_hidden((Layer *)s_background_layer, true);
  
  AppTimer *updateTimer = app_timer_register(3000, (AppTimerCallback) update_timer_callback, NULL);
	}

static void window_load(Window *s_window) {
  
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(s_window, GColorWhite);
  
  
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_font62));
  s_date_font3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_font50));
  s_date_font2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_font30));
  
  
  //Background
  s_background_map = gbitmap_create_with_resource(RESOURCE_ID_hexa);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_map);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  
  //DATE
  s_hour_layer = text_layer_create PBL_IF_ROUND_ELSE((GRect(45, 140, 90, 130)), (GRect(0,130,144,168)));
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorOxfordBlue);
  PBL_IF_ROUND_ELSE((text_layer_set_font(s_hour_layer, s_date_font2)), (text_layer_set_font(s_hour_layer, s_date_font2)));
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_hour_layer));
  layer_set_hidden((Layer *)s_hour_layer, false);
  

  //Time
  s_time_layer = text_layer_create PBL_IF_ROUND_ELSE((GRect(0, 45, 180, 90)), (GRect(0,49,144,70)));
  text_layer_set_background_color(s_time_layer, GColorOxfordBlue);
  text_layer_set_text_color(s_time_layer, GColorFashionMagenta);
  PBL_IF_ROUND_ELSE((text_layer_set_font(s_time_layer, s_date_font)), (text_layer_set_font(s_time_layer, s_date_font3)));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_time_layer));
  layer_set_hidden((Layer *)s_time_layer, true);
  
}


static void window_unload(Window *window) {
  
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_time_layer);
  bitmap_layer_destroy(s_background_layer);
  gbitmap_destroy(s_background_map);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_date_font2);
  fonts_unload_custom_font(s_date_font3);
  window_destroy(s_window);
  
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  s_num_buffer[0] = '\0';


  accel_tap_service_subscribe(tap_handler);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  
  
}

static void deinit() {


  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
}


  
  



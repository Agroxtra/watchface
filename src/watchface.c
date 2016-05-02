#include <pebble.h>

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_con_layer;
static TextLayer *s_date_layer;
static TextLayer *s_time_until_layer;
//static TextLayer *s_seconds_layer;
static GFont s_time_font;
static GFont s_weather_font;
static GFont s_con_font;
static GFont s_date_font;
static GFont s_time_until_font;
//static GFont s_seconds_font;
static int seconds = 0;
static GColor secondsColor;



static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);

  // If all data is available, use it
  //if(temp_tuple && conditions_tuple) {
  snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", (int)temp_tuple->value->int32);
  snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
  
    //APP_LOG(APP_LOG_LEVEL_ERROR, conditions_buffer);
  //}
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static int calcLen1(){
  int countPixels = seconds * 10.4;
  if (countPixels >= 72){
    return 72;
  }
  else {
    return countPixels;
  }
}

static int calcLen2(){
  if (calcLen1() == 72){
    int countPixels = seconds * 10.4 - 72;
    if (countPixels >= 168){
      return 168;
    }
    else {
      return countPixels;
    }
  }
  return 0;
}

static int calcLen3(){
  if (calcLen2() == 168){
    int countPixels = seconds * 10.4 - 72 - 168;
    if (countPixels >= 144){
      return 144;
    }
    return countPixels;
  }
  return 0;
}

static int calcLen4(){
  if (calcLen3() == 144){
    int countPixels = seconds * 10.4 - 72 - 168 - 144;
    if (countPixels >= 168){
      return 168;
    }
    return countPixels;
  }
  return 0;
}

static int calcLen5(){
  if (calcLen4() == 168){
    int countPixels = seconds *10.4 - 72 - 168*2 - 144;
    if (countPixels >= 72){
      return 72;
    }
    return countPixels;
  }
  return 0;
}

static void update_display(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, secondsColor);
  
  int len1 = calcLen1();
  int len2 = calcLen2();
  int len3 = calcLen3();
  int len4 = calcLen4();
  int len5 = calcLen5();
  
  
  //len1 upper right streak 
  graphics_fill_rect(ctx, GRect(72, 0, len1, 3), 0, 0);
  //len2 right streak
  graphics_fill_rect(ctx, GRect(144 - 3, 0, 3, len2), 0, 0);
  //len3 lower streak
  graphics_fill_rect(ctx, GRect(144 - len3, 168 - 3, len3, 3), 0, 0);
  //len4 left streak
  graphics_fill_rect(ctx, GRect(0, 168 - len4, 3, len4), 0, 0);
  //len5 upper left streak
  graphics_fill_rect(ctx, GRect(0, 0, len5, 3), 0, 0);
  

  //APP_LOG(APP_LOG_LEVEL_INFO, "test");
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[10];
  static char s_date_buffer[12];
//  static char s_buffer_seconds[3];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  strftime(s_date_buffer, sizeof(s_date_buffer),  "%d. %b", tick_time);
//  strftime(s_buffer_seconds, sizeof(s_buffer_seconds), clock_is_24h_style() ? "%S":"%S", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void battery_handler(BatteryChargeState charge_state) {
  if (charge_state.is_charging){
    secondsColor = GColorBlue;
  }
  else {
    if (charge_state.charge_percent > 50){
      secondsColor = GColorGreen;
    }
    else if (charge_state.charge_percent > 20){
      secondsColor = GColorYellow;
    }
    else if (charge_state.charge_percent > 10){
      secondsColor = GColorOrange;
    }
    else {
      secondsColor = GColorRed;
    }
  }
}

static void bluetooth_handler(bool con){
  if (con){
    text_layer_set_text(s_con_layer, "Connected");
    text_layer_set_text_color(s_con_layer, GColorWhite);
  }
  else{
    text_layer_set_text(s_con_layer, "Disconnected");
    text_layer_set_text_color(s_con_layer, GColorRed);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  seconds = tick_time->tm_sec;
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void main_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  secondsColor = GColorRed;
  
  s_weather_font =  fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_20));
  s_time_font =  fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_48));
//  s_seconds_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_18));
  s_con_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_8));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_24));
  s_time_until_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_20));
  
  GRect bounds = layer_get_bounds(window_layer);
  Layer *layer = layer_create(bounds);
  window_set_background_color(window, GColorBlack);
  
//  s_seconds_layer = text_layer_create(GRect(bounds.size.w - 26, PBL_IF_ROUND_ELSE(26, 20), 50, 30));
  s_time_layer = text_layer_create(GRect(0, 7, bounds.size.w, 50));
  s_date_layer = text_layer_create(GRect(0, 67, bounds.size.w, 30));
  s_time_until_layer = text_layer_create(GRect(0, 95, bounds.size.w, 20));
  s_weather_layer = text_layer_create(GRect(0, 120, bounds.size.w, 30));
  s_con_layer = text_layer_create(GRect(0, 150, bounds.size.w, 10));
 
  
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_background_color(s_time_until_layer, GColorClear);
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_background_color(s_con_layer, GColorClear);
//  text_layer_set_background_color(s_seconds_layer, GColorClear);
  
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text_color(s_time_until_layer, GColorWhite);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_color(s_con_layer, GColorWhite);
//  text_layer_set_text_color(s_seconds_layer, GColorWhite);
  
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text(s_time_until_layer, "50");
//  text_layer_set_text(s_seconds_layer, "00");
  
  update_time();
  
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_font(s_time_until_layer, s_time_until_font);
  text_layer_set_font(s_weather_layer, s_weather_font);
  text_layer_set_font(s_con_layer, s_con_font);
//  text_layer_set_font(s_seconds_layer, s_seconds_font);
  
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_time_until_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_con_layer, GTextAlignmentCenter);
//  text_layer_set_text_alignment(s_seconds_layer, GTextAlignmentLeft);
  
  text_layer_set_text(s_weather_layer, "Loading...");
  
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_until_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_con_layer));
  //  layer_add_child(window_layer, text_layer_get_layer(s_seconds_layer));

  layer_set_hidden(layer, false);
  layer_set_hidden(text_layer_get_layer(s_time_until_layer), true);
  layer_add_child(window_layer, layer);

  //Handlers
  layer_set_update_proc(layer, update_display);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_handler
  });
  battery_state_service_subscribe(battery_handler);
  bluetooth_handler(connection_service_peek_pebble_app_connection());
  battery_handler(battery_state_service_peek());
}

static void main_window_unload(Window *window){
  tick_timer_service_unsubscribe();
  connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_con_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_until_layer);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_con_font);
  fonts_unload_custom_font(s_weather_font);
  fonts_unload_custom_font(s_time_until_font);
  //text_layer_destroy(s_seconds_layer);
  //fonts_unload_custom_font(s_seconds_font);

}


static void init(void) {
	s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers){.load = main_window_load, .unload = main_window_unload});
  window_stack_push(s_main_window, true);
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
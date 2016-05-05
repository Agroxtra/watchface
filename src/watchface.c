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


static Layer *l1;
static Layer *l2;
static Layer *l3;
static Layer *l4;
static Layer *l5;
static int len1Old = 0;
static int len2Old = 0;
static int len3Old = 0;
static int len4Old = 0;
static int len5Old = 0;
static Animation *anim1;
static Animation *anim2;
static Animation *anim3;
static Animation *anim4;
static Animation *anim5;



static void time_until_update(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  int hour = tick_time->tm_hour;
  int minute = tick_time->tm_min;
  int diff = 0;
  if ((hour == 7 && minute >= 50) || (hour == 8 && minute <= 40)){ // 1. lesson
    if (hour == 7){
      diff = 60 - minute + 40;
    }
    else {
      diff = (40 - minute);
    }
  }
  else if ((hour == 8 && minute > 40) || (hour == 9 && minute <= 30)){ // 2. lesson
    if (hour == 8){
      diff = 60 - minute + 30;
    }
    else {
      diff = (30 - minute);
    }
  }
  else if (hour == 9 && minute > 30 && minute <= 35){
    diff = (35 - minute);
  }
  else if ((hour == 9 && minute > 35) || (hour == 10 && minute <= 25)){ // 3. lesson
    if (hour == 9){
      diff = 60 - minute + 25;
    }
    else {
      diff = (25 - minute);
    }
  }
  else if (hour == 10 && minute > 25 && minute <= 35){
    if (hour == 10){
      diff = 60 - minute + 35;
    }
    else {
      diff = (35 - minute);
    }
  }
  else if ((hour == 10 && minute > 35) || (hour == 11 && minute <= 25)){ // 4. lesson
    if (hour == 10){
      diff = 60 - minute + 25;
    }
    else {
      diff = (25 - minute);
    }
  }
  else if (hour == 11 && minute > 25 && minute <= 30){
    diff += (30 - minute);
  }
  else if ((hour == 11 && minute > 30) || (hour == 12 && minute <= 20)){ // 5. lesson
    if (hour == 11){
      diff = 60 - minute + 20;
    }
    else {
      diff = (20 - minute);
    }
  }
  else if (hour == 12 && minute > 20 && minute <= 25){
    diff = (25 - minute);
  }
  else if ((hour == 12 && minute > 25) || (hour == 13 && minute <= 15)){ // 6. lesson
    if (hour == 12){
      diff = 60 - minute + 15;
    }
    else {
      diff = (15 - minute);
    }
  }
  else if (hour == 13 && minute > 15 && minute <= 25){
    diff += (25 - minute);
  }
  else if ((hour == 13 && minute > 25) || (hour == 14 && minute <= 15)){ // 7. lesson
    if (hour == 13){
      diff = 60 - minute + 15;
    }
    else {
      diff = (15 - minute);
    }
  }
  else if (hour == 14 && minute > 15 && minute <= 20){
    diff += (20 - minute);
  }
  else if ((hour == 14 && minute > 20) || (hour == 15 && minute <= 10)){ // 8. lesson
    if (hour == 14){
      diff = 60 - minute + 10;
    }
    else {
      diff = (10 - minute);
    }
  }
  else if (hour == 15 && minute > 10 && minute <= 15){
    diff += (15 - minute);
  }
  else if ((hour == 15 && minute > 15) || (hour == 16 && minute <= 5)){
    if (hour == 15){
      diff = 60 - minute + 5;
    }
    else {
      diff = (5 - minute);
    }
  }
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%d", diff);
  text_layer_set_text(s_time_until_layer, buffer);
  //layer_set_hidden(text_layer_get_layer(s_time_until_layer), diff == 0);
//  APP_LOG(APP_LOG_LEVEL_INFO, buffer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);
  
  // If all data is available, use it
  snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", (int)temp_tuple->value->int32);
  snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
  
  
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


static void l1_update(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, secondsColor);
  graphics_fill_rect(ctx, GRect(0,0,168, 168), 0, 0);
}
static void l2_update(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, secondsColor);
  graphics_fill_rect(ctx, GRect(0,0,168, 168), 0, 0);
}
static void l3_update(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, secondsColor);
  graphics_fill_rect(ctx, GRect(0,0,168, 168), 0, 0);
}
static void l4_update(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, secondsColor);
  graphics_fill_rect(ctx, GRect(0,0,168, 168), 0, 0);
}
static void l5_update(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, secondsColor);
  graphics_fill_rect(ctx, GRect(0,0,168, 168), 0, 0);
}

static void a1_stopped(Animation *animation, bool finished, void *context){
  if (finished && calcLen1() == 72){
    animation_schedule(anim2);
  }
}
static void a2_stopped(Animation *animation, bool finished, void *context){
  if (finished && calcLen2() == 168){
    animation_schedule(anim3);
  }
}
static void a3_stopped(Animation *animation, bool finished, void *context){
  if (finished && calcLen3() == 144){
    animation_schedule(anim4);
  }
}
static void a4_stopped(Animation *animation, bool finished, void *context){
  if (finished && calcLen4() == 168){
    animation_schedule(anim5);
  }
}

static void update_seconds(){
  int len1 = calcLen1();
  int len2 = calcLen2();
  int len3 = calcLen3();
  int len4 = calcLen4();
  int len5 = calcLen5();
  GRect start1 = GRect(72,0,len1Old,3);
  GRect end1 = GRect(72, 0, len1, 3);
  GRect start2 = GRect(141,0,3,len2Old);
  GRect end2 = GRect(141, 0, 3,len2);
  GRect start3 = GRect(144-len3Old,168-3,144,3);
  GRect end3 = GRect(144-len3, 168-3, 144, 3);
  GRect start4 = GRect(0,168-len4Old,3, 168);
  GRect end4 = GRect(0,168-len4,3, 168);
  GRect start5 = GRect(0,0,len5Old, 3);
  GRect end5 = GRect(0, 0, len5, 3);
  PropertyAnimation *p_anim1 = property_animation_create_layer_frame(l1, &start1, &end1);
  anim1 = property_animation_get_animation(p_anim1);
  animation_set_duration(anim1, 1040);
  PropertyAnimation *p_anim2 = property_animation_create_layer_frame(l2, &start2, &end2);
  anim2 = property_animation_get_animation(p_anim2);
  animation_set_duration(anim2, 1040);
  PropertyAnimation *p_anim3 = property_animation_create_layer_frame(l3, &start3, &end3);
  anim3 = property_animation_get_animation(p_anim3);
  animation_set_duration(anim3, 1040);
  PropertyAnimation *p_anim4 = property_animation_create_layer_frame(l4, &start4, &end4);
  anim4 = property_animation_get_animation(p_anim4);
  animation_set_duration(anim4, 1040);
  PropertyAnimation *p_anim5 = property_animation_create_layer_frame(l5, &start5, &end5);
  anim5 = property_animation_get_animation(p_anim5);
  animation_set_duration(anim5, 1040);
  
  animation_set_handlers(anim1,(AnimationHandlers) {
    .stopped = a1_stopped
  }, NULL);
  animation_set_handlers(anim2,(AnimationHandlers) {
    .stopped = a2_stopped
  }, NULL);
  animation_set_handlers(anim3,(AnimationHandlers) {
    .stopped = a3_stopped
  }, NULL);
  animation_set_handlers(anim4,(AnimationHandlers) {
    .stopped = a4_stopped
  }, NULL);
  
  animation_schedule(anim1);
  
  len1Old = len1;
  len2Old = len2;
  len3Old = len3;
  len4Old = len4;
  len5Old = len5;
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
  time_until_update();
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
  if (tick_time->tm_sec == 0){
    len1Old = 0;
    len2Old = 0;
    len3Old = 0;
    len4Old = 0;
    len5Old = 0;
    update_time();
  }
  seconds = tick_time->tm_sec;
  update_seconds();
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
  
  
  
  l1 = layer_create(GRect(72,0,0,3));
  l2 = layer_create(GRect(141,0,3,0));
  l3 = layer_create(GRect(0, 168-3, 0, 3));
  l4 = layer_create(GRect(0,0,3,0));
  l5 = layer_create(GRect(0,0,0,3));
  layer_set_update_proc(l1, l1_update);
  layer_set_update_proc(l2, l2_update);
  layer_set_update_proc(l3, l3_update);
  layer_set_update_proc(l4, l4_update);
  layer_set_update_proc(l5, l5_update);
  layer_add_child(layer, l1);
  layer_add_child(layer, l2);
  layer_add_child(layer, l3);
  layer_add_child(layer, l4);
  layer_add_child(layer, l5);
  
  
  
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
  
  //update_time();
  
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
  //layer_set_hidden(text_layer_get_layer(s_time_until_layer), true);
  layer_add_child(window_layer, layer);
  
  //Handlers
  //layer_set_update_proc(layer, update_display);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_handler
  });
  battery_state_service_subscribe(battery_handler);
  bluetooth_handler(connection_service_peek_pebble_app_connection());
  battery_handler(battery_state_service_peek());
  //update_time();
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
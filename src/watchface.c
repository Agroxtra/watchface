#include <pebble.h>

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_HOURS_TO 2
#define KEY_MINUTES_TO 3
#define KEY_SECONDS_STYLE 4
#define POS_X 72
#define POS_Y 165/2
#define DISTANCE 6
#define RADIUS_SECONDS 5

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
static Layer *layer;
static int secondsStyle = 2;
static int hours_to = -1;
static int minutes_to = -1;


static void update_display_style1(Layer *layer, GContext *ctx);
static void update_display_style2(Layer *layer, GContext *ctx);
static void update_style();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);
  Tuple *style_tuple = dict_find(iterator, KEY_SECONDS_STYLE);
  Tuple *hours_to_tuple = dict_find(iterator, KEY_HOURS_TO);
  Tuple *minutes_to_tuple = dict_find(iterator, KEY_MINUTES_TO);

  if ((int)style_tuple->value->int32 != -1){
    //int secondsStyleOld = secondsStyle;
    secondsStyle = (int)style_tuple->value->int32;
    char buffer[15];
    snprintf(buffer, sizeof(buffer), "%d", secondsStyle);
    APP_LOG(APP_LOG_LEVEL_INFO, buffer);
    tick_timer_service_unsubscribe();
    tick_timer_service_subscribe(secondsStyle == 0 ? MINUTE_UNIT : SECOND_UNIT, tick_handler);
    update_style();
    persist_write_int(KEY_SECONDS_STYLE, secondsStyle);
  }

  if ((int)hours_to_tuple->value->int32 != -1 && (int)minutes_to_tuple->value->int32 != -1){
    hours_to = (int)hours_to_tuple->value->int32;
    minutes_to = (int)minutes_to_tuple->value->int32;
  }

  if ((int)temp_tuple->value->int32 != -274 && (int)conditions_tuple->value->int32 != -1){
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
  }
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

static void update_time_to(){
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}

static int time_until_update(){
  int result = -1;
  if (hours_to >= 0 && minutes_to >= 0){
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    int temp_hour = tick_time->tm_hour;
    int min = tick_time->tm_min;
    while (temp_hour > 0){
      min += 60;
      temp_hour -= 1;
    }
    temp_hour = hours_to;
    int minTo = minutes_to;
    while (temp_hour > 0){
      minTo += 60;
      temp_hour -= 1;
    }
    char bu[8];
    snprintf(bu, sizeof(bu), "%d", min);
    APP_LOG(APP_LOG_LEVEL_INFO, bu);

    snprintf(bu, sizeof(bu), "%d", minTo);
    APP_LOG(APP_LOG_LEVEL_INFO, bu);
    if (min <= minTo){
      int diff = minTo - min;

      snprintf(bu, sizeof(bu), "%d", diff);
      APP_LOG(APP_LOG_LEVEL_INFO, bu);
      result = diff;

      //text_layer_set_text(s_time_until_layer, bu);
    }
    else {
      update_time_to();
      time_until_update();
    }
  }
  return result;
}

static GPoint calcNew(){
  if (seconds == 0){
    return GPoint(72, DISTANCE);
  }
  else if (seconds == 1){
    return GPoint(80, DISTANCE);
  }
  else if (seconds == 2){
    return GPoint(88, DISTANCE);
  }
  else if (seconds == 3){
    return GPoint(97, DISTANCE);
  }
  else if (seconds == 4){
    return GPoint(106, DISTANCE);
  }
  else if (seconds == 5){
    return GPoint(116, DISTANCE);
  }
  else if (seconds == 6){
    return GPoint(128, DISTANCE);
  }
  else if (seconds == 7){
    return GPoint(144-DISTANCE, 8);
  }
  else if (seconds == 8){
    return GPoint(144-DISTANCE, 21);
  }
  else if (seconds == 9){
    return GPoint(144-DISTANCE, 33);
  }
  else if (seconds == 10){
    return GPoint(144-DISTANCE, 44);
  }
  else if (seconds == 11){
    return GPoint(144-DISTANCE, 54);
  }
  else if (seconds == 12){
    return GPoint(144-DISTANCE, 61);
  }
  else if (seconds == 13){
    return GPoint(144-DISTANCE, 67);
  }
  else if (seconds == 14){
    return GPoint(144-DISTANCE, 74);
  }
  else if (seconds == 15){
    return GPoint(144-DISTANCE, 81);
  }
  else if (seconds == 16){
    return GPoint(144-DISTANCE, 88);
  }
  else if (seconds == 17){
    return GPoint(144-DISTANCE, 96);
  }
  else if (seconds == 18){
    return GPoint(144-DISTANCE, 104);
  }
  else if (seconds == 19){
    return GPoint(144-DISTANCE, 111);
  }
  else if (seconds == 20){
    return GPoint(144-DISTANCE, 120);
  }
  else if (seconds == 21){
    return GPoint(144-DISTANCE, 129);
  }
  else if (seconds == 22){
    return GPoint(144-DISTANCE, 141);
  }
  else if (seconds == 23){
    return GPoint(144-DISTANCE, 156);
  }
  else if (seconds == 24){
    return GPoint(129, 165 - DISTANCE);
  }
  else if (seconds == 25){
    return GPoint(117, 165 - DISTANCE);
  }
  else if (seconds == 26){
    return GPoint(106, 165 - DISTANCE);
  }
  else if (seconds == 27){
    return GPoint(97, 165 - DISTANCE);
  }
  else if (seconds == 28){
    return GPoint(89, 165 - DISTANCE);
  }
  else if (seconds == 29){
    return GPoint(80, 165 - DISTANCE);
  }
  else if (seconds == 30){
    return GPoint(72, 165 - DISTANCE);
  }
  else if (seconds == 31){
    return GPoint(64, 165 - DISTANCE);
  }
  else if (seconds == 32){
    return GPoint(54, 165 - DISTANCE);
  }
  else if (seconds == 33){
    return GPoint(46, 165 - DISTANCE);
  }
  else if (seconds == 34){
    return GPoint(37, 165 - DISTANCE);
  }
  else if (seconds == 35){
    return GPoint(27, 165 - DISTANCE);
  }
  else if (seconds == 36){
    return GPoint(15, 165 - DISTANCE);
  }
  else if (seconds == 37){
    return GPoint(DISTANCE, 153);
  }
  else if (seconds == 38){
    return GPoint(DISTANCE, 140);
  }
  else if (seconds == 39){
    return GPoint(DISTANCE, 128);
  }
  else if (seconds == 40){
    return GPoint(DISTANCE, 118);
  }
  else if (seconds == 41){
    return GPoint(DISTANCE, 109);
  }
  else if (seconds == 42){
    return GPoint(DISTANCE, 101);
  }
  else if (seconds == 43){
    return GPoint(DISTANCE, 92);
  }
  else if (seconds == 44){
    return GPoint(DISTANCE, 86);
  }
  else if (seconds == 45){
    return GPoint(DISTANCE, 80);
  }
  else if (seconds == 46){
    return GPoint(DISTANCE, 73);
  }
  else if (seconds == 47){
    return GPoint(DISTANCE, 66);
  }
  else if (seconds == 48){
    return GPoint(DISTANCE, 58);
  }
  else if (seconds == 49){
    return GPoint(DISTANCE, 50);
  }
  else if (seconds == 50){
    return GPoint(DISTANCE, 42);
  }
  else if (seconds == 51){
    return GPoint(DISTANCE, 32);
  }
  else if (seconds == 52){
    return GPoint(DISTANCE, 20);
  }
  else if (seconds == 53){
    return GPoint(DISTANCE, 6);
  }
  else if (seconds == 54){
    return GPoint(15, DISTANCE);
  }
  else if (seconds == 55){
    return GPoint(26, DISTANCE);
  }
  else if (seconds == 56){
    return GPoint(37, DISTANCE);
  }
  else if (seconds == 57){
    return GPoint(47, DISTANCE);
  }
  else if (seconds == 58){
    return GPoint(55, DISTANCE);
  }
  else if (seconds == 59){
    return GPoint(63, DISTANCE);
  }


  return (GPoint(0,0));
}

static int calcLen1(int seconds){
  int countPixels = seconds * 10.4;
  if (countPixels >= 72){
    return 72;
  }
  else {
    return countPixels;
  }
}

static int calcLen2(int seconds){
  if (calcLen1(seconds) == 72){
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

static int calcLen3(int seconds){
  if (calcLen2(seconds) == 168){
    int countPixels = seconds * 10.4 - 72 - 168;
    if (countPixels >= 144){
      return 144;
    }
    return countPixels;
  }
  return 0;
}

static int calcLen4(int seconds){
  if (calcLen3(seconds) == 144){
    int countPixels = seconds * 10.4 - 72 - 168 - 144;
    if (countPixels >= 168){
      return 168;
    }
    return countPixels;
  }
  return 0;
}

static int calcLen5(int seconds){
  if (calcLen4(seconds) == 168){
    int countPixels = seconds *10.4 - 72 - 168*2 - 144;
    if (countPixels >= 72){
      return 72;
    }
    return countPixels;
  }
  return 0;
}

static void update_display_style2(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, secondsColor);

  int len1 = calcLen1(seconds);
  int len2 = calcLen2(seconds);
  int len3 = calcLen3(seconds);
  int len4 = calcLen4(seconds);
  int len5 = calcLen5(seconds);


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
}

static void update_display_style1(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, secondsColor);
  graphics_fill_circle(ctx, calcNew(), RADIUS_SECONDS);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[10];
  static char s_date_buffer[12];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  strftime(s_date_buffer, sizeof(s_date_buffer),  "%d. %b", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);

  if (tick_time->tm_min == 0 && tick_time->tm_hour == 0){
    update_time_to();
  }

  int minutes = time_until_update();
  static char bu[5];
  if (minutes >= 0 && minutes <= 60){
    snprintf(bu, sizeof(bu), "%d", minutes);
  }
  text_layer_set_text(s_time_until_layer, bu);
  if (minutes == 0){
    vibes_short_pulse();
  }
}

static void update_style(){
  if (secondsStyle == 1){
    layer_set_update_proc(layer, update_display_style1);
  }
  else if (secondsStyle == 2){
    layer_set_update_proc(layer, update_display_style2);
  }
  else if (secondsStyle == 0){
    layer_set_update_proc(layer, NULL);
  }
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
    update_time();
  }
  seconds = tick_time->tm_sec;
  layer_mark_dirty(layer);
  if(tick_time->tm_min % 30 == 0 && seconds == 0) {
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
  s_con_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_8));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_24));
  s_time_until_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_POPPINS_20));

  GRect bounds = layer_get_bounds(window_layer);
  layer = layer_create(bounds);
  window_set_background_color(window, GColorBlack);


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
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text_color(s_time_until_layer, GColorWhite);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_color(s_con_layer, GColorWhite);

  text_layer_set_text(s_time_layer, "00:00");
  //text_layer_set_text(s_time_until_layer, "50");

  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_font(s_time_until_layer, s_time_until_font);
  text_layer_set_font(s_weather_layer, s_weather_font);
  text_layer_set_font(s_con_layer, s_con_font);

  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_time_until_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_con_layer, GTextAlignmentCenter);

  text_layer_set_text(s_weather_layer, "Loading...");

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_time_until_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_con_layer));

  layer_set_hidden(layer, false);
  layer_add_child(window_layer, layer);

  if (persist_read_int(KEY_SECONDS_STYLE)) {
    secondsStyle = persist_read_int(KEY_SECONDS_STYLE);
    char bu[10];
    snprintf(bu, sizeof(bu), "%d", secondsStyle);
    APP_LOG(APP_LOG_LEVEL_INFO, bu);
  }

  update_time_to();
  //Handlers
  update_style();
  tick_timer_service_subscribe(secondsStyle == 0 ? MINUTE_UNIT : SECOND_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_handler
  });
  battery_state_service_subscribe(battery_handler);
  bluetooth_handler(connection_service_peek_pebble_app_connection());
  battery_handler(battery_state_service_peek());
  update_time();
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

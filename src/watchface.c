#include <pebble.h>
#include <math.h>

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_SECONDS_STYLE 2
#define PI 3.1415926535
#define POS_X 72
#define POS_Y 165/2
#define DEG PI/180
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
static Layer *l;
static int secondsStyle = 1;


static void update_display_style1(Layer *layer, GContext *ctx);
static void update_display_style2(Layer *layer, GContext *ctx);
static void update_style();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

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
  //APP_LOG(APP_LOG_LEVEL_INFO, buffer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, KEY_CONDITIONS);
  Tuple *style_tuple = dict_find(iterator, KEY_SECONDS_STYLE);
  if ((int)style_tuple->value->int32 != -1){
    int secondsStyleOld = secondsStyle;
    secondsStyle = (int)style_tuple->value->int32;
    char buffer[15];
    snprintf(buffer, sizeof(buffer), "%d", secondsStyle);
    APP_LOG(APP_LOG_LEVEL_INFO, buffer);
    if (secondsStyleOld != secondsStyle){
      if (secondsStyleOld == 0){
        tick_timer_service_unsubscribe();
        tick_timer_service_subscribe(secondsStyle == 0 ? MINUTE_UNIT : SECOND_UNIT, tick_handler);
      }
      update_style();
    }
  }
  char buffer1[12];
  char buffer2[12];
  //char bufferNull[4] = "null";

  //snprintf(buffer1, sizeof(buffer1), "%s", temp_tuple->value->cstring);
  //snprintf(buffer2, sizeof(buffer2), "%s", conditions_tuple->value->cstring);

  if ((int)temp_tuple->value->int32 != -274 && (int)conditions_tuple->value->int32 != -1){
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°C", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
  }


  // If all data is available, use it

  //if (style_tuple != NULL){

    //}
    /*main_window_unload(s_main_window);
    deinit();
    init();
    main_window_load(s_main_window);*/
  //}
  //secondsStyle = (int)style_tuple->value->int32;

  // Assemble full string and display

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

static GPoint calc(){
  GPoint p = GPoint(cos((seconds * 6 -90)*DEG)*110+POS_X,sin((seconds*6-90)*DEG)*110+POS_Y);


  //double k = p.y/p.x;

  /*char buffer[256];
  snprintf(buffer, sizeof(buffer), "Sekunde %d: y(x) = %d * x", seconds, (int)k);
  APP_LOG(APP_LOG_LEVEL_INFO, buffer);*/

  return p;
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

static void update_layer(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0,0,11, 11), 0, 0);
  APP_LOG(APP_LOG_LEVEL_INFO, "update_layer called");
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
  //graphics_context_set_stroke_color(ctx, secondsColor);
  //graphics_context_set_stroke_width(ctx, RADIUS_SECONDS);
  //graphics_draw_line(ctx, GPoint(POS_X, POS_Y), calc());
  graphics_fill_circle(ctx, calcNew(), RADIUS_SECONDS);
}

static void update_time() {
  char buffer[3];
  snprintf(buffer, sizeof(buffer), "%d", secondsStyle);
  APP_LOG(APP_LOG_LEVEL_INFO, buffer);
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[10];
  static char s_date_buffer[12];
  //static char s_buffer_seconds[3];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  strftime(s_date_buffer, sizeof(s_date_buffer),  "%d. %b", tick_time);
  //strftime(s_buffer_seconds, sizeof(s_buffer_seconds), clock_is_24h_style() ? "%S":"%S", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, s_date_buffer);
  time_until_update();
}

static void update_style(){
  if (secondsStyle == 1){
    layer_set_update_proc(layer, update_display_style1);
  }
  else if (secondsStyle == 2){
    layer_set_update_proc(layer, update_display_style2);

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

  window_set_background_color(window, GColorBlack);
  GRect bounds = layer_get_bounds(window_layer);

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
  text_layer_set_text(s_time_until_layer, "50");


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

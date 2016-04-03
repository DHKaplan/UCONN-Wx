#include "pebble.h"

Window *window;

static GBitmap     *image;
static BitmapLayer *image_layer;

TextLayer *text_degrees_layer;
TextLayer *text_dayname_layer;
TextLayer *text_mmdd_layer;
TextLayer *text_year_layer;
TextLayer *text_time_layer;
TextLayer *text_battery_layer;
TextLayer *text_location_layer;

Layer       *BatteryLineLayer;

Layer       *BTLayer;

GBitmap     *battery;


GFont        fontHelvNewLight15;
GFont        fontHelvNewLight20;
GFont		     fontRobotoCondensed21; 
GFont		     fontRobotoCondensed25; 	
GFont        fontSystemGothicBold28;
GFont        fontRobotoBoldSubset49;

static char text_location[144];
static char degreesstr[] = "====";
static int  batterychargepct;
static int  batterycharging = 0;
static int  BatteryVibesDone = 0;
static char TimeOfDay[]="03:15PM ";
static char ErrMsg[40];
static char tempMsg[40];

GPoint     Linepoint;

static int BTConnected = 1;
static int BTVibesDone = 0;

GRect        bluetooth_rect;

GColor TextColorHold;
GColor BGColorHold;

void LogMsg(char ErrMsg[]) {
  static char ErrMsgResult[50];
  
  strcpy(ErrMsgResult, TimeOfDay);
  strcat(ErrMsgResult, ": ");
  strcat(ErrMsgResult, ErrMsg);
  
  APP_LOG(APP_LOG_LEVEL_INFO, ErrMsgResult);
}

void handle_bluetooth(bool connected) {

    if (connected) {

         BTConnected = 1;     // Connected
         BTVibesDone = 0;

    } else {
         BTConnected = 0;      // Not Connected

         if (BTVibesDone == 0) {    //Do First Vibe
             BTVibesDone = 1;

             vibes_long_pulse();
         }
    }
    layer_mark_dirty(BTLayer);
}


//BT Logo Callback;
void BTLine_update_callback(Layer *BTLayer, GContext* BT1ctx) {

       GPoint BTLinePointStart;
       GPoint BTLinePointEnd;
  
      graphics_context_set_stroke_color(BT1ctx, TextColorHold);
      
      if (BTConnected == 0) {
        
        #ifdef PBL_COLOR 
            graphics_context_set_stroke_color(BT1ctx, GColorRed);
            graphics_context_set_fill_color(BT1ctx, GColorWhite);
            graphics_fill_rect(BT1ctx, layer_get_bounds(BTLayer), 0, GCornerNone);
        #else
            graphics_context_set_stroke_color(BT1ctx, GColorBlack);
            graphics_context_set_fill_color(BT1ctx, GColorWhite);
            graphics_fill_rect(BT1ctx, layer_get_bounds(BTLayer), 0, GCornerNone);

        #endif 
      
        // "X"" Line 1
          BTLinePointStart.x = 1;
          BTLinePointStart.y = 1;
  
          BTLinePointEnd.x = 20;
          BTLinePointEnd.y = 20;
          graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);   
          
         // "X"" Line 2
          BTLinePointStart.x = 1;
          BTLinePointStart.y =20;
  
          BTLinePointEnd.x = 20;
          BTLinePointEnd.y = 1;
          graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
         
      } else {   

       //Line 1
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 1;
  
       BTLinePointEnd.x = 10;
       BTLinePointEnd.y = 20;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
         
       //Line 1a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 1;
  
       BTLinePointEnd.x = 11;
       BTLinePointEnd.y = 20;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
   
       //Line 2
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 1;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 6;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
      
       //Line 2a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 1;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 5;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
 
       //Line 3
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 5;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 15;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
  
       //Line 3a
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 6;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 16;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);

       //Line 4
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 15;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 5;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
  
       //Line 4a
       BTLinePointStart.x = 4;
       BTLinePointStart.y = 16;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 6;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
  
       //Line 5
       BTLinePointStart.x = 10;
       BTLinePointStart.y = 20;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 15;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);
  
       //Line 5a
       BTLinePointStart.x = 11;
       BTLinePointStart.y = 20;
  
       BTLinePointEnd.x = 17;
       BTLinePointEnd.y = 16;
       graphics_draw_line(BT1ctx, BTLinePointStart, BTLinePointEnd);   
      }            
}    

void battery_line_layer_update_callback(Layer *BatteryLineLayer, GContext* ctx) { 
     graphics_context_set_fill_color(ctx, TextColorHold);
     graphics_fill_rect(ctx, layer_get_bounds(BatteryLineLayer), 3, GCornersAll);

     if (batterycharging == 1) {
       #ifdef PBL_COLOR
          graphics_context_set_fill_color(ctx, GColorBlue);
       #else
          graphics_context_set_fill_color(ctx, GColorBlack);
       #endif

       graphics_fill_rect(ctx, GRect(2, 1, 100, 4), 3, GCornersAll);

     } else if (batterychargepct > 20) {
       #ifdef PBL_COLOR
          graphics_context_set_fill_color(ctx, GColorGreen);
       #else
          graphics_context_set_fill_color(ctx, GColorBlack);
       #endif
         
       graphics_fill_rect(ctx, GRect(2, 1, batterychargepct, 4), 3, GCornersAll);
       
     } else {
      #ifdef PBL_COLOR
          graphics_context_set_fill_color(ctx, GColorRed);
       #else
          graphics_context_set_fill_color(ctx, GColorBlack);
       #endif
         
       graphics_fill_rect(ctx, GRect(2, 1, batterychargepct, 4),3, GCornersAll);
     }
}

void handle_battery(BatteryChargeState charge_state) {
    static char BatteryPctTxt[] = "+100%";

  batterychargepct = charge_state.charge_percent;

  if (charge_state.is_charging) {
    batterycharging = 1;
  } else {
    batterycharging = 0;
  }

  // Reset if Battery > 20% ********************************
  if (batterychargepct > 20) {
     if (BatteryVibesDone == 1) {     //OK Reset to normal
         BatteryVibesDone = 0;
     }
  }

  //
  if (batterychargepct < 30) {
     if (BatteryVibesDone == 0) {            // Do Once
         BatteryVibesDone = 1;
         vibes_long_pulse();
      }
  }

   if (charge_state.is_charging) {
     strcpy(BatteryPctTxt, "Chrg");
  } else {
     snprintf(BatteryPctTxt, 5, "%d%%", charge_state.charge_percent);
  }
   text_layer_set_text(text_battery_layer, BatteryPctTxt);

  layer_mark_dirty(BatteryLineLayer);
}


void handle_appfocus(bool in_focus){
    if (in_focus) {
        handle_bluetooth(bluetooth_connection_service_peek());
    }
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

// Need to be static because they're used by the system later.
  static char dayname_text[] = "XXX";
  static char time_text[] = "00:00";
  static char mmdd_text[] = "XXXX00";
  static char year_text[] = "0000";
   
  int FirstTime = 0;
  
  char *time_format;
  
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  
  clock_copy_time_string(TimeOfDay, sizeof(TimeOfDay));

  handle_battery(battery_state_service_peek());
  
  handle_bluetooth(bluetooth_connection_service_peek());
    
  // Set Time of Day  
  strftime(time_text, sizeof(time_text), time_format, tick_time); 

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
 
//Set Day and Date
  strftime(dayname_text, sizeof(dayname_text), "%a",    tick_time);
  strftime(mmdd_text,    sizeof(mmdd_text),    "%b %d", tick_time); 
  strftime(year_text,    sizeof(year_text),    "%Y",    tick_time); 
   
//Initialize
if (FirstTime == 0) {
    text_layer_set_text(text_dayname_layer, dayname_text);
    text_layer_set_text(text_mmdd_layer, mmdd_text);
    text_layer_set_text(text_year_layer, year_text);    
    FirstTime = 1;
    }

if (units_changed & DAY_UNIT) {
   // Only update the day name & date when it's changed. 
    text_layer_set_text(text_dayname_layer, dayname_text);
    text_layer_set_text(text_mmdd_layer, mmdd_text);
    text_layer_set_text(text_year_layer, year_text);    
  }

// Get weather update every 15 minutes ************************
  if(tick_time->tm_min % 15 == 0) {
    LogMsg("In *15* minute update");
  
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
    
  
 // Always update time of day
  text_layer_set_text(text_time_layer, time_text);
}


//Receive Temperature  and City * * *
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  int FirstCall = 0;
  
  // Read Temperature
  Tuple *t = dict_read_first(iterator);
  
  strcpy(degreesstr,(t->value->cstring));
  

  if (strcmp(degreesstr, "N/A") != 0) {
      int tempint = 100;
  
      tempint = atoi(degreesstr);
  
      tempint = ((tempint * 9) / 5) + 32;
  
      // Assemble full string and display
      snprintf(degreesstr, 5, "%dF", tempint);
     }  
     else {
      strcpy(degreesstr, "N/A");  
     } 

  text_layer_set_text(text_degrees_layer, degreesstr); 
  
  //Read City
  Tuple *u = dict_read_next(iterator);
  strcpy(text_location,(u->value->cstring));
  text_layer_set_text(text_location_layer, text_location);
  
  APP_LOG(APP_LOG_LEVEL_ERROR, text_location);
  
  if (FirstCall == 0) {
      clock_copy_time_string(TimeOfDay, sizeof(TimeOfDay));
      FirstCall = 1;
  }
  
  strcpy(tempMsg, "In Inbox Rx: Location: ");
  strcat(tempMsg, text_location);
  LogMsg(tempMsg);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, TimeOfDay);
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, TimeOfDay);
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  LogMsg("Outbox send success");
}
 

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  app_focus_service_unsubscribe();
  
  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_year_layer);
  text_layer_destroy(text_mmdd_layer);
  text_layer_destroy(text_dayname_layer);
  text_layer_destroy(text_degrees_layer);
  text_layer_destroy(text_battery_layer);

  layer_destroy(BTLayer);
 
  gbitmap_destroy(image);

  bitmap_layer_destroy(image_layer);
  
  app_message_deregister_callbacks();

  fonts_unload_custom_font(fontHelvNewLight15);
  fonts_unload_custom_font(fontHelvNewLight20);
  fonts_unload_custom_font(fontRobotoCondensed25);
  fonts_unload_custom_font(fontRobotoBoldSubset49); 
  
  window_destroy(window);
  
}

void handle_init(void) { 
  GColor BGCOLOR   = COLOR_FALLBACK(GColorDukeBlue, GColorBlack);
  BGColorHold = BGCOLOR;
  
  GColor TEXTCOLOR = COLOR_FALLBACK(GColorWhite, GColorWhite);
  TextColorHold = TEXTCOLOR;

  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, BGCOLOR);

  Layer *window_layer = window_get_root_layer(window);
   
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  fontHelvNewLight15    = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HELV_NEW_LIGHT_15));
  fontHelvNewLight20     = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HELV_NEW_LIGHT_20));
  fontRobotoCondensed21  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21));
  fontRobotoCondensed25  = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_25));
  fontSystemGothicBold28 = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  fontRobotoBoldSubset49 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49));
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
   //degrees
  text_degrees_layer = text_layer_create(GRect(25, 1, 55, 200)); 
  text_layer_set_text_alignment(text_degrees_layer, GTextAlignmentRight);		
  text_layer_set_text(text_degrees_layer, degreesstr); 
  text_layer_set_font(text_degrees_layer, fontHelvNewLight20);
  text_layer_set_background_color(text_degrees_layer, BGCOLOR);
  text_layer_set_text_color(text_degrees_layer, TEXTCOLOR); 
  layer_add_child(window_layer, text_layer_get_layer(text_degrees_layer));
  
  // year
  text_year_layer = text_layer_create(GRect(56, 86, 88, 82));  //was 62, 86, 82, 82   144wx168tall
  text_layer_set_text_color(text_year_layer, TEXTCOLOR);
  text_layer_set_background_color(text_year_layer, BGCOLOR);
  text_layer_set_font(text_year_layer, fontRobotoCondensed25);
  text_layer_set_text_alignment(text_year_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_year_layer));
  
  // mmdd
  text_mmdd_layer = text_layer_create(GRect(56, 62, 88, 28));
  text_layer_set_text_color(text_mmdd_layer, TEXTCOLOR);
  text_layer_set_background_color(text_mmdd_layer, BGCOLOR);
  text_layer_set_font(text_mmdd_layer, fontRobotoCondensed25);
  text_layer_set_text_alignment(text_mmdd_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_mmdd_layer));
  
   // Dayname
  text_dayname_layer = text_layer_create(GRect(56, 36, 88, 28));
  text_layer_set_text_color(text_dayname_layer, TEXTCOLOR);
  text_layer_set_background_color(text_dayname_layer, BGCOLOR);
  text_layer_set_font(text_dayname_layer, fontSystemGothicBold28);
  text_layer_set_text_alignment(text_dayname_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_dayname_layer)); 
  
   //Location
  text_location_layer = text_layer_create(GRect(1, 23, 144, 15)); 
  text_layer_set_text_alignment(text_location_layer, GTextAlignmentCenter);		
  text_layer_set_text(text_location_layer, text_location); 
  text_layer_set_font(text_location_layer, fontHelvNewLight15);
  text_layer_set_background_color(text_location_layer, BGCOLOR);
  text_layer_set_text_color(text_location_layer, TEXTCOLOR);
  layer_add_child(window_layer, text_layer_get_layer(text_location_layer));

// The bitmap layer holds the image for logo display
  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UCONN_COLOR_LOGO);
  image_layer = bitmap_layer_create(GRect(1, 40, 60, 77));
  bitmap_layer_set_bitmap(image_layer, image);
  bitmap_layer_set_background_color(image_layer, BGCOLOR);
  bitmap_layer_set_alignment(image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
  
  
  // Time of Day is here
  text_time_layer = text_layer_create(GRect(1, 115, 144, 53));
  text_layer_set_text_color(text_time_layer, TEXTCOLOR);
  text_layer_set_background_color(text_time_layer, BGCOLOR);
  text_layer_set_font(text_time_layer, fontRobotoBoldSubset49);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
  
  // Battery Line
  GRect battery_line_frame = GRect(22, 117, 104, 6);
  BatteryLineLayer = layer_create(battery_line_frame);
  layer_set_update_proc(BatteryLineLayer, battery_line_layer_update_callback);
  layer_add_child(window_layer, BatteryLineLayer);
    
  
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);


  //Bluetooth Logo Setup area
  GRect BTArea = GRect(1, 5, 20, 20);
  BTLayer = layer_create(BTArea);

  layer_add_child(window_layer, BTLayer); 
    
  layer_set_update_proc(BTLayer, BTLine_update_callback);  
  
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  
  handle_bluetooth(bluetooth_connection_service_peek());
         
  text_battery_layer = text_layer_create(GRect(85,2,55,26));
  text_layer_set_text_color(text_battery_layer, TEXTCOLOR);
  text_layer_set_background_color(text_battery_layer, BGCOLOR);
  text_layer_set_font(text_battery_layer,  fontHelvNewLight20);
  text_layer_set_text_alignment(text_battery_layer, GTextAlignmentRight);

  layer_add_child(window_layer, text_layer_get_layer(text_battery_layer));

  battery_state_service_subscribe(&handle_battery);
        
  app_focus_service_subscribe(&handle_appfocus);

  handle_battery(battery_state_service_peek());
  
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_tick(current_time, SECOND_UNIT);
  
}


int main(void) {
   handle_init();

   app_event_loop();
   
   handle_deinit();
}



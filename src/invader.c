#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

// 144x168

#define MY_UUID { 0xAA, 0xF1, 0xE8, 0xBE, 0x89, 0x39, 0x48, 0x17, 0x80, 0xBE, 0x36, 0xA4, 0x11, 0x38, 0x55, 0x3B }
PBL_APP_INFO(MY_UUID,
             "Invader", "Sean McMains",
             0, 1, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

TextLayer timeLayer; // The clock

Layer imageLayer; // where we'll draw the invader

BmpContainer invader1_image;
BmpContainer invader2_image;



void draw_layer_image(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;

  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

  PblTm tick_time;
  get_time(&tick_time);

  if ( ( tick_time.tm_sec % 2 ) == 0 ) {
  GRect destination = layer_get_frame(&invader1_image.layer.layer);
  graphics_draw_bitmap_in_rect(ctx, &invader1_image.bmp, destination);
} else {
	GRect destination = layer_get_frame(&invader2_image.layer.layer);
  graphics_draw_bitmap_in_rect(ctx, &invader2_image.bmp, destination);
  
}

}


// Called once per second
void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  static char timeText[] = "00:00:00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;

  get_time(&currentTime);

  string_format_time(timeText, sizeof(timeText), "%T", &currentTime);

  text_layer_set_text(&timeLayer, timeText);

  layer_mark_dirty( &imageLayer ); // request the image layer redraw itself later

}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Invader");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  text_layer_init(&timeLayer, GRect(29, 130, 144-40 /* width */, 168-130 /* height */));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_second_tick(ctx, NULL);

  layer_add_child(&window.layer, &timeLayer.layer);

  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_INVADER1, &invader1_image);
  bmp_init_container(RESOURCE_ID_INVADER2, &invader2_image);

  layer_init(&imageLayer, GRect( 6, 6, 132, 96));
  imageLayer.update_proc = &draw_layer_image;
  layer_add_child(&window.layer, &imageLayer);

}

void handle_deinit(AppContextRef ctx) {
	(void)ctx;
  
	bmp_deinit_container(&invader1_image);
	bmp_deinit_container(&invader2_image);
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}

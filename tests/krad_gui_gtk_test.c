#include "krad_gui_gtk.h"

#define TEST_TIME 5222


void render_nogtk_benchmark() {

  kradgui_t *kradgui;

  int count = 0;

  kradgui = kradgui_create_with_internal_surface(960, 540);

  kradgui->update_drawtime = 1;
  kradgui->print_drawtime = 1;
  
  kradgui->render_ftest = 1;
  kradgui->render_tearbar = 1;
  kradgui_test_screen(kradgui, "bench test");
  
  while (count < TEST_TIME) {

    count++;

    kradgui_render(kradgui);
  }
  
  
  kradgui_destroy(kradgui);
  
  
}

void render_ftest() {

  kradgui_t *kradgui;

  int count = 0;

  kradgui = kradgui_create(960, 540);

  kradgui->update_drawtime = 1;
  kradgui->print_drawtime = 1;
  //kradgui->render_wheel = 1;
  kradgui->render_ftest = 1;
  //kradgui->render_tearbar = 1;
  //kradgui_test_screen(kradgui, "Gtk test");
  
  kradgui_gtk_start(kradgui);
  
  //kradgui_gtk_set_fps(kradgui, 110);
  
  while (count < TEST_TIME) {

    count++;

    usleep(50000);
  }
  
  kradgui_gtk_end(kradgui);
  kradgui_destroy(kradgui);
}


void render_test_screen() {

  kradgui_t *kradgui;

  int count = 0;

  kradgui = kradgui_create(1280, 720);

  //kradgui_add_item(kradgui, REEL_TO_REEL);
  //kradgui_add_item(kradgui, PLAYBACK_STATE_STATUS);
  //kradgui_set_total_track_time_ms(kradgui, 5 * 60 * 1000);

  kradgui_gtk_start(kradgui);
  
  kradgui_test_screen(kradgui, "Gtk test");
  
  while (count < TEST_TIME) {

    count++;

    usleep(50000);
  }
  
  kradgui_gtk_end(kradgui);
  kradgui_destroy(kradgui);
}


void render_live_recording_test() {

  kradgui_t *kradgui;

  int count = 0;

  kradgui = kradgui_create(1280, 720);

  //kradgui_add_item(kradgui, REEL_TO_REEL);
  //kradgui_add_item(kradgui, PLAYBACK_STATE_STATUS);
  //kradgui_set_total_track_time_ms(kradgui, 5 * 60 * 1000);

  kradgui_gtk_start(kradgui);
  
  while (count < TEST_TIME) {

    if (count > 10) {
      kradgui_go_live(kradgui);
    }
  
    if (count > 30) {
      kradgui_start_recording(kradgui);
    }
  
    if (count > 210) {
      kradgui_go_off(kradgui);
    }
  
    if (count > 230) {
      kradgui_stop_recording(kradgui);
    }

    if (count > 230) {
      kradgui_go_live(kradgui);
    }
  
    if (count > 260) {
      kradgui_start_recording(kradgui);
    }

    count++;

    usleep(50000);
  }
  
  kradgui_gtk_end(kradgui);
  kradgui_destroy(kradgui);
}


void render_reel_to_reel_test() {

  kradgui_t *kradgui;

  int count = 0;

  kradgui = kradgui_create(320, 160);

  kradgui->update_drawtime = 1;
  kradgui->print_drawtime = 1;

  kradgui_add_item(kradgui, REEL_TO_REEL);
  kradgui_add_item(kradgui, PLAYBACK_STATE_STATUS);
  kradgui_set_total_track_time_ms(kradgui, 5 * 60 * 1000);

  kradgui_gtk_start(kradgui);
  
  while ((count < TEST_TIME)) {

    kradgui_add_current_track_time_ms(kradgui, 6 *  5);
  
    if (kradgui->current_track_time_ms >= kradgui->total_track_time_ms) {
      kradgui_set_current_track_time_ms(kradgui, 0);
    }
  
    count++;

    usleep(50000);
  }
  
  kradgui_gtk_end(kradgui);
  kradgui_destroy(kradgui);
}



int main (int argc, char *argv[]) {

  //render_nogtk_benchmark();

  //render_reel_to_reel_test();

  //render_live_recording_test();

  //render_test_screen();

  render_ftest();

    return 0;
}


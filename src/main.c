/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE /* needed for usleep() */
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#include "lvgl/lvgl.h"
#include <SDL.h>
#include "services/Data.h"

#include "hal/hal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static char *format_milliseconds(long long total_ms);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_USE_OS != LV_OS_FREERTOS

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  sdl_hal_init(800, 480);

  /* Run the default demo */
  /* To try a different demo or example, replace this with one of: */
  /* - lv_demo_benchmark(); */
  /* - lv_demo_stress(); */
  /* - lv_example_label_1(); */
  /* - etc. */

  // Create 3 panels
  lv_obj_t *screen = lv_screen_active();

  lv_obj_set_style_bg_color(
      screen,
      lv_color_hex(0x0F0C12),
      0);

  lv_obj_set_style_bg_opa(
      screen,
      LV_OPA_COVER,
      0);

  lv_obj_t *leftPanel = lv_obj_create(screen);
  lv_obj_set_size(leftPanel, 220, 420);
  lv_obj_align(leftPanel, LV_ALIGN_LEFT_MID, 20, 0);

  lv_obj_t *middlePanel = lv_obj_create(screen);
  lv_obj_set_size(middlePanel, 300, 420);
  lv_obj_align(middlePanel, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *spotifyTitle = lv_label_create(middlePanel);
  lv_obj_t *albumCover = lv_image_create(middlePanel);
  lv_obj_t *progressBar = lv_bar_create(middlePanel);
  lv_obj_t *spotifyArtist = lv_label_create(middlePanel);
  lv_obj_t *currentTime = lv_label_create(middlePanel);
  lv_obj_t *duration = lv_label_create(middlePanel);

  lv_obj_t *rightPanel = lv_obj_create(screen);
  lv_obj_set_size(rightPanel, 220, 420);
  lv_obj_align(rightPanel, LV_ALIGN_RIGHT_MID, -20, 0);

  lv_obj_set_style_radius(leftPanel, 20, 0);
  lv_obj_set_style_radius(middlePanel, 20, 0);
  lv_obj_set_style_radius(rightPanel, 20, 0);

  lv_obj_set_style_bg_color(leftPanel, lv_color_hex(0x1A1620), 0);
  lv_obj_set_style_bg_color(middlePanel, lv_color_hex(0x1A1620), 0);
  lv_obj_set_style_bg_color(rightPanel, lv_color_hex(0x1A1620), 0);

  lv_obj_set_style_border_width(leftPanel, 0, 0);
  lv_obj_set_style_border_width(middlePanel, 0, 0);
  lv_obj_set_style_border_width(rightPanel, 0, 0);

  Song song = {};
  char displayed_song_id[sizeof(song.id)] = {};
  uint32_t last_spotify_poll = 0;
  uint32_t last_progress_update = 0;
  long long playback_position_ms = 0;
  bool has_song = false;

  while (1)
  {

    // START -----------------------------------------------
    if (!has_song || lv_tick_elaps(last_spotify_poll) >= 5000)
    {
      song = getCurrentSong();
      last_spotify_poll = lv_tick_get();
      last_progress_update = last_spotify_poll;
      playback_position_ms = song.progress_ms;
      has_song = true;

      if (strcmp(song.id, displayed_song_id) != 0 &&
          downloadAlbumArt(song.album_art_url, "album_art.jpg"))
      {
        lv_image_set_src(albumCover, "A:album_art.jpg");
        strncpy(displayed_song_id, song.id, sizeof(displayed_song_id) - 1);
        displayed_song_id[sizeof(displayed_song_id) - 1] = '\0';
      }

      lv_label_set_text(spotifyTitle, song.name);
      lv_obj_align(spotifyTitle, LV_ALIGN_BOTTOM_MID, 0, -100);

      lv_label_set_text(spotifyArtist, song.artist);
      lv_obj_align(spotifyArtist, LV_ALIGN_BOTTOM_MID, 0, -75);

      lv_obj_set_size(progressBar, 240, 12);
      lv_obj_align(progressBar, LV_ALIGN_BOTTOM_MID, 0, -55);

      lv_bar_set_range(progressBar, 0, song.duration_ms);

      char *duration_text = format_milliseconds(song.duration_ms);
      if (duration_text != NULL)
      {
        lv_label_set_text(duration, duration_text);
        free(duration_text);
      }
      lv_obj_align(duration, LV_ALIGN_BOTTOM_RIGHT, -20, -25);
    }

    uint32_t elapsed_ms = lv_tick_elaps(last_progress_update);
    last_progress_update = lv_tick_get();
    if (song.is_playing && song.duration_ms > 0)
    {
      playback_position_ms += elapsed_ms;
      if (playback_position_ms > song.duration_ms)
      {
        playback_position_ms = song.duration_ms;
      }
    }

    lv_bar_set_value(progressBar, (int32_t)playback_position_ms, LV_ANIM_OFF);

    char *current_time_text = format_milliseconds(playback_position_ms);
    if (current_time_text != NULL)
    {
      lv_label_set_text(currentTime, current_time_text);
      free(current_time_text);
    }
    lv_obj_align(currentTime, LV_ALIGN_BOTTOM_LEFT, 20, -25);

    // change text color for labels
    lv_obj_set_style_text_color(spotifyTitle, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_color(spotifyArtist, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_color(currentTime, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_color(duration, lv_color_hex(0xFFFFFF), 0);

    // Album cover
    lv_obj_set_size(albumCover, 200, 200);
    lv_obj_align(albumCover, LV_ALIGN_CENTER, 0, -70);

    // Scale the complete square artwork to fit inside the widget.
    lv_image_set_inner_align(albumCover, LV_IMAGE_ALIGN_CONTAIN);

    // Style and crop overflow
    lv_obj_set_style_radius(albumCover, 20, 0);
    lv_obj_set_style_clip_corner(albumCover, true, 0);

    // END -----------------------------------------------

    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    uint32_t sleep_time_ms = lv_timer_handler();
    if (sleep_time_ms == LV_NO_TIMER_READY)
    {
      sleep_time_ms = LV_DEF_REFR_PERIOD;
    }
#ifdef _MSC_VER
    Sleep(sleep_time_ms);
#else
    usleep(sleep_time_ms * 1000);
#endif
  }

  return 0;
}

#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

static char *format_milliseconds(long long total_ms)
{
  long long total_seconds;
  int minutes;
  int seconds;
  size_t result_size = 16;
  char *result_buffer;

  if (total_ms < 0)
  {
    total_ms = 0;
  }

  total_seconds = total_ms / 1000;
  minutes = (int)(total_seconds / 60);
  seconds = (int)(total_seconds % 60);

  result_buffer = (char *)malloc(result_size);
  if (result_buffer == NULL)
  {
    return NULL;
  }

  snprintf(result_buffer, result_size, "%d:%02d", minutes, seconds);

  return result_buffer;
}
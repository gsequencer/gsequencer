/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <cairo.h>

#include <math.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

void ags_visual_phase_shift_putpixel(unsigned char *data, int x, int y, unsigned long int pixel);

void ags_visual_phase_shift_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
void ags_visual_phase_shift_button_press_callback(GtkWidget *widget, GdkEvent *event, gpointer data);
gboolean ags_visual_phase_shift_timeout(gpointer data);
gboolean ags_visual_phase_shift_orig_wave_draw(GtkWidget *widget,
					       cairo_t *cr,
					       gpointer user_data);
gboolean ags_visual_phase_shift_phase_shifted_wave_draw(GtkWidget *widget,
							cairo_t *cr,
							gpointer user_data);

#define AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS (2)

#define AGS_VISUAL_PHASE_SHIFT_TEST_SAMPLERATE (44100)
#define AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE (1024)
#define AGS_VISUAL_PHASE_SHIFT_TEST_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)

#define AGS_VISUAL_PHASE_SHIFT_TEST_FRAME_COUNT (22050.0)
#define AGS_VISUAL_PHASE_SHIFT_TEST_BASE_FREQ (440.0)
#define AGS_VISUAL_PHASE_SHIFT_TEST_VOLUME (1.0)

#define AGS_VISUAL_PHASE_SHIFT_TEST_AMOUNT (M_PI)
#define AGS_VISUAL_PHASE_SHIFT_TEST_PHASE (0.0)

#define AGS_VISUAL_PHASE_SHIFT_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"				\
  "simple-file=true\n"					\
  "disable-feature=experimental\n"			\
  "segmentation=4/4\n"					\
  "\n"							\
  "[thread]\n"						\
  "model=super-threaded\n"				\
  "super-threaded-scope=audio\n"			\
  "lock-global=ags-thread\n"				\
  "lock-parent=ags-recycling-thread\n"			\
  "thread-pool-max-unused-threads=8\n"			\
  "max-precision=125\n"					\
  "\n"							\
  "[soundcard-0]\n"					\
  "backend=alsa\n"					\
  "device=hw:CARD=PCH,DEV=0\n"				\
  "samplerate=44100\n"					\
  "buffer-size=1024\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"

AgsAudioApplicationContext *audio_application_context;

AgsAudio *output_panel;
AgsAudio *wave_player;

AgsTaskLauncher *task_launcher;

GObject *output_soundcard;

GtkWindow *window;
GtkDrawingArea *orig_wave;
GtkDrawingArea *phase_shifted_wave;

cairo_surface_t *orig_surface = NULL;
cairo_surface_t *phase_shifted_surface = NULL;

GList *start_sine_wave;
GList *start_phase_shifted_sine_wave;

guint RED_PIXEL = 0xff0000;
guint WHITE_PIXEL = 0xffffff;
guint BLACK_PIXEL = 0x0;

gint STRIDE;

gint16 s16_buffer[(guint) AGS_VISUAL_PHASE_SHIFT_TEST_FRAME_COUNT];
gint16 s16_phase_shifted_buffer[(guint) AGS_VISUAL_PHASE_SHIFT_TEST_FRAME_COUNT];

gdouble orig_buffer[1920];
gdouble shift_buffer[1920];
gdouble phase_shifted_buffer[1920];

gint64 start_playback = -1;
gboolean is_playing = FALSE;

void
ags_visual_phase_shift_putpixel(unsigned char *data, int x, int y, unsigned long int pixel)
{
  int bpp = 4; // STRIDE / 400;
  /* Here p is the address to the pixel we want to set */
  unsigned char *p = data + y * STRIDE + x * bpp;

  if(x < 0 ||
     y < 0 ||
     x >= 1920 ||
     y >= 200){
    return;
  }
  
  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(unsigned short int *)p = pixel;
    break;

  case 3:
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    break;

  case 4:
    *(unsigned long int *)p = pixel;
    break;
  }
}


void
ags_visual_phase_shift_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  /* leave main loop */
  gtk_main_quit();
}

void
ags_visual_phase_shift_button_press_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  AgsStartAudio *start_audio;
  AgsStartSoundcard *start_soundcard;

  g_message("play");
  
  if(widget == orig_wave){
    wave_player->wave = start_sine_wave;
  }else{
    wave_player->wave = start_phase_shifted_sine_wave;
  }

  start_soundcard = ags_start_soundcard_new(audio_application_context);
  
  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) start_soundcard);
  
  start_audio = ags_start_audio_new(wave_player,
				    AGS_SOUND_SCOPE_WAVE);
  
  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) start_audio);

  is_playing = TRUE;
  start_playback = g_get_monotonic_time();
}

gboolean
ags_visual_phase_shift_timeout(gpointer data)
{
  AgsCancelAudio *cancel_audio;

  gint64 current_playback;
  
  if(!is_playing){
    return(G_SOURCE_CONTINUE);
  }
  
  current_playback = g_get_monotonic_time();

  if(current_playback > start_playback + G_USEC_PER_SEC * (AGS_VISUAL_PHASE_SHIFT_TEST_SAMPLERATE / AGS_VISUAL_PHASE_SHIFT_TEST_FRAME_COUNT)){
    g_message("stop");

    /* create cancel task */
    cancel_audio = ags_cancel_audio_new(wave_player,
					AGS_SOUND_SCOPE_WAVE);

    ags_task_launcher_add_task(task_launcher,
			       (AgsTask *) cancel_audio);

    wave_player->wave = NULL;

    is_playing = FALSE;
  }

  return(G_SOURCE_CONTINUE);
}

gboolean
ags_visual_phase_shift_orig_wave_draw(GtkWidget *widget,
				      cairo_t *cr,
				      gpointer user_data)
{
  unsigned char *data;

  double value;
  int i;

  data = cairo_image_surface_get_data(orig_surface);

  memset(data, 0xff, 4 * 1920 * 200 * sizeof(guchar));

  for(i = 0; i < 1920; i++){
    value = orig_buffer[i] * 100.0;    

    ags_visual_phase_shift_putpixel(data, i, 100 + (int) floor(value), BLACK_PIXEL);
  }

  cairo_surface_flush(orig_surface);

  /* do painting */
  cairo_set_source_surface(cr, orig_surface, 0, 0);
  cairo_surface_mark_dirty(orig_surface);

  cairo_paint(cr);
    
  return(FALSE);
}

gboolean
ags_visual_phase_shift_phase_shifted_wave_draw(GtkWidget *widget,
					       cairo_t *cr,
					       gpointer user_data)
{
  guchar *data;

  double value;
  int i;

  data = cairo_image_surface_get_data(phase_shifted_surface);

  memset(data, 0xff, 4 * 1920 * 200 * sizeof(guchar));
  
  for(i = 0; i < 1920; i++){
    value = phase_shifted_buffer[i] * 100.0;    

    ags_visual_phase_shift_putpixel(data, i, 100 + (int) floor(value), BLACK_PIXEL);
  }

  for(i = 0; i < 1920; i++){
    value = shift_buffer[i] * 100.0;    

    ags_visual_phase_shift_putpixel(data, i, 100 + (int) floor(value), RED_PIXEL);
  }
  
  cairo_surface_flush(phase_shifted_surface);

  /* do painting */
  cairo_set_source_surface(cr, phase_shifted_surface, 0, 0);
  cairo_surface_mark_dirty(phase_shifted_surface);

  cairo_paint(cr);

  return(FALSE);
}

GList*
ags_visual_phase_shift_test_create_sine_wave()
{
  GList *start_wave, *wave;

  guint i, j;

  ags_synth_util_sin_s16(s16_buffer,
			 AGS_VISUAL_PHASE_SHIFT_TEST_BASE_FREQ, 0.0, 1.0,
			 AGS_VISUAL_PHASE_SHIFT_TEST_SAMPLERATE,
			 0, (guint) AGS_VISUAL_PHASE_SHIFT_TEST_FRAME_COUNT);
  
  start_wave = NULL;
  
  for(i = 0; i < AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;
    
    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", AGS_VISUAL_PHASE_SHIFT_TEST_SAMPLERATE,
		 "buffer-size", AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE,
		 "format", AGS_VISUAL_PHASE_SHIFT_TEST_FORMAT,
		 NULL);

    start_wave = ags_wave_add(start_wave,
			      current_wave);

    for(j = 0; j < floor(AGS_VISUAL_PHASE_SHIFT_TEST_FRAME_COUNT / AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE); j++){
      AgsBuffer *buffer;
      
      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", AGS_VISUAL_PHASE_SHIFT_TEST_SAMPLERATE,
		   "buffer-size", AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE,
		   "format", AGS_VISUAL_PHASE_SHIFT_TEST_FORMAT,
		   "x", (guint64) j * AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE,
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);

      memcpy(buffer->data, s16_buffer + j * AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE, AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE * sizeof(gint16));
    }
  }

  return(start_wave);
}

GList*
ags_visual_phase_shift_test_create_phase_shifted_sine_wave()
{
  GList *start_wave, *wave;

  guint i, j;

  ags_phase_shift_util_compute_s16(s16_phase_shifted_buffer,
				   s16_buffer,
				   (guint) AGS_VISUAL_PHASE_SHIFT_TEST_FRAME_COUNT,
				   AGS_VISUAL_PHASE_SHIFT_TEST_SAMPLERATE,
				   AGS_VISUAL_PHASE_SHIFT_TEST_BASE_FREQ,
				   AGS_VISUAL_PHASE_SHIFT_TEST_AMOUNT,
				   AGS_VISUAL_PHASE_SHIFT_TEST_PHASE);

  start_wave = NULL;
  
  for(i = 0; i < AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS; i++){
    AgsWave *current_wave;
    
    current_wave = ags_wave_new(wave_player,
				i);

    g_object_set(current_wave,
		 "samplerate", AGS_VISUAL_PHASE_SHIFT_TEST_SAMPLERATE,
		 "buffer-size", AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE,
		 "format", AGS_VISUAL_PHASE_SHIFT_TEST_FORMAT,
		 NULL);

    start_wave = ags_wave_add(start_wave,
			      current_wave);

    for(j = 0; j < floor(AGS_VISUAL_PHASE_SHIFT_TEST_FRAME_COUNT / AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE); j++){
      AgsBuffer *buffer;
      
      buffer = ags_buffer_new();
      g_object_set(buffer,
		   "samplerate", AGS_VISUAL_PHASE_SHIFT_TEST_SAMPLERATE,
		   "buffer-size", AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE,
		   "format", AGS_VISUAL_PHASE_SHIFT_TEST_FORMAT,
		   "x", (guint64) j * AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE,
		   NULL);
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);

      memcpy(buffer->data, s16_phase_shifted_buffer + j * AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE, AGS_VISUAL_PHASE_SHIFT_TEST_BUFFER_SIZE * sizeof(gint16));
    }
  }

  return(start_wave);
}

int
main(int argc, char* argv[])
{
  GtkBox *vbox;

  AgsChannel *channel, *link;

  AgsConfig *config;

  GList *start_list;

  guint i;
  
  gtk_init(&argc, &argv);

  ags_priority_load_defaults(ags_priority_get_instance());  

  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    AGS_VISUAL_PHASE_SHIFT_TEST_CONFIG,
			    strlen(AGS_VISUAL_PHASE_SHIFT_TEST_CONFIG));

  audio_application_context = (AgsApplicationContext *) ags_audio_application_context_new();
  g_object_ref(audio_application_context);

  ags_application_context_prepare(audio_application_context);
  ags_application_context_setup(audio_application_context);

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(audio_application_context));

  /* output soundcard */
  output_soundcard = audio_application_context->soundcard->data;

  start_list = NULL;
  
  /* output panel */
  output_panel = ags_audio_new(output_soundcard);

  g_object_ref(output_panel);
  start_list = g_list_prepend(start_list,
			      output_panel);
  
  ags_audio_set_flags(output_panel, (AGS_AUDIO_SYNC));

  ags_audio_set_audio_channels(output_panel,
			       AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(output_panel,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(output_panel,
		     AGS_TYPE_INPUT,
		     1, 0);

  /* ags-play */
  ags_recall_factory_create(output_panel,
			    NULL, NULL,
			    "ags-play-master",
			    0, AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS,
			    0, 1,
			    (AGS_RECALL_FACTORY_INPUT,
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  ags_connectable_connect(AGS_CONNECTABLE(output_panel));

  /* wave player */
  wave_player = ags_audio_new(output_soundcard);

  g_object_ref(wave_player);
  start_list = g_list_prepend(start_list,
			      wave_player);
  
  ags_audio_set_flags(wave_player, (AGS_AUDIO_SYNC |
				    AGS_AUDIO_OUTPUT_HAS_RECYCLING |
				    AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(wave_player, (AGS_SOUND_ABILITY_WAVE));
  
  ags_audio_set_audio_channels(wave_player,
			       AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS, 0);
  
  ags_audio_set_pads(wave_player,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(wave_player,
		     AGS_TYPE_INPUT,
		     1, 0);

  channel = wave_player->output;

  for(i = 0; i < AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS; i++){
    ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));

    channel = channel->next;
  }
  
  /* ags-play-wave */
  ags_recall_factory_create(wave_player,
			    NULL, NULL,
			    "ags-play-wave",
			    0, AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS,
			    0, 1,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  ags_connectable_connect(AGS_CONNECTABLE(wave_player));

  /* add to application context */
  start_list = g_list_reverse(start_list);
  ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(audio_application_context),
			       start_list);

  /* link */
  channel = output_panel->input;
  link = wave_player->output;
  
  for(i = 0; i < AGS_VISUAL_PHASE_SHIFT_TEST_AUDIO_CHANNELS; i++){
    GError *error;
    
    error = NULL;
    ags_channel_set_link(channel,
			 link,
			 &error);

    channel = channel->next;
    link = link->next;
  }

  for(i = 0; i < 1920; i++){
    orig_buffer[i] = sin(((double) i / (1920.0 / 8.0)) * 2.0 * M_PI);
  }

  {
    gdouble samplerate;
    gdouble frequency;
    gdouble freq_period;
    gdouble phase_period;
    gdouble amount;
    gdouble amount_period;

    samplerate = 1920.0;

    frequency = 8.0;
    freq_period = samplerate / frequency;

    amount = AGS_VISUAL_PHASE_SHIFT_TEST_AMOUNT;
    amount_period = (amount / (2.0 * M_PI)) * freq_period;
    
    phase_period = (AGS_VISUAL_PHASE_SHIFT_TEST_PHASE / (2.0 * M_PI)) * freq_period;
    
    for(i = 0; i < 1920; i++){
      gdouble phase_shift;

      shift_buffer[i] = 0.5 * (amount / (2.0 * M_PI)) * ((((int) ceil(i + phase_period) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    }
  }
  
  ags_phase_shift_util_compute_double(phase_shifted_buffer,
				      orig_buffer,
				      1920,
				      1920,
				      8.0,
				      AGS_VISUAL_PHASE_SHIFT_TEST_AMOUNT,
				      AGS_VISUAL_PHASE_SHIFT_TEST_PHASE);
  
  window = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_container_add((GtkContainer *) window,
		    (GtkWidget *) vbox);

  orig_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
					    1920, 200);
  phase_shifted_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
						     1920, 200);

  STRIDE = cairo_image_surface_get_stride(orig_surface);
  
  orig_wave = (GtkDrawingArea *) gtk_drawing_area_new();

  gtk_widget_set_events((GtkWidget *) orig_wave,
			GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);
  
  gtk_widget_set_size_request((GtkWidget *) orig_wave,
			      1920, 200);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) orig_wave,
		     FALSE, FALSE,
		     0);
  
  phase_shifted_wave = (GtkDrawingArea *) gtk_drawing_area_new();

  gtk_widget_set_events((GtkWidget *) phase_shifted_wave,
			GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);

  gtk_widget_set_size_request((GtkWidget *) phase_shifted_wave,
			      1920, 200);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) phase_shifted_wave,
		     FALSE, FALSE,
		     0);

  start_sine_wave = ags_visual_phase_shift_test_create_sine_wave();
  start_phase_shifted_sine_wave = ags_visual_phase_shift_test_create_phase_shifted_sine_wave();

  g_timeout_add(33,
		ags_visual_phase_shift_timeout,
		NULL);
  
  g_signal_connect(window, "delete-event",
		   G_CALLBACK(ags_visual_phase_shift_window_delete_event), NULL);
  
  g_signal_connect(orig_wave, "draw",
		   G_CALLBACK(ags_visual_phase_shift_orig_wave_draw), NULL);

  g_signal_connect(orig_wave, "button_press_event",
		   G_CALLBACK(ags_visual_phase_shift_button_press_callback), NULL);

  g_signal_connect(phase_shifted_wave, "draw",
		   G_CALLBACK(ags_visual_phase_shift_phase_shifted_wave_draw), NULL);

  g_signal_connect(phase_shifted_wave, "button_press_event",
		   G_CALLBACK(ags_visual_phase_shift_button_press_callback), NULL);
  
  gtk_widget_show_all((GtkWidget *) window);
  
  /* start audio and soundcard task */    
  gtk_main();
  
  return(0);
}

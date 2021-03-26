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

#define AGS_VISUAL_PHASE_SHIFT_TEST_CONFIG "[generic]\n"	\
  "autosave-thread=false\n"				\
  "simple-file=true\n"					\
  "disable-feature=experimental\n"			\
  "segmentation=4/4\n"					\
  "\n"							\
  "[thread]\n"						\
  "model=super-threaded\n"				\
  "super-threaded-scope=channel\n"			\
  "lock-global=ags-thread\n"				\
  "lock-parent=ags-recycling-thread\n"			\
  "\n"							\
  "[soundcard]\n"					\
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

GObject *output_soundcard;

GtkWindow *window;
GtkDrawingArea *orig_wave;
GtkDrawingArea *phase_shifted_wave;

cairo_surface_t *orig_surface = NULL;
cairo_surface_t *phase_shifted_surface = NULL;

guint RED_PIXEL = 0xff0000;
guint WHITE_PIXEL = 0xffffff;
guint BLACK_PIXEL = 0x0;

gint STRIDE;

gdouble orig_buffer[1920];
gdouble shift_buffer[1920];
gdouble *phase_shifted_buffer;

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
    gdouble phase;
    gdouble frequency;
    gdouble freq_period;
    gdouble amount;
    gdouble amount_period;

    samplerate = 1920.0;

    frequency = 8.0;
    freq_period = samplerate / frequency;

    amount = 0.5 * M_PI;
    amount_period = (amount / (2.0 * M_PI)) * freq_period;
    
    phase = freq_period - amount_period;
    
    for(i = 0; i < 1920; i++){
      shift_buffer[i] = ((((int) ceil(i + phase) % (int) ceil(freq_period)) * 2.0 * frequency / samplerate) - 1.0);
    }
  }
  
  phase_shifted_buffer = NULL;
  
  ags_phase_shift_util_compute_double(orig_buffer,
				      1920,
				      1920,
				      8.0,
				      0.5 * M_PI,
				      &phase_shifted_buffer);
  
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
  gtk_widget_set_size_request((GtkWidget *) orig_wave,
			      1920, 200);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) orig_wave,
		     FALSE, FALSE,
		     0);
  
  phase_shifted_wave = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_size_request((GtkWidget *) phase_shifted_wave,
			      1920, 200);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) phase_shifted_wave,
		     FALSE, FALSE,
		     0);

  g_signal_connect(window, "delete-event",
		   G_CALLBACK(ags_visual_phase_shift_window_delete_event), NULL);
  
  g_signal_connect(orig_wave, "draw",
		   G_CALLBACK(ags_visual_phase_shift_orig_wave_draw), NULL);

  g_signal_connect(phase_shifted_wave, "draw",
		   G_CALLBACK(ags_visual_phase_shift_phase_shifted_wave_draw), NULL);

  gtk_widget_show_all((GtkWidget *) window);
  
  gtk_main();
  
  return(0);
}

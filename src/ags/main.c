/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_main_loop.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/file/ags_file.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_gui_thread.h>
#include <ags/thread/ags_autosave_thread.h>
#include <ags/thread/ags_single_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recycling.h>

#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>

#include <ags/audio/ags_synths.h>

#include <sys/mman.h>

#include <gtk/gtk.h>
#include <libintl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include <mcheck.h>

#include <X11/Xlib.h>

#include <gdk/gdk.h>

#include <sys/types.h>
#include <pwd.h>

#include "config.h"

void ags_main_class_init(AgsMainClass *ags_main);
void ags_main_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_main_init(AgsMain *ags_main);
void ags_main_connect(AgsConnectable *connectable);
void ags_main_disconnect(AgsConnectable *connectable);
void ags_main_finalize(GObject *gobject);

void ags_colors_alloc();

static void ags_signal_cleanup();
void ags_signal_handler(int signr);

static gpointer ags_main_parent_class = NULL;
static sigset_t ags_wait_mask;

pthread_mutex_t ags_application_mutex = PTHREAD_MUTEX_INITIALIZER;
static const gchar *ags_config_thread = AGS_CONFIG_THREAD;
static const gchar *ags_config_devout = AGS_CONFIG_DEVOUT;

extern void ags_thread_resume_handler(int sig);
extern void ags_thread_suspend_handler(int sig);

extern AgsConfig *config;

extern GtkStyle *matrix_style;
extern GtkStyle *ffplayer_style;
extern GtkStyle *editor_style;
extern GtkStyle *notebook_style;
extern GtkStyle *meter_style;
extern GtkStyle *note_edit_style;

extern AgsLadspaManager *ags_ladspa_manager;

struct sigaction ags_sigact;

GType
ags_main_get_type()
{
  static GType ags_type_main = 0;

  if(!ags_type_main){
    static const GTypeInfo ags_main_info = {
      sizeof (AgsMainClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_main_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMain),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_main_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_main_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_main = g_type_register_static(G_TYPE_OBJECT,
					   "AgsMain\0",
					   &ags_main_info,
					   0);

    g_type_add_interface_static(ags_type_main,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_main);
}

void
ags_main_class_init(AgsMainClass *ags_main)
{
  GObjectClass *gobject;

  ags_main_parent_class = g_type_class_peek_parent(ags_main);

  /* GObjectClass */
  gobject = (GObjectClass *) ags_main;

  gobject->finalize = ags_main_finalize;
}

void
ags_main_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_main_connect;
  connectable->disconnect = ags_main_disconnect;
}

void
ags_main_init(AgsMain *ags_main)
{
  GFile *file;
  struct sigaction sa;
  struct passwd *pw;
  uid_t uid;
  gchar *wdir, *filename;
    
  uid = getuid();
  pw = getpwuid(uid);
  
  wdir = g_strdup_printf("%s/%s\0",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  file = g_file_new_for_path(wdir);

  g_file_make_directory(file,
			NULL,
			NULL);

  ags_main->flags = 0;

  ags_main->version = AGS_VERSION;
  ags_main->build_id = AGS_BUILD_ID;

  ags_main->log = (AgsLog *) g_object_new(AGS_TYPE_LOG,
					  "file\0", stdout,
					  NULL);
  ags_colors_alloc();

  ags_main->main_loop = NULL;

  /* AgsAutosaveThread */
  ags_main->autosave_thread = NULL;
  ags_main->autosave_thread = ags_autosave_thread_new(NULL, ags_main);
  g_object_ref(G_OBJECT(ags_main->autosave_thread));
  ags_connectable_connect(AGS_CONNECTABLE(ags_main->autosave_thread));
  
  ags_main->thread_pool = ags_thread_pool_new(NULL);
  ags_main->server = NULL;
  ags_main->devout = NULL;
  ags_main->window = NULL;
  // ags_log_message(ags_default_log, "starting Advanced Gtk+ Sequencer\n\0");

  sigfillset(&(ags_wait_mask));
  sigdelset(&(ags_wait_mask), AGS_THREAD_SUSPEND_SIG);
  sigdelset(&(ags_wait_mask), AGS_THREAD_RESUME_SIG);

  sigfillset(&(sa.sa_mask));
  sa.sa_flags = 0;

  sa.sa_handler = ags_thread_resume_handler;
  sigaction(AGS_THREAD_RESUME_SIG, &sa, NULL);

  sa.sa_handler = ags_thread_suspend_handler;
  sigaction(AGS_THREAD_SUSPEND_SIG, &sa, NULL);
  
  filename = g_strdup_printf("%s/%s\0",
			     wdir,
			     AGS_DEFAULT_CONFIG);

  ags_main->config = config;
  //TODO:JK: ugly
  config->ags_main = ags_main;

  g_free(filename);
  g_free(wdir);
}

void
ags_main_connect(AgsConnectable *connectable)
{
  AgsMain *ags_main;
  GList *list;

  ags_main = AGS_MAIN(connectable);

  if((AGS_MAIN_CONNECTED & (ags_main->flags)) != 0)
    return;

  ags_main->flags |= AGS_MAIN_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(ags_main->main_loop));
  ags_connectable_connect(AGS_CONNECTABLE(ags_main->thread_pool));

  g_message("connected threads\0");

  list = ags_main->devout;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_message("connected audio\0");

  ags_connectable_connect(AGS_CONNECTABLE(ags_main->window));
  g_message("connected gui\0");
}

void
ags_main_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_main_finalize(GObject *gobject)
{
  AgsMain *ags_main;

  ags_main = AGS_MAIN(gobject);

  G_OBJECT_CLASS(ags_main_parent_class)->finalize(gobject);
}

void
ags_colors_alloc()
{
  static gboolean allocated = FALSE;

  if(!allocated){
    allocated = TRUE;

    matrix_style = gtk_style_new();
    matrix_style->fg[0].red = 120 * (65535/255);
    matrix_style->fg[0].green = 220 * (65535/255);
    matrix_style->fg[0].blue = 120 * (65535/255);
    matrix_style->fg[0].pixel = (gulong)(120*65536 + 220*256 + 120);

    matrix_style->light[0].red = 100 * (65535/255);
    matrix_style->light[0].green = 200 * (65535/255);
    matrix_style->light[0].blue = 100 * (65535/255);
    matrix_style->light[0].pixel = (gulong)(100*65536 + 200*256 + 100);

    matrix_style->bg[0].red = 0 * (65535/255);
    matrix_style->bg[0].green = 80 * (65535/255);
    matrix_style->bg[0].blue = 0 * (65535/255);
    matrix_style->bg[0].pixel = (gulong)(0*65536 + 80*256 + 0);

    matrix_style->bg[1].red = 120 * (65535/255);
    matrix_style->bg[1].green = 220 * (65535/255);
    matrix_style->bg[1].blue = 120 * (65535/255);
    matrix_style->bg[1].pixel = (gulong)(120*65536 + 220*256 + 120);

    matrix_style->bg[2].red = 120 * (65535/255);
    matrix_style->bg[2].green = 220 * (65535/255);
    matrix_style->bg[2].blue = 120 * (65535/255);
    matrix_style->bg[2].pixel = (gulong)(120*65536 + 220*256 + 120);

    matrix_style->base[0].red = 120 * (65535/255);
    matrix_style->base[0].green = 220 * (65535/255);
    matrix_style->base[0].blue = 120 * (65535/255);
    matrix_style->base[0].pixel = (gulong)(120*65536 + 220*256 + 120);


    /*  matrix_style->fg[1] = matrix_green;
	matrix_style->fg[2] = matrix_green;
	matrix_style->fg[3] = matrix_green;
	matrix_style->fg[4] = matrix_green;

	matrix_style->bg[0] = matrix_led_bg;
	matrix_style->bg[1] = matrix_led_bg;
	matrix_style->bg[2] = matrix_led_bg;
	matrix_style->bg[2] = matrix_led_bg;
	matrix_style->bg[3] = matrix_led_bg;
	matrix_style->bg[4] = matrix_led_bg;
    */

    /* ffplayer style */
    ffplayer_style = gtk_style_new();
    ffplayer_style->fg[0].red = 180 * (65535/255);
    ffplayer_style->fg[0].green = 180 * (65535/255);
    ffplayer_style->fg[0].blue = 250 * (65535/255);
    ffplayer_style->fg[0].pixel = (gulong)(120*65536 + 120*256 + 220);

    ffplayer_style->bg[0].red = 4 * (65535/255);
    ffplayer_style->bg[0].green = 8 * (65535/255);
    ffplayer_style->bg[0].blue = 56 * (65535/255);
    ffplayer_style->bg[0].pixel = (gulong)(4*65536 + 8*256 + 56);

    ffplayer_style->bg[1].red = 120 * (65535/255);
    ffplayer_style->bg[1].green = 120 *(65535/255);
    ffplayer_style->bg[1].blue = 120 * (65535/255);
    ffplayer_style->bg[1].pixel = (gulong)(120*65536 + 120*256 + 120);

    ffplayer_style->bg[2].red = 120 * (65535/255);
    ffplayer_style->bg[2].green = 120 *(65535/255);
    ffplayer_style->bg[2].blue = 120 * (65535/255);
    ffplayer_style->bg[2].pixel = (gulong)(255*65536 + 255*256 + 255);

    ffplayer_style->mid[0].red = 4 * (65535/255);
    ffplayer_style->mid[0].green = 8 * (65535/255);
    ffplayer_style->mid[0].blue = 56 * (65535/255);
    ffplayer_style->mid[0].pixel = (gulong)(4*65536 + 8*256 + 56);

    ffplayer_style->light[0].red = 120 * (65535/255);
    ffplayer_style->light[0].green = 120 * (65535/255);
    ffplayer_style->light[0].blue = 220 * (65535/255);
    ffplayer_style->light[0].pixel = (gulong)(120*65536 + 120*256 + 220);

    ffplayer_style->base[0].red = 120 * (65535/255);
    ffplayer_style->base[0].green = 120 * (65535/255);
    ffplayer_style->base[0].blue = 220 * (65535/255);
    ffplayer_style->base[0].pixel = (gulong)(120*65536 + 120*256 + 220);


    /* editor style */
    editor_style = gtk_style_new();
    editor_style->fg[0].red = 255 * (65535/255);
    editor_style->fg[0].green = 240 *(65535/255);
    editor_style->fg[0].blue = 200 * (65535/255);
    editor_style->fg[0].pixel = (gulong)(255*65536 + 240*256 + 200);

    editor_style->bg[0].red = 255 * (65535/255);
    editor_style->bg[0].green = 255 *(65535/255);
    editor_style->bg[0].blue = 100 * (65535/255);
    editor_style->bg[0].pixel = (gulong)(255*65536 + 255*256 + 255);

    editor_style->mid[0].red = 250 * (65535/255);
    editor_style->mid[0].green = 0 *(65535/255);
    editor_style->mid[0].blue = 250 * (65535/255);
    editor_style->mid[0].pixel = (gulong)(150*65536 + 150*256 + 250);


    editor_style->base[0].red = 250 * (65535/255);
    editor_style->base[0].green = 250 *(65535/255);
    editor_style->base[0].blue = 250 * (65535/255);
    editor_style->base[0].pixel = (gulong)(250*65536 + 250*256 + 250);


    /* notebook style */
    notebook_style = gtk_style_new();
    notebook_style->fg[0].red = 255 * (65535/255);
    notebook_style->fg[0].green = 255 *(65535/255);
    notebook_style->fg[0].blue = 0 * (65535/255);
    notebook_style->fg[0].pixel = (gulong)(255*65536 + 255*256 + 0);

    notebook_style->bg[0].red = 40 * (65535/255);
    notebook_style->bg[0].green = 40 *(65535/255);
    notebook_style->bg[0].blue = 40 * (65535/255);
    notebook_style->bg[0].pixel = (gulong)(40*65536 + 40*256 + 40);

    notebook_style->mid[0].red = 10 * (65535/255);
    notebook_style->mid[0].green = 20 *(65535/255);
    notebook_style->mid[0].blue = 230 * (65535/255);
    notebook_style->mid[0].pixel = (gulong)(10*65536 + 20*256 + 230);

    notebook_style->dark[0].red = 192 * (65535/255);
    notebook_style->dark[0].green = 192 *(65535/255);
    notebook_style->dark[0].blue = 0 * (65535/255);
    notebook_style->dark[0].pixel = (gulong)(150*65536 + 150*256 + 250);

    notebook_style->light[0].red = 255 * (65535/255);
    notebook_style->light[0].green = 255 *(65535/255);
    notebook_style->light[0].blue = 63 * (65535/255);
    notebook_style->light[0].pixel = (gulong)(255*65536 + 255*256 + 63);

    notebook_style->base[0].red = 0 * (65535/255);
    notebook_style->base[0].green = 0 *(65535/255);
    notebook_style->base[0].blue = 0 * (65535/255);
    notebook_style->base[0].pixel = (gulong)(0*65536 + 0*256 + 0);


    /* ruler style */
    //TODO:JK: deprecated
    //    ruler_style = gtk_style_new();
    //    ruler_style->fg[0].red = 255 * (65535/255);
    //    ruler_style->fg[0].green = 240 *(65535/255);
    //    ruler_style->fg[0].blue = 200 * (65535/255);
    //    ruler_style->fg[0].pixel = (gulong)(255*65536 + 240*256 + 200);

    //    ruler_style->bg[0].red = 40 * (65535/255);
    //    ruler_style->bg[0].green = 40 *(65535/255);
    //    ruler_style->bg[0].blue = 40 * (65535/255);
    //    ruler_style->bg[0].pixel = (gulong)(40*65536 + 40*256 + 40);

    //    ruler_style->mid[0].red = 250 * (65535/255);
    //    ruler_style->mid[0].green = 0 *(65535/255);
    //    ruler_style->mid[0].blue = 250 * (65535/255);
    //    ruler_style->mid[0].pixel = (gulong)(150*65536 + 150*256 + 250);

    //    ruler_style->base[0].red = 250 * (65535/255);
    //    ruler_style->base[0].green = 250 *(65535/255);
    //    ruler_style->base[0].blue = 250 * (65535/255);
    //    ruler_style->base[0].pixel = (gulong)(250*65536 + 250*256 + 250);


    /* meter style */
    meter_style = gtk_style_new();
    meter_style->fg[0].red = 100 * (65535/255);
    meter_style->fg[0].green = 200 *(65535/255);
    meter_style->fg[0].blue = 255 * (65535/255);
    meter_style->fg[0].pixel = (gulong)(100*65536 + 200*256 + 255);

    meter_style->bg[0].red = 40 * (65535/255);
    meter_style->bg[0].green = 40 *(65535/255);
    meter_style->bg[0].blue = 40 * (65535/255);
    meter_style->bg[0].pixel = (gulong)(40*65536 + 40*256 + 40);

    meter_style->mid[0].red = 10 * (65535/255);
    meter_style->mid[0].green = 20 *(65535/255);
    meter_style->mid[0].blue = 230 * (65535/255);
    meter_style->mid[0].pixel = (gulong)(10*65536 + 20*256 + 230);

    meter_style->light[0].red = 0 * (65535/255);
    meter_style->light[0].green = 150 *(65535/255);
    meter_style->light[0].blue = 255 * (65535/255);
    meter_style->light[0].pixel = (gulong)(150*65536 + 150*256 + 250);

    meter_style->base[0].red = 0 * (65535/255);
    meter_style->base[0].green = 0 *(65535/255);
    meter_style->base[0].blue = 0 * (65535/255);
    meter_style->base[0].pixel = (gulong)(0*65536 + 0*256 + 0);


    /* note_edit style */
    note_edit_style = gtk_style_new();
    note_edit_style->fg[0].red = 255 * (65535/255);
    note_edit_style->fg[0].green = 240 *(65535/255);
    note_edit_style->fg[0].blue = 200 * (65535/255);
    note_edit_style->fg[0].pixel = (gulong)(255*65536 + 240*256 + 200);

    note_edit_style->bg[0].red = 255 * (65535/255);
    note_edit_style->bg[0].green = 255 *(65535/255);
    note_edit_style->bg[0].blue = 100 * (65535/255);
    note_edit_style->bg[0].pixel = (gulong)(255*65536 + 255*256 + 255);

    note_edit_style->mid[0].red = 250 * (65535/255);
    note_edit_style->mid[0].green = 0 *(65535/255);
    note_edit_style->mid[0].blue = 250 * (65535/255);
    note_edit_style->mid[0].pixel = (gulong)(150*65536 + 150*256 + 250);


    note_edit_style->base[0].red = 250 * (65535/255);
    note_edit_style->base[0].green = 250 *(65535/255);
    note_edit_style->base[0].blue = 250 * (65535/255);
    note_edit_style->base[0].pixel = (gulong)(250*65536 + 250*256 + 250);
  }
}

void
ags_main_load_config(AgsMain *ags_main)
{
  AgsConfig *config;
  GList *list;

  auto void ags_main_load_config_thread(AgsThread *thread);
  auto void ags_main_load_config_devout(AgsDevout *devout);

  void ags_main_load_config_thread(AgsThread *thread){
    gchar *model;
    
    model = ags_config_get(config,
			   ags_config_devout,
			   "model\0");
    
    if(model != NULL){
      if(!strncmp(model,
		  "single-threaded\0",
		  16)){
	//TODO:JK: implement me
	
      }else if(!strncmp(model,
			"multi-threaded",
			15)){
	//TODO:JK: implement me
      }else if(!strncmp(model,
			"super-threaded",
			15)){
	//TODO:JK: implement me
      }
    }
  }
  void ags_main_load_config_devout(AgsDevout *devout){
    gchar *alsa_handle;
    guint samplerate;
    guint buffer_size;
    guint pcm_channels, dsp_channels;

    alsa_handle = ags_config_get(config,
				 ags_config_devout,
				 "alsa-handle\0");

    dsp_channels = strtoul(ags_config_get(config,
					  ags_config_devout,
					  "dsp-channels\0"),
			   NULL,
			   10);
    
    pcm_channels = strtoul(ags_config_get(config,
					  ags_config_devout,
					  "pcm-channels\0"),
			   NULL,
			   10);

    samplerate = strtoul(ags_config_get(config,
					ags_config_devout,
					"samplerate\0"),
			 NULL,
			 10);

    buffer_size = strtoul(ags_config_get(config,
					 ags_config_devout,
					 "buffer-size\0"),
			  NULL,
			  10);
    
    g_object_set(G_OBJECT(devout),
		 "device\0", alsa_handle,
		 "dsp-channels\0", dsp_channels,
		 "pcm-channels\0", pcm_channels,
		 "frequency\0", samplerate,
		 "buffer-size\0", buffer_size,
		 NULL);
  }
  
  if(ags_main == NULL){
    return;
  }

  config = ags_main->config;

  if(config == NULL){
    return;
  }

  /* thread */
  ags_main_load_config_thread(ags_main->main_loop);

  /* devout */
  list = ags_main->devout;

  while(list != NULL){
    ags_main_load_config_devout(AGS_DEVOUT(list->data));

    list = list->next;
  }
}

void
ags_main_add_devout(AgsMain *ags_main,
		    AgsDevout *devout)
{
  if(ags_main == NULL ||
     devout == NULL){
    return;
  }

  g_object_ref(G_OBJECT(devout));
  ags_main->devout = g_list_prepend(ags_main->devout,
				    devout);
}

void
ags_main_register_recall_type()
{
  ags_play_audio_get_type();
  ags_play_channel_get_type();
  ags_play_channel_run_get_type();
  ags_play_channel_run_master_get_type();

  ags_stream_channel_get_type();
  ags_stream_channel_run_get_type();

  ags_loop_channel_get_type();
  ags_loop_channel_run_get_type();

  ags_copy_channel_get_type();
  ags_copy_channel_run_get_type();

  ags_volume_channel_get_type();
  ags_volume_channel_run_get_type();

  ags_peak_channel_get_type();
  ags_peak_channel_run_get_type();

  ags_recall_ladspa_get_type();
  ags_recall_channel_run_dummy_get_type();
  ags_recall_ladspa_run_get_type();

  ags_delay_audio_get_type();
  ags_delay_audio_run_get_type();

  ags_count_beats_audio_get_type();
  ags_count_beats_audio_run_get_type();

  ags_copy_pattern_audio_get_type();
  ags_copy_pattern_audio_run_get_type();
  ags_copy_pattern_channel_get_type();
  ags_copy_pattern_channel_run_get_type();

  ags_buffer_channel_get_type();
  ags_buffer_channel_run_get_type();

  ags_play_notation_audio_get_type();
  ags_play_notation_audio_run_get_type();
}

void
ags_main_register_task_type()
{
  //TODO:JK: implement me
}

void
ags_main_register_widget_type()
{
  ags_dial_get_type();
}

void
ags_main_register_machine_type()
{
  ags_panel_get_type();
  ags_panel_input_pad_get_type();
  ags_panel_input_line_get_type();

  ags_mixer_get_type();
  ags_mixer_input_pad_get_type();
  ags_mixer_input_line_get_type();

  ags_drum_get_type();
  ags_drum_output_pad_get_type();
  ags_drum_output_line_get_type();
  ags_drum_input_pad_get_type();
  ags_drum_input_line_get_type();

  ags_matrix_get_type();

  ags_synth_get_type();
  ags_synth_input_pad_get_type();
  ags_synth_input_line_get_type();

  ags_ffplayer_get_type();
}

void
ags_main_register_thread_type()
{
  ags_thread_get_type();

  ags_audio_loop_get_type();
  ags_task_thread_get_type();
  ags_devout_thread_get_type();
  ags_iterator_thread_get_type();
  ags_recycling_thread_get_type();
  ags_timestamp_thread_get_type();
  ags_gui_thread_get_type();

  ags_thread_pool_get_type();
  ags_returnable_thread_get_type();
}

void
ags_main_quit(AgsMain *ags_main)
{
  ags_thread_stop(AGS_AUDIO_LOOP(ags_main->main_loop)->gui_thread);
}

AgsMain*
ags_main_new()
{
  AgsMain *ags_main;

  ags_main = (AgsMain *) g_object_new(AGS_TYPE_MAIN,
				      NULL);

  return(ags_main);
}

void
ags_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_sigact.sa_mask));

    if(signr == AGS_ASYNC_QUEUE_SIGNAL_HIGH){
      // pthread_yield();
    }
  }
}

static void
ags_signal_cleanup()
{
  sigemptyset(&(ags_sigact.sa_mask));
}

int
main(int argc, char **argv)
{
  AgsMain *ags_main;
  AgsDevout *devout;
  AgsWindow *window;
  AgsGuiThread *gui_thread;
  GFile *autosave_file;
  struct sched_param param;
  struct rlimit rl;
  gchar *filename, *autosave_filename;

  struct passwd *pw;
  uid_t uid;
  gchar *wdir, *config_file;
  int result;
  gboolean single_thread = FALSE;
  guint i;

  const char *error;
  const rlim_t kStackSize = 128L * 1024L * 1024L;   // min stack size = 128 Mb

  //  mtrace();
  atexit(ags_signal_cleanup);

  result = getrlimit(RLIMIT_STACK, &rl);

  /* set stack size 64M */
  if(result == 0){
    if(rl.rlim_cur < kStackSize){
      rl.rlim_cur = kStackSize;
      result = setrlimit(RLIMIT_STACK, &rl);

      if(result != 0){
	//TODO:JK
      }
    }
  }

  /* Ignore interactive and job-control signals.  */
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  ags_sigact.sa_handler = ags_signal_handler;
  sigemptyset(&ags_sigact.sa_mask);
  ags_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_sigact, (struct sigaction *) NULL);

  /**/
  LIBXML_TEST_VERSION;

  XInitThreads();

  g_thread_init(NULL);
  gdk_threads_init();

  gtk_init(&argc, &argv);
  ipatch_init();

  ao_initialize();

  filename = NULL;

  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--single-thread\0", 16)){
      single_thread = TRUE;
    }else if(!strncmp(argv[i], "--filename\0", 11)){
      filename = argv[i + 1];
      i++;
    }
  }

  config = ags_config_new();
  uid = getuid();
  pw = getpwuid(uid);
  
  wdir = g_strdup_printf("%s/%s\0",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  config_file = g_strdup_printf("%s/%s\0",
				wdir,
				AGS_DEFAULT_CONFIG);

  ags_config_load_from_file(config,
			    config_file);

  g_free(wdir);
  g_free(config_file);

  if(filename != NULL){
    AgsFile *file;

    file = g_object_new(AGS_TYPE_FILE,
			"filename\0", filename,
			NULL);
    ags_file_open(file);
    ags_file_read(file);

    ags_main = AGS_MAIN(file->ags_main);
    ags_file_close(file);

    ags_thread_start(ags_main->main_loop);

    /* complete thread pool */
    ags_main->thread_pool->parent = AGS_THREAD(ags_main->main_loop);
    ags_thread_pool_start(ags_main->thread_pool);

#ifdef _USE_PTH
    pth_join(AGS_AUDIO_LOOP(ags_main->main_loop)->gui_thread->thread,
	     NULL);
#else
    pthread_join(AGS_AUDIO_LOOP(ags_main->main_loop)->gui_thread->thread,
		 NULL);
#endif
  }else{
    ags_main = ags_main_new();

    if(single_thread){
      ags_main->flags = AGS_MAIN_SINGLE_THREAD;
    }

    /* Declare ourself as a real time task */
    param.sched_priority = AGS_PRIORITY;

    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed\0");
    }

    mlockall(MCL_CURRENT | MCL_FUTURE);

    if((AGS_MAIN_SINGLE_THREAD & (ags_main->flags)) == 0){
      //      GdkFrameClock *frame_clock;

#ifdef AGS_WITH_XMLRPC_C
      AbyssInit(&error);

      xmlrpc_env_init(&(ags_main->env));
#endif /* AGS_WITH_XMLRPC_C */

      /* AgsDevout */
      devout = ags_devout_new((GObject *) ags_main);
      ags_main_add_devout(ags_main,
			  devout);

      /*  */
      g_object_set(G_OBJECT(ags_main->autosave_thread),
		   "devout\0", devout,
		   NULL);

      /* AgsWindow */
      ags_main->window =
	window = ags_window_new((GObject *) ags_main);
      g_object_set(G_OBJECT(window),
		   "devout\0", devout,
		   NULL);
      g_object_ref(G_OBJECT(window));

      gtk_window_set_default_size((GtkWindow *) window, 500, 500);
      gtk_paned_set_position((GtkPaned *) window->paned, 300);

      ags_connectable_connect(window);
      gtk_widget_show_all((GtkWidget *) window);

      /* AgsServer */
      ags_main->server = ags_server_new((GObject *) ags_main);

      /* AgsMainLoop */
      ags_main->main_loop = AGS_MAIN_LOOP(ags_audio_loop_new((GObject *) devout, (GObject *) ags_main));
      g_object_ref(G_OBJECT(ags_main->main_loop));
      ags_connectable_connect(AGS_CONNECTABLE(ags_main->main_loop));

      /* start thread tree */
      ags_thread_start(ags_main->main_loop);

      /* complete thread pool */
      ags_main->thread_pool->parent = AGS_THREAD(ags_main->main_loop);
      ags_thread_pool_start(ags_main->thread_pool);
    }else{
      AgsSingleThread *single_thread;

      devout = ags_devout_new((GObject *) ags_main);
      ags_main_add_devout(ags_main,
			  devout);

      g_object_set(G_OBJECT(ags_main->autosave_thread),
		   "devout\0", devout,
		   NULL);

      /* threads */
      single_thread = ags_single_thread_new((GObject *) devout);

      /* AgsWindow */
      ags_main->window = 
	window = ags_window_new((GObject *) ags_main);
      g_object_set(G_OBJECT(window),
		   "devout\0", devout,
		   NULL);
      gtk_window_set_default_size((GtkWindow *) window, 500, 500);
      gtk_paned_set_position((GtkPaned *) window->paned, 300);

      ags_connectable_connect(window);
      gtk_widget_show_all((GtkWidget *) window);

      /* AgsMainLoop */
      ags_main->main_loop = AGS_MAIN_LOOP(ags_audio_loop_new((GObject *) devout, (GObject *) ags_main));
      g_object_ref(G_OBJECT(ags_main->main_loop));

      /* complete thread pool */
      ags_main->thread_pool->parent = AGS_THREAD(ags_main->main_loop);
      ags_thread_pool_start(ags_main->thread_pool);

      /* start thread tree */
      ags_thread_start((AgsThread *) single_thread);
    }

    if(!single_thread){
      /* join gui thread */
#ifdef _USE_PTH
      pth_join(AGS_AUDIO_LOOP(ags_main->main_loop)->gui_thread->thread,
	       NULL);
#else
      pthread_join(AGS_AUDIO_LOOP(ags_main->main_loop)->gui_thread->thread,
		   NULL);
#endif
    }
  }

  /* free managers */
  if(ags_ladspa_manager != NULL){
    g_object_unref(ags_ladspa_manager_get_instance());
  }

  uid = getuid();
  pw = getpwuid(uid);
  
  autosave_filename = g_strdup_printf("%s/%s/%d-%s\0",
				      pw->pw_dir,
				      AGS_DEFAULT_DIRECTORY,
				      getpid(),
				      AGS_AUTOSAVE_THREAD_DEFAULT_FILENAME);
  
  autosave_file = g_file_new_for_path(autosave_filename);

  if(g_file_query_exists(autosave_file,
			 NULL)){
    g_file_delete(autosave_file,
		  NULL,
		  NULL);
  }

  g_free(autosave_filename);
  g_object_unref(autosave_file);

  //  muntrace();

  return(0);
}

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
#define __USE_UNIX98
#include <sys/mman.h>

#include <gtk/gtk.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_main_loop.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_gui_thread.h>
#include <ags/thread/ags_single_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recycling.h>

#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>

#include <ags/audio/ags_synths.h>

#include <libintl.h>
#include <stdio.h>

#include "config.h"

void ags_main_class_init(AgsMainClass *main);
void ags_main_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_main_init(AgsMain *main);
void ags_main_connect(AgsConnectable *connectable);
void ags_main_disconnect(AgsConnectable *connectable);
void ags_main_finalize(GObject *gobject);

void ags_colors_alloc();

static gpointer ags_main_parent_class = NULL;

extern GtkStyle *matrix_style;
extern GtkStyle *ffplayer_style;
extern GtkStyle *editor_style;
extern GtkStyle *notebook_style;
extern GtkStyle *ruler_style;
extern GtkStyle *meter_style;

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
ags_main_class_init(AgsMainClass *main)
{
  GObjectClass *gobject;

  ags_main_parent_class = g_type_class_peek_parent(main);

  /* GObjectClass */
  gobject = (GObjectClass *) main;

  gobject->finalize = ags_main_finalize;
}

void
ags_main_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_main_connect;
  connectable->disconnect = ags_main_disconnect;
}

void
ags_main_init(AgsMain *main)
{
  main->log = (AgsLog *) g_object_new(AGS_TYPE_LOG,
				      "file\0", stdout,
				      NULL);
  ags_colors_alloc();

  // ags_log_message(ags_default_log, "starting Advanced Gtk+ Sequencer\n\0");
}

void
ags_main_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_main_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_main_finalize(GObject *gobject)
{
  AgsMain *main;

  main = AGS_MAIN(gobject);

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
    ruler_style = gtk_style_new();
    ruler_style->fg[0].red = 255 * (65535/255);
    ruler_style->fg[0].green = 240 *(65535/255);
    ruler_style->fg[0].blue = 200 * (65535/255);
    ruler_style->fg[0].pixel = (gulong)(255*65536 + 240*256 + 200);

    ruler_style->bg[0].red = 40 * (65535/255);
    ruler_style->bg[0].green = 40 *(65535/255);
    ruler_style->bg[0].blue = 40 * (65535/255);
    ruler_style->bg[0].pixel = (gulong)(40*65536 + 40*256 + 40);

    ruler_style->mid[0].red = 250 * (65535/255);
    ruler_style->mid[0].green = 0 *(65535/255);
    ruler_style->mid[0].blue = 250 * (65535/255);
    ruler_style->mid[0].pixel = (gulong)(150*65536 + 150*256 + 250);

    ruler_style->base[0].red = 250 * (65535/255);
    ruler_style->base[0].green = 250 *(65535/255);
    ruler_style->base[0].blue = 250 * (65535/255);
    ruler_style->base[0].pixel = (gulong)(250*65536 + 250*256 + 250);


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
  }
}

void
ags_main_quit(AgsMain *main)
{
  ags_thread_stop(main->gui_loop);
}

AgsMain*
ags_main_new()
{
  AgsMain *main;

  main = (AgsMain *) g_object_new(AGS_TYPE_MAIN,
				  NULL);

  return(main);
}

int
main(int argc, char **argv)
{
  AgsMain *main;
  AgsDevout *devout;
  AgsWindow *window;
  AgsGuiThread *gui_thread;
  struct sched_param param;
  const char *error;
  gboolean single_thread = FALSE;
  guint i;

  for(i = 0; i < argc; i++){
    if(!strncmp(argv[i], "--single-thread\0", 16)){
      single_thread = TRUE;
    }
  }

  /* Declare ourself as a real time task */
  param.sched_priority = AGS_PRIORITY;

  if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed\0");
  }

  mlockall(MCL_CURRENT | MCL_FUTURE);

  LIBXML_TEST_VERSION;

  gtk_init(&argc, &argv);
  ipatch_init();

  if(!single_thread){
    AbyssInit(&error);

    main = ags_main_new();
    xmlrpc_env_init(&(main->env));

    devout = ags_devout_new(main);
    main->devout = devout; //TODO:JK: really ugly
    main->devout->main = main; //TODO:JK: really ugly

    /* threads */
    devout->audio_loop = ags_audio_loop_new(G_OBJECT(devout), G_OBJECT(main));
    devout->task_thread = AGS_TASK_THREAD(devout->audio_loop->task_thread);
    devout->devout_thread = AGS_DEVOUT_THREAD(devout->audio_loop->devout_thread);

    /* AgsWindow */
    window = ags_window_new(main);
    window->devout = devout; //TODO:JK: really ugly

    gtk_window_set_default_size((GtkWindow *) window, 500, 500);
    gtk_paned_set_position((GtkPaned *) window->paned, 300);

    ags_connectable_connect(window);
    gtk_widget_show_all((GtkWidget *) window);

    /* AgsServer */
    main->server = ags_server_new(main);

    /* AgsMainLoop */
    main->main_loop = window->devout->audio_loop;
    g_object_set(G_OBJECT(main->main_loop),
		 "devout\0", main->devout,
		 NULL);

    ags_thread_start(main->main_loop);

    //TODO:JK: really ugly
    AGS_AUDIO_LOOP(main->main_loop)->main = main;

    /*  */
    main->gui_loop = AGS_AUDIO_LOOP(main->main_loop)->gui_thread;

    ags_thread_start(main->gui_loop);

#ifdef _USE_PTH
    pth_join(main->gui_loop->thread,
	     NULL);
#else
    pthread_join(main->gui_loop->thread,
		 NULL);
#endif
  }else{
    AgsSingleThread *single_thread;

    main = ags_main_new();

    devout = ags_devout_new(main);
    main->devout = devout; //TODO:JK: really ugly
    main->devout->main = main; //TODO:JK: really ugly

    /* threads */
    single_thread = ags_single_thread_new(devout);
    devout->audio_loop = single_thread->audio_loop;
    devout->audio_loop->main = main; //TODO:JK: really ugly
    devout->task_thread = single_thread->task_thread;
    devout->devout_thread = single_thread->devout_thread;

    /* AgsWindow */
    window = ags_window_new(main);
    window->devout = devout; //TODO:JK: really ugly

    gtk_window_set_default_size((GtkWindow *) window, 500, 500);
    gtk_paned_set_position((GtkPaned *) window->paned, 300);

    ags_connectable_connect(window);
    gtk_widget_show_all((GtkWidget *) window);

    /* AgsMainLoop */
    main->main_loop = window->devout->audio_loop;
    g_object_set(G_OBJECT(main->main_loop),
		 "devout\0", main->devout,
		 NULL);

    /*  */
    main->gui_loop = AGS_AUDIO_LOOP(main->main_loop)->gui_thread;

    ags_thread_start(single_thread);
  }

  return(0);
}

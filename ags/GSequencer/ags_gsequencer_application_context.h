/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_GSEQUENCER_APPLICATION_CONTEXT_H__
#define __AGS_GSEQUENCER_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSEQUENCER_APPLICATION_CONTEXT                (ags_gsequencer_application_context_get_type())
#define AGS_GSEQUENCER_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSEQUENCER_APPLICATION_CONTEXT, AgsGSequencerApplicationContext))
#define AGS_GSEQUENCER_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GSEQUENCER_APPLICATION_CONTEXT, AgsGSequencerApplicationContextClass))
#define AGS_IS_GSEQUENCER_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GSEQUENCER_APPLICATION_CONTEXT))
#define AGS_IS_GSEQUENCER_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GSEQUENCER_APPLICATION_CONTEXT))
#define AGS_GSEQUENCER_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GSEQUENCER_APPLICATION_CONTEXT, AgsGSequencerApplicationContextClass))

#define AGS_GSEQUENCER_VERSION "3.0.0"
#define AGS_GSEQUENCER_BUILD_ID "Thu Nov  7 01:44:21 UTC 2019"

#define AGS_GSEQUENCER_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL (1000 / 25)

typedef struct _AgsGSequencerApplicationContext AgsGSequencerApplicationContext;
typedef struct _AgsGSequencerApplicationContextClass AgsGSequencerApplicationContextClass;

struct _AgsGSequencerApplicationContext
{
  AgsApplicationContext application_context;

  guint flags;
  
  AgsThreadPool *thread_pool;

  GList *worker;

  GMainContext *server_main_context;

  gboolean is_operating;

  AgsServerStatus *server_status;
  
  AgsRegistry *registry;
  
  GList *server;

  GMainContext *audio_main_context;
  GMainContext *osc_server_main_context;
  
  GObject *default_soundcard;

  AgsThread *default_soundcard_thread;
  AgsThread *default_export_thread;
  
  GList *soundcard;
  GList *sequencer;

  GList *audio;

  GList *sound_server;

  GList *osc_server;

  gboolean gui_ready;
  gboolean show_animation;
  gboolean file_ready;

  gdouble gui_scale_factor;
  
  GList *task;

  GtkWidget *animation_window;
  GtkWidget *window;
  GtkWidget *automation_window;
  GtkWidget *wave_window;
  GtkWidget *sheet_window;

  GtkWidget *export_window;

  GtkWidget *online_help_window;

  GtkWidget *preferences;
  
  GtkWidget *history_browser;
  GtkWidget *midi_browser;
  GtkWidget *sample_browser;

  GtkWidget *midi_import_wizard;
  GtkWidget *midi_export_wizard;

  GList *machine;

  gboolean use_composite_editor;  

  GtkWidget *composite_editor;

  GtkWidget *navigation;

  gboolean start_loader;
  
  gboolean setup_ready;
  gboolean loader_ready;
  gboolean loader_completed;
  
  gboolean ladspa_loading;
  gboolean dssi_loading;
  gboolean lv2_loading;
  gboolean vst3_loading;
  
  GList *ladspa_loader;
  GList *dssi_loader;
  GList *lv2_loader;
  GList *vst3_loader;
  
  AgsLv2TurtleScanner *lv2_turtle_scanner;
};

struct _AgsGSequencerApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_gsequencer_application_context_get_type();

gboolean ags_gsequencer_application_context_message_monitor_timeout(AgsGSequencerApplicationContext *gsequencer_application_context);
gboolean ags_gsequencer_application_context_task_timeout(AgsGSequencerApplicationContext *gsequencer_application_context);

gboolean ags_gsequencer_application_context_loader_timeout(AgsGSequencerApplicationContext *gsequencer_application_context);

AgsGSequencerApplicationContext* ags_gsequencer_application_context_new();

G_END_DECLS

#endif /*__AGS_GSEQUENCER_APPLICATION_CONTEXT_H__*/

/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_XORG_APPLICATION_CONTEXT_H__
#define __AGS_XORG_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#define AGS_TYPE_XORG_APPLICATION_CONTEXT                (ags_xorg_application_context_get_type())
#define AGS_XORG_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_XORG_APPLICATION_CONTEXT, AgsXorgApplicationContext))
#define AGS_XORG_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_XORG_APPLICATION_CONTEXT, AgsXorgApplicationContextClass))
#define AGS_IS_XORG_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_XORG_APPLICATION_CONTEXT))
#define AGS_IS_XORG_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_XORG_APPLICATION_CONTEXT))
#define AGS_XORG_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_XORG_APPLICATION_CONTEXT, AgsXorgApplicationContextClass))

#define AGS_XORG_VERSION "3.0.0"
#define AGS_XORG_BUILD_ID "Thu Nov  7 01:44:21 UTC 2019"

typedef struct _AgsXorgApplicationContext AgsXorgApplicationContext;
typedef struct _AgsXorgApplicationContextClass AgsXorgApplicationContextClass;

struct _AgsXorgApplicationContext
{
  AgsApplicationContext application_context;

  guint flags;
  
  AgsThreadPool *thread_pool;

  GList *worker;

  gboolean is_operating;

  AgsServerStatus *server_status;
  
  AgsRegistry *registry;
  
  GList *server;

  GObject *default_soundcard;

  AgsThread *default_soundcard_thread;
  AgsThread *default_export_thread;

  AgsThread *gui_thread;
  
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

  GtkWidget *preferences;
  
  GtkWidget *history_browser;
  GtkWidget *midi_browser;
  GtkWidget *sample_browser;

  GtkWidget *midi_import_wizard;
  GtkWidget *midi_export_wizard;

  GList *machine;

  GtkWidget *composite_editor;

  GtkWidget *navigation;
};

struct _AgsXorgApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_xorg_application_context_get_type();

void ags_xorg_application_context_load_gui_scale(AgsXorgApplicationContext *xorg_application_context);

void ags_xorg_application_schedule_task(AgsXorgApplicationContext *xorg_application_context,
					GObject *task);
void ags_xorg_application_schedule_task_list(AgsXorgApplicationContext *xorg_application_context,
					     GList *task);

gboolean ags_xorg_application_context_message_monitor_timeout(AgsXorgApplicationContext *xorg_application_context);
gboolean ags_xorg_application_context_task_timeout(AgsXorgApplicationContext *xorg_application_context);

AgsXorgApplicationContext* ags_xorg_application_context_new();

#endif /*__AGS_XORG_APPLICATION_CONTEXT_H__*/

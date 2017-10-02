/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/task/ags_add_soundcard_editor_sink.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/X/thread/ags_gui_thread.h>

void ags_add_soundcard_editor_sink_class_init(AgsAddSoundcardEditorSinkClass *add_soundcard_editor_sink);
void ags_add_soundcard_editor_sink_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_soundcard_editor_sink_init(AgsAddSoundcardEditorSink *add_soundcard_editor_sink);
void ags_add_soundcard_editor_sink_connect(AgsConnectable *connectable);
void ags_add_soundcard_editor_sink_disconnect(AgsConnectable *connectable);
void ags_add_soundcard_editor_sink_finalize(GObject *gobject);

void ags_add_soundcard_editor_sink_launch(AgsTask *task);

/**
 * SECTION:ags_add_soundcard_editor_sink
 * @short_description: add sink to distributed manager
 * @title: AgsAddSoundcardEditorSink
 * @section_id:
 * @include: ags/X/task/ags_add_soundcard_editor_sink.h
 *
 * The #AgsAddSoundcardEditorSink task adds #AgsSoundcard to #AgsDistributedManager.
 */

static gpointer ags_add_soundcard_editor_sink_parent_class = NULL;
static AgsConnectableInterface *ags_add_soundcard_editor_sink_parent_connectable_interface;

GType
ags_add_soundcard_editor_sink_get_type()
{
  static GType ags_type_add_soundcard_editor_sink = 0;

  if(!ags_type_add_soundcard_editor_sink){
    static const GTypeInfo ags_add_soundcard_editor_sink_info = {
      sizeof (AgsAddSoundcardEditorSinkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_soundcard_editor_sink_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddSoundcardEditorSink),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_soundcard_editor_sink_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_soundcard_editor_sink_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_soundcard_editor_sink = g_type_register_static(AGS_TYPE_TASK,
								"AgsAddSoundcardEditorSink",
								&ags_add_soundcard_editor_sink_info,
								0);

    g_type_add_interface_static(ags_type_add_soundcard_editor_sink,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_soundcard_editor_sink);
}

void
ags_add_soundcard_editor_sink_class_init(AgsAddSoundcardEditorSinkClass *add_soundcard_editor_sink)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_add_soundcard_editor_sink_parent_class = g_type_class_peek_parent(add_soundcard_editor_sink);

  /* gobject */
  gobject = (GObjectClass *) add_soundcard_editor_sink;

  gobject->finalize = ags_add_soundcard_editor_sink_finalize;

  /* task */
  task = (AgsTaskClass *) add_soundcard_editor_sink;

  task->launch = ags_add_soundcard_editor_sink_launch;
}

void
ags_add_soundcard_editor_sink_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_soundcard_editor_sink_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_soundcard_editor_sink_connect;
  connectable->disconnect = ags_add_soundcard_editor_sink_disconnect;
}

void
ags_add_soundcard_editor_sink_init(AgsAddSoundcardEditorSink *add_soundcard_editor_sink)
{
  add_soundcard_editor_sink->soundcard_editor = NULL;
}

void
ags_add_soundcard_editor_sink_connect(AgsConnectable *connectable)
{
  ags_add_soundcard_editor_sink_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_soundcard_editor_sink_disconnect(AgsConnectable *connectable)
{
  ags_add_soundcard_editor_sink_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_soundcard_editor_sink_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_soundcard_editor_sink_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_soundcard_editor_sink_launch(AgsTask *task)
{
  AgsPreferences *preferences;
  AgsWindow *window;

  AgsGuiThread *gui_thread;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *application_mutex;
  
  preferences = (AgsPreferences *) gtk_widget_get_toplevel((GtkWidget *) AGS_ADD_SOUNDCARD_EDITOR_SINK(task)->soundcard_editor);
  window = (AgsWindow *) preferences->window;
  
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type((AgsThread *) main_loop,
						      AGS_TYPE_GUI_THREAD);

  ags_soundcard_editor_add_sink(AGS_ADD_SOUNDCARD_EDITOR_SINK(task)->soundcard_editor,
				NULL);
}

/**
 * ags_add_soundcard_editor_sink_new:
 * @soundcard_editor: the #AgsAddSoundcardEditor
 *
 * Creates an #AgsAddSoundcardEditorSink.
 *
 * Returns: an new #AgsAddSoundcardEditorSink.
 *
 * Since: 1.0.0
 */
AgsAddSoundcardEditorSink*
ags_add_soundcard_editor_sink_new(AgsSoundcardEditor *soundcard_editor)
{
  AgsAddSoundcardEditorSink *add_soundcard_editor_sink;

  add_soundcard_editor_sink = (AgsAddSoundcardEditorSink *) g_object_new(AGS_TYPE_ADD_SOUNDCARD_EDITOR_SINK,
									 NULL);

  add_soundcard_editor_sink->soundcard_editor = soundcard_editor;

  return(add_soundcard_editor_sink);
}

/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/thread/ags_autosave_thread.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <ags/i18n.h>

void ags_autosave_thread_class_init(AgsAutosaveThreadClass *autosave_thread);
void ags_autosave_thread_init(AgsAutosaveThread *autosave_thread);
void ags_autosave_thread_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_autosave_thread_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_autosave_thread_finalize(GObject *gobject);

void ags_autosave_thread_start(AgsThread *thread);
void ags_autosave_thread_run(AgsThread *thread);

/**
 * SECTION:ags_autosave_thread
 * @short_description: auto safe
 * @title: AgsAutosaveThread
 * @section_id:
 * @include: ags/thread/ags_autosave_thread.h
 *
 * The #AgsAutosaveThread performs auto-safe.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
};

static gpointer ags_autosave_thread_parent_class = NULL;

GType
ags_autosave_thread_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_autosave_thread = 0;

    static const GTypeInfo ags_autosave_thread_info = {
      sizeof (AgsAutosaveThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_autosave_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutosaveThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_autosave_thread_init,
    };

    ags_type_autosave_thread = g_type_register_static(AGS_TYPE_THREAD,
						      "AgsAutosaveThread",
						      &ags_autosave_thread_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_autosave_thread);
  }

  return g_define_type_id__volatile;
}

void
ags_autosave_thread_class_init(AgsAutosaveThreadClass *autosave_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_autosave_thread_parent_class = g_type_class_peek_parent(autosave_thread);

  /* GObject */
  gobject = (GObjectClass *) autosave_thread;

  gobject->get_property = ags_autosave_thread_get_property;
  gobject->set_property = ags_autosave_thread_set_property;

  gobject->finalize = ags_autosave_thread_finalize;

  /* properties */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("application context to check against"),
				   i18n_pspec("The application context to check against serialization"),
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) autosave_thread;

  thread->start = ags_autosave_thread_start;
  thread->run = ags_autosave_thread_run;
}

void
ags_autosave_thread_init(AgsAutosaveThread *autosave_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(autosave_thread);

  thread->freq = AGS_AUTOSAVE_THREAD_DEFAULT_JIFFIE;

  g_atomic_int_set(&(autosave_thread->tic), 0);
  g_atomic_int_set(&(autosave_thread->last_sync), 0);

  autosave_thread->application_context = NULL;
  autosave_thread->counter = 0;
}

void
ags_autosave_thread_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = AGS_AUTOSAVE_THREAD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(autosave_thread->application_context == (GObject *) application_context){
	return;
      }

      if(autosave_thread->application_context != NULL){
	g_object_unref(autosave_thread->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      autosave_thread->application_context = (GObject *) application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_autosave_thread_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = AGS_AUTOSAVE_THREAD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, autosave_thread->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_autosave_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_autosave_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_autosave_thread_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_autosave_thread_parent_class)->start(thread);
}

void
ags_autosave_thread_run(AgsThread *thread)
{
  AgsAutosaveThread *autosave_thread;

  static const struct timespec delay = {
    1,
    0,
  };

  autosave_thread = AGS_AUTOSAVE_THREAD(thread);

  if(autosave_thread->counter != autosave_thread->delay){
    autosave_thread->counter += 1;
  }else{
    AgsFile *file;
    struct passwd *pw;
    uid_t uid;
    gchar *filename;

    autosave_thread->counter = 0;

    uid = getuid();
    pw = getpwuid(uid);

    filename = g_strdup_printf("%s/%s/%d-%s",
			       pw->pw_dir,
			       AGS_DEFAULT_DIRECTORY,
			       getpid(),
			       AGS_AUTOSAVE_THREAD_DEFAULT_FILENAME);
    
    file = (AgsFile *) g_object_new(AGS_TYPE_FILE,
				    "application-context", autosave_thread->application_context,
				    "filename", filename,
				    NULL);
    ags_file_write_concurrent(file);
    g_object_unref(file);
  }

  nanosleep(&delay, NULL);
}

/**
 * ags_autosave_thread_new:
 * @application_context: the #AgsApplicationContext
 *
 * Create a new instance of #AgsAutosaveThread.
 *
 * Returns: the new #AgsAutosaveThread
 *
 * Since: 2.0.0
 */
AgsAutosaveThread*
ags_autosave_thread_new(GObject *application_context)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = (AgsAutosaveThread *) g_object_new(AGS_TYPE_AUTOSAVE_THREAD,
						       "application-context", application_context,
						       NULL);

  return(autosave_thread);
}

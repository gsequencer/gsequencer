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

#include <ags/X/thread/ags_simple_autosave_thread.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

#include <ags/thread/ags_task_thread.h>

#include <ags/X/file/ags_simple_file.h>
#include <ags/X/task/ags_simple_file_write.h>

#include <sys/types.h>
#include <pwd.h>

void ags_simple_autosave_thread_class_init(AgsSimpleAutosaveThreadClass *simple_autosave_thread);
void ags_simple_autosave_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_simple_autosave_thread_init(AgsSimpleAutosaveThread *autosave_thread);
void ags_simple_autosave_thread_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_simple_autosave_thread_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_simple_autosave_thread_connect(AgsConnectable *connectable);
void ags_simple_autosave_thread_disconnect(AgsConnectable *connectable);
void ags_simple_autosave_thread_finalize(GObject *gobject);

void ags_simple_autosave_thread_start(AgsThread *thread);
void ags_simple_autosave_thread_run(AgsThread *thread);

/**
 * SECTION:ags_simple_autosave_thread
 * @short_description: auto safe
 * @title: AgsSimpleAutosaveThread
 * @section_id:
 * @include: ags/thread/ags_simple_autosave_thread.h
 *
 * The #AgsSimpleAutosaveThread performs auto-safe.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
};

static gpointer ags_simple_autosave_thread_parent_class = NULL;
static AgsConnectableInterface *ags_simple_autosave_thread_parent_connectable_interface;

GType
ags_simple_autosave_thread_get_type()
{
  static GType ags_type_simple_autosave_thread = 0;

  if(!ags_type_simple_autosave_thread){
    static const GTypeInfo ags_simple_autosave_thread_info = {
      sizeof (AgsSimpleAutosaveThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_simple_autosave_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSimpleAutosaveThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_simple_autosave_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_simple_autosave_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_simple_autosave_thread = g_type_register_static(AGS_TYPE_THREAD,
							     "AgsSimpleAutosaveThread\0",
							     &ags_simple_autosave_thread_info,
							     0);
    
    g_type_add_interface_static(ags_type_simple_autosave_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_simple_autosave_thread);
}

void
ags_simple_autosave_thread_class_init(AgsSimpleAutosaveThreadClass *simple_autosave_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_simple_autosave_thread_parent_class = g_type_class_peek_parent(simple_autosave_thread);

  /* GObject */
  gobject = (GObjectClass *) simple_autosave_thread;

  gobject->get_property = ags_simple_autosave_thread_get_property;
  gobject->set_property = ags_simple_autosave_thread_set_property;

  gobject->finalize = ags_simple_autosave_thread_finalize;

  /* properties */
  param_spec = g_param_spec_object("application-context\0",
				   "application context to check against\0",
				   "The application context to check against serialization.\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) simple_autosave_thread;

  thread->start = ags_simple_autosave_thread_start;
  thread->run = ags_simple_autosave_thread_run;
}

void
ags_simple_autosave_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_simple_autosave_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_simple_autosave_thread_connect;
  connectable->disconnect = ags_simple_autosave_thread_disconnect;
}

void
ags_simple_autosave_thread_init(AgsSimpleAutosaveThread *simple_autosave_thread)
{
  AgsThread *thread;

  gchar *filename, *offset;

  struct passwd *pw;
  uid_t uid;
  
  thread = AGS_THREAD(simple_autosave_thread);

  thread->freq = AGS_SIMPLE_AUTOSAVE_THREAD_DEFAULT_JIFFIE;

  simple_autosave_thread->application_context = NULL;
  
  simple_autosave_thread->delay = AGS_SIMPLE_AUTOSAVE_THREAD_DEFAULT_DELAY;
  simple_autosave_thread->counter = 0;

  uid = getuid();
  pw = getpwuid(uid);

  filename = g_strdup_printf("%s/%s/%s\0",
			     pw->pw_dir,
			     AGS_DEFAULT_DIRECTORY,
			     AGS_SIMPLE_AUTOSAVE_THREAD_DEFAULT_FILENAME);

  if((offset = g_strstr(filename,
			"{PID}")) != NULL){
    gchar *tmp0, *tmp1;

    tmp0 = filename;
    tmp1 = g_strndup(filename,
		     offset - filename);
    filename = g_strdup_printf("%s%d%s",
			       tmp1,
			       getpid(),
			       &(offset[5]));

    g_free(tmp0);
    g_free(tmp1);
  }

  simple_autosave_thread->filename = filename;
}

void
ags_simple_autosave_thread_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsSimpleAutosaveThread *simple_autosave_thread;

  simple_autosave_thread = AGS_SIMPLE_AUTOSAVE_THREAD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(simple_autosave_thread->application_context == application_context){
	return;
      }

      if(simple_autosave_thread->application_context != NULL){
	g_object_unref(simple_autosave_thread->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      simple_autosave_thread->application_context = application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_simple_autosave_thread_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsSimpleAutosaveThread *simple_autosave_thread;

  simple_autosave_thread = AGS_SIMPLE_AUTOSAVE_THREAD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, simple_autosave_thread->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_simple_autosave_thread_connect(AgsConnectable *connectable)
{
  ags_simple_autosave_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_simple_autosave_thread_disconnect(AgsConnectable *connectable)
{
  ags_simple_autosave_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_simple_autosave_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_simple_autosave_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_simple_autosave_thread_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_simple_autosave_thread_parent_class)->start(thread);

  AGS_SIMPLE_AUTOSAVE_THREAD(thread)->counter = 0;
}

void
ags_simple_autosave_thread_run(AgsThread *thread)
{
  AgsSimpleAutosaveThread *simple_autosave_thread;

  AgsTaskThread *task_thread;
  
  simple_autosave_thread = AGS_SIMPLE_AUTOSAVE_THREAD(thread);
  task_thread = AGS_APPLICATION_CONTEXT(simple_autosave_thread->application_context)->task_thread;

  if(simple_autosave_thread->counter != simple_autosave_thread->delay){
    simple_autosave_thread->counter += 1;
  }else{
    AgsSimpleFile *simple_file;
    AgsSimpleFileWrite *simple_file_write;
    
    simple_autosave_thread->counter = 0;
    
    simple_file = (AgsFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
					   "application-context\0", simple_autosave_thread->application_context,
					   "filename\0", simple_file->filename,
					   NULL);

    simple_file_write = ags_simple_file_write_new(simple_file_write);
    ags_task_thread_append_task(task_thread,
				simple_file_write);

    g_object_unref(simple_file);
  }
}

/**
 * ags_simple_autosave_thread_new:
 * @application_context: the #AgsApplicationContext
 *
 * Create a new #AgsSimpleAutosaveThread.
 *
 * Returns: the new #AgsSimpleAutosaveThread
 *
 * Since: 0.7.25
 */
AgsSimpleAutosaveThread*
ags_simple_autosave_thread_new(GObject *application_context)
{
  AgsSimpleAutosaveThread *simple_autosave_thread;

  simple_autosave_thread = (AgsSimpleAutosaveThread *) g_object_new(AGS_TYPE_SIMPLE_AUTOSAVE_THREAD,
								    "application-context\0", application_context,
								    NULL);

  return(simple_autosave_thread);
}
